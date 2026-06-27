#include "customer/Fan.h"
#include <fstream>

void Fan::exportToJSON(std::ofstream& f) const {
    f << "  {\n"
      << "    \"id\": "              << id_              << ",\n"
      << "    \"name\": \""          << name_            << "\",\n"
      << "    \"email\": \""         << email_           << "\",\n"
      << "    \"fav_football_club\": \"" << favFootballClub_ << "\",\n"
      << "    \"fav_cricket_team\": \""  << favCricketTeam_  << "\",\n"
      << "    \"total_spent\": "     << totalSpent_      << ",\n"
      << "    \"order_count\": "     << purchaseHistory_.size() << ",\n"
      << "    \"fav_players\": [";
    for (int i = 0; i < (int)favPlayers_.size(); i++) {
        if (i > 0) f << ", ";
        f << "\"" << favPlayers_[i] << "\"";
    }
    f << "]\n  }";
}
