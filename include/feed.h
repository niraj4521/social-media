#ifndef FEED_H
#define FEED_H

#include "Post.h"
#include <vector>
#include <algorithm>
#include <iostream>

// Abstract Feed Interface
struct IFeed {
    virtual void addPost(const Post& p) = 0;
    virtual std::vector<Post> getFeed() const = 0;
    virtual void sortByTimestamp(bool desc = true) = 0;
    virtual void sortByLikes(bool desc = true) = 0;
    virtual void clear() = 0;
    virtual ~IFeed() = default;
};

// Template-based Feed Implementation
template<typename T>
class TextFeed : public IFeed {
private:
    std::vector<T> posts;

public:
    // Add post to feed
    void addPost(const T& p) override {
        posts.push_back(p);
    }

    // Get all posts
    std::vector<Post> getFeed() const override {
        std::vector<Post> result;
        for (const auto& p : posts) {
            result.push_back(p);
        }
        return result;
    }

    // Sort by timestamp (newest first by default)
    void sortByTimestamp(bool desc = true) override {
        if (desc) {
            std::sort(posts.begin(), posts.end(), 
                [](const T& a, const T& b) { return a.getTimestamp() > b.getTimestamp(); });
        } else {
            std::sort(posts.begin(), posts.end(), 
                [](const T& a, const T& b) { return a.getTimestamp() < b.getTimestamp(); });
        }
    }

    // Sort by likes (most liked first by default)
    void sortByLikes(bool desc = true) override {
        if (desc) {
            std::sort(posts.begin(), posts.end(), 
                [](const T& a, const T& b) { return a.getLikes() > b.getLikes(); });
        } else {
            std::sort(posts.begin(), posts.end(), 
                [](const T& a, const T& b) { return a.getLikes() < b.getLikes(); });
        }
    }

    // Clear feed
    void clear() override {
        posts.clear();
    }

    // Display feed
    void displayFeed() const {
        if (posts.empty()) {
            std::cout << "\n📭 Your feed is empty. Follow some users to see their posts!\n";
            return;
        }

        std::cout << "\n═══════════════ FEED ═══════════════\n";
        for (const auto& post : posts) {
            post.display();
            std::cout << "────────────────────────────────────\n";
        }
    }

    // Get post count
    size_t size() const {
        return posts.size();
    }
};

#endif // FEED_H