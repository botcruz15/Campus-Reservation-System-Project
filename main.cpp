// Campus Resource Reservation System
// Resource Management + Reservation Management demo driver

#include <iostream>
#include <string>
#include <limits>
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
