#pragma once
#include "common/types.h"
#include "common/Colors.h"
#include "inventory/Jersey.h"
#include <vector>
#include <map>
#include <algorithm>
#include <numeric>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <iomanip>

// ─────────────────────────────────────────────
//  SalesRecord — one completed sale
// ─────────────────────────────────────────────
struct SalesRecord {
    int         orderId;
    int         fanId;
    std::string fanName;
    int         jerseyId;
    std::string jerseyName;
    std::string team;
    std::string player;
    Sport       sport;
    int         quantity;
    double      revenue;
    std::string date;
};

// ─────────────────────────────────────────────
//  Analytics — sales dashboard + recommender
// ─────────────────────────────────────────────
class Analytics {
    std::vector<SalesRecord> records_;

public:
    void recordSale(const SalesRecord& r) { records_.push_back(r); }

    // ── Sales Dashboard ───────────────────────
    void showDashboard() const {
        printHeader("SALES ANALYTICS DASHBOARD");

        if (records_.empty()) {
            printWarning("No sales data yet.");
            return;
        }

        double totalRevenue = 0;
        int    totalOrders  = 0;
        std::map<std::string, int>    jerseyCount;
        std::map<std::string, double> jerseyRevenue;
        std::map<std::string, int>    teamCount;
        int footballSales = 0, cricketSales = 0;

        for (auto& r : records_) {
            totalRevenue        += r.revenue;
            totalOrders         += r.quantity;
            jerseyCount[r.jerseyName]  += r.quantity;
            jerseyRevenue[r.jerseyName]+= r.revenue;
            teamCount[r.team]          += r.quantity;
            if (r.sport == Sport::FOOTBALL) footballSales += r.quantity;
            else                            cricketSales  += r.quantity;
        }

        // Summary
        std::cout << GREEN << BOLD;
        printf("  Total Revenue : Rs.%.2f\n", totalRevenue);
        printf("  Total Orders  : %d\n",      totalOrders);
        printf("  Football Sales: %d  Cricket Sales: %d\n",
               footballSales, cricketSales);
        std::cout << RESET;
        printDivider();

        // Top selling jerseys
        std::cout << YELLOW << BOLD << "\n  TOP SELLING JERSEYS\n" << RESET;
        std::vector<std::pair<std::string,int>> sorted(jerseyCount.begin(), jerseyCount.end());
        std::sort(sorted.begin(), sorted.end(),
            [](auto& a, auto& b){ return a.second > b.second; });

        int rank = 1;
        for (auto& [name, count] : sorted) {
            if (rank > 5) break;
            printf("  %d. %-28s %3d sold   Rs.%.2f\n",
                   rank++, name.substr(0,26).c_str(),
                   count, jerseyRevenue[name]);
        }

        printDivider();

        // Top teams
        std::cout << YELLOW << BOLD << "\n  TOP TEAMS BY SALES\n" << RESET;
        std::vector<std::pair<std::string,int>> teamSorted(teamCount.begin(), teamCount.end());
        std::sort(teamSorted.begin(), teamSorted.end(),
            [](auto& a, auto& b){ return a.second > b.second; });

        rank = 1;
        for (auto& [team, count] : teamSorted) {
            if (rank > 5) break;
            printf("  %d. %-28s %3d sold\n", rank++, team.c_str(), count);
        }
        printDivider();
    }

    // ── CSV Export ────────────────────────────
    void exportCSV(const std::string& filepath = "data/sales_report.csv") const {
        std::filesystem::create_directories("data");
        std::ofstream f(filepath);
        f << "OrderId,FanId,FanName,JerseyId,JerseyName,Team,Player,"
          << "Sport,Quantity,Revenue,Date\n";
        for (auto& r : records_) {
            f << r.orderId    << ","
              << r.fanId      << ","
              << r.fanName    << ","
              << r.jerseyId   << ","
              << r.jerseyName << ","
              << r.team       << ","
              << r.player     << ","
              << (r.sport == Sport::FOOTBALL ? "Football" : "Cricket") << ","
              << r.quantity   << ","
              << r.revenue    << ","
              << r.date       << "\n";
        }
        printSuccess("Sales report exported: " + filepath);
    }

    // ── Frequency-based Recommender ───────────
    // Looks at what fan has bought, finds other fans
    // with similar purchases, recommends their items
    std::vector<int> recommend(int fanId,
                               const std::vector<int>& alreadyBought,
                               int topN = 5) const {
        // Step 1: count how often each jersey was bought by OTHER fans
        std::map<int, int> jerseyFreq;
        for (auto& r : records_) {
            if (r.fanId == fanId) continue; // skip own purchases
            jerseyFreq[r.jerseyId] += r.quantity;
        }

        // Step 2: boost jerseys from same teams as fan's purchases
        // (content-based boost)
        std::set<std::string> fanTeams;
        for (auto& r : records_) {
            if (r.fanId == fanId) fanTeams.insert(r.team);
        }

        for (auto& [jid, freq] : jerseyFreq) {
            for (auto& r : records_) {
                if (r.jerseyId == jid && fanTeams.count(r.team))
                    freq += 3; // boost same-team jerseys
            }
        }

        // Step 3: remove already bought
        for (int id : alreadyBought) jerseyFreq.erase(id);

        // Step 4: sort by frequency and return top N
        std::vector<std::pair<int,int>> sorted(jerseyFreq.begin(), jerseyFreq.end());
        std::sort(sorted.begin(), sorted.end(),
            [](auto& a, auto& b){ return a.second > b.second; });

        std::vector<int> result;
        for (auto& [jid, _] : sorted) {
            if ((int)result.size() >= topN) break;
            result.push_back(jid);
        }
        return result;
    }

    void showRecommendations(int fanId,
                             const std::vector<int>& alreadyBought,
                             const std::map<int, Jersey*>& jerseyMap) const {
        auto recs = recommend(fanId, alreadyBought);
        printHeader("RECOMMENDED FOR YOU");
        if (recs.empty()) {
            printInfo("Browse some jerseys first to get recommendations!");
            return;
        }
        int i = 1;
        for (int id : recs) {
            auto it = jerseyMap.find(id);
            if (it == jerseyMap.end()) continue;
            Jersey* j = it->second;
            std::string sport = (j->getSport() == Sport::FOOTBALL) ? "⚽" : "🏏";
            printf("  %d. %s %-30s Rs.%.2f\n",
                   i++, sport.c_str(),
                   j->getName().substr(0,28).c_str(),
                   j->getFinalPrice());
        }
        printDivider();
    }

    int totalSales() const { return (int)records_.size(); }
};
