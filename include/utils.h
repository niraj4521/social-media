#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <cstdint>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <chrono>

// ID Generation
std::string generateID(const std::string& prefix);

// Timestamp utilities
uint64_t currentTimestamp();
std::string formatTimestamp(uint64_t timestamp);

// String utilities
std::vector<std::string> safeSplit(const std::string& s, char delim);
std::string urlEncode(const std::string& value);
std::string urlDecode(const std::string& value);
std::string trim(const std::string& str);

// Logging
void log(const std::string& level, const std::string& message);

#endif // UTILS_H