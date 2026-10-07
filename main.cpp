#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include "Resource.h"
#include "ReservationList.h"
#include "CancellationHistory.h"
#include "WaitingList.h"
#include "Analytics.h"    // NEW: quick sort, binary/linear search, four reports

static std::string generateReservationID(int& counter) {
    counter++;
    return "RES" + std::to_string(counter);
}

static void printMenu() {
    std::cout << "\n===== Campus Resource Reservation System =====\n";
    std::cout << "  1. Display all resources\n";
    std::cout << "  2. Display resource availability\n";
    std::cout << "  3. Create a reservation\n";
    std::cout << "  4. Cancel a reservation\n";
    std::cout << "  5. Display active reservations\n";
    std::cout << "  6. Display waiting list\n";
    std::cout << "  7. Display cancellation history\n";
    std::cout << "  8. Undo last cancellation\n";
    std::cout << "  9. Search resource by ID\n";             // NEW
    std::cout << " 10. Find reservations for a student\n";      // NEW
    std::cout << " 11. Display resources sorted\n";             // NEW
    std::cout << " 12. Display reservations sorted by time\n";  // NEW
    std::cout << " 13. Reports\n";                              // NEW
    std::cout << "  0. Exit\n";
    std::cout << "Enter your choice: ";
}

// NEW: trims leading/trailing spaces and tabs so " R001 " is treated as "R001".
static std::string trim(const std::string& s) {
    size_t start = 0;
    while (start < s.size() && (s[start] == ' ' || s[start] == '\t' || s[start] == '\r')) start++;
    size_t end = s.size();
    while (end > start && (s[end - 1] == ' ' || s[end - 1] == '\t' || s[end - 1] == '\r')) end--;
    return s.substr(start, end - start);
}

static std::string readLine() {
    std::string line;
    std::getline(std::cin, line);
    return trim(line);
}

static void clearInputError() {
    std::cin.clear();
    std::cin.ignore(
        std::numeric_limits<std::streamsize>::max(),
        '\n'
    );
}

// NEW: asks for a whole number between low and high (inclusive).
// Returns false (after printing an error) if the input is not a valid choice.
static bool readChoiceInRange(const std::string& prompt, int low, int high, int& out) {
    std::cout << prompt;
    std::string line = readLine();
    if (line.empty()) {
        std::cout << "Invalid input. Please enter a number.\n";
        return false;
    }
    for (size_t i = 0; i < line.size(); i++) {
        if (line[i] < '0' || line[i] > '9' || line.size() > 6) {
            std::cout << "Invalid input. Please enter a number.\n";
            return false;
        }
    }
    int value = std::stoi(line);
    if (value < low || value > high) {
        std::cout << "Invalid choice. Please enter a number from "
                  << low << " to " << high << ".\n";
        return false;
    }
    out = value;
    return true;
}

// NEW: prints one reservation on one line (same format as displayReservations)
static void printReservation(const ReservationNode& r) {
    std::cout << "Reservation ID: " << r.reservationID
              << " | Student: " << r.studentID
              << " | Resource: " << r.resourceID
              << " | Time Slot: " << r.timeSlot << "\n";
}

int main() {
    ResourceManager resourceManager;
    ReservationList reservationList;
    WaitingList waitingList;
    CancellationHistory cancellationHistory;

    int reservationCounter = 0;
    const std::string dataFile = "resources.txt";

    if (!resourceManager.loadFromFile(dataFile)) {
        std::cout << "Could not load '"
                  << dataFile
                  << "'. Starting with zero resources.\n";
    }
    else {
        std::cout << "Loaded "
                  << resourceManager.getResourceCount()
                  << " resource(s) from "
                  << dataFile << "\n";
    }

    int choice = -1;
    while (choice != 0) {
        printMenu();

        if (!(std::cin >> choice)) {
            // NEW: if input has ended (Ctrl+D / end of a test file) stop instead of looping forever
            if (std::cin.eof()) {
                std::cout << "\nInput ended. Exiting.\n";
                break;
            }
            std::cout << "Invalid input. Please enter a number.\n";
            clearInputError();
            choice = -1;   // NEW: keep the loop running after bad input
            continue;
        }
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(),
            '\n'
        );

        switch (choice) {
        case 1: {
            resourceManager.displayAllResources();
            break;
        }

        case 2: {
            resourceManager.displayAvailability();
            break;
        }

        case 3: {
            std::string studentID;
            std::string resourceID;
            std::string timeSlot;

            std::cout << "Enter student ID: ";
            studentID = readLine();
            std::cout << "Enter resource ID: ";
            resourceID = readLine();
            std::cout << "Enter time slot (e.g. Mon-9AM): ";
            timeSlot = readLine();

            if (studentID.empty() ||
                resourceID.empty() ||
                timeSlot.empty()) {
                std::cout << "Error: all fields are required.\n";
                break;
            }

            if (!resourceManager.resourceExists(resourceID)) {
                std::cout << "Error: resource '" << resourceID << "' does not exist.\n";
                break;
            }

            // NEW: reject duplicate requests (same student, resource and slot)
            if (reservationList.hasReservation(studentID, resourceID, timeSlot)) {
                std::cout << "Error: student '" << studentID
                          << "' already has a reservation for " << resourceID
                          << " at " << timeSlot << ".\n";
                break;
            }
            if (waitingList.contains(studentID, resourceID, timeSlot)) {
                std::cout << "Error: student '" << studentID
                          << "' is already on the waiting list for " << resourceID
                          << " at " << timeSlot << ".\n";
                break;
            }

            Resource* res = resourceManager.findResource(resourceID);

            // NEW: a capacity of 0 or less can never be booked
            if (res->totalCapacity <= 0) {
                std::cout << "Error: resource '" << resourceID
                          << "' has no capacity and cannot be reserved.\n";
                break;
            }

            // NEW: this is a valid request (booked or waitlisted), so count it for the
            // "most requested resources" report and for popularity sorting
            resourceManager.recordRequest(resourceID);

            int reservationsForSlot =
                reservationList.countReservationsForSlot(resourceID, timeSlot);

            if (reservationsForSlot >= res->totalCapacity) {
                waitingList.enqueue(studentID, resourceID, timeSlot);
                std::cout << "Resource '" << resourceID
                          << "' is fully booked for " << timeSlot << ".\n";
                std::cout << "Student added to the waiting list.\n";
                break;
            }

            std::string newID = generateReservationID(reservationCounter);
            reservationList.insertReservation(newID, studentID, resourceID, timeSlot);
            std::cout << "Reservation created successfully. "
                      << "Reservation ID: " << newID << "\n";
            break;
        }

        case 4: {
            // NEW: nothing to cancel if the list is empty
            if (reservationList.getCount() == 0) {
                std::cout << "There are no active reservations to cancel.\n";
                break;
            }

            std::string reservationID;
            std::cout << "Enter reservation ID to cancel: ";
            reservationID = readLine();

            // NEW: a blank ID can never match a reservation
            if (reservationID.empty()) {
                std::cout << "Error: reservation ID cannot be empty.\n";
                break;
            }

            ReservationNode removed;
            bool found = reservationList.removeReservation(reservationID, removed);

            if (!found) {
                std::cout << "Error: no active reservation found with ID '"
                          << reservationID << "'.\n";
                break;
            }

            cancellationHistory.pushCancellation(removed);

            std::cout << "Reservation '" << reservationID << "' cancelled.\n";
            std::cout << "Reservation added to cancellation history.\n";

            WaitingNode nextStudent;
            if (waitingList.dequeueForResource(
                    removed.resourceID,
                    removed.timeSlot,
                    nextStudent)) {
                std::string newID = generateReservationID(reservationCounter);
                reservationList.insertReservation(
                    newID,
                    nextStudent.studentID,
                    nextStudent.resourceID,
                    nextStudent.timeSlot
                );
                std::cout << "Student " << nextStudent.studentID
                          << " was next on the waiting list.\n";
                std::cout << "Reservation created automatically. "
                          << "Reservation ID: " << newID << "\n";
            }
            break;
        }

        case 5: {
            reservationList.displayReservations();
            break;
        }

        case 6: {
            waitingList.displayWaitingList();
            break;
        }

        case 7: {
            cancellationHistory.displayHistory();
            break;
        }

        case 8: {
            ReservationNode restored;
            if (!cancellationHistory.popCancellation(restored)) {
                std::cout << "No cancelled reservations to restore.\n";
                break;
            }

            Resource* res = resourceManager.findResource(restored.resourceID);
            if (res == nullptr) {
                std::cout << "Could not restore reservation. "
                          << "Resource does not exist.\n";
                cancellationHistory.pushCancellation(restored);
                break;
            }

            int reservationsForSlot =
                reservationList.countReservationsForSlot(
                    restored.resourceID,
                    restored.timeSlot
                );

            if (reservationsForSlot >= res->totalCapacity) {
                std::cout << "Could not restore reservation. "
                          << "The original time slot is currently full.\n";
                cancellationHistory.pushCancellation(restored);
                break;
            }

            reservationList.insertReservation(
                restored.reservationID,
                restored.studentID,
                restored.resourceID,
                restored.timeSlot
            );
            std::cout << "Reservation '" << restored.reservationID
                      << "' restored successfully.\n";
            break;
        }

        // ---------------- NEW: searching ----------------
        case 9: {
            // Binary search: copy the resources, quick-sort the copy by ID, then binary-search it.
            if (resourceManager.getResourceCount() == 0) {
                std::cout << "No resources loaded.\n";
                break;
            }
            std::cout << "Enter resource ID to search for: ";
            std::string id = readLine();
            if (id.empty()) {
                std::cout << "Error: resource ID cannot be empty.\n";
                break;
            }
            std::vector<Resource> byID = resourceManager.getAllResources();
            quickSort(byID, resourceIDLess);                 // binary search needs sorted data
            int index = binarySearchResourceByID(byID, id);
            if (index < 0) {
                std::cout << "No resource found with ID '" << id << "'.\n";
            }
            else {
                const Resource& r = byID[index];
                std::cout << "\n--- Resource Found ---\n"
                          << "ID: " << r.id << " | Name: " << r.name << " | Type: " << r.type
                          << " | Capacity: " << r.totalCapacity << " | Requests: " << r.requestCount << "\n";
            }
            break;
        }

        case 10: {
            // Linear search: every reservation held by one student.
            if (reservationList.getCount() == 0) {
                std::cout << "There are no active reservations to search.\n";
                break;
            }
            std::cout << "Enter student ID: ";
            std::string sid = readLine();
            if (sid.empty()) {
                std::cout << "Error: student ID cannot be empty.\n";
                break;
            }
            std::vector<ReservationNode> matches =
                findReservationsForStudent(reservationList.getAllReservations(), sid);
            if (matches.empty()) {
                std::cout << "No active reservations found for student '" << sid << "'.\n";
            }
            else {
                std::cout << "\n--- Reservations for " << sid << " (" << matches.size() << ") ---\n";
                for (size_t i = 0; i < matches.size(); i++) printReservation(matches[i]);
            }
            break;
        }

        // ---------------- NEW: sorting ----------------
        case 11: {
            // Quick-sort a COPY of the resources and display it.
            if (resourceManager.getResourceCount() == 0) {
                std::cout << "No resources loaded.\n";
                break;
            }
            std::cout << "Sort resources by:\n  1. Name (A-Z)\n  2. Popularity (most requested first)\n";
            int sortChoice = 0;
            if (!readChoiceInRange("Enter your choice: ", 1, 2, sortChoice)) break;

            std::vector<Resource> sorted = resourceManager.getAllResources();
            if (sortChoice == 1) {
                quickSort(sorted, resourceNameLess);
                std::cout << "\n--- Resources Sorted by Name ---\n";
            }
            else {
                quickSort(sorted, resourcePopularityMore);
                std::cout << "\n--- Resources Sorted by Popularity ---\n";
            }
            for (size_t i = 0; i < sorted.size(); i++) {
                std::cout << "ID: " << sorted[i].id << " | Name: " << sorted[i].name
                          << " | Type: " << sorted[i].type << " | Capacity: " << sorted[i].totalCapacity
                          << " | Requests: " << sorted[i].requestCount << "\n";
            }
            break;
        }

        case 12: {
            // Quick-sort a COPY of the reservations by day and hour.
            if (reservationList.getCount() == 0) {
                std::cout << "There are no active reservations to sort.\n";
                break;
            }
            std::vector<ReservationNode> all = reservationList.getAllReservations();
            quickSort(all, reservationTimeLess);
            std::cout << "\n--- Active Reservations Sorted by Time Slot ---\n";
            for (size_t i = 0; i < all.size(); i++) printReservation(all[i]);
            break;
        }

        // ---------------- NEW: reports ----------------
        case 13: {
            std::cout << "Reports:\n"
                      << "  1. Active reservations\n"
                      << "  2. Resource utilization\n"
                      << "  3. Most requested resources\n"
                      << "  4. Waiting-list statistics\n"
                      << "  0. Back\n";
            int reportChoice = 0;
            if (!readChoiceInRange("Enter your choice: ", 0, 4, reportChoice)) break;
            if (reportChoice == 1) reportActiveReservations(reservationList);
            else if (reportChoice == 2) reportResourceUtilization(resourceManager, reservationList);
            else if (reportChoice == 3) reportMostRequested(resourceManager);
            else if (reportChoice == 4) reportWaitingListStats(waitingList, resourceManager);
            break;
        }

        case 0: {
            std::cout << "Exiting Campus Resource Reservation System. "
                      << "Goodbye!\n";
            break;
        }

        default: {
            std::cout << "Invalid choice. "
                      << "Please select a valid menu option.\n";
            break;
        }
        }
    }

    return 0;
}
