#pragma once
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>

// ─────────────────────────────────────────────
//  Minimal JSON builder — no external deps
//  Enough for our REST API responses
// ─────────────────────────────────────────────
namespace JSON {

inline std::string escape(const std::string& s) {
    std::string out;
    for (char c : s) {
        if      (c == '"')  out += "\\\"";
        else if (c == '\\') out += "\\\\";
        else if (c == '\n') out += "\\n";
        else if (c == '\r') out += "\\r";
        else if (c == '\t') out += "\\t";
        else                out += c;
    }
    return out;
}

inline std::string str(const std::string& v) {
    return "\"" + escape(v) + "\"";
}

inline std::string num(double v) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << v;
    return oss.str();
}

inline std::string num(int v)    { return std::to_string(v); }
inline std::string boolean(bool v) { return v ? "true" : "false"; }
inline std::string null_val()    { return "null"; }

// Build a JSON object from key-value pairs
// Usage: object({{"key", val}, {"key2", val2}})
inline std::string object(std::vector<std::pair<std::string,std::string>> fields) {
    std::string out = "{";
    for (int i = 0; i < (int)fields.size(); i++) {
        if (i) out += ",";
        out += "\"" + fields[i].first + "\":" + fields[i].second;
    }
    return out + "}";
}

// Build a JSON array from a list of already-serialized values
inline std::string array(const std::vector<std::string>& items) {
    std::string out = "[";
    for (int i = 0; i < (int)items.size(); i++) {
        if (i) out += ",";
        out += items[i];
    }
    return out + "]";
}

// Wrap in standard API response envelope
inline std::string ok(const std::string& data, const std::string& msg = "success") {
    return object({
        {"success", "true"},
        {"message", str(msg)},
        {"data",    data}
    });
}

inline std::string error(const std::string& msg) {
    return object({
        {"success", "false"},
        {"message", str(msg)},
        {"data",    "null"}
    });
}

} // namespace JSON
