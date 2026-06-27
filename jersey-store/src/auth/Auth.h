#pragma once
#include "common/Colors.h"
#include <string>
#include <map>
#include <iostream>

enum class UserRole { ADMIN, CUSTOMER, NONE };

struct UserAccount {
    int         id;
    std::string username;
    std::string password;   // plaintext for now (hash in production)
    std::string name;
    UserRole    role;
};

// ─────────────────────────────────────────────
//  AuthManager — handles login/logout
// ─────────────────────────────────────────────
class AuthManager {
    std::map<std::string, UserAccount> users_;
    UserAccount* currentUser_ = nullptr;
    int nextId_ = 100;

public:
    AuthManager() {
        // Seed default accounts
        addUser("admin",   "admin123",  "Store Admin",  UserRole::ADMIN);
        addUser("tarun",   "tarun123",  "Tarun",        UserRole::CUSTOMER);
        addUser("arjun",   "arjun123",  "Arjun",        UserRole::CUSTOMER);
        addUser("priya",   "priya123",  "Priya",        UserRole::CUSTOMER);
    }

    void addUser(const std::string& username, const std::string& password,
                 const std::string& name, UserRole role) {
        users_[username] = { nextId_++, username, password, name, role };
    }

    bool registerCustomer(const std::string& username,
                          const std::string& password,
                          const std::string& name) {
        if (users_.count(username)) {
            printError("Username already taken!");
            return false;
        }
        addUser(username, password, name, UserRole::CUSTOMER);
        printSuccess("Account created! Please login.");
        return true;
    }

    bool login(const std::string& username, const std::string& password) {
        auto it = users_.find(username);
        if (it == users_.end() || it->second.password != password) {
            printError("Invalid username or password!");
            return false;
        }
        currentUser_ = &it->second;
        printSuccess("Welcome back, " + currentUser_->name + "!");
        return true;
    }

    void logout() {
        if (currentUser_) {
            printInfo("Goodbye, " + currentUser_->name + "!");
            currentUser_ = nullptr;
        }
    }

    bool isLoggedIn()  const { return currentUser_ != nullptr; }
    bool isAdmin()     const { return currentUser_ && currentUser_->role == UserRole::ADMIN; }
    bool isCustomer()  const { return currentUser_ && currentUser_->role == UserRole::CUSTOMER; }

    UserAccount* getCurrentUser() { return currentUser_; }

    std::string getCurrentUsername() const {
        return currentUser_ ? currentUser_->username : "";
    }

    int getCurrentUserId() const {
        return currentUser_ ? currentUser_->id : -1;
    }

    std::string getCurrentName() const {
        return currentUser_ ? currentUser_->name : "";
    }

    // Map username to fan ID (1-based index for Fan lookup)
    int getFanId() const {
        if (!currentUser_) return -1;
        // tarun=1, arjun=2, priya=3 (matches loadSampleFans order)
        std::map<std::string,int> fanMap = {{"tarun",1},{"arjun",2},{"priya",3}};
        auto it = fanMap.find(currentUser_->username);
        return it != fanMap.end() ? it->second : -1;
    }
};
