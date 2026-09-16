# Campus Reservation System Project
CSCE 2110 Project 1-CRRS(Campus Reservatiton Resource system) 
## Complexity analysis report

### Reservation Insertion—O(1)
A new reservation is inserted at the beginning of the linked list. Since the list does not need to be traversed, insertion takes constant time.

### Reservation Removal—O(n)
Removing a reservation may require traversing the linked list to find the matching reservation.

### Waiting List Processing—O(1)
The waiting list uses a queue. Students are added to the rear and removed from the front, so these operations take constant time.

### Undo Cancellation—O(1)
Cancellation history uses a stack. The most recently cancelled reservation is stored at the top and it can be restored using a pop operation in constant time.
