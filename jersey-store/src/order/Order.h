#pragma once
#include "customer/Fan.h"
#include "inventory/Jersey.h"
#include "inventory/Inventory.h"
#include <vector>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <filesystem>

// ─────────────────────────────────────────────
//  Order — ties Fan + Jerseys together
// ─────────────────────────────────────────────
class Order {
    static int nextOrderId_;

    int          orderId_;
    Fan&         fan_;
    Inventory&   inventory_;
    OrderStatus  status_;
    std::string  date_;

    struct OrderItem {
        int    jerseyId;
        int    quantity;
        double priceAtPurchase;
        double discountApplied;
    };

    std::vector<OrderItem> items_;
    double totalAmount_ = 0.0;

    std::string currentDate() const {
        auto now  = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d");
        return oss.str();
    }

public:
    Order(Fan& fan, Inventory& inventory)
        : orderId_(nextOrderId_++)
        , fan_(fan)
        , inventory_(inventory)
        , status_(OrderStatus::PENDING)
        , date_(currentDate()) {}

    // Add jersey to order
    bool addItem(int jerseyId, int qty = 1) {
        Jersey* j = inventory_.getJersey(jerseyId);
        if (!j) {
            std::cout << "Jersey ID " << jerseyId << " not found.\n";
            return false;
        }
        if (j->getStock() < qty) {
            std::cout << "Insufficient stock for: " << j->getName() << "\n";
            return false;
        }
        items_.push_back({ jerseyId, qty, j->getFinalPrice(), j->getDiscount() });
        totalAmount_ += j->getFinalPrice() * qty;
        std::cout << "Added to cart: " << j->getName()
                  << " x" << qty << " @ Rs." << j->getFinalPrice() << "\n";
        return true;
    }

    // Confirm and process order
    bool checkout() {
        if (items_.empty()) {
            std::cout << "Cart is empty!\n";
            return false;
        }

        // Reduce stock for each item
        for (auto& item : items_) {
            Jersey* j = inventory_.getJersey(item.jerseyId);
            j->reduceStock(item.quantity);

            // Record transaction for ML
            Transaction t;
            t.fan_id           = fan_.getId();
            t.jersey_id        = item.jerseyId;
            t.team             = j->getTeam();
            t.player           = j->getPlayer();
            t.sport            = j->getSport();
            t.price_paid       = item.priceAtPurchase * item.quantity;
            t.discount_applied = item.discountApplied;
            t.date             = date_;
            t.season           = j->getSeason();
            fan_.recordPurchase(t);
        }

        status_ = OrderStatus::CONFIRMED;
        std::cout << "\n✅ Order #" << orderId_ << " confirmed!"
                  << " Total: Rs." << totalAmount_ << "\n";
        exportTransactionJSON();
        return true;
    }

    void display() const {
        std::cout << "\n=== ORDER #" << orderId_ << " ===\n"
                  << "Fan    : " << fan_.getName() << "\n"
                  << "Date   : " << date_ << "\n"
                  << "Status : ";
        switch(status_) {
            case OrderStatus::PENDING:   std::cout << "Pending\n";   break;
            case OrderStatus::CONFIRMED: std::cout << "Confirmed\n"; break;
            case OrderStatus::SHIPPED:   std::cout << "Shipped\n";   break;
            case OrderStatus::DELIVERED: std::cout << "Delivered\n"; break;
            case OrderStatus::CANCELLED: std::cout << "Cancelled\n"; break;
        }
        std::cout << "Items  :\n";
        for (auto& item : items_) {
            std::cout << "  Jersey #" << item.jerseyId
                      << " x" << item.quantity
                      << " @ Rs." << item.priceAtPurchase
                      << " (Discount: " << item.discountApplied << "%)\n";
        }
        std::cout << "Total  : Rs." << totalAmount_ << "\n";
    }

    double getTotal()   const { return totalAmount_; }
    int    getOrderId() const { return orderId_; }

    // Export this order's transactions to JSON for ML
    void exportTransactionJSON(const std::string& filepath = "data/transactions.json") const;
};


