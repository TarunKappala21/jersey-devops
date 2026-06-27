#include <iostream>
#include "Store.h"
#include "auth/Auth.h"
#include "analytics/Analytics.h"
#include "api/ApiServer.h"

int main(int argc, char* argv[]) {
    int port = 8080;
    if (argc > 1) port = std::stoi(argv[1]);

    Store       store;
    AuthManager auth;
    Analytics   analytics;

    // Seed analytics with sample data
    analytics.recordSale({2000,101,"Tarun",  7,"Kohli #18 T20 WC 2024",    "India",      "Virat Kohli",  Sport::CRICKET,  1,2999,"2024-01-01"});
    analytics.recordSale({2000,101,"Tarun",  1,"Haaland #9 Home 2024/25",  "Man City",   "Haaland",      Sport::FOOTBALL, 1,4749,"2024-01-01"});
    analytics.recordSale({2001,102,"Arjun",  2,"Vinicius Jr #7 Away",      "Real Madrid","Vinicius Jr",  Sport::FOOTBALL, 1,4949,"2024-01-02"});
    analytics.recordSale({2001,102,"Arjun",  8,"Dhoni #7 CSK IPL 2024",    "CSK",        "MS Dhoni",     Sport::CRICKET,  1,3499,"2024-01-02"});
    analytics.recordSale({2002,103,"Priya",  4,"Salah #11 Home 2024/25",   "Liverpool",  "Salah",        Sport::FOOTBALL, 1,4559,"2024-01-03"});
    analytics.recordSale({2002,103,"Priya",  9,"Rohit #45 ODI WC 2023",    "India",      "Rohit Sharma", Sport::CRICKET,  1,2799,"2024-01-03"});

    ApiServer server(store, auth, analytics);
    server.start(port);

    return 0;
}
