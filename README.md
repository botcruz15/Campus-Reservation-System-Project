# Campus Reservation System Project
CSCE 2110 Project 1 - CRRS (Campus Reservation Resource System)

## Complexity Analysis Report

### Reservation Insertion — O(1)
A new reservation is inserted at the beginning of the linked list. Since the list does not need to be traversed, insertion takes constant time.

### Reservation Removal — O(n)
Removing a reservation may require traversing the linked list to find the matching reservation. In the worst case, the program may need to check every reservation in the list.

### Waiting List Processing — O(n)
Students are added to the rear of the queue in O(1) time. When a reservation is cancelled, the waiting list may need to be traversed to find the first student waiting for the same resource and time slot. In the worst case, this takes O(n) time.

### Undo Cancellation — O(n)
The cancellation history uses a stack, so accessing and removing the most recently cancelled reservation with a pop operation takes O(1) time. However, before restoring the reservation, the program checks the active reservation linked list to make sure the original resource and time slot still have space available. This check may require traversing the linked list, making the complete undo operation O(n) in the worst case.
