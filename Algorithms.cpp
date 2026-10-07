#include "Algorithms.h"
#include <cctype>

// ---------------------------------------------------------------------
// Small string helpers (file-local)
// ---------------------------------------------------------------------
static std::string toLower(const std::string& s) {
    std::string out = s;
    for (size_t i = 0; i < out.size(); i++) {
        out[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(out[i])));
    }
    return out;
}

// =====================================================================
// LINEAR SEARCH
// Check each element from the start until a match is found.
// Used instead of binary search because reservations are kept in a
// linked list in insertion order (not sorted by ID).
// Time: O(n).
// =====================================================================
int linearSearchReservationByID(
    const std::vector<ReservationNode>& reservations,
    const std::string& reservationID) {
    for (size_t i = 0; i < reservations.size(); i++) {
        if (reservations[i].reservationID == reservationID) {
            return static_cast<int>(i);   // found: return its position
        }
    }
    return -1;                            // checked everything: not found
}

// Same idea, but a student can have many reservations, so we do not stop
// at the first match - we collect every match.
std::vector<ReservationNode> linearSearchReservationsByStudent(
    const std::vector<ReservationNode>& reservations,
    const std::string& studentID) {
    std::vector<ReservationNode> matches;
    for (size_t i = 0; i < reservations.size(); i++) {
        if (reservations[i].studentID == studentID) {
            matches.push_back(reservations[i]);
        }
    }
    return matches;
}

// =====================================================================
// COMPARATORS used by mergeSort (the merge sort itself is in Algorithms.h
// because it is a template and works for both Resource and ReservationNode)
// =====================================================================

// Resources A-Z by name (case-insensitive). Ties broken by ID so the order is predictable.
bool resourceNameLess(const Resource& a, const Resource& b) {
    std::string an = toLower(a.name);
    std::string bn = toLower(b.name);
    if (an != bn) return an < bn;
    return a.id < b.id;
}

// Most requested first. Ties broken by name.
bool resourcePopularityGreater(const Resource& a, const Resource& b) {
    if (a.requestCount != b.requestCount) return a.requestCount > b.requestCount;
    return resourceNameLess(a, b);
}

// Turn the day part of a slot such as "Mon-9AM" into 1..7. Unknown text goes last (8).
static int dayRank(const std::string& slot) {
    if (slot.size() < 3) return 8;
    std::string d = toLower(slot.substr(0, 3));
    const char* days[7] = {"mon", "tue", "wed", "thu", "fri", "sat", "sun"};
    for (int i = 0; i < 7; i++) {
        if (d == days[i]) return i + 1;
    }
    return 8;
}

// Turn the time part of "Mon-9AM" / "Tue-2PM" into a 24-hour number. Unknown goes last (99).
static int hourValue(const std::string& slot) {
    size_t dash = slot.find('-');
    if (dash == std::string::npos) return 99;
    size_t i = dash + 1;
    int hour = 0;
    bool sawDigit = false;
    while (i < slot.size() && std::isdigit(static_cast<unsigned char>(slot[i])) && hour < 1000) {
        hour = hour * 10 + (slot[i] - '0');
        sawDigit = true;
        i++;
    }
    if (!sawDigit) return 99;
    std::string suffix = toLower(slot.substr(i));
    if (suffix.compare(0, 2, "pm") == 0 && hour < 12) hour += 12;
    if (suffix.compare(0, 2, "am") == 0 && hour == 12) hour = 0;
    return hour;
}

// Reservations in calendar order: day of week, then hour, then plain text, then reservation ID.
bool reservationTimeSlotLess(const ReservationNode& a, const ReservationNode& b) {
    int da = dayRank(a.timeSlot), db = dayRank(b.timeSlot);
    if (da != db) return da < db;
    int ha = hourValue(a.timeSlot), hb = hourValue(b.timeSlot);
    if (ha != hb) return ha < hb;
    if (a.timeSlot != b.timeSlot) return a.timeSlot < b.timeSlot;
    // Same slot: shorter ID first so RES2 comes before RES10.
    if (a.reservationID.size() != b.reservationID.size())
        return a.reservationID.size() < b.reservationID.size();
    return a.reservationID < b.reservationID;
}
