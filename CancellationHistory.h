#ifndef CANCELLATION_HISTORY_H
#define CANCELLATION_HISTORY_H

#include "ReservationList.h"

struct CancellationNode {
    ReservationNode reservation;
    CancellationNode* next;
};

class CancellationHistory {
private:
    CancellationNode* top;
    int count;

public:
    CancellationHistory();
    ~CancellationHistory();

    void pushCancellation(const ReservationNode& reservation);

    bool popCancellation(ReservationNode& restoredReservation);

    bool peekCancellation(ReservationNode& reservation) const;

    void displayHistory() const;

    bool isEmpty() const;
    int getCount() const;
};

#endif