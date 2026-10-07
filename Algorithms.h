#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <string>
#include <vector>
#include "Resource.h"
#include "ReservationList.h"


// SEARCHING - manual Linear Search (no std::find)


// Returns the index of the reservation with this ID, or -1 if not found.
int linearSearchReservationByID(
    const std::vector<ReservationNode>& reservations,
    const std::string& reservationID);

// Returns every reservation that belongs to this student (empty vector if none).
std::vector<ReservationNode> linearSearchReservationsByStudent(
    const std::vector<ReservationNode>& reservations,
    const std::string& studentID);


// SORTING - manual Merge Sort (no std::sort)


// Comparators: each returns true when "a" must come BEFORE "b".
bool resourceNameLess(const Resource& a, const Resource& b);          // A-Z by name
bool resourcePopularityGreater(const Resource& a, const Resource& b); // most requested first
bool reservationTimeSlotLess(const ReservationNode& a, const ReservationNode& b); // Mon..Sun, then hour

// Merge sort helper: sorts data[left..right] using the temp buffer.
template <typename T>
void mergeSortHelper(std::vector<T>& data, std::vector<T>& temp,
                     int left, int right,
                     bool (*comesBefore)(const T&, const T&)) {
    if (left >= right) return;               // 0 or 1 element: already sorted

    int mid = left + (right - left) / 2;
    mergeSortHelper(data, temp, left, mid, comesBefore);       // sort left half
    mergeSortHelper(data, temp, mid + 1, right, comesBefore);  // sort right half

    // Merge the two sorted halves into temp.
    int i = left;       // walks the left half
    int j = mid + 1;    // walks the right half
    int k = left;       // next free slot in temp
    while (i <= mid && j <= right) {
        // Take from the right half only if it strictly belongs first.
        // Otherwise take from the left, which keeps equal items in their original order (stable).
        if (comesBefore(data[j], data[i])) temp[k++] = data[j++];
        else                               temp[k++] = data[i++];
    }
    while (i <= mid)   temp[k++] = data[i++];   // leftovers from left half
    while (j <= right) temp[k++] = data[j++];   // leftovers from right half

    for (int x = left; x <= right; x++) data[x] = temp[x];   // copy merged result back
}

// Public entry point. Safe on empty and single-element vectors.
// Time: O(n log n). Extra space: O(n).
template <typename T>
void mergeSort(std::vector<T>& data, bool (*comesBefore)(const T&, const T&)) {
    if (data.size() < 2) return;
    std::vector<T> temp(data.size());
    mergeSortHelper(data, temp, 0, static_cast<int>(data.size()) - 1, comesBefore);
}

#endif
