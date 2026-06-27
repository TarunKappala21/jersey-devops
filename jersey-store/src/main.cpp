#include <iostream>
#include <limits>
#include <set>
#include "Store.h"
#include "auth/Auth.h"
#include "cart/Cart.h"
#include "analytics/Analytics.h"

// ── Globals ───────────────────────────────────
AuthManager auth;
Analytics   analytics;
int         orderCounter = 2001;

void clearInput() { std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); }

std::string getInput(const std::string& prompt) {
    std::cout << CYAN << prompt << RESET;
    std::string s; std::getline(std::cin, s); return s;
}

int getInt(const std::string& prompt) {
    std::cout << CYAN << prompt << RESET;
    int v; std::cin >> v; clearInput(); return v;
}

// ─────────────────────────────────────────────
//  Login / Register screen
// ─────────────────────────────────────────────
void showLoginMenu(Store& store) {
    while (!auth.isLoggedIn()) {
        printHeader("SPORTS JERSEY STORE");
        std::cout << BOLD
                  << "  [1] Login\n"
                  << "  [2] Register\n"
                  << "  [0] Exit\n"
                  << RESET;
        int c = getInt("Choice: ");

        if (c == 0) exit(0);

        std::string user = getInput("Username: ");
        std::string pass = getInput("Password: ");

        if (c == 1) {
            auth.login(user, pass);
        } else if (c == 2) {
            std::string name = getInput("Full Name: ");
            if (auth.registerCustomer(user, pass, name))
                auth.login(user, pass);
        }
    }
}

// ─────────────────────────────────────────────
//  Admin Menu
// ─────────────────────────────────────────────
void adminMenu(Store& store) {
    while (true) {
        printHeader("ADMIN PANEL — " + auth.getCurrentName());
        std::cout << BOLD
                  << "  [1] View All Jerseys\n"
                  << "  [2] Add Jersey\n"
                  << "  [3] Update Stock\n"
                  << "  [4] Delete Jersey\n"
                  << "  [5] Low Stock Alerts\n"
                  << "  [6] Sales Dashboard\n"
                  << "  [7] Export Sales CSV\n"
                  << "  [8] Export Data for ML\n"
                  << "  [0] Logout\n"
                  << RESET;

        int c = getInt("Choice: ");
        switch(c) {
            case 0: auth.logout(); return;

            case 1: store.showAllJerseys(); break;

            case 2: {
                printHeader("ADD JERSEY");
                int    id      = getInt("Jersey ID: ");
                std::string name   = getInput("Name: ");
                std::string player = getInput("Player: ");
                std::string team   = getInput("Team: ");
                double price  = std::stod(getInput("Price (Rs.): "));
                int    stock  = getInt("Stock: ");
                std::string season = getInput("Season (e.g. 2024/25): ");

                std::cout << "Sport: [1] Football  [2] Cricket\n";
                int sp = getInt("Sport: ");

                if (sp == 1) {
                    std::string league = getInput("League: ");
                    store.getInventory().addJersey(std::make_unique<FootballJersey>(
                        id, name, player, team, price, stock, season,
                        SizeType::M, league, KitType::HOME));
                } else {
                    std::string tourn = getInput("Tournament: ");
                    store.getInventory().addJersey(std::make_unique<CricketJersey>(
                        id, name, player, team, price, stock, season,
                        SizeType::M, tourn, CricketFormat::T20));
                }
                printSuccess("Jersey added!");
                break;
            }

            case 3: {
                int id  = getInt("Jersey ID: ");
                int qty = getInt("New stock quantity: ");
                try {
                    store.getInventory().restock(id, qty);
                    printSuccess("Stock updated!");
                } catch (std::exception& e) { printError(e.what()); }
                break;
            }

            case 4: {
                int id = getInt("Jersey ID to delete: ");
                if (store.getInventory().removeJersey(id))
                    printSuccess("Jersey removed.");
                else
                    printError("Jersey not found.");
                break;
            }

            case 5: store.showLowStock(); break;

            case 6: analytics.showDashboard(); break;

            case 7: analytics.exportCSV(); break;

            case 8: store.exportAllData(); break;

            default: printError("Invalid choice.");
        }
    }
}

// ─────────────────────────────────────────────
//  Customer Menu
// ─────────────────────────────────────────────
void customerMenu(Store& store) {
    int fanId = auth.getFanId();
    Cart cart(fanId > 0 ? fanId : auth.getCurrentUserId());

    // Track what this fan has bought for recommender
    std::vector<int> boughtIds;

    while (true) {
        printHeader("JERSEY STORE — " + auth.getCurrentName());
        std::cout << BOLD
                  << "  [1] Browse All Jerseys\n"
                  << "  [2] Browse Football Jerseys\n"
                  << "  [3] Browse Cricket Jerseys\n"
                  << "  [4] Search Jerseys\n"
                  << "  [5] Filter by Price\n"
                  << "  [6] Add to Cart\n"
                  << "  [7] View Cart\n"
                  << "  [8] Remove from Cart\n"
                  << "  [9] Checkout\n"
                  << "  [10] Recommendations\n"
                  << "  [11] My Profile\n"
                  << "  [0]  Logout\n"
                  << RESET;

        int c = getInt("Choice: ");

        switch(c) {
            case 0: auth.logout(); return;

            case 1: store.showAllJerseys(); break;

            case 2: store.showFootball(); break;

            case 3: store.showCricket(); break;

            case 4: {
                std::string q = getInput("Search (player/team/name): ");
                auto results = store.getInventory().search(q);
                printHeader("SEARCH RESULTS: " + q);
                store.getInventory().displayList(results);
                break;
            }

            case 5: {
                double minP = std::stod(getInput("Min price (Rs.): "));
                double maxP = std::stod(getInput("Max price (Rs.): "));
                auto results = store.getInventory().getByPriceRange(minP, maxP);
                printHeader("JERSEYS Rs." + std::to_string((int)minP)
                          + " - Rs." + std::to_string((int)maxP));
                store.getInventory().displayList(results);
                break;
            }

            case 6: {
                store.showAllJerseys();
                int id  = getInt("Jersey ID to add: ");
                int qty = getInt("Quantity: ");
                Jersey* j = store.getInventory().getJersey(id);
                if (j) cart.addItem(j, qty);
                else   printError("Jersey not found.");
                break;
            }

            case 7: cart.display(); break;

            case 8: {
                cart.display();
                if (!cart.isEmpty()) {
                    int id = getInt("Jersey ID to remove: ");
                    cart.removeItem(id);
                }
                break;
            }

            case 9: {
                cart.display();
                if (cart.isEmpty()) break;

                std::cout << YELLOW << "Confirm checkout? [y/n]: " << RESET;
                std::string confirm = getInput("");
                if (confirm != "y" && confirm != "Y") break;

                // Process each cart item
                int orderId = orderCounter++;
                bool success = true;
                for (auto& item : cart.getItems()) {
                    Jersey* j = store.getInventory().getJersey(item.jerseyId);
                    if (!j || j->getStock() < item.quantity) {
                        printError("Stock issue with: " + item.jerseyName);
                        success = false; break;
                    }
                    j->reduceStock(item.quantity);

                    // Record in analytics
                    analytics.recordSale({
                        orderId,
                        auth.getCurrentUserId(),
                        auth.getCurrentName(),
                        item.jerseyId,
                        item.jerseyName,
                        item.team,
                        item.player,
                        item.sport,
                        item.quantity,
                        item.subtotal(),
                        "2024-01-01"
                    });
                    boughtIds.push_back(item.jerseyId);
                }

                if (success) {
                    std::string invoicePath = cart.generateInvoice(auth.getCurrentName(), orderId);
                    printSuccess("Order #" + std::to_string(orderId) + " placed!");
                    printSuccess("Invoice saved: " + invoicePath);
                    cart.clear();
                }
                break;
            }

            case 10: {
                auto jerseyMap = store.getInventory().getJerseyMap();
                analytics.showRecommendations(
                    auth.getCurrentUserId(), boughtIds, jerseyMap);
                break;
            }

            case 11: {
                if (fanId > 0) store.showFan(fanId);
                else {
                    printInfo("Name  : " + auth.getCurrentName());
                    printInfo("Email : " + auth.getCurrentUsername() + "@store.com");
                }
                break;
            }

            default: printError("Invalid choice.");
        }
    }
}

// ─────────────────────────────────────────────
//  Main
// ─────────────────────────────────────────────
int main() {
    Store store;
    std::cout << CLEAR;

    // Seed some analytics data so recommender works from start
    analytics.recordSale({2000, 101, "Tarun",  7, "Kohli #18 T20 WC 2024", "India", "Virat Kohli",    Sport::CRICKET,  1, 2999, "2024-01-01"});
    analytics.recordSale({2000, 101, "Tarun",  1, "Haaland #9 Home 2024/25","Man City","Haaland",       Sport::FOOTBALL, 1, 4749, "2024-01-01"});
    analytics.recordSale({2001, 102, "Arjun",  2, "Vinicius Jr #7 Away",   "Real Madrid","Vinicius Jr", Sport::FOOTBALL, 1, 4949, "2024-01-02"});
    analytics.recordSale({2001, 102, "Arjun",  8, "Dhoni #7 CSK IPL 2024", "CSK","MS Dhoni",           Sport::CRICKET,  1, 3499, "2024-01-02"});
    analytics.recordSale({2002, 103, "Priya",  4, "Salah #11 Home 2024/25","Liverpool","Salah",         Sport::FOOTBALL, 1, 4559, "2024-01-03"});
    analytics.recordSale({2002, 103, "Priya",  9, "Rohit #45 ODI WC 2023", "India","Rohit Sharma",     Sport::CRICKET,  1, 2799, "2024-01-03"});

    while (true) {
        showLoginMenu(store);
        if (auth.isAdmin())    adminMenu(store);
        else if (auth.isCustomer()) customerMenu(store);
    }
    return 0;
}
