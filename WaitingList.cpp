#include "WaitingList.h"
#include <iostream>

WaitingList::WaitingList() : front(nullptr), rear(nullptr), count(0) {
}

WaitingList::~WaitingList() {
    WaitingNode* current = front;

    while (current != nullptr) {
        WaitingNode* toDelete = current;
        current = current->next;
        delete toDelete;
    }

    front = nullptr;
    rear = nullptr;
}
void WaitingList::enqueue(const std::string& studentID,
                          const std::string& resourceID,
                          const std::string& timeSlot) {

    WaitingNode* newNode = new WaitingNode();

    newNode->studentID = studentID;
    newNode->resourceID = resourceID;
    newNode->timeSlot = timeSlot;
    newNode->next = nullptr;

    if (rear == nullptr) {
        front = newNode;
        rear = newNode;
    }
    else {
        rear->next = newNode;
        rear = newNode;
    }
      count++;
}
bool WaitingList::dequeue(WaitingNode& removedStudent) {
    if (front == nullptr) {
        return false;
    }

    WaitingNode* temp = front;

    removedStudent = *front;
    removedStudent.next = nullptr;

    front = front->next;

    if (front == nullptr) {
        rear = nullptr;
    }

    delete temp;
    count--;

    return true;
}
void WaitingList::displayWaitingList() const {
    if (front == nullptr) {
        std::cout << "No students are currently waiting.\n";
        return;
    }

    std::cout << "\n--- Waiting List ---\n";

    WaitingNode* current = front;
    int position = 1;

    while (current != nullptr) {
        std::cout << position << ". "
                  << "Student ID: " << current->studentID
                  << " | Resource: " << current->resourceID
                  << " | Time Slot: " << current->timeSlot
                  << "\n";

        current = current->next;
        position++;
    }
}

bool WaitingList::isEmpty() const {
    return front == nullptr;
}

int WaitingList::getCount() const {
    return count;
}
