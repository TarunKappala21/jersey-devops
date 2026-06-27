#include "inventory/Inventory.h"
#include <fstream>
#include <iostream>
#include <filesystem>

void Inventory::exportToJSON(const std::string& filepath) const {
    std::filesystem::create_directories("data");
    std::ofstream f(filepath);
    if (!f.is_open()) {
        std::cerr << "Warning: could not open " << filepath << "\n";
        return;
    }

    f << "[\n";
    bool first = true;
    for (auto& [id, j] : jerseys_) {
        if (!first) f << ",\n";
        first = false;
        f << "  {\n"
          << "    \"id\": "       << j->getId()              << ",\n"
          << "    \"name\": \""   << j->getName()            << "\",\n"
          << "    \"player\": \"" << j->getPlayer()          << "\",\n"
          << "    \"team\": \""   << j->getTeam()            << "\",\n"
          << "    \"sport\": \""  << sportStr(j->getSport()) << "\",\n"
          << "    \"price\": "    << j->getPrice()           << ",\n"
          << "    \"stock\": "    << j->getStock()           << ",\n"
          << "    \"season\": \"" << j->getSeason()          << "\",\n"
          << "    \"size\": \""   << sizeStr(j->getSize())   << "\",\n"
          << "    \"discount\": " << j->getDiscount()        << ",\n"
          << "    \"details\": \" "<< j->getDetails()        << "\",\n"
          << "    \"limited\": "  << (j->isLimited() ? "true" : "false") << "\n"
          << "  }";
    }
    f << "\n]\n";
    std::cout << "Inventory exported to " << filepath << "\n";
}
