#include "../include/Utils.h"
#include <iostream>
#include <fstream>
#include <random>

// Generate unique ID with prefix
std::string generateID(const std::string& prefix) {
    static int counter = 1000;
    std::ostringstream oss;
    oss << prefix << std::setfill('0') << std::setw(4) << counter++;
    return oss.str();
}

// Get current Unix timestamp
uint64_t currentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::seconds>(duration).count();
}

// Format timestamp for display
std::string formatTimestamp(uint64_t timestamp) {
    std::time_t time = static_cast<std::time_t>(timestamp);
    std::tm* tm_info = std::localtime(&time);
    
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm_info);
    return std::string(buffer);
}

// Split string by delimiter
std::vector<std::string> safeSplit(const std::string& s, char delim) {
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;
    
    while (std::getline(ss, item, delim)) {
        result.push_back(item);
    }
    
    return result;
}

// URL encode for safe serialization
std::string urlEncode(const std::string& value) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (char c : value) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~' || c == ' ') {
            escaped << c;
        } else {
            escaped << '%' << std::setw(2) << int((unsigned char)c);
        }
    }

    return escaped.str();
}

// URL decode
std::string urlDecode(const std::string& value) {
    std::string result;
    for (size_t i = 0; i < value.length(); ++i) {
        if (value[i] == '%' && i + 2 < value.length()) {
            std::string hex = value.substr(i + 1, 2);
            char ch = static_cast<char>(std::stoi(hex, nullptr, 16));
            result += ch;
            i += 2;
        } else {
            result += value[i];
        }
    }
    return result;
}

// Trim whitespace
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

// Simple logging function
void log(const std::string& level, const std::string& message) {
    std::ofstream logFile("data/logs.txt", std::ios::app);
    if (logFile.is_open()) {
        logFile << "[" << formatTimestamp(currentTimestamp()) << "] "
                << level << ": " << message << "\n";
        logFile.close();
    }
    
    // Also print to console for debugging
    std::cout << "[" << level << "] " << message << "\n";
}