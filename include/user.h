#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include <algorithm>

class User {
private:
    std::string userID;
    std::string username;
    std::string name;
    std::string bio;
    std::vector<std::string> followers;
    std::vector<std::string> following;

public:
    // Constructors
    User();
    User(const std::string& id, const std::string& uname);
    User(const std::string& id, const std::string& uname, const std::string& n, const std::string& b);

    // Getters
    std::string getUserID() const;
    std::string getUsername() const;
    std::string getName() const;
    std::string getBio() const;
    std::vector<std::string> getFollowers() const;
    std::vector<std::string> getFollowing() const;
    int getFollowerCount() const;
    int getFollowingCount() const;

    // Setters
    void setName(const std::string& n);
    void setBio(const std::string& b);

    // Follow/Follower Management
    void addFollower(const std::string& followerID);
    void removeFollower(const std::string& followerID);
    void follow(const std::string& otherUserID);
    void unfollow(const std::string& otherUserID);
    bool isFollowing(const std::string& otherUserID) const;
    bool hasFollower(const std::string& followerID) const;

    // Serialization for persistence
    std::string serialize() const;
    static User deserialize(const std::string& line);

    // Display
    void displayProfile() const;
};

#endif // USER_H