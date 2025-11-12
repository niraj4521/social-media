#ifndef POST_H
#define POST_H

#include <string>
#include <cstdint>

class Post {
private:
    std::string postID;
    std::string userID;
    std::string content;
    uint64_t timestamp;
    int likes;

public:
    // Constructors
    Post();
    Post(const std::string& pID, const std::string& uID, const std::string& cont, uint64_t ts);

    // Getters
    std::string getPostID() const;
    std::string getUserID() const;
    std::string getContent() const;
    uint64_t getTimestamp() const;
    int getLikes() const;

    // Setters & Actions
    void like();
    void editContent(const std::string& newContent);
    
    // Serialization
    std::string serialize() const;
    static Post deserialize(const std::string& line);
    
    // Display
    void display() const;
    
    // Comparison for sorting
    bool operator<(const Post& other) const; // By timestamp
};

#endif // POST_H