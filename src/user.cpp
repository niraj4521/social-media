#include "../include/User.h"
#include "../include/Utils.h"
#include <sstream>
#include <iostream>

// Constructors
User::User() : userID(""), username(""), name(""), bio("") {}

User::User(const std::string& id, const std::string& uname) 
    : userID(id), username(uname), name(uname), bio("") {}

User::User(const std::string& id, const std::string& uname, const std::string& n, const std::string& b)
    : userID(id), username(uname), name(n), bio(b) {}

// Getters
std::string User::getUserID() const { return userID; }
std::string User::getUsername() const { return username; }
std::string User::getName() const { return name; }
std::string User::getBio() const { return bio; }
std::vector<std::string> User::getFollowers() const { return followers; }
std::vector<std::string> User::getFollowing() const { return following; }
int User::getFollowerCount() const { return followers.size(); }
int User::getFollowingCount() const { return following.size(); }

// Setters
void User::setName(const std::string& n) { name = n; }
void User::setBio(const std::string& b) { bio = b; }

// Follow Management
void User::addFollower(const std::string& followerID) {
    if (!hasFollower(followerID)) {
        followers.push_back(followerID);
    }
}

void User::removeFollower(const std::string& followerID) {
    auto it = std::find(followers.begin(), followers.end(), followerID);
    if (it != followers.end()) {
        followers.erase(it);
    }
}

void User::follow(const std::string& otherUserID) {
    if (!isFollowing(otherUserID) && otherUserID != userID) {
        following.push_back(otherUserID);
    }
}

void User::unfollow(const std::string& otherUserID) {
    auto it = std::find(following.begin(), following.end(), otherUserID);
    if (it != following.end()) {
        following.erase(it);
    }
}

bool User::isFollowing(const std::string& otherUserID) const {
    return std::find(following.begin(), following.end(), otherUserID) != following.end();
}

bool User::hasFollower(const std::string& followerID) const {
    return std::find(followers.begin(), followers.end(), followerID) != followers.end();
}

// Serialization: userID|username|name|bio|follower1,follower2|following1,following2
std::string User::serialize() const {
    std::string result = userID + "|" + username + "|" + 
                        urlEncode(name) + "|" + urlEncode(bio) + "|";
    
    // Serialize followers
    for (size_t i = 0; i < followers.size(); ++i) {
        result += followers[i];
        if (i < followers.size() - 1) result += ",";
    }
    result += "|";
    
    // Serialize following
    for (size_t i = 0; i < following.size(); ++i) {
        result += following[i];
        if (i < following.size() - 1) result += ",";
    }
    
    return result;
}

User User::deserialize(const std::string& line) {
    std::vector<std::string> parts = safeSplit(line, '|');
    
    if (parts.size() < 4) {
        throw std::runtime_error("Invalid user data format");
    }
    
    User u(parts[0], parts[1]);
    u.setName(urlDecode(parts[2]));
    u.setBio(urlDecode(parts[3]));
    
    // Deserialize followers
    if (parts.size() > 4 && !parts[4].empty()) {
        u.followers = safeSplit(parts[4], ',');
    }
    
    // Deserialize following
    if (parts.size() > 5 && !parts[5].empty()) {
        u.following = safeSplit(parts[5], ',');
    }
    
    return u;
}

void User::displayProfile() const {
    std::cout << "\n--- Profile ---\n";
    std::cout << "ID: " << userID << "\n";
    std::cout << "Username: @" << username << "\n";
    std::cout << "Name: " << name << "\n";
    std::cout << "Bio: " << bio << "\n";
    std::cout << "Followers: " << getFollowerCount() << "\n";
    std::cout << "Following: " << getFollowingCount() << "\n";
}