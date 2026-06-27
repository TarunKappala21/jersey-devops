#pragma once
#include "api/httplib.h"
#include "api/JsonBuilder.h"
#include "api/Serializer.h"
#include "Store.h"
#include "auth/Auth.h"
#include "cart/Cart.h"
#include "analytics/Analytics.h"
#include <map>
#include <iostream>

// ─────────────────────────────────────────────
//  ApiServer — wires all REST endpoints
//  Port: 8080
//
//  Endpoints:
//  POST /auth/login
//  POST /auth/register
//  GET  /jerseys
//  GET  /jerseys/football
//  GET  /jerseys/cricket
//  GET  /jerseys/search?q=
//  GET  /jerseys/filter?min=&max=
//  GET  /jerseys/:id
//  POST /jerseys          (admin)
//  PUT  /jerseys/:id/stock (admin)
//  DELETE /jerseys/:id    (admin)
//  GET  /cart/:fanId
//  POST /cart/:fanId/add
//  DELETE /cart/:fanId/remove/:jerseyId
//  POST /cart/:fanId/checkout
//  GET  /recommendations/:fanId
//  GET  /admin/analytics
//  GET  /admin/lowstock
//  GET  /health
// ─────────────────────────────────────────────
class ApiServer {
    httplib::Server   svr_;
    Store&            store_;
    AuthManager&      auth_;
    Analytics&        analytics_;

    // Per-session carts — fanId → Cart
    std::map<int, Cart> carts_;

    // Simple session tokens — token → username
    std::map<std::string, std::string> sessions_;

    // Sales records (for serializer access)
    std::vector<SalesRecord> salesRecords_;

    int orderCounter_ = 3001;

    // ── CORS headers ──────────────────────────
    void addCORS(httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin",  "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
    }

    void jsonResponse(httplib::Response& res,
                      const std::string& body, int status = 200) {
        addCORS(res);
        res.set_content(body, "application/json");
        res.status = status;
    }

    // ── Token helpers ─────────────────────────
    std::string generateToken(const std::string& username) {
        // Simple token for demo — use JWT in production
        std::string token = "tok_" + username + "_" + std::to_string(rand());
        sessions_[token] = username;
        return token;
    }

    bool isAdmin(const httplib::Request& req) {
        auto it = req.headers.find("Authorization");
        if (it == req.headers.end()) return false;
        std::string token = it->second;
        auto sit = sessions_.find(token);
        if (sit == sessions_.end()) return false;
        return sit->second == "admin";
    }

    bool isAuthenticated(const httplib::Request& req) {
        auto it = req.headers.find("Authorization");
        if (it == req.headers.end()) return false;
        return sessions_.count(it->second) > 0;
    }

    Cart& getCart(int fanId) {
        if (!carts_.count(fanId)) carts_.emplace(fanId, Cart(fanId));
        return carts_.at(fanId);
    }

    // ── Simple body parser ────────────────────
    // Parses "key=value&key2=value2" or JSON-like {"key":"value"}
    std::map<std::string,std::string> parseBody(const std::string& body) {
        std::map<std::string,std::string> params;
        // Try form-style first: key=value&key2=value2
        std::stringstream ss(body);
        std::string token;
        while (std::getline(ss, token, '&')) {
            auto pos = token.find('=');
            if (pos != std::string::npos)
                params[token.substr(0,pos)] = token.substr(pos+1);
        }
        // If empty, try crude JSON parse
        if (params.empty()) {
            auto extract = [&](const std::string& key) {
                std::string search = "\"" + key + "\"";
                auto p = body.find(search);
                if (p == std::string::npos) return;
                p = body.find(":", p + search.size());
                if (p == std::string::npos) return;
                p++;
                while (p < body.size() && (body[p]==' '||body[p]=='"')) p++;
                auto end = body.find_first_of("\",}", p);
                params[key] = body.substr(p, end - p);
            };
            for (auto& k : {"username","password","name","query",
                            "jerseyId","quantity","minPrice","maxPrice",
                            "fanId","sport","team","price","stock","season","league","tournament"})
                extract(k);
        }
        return params;
    }

public:
    ApiServer(Store& store, AuthManager& auth, Analytics& analytics)
        : store_(store), auth_(auth), analytics_(analytics) {
        setupRoutes();
    }

    void start(int port = 8080) {
        std::cout << GREEN << BOLD
                  << "\n🚀 Jersey Store API running on http://localhost:" << port
                  << RESET << "\n\n";
        printEndpoints();
        svr_.listen("0.0.0.0", port);
    }

    void printEndpoints() {
        std::cout << CYAN << "Available endpoints:\n" << RESET;
        std::vector<std::string> eps = {
            "POST /auth/login          — Login",
            "POST /auth/register       — Register",
            "GET  /jerseys             — All jerseys",
            "GET  /jerseys/football    — Football jerseys",
            "GET  /jerseys/cricket     — Cricket jerseys",
            "GET  /jerseys/search?q=   — Search",
            "GET  /jerseys/filter      — Filter by price ?min=&max=",
            "GET  /jerseys/:id         — Single jersey",
            "POST /jerseys             — Add jersey (admin)",
            "PUT  /jerseys/:id/stock   — Update stock (admin)",
            "DELETE /jerseys/:id       — Delete jersey (admin)",
            "GET  /cart/:fanId         — View cart",
            "POST /cart/:fanId/add     — Add to cart",
            "DELETE /cart/:fanId/:jId  — Remove from cart",
            "POST /cart/:fanId/checkout— Checkout",
            "GET  /recommend/:fanId    — Recommendations",
            "GET  /admin/analytics     — Sales dashboard (admin)",
            "GET  /admin/lowstock      — Low stock alerts (admin)",
            "GET  /health              — Health check",
        };
        for (auto& e : eps) std::cout << "  " << e << "\n";
        std::cout << "\n";
    }

    void addSalesRecord(const SalesRecord& r) {
        salesRecords_.push_back(r);
        analytics_.recordSale(r);
    }

private:
    void setupRoutes() {

        // ── OPTIONS preflight for CORS ─────────
        svr_.Options(".*", [this](const httplib::Request&, httplib::Response& res) {
            addCORS(res);
            res.status = 204;
        });

        // ── Health check ──────────────────────
        svr_.Get("/health", [this](const httplib::Request&, httplib::Response& res) {
            jsonResponse(res, JSON::ok(JSON::object({
                {"status",   JSON::str("ok")},
                {"jerseys",  JSON::num(store_.getInventory().size())},
                {"version",  JSON::str("1.0.0")}
            })));
        });

        // ─────────────────────────────────────
        //  AUTH
        // ─────────────────────────────────────

        // POST /auth/login
        svr_.Post("/auth/login", [this](const httplib::Request& req, httplib::Response& res) {
            auto p = parseBody(req.body);
            std::string username = p.count("username") ? p["username"] : "";
            std::string password = p.count("password") ? p["password"] : "";

            if (auth_.login(username, password)) {
                std::string token = generateToken(username);
                bool isAdm = (username == "admin");
                jsonResponse(res, JSON::ok(JSON::object({
                    {"token",    JSON::str(token)},
                    {"username", JSON::str(username)},
                    {"name",     JSON::str(auth_.getCurrentName())},
                    {"role",     JSON::str(isAdm ? "admin" : "customer")}
                })));
                auth_.logout(); // reset global state — token holds session
            } else {
                jsonResponse(res, JSON::error("Invalid credentials"), 401);
            }
        });

        // POST /auth/register
        svr_.Post("/auth/register", [this](const httplib::Request& req, httplib::Response& res) {
            auto p = parseBody(req.body);
            std::string username = p.count("username") ? p["username"] : "";
            std::string password = p.count("password") ? p["password"] : "";
            std::string name     = p.count("name")     ? p["name"]     : username;

            if (auth_.registerCustomer(username, password, name)) {
                std::string token = generateToken(username);
                jsonResponse(res, JSON::ok(JSON::object({
                    {"token",    JSON::str(token)},
                    {"username", JSON::str(username)},
                    {"name",     JSON::str(name)},
                    {"role",     JSON::str("customer")}
                }), "Registration successful"));
            } else {
                jsonResponse(res, JSON::error("Username already taken"), 400);
            }
        });

        // ─────────────────────────────────────
        //  JERSEYS
        // ─────────────────────────────────────

        // GET /jerseys
        svr_.Get("/jerseys", [this](const httplib::Request&, httplib::Response& res) {
            std::vector<Jersey*> all;
            auto jmap = store_.getInventory().getJerseyMap();
            for (auto& [id, j] : jmap) all.push_back(j);
            jsonResponse(res, JSON::ok(Serializer::jerseyList(all)));
        });

        // GET /jerseys/football
        svr_.Get("/jerseys/football", [this](const httplib::Request&, httplib::Response& res) {
            auto list = store_.getInventory().getBySport(Sport::FOOTBALL);
            jsonResponse(res, JSON::ok(Serializer::jerseyList(list)));
        });

        // GET /jerseys/cricket
        svr_.Get("/jerseys/cricket", [this](const httplib::Request&, httplib::Response& res) {
            auto list = store_.getInventory().getBySport(Sport::CRICKET);
            jsonResponse(res, JSON::ok(Serializer::jerseyList(list)));
        });

        // GET /jerseys/search?q=kohli
        svr_.Get("/jerseys/search", [this](const httplib::Request& req, httplib::Response& res) {
            std::string q = req.has_param("q") ? req.get_param_value("q") : "";
            if (q.empty()) {
                jsonResponse(res, JSON::error("Missing query param ?q="), 400);
                return;
            }
            auto list = store_.getInventory().search(q);
            jsonResponse(res, JSON::ok(Serializer::jerseyList(list)));
        });

        // GET /jerseys/filter?min=1000&max=5000
        svr_.Get("/jerseys/filter", [this](const httplib::Request& req, httplib::Response& res) {
            double minP = req.has_param("min") ? std::stod(req.get_param_value("min")) : 0;
            double maxP = req.has_param("max") ? std::stod(req.get_param_value("max")) : 99999;
            auto list = store_.getInventory().getByPriceRange(minP, maxP);
            jsonResponse(res, JSON::ok(Serializer::jerseyList(list)));
        });

        // GET /jerseys/:id
        svr_.Get(R"(/jerseys/(\d+))", [this](const httplib::Request& req, httplib::Response& res) {
            int id = std::stoi(req.matches[1]);
            Jersey* j = store_.getInventory().getJersey(id);
            if (!j) { jsonResponse(res, JSON::error("Jersey not found"), 404); return; }
            jsonResponse(res, JSON::ok(Serializer::jersey(j)));
        });

        // POST /jerseys (admin)
        svr_.Post("/jerseys", [this](const httplib::Request& req, httplib::Response& res) {
            if (!isAdmin(req)) {
                jsonResponse(res, JSON::error("Unauthorized"), 401); return;
            }
            auto p = parseBody(req.body);
            try {
                int    id     = p.count("id")     ? std::stoi(p["id"])         : rand() % 1000 + 100;
                double price  = p.count("price")  ? std::stod(p["price"])      : 0;
                int    stock  = p.count("stock")  ? std::stoi(p["stock"])      : 0;
                std::string sport = p.count("sport") ? p["sport"] : "football";

                if (sport == "cricket" || sport == "Cricket") {
                    store_.getInventory().addJersey(std::make_unique<CricketJersey>(
                        id, p["name"], p["player"], p["team"], price, stock,
                        p.count("season") ? p["season"] : "2024",
                        SizeType::M,
                        p.count("tournament") ? p["tournament"] : "IPL",
                        CricketFormat::T20));
                } else {
                    store_.getInventory().addJersey(std::make_unique<FootballJersey>(
                        id, p["name"], p["player"], p["team"], price, stock,
                        p.count("season") ? p["season"] : "2024/25",
                        SizeType::M,
                        p.count("league") ? p["league"] : "EPL",
                        KitType::HOME));
                }
                jsonResponse(res, JSON::ok(JSON::str("Jersey added"), "Jersey added successfully"));
            } catch (std::exception& e) {
                jsonResponse(res, JSON::error(e.what()), 400);
            }
        });

        // PUT /jerseys/:id/stock (admin)
        svr_.Put(R"(/jerseys/(\d+)/stock)", [this](const httplib::Request& req, httplib::Response& res) {
            if (!isAdmin(req)) {
                jsonResponse(res, JSON::error("Unauthorized"), 401); return;
            }
            int id = std::stoi(req.matches[1]);
            auto p = parseBody(req.body);
            int qty = p.count("stock") ? std::stoi(p["stock"]) : 0;
            Jersey* j = store_.getInventory().getJersey(id);
            if (!j) { jsonResponse(res, JSON::error("Jersey not found"), 404); return; }
            store_.getInventory().updateStock(id, qty);
            jsonResponse(res, JSON::ok(JSON::str("Stock updated")));
        });

        // DELETE /jerseys/:id (admin)
        svr_.Delete(R"(/jerseys/(\d+))", [this](const httplib::Request& req, httplib::Response& res) {
            if (!isAdmin(req)) {
                jsonResponse(res, JSON::error("Unauthorized"), 401); return;
            }
            int id = std::stoi(req.matches[1]);
            if (store_.getInventory().removeJersey(id))
                jsonResponse(res, JSON::ok(JSON::str("deleted")));
            else
                jsonResponse(res, JSON::error("Jersey not found"), 404);
        });

        // ─────────────────────────────────────
        //  CART
        // ─────────────────────────────────────

        // GET /cart/:fanId
        svr_.Get(R"(/cart/(\d+))", [this](const httplib::Request& req, httplib::Response& res) {
            int fanId = std::stoi(req.matches[1]);
            jsonResponse(res, JSON::ok(Serializer::cart(getCart(fanId))));
        });

        // POST /cart/:fanId/add   body: jerseyId=1&quantity=2
        svr_.Post(R"(/cart/(\d+)/add)", [this](const httplib::Request& req, httplib::Response& res) {
            int fanId = std::stoi(req.matches[1]);
            auto p    = parseBody(req.body);
            int jerseyId = p.count("jerseyId") ? std::stoi(p["jerseyId"]) : -1;
            int qty      = p.count("quantity") ? std::stoi(p["quantity"]) : 1;

            Jersey* j = store_.getInventory().getJersey(jerseyId);
            if (!j) { jsonResponse(res, JSON::error("Jersey not found"), 404); return; }

            Cart& cart = getCart(fanId);
            if (cart.addItem(j, qty))
                jsonResponse(res, JSON::ok(Serializer::cart(cart), "Item added to cart"));
            else
                jsonResponse(res, JSON::error("Could not add item"), 400);
        });

        // DELETE /cart/:fanId/:jerseyId
        svr_.Delete(R"(/cart/(\d+)/(\d+))", [this](const httplib::Request& req, httplib::Response& res) {
            int fanId    = std::stoi(req.matches[1]);
            int jerseyId = std::stoi(req.matches[2]);
            Cart& cart = getCart(fanId);
            if (cart.removeItem(jerseyId))
                jsonResponse(res, JSON::ok(Serializer::cart(cart), "Item removed"));
            else
                jsonResponse(res, JSON::error("Item not in cart"), 404);
        });

        // POST /cart/:fanId/checkout
        svr_.Post(R"(/cart/(\d+)/checkout)", [this](const httplib::Request& req, httplib::Response& res) {
            int fanId  = std::stoi(req.matches[1]);
            Cart& cart = getCart(fanId);

            if (cart.isEmpty()) {
                jsonResponse(res, JSON::error("Cart is empty"), 400); return;
            }

            int orderId = orderCounter_++;
            bool success = true;

            for (auto& item : cart.getItems()) {
                Jersey* j = store_.getInventory().getJersey(item.jerseyId);
                if (!j || j->getStock() < item.quantity) {
                    jsonResponse(res, JSON::error("Insufficient stock for: " + item.jerseyName), 400);
                    success = false; break;
                }
                j->reduceStock(item.quantity);
                addSalesRecord({
                    orderId, fanId, "Fan_" + std::to_string(fanId),
                    item.jerseyId, item.jerseyName, item.team, item.player,
                    item.sport, item.quantity, item.subtotal(), "2024-01-01"
                });
            }

            if (success) {
                // Generate invoice
                std::string invoicePath = cart.generateInvoice(
                    "Fan_" + std::to_string(fanId), orderId);

                double grandTotal = cart.getGrandTotal();
                cart.clear();

                jsonResponse(res, JSON::ok(JSON::object({
                    {"orderId",      JSON::num(orderId)},
                    {"grandTotal",   JSON::num(grandTotal)},
                    {"invoicePath",  JSON::str(invoicePath)},
                    {"message",      JSON::str("Order placed successfully!")}
                }), "Checkout successful"));
            }
        });

        // ─────────────────────────────────────
        //  RECOMMENDATIONS
        // ─────────────────────────────────────

        // GET /recommend/:fanId
        svr_.Get(R"(/recommend/(\d+))", [this](const httplib::Request& req, httplib::Response& res) {
            int fanId = std::stoi(req.matches[1]);

            // Get what this fan has already bought
            std::vector<int> boughtIds;
            for (auto& r : salesRecords_)
                if (r.fanId == fanId) boughtIds.push_back(r.jerseyId);

            auto jmap = store_.getInventory().getJerseyMap();
            auto recIds = analytics_.recommend(fanId, boughtIds);

            std::vector<Jersey*> recJerseys;
            for (int id : recIds) {
                auto it = jmap.find(id);
                if (it != jmap.end()) recJerseys.push_back(it->second);
            }

            jsonResponse(res, JSON::ok(Serializer::jerseyList(recJerseys)));
        });

        // ─────────────────────────────────────
        //  ADMIN
        // ─────────────────────────────────────

        // GET /admin/analytics
        svr_.Get("/admin/analytics", [this](const httplib::Request& req, httplib::Response& res) {
            if (!isAdmin(req)) {
                jsonResponse(res, JSON::error("Unauthorized"), 401); return;
            }
            jsonResponse(res, JSON::ok(
                Serializer::analyticsOverview(analytics_, salesRecords_)));
        });

        // GET /admin/lowstock
        svr_.Get("/admin/lowstock", [this](const httplib::Request& req, httplib::Response& res) {
            if (!isAdmin(req)) {
                jsonResponse(res, JSON::error("Unauthorized"), 401); return;
            }
            int threshold = req.has_param("threshold")
                          ? std::stoi(req.get_param_value("threshold")) : 5;
            auto list = store_.getInventory().getLowStock(threshold);
            jsonResponse(res, JSON::ok(Serializer::jerseyList(list)));
        });

    } // setupRoutes
};
