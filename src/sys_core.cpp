#include "sys_core.h"
#include "../include/Utils.h"
#include <fstream>
#include <iostream>
#include <algorithm>

// Initialize static members
SystemCore* SystemCore::instance = nullptr;
std::mutex SystemCore::instanceMutex;

SystemCore::SystemCore() {
    log("INFO", "SystemCore initialized");
}

SystemCore::~SystemCore() {
    log("INFO", "SystemCore destroyed");
}

SystemCore& SystemCore::getInstance() {
    std::lock_guard<std::mutex> lock(instanceMutex);
    if (!instance) {
        instance = new SystemCore();
    }
    return *instance;
}

// Load all data from files
void SystemCore::loadAllData() {
    std::lock_guard<std::mutex> lock(coreMutex);
    
    // Load users
    std::ifstream userFile("data/users.txt");
    if (userFile.is_open()) {
        std::string line;
        int count = 0;
        while (std::getline(userFile, line)) {
            if (line.empty()) continue;
            try {
                User u = User::deserialize(line);
                users[u.getUserID()] = u;
                count++;
            } catch (const std::exception& e) {
                log("ERROR", "Failed to deserialize user: " + std::string(e.what()));
            }
        }
        userFile.close();
        log("INFO", "Loaded " + std::to_string(count) + " users");
    } else {
        log("WARNING", "users.txt not found, starting fresh");
    }
    
    // Load posts
    std::ifstream postFile("data/posts.txt");
    if (postFile.is_open()) {
        std::string line;
        int count = 0;
        while (std::getline(postFile, line)) {
            if (line.empty()) continue;
            try {
                Post p = Post::deserialize(line);
                posts[p.getPostID()] = p;
                count++;
            } catch (const std::exception& e) {
                log("ERROR", "Failed to deserialize post: " + std::string(e.what()));
            }
        }
        postFile.close();
        log("INFO", "Loaded " + std::to_string(count) + " posts");
    } else {
        log("WARNING", "posts.txt not found, starting fresh");
    }
}

// Save all data to files
void SystemCore::saveAllData() {
    std::lock_guard<std::mutex> lock(coreMutex);
    
    // Save users
    std::ofstream userFile("data/users.txt");
    if (userFile.is_open()) {
        for (const auto& pair : users) {
            userFile << pair.second.serialize() << "\n";
        }
        userFile.close();
        log("INFO", "Saved " + std::to_string(users.size()) + " users");
    } else {
        log("ERROR", "Failed to open users.txt for writing");
    }
    
    // Save posts
    std::ofstream postFile("data/posts.txt");
    if (postFile.is_open()) {
        for (const auto& pair : posts) {
            postFile << pair.second.serialize() << "\n";
        }
        postFile.close();
        log("INFO", "Saved " + std::to_string(posts.size()) + " posts");
    } else {
        log("ERROR", "Failed to open posts.txt for writing");
    }
}

// User management
User* SystemCore::getUser(const std::string& userID) {
    auto it = users.find(userID);
    return (it != users.end()) ? &(it->second) : nullptr;
}

bool SystemCore::addUser(const User& u) {
    std::lock_guard<std::mutex> lock(coreMutex);
    if (users.find(u.getUserID()) != users.end()) {
        log("WARNING", "User already exists: " + u.getUserID());
        return false;
    }
    users[u.getUserID()] = u;
    userNotifiers[u.getUserID()] = std::make_unique<PostNotifier>();
    log("INFO", "User added: " + u.getUserID());
    return true;
}

bool SystemCore::userExists(const std::string& userID) {
    return users.find(userID) != users.end();
}

bool SystemCore::usernameExists(const std::string& username) {
    for (const auto& pair : users) {
        if (pair.second.getUsername() == username) {
            return true;
        }
    }
    return false;
}

std::vector<User> SystemCore::getAllUsers() {
    std::vector<User> result;
    for (const auto& pair : users) {
        result.push_back(pair.second);
    }
    return result;
}

// Post management
Post* SystemCore::getPost(const std::string& postID) {
    auto it = posts.find(postID);
    return (it != posts.end()) ? &(it->second) : nullptr;
}

bool SystemCore::addPost(const Post& p) {
    std::lock_guard<std::mutex> lock(coreMutex);
    if (posts.find(p.getPostID()) != posts.end()) {
        log("WARNING", "Post already exists: " + p.getPostID());
        return false;
    }
    posts[p.getPostID()] = p;
    log("INFO", "Post added: " + p.getPostID());
    
    // Notify followers
    notifyFollowers(p.getUserID(), p);
    
    return true;
}

std::vector<Post> SystemCore::getPostsByUser(const std::string& userID) {
    std::vector<Post> result;
    for (const auto& pair : posts) {
        if (pair.second.getUserID() == userID) {
            result.push_back(pair.second);
        }
    }
    return result;
}

std::vector<Post> SystemCore::getAllPosts() {
    std::vector<Post> result;
    for (const auto& pair : posts) {
        result.push_back(pair.second);
    }
    return result;
}

// Follow operations
bool SystemCore::followUser(const std::string& followerID, const std::string& followeeID) {
    std::lock_guard<std::mutex> lock(coreMutex);
    
    User* follower = getUser(followerID);
    User* followee = getUser(followeeID);
    
    if (!follower || !followee) {
        log("ERROR", "User not found in follow operation");
        return false;
    }
    
    follower->follow(followeeID);
    followee->addFollower(followerID);
    
    log("INFO", followerID + " followed " + followeeID);
    return true;
}

bool SystemCore::unfollowUser(const std::string& followerID, const std::string& followeeID) {
    std::lock_guard<std::mutex> lock(coreMutex);
    
    User* follower = getUser(followerID);
    User* followee = getUser(followeeID);
    
    if (!follower || !followee) {
        log("ERROR", "User not found in unfollow operation");
        return false;
    }
    
    follower->unfollow(followeeID);
    followee->removeFollower(followerID);
    
    log("INFO", followerID + " unfollowed " + followeeID);
    return true;
}

// Observer pattern
void SystemCore::registerObserverForUser(const std::string& userID, IObserver* observer) {
    if (userNotifiers.find(userID) != userNotifiers.end()) {
        userNotifiers[userID]->registerObserver(observer);
    }
}

void SystemCore::notifyFollowers(const std::string& userID, const Post& p) {
    if (userNotifiers.find(userID) != userNotifiers.end()) {
        userNotifiers[userID]->notifyObservers(p);
    }
}

// Feed generation
std::vector<Post> SystemCore::generateFeedForUser(const std::string& userID) {
    std::vector<Post> feed;
    
    User* user = getUser(userID);
    if (!user) return feed;
    
    std::vector<std::string> following = user->getFollowing();
    
    // Collect posts from all followed users
    for (const std::string& followedID : following) {
        std::vector<Post> userPosts = getPostsByUser(followedID);
        feed.insert(feed.end(), userPosts.begin(), userPosts.end());
    }
    
    // Sort by timestamp (newest first)
    std::sort(feed.begin(), feed.end(), 
        [](const Post& a, const Post& b) { return a.getTimestamp() > b.getTimestamp(); });
    
    return feed;
}

// Statistics
int SystemCore::getUserCount() const {
    return users.size();
}

int SystemCore::getPostCount() const {
    return posts.size();
}

// Cleanup
void SystemCore::clearAllData() {
    std::lock_guard<std::mutex> lock(coreMutex);
    users.clear();
    posts.clear();
    userNotifiers.clear();
    log("INFO", "All data cleared");
}