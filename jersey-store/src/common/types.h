#pragma once
#include <string>
#include <vector>
#include <map>

// ─────────────────────────────────────────────
//  Enums
// ─────────────────────────────────────────────
enum class Sport       { FOOTBALL, CRICKET };
enum class KitType     { HOME, AWAY, THIRD, TRAINING };
enum class CricketFormat { T20, ODI, TEST };
enum class OrderStatus { PENDING, CONFIRMED, SHIPPED, DELIVERED, CANCELLED };
enum class SizeType    { XS, S, M, L, XL, XXL };

inline std::string sportStr(Sport s) {
    return s == Sport::FOOTBALL ? "Football" : "Cricket";
}

inline std::string kitTypeStr(KitType k) {
    switch(k) {
        case KitType::HOME:     return "Home";
        case KitType::AWAY:     return "Away";
        case KitType::THIRD:    return "Third";
        case KitType::TRAINING: return "Training";
        default:                return "Unknown";
    }
}

inline std::string sizeStr(SizeType s) {
    switch(s) {
        case SizeType::XS:  return "XS";
        case SizeType::S:   return "S";
        case SizeType::M:   return "M";
        case SizeType::L:   return "L";
        case SizeType::XL:  return "XL";
        case SizeType::XXL: return "XXL";
        default:            return "M";
    }
}

// ─────────────────────────────────────────────
//  Transaction record (exported to JSON for ML)
// ─────────────────────────────────────────────
struct Transaction {
    int         fan_id;
    int         jersey_id;
    std::string team;
    std::string player;
    Sport       sport;
    double      price_paid;
    double      discount_applied;
    std::string date;           // YYYY-MM-DD
    std::string season;
};
