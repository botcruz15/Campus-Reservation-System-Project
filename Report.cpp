#include "Reports.h"
#include "Algorithms.h"
#include <iostream>
#include <iomanip>
#include <map>
#include <string>
#include <vector>

// ---------------------------------------------------------------------
// REPORT 1: Active reservations
// Totals come from counting the reservations currently in the list.
// ---------------------------------------------------------------------
void reportActiveReservations(const ReservationList& reservations) {
    std::vector<ReservationNode> all = reservations.getAllReservations();
    std::cout << "\n=== Report: Active Reservations ===\n";
    if (all.empty()) {
        std::cout << "No active reservations to report.\n";
        return;
    }

    std::map<std::string, int> perStudent;   // student ID -> how many reservations they hold
    std::map<std::string, int> perResource;  // resource ID -> how many active reservations
    for (size_t i = 0; i < all.size(); i++) {
        perStudent[all[i].studentID]++;
        perResource[all[i].resourceID]++;
    }

    std::cout << "Total active reservations: " << all.size() << "\n";
    std::cout << "Students with a reservation: " << perStudent.size() << "\n";
    std::cout << "Active reservations per resource:\n";
    for (std::map<std::string, int>::const_iterator it = perResource.begin();
         it != perResource.end(); ++it) {
        std::cout << "  " << it->first << ": " << it->second << "\n";
    }
}

// ---------------------------------------------------------------------
// REPORT 2: Resource utilization
// For each resource:
//   active      = reservations currently held for it
//   slotsInUse  = number of different time slots that have at least one reservation
//   utilization = active / (capacity * slotsInUse) * 100
// i.e. "of the slots people actually booked, how full are they?"
// ---------------------------------------------------------------------
void reportResourceUtilization(const ResourceManager& resources,
                               const ReservationList& reservations) {
    std::vector<Resource> allResources = resources.getAllResources();
    std::vector<ReservationNode> all = reservations.getAllReservations();

    std::cout << "\n=== Report: Resource Utilization ===\n";
    if (allResources.empty()) {
        std::cout << "No resources loaded.\n";
        return;
    }
    if (all.empty()) {
        std::cout << "No active reservations, so every resource is at 0% utilization.\n";
    }

    std::cout << std::fixed << std::setprecision(1);
    for (size_t r = 0; r < allResources.size(); r++) {
        const Resource& res = allResources[r];

        int active = 0;
        std::map<std::string, int> slotsUsed;      // distinct time slots for this resource
        for (size_t i = 0; i < all.size(); i++) {
            if (all[i].resourceID == res.id) {
                active++;
                slotsUsed[all[i].timeSlot]++;
            }
        }

        std::cout << res.id << " (" << res.name << ") | Active: " << active
                  << " | Slots in use: " << slotsUsed.size()
                  << " | Capacity per slot: " << res.totalCapacity << " | Utilization: ";
        if (active == 0) {
            std::cout << "0.0%\n";
        }
        else if (res.totalCapacity <= 0) {
            std::cout << "N/A (invalid capacity)\n";   // avoids dividing by zero
        }
        else {
            double possible = static_cast<double>(res.totalCapacity) * slotsUsed.size();
            std::cout << (100.0 * active / possible) << "%\n";
        }
    }
}

// ---------------------------------------------------------------------
// REPORT 3: Most requested resources
// Each valid reservation request (booked OR waitlisted) increments the
// resource's requestCount. Here we sort a copy by that count with merge sort.
// ---------------------------------------------------------------------
void reportMostRequestedResources(const ResourceManager& resources) {
    std::vector<Resource> sorted = resources.getAllResources();

    std::cout << "\n=== Report: Most Requested Resources ===\n";
    if (sorted.empty()) {
        std::cout << "No resources loaded.\n";
        return;
    }

    int totalRequests = 0;
    for (size_t i = 0; i < sorted.size(); i++) totalRequests += sorted[i].requestCount;
    if (totalRequests == 0) {
        std::cout << "No reservation requests have been made yet.\n";
        return;
    }

    mergeSort(sorted, resourcePopularityGreater);   // most requested first

    std::cout << "Total requests: " << totalRequests << "\n";
    int rank = 1;
    for (size_t i = 0; i < sorted.size(); i++) {
        if (sorted[i].requestCount == 0) break;     // sorted, so the rest are also 0
        std::cout << rank << ". " << sorted[i].id << " (" << sorted[i].name
                  << ") - " << sorted[i].requestCount << " request(s)\n";
        rank++;
    }
}
