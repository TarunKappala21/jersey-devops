#pragma once
#include "inventory/Jersey.h"
#include "common/Colors.h"
#include <vector>
#include <map>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <ctime>
#include <chrono>

// ─────────────────────────────────────────────
//  CartItem — one line in the cart
// ─────────────────────────────────────────────
struct CartItem {
    int    jerseyId;
    std::string jerseyName;
    std::string team;
    std::string player;
    Sport  sport;
    int    quantity;
    double unitPrice;   // after discount
    double discount;

    double subtotal() const { return unitPrice * quantity; }
};

// ─────────────────────────────────────────────
//  Cart — composition: Cart HAS CartItems
// ─────────────────────────────────────────────
class Cart {
    std::vector<CartItem> items_;
    int fanId_;

public:
    explicit Cart(int fanId) : fanId_(fanId) {}

    bool addItem(Jersey* jersey, int qty = 1) {
        if (!jersey) return false;
        if (jersey->getStock() < qty) {
            printError("Only " + std::to_string(jersey->getStock()) + " in stock!");
            return false;
        }

        // If already in cart, increase qty
        for (auto& item : items_) {
            if (item.jerseyId == jersey->getId()) {
                item.quantity += qty;
                printSuccess("Updated quantity for " + jersey->getName());
                return true;
            }
        }

        items_.push_back({
            jersey->getId(),
            jersey->getName(),
            jersey->getTeam(),
            jersey->getPlayer(),
            jersey->getSport(),
            qty,
            jersey->getFinalPrice(),
            jersey->getDiscount()
        });
        printSuccess("Added to cart: " + jersey->getName());
        return true;
    }

    bool removeItem(int jerseyId) {
        auto it = std::find_if(items_.begin(), items_.end(),
            [jerseyId](const CartItem& c){ return c.jerseyId == jerseyId; });
        if (it == items_.end()) {
            printError("Item not found in cart.");
            return false;
        }
        printInfo("Removed: " + it->jerseyName);
        items_.erase(it);
        return true;
    }

    void clear() { items_.clear(); }

    bool isEmpty() const { return items_.empty(); }

    double getTotal() const {
        double total = 0;
        for (auto& item : items_) total += item.subtotal();
        return total;
    }

    double getGST() const { return getTotal() * 0.18; }

    double getGrandTotal() const { return getTotal() + getGST(); }

    const std::vector<CartItem>& getItems() const { return items_; }

    int size() const { return (int)items_.size(); }

    void display() const {
        if (items_.empty()) {
            printWarning("Your cart is empty!");
            return;
        }
        printHeader("SHOPPING CART");
        printf("%-28s %6s %10s %8s\n", "Jersey", "Qty", "Price", "Subtotal");
        printDivider();

        for (auto& item : items_) {
            std::string sport = (item.sport == Sport::FOOTBALL) ? "⚽" : "🏏";
            printf("%s %-26s %6d %10.2f %8.2f\n",
                sport.c_str(),
                item.jerseyName.substr(0, 24).c_str(),
                item.quantity,
                item.unitPrice,
                item.subtotal());
        }
        printDivider();
        printf("%-28s %26.2f\n", "Subtotal:", getTotal());
        printf("%-28s %26.2f\n", "GST (18%):", getGST());
        std::cout << GREEN << BOLD;
        printf("%-28s %26.2f\n", "Grand Total:", getGrandTotal());
        std::cout << RESET;
        printDivider();
    }

    // Generate invoice as TXT file
    std::string generateInvoice(const std::string& customerName, int orderId) const {
        std::filesystem::create_directories("invoices");
        std::string filename = "invoices/invoice_" + std::to_string(orderId) + ".txt";
        std::ofstream f(filename);

        auto now  = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);

        f << "================================================\n"
          << "         SPORTS JERSEY STORE\n"
          << "         Invoice #" << orderId << "\n"
          << "================================================\n"
          << "Customer : " << customerName << "\n"
          << "Date     : " << std::ctime(&time)
          << "------------------------------------------------\n";

        f << std::left << std::setw(28) << "Item"
          << std::setw(6)  << "Qty"
          << std::setw(10) << "Price"
          << std::setw(10) << "Subtotal" << "\n";
        f << std::string(54, '-') << "\n";

        for (auto& item : items_) {
            f << std::left  << std::setw(28) << item.jerseyName.substr(0,26)
              << std::setw(6)  << item.quantity
              << std::right << std::fixed << std::setprecision(2)
              << std::setw(10) << item.unitPrice
              << std::setw(10) << item.subtotal() << "\n";
            if (item.discount > 0)
                f << "  (Discount applied: " << item.discount << "%)\n";
        }

        f << std::string(54, '-') << "\n"
          << std::left << std::setw(44) << "Subtotal:"
          << std::right << std::setw(10) << getTotal() << "\n"
          << std::left << std::setw(44) << "GST (18%):"
          << std::right << std::setw(10) << getGST() << "\n"
          << std::string(54, '=') << "\n"
          << std::left << std::setw(44) << "GRAND TOTAL:"
          << std::right << std::setw(10) << getGrandTotal() << "\n"
          << std::string(54, '=') << "\n"
          << "\nThank you for shopping! Come back soon.\n";

        return filename;
    }
};
