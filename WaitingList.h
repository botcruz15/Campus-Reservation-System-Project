#ifndef WAITING_LIST_H
#define WAITING_LIST_H

#include <string>
#include <vector>

struct WaitingNode {
    std::string studentID;
    std::string resourceID;
    std::string timeSlot;
    WaitingNode* next;
};

class WaitingList {
private:
    WaitingNode* front;
    WaitingNode* rear;
    int count;

public:
    WaitingList();
    ~WaitingList();

    void enqueue(
        const std::string& studentID,
        const std::string& resourceID,
        const std::string& timeSlot
    );

    bool dequeue(
        WaitingNode& removedStudent
    );

    bool dequeueForResource(
        const std::string& resourceID,
        const std::string& timeSlot,
        WaitingNode& removedStudent
    );

    void displayWaitingList() const;

    bool isEmpty() const;

    int getCount() const;

    // NEW: copy the queue (front to rear) into a vector for the waiting-list report
    std::vector<WaitingNode> getAllWaiting() const;

    // NEW: true if this student is already waiting for this resource/time slot
    bool contains(
        const std::string& studentID,
        const std::string& resourceID,
        const std::string& timeSlot
    ) const;
};

#endif
