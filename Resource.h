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
};


#endif
