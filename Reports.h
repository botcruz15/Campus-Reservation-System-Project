#ifndef REPORTS_H
#define REPORTS_H

#include "Resource.h"
#include "ReservationList.h"

// Every report calculates its numbers from the live data structures when it is called.

void reportActiveReservations(const ReservationList& reservations);

void reportResourceUtilization(const ResourceManager& resources,
                               const ReservationList& reservations);

void reportMostRequestedResources(const ResourceManager& resources);

#endif
