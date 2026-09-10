#ifndef RESERVATION_LIST_H
#define RESERVATION_LIST_H

#include <string>

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

    void insertReservation(const std::string& reservationID,
                            const std::string& studentID,
                            const std::string& resourceID,
                            const std::string& timeSlot);
    bool removeReservation(const std::string& reservationID, ReservationNode& removedOut);
    bool findReservation(const std::string& reservationID, ReservationNode& out) const;
    bool exists(const std::string& reservationID) const;
    void displayReservations() const;
    int getCount() const;
};

#endif
