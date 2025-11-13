#ifndef SYSTEMCORE_H
#define SYSTEMCORE_H

#include "User.h"
#include "Post.h"
#include "Observer.h"
#include <unordered_map>
#include <mutex>
#include <memory>

class SystemCore {
private:
    // Singleton instance
    static SystemCore* instance;
    static std::mutex instanceMutex;

    // Data storage
    std::unordered_map<std::string, User> users;
    std::unordered_map<std::string, Post> posts;
    std::unordered_map<std::string, std::unique_ptr<PostNotifier>> userNotifiers;
    
    // Mutex for thread safety
    std::mutex coreMutex;

    // Private constructor for Singleton
    SystemCore();

    // Prevent copying
    SystemCore(const SystemCore&) = delete;
    SystemCore& operator=(const SystemCore&) = delete;

    // Unique ID trackers
    int nextUserID;
    int nextPostID;

    // Helpers
    
public:
    // Singleton access
    static SystemCore& getInstance();
    
    // Destructor
    ~SystemCore();
    
    // Unique ID generators
    std::string generateUserID();
    std::string generatePostID();
    
    // Data persistence
    void loadAllData();
    void saveAllData();
    
    void updateNextUserID();
    void updateNextPostID();
    // User management
    User* getUser(const std::string& userID);
    bool addUser(const User& u);
    bool userExists(const std::string& userID);
    bool usernameExists(const std::string& username);
    std::vector<User> getAllUsers();
    
    // Post management
    Post* getPost(const std::string& postID);
    bool addPost(const Post& p);
    std::vector<Post> getPostsByUser(const std::string& userID);
    std::vector<Post> getAllPosts();
    
    // Follow operations (bidirectional)
    bool followUser(const std::string& followerID, const std::string& followeeID);
    bool unfollowUser(const std::string& followerID, const std::string& followeeID);
    
    // Observer pattern for notifications
    void registerObserverForUser(const std::string& userID, IObserver* observer);
    void notifyFollowers(const std::string& userID, const Post& p);
    
    // Feed generation
    std::vector<Post> generateFeedForUser(const std::string& userID);
    
    // Statistics
    int getUserCount() const;
    int getPostCount() const;
    
    // Cleanup
    void clearAllData();
};

#endif // SYSTEMCORE_H
