#pragma once
#include "inventory/Inventory.h"
#include "inventory/Jersey.h"
#include "customer/Fan.h"
#include "order/Order.h"
#include <map>
#include <memory>
#include <iostream>
#include <filesystem>
#include <fstream>

// ─────────────────────────────────────────────
//  Store — top-level facade
//  Wires Inventory + Fans + Orders together
// ─────────────────────────────────────────────
class Store {
    Inventory                       inventory_;
    std::map<int, Fan>              fans_;
    std::vector<std::unique_ptr<Order>> orders_;
    int                             nextFanId_ = 1;

public:
    Store() {
        std::filesystem::create_directories("data");
        loadSampleInventory();
        loadSampleFans();
    }

    // ── Inventory ─────────────────────────────
    Inventory& getInventory() { return inventory_; }

    void showAllJerseys()  const { inventory_.displayAll(); }
    void showFootball()    const { inventory_.displayBySport(Sport::FOOTBALL); }
    void showCricket()     const { inventory_.displayBySport(Sport::CRICKET); }

    void showLowStock() const {
        auto low = inventory_.getLowStock(5);
        std::cout << "\n⚠️  LOW STOCK ALERT (" << low.size() << " jerseys)\n";
        for (auto* j : low)
            std::cout << "  #" << j->getId() << " " << j->getName()
                      << " — Stock: " << j->getStock() << "\n";
    }

    // ── Fans ──────────────────────────────────
    Fan& addFan(std::string name, std::string email,
                std::string favFootball = "", std::string favCricket = "") {
        int id = nextFanId_++;
        fans_.emplace(id, Fan(id, name, email, favFootball, favCricket));
        return fans_.at(id);
    }

    Fan* getFan(int id) {
        auto it = fans_.find(id);
        return it != fans_.end() ? &it->second : nullptr;
    }

    void showFan(int id) {
        Fan* f = getFan(id);
        if (f) f->display();
        else std::cout << "Fan not found.\n";
    }

    // ── Orders ────────────────────────────────
    Order& createOrder(int fanId) {
        Fan* fan = getFan(fanId);
        if (!fan) throw std::runtime_error("Fan not found: " + std::to_string(fanId));
        orders_.push_back(std::make_unique<Order>(*fan, inventory_));
        return *orders_.back();
    }

    // ── Export everything for ML ──────────────
    void exportAllData() {
        inventory_.exportToJSON("data/inventory.json");
        exportFansJSON("data/fans.json");
        std::cout << "All data exported to data/ folder for ML pipeline\n";
    }

    void exportFansJSON(const std::string& filepath) {
        std::ofstream f(filepath);
        f << "[\n";
        bool first = true;
        for (auto& [id, fan] : fans_) {
            if (!first) f << ",\n";
            first = false;
            fan.exportToJSON(f);
        }
        f << "\n]\n";
        std::cout << "Fans exported to " << filepath << "\n";
    }

    // ── Sample data loader ────────────────────
    void loadSampleInventory() {
        // Football jerseys
        inventory_.addJersey(std::make_unique<FootballJersey>(
            1, "Haaland #9 Home 2024/25", "Erling Haaland", "Manchester City",
            4999.0, 20, "2024/25", SizeType::L, "EPL", KitType::HOME, true));

        inventory_.addJersey(std::make_unique<FootballJersey>(
            2, "Vinicius Jr #7 Away 2024/25", "Vinicius Jr", "Real Madrid",
            5499.0, 15, "2024/25", SizeType::M, "La Liga", KitType::AWAY, true));

        inventory_.addJersey(std::make_unique<FootballJersey>(
            3, "Mbappe #9 Home 2024/25", "Kylian Mbappe", "Real Madrid",
            5999.0, 8, "2024/25", SizeType::L, "La Liga", KitType::HOME, true, true));

        inventory_.addJersey(std::make_unique<FootballJersey>(
            4, "Salah #11 Home 2024/25", "Mohamed Salah", "Liverpool",
            4799.0, 25, "2024/25", SizeType::XL, "EPL", KitType::HOME, true));

        inventory_.addJersey(std::make_unique<FootballJersey>(
            5, "Saka #7 Away 2024/25", "Bukayo Saka", "Arsenal",
            4599.0, 18, "2024/25", SizeType::M, "EPL", KitType::AWAY, true));

        inventory_.addJersey(std::make_unique<FootballJersey>(
            6, "Ronaldo #7 Away 2024/25", "Cristiano Ronaldo", "Al Nassr",
            3999.0, 30, "2024/25", SizeType::L, "Saudi Pro League", KitType::AWAY, true));

        // Cricket jerseys
        inventory_.addJersey(std::make_unique<CricketJersey>(
            7, "Kohli #18 T20 WC 2024", "Virat Kohli", "India",
            2999.0, 50, "2024", SizeType::M, "T20 World Cup", CricketFormat::T20));

        inventory_.addJersey(std::make_unique<CricketJersey>(
            8, "Dhoni #7 CSK IPL 2024", "MS Dhoni", "CSK",
            3499.0, 3, "IPL 2024", SizeType::L, "IPL", CricketFormat::T20, "Chennai Super Kings", true));

        inventory_.addJersey(std::make_unique<CricketJersey>(
            9, "Rohit #45 ODI WC 2023", "Rohit Sharma", "India",
            2799.0, 35, "2023", SizeType::XL, "ODI World Cup", CricketFormat::ODI));

        inventory_.addJersey(std::make_unique<CricketJersey>(
            10, "Bumrah #93 Test 2024", "Jasprit Bumrah", "India",
            2499.0, 12, "2024", SizeType::M, "Border-Gavaskar Trophy", CricketFormat::TEST));

        inventory_.addJersey(std::make_unique<CricketJersey>(
            11, "Hardik #33 MI IPL 2024", "Hardik Pandya", "MI",
            3299.0, 22, "IPL 2024", SizeType::L, "IPL", CricketFormat::T20, "Mumbai Indians"));

        inventory_.addJersey(std::make_unique<CricketJersey>(
            12, "Stokes #55 Test 2024", "Ben Stokes", "England",
            2699.0, 4, "2024", SizeType::M, "The Ashes", CricketFormat::TEST));
    }

    void loadSampleFans() {
        // Sample fans with different preferences
        auto& tarun = addFan("Tarun", "tarun@nitw.ac.in", "Manchester City", "India");
        tarun.addFavPlayer("Haaland");
        tarun.addFavPlayer("Kohli");
        tarun.addFavLeague("EPL");
        tarun.addFavLeague("IPL");

        auto& fan2 = addFan("Arjun", "arjun@gmail.com", "Real Madrid", "CSK");
        fan2.addFavPlayer("Vinicius Jr");
        fan2.addFavPlayer("Dhoni");

        auto& fan3 = addFan("Priya", "priya@gmail.com", "Liverpool", "India");
        fan3.addFavPlayer("Salah");
        fan3.addFavPlayer("Rohit Sharma");
    }
};
