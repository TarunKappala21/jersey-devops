#pragma once
#include "api/JsonBuilder.h"
#include "inventory/Jersey.h"
#include "cart/Cart.h"
#include "analytics/Analytics.h"
#include <vector>

// ─────────────────────────────────────────────
//  Serializer — converts domain objects → JSON
// ─────────────────────────────────────────────
namespace Serializer {

inline std::string jersey(const Jersey* j) {
    if (!j) return JSON::null_val();
    return JSON::object({
        {"id",       JSON::num(j->getId())},
        {"name",     JSON::str(j->getName())},
        {"player",   JSON::str(j->getPlayer())},
        {"team",     JSON::str(j->getTeam())},
        {"sport",    JSON::str(sportStr(j->getSport()))},
        {"category", JSON::str(j->getCategory())},
        {"details",  JSON::str(j->getDetails())},
        {"price",    JSON::num(j->getPrice())},
        {"finalPrice",JSON::num(j->getFinalPrice())},
        {"discount", JSON::num(j->getDiscount())},
        {"stock",    JSON::num(j->getStock())},
        {"season",   JSON::str(j->getSeason())},
        {"size",     JSON::str(sizeStr(j->getSize()))},
        {"limited",  JSON::boolean(j->isLimited())},
        {"inStock",  JSON::boolean(j->inStock())}
    });
}

inline std::string jerseyList(const std::vector<Jersey*>& list) {
    std::vector<std::string> items;
    for (auto* j : list) items.push_back(jersey(j));
    return JSON::array(items);
}

inline std::string cartItem(const CartItem& item) {
    return JSON::object({
        {"jerseyId",   JSON::num(item.jerseyId)},
        {"jerseyName", JSON::str(item.jerseyName)},
        {"team",       JSON::str(item.team)},
        {"player",     JSON::str(item.player)},
        {"sport",      JSON::str(item.sport == Sport::FOOTBALL ? "Football" : "Cricket")},
        {"quantity",   JSON::num(item.quantity)},
        {"unitPrice",  JSON::num(item.unitPrice)},
        {"discount",   JSON::num(item.discount)},
        {"subtotal",   JSON::num(item.subtotal())}
    });
}

inline std::string cart(const Cart& c) {
    std::vector<std::string> items;
    for (auto& item : c.getItems()) items.push_back(cartItem(item));
    return JSON::object({
        {"items",      JSON::array(items)},
        {"subtotal",   JSON::num(c.getTotal())},
        {"gst",        JSON::num(c.getGST())},
        {"grandTotal", JSON::num(c.getGrandTotal())},
        {"itemCount",  JSON::num(c.size())}
    });
}

inline std::string analyticsOverview(const Analytics& a,
                                      const std::vector<SalesRecord>& records) {
    double totalRevenue = 0; int totalOrders = 0;
    int    footballSales = 0, cricketSales = 0;
    std::map<std::string,int>    jerseyCount;
    std::map<std::string,double> jerseyRevenue;
    std::map<std::string,int>    teamCount;

    for (auto& r : records) {
        totalRevenue += r.revenue;
        totalOrders  += r.quantity;
        jerseyCount[r.jerseyName]   += r.quantity;
        jerseyRevenue[r.jerseyName] += r.revenue;
        teamCount[r.team]           += r.quantity;
        if (r.sport == Sport::FOOTBALL) footballSales += r.quantity;
        else                            cricketSales  += r.quantity;
    }

    // Top 5 jerseys
    std::vector<std::pair<std::string,int>> sorted(jerseyCount.begin(), jerseyCount.end());
    std::sort(sorted.begin(), sorted.end(),
        [](auto& a, auto& b){ return a.second > b.second; });

    std::vector<std::string> topJerseys;
    int rank = 1;
    for (auto& [name, count] : sorted) {
        if (rank++ > 5) break;
        topJerseys.push_back(JSON::object({
            {"name",    JSON::str(name)},
            {"sold",    JSON::num(count)},
            {"revenue", JSON::num(jerseyRevenue[name])}
        }));
    }

    // Top 5 teams
    std::vector<std::pair<std::string,int>> teamSorted(teamCount.begin(), teamCount.end());
    std::sort(teamSorted.begin(), teamSorted.end(),
        [](auto& a, auto& b){ return a.second > b.second; });

    std::vector<std::string> topTeams;
    rank = 1;
    for (auto& [team, count] : teamSorted) {
        if (rank++ > 5) break;
        topTeams.push_back(JSON::object({
            {"team", JSON::str(team)},
            {"sold", JSON::num(count)}
        }));
    }

    return JSON::object({
        {"totalRevenue",   JSON::num(totalRevenue)},
        {"totalOrders",    JSON::num(totalOrders)},
        {"footballSales",  JSON::num(footballSales)},
        {"cricketSales",   JSON::num(cricketSales)},
        {"topJerseys",     JSON::array(topJerseys)},
        {"topTeams",       JSON::array(topTeams)}
    });
}

} // namespace Serializer
