#include "CancellationHistory.h"
#include <iostream>

CancellationHistory::CancellationHistory() : top(nullptr), count(0) {
}

CancellationHistory::~CancellationHistory() {
    CancellationNode* current = top;

    while (current != nullptr) {
        CancellationNode* toDelete = current;
        current = current->next;
        delete toDelete;
    }

    top = nullptr;
}

void CancellationHistory::pushCancellation(const ReservationNode& reservation) {

    CancellationNode* newNode = new CancellationNode();

    newNode->reservation = reservation;

    newNode->reservation.next = nullptr;

    newNode->next = top;
    top = newNode;

    count++;
}

bool CancellationHistory::popCancellation(ReservationNode& restoredReservation) {

    if (top == nullptr) {
        return false;
    }

    CancellationNode* temp = top;

    restoredReservation = top->reservation;
    restoredReservation.next = nullptr;

    top = top->next;

    delete temp;
    count--;

    return true;
}

bool CancellationHistory::peekCancellation(ReservationNode& reservation) const {

    if (top == nullptr) {
        return false;
    }

    reservation = top->reservation;
    reservation.next = nullptr;

    return true;
}

void CancellationHistory::displayHistory() const {

    if (top == nullptr) {
        std::cout << "No cancelled reservations in history.\n";
        return;
    }

    std::cout << "\n--- Cancellation History ---\n";

    CancellationNode* current = top;
    int position = 1;

    while (current != nullptr) {

        std::cout << position << ". "
                  << "Reservation ID: "
                  << current->reservation.reservationID

                  << " | Student: "
                  << current->reservation.studentID

                  << " | Resource: "
                  << current->reservation.resourceID

                  << " | Time Slot: "
                  << current->reservation.timeSlot
                  << "\n";

        current = current->next;
        position++;
    }
}

bool CancellationHistory::isEmpty() const {
    return top == nullptr;
}

int CancellationHistory::getCount() const {
    return count;
}