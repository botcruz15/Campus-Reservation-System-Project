#include "Resource.h"
#include <iostream>
#include <fstream>
#include <sstream>

ResourceManager::ResourceManager() {}

static std::vector<std::string> splitCSV(const std::string& line) {
    std::vector<std::string> fields;
    std::stringstream ss(line);
    std::string field;
    while (std::getline(ss, field, ',')) fields.push_back(field);
    return fields;
}

bool ResourceManager::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: could not open resource file '" << filename << "'\n";
        return false;
    }

    resources.clear();
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::vector<std::string> fields = splitCSV(line);
        if (fields.size() < 4) {
            std::cerr << "Warning: skipping malformed line: " << line << "\n";
            continue;
        }

        Resource r;
        r.id = fields[0];
        r.name = fields[1];
        r.type = fields[2];
        try {
            r.totalCapacity = std::stoi(fields[3]);
        } catch (...) {
            std::cerr << "Warning: invalid capacity on line: " << line << "\n";
            continue;
        }
        r.availableCount = r.totalCapacity;
        resources.push_back(r);
    }

    file.close();
    return true;
}

void ResourceManager::displayAllResources() const {
    if (resources.empty()) { std::cout << "No resources loaded.\n"; return; }
    std::cout << "\n--- All Resources ---\n";
    for (const auto& r : resources) {
        std::cout << "ID: " << r.id << " | Name: " << r.name
                  << " | Type: " << r.type << " | Capacity: " << r.totalCapacity << "\n";
    }
}

void ResourceManager::displayAvailability() const {
    if (resources.empty()) { std::cout << "No resources loaded.\n"; return; }
    std::cout << "\n--- Resource Availability ---\n";
    for (const auto& r : resources) {
        std::cout << "ID: " << r.id << " | Name: " << r.name
                  << " | Available: " << r.availableCount << " / " << r.totalCapacity << "\n";
    }
}

bool ResourceManager::resourceExists(const std::string& resourceID) const {
    for (const auto& r : resources) if (r.id == resourceID) return true;
    return false;
}

Resource* ResourceManager::findResource(const std::string& resourceID) {
    for (auto& r : resources) if (r.id == resourceID) return &r;
    return nullptr;
}

bool ResourceManager::decrementAvailability(const std::string& resourceID) {
    Resource* r = findResource(resourceID);
    if (!r || r->availableCount <= 0) return false;
    r->availableCount--;
    return true;
}

bool ResourceManager::incrementAvailability(const std::string& resourceID) {
    Resource* r = findResource(resourceID);
    if (!r || r->availableCount >= r->totalCapacity) return false;
    r->availableCount++;
    return true;
}

int ResourceManager::getResourceCount() const {
    return static_cast<int>(resources.size());
}
