#ifndef RESOURCE_H
#define RESOURCE_H

#include <string>
#include <vector>

struct Resource {
    std::string id;
    std::string name;
    std::string type;
    int totalCapacity;
    int availableCount;
    int requestCount = 0;   // NEW: how many valid reservation requests this resource has received
};

class ResourceManager {
private:
    std::vector<Resource> resources;

public:
    ResourceManager();

    bool loadFromFile(const std::string& filename);

    void displayAllResources() const;
    void displayAvailability() const;

    bool resourceExists(const std::string& resourceID) const;
    Resource* findResource(const std::string& resourceID);

    bool decrementAvailability(const std::string& resourceID);
    bool incrementAvailability(const std::string& resourceID);

    int getResourceCount() const;

    // NEW: returns a copy of all resources (used by sorting and reports so the
    // original list is never reordered)
    std::vector<Resource> getAllResources() const;

    // NEW: adds 1 to a resource's request counter (used by the "most requested" report)
    bool recordRequest(const std::string& resourceID);
};

#endif

