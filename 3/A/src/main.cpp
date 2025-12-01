#include <iostream>
#include <map>
#include <string>

int main() {
    size_t n = 0;
    std::cin >> n;

    std::map<std::string, std::string> phonebook;

    for (size_t i = 0; i < n; ++i) {
        std::string command;
        std::cin >> command;

        if (command == "ADD") {
            std::string name, value;
            std::cin >> name >> value;
            auto it = phonebook.find(name);
            if (it != phonebook.end()) {
                std::cout << "ERROR\n";
            } else {
                phonebook[name] = value;
            }
        }

        else if (command == "DELETE") {
            std::string name;
            std::cin >> name;
            auto it = phonebook.find(name);
            if (it == phonebook.end()) {
                std::cout << "ERROR\n";
            } else {
                phonebook.erase(it);
            }
        }

        else if (command == "EDITPHONE") {
            std::string name, value;
            std::cin >> name >> value;
            auto it = phonebook.find(name);
            if (it == phonebook.end()) {
                std::cout << "ERROR\n";
            } else {
                it->second = value;
            }
        }

        else if (command == "PRINT") {
            std::string name;
            std::cin >> name;
            auto it = phonebook.find(name);
            if (it == phonebook.end()) {
                std::cout << "ERROR\n";
            } else {
                std::cout << it->first << ' ' << it->second << '\n';
            }
        }
    }

    return 0;
}
