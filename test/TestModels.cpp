
#include <memory>
#include <iostream>
#include <backend/IndieBackModels.hpp> 
#include <JSON.hpp>
#include <util/UUID.hpp>

std::unique_ptr<indiepub::User> user;


void testUserCRUD() {
    // Create a User object
    user = std::make_unique<indiepub::User>("user_id", UUID::random(), "fan", "John Doe", std::time(nullptr));
    std::cout << "User ID: " << user->user_id() << std::endl;
    std::cout << "Email: " << user->email() << std::endl;
    std::cout << "Role: " << user->role() << std::endl;
    std::cout << "Name: " << user->name() << std::endl;
    std::cout << "Created At: " << user->created_at() << std::endl;
    std::cout << "User JSON: " << user->to_json() << std::endl;
}

int main() {
    testUserCRUD();
    return 0;
}