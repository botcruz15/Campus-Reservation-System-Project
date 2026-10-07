#include "Analytics.h"
#include <cctype>
#include <iomanip>
#include <iostream>
#include <map>

// ---------------- small helpers (file-local) ----------------
static std::string lower(const std::string& s) {
    std::string out = s;
    for (size_t i = 0; i < out.size(); i++)
        out[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(out[i])));
    return out;
}

// ---------------- comparators ----------------
bool resourceIDLess(const Resource& a, const Resource& b) { return a.id < b.id; }

bool resourceNameLess(const Resource& a, const Resource& b) {
    std::string x = lower(a.name), y = lower(b.name);
    if (x != y) return x < y;
    return a.id < b.id;                       // tie-break keeps the order predictable
}

bool resourcePopularityMore(const Resource& a, const Resource& b) {
    if (a.requestCount != b.requestCount) return a.requestCount > b.requestCount;
    return resourceNameLess(a, b);
}

// "Mon-9AM" -> day 1..7 (unknown = 8)
static int dayOf(const std::string& slot) {
    static const char* names[7] = {"mon", "tue", "wed", "thu", "fri", "sat", "sun"};
    if (slot.size() < 3) return 8;
    std::string d = lower(slot.substr(0, 3));
    for (int i = 0; i < 7; i++) if (d == names[i]) return i + 1;
    return 8;
}

// "Mon-9AM" / "Tue-2PM" -> hour on a 24h clock (unknown = 99)
static int hourOf(const std::string& slot) {
    size_t dash = slot.find('-');
    if (dash == std::string::npos) return 99;
    size_t i = dash + 1;
    int h = 0;
    bool any = false;
    while (i < slot.size() && std::isdigit(static_cast<unsigned char>(slot[i])) && h < 1000) {
        h = h * 10 + (slot[i] - '0');
        any = true;
        i++;
    }
    if (!any) return 99;
    std::string rest = lower(slot.substr(i));
    if (rest.compare(0, 2, "pm") == 0 && h < 12) h += 12;
    if (rest.compare(0, 2, "am") == 0 && h == 12) h = 0;
    return h;
}

bool reservationTimeLess(const ReservationNode& a, const ReservationNode& b) {
    if (dayOf(a.timeSlot) != dayOf(b.timeSlot)) return dayOf(a.timeSlot) < dayOf(b.timeSlot);
    if (hourOf(a.timeSlot) != hourOf(b.timeSlot)) return hourOf(a.timeSlot) < hourOf(b.timeSlot);
    if (a.timeSlot != b.timeSlot) return a.timeSlot < b.timeSlot;
    if (a.reservationID.size() != b.reservationID.size())   // RES2 before RES10
        return a.reservationID.size() < b.reservationID.size();
    return a.reservationID < b.reservationID;
}

// ---------------- searching ----------------

// BINARY SEARCH: repeatedly look at the middle element and throw away the half
// that cannot contain the ID. Needs sorted data. O(log n).
int binarySearchResourceByID(const std::vector<Resource>& sortedByID, const std::string& id) {
    int low = 0;
    int high = static_cast<int>(sortedByID.size()) - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (sortedByID[mid].id == id) return mid;       // found
        if (sortedByID[mid].id < id) low = mid + 1;     // ID is in the right half
        else                         high = mid - 1;    // ID is in the left half
    }
    return -1;                                          // search space is empty: not found
}

// LINEAR SEARCH: check every reservation, keep the ones that match. O(n).
std::vector<ReservationNode> findReservationsForStudent(
    const std::vector<ReservationNode>& all, const std::string& studentID) {
    std::vector<ReservationNode> found;
    for (size_t i = 0; i < all.size(); i++)
        if (all[i].studentID == studentID) found.push_back(all[i]);
    return found;
}

// ---------------- reports ----------------

static std::string nameOf(const std::vector<Resource>& rs, const std::string& id) {
    for (size_t i = 0; i < rs.size(); i++) if (rs[i].id == id) return rs[i].name;
    return id;
}

// Report 1: counts what is in the reservation list right now.
void reportActiveReservations(const ReservationList& reservations) {
    std::vector<ReservationNode> all = reservations.getAllReservations();
    std::cout << "\n=== Report: Active Reservations ===\n";
    if (all.empty()) { std::cout << "No active reservations.\n"; return; }

    std::map<std::string, int> byStudent, bySlot;
    for (size_t i = 0; i < all.size(); i++) {
        byStudent[all[i].studentID]++;
        bySlot[all[i].timeSlot]++;
    }
    // Busiest time slot = largest count in bySlot
    std::string busiest;
    int busiestCount = 0;
    for (std::map<std::string, int>::const_iterator it = bySlot.begin(); it != bySlot.end(); ++it)
        if (it->second > busiestCount) { busiestCount = it->second; busiest = it->first; }

    std::cout << "Total active reservations: " << all.size() << "\n";
    std::cout << "Different students: " << byStudent.size() << "\n";
    std::cout << "Different time slots in use: " << bySlot.size() << "\n";
    std::cout << "Busiest time slot: " << busiest << " (" << busiestCount << " reservation(s))\n";
}

// Report 2: for each resource and each booked time slot, show used/capacity.
// Overall utilization = total booked seats / total capacity of the slots that are booked.
void reportResourceUtilization(const ResourceManager& resources, const ReservationList& reservations) {
    std::vector<Resource> rs = resources.getAllResources();
    std::vector<ReservationNode> all = reservations.getAllReservations();
    std::cout << "\n=== Report: Resource Utilization ===\n";
    if (rs.empty()) { std::cout << "No resources loaded.\n"; return; }
    if (all.empty()) { std::cout << "No active reservations: all resources are unused (0%).\n"; return; }

    long usedTotal = 0, capacityTotal = 0;
    std::cout << std::fixed << std::setprecision(1);
    for (size_t r = 0; r < rs.size(); r++) {
        std::map<std::string, int> perSlot;               // slot -> reservations for this resource
        for (size_t i = 0; i < all.size(); i++)
            if (all[i].resourceID == rs[r].id) perSlot[all[i].timeSlot]++;
        if (perSlot.empty()) continue;                    // never booked: nothing to show

        std::cout << rs[r].id << " (" << rs[r].name << "), capacity " << rs[r].totalCapacity << " per slot:\n";
        for (std::map<std::string, int>::const_iterator it = perSlot.begin(); it != perSlot.end(); ++it) {
            std::cout << "   " << it->first << ": " << it->second << "/" << rs[r].totalCapacity;
            if (rs[r].totalCapacity > 0)
                std::cout << " (" << (100.0 * it->second / rs[r].totalCapacity) << "%)";
            std::cout << "\n";
            usedTotal += it->second;
            capacityTotal += rs[r].totalCapacity;
        }
    }
    if (capacityTotal > 0)
        std::cout << "Overall utilization of booked slots: " << (100.0 * usedTotal / capacityTotal) << "%\n";
    else
        std::cout << "Overall utilization: N/A (no valid capacity)\n";
}

// Report 3: sort a copy of the resources by request count (quick sort), print those requested.
void reportMostRequested(const ResourceManager& resources) {
    std::vector<Resource> rs = resources.getAllResources();
    std::cout << "\n=== Report: Most Requested Resources ===\n";
    if (rs.empty()) { std::cout << "No resources loaded.\n"; return; }

    int total = 0;
    for (size_t i = 0; i < rs.size(); i++) total += rs[i].requestCount;
    if (total == 0) { std::cout << "No reservation requests have been made yet.\n"; return; }

    quickSort(rs, resourcePopularityMore);
    std::cout << "Total requests (booked + waitlisted): " << total << "\n";
    for (size_t i = 0; i < rs.size() && rs[i].requestCount > 0; i++) {
        std::cout << (i + 1) << ". " << rs[i].id << " (" << rs[i].name << ") - "
                  << rs[i].requestCount << " request(s), "
                  << std::fixed << std::setprecision(1)
                  << (100.0 * rs[i].requestCount / total) << "% of all requests\n";
    }
}

// Report 4: everything is counted from the students currently in the queue.
void reportWaitingListStats(const WaitingList& waiting, const ResourceManager& resources) {
    std::vector<WaitingNode> q = waiting.getAllWaiting();
    std::vector<Resource> rs = resources.getAllResources();
    std::cout << "\n=== Report: Waiting-List Statistics ===\n";
    if (q.empty()) { std::cout << "The waiting list is empty.\n"; return; }

    std::map<std::string, int> byResource, byStudent;
    for (size_t i = 0; i < q.size(); i++) {
        byResource[q[i].resourceID]++;
        byStudent[q[i].studentID]++;
    }
    std::string worstID;
    int worst = 0;
    for (std::map<std::string, int>::const_iterator it = byResource.begin(); it != byResource.end(); ++it)
        if (it->second > worst) { worst = it->second; worstID = it->first; }

    std::cout << "Students waiting: " << q.size() << " entries from " << byStudent.size() << " student(s)\n";
    std::cout << "Longest queue: " << worstID << " (" << nameOf(rs, worstID) << ") with " << worst << "\n";
    std::cout << "Next in line overall: " << q[0].studentID << " for " << q[0].resourceID
              << " at " << q[0].timeSlot << "\n";
    for (std::map<std::string, int>::const_iterator it = byResource.begin(); it != byResource.end(); ++it)
        std::cout << "   " << it->first << " (" << nameOf(rs, it->first) << "): " << it->second << " waiting\n";
}
