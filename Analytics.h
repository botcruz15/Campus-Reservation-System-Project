#ifndef ANALYTICS_H
#define ANALYTICS_H

#include <string>
#include <utility>
#include <vector>
#include "Resource.h"
#include "ReservationList.h"
#include "WaitingList.h"

// =====================================================================
// SORTING - manual Quick Sort (no std::sort)
// =====================================================================

// Sorts data[lo..hi] in place. "before(a, b)" must return true when a belongs
// before b. Pivot = middle element; Hoare-style partition.
// Average O(n log n), worst case O(n^2). We always recurse into the SMALLER
// half and loop on the larger one, so recursion depth stays O(log n).
template <typename T>
void quickSortRange(std::vector<T>& data, int lo, int hi,
                    bool (*before)(const T&, const T&)) {
    while (lo < hi) {
        T pivot = data[lo + (hi - lo) / 2];   // copy of the middle value
        int i = lo;
        int j = hi;
        while (i <= j) {
            while (before(data[i], pivot)) i++;   // skip items already on the left side
            while (before(pivot, data[j])) j--;   // skip items already on the right side
            if (i <= j) {
                std::swap(data[i], data[j]);      // both are on the wrong side: swap
                i++;
                j--;
            }
        }
        // Now data[lo..j] <= pivot <= data[i..hi]. Recurse on the smaller side.
        if (j - lo < hi - i) {
            quickSortRange(data, lo, j, before);
            lo = i;
        } else {
            quickSortRange(data, i, hi, before);
            hi = j;
        }
    }
}

// Public entry point. Safe for empty and 1-element vectors.
template <typename T>
void quickSort(std::vector<T>& data, bool (*before)(const T&, const T&)) {
    if (data.size() < 2) return;
    quickSortRange(data, 0, static_cast<int>(data.size()) - 1, before);
}

// Comparators ("true if a goes before b")
bool resourceIDLess(const Resource& a, const Resource& b);          // by ID
bool resourceNameLess(const Resource& a, const Resource& b);        // by name, A-Z
bool resourcePopularityMore(const Resource& a, const Resource& b);  // most requested first
bool reservationTimeLess(const ReservationNode& a, const ReservationNode& b); // Mon..Sun, then hour

// =====================================================================
// SEARCHING
// =====================================================================

// Binary Search. REQUIRES the vector to already be sorted by resourceIDLess.
// Returns the index of the resource, or -1 if it is not there.
int binarySearchResourceByID(const std::vector<Resource>& sortedByID, const std::string& id);

// Linear Search. Returns every reservation held by this student.
std::vector<ReservationNode> findReservationsForStudent(
    const std::vector<ReservationNode>& all, const std::string& studentID);

// =====================================================================
// REPORTS (all numbers are computed from the live data when called)
// =====================================================================
void reportActiveReservations(const ReservationList& reservations);
void reportResourceUtilization(const ResourceManager& resources, const ReservationList& reservations);
void reportMostRequested(const ResourceManager& resources);
void reportWaitingListStats(const WaitingList& waiting, const ResourceManager& resources);

#endif
