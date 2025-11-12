#include "../include/Post.h"
#include "../include/Utils.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <ctime>

// Constructors
Post::Post() : postID(""), userID(""), content(""), timestamp(0), likes(0) {}

Post::Post(const std::string& pID, const std::string& uID, const std::string& cont, uint64_t ts)
    : postID(pID), userID(uID), content(cont), timestamp(ts), likes(0) {}

// Getters
std::string Post::getPostID() const { return postID; }
std::string Post::getUserID() const { return userID; }
std::string Post::getContent() const { return content; }
uint64_t Post::getTimestamp() const { return timestamp; }
int Post::getLikes() const { return likes; }

// Actions
void Post::like() {
    likes++;
}

void Post::editContent(const std::string& newContent) {
    if (!newContent.empty()) {
        content = newContent;
    }
}

// Serialization: postID|userID|timestamp|likes|content_encoded
std::string Post::serialize() const {
    std::ostringstream oss;
    oss << postID << "|" << userID << "|" << timestamp << "|" << likes << "|" << urlEncode(content);
    return oss.str();
}

Post Post::deserialize(const std::string& line) {
    std::vector<std::string> parts = safeSplit(line, '|');
    
    if (parts.size() < 5) {
        throw std::runtime_error("Invalid post data format");
    }
    
    Post p(parts[0], parts[1], urlDecode(parts[4]), std::stoull(parts[2]));
    p.likes = std::stoi(parts[3]);
    
    return p;
}

void Post::display() const {
    std::cout << "\n[@" << userID << "] - " << formatTimestamp(timestamp) << "\n";
    std::cout << content << "\n";
    std::cout << "❤️  " << likes << " likes\n";
}

bool Post::operator<(const Post& other) const {
    return timestamp < other.timestamp; // Older posts are "less than" newer
}