#pragma once
#include "common/types.h"
#include <iostream>
#include <iomanip>
#include <stdexcept>

// ─────────────────────────────────────────────
//  Abstract base class — Jersey
// ─────────────────────────────────────────────
class Jersey {
protected:
    int         id_;
    std::string name_;          // e.g. "Haaland #9 Home 2024/25"
    std::string player_;        // e.g. "Erling Haaland"
    std::string team_;          // e.g. "Manchester City"
    double      price_;
    int         stock_;
    std::string season_;        // e.g. "2024/25"
    SizeType    size_;
    bool        isLimitedEdition_;

public:
    Jersey(int id, std::string name, std::string player,
           std::string team, double price, int stock,
           std::string season, SizeType size, bool limited = false)
        : id_(id), name_(std::move(name)), player_(std::move(player)),
          team_(std::move(team)), price_(price), stock_(stock),
          season_(std::move(season)), size_(size), isLimitedEdition_(limited) {}

    virtual ~Jersey() = default;

    // ── Pure virtual ──────────────────────────
    virtual std::string getCategory() const = 0;
    virtual void        display()     const = 0;
    virtual double      getDiscount() const = 0;   // polymorphic discount logic
    virtual Sport       getSport()    const = 0;
    virtual std::string getDetails()  const = 0;   // sport-specific details

    // ── Concrete methods ──────────────────────
    int         getId()      const { return id_; }
    std::string getName()    const { return name_; }
    std::string getPlayer()  const { return player_; }
    std::string getTeam()    const { return team_; }
    double      getPrice()   const { return price_; }
    int         getStock()   const { return stock_; }
    std::string getSeason()  const { return season_; }
    SizeType    getSize()    const { return size_; }
    bool        isLimited()  const { return isLimitedEdition_; }
    bool        inStock()    const { return stock_ > 0; }

    double getFinalPrice() const {
        return price_ * (1.0 - getDiscount() / 100.0);
    }

    void reduceStock(int qty = 1) {
        if (stock_ < qty)
            throw std::runtime_error("Insufficient stock for: " + name_);
        stock_ -= qty;
    }

    void restock(int qty) { stock_ += qty; }
};


// ─────────────────────────────────────────────
//  Derived class — FootballJersey
// ─────────────────────────────────────────────
class FootballJersey : public Jersey {
    std::string league_;        // EPL, La Liga, ISL, UCL
    KitType     kitType_;
    bool        isPlayerEdition_;  // true = name+number on back

public:
    FootballJersey(int id, std::string name, std::string player,
                   std::string team, double price, int stock,
                   std::string season, SizeType size,
                   std::string league, KitType kitType,
                   bool playerEdition = true, bool limited = false)
        : Jersey(id, std::move(name), std::move(player), std::move(team),
                 price, stock, std::move(season), size, limited)
        , league_(std::move(league))
        , kitType_(kitType)
        , isPlayerEdition_(playerEdition) {}

    // Discount logic: away + third kits get higher discount
    // Limited editions never discounted
    double getDiscount() const override {
        if (isLimitedEdition_) return 0.0;
        if (kitType_ == KitType::AWAY)     return 10.0;
        if (kitType_ == KitType::THIRD)    return 15.0;
        if (kitType_ == KitType::TRAINING) return 20.0;
        return 5.0;  // home kit — small discount
    }

    std::string getCategory() const override { return "Football Jersey"; }
    Sport       getSport()    const override { return Sport::FOOTBALL; }

    std::string getDetails() const override {
        return league_ + " | " + kitTypeStr(kitType_)
             + (isPlayerEdition_ ? " | Player Edition" : " | Replica");
    }

    std::string getLeague()  const { return league_; }
    KitType     getKitType() const { return kitType_; }

    void display() const override {
        std::cout << "\n  [FOOTBALL] " << name_ << "\n"
                  << "  Player  : " << player_ << " | Team: " << team_ << "\n"
                  << "  League  : " << league_ << " | Kit: " << kitTypeStr(kitType_) << "\n"
                  << "  Season  : " << season_  << " | Size: " << sizeStr(size_) << "\n"
                  << "  Price   : Rs." << std::fixed << std::setprecision(2) << price_
                  << "  Discount: " << getDiscount() << "%"
                  << "  Final: Rs." << getFinalPrice() << "\n"
                  << "  Stock   : " << stock_
                  << (isLimitedEdition_ ? "  [LIMITED EDITION]" : "") << "\n";
    }
};


// ─────────────────────────────────────────────
//  Derived class — CricketJersey
// ─────────────────────────────────────────────
class CricketJersey : public Jersey {
    std::string    tournament_;   // IPL, T20 WC, Border-Gavaskar
    CricketFormat  format_;
    std::string    franchiseName_; // e.g. "Chennai Super Kings"

public:
    CricketJersey(int id, std::string name, std::string player,
                  std::string team, double price, int stock,
                  std::string season, SizeType size,
                  std::string tournament, CricketFormat format,
                  std::string franchise = "", bool limited = false)
        : Jersey(id, std::move(name), std::move(player), std::move(team),
                 price, stock, std::move(season), size, limited)
        , tournament_(std::move(tournament))
        , format_(format)
        , franchiseName_(std::move(franchise)) {}

    // Discount logic: post-tournament, test jerseys discounted more
    double getDiscount() const override {
        if (isLimitedEdition_) return 0.0;
        if (format_ == CricketFormat::TEST) return 20.0;  // less popular
        if (format_ == CricketFormat::ODI)  return 10.0;
        return 5.0;  // T20 / IPL — most popular, least discount
    }

    std::string getCategory() const override { return "Cricket Jersey"; }
    Sport       getSport()    const override { return Sport::CRICKET; }

    std::string getDetails() const override {
        std::string fmt = (format_ == CricketFormat::T20) ? "T20"
                        : (format_ == CricketFormat::ODI)  ? "ODI" : "Test";
        return tournament_ + " | " + fmt
             + (franchiseName_.empty() ? "" : " | " + franchiseName_);
    }

    std::string getTournament() const { return tournament_; }
    CricketFormat getFormat()   const { return format_; }

    void display() const override {
        std::cout << "\n  [CRICKET] " << name_ << "\n"
                  << "  Player     : " << player_ << " | Team: " << team_ << "\n"
                  << "  Tournament : " << tournament_ << "\n"
                  << "  Season     : " << season_ << " | Size: " << sizeStr(size_) << "\n"
                  << "  Price      : Rs." << std::fixed << std::setprecision(2) << price_
                  << "  Discount: " << getDiscount() << "%"
                  << "  Final: Rs." << getFinalPrice() << "\n"
                  << "  Stock      : " << stock_
                  << (isLimitedEdition_ ? "  [LIMITED EDITION]" : "") << "\n";
    }
};
