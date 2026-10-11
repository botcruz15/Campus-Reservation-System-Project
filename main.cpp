// Campus Resource Reservation System
// Resource Management + Reservation Management demo driver

#include <iostream>
#include <string>
#include <limits>
#include <vector>
#include "Resource.h"
#include "ReservationList.h"

static std::string generateReservationID(int& counter) {
    counter++;
    return "RES" + std::to_string(counter);
}

static void printMenu() {
    std::cout << "\n===== Campus Resource Reservation System =====\n";
    std::cout << " 1. Display all resources\n";
    std::cout << " 2. Display resource availability\n";
    std::cout << " 3. Create a reservation\n";
    std::cout << " 4. Cancel a reservation\n";
    std::cout << " 5. Display active reservations\n";
    std::cout << " 6. Search resource by ID\n";
    std::cout << " 7. Display resources sorted by name\n";
    std::cout << " 0. Exit\n";
    std::cout << "Enter your choice: ";
}

static std::string readLine() {
    std::string line;
    std::getline(std::cin, line);
    return line;
}

static void clearInputError() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
static void mergeResources(std::vector<Resource>& items,
                           std::vector<Resource>& temporary,
                           std::size_t left,
                           std::size_t middle,
                           std::size_t right) {
    std::size_t i = left;
    std::size_t j = middle;
    std::size_t k = left;

    while (i < middle && j < right) {
        if (items[i].name <= items[j].name) {
            temporary[k++] = items[i++];
        } else {
            temporary[k++] = items[j++];
        }
    }

    while (i < middle) {
        temporary[k++] = items[i++];
    }

    while (j < right) {
        temporary[k++] = items[j++];
    }

    for (std::size_t index = left; index < right; ++index) {
        items[index] = temporary[index];
    }
}

static void mergeSortResources(std::vector<Resource>& items,
                               std::vector<Resource>& temporary,
                               std::size_t left,
                               std::size_t right) {
    if (right - left <= 1) {
        return;
    }

    std::size_t middle = left + (right - left) / 2;

    mergeSortResources(items, temporary, left, middle);
    mergeSortResources(items, temporary, middle, right);
    mergeResources(items, temporary, left, middle, right);
}

static void displayResourcesSortedByName(
    const ResourceManager& resourceManager) {

    std::vector<Resource> sorted = resourceManager.getAllResources();

    if (sorted.empty()) {
        std::cout << "No resources loaded.\n";
        return;
    }

    std::vector<Resource> temporary(sorted.size());
    mergeSortResources(sorted, temporary, 0, sorted.size());

    std::cout << "\n--- Resources Sorted by Name ---\n";

    for (const Resource& resource : sorted) {
        std::cout << "ID: " << resource.id
                  << " | Name: " << resource.name
                  << " | Type: " << resource.type
                  << " | Capacity: " << resource.totalCapacity
                  << " | Available: " << resource.availableCount
                  << "\n";
    }
}

int main() {
    ResourceManager resourceManager;
    ReservationList reservationList;
    int reservationCounter = 0;
    const std::string dataFile = "data/resources.txt";

    if (!resourceManager.loadFromFile(dataFile)) {
        std::cout << "Could not load '" << dataFile
                  << "'. Starting with zero resources.\n";
    } else {
        std::cout << "Loaded " << resourceManager.getResourceCount()
                  << " resource(s) from " << dataFile << "\n";
    }

    int choice = -1;
    while (choice != 0) {
        printMenu();

        if (!(std::cin >> choice)) {
            std::cout << "Invalid input. Please enter a number.\n";
            clearInputError();
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1: {
                resourceManager.displayAllResources();
                break;
            }
            case 2: {
                resourceManager.displayAvailability();
                break;
            }
            case 3: { // Create reservation
                std::string studentID, resourceID, timeSlot;

                std::cout << "Enter student ID: ";
                studentID = readLine();
                std::cout << "Enter resource ID: ";
                resourceID = readLine();
                std::cout << "Enter time slot (e.g. Mon-9AM): ";
                timeSlot = readLine();

                // ---- Validation ----
                if (studentID.empty() || resourceID.empty() || timeSlot.empty()) {
                    std::cout << "Error: all fields are required.\n";
                    break;
                }
                if (!resourceManager.resourceExists(resourceID)) {
                    std::cout << "Error: resource '" << resourceID << "' does not exist.\n";
                    break;
                }

                Resource* res = resourceManager.findResource(resourceID);
                if (res->availableCount <= 0) {
                    std::cout << "Error: resource '" << resourceID
                              << "' is fully booked. No availability.\n";
                    break;
                }

                std::string newID = generateReservationID(reservationCounter);
                resourceManager.decrementAvailability(resourceID);
                reservationList.insertReservation(newID, studentID, resourceID, timeSlot);
                std::cout << "Reservation created successfully. Reservation ID: " << newID << "\n";
                break;
            }
            case 4: { // Cancel reservation
                std::string reservationID;
                std::cout << "Enter reservation ID to cancel: ";
                reservationID = readLine();

                ReservationNode removed;
                bool found = reservationList.removeReservation(reservationID, removed);
                if (!found) {
                    std::cout << "Error: no active reservation found with ID '"
                              << reservationID << "'.\n";
                    break;
                }

                resourceManager.incrementAvailability(removed.resourceID);
                std::cout << "Reservation '" << reservationID << "' cancelled.\n";
                break;
            }
            case 5: {
                reservationList.displayReservations();
                break;
            }
            case 6: {
                std::cout << "Enter resource ID to search: ";
                std::string resourceID = readLine();

                if (resourceID.empty()) {
                std::cout << "Error: resource ID is required.\n";
                break;
                }

                Resource* resource = resourceManager.findResource(resourceID);

                if (resource == nullptr) {
                std::cout << "No resource found with ID '"
                      << resourceID << "'.\n";
                } else {
                std::cout << "\n--- Resource Found ---\n";
                std::cout << "ID: " << resource->id
                      << " | Name: " << resource->name
                  << " | Type: " << resource->type
                  << " | Capacity: " << resource->totalCapacity
                  << " | Available: " << resource->availableCount
                  << "\n";
                }

                break;
                }

            case 7: {
                displayResourcesSortedByName(resourceManager);
                break;
            }
            case 0: {
                std::cout << "Exiting Campus Resource Reservation System. Goodbye!\n";
                break;
            }
            default: {
                std::cout << "Invalid choice. Please select a valid menu option.\n";
                break;
            }
        }
    }

    return 0;
}
