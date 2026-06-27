#pragma once
#include <string>

// ─────────────────────────────────────────────
//  ANSI Color codes for terminal UI
// ─────────────────────────────────────────────
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define CLEAR   "\033[2J\033[H"

// ─────────────────────────────────────────────
//  UI helper functions
// ─────────────────────────────────────────────
inline void printHeader(const std::string& title) {
    int width = 42;
    std::string border(width, '=');
    int padding = (width - (int)title.size()) / 2;
    std::string padded = std::string(padding, ' ') + title;
    std::cout << CYAN << BOLD
              << border << "\n"
              << padded << "\n"
              << border << "\n"
              << RESET;
}

inline void printSuccess(const std::string& msg) {
    std::cout << GREEN << "✅ " << msg << RESET << "\n";
}

inline void printError(const std::string& msg) {
    std::cout << RED << "❌ " << msg << RESET << "\n";
}

inline void printWarning(const std::string& msg) {
    std::cout << YELLOW << "⚠️  " << msg << RESET << "\n";
}

inline void printInfo(const std::string& msg) {
    std::cout << CYAN << "ℹ️  " << msg << RESET << "\n";
}

inline void printDivider() {
    std::cout << CYAN << std::string(42, '-') << RESET << "\n";
}
