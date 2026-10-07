#ifndef RESERVATION_LIST_H
#define RESERVATION_LIST_H

#include <string>
#include <vector>

struct ReservationNode {
    std::string reservationID;
    std::string studentID;
    std::string resourceID;
    std::string timeSlot;
    ReservationNode* next;
};

class ReservationList {
private:
    ReservationNode* head;
    int count;

public:
    ReservationList();
    ~ReservationList();

    void insertReservation(
        const std::string& reservationID,
        const std::string& studentID,
        const std::string& resourceID,
        const std::string& timeSlot
    );

    bool removeReservation(
        const std::string& reservationID,
        ReservationNode& removedOut
    );

    bool findReservation(
        const std::string& reservationID,
        ReservationNode& out
    ) const;

    bool exists(
        const std::string& reservationID
    ) const;

    int countReservationsForSlot(
        const std::string& resourceID,
        const std::string& timeSlot
    ) const;

    void displayReservations() const;

    int getCount() const;

    // NEW: copy every active reservation into a vector (next pointers set to nullptr).
    // Searching, sorting and reports work on this copy so the linked list is never changed.
    std::vector<ReservationNode> getAllReservations() const;

    // NEW: true if this student already holds this resource/time slot (duplicate-request check)
    bool hasReservation(
        const std::string& studentID,
        const std::string& resourceID,
        const std::string& timeSlot
    ) const;
};

#endif
