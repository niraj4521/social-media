#include "sys_core.h"
#include "feed.h"
#include "utils.h"
#include <iostream>
#include <limits>
#include <clocale>

// Current logged-in user
std::string currentUserID = "";

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Wait for Enter reliably
void pause() {
    std::cout << "\nPress Enter to continue...";
    std::string tmp;
    std::getline(std::cin, tmp);
}

void displayBanner() {
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════╗\n";
    std::cout << "║    SOCIAL MEDIA FEED ENGINE               ║\n";
    std::cout << "║   Built with C++ OOP by Team of 4         ║\n";
    std::cout << "╚═══════════════════════════════════════════╝\n";
}

bool signup() {
    std::string username, name, bio;

    std::cout << "\n--- Sign Up ---\n";
    std::cout << "Enter username: ";
    std::cin >> username;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    SystemCore& core = SystemCore::getInstance();

    if (core.usernameExists(username)) {
        std::cout << " Username already exists!\n";
        return false;
    }

    std::cout << "Enter full name: ";
    std::getline(std::cin, name);

    std::cout << "Enter bio: ";
    std::getline(std::cin, bio);

    // Use SystemCore's generator (do not call undefined generateID)
    std::string userID = core.generateUserID();
    User newUser(userID, username, name, bio);

    if (core.addUser(newUser)) {
        // persist immediately
        core.saveAllData();
        std::cout << "Account created successfully! Your ID: " << userID << "\n";
        return true;
    }

    std::cout << " Failed to create account.\n";
    return false;
}

bool login() {
    std::string username;

    std::cout << "\n--- Login ---\n";
    std::cout << "Enter username: ";
    std::cin >> username;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    SystemCore& core = SystemCore::getInstance();
    std::vector<User> allUsers = core.getAllUsers();

    for (const User& u : allUsers) {
        if (u.getUsername() == username) {
            currentUserID = u.getUserID();
            std::cout << " Welcome back, " << u.getName() << "!\n";
            return true;
        }
    }

    std::cout << " User not found!\n";
    return false;
}

void createPost() {
    if (currentUserID.empty()) {
        std::cout << " You must be logged in to post.\n";
        return;
    }

    std::string content;
    std::cout << "\n--- Create Post ---\n";
    std::cout << "What's on your mind? ";
    std::getline(std::cin, content);

    if (content.empty()) {
        std::cout << " Post cannot be empty.\n";
        return;
    }

    SystemCore& core = SystemCore::getInstance();

    // generate unique post id from SystemCore
    Post newPost(core.generatePostID(), currentUserID, content, currentTimestamp());

    if (core.addPost(newPost)) {
        // persist immediately
        core.saveAllData();
        std::cout << " Post created successfully!\n";
    } else {
        std::cout << " Failed to create post.\n";
    }
}

void viewMyPosts() {
    if (currentUserID.empty()) {
        std::cout << " You must be logged in.\n";
        return;
    }

    SystemCore& core = SystemCore::getInstance();
    std::vector<Post> myPosts = core.getPostsByUser(currentUserID);

    if (myPosts.empty()) {
        std::cout << "\n You haven't posted anything yet.\n";
        return;
    }

    std::cout << "\n═══════════════ MY POSTS ═══════════════\n";
    for (const Post& p : myPosts) {
        p.display();
        std::cout << "────────────────────────────────────\n";
    }
}

void followUser() {
    if (currentUserID.empty()) {
        std::cout << " You must be logged in.\n";
        return;
    }

    SystemCore& core = SystemCore::getInstance();
    std::vector<User> allUsers = core.getAllUsers();

    std::cout << "\n--- Available Users ---\n";
    int index = 1;
    for (const User& u : allUsers) {
        if (u.getUserID() != currentUserID) {
            std::cout << index++ << ". @" << u.getUsername()
                      << " - " << u.getName() << "\n";
        }
    }

    if (index == 1) {
        std::cout << " No other users found.\n";
        return;
    }

    std::string username;
    std::cout << "\nEnter username to follow: ";
    std::cin >> username;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    for (const User& u : allUsers) {
        if (u.getUsername() == username) {
            if (core.followUser(currentUserID, u.getUserID())) {
                core.saveAllData();
                std::cout << " You are now following @" << username << "\n";
            } else {
                std::cout << " Failed to follow user.\n";
            }
            return;
        }
    }

    std::cout << " User not found.\n";
}

void unfollowUser() {
    if (currentUserID.empty()) {
        std::cout << " You must be logged in.\n";
        return;
    }

    SystemCore& core = SystemCore::getInstance();
    User* currentUser = core.getUser(currentUserID);

    if (!currentUser) return;

    std::vector<std::string> following = currentUser->getFollowing();

    if (following.empty()) {
        std::cout << "\n You are not following anyone.\n";
        return;
    }

    std::cout << "\n--- Following ---\n";
    int index = 1;
    for (const std::string& uid : following) {
        User* u = core.getUser(uid);
        if (u) {
            std::cout << index++ << ". @" << u->getUsername() << "\n";
        }
    }

    std::string username;
    std::cout << "\nEnter username to unfollow: ";
    std::cin >> username;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    for (const std::string& uid : following) {
        User* u = core.getUser(uid);
        if (u && u->getUsername() == username) {
            if (core.unfollowUser(currentUserID, uid)) {
                core.saveAllData();
                std::cout << " You unfollowed @" << username << "\n";
            } else {
                std::cout << " Failed to unfollow user.\n";
            }
            return;
        }
    }

    std::cout << " User not found in your following list.\n";
}

void viewFeed() {
    if (currentUserID.empty()) {
        std::cout << " You must be logged in.\n";
        return;
    }

    SystemCore& core = SystemCore::getInstance();
    std::vector<Post> feedPosts = core.generateFeedForUser(currentUserID);

    TextFeed<Post> feed;
    for (const Post& p : feedPosts) {
        feed.addPost(p);
    }

    feed.sortByTimestamp(true); // Newest first
    feed.displayFeed();
}

void viewProfile() {
    if (currentUserID.empty()) {
        std::cout << " You must be logged in.\n";
        return;
    }

    SystemCore& core = SystemCore::getInstance();
    User* user = core.getUser(currentUserID);

    if (user) {
        user->displayProfile();
    }
}

void editProfile() {
    if (currentUserID.empty()) {
        std::cout << " You must be logged in.\n";
        return;
    }

    SystemCore& core = SystemCore::getInstance();
    User* user = core.getUser(currentUserID);

    if (!user) return;

    std::cout << "\n--- Edit Profile ---\n";
    std::cout << "1. Change Name\n";
    std::cout << "2. Change Bio\n";
    std::cout << "Enter choice: ";

    int choice;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (choice == 1) {
        std::string newName;
        std::cout << "Enter new name: ";
        std::getline(std::cin, newName);
        user->setName(newName);
        core.saveAllData();
        std::cout << " Name updated!\n";
    } else if (choice == 2) {
        std::string newBio;
        std::cout << "Enter new bio: ";
        std::getline(std::cin, newBio);
        user->setBio(newBio);
        core.saveAllData();
        std::cout << " Bio updated!\n";
    }
}

void likePost() {
    if (currentUserID.empty()) {
        std::cout << " You must be logged in.\n";
        return;
    }

    SystemCore& core = SystemCore::getInstance();
    std::vector<Post> feedPosts = core.generateFeedForUser(currentUserID);

    if (feedPosts.empty()) {
        std::cout << "\n No posts in your feed.\n";
        return;
    }

    std::cout << "\n--- Posts in Feed ---\n";
    for (size_t i = 0; i < feedPosts.size(); ++i) {
        std::cout << "\n[" << (i + 1) << "]\n";
        feedPosts[i].display();
    }

    std::cout << "\nEnter post number to like: ";
    int choice;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (choice > 0 && choice <= static_cast<int>(feedPosts.size())) {
        Post* post = core.getPost(feedPosts[choice - 1].getPostID());
        if (post) {
            post->like();
            core.saveAllData();
            std::cout << " Post liked!\n";
        } else {
            std::cout << " Post not found.\n";
        }
    } else {
        std::cout << " Invalid choice.\n";
    }
}

void showStatistics() {
    SystemCore& core = SystemCore::getInstance();

    std::cout << "\n═══════════════ STATISTICS ═══════════════\n";
    std::cout << "Total Users: " << core.getUserCount() << "\n";
    std::cout << "Total Posts: " << core.getPostCount() << "\n";

    if (!currentUserID.empty()) {
        User* user = core.getUser(currentUserID);
        if (user) {
            std::cout << "\nYour Stats:\n";
            std::cout << "Followers: " << user->getFollowerCount() << "\n";
            std::cout << "Following: " << user->getFollowingCount() << "\n";
            std::cout << "Posts: " << core.getPostsByUser(currentUserID).size() << "\n";
        }
    }
    std::cout << "═══════════════════════════════════════════\n";
}

void mainMenu() {
    while (true) {
        clearScreen();
        displayBanner();

        if (currentUserID.empty()) {
            std::cout << "\n--- Main Menu ---\n";
            std::cout << "1. Sign Up\n";
            std::cout << "2. Login\n";
            std::cout << "3. Exit\n";
            std::cout << "Choice: ";

            int choice;
            std::cin >> choice;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            switch (choice) {
                case 1:
                    signup();
                    pause();
                    break;
                case 2:
                    login();
                    pause();
                    break;
                case 3:
                    SystemCore::getInstance().saveAllData();
                    std::cout << "\n Goodbye!\n";
                    return;
                default:
                    std::cout << " Invalid choice.\n";
                    pause();
            }
        } else {
            SystemCore& core = SystemCore::getInstance();
            User* user = core.getUser(currentUserID);

            if (!user) {
                // If user was deleted or not found, force logout
                currentUserID = "";
                continue;
            }

            std::cout << "\n👤 Logged in as: @" << user->getUsername() << "\n";
            std::cout << "\n--- Menu ---\n";
            std::cout << "1. Create Post\n";
            std::cout << "2. View My Posts\n";
            std::cout << "3. View Feed\n";
            std::cout << "4. Follow User\n";
            std::cout << "5. Unfollow User\n";
            std::cout << "6. Like Post\n";
            std::cout << "7. View Profile\n";
            std::cout << "8. Edit Profile\n";
            std::cout << "9. Statistics\n";
            std::cout << "10. Logout\n";
            std::cout << "Choice: ";

            int choice;
            std::cin >> choice;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            switch (choice) {
                case 1:
                    createPost();
                    pause();
                    break;
                case 2:
                    viewMyPosts();
                    pause();
                    break;
                case 3:
                    viewFeed();
                    pause();
                    break;
                case 4:
                    followUser();
                    pause();
                    break;
                case 5:
                    unfollowUser();
                    pause();
                    break;
                case 6:
                    likePost();
                    pause();
                    break;
                case 7:
                    viewProfile();
                    pause();
                    break;
                case 8:
                    editProfile();
                    pause();
                    break;
                case 9:
                    showStatistics();
                    pause();
                    break;
                case 10:
                    currentUserID = "";
                    std::cout << " Logged out successfully.\n";
                    pause();
                    break;
                default:
                    std::cout << " Invalid choice.\n";
                    pause();
            }
        }
    }
}

int main() {
    std::setlocale(LC_ALL, "en_US.UTF-8");

    // Initialize system and load data
    SystemCore& core = SystemCore::getInstance();

    std::cout << " Loading data...\n";
    core.loadAllData();

    // ensure post id counter continues after existing posts
    // requires SystemCore::updateNextPostID() to be implemented and public
    // if you used a different name, call that here instead
    try {
        core.updateNextPostID();
    } catch (...) {
        // if updateNextPostID is not available, ignore silently
        // (but recommended to implement it in SystemCore)
    }

    std::cout << " System ready!\n";
    pause();

    // Run main menu
    mainMenu();

    // Save data before exit
    std::cout << "\n Saving data...\n";
    core.saveAllData();
    std::cout << " Data saved successfully!\n";

    return 0;
}
