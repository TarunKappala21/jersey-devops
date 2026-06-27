#pragma once
#include "inventory/Jersey.h"
#include "common/Colors.h"
#include <map>
#include <vector>
#include <memory>
#include <algorithm>
#include <fstream>
#include <set>

class Inventory {
    std::map<int, std::unique_ptr<Jersey>> jerseys_;
    int nextId_ = 1;

public:
    void addJersey(std::unique_ptr<Jersey> jersey) {
        int id = jersey->getId();
        jerseys_[id] = std::move(jersey);
    }

    bool removeJersey(int id) {
        auto it = jerseys_.find(id);
        if (it == jerseys_.end()) return false;
        jerseys_.erase(it);
        return true;
    }

    Jersey* getJersey(int id) {
        auto it = jerseys_.find(id);
        return it != jerseys_.end() ? it->second.get() : nullptr;
    }

    void restock(int id, int qty) {
        Jersey* j = getJersey(id);
        if (j) j->restock(qty);
        else throw std::runtime_error("Jersey ID not found: " + std::to_string(id));
    }

    // ── Filters (STL algorithms) ───────────────
    std::vector<Jersey*> getBySport(Sport sport) const {
        std::vector<Jersey*> result;
        for (auto& [id, j] : jerseys_)
            if (j->getSport() == sport) result.push_back(j.get());
        return result;
    }

    std::vector<Jersey*> getByTeam(const std::string& team) const {
        std::vector<Jersey*> result;
        for (auto& [id, j] : jerseys_)
            if (j->getTeam() == team) result.push_back(j.get());
        return result;
    }

    std::vector<Jersey*> getByPriceRange(double minPrice, double maxPrice) const {
        std::vector<Jersey*> result;
        for (auto& [id, j] : jerseys_)
            if (j->getFinalPrice() >= minPrice && j->getFinalPrice() <= maxPrice)
                result.push_back(j.get());
        return result;
    }

    std::vector<Jersey*> getBySize(SizeType size) const {
        std::vector<Jersey*> result;
        for (auto& [id, j] : jerseys_)
            if (j->getSize() == size) result.push_back(j.get());
        return result;
    }

    // ── Search ────────────────────────────────
    std::vector<Jersey*> search(const std::string& query) const {
        std::string q = query;
        std::transform(q.begin(), q.end(), q.begin(), ::tolower);

        std::vector<Jersey*> result;
        for (auto& [id, j] : jerseys_) {
            std::string name   = j->getName();   std::transform(name.begin(),   name.end(),   name.begin(),   ::tolower);
            std::string player = j->getPlayer(); std::transform(player.begin(), player.end(), player.begin(), ::tolower);
            std::string team   = j->getTeam();   std::transform(team.begin(),   team.end(),   team.begin(),   ::tolower);

            if (name.find(q)   != std::string::npos ||
                player.find(q) != std::string::npos ||
                team.find(q)   != std::string::npos) {
                result.push_back(j.get());
            }
        }
        return result;
    }

    std::vector<Jersey*> getLowStock(int threshold = 5) const {
        std::vector<Jersey*> result;
        for (auto& [id, j] : jerseys_)
            if (j->getStock() < threshold) result.push_back(j.get());
        return result;
    }

    // For recommender engine
    std::map<int, Jersey*> getJerseyMap() const {
        std::map<int, Jersey*> m;
        for (auto& [id, j] : jerseys_) m[id] = j.get();
        return m;
    }

    // ── Display helpers ────────────────────────
    void displayList(const std::vector<Jersey*>& list) const {
        if (list.empty()) { printWarning("No jerseys found."); return; }
        printDivider();
        printf("%-4s %-30s %-12s %8s %6s %6s\n",
               "ID", "Name", "Team", "Price", "Disc%", "Stock");
        printDivider();
        for (auto* j : list) {
            std::string sport = (j->getSport() == Sport::FOOTBALL) ? "⚽" : "🏏";
            std::string stockColor = j->getStock() < 5 ? RED : (j->getStock() < 10 ? YELLOW : GREEN);
            printf("%s%-4d %-30s %-12s %8.0f %5.0f%% %s%6d%s\n",
                   sport.c_str(),
                   j->getId(),
                   j->getName().substr(0,28).c_str(),
                   j->getTeam().substr(0,10).c_str(),
                   j->getFinalPrice(),
                   j->getDiscount(),
                   stockColor.c_str(), j->getStock(), RESET);
        }
        printDivider();
    }

    void displayAll() const {
        printHeader("ALL JERSEYS");
        std::vector<Jersey*> all;
        for (auto& [id, j] : jerseys_) all.push_back(j.get());
        displayList(all);
    }

    void displayBySport(Sport sport) const {
        printHeader(sportStr(sport) + " JERSEYS");
        displayList(getBySport(sport));
    }

    void updateStock(int id, int newStock) {
        Jersey* j = getJersey(id);
        if (j) { j->restock(newStock - j->getStock()); }
    }

    int size() const { return (int)jerseys_.size(); }

    void exportToJSON(const std::string& filepath = "data/inventory.json") const;
};
