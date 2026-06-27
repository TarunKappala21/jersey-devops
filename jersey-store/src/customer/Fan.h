#pragma once
#include "common/types.h"
#include <string>
#include <vector>
#include <iostream>

// ─────────────────────────────────────────────
//  Abstract base — Person
// ─────────────────────────────────────────────
class Person {
protected:
    int         id_;
    std::string name_;
    std::string email_;

public:
    Person(int id, std::string name, std::string email)
        : id_(id), name_(std::move(name)), email_(std::move(email)) {}

    virtual ~Person() = default;
    virtual void display() const = 0;  // pure virtual

    int         getId()    const { return id_; }
    std::string getName()  const { return name_; }
    std::string getEmail() const { return email_; }
};


// ─────────────────────────────────────────────
//  Derived — Fan (your customer)
// ─────────────────────────────────────────────
class Fan : public Person {
    std::string              favFootballClub_;   // e.g. "Manchester City"
    std::string              favCricketTeam_;    // e.g. "India"
    std::vector<std::string> favPlayers_;        // e.g. ["Haaland", "Kohli"]
    std::vector<std::string> favLeagues_;        // e.g. ["EPL", "IPL"]
    std::vector<Transaction> purchaseHistory_;
    double                   totalSpent_ = 0.0;

public:
    Fan(int id, std::string name, std::string email,
        std::string favFootballClub = "",
        std::string favCricketTeam  = "")
        : Person(id, std::move(name), std::move(email))
        , favFootballClub_(std::move(favFootballClub))
        , favCricketTeam_(std::move(favCricketTeam)) {}

    // Getters
    std::string              getFavFootballClub() const { return favFootballClub_; }
    std::string              getFavCricketTeam()  const { return favCricketTeam_; }
    std::vector<std::string> getFavPlayers()      const { return favPlayers_; }
    std::vector<Transaction> getPurchaseHistory() const { return purchaseHistory_; }
    double                   getTotalSpent()      const { return totalSpent_; }

    // Setters
    void addFavPlayer(const std::string& player) { favPlayers_.push_back(player); }
    void addFavLeague(const std::string& league) { favLeagues_.push_back(league); }

    void recordPurchase(const Transaction& t) {
        purchaseHistory_.push_back(t);
        totalSpent_ += t.price_paid;
    }

    // Tags for ML recommendation engine
    // Returns all team/player preferences as a tag list
    std::vector<std::string> getPreferenceTags() const {
        std::vector<std::string> tags;
        if (!favFootballClub_.empty()) tags.push_back(favFootballClub_);
        if (!favCricketTeam_.empty())  tags.push_back(favCricketTeam_);
        for (auto& p : favPlayers_)   tags.push_back(p);
        for (auto& l : favLeagues_)   tags.push_back(l);
        // Add teams from purchase history
        for (auto& t : purchaseHistory_) tags.push_back(t.team);
        return tags;
    }

    bool isSportsFan(Sport sport) const {
        if (sport == Sport::FOOTBALL) return !favFootballClub_.empty();
        return !favCricketTeam_.empty();
    }

    void display() const override {
        std::cout << "\n  Fan #" << id_ << ": " << name_
                  << " | Email: " << email_ << "\n"
                  << "  Fav Football Club : " << (favFootballClub_.empty() ? "N/A" : favFootballClub_) << "\n"
                  << "  Fav Cricket Team  : " << (favCricketTeam_.empty()  ? "N/A" : favCricketTeam_)  << "\n"
                  << "  Fav Players       : ";
        for (auto& p : favPlayers_) std::cout << p << "  ";
        std::cout << "\n  Total Spent       : Rs." << totalSpent_
                  << " | Orders: " << purchaseHistory_.size() << "\n";
    }

    // Export fan profile to JSON for ML
    void exportToJSON(std::ofstream& f) const;
};
