#ifndef OBSERVER_H
#define OBSERVER_H

#include "Post.h"
#include <vector>

// Observer interface - objects that want to be notified of new posts
class IObserver {
public:
    virtual void onNotifyNewPost(const Post& p) = 0;
    virtual ~IObserver() = default;
};

// Subject interface - objects that notify observers
class ISubject {
public:
    virtual void registerObserver(IObserver* observer) = 0;
    virtual void removeObserver(IObserver* observer) = 0;
    virtual void notifyObservers(const Post& p) = 0;
    virtual ~ISubject() = default;
};

// Concrete Subject - Manages observers and notifications
class PostNotifier : public ISubject {
private:
    std::vector<IObserver*> observers;

public:
    void registerObserver(IObserver* observer) override {
        if (observer) {
            observers.push_back(observer);
        }
    }

    void removeObserver(IObserver* observer) override {
        auto it = std::find(observers.begin(), observers.end(), observer);
        if (it != observers.end()) {
            observers.erase(it);
        }
    }

    void notifyObservers(const Post& p) override {
        for (auto observer : observers) {
            observer->onNotifyNewPost(p);
        }
    }
};

// Concrete Observer - User as observer (can be notified)
class UserObserver : public IObserver {
private:
    std::string userID;
    int notificationCount;

public:
    UserObserver(const std::string& uid) : userID(uid), notificationCount(0) {}

    void onNotifyNewPost(const Post& p) override {
        notificationCount++;
        // In a real app, this would update a notification list
        // For now, we just track the count
    }

    int getNotificationCount() const {
        return notificationCount;
    }

    void clearNotifications() {
        notificationCount = 0;
    }
};

#endif // OBSERVER_H