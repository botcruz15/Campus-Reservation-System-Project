#include "ReservationList.h"
#include <iostream>

ReservationList::ReservationList()
    : head(nullptr), count(0) {
}

ReservationList::~ReservationList() {
    ReservationNode* current = head;
    while (current != nullptr) {
        ReservationNode* toDelete = current;
        current = current->next;
        delete toDelete;
    }
    head = nullptr;
    count = 0;
}

void ReservationList::insertReservation(
    const std::string& reservationID,
    const std::string& studentID,
    const std::string& resourceID,
    const std::string& timeSlot) {
    ReservationNode* newNode = new ReservationNode();
    newNode->reservationID = reservationID;
    newNode->studentID = studentID;
    newNode->resourceID = resourceID;
    newNode->timeSlot = timeSlot;
    newNode->next = head;
    head = newNode;
    count++;
}

bool ReservationList::removeReservation(
    const std::string& reservationID,
    ReservationNode& removedOut) {
    ReservationNode* current = head;
    ReservationNode* previous = nullptr;
    while (current != nullptr) {
        if (current->reservationID == reservationID) {
            removedOut = *current;
            removedOut.next = nullptr;
            if (previous == nullptr) {
                head = current->next;
            }
            else {
                previous->next = current->next;
            }
            delete current;
            count--;
            return true;
        }
        previous = current;
        current = current->next;
    }
    return false;
}

bool ReservationList::findReservation(
    const std::string& reservationID,
    ReservationNode& out) const {
    ReservationNode* current = head;
    while (current != nullptr) {
        if (current->reservationID == reservationID) {
            out = *current;
            out.next = nullptr;
            return true;
        }
        current = current->next;
    }
    return false;
}

bool ReservationList::exists(
    const std::string& reservationID) const {
    ReservationNode temp;
    return findReservation(
        reservationID,
        temp
    );
}

int ReservationList::countReservationsForSlot(
    const std::string& resourceID,
    const std::string& timeSlot) const {
    int slotCount = 0;
    ReservationNode* current = head;
    while (current != nullptr) {
        if (current->resourceID == resourceID &&
            current->timeSlot == timeSlot) {
            slotCount++;
        }
        current = current->next;
    }
    return slotCount;
}

void ReservationList::displayReservations() const {
    if (head == nullptr) {
        std::cout
            << "No active reservations.\n";
        return;
    }
    std::cout
        << "\n--- Active Reservations ---\n";
    ReservationNode* current = head;
    while (current != nullptr) {
        std::cout
            << "Reservation ID: "
            << current->reservationID
            << " | Student: "
            << current->studentID
            << " | Resource: "
            << current->resourceID
            << " | Time Slot: "
            << current->timeSlot
            << "\n";
        current = current->next;
    }
}

int ReservationList::getCount() const {
    return count;
}

// NEW: walk the linked list once and copy each node into a vector.
// The list is newest-first (insertion is at the head), so the vector is too.
std::vector<ReservationNode> ReservationList::getAllReservations() const {
    std::vector<ReservationNode> all;
    ReservationNode* current = head;
    while (current != nullptr) {
        ReservationNode copy = *current;
        copy.next = nullptr;          // the copy must not point into the live list
        all.push_back(copy);
        current = current->next;
    }
    return all;
}

// NEW: used to reject a student booking the same resource/time slot twice.
bool ReservationList::hasReservation(
    const std::string& studentID,
    const std::string& resourceID,
    const std::string& timeSlot) const {
    ReservationNode* current = head;
    while (current != nullptr) {
        if (current->studentID == studentID &&
            current->resourceID == resourceID &&
            current->timeSlot == timeSlot) {
            return true;
        }
        current = current->next;
    }
    return false;
}
