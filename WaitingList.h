#ifndef WAITING_LIST_H
#define WAITING_LIST_H

#include <string>
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

  void enqueue(const std::string& studentID,
               const std::string& resourceID,
               const std::string& timeSlot);

  bool dequeue(WaitingNode& removedStudent);

  void displayWaitingList() const;

  bool isEmpty() const;

  int getCount() const;
};

#endif
