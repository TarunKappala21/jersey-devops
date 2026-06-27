#include "order/Order.h"
#include <fstream>

int Order::nextOrderId_ = 1001;
#include <filesystem>
#include <iostream>

void Order::exportTransactionJSON(const std::string& filepath) const {
    std::filesystem::create_directories("data");

    // Append to existing file
    std::ofstream f(filepath, std::ios::app);
    if (!f.is_open()) {
        std::cerr << "Warning: could not open " << filepath << "\n";
        return;
    }

    for (auto& item : items_) {
        f << "{"
          << "\"order_id\": "    << orderId_           << ", "
          << "\"fan_id\": "      << fan_.getId()        << ", "
          << "\"jersey_id\": "   << item.jerseyId       << ", "
          << "\"quantity\": "    << item.quantity        << ", "
          << "\"price_paid\": "  << item.priceAtPurchase << ", "
          << "\"discount\": "    << item.discountApplied << ", "
          << "\"date\": \""      << date_               << "\""
          << "}\n";
    }
}
