
#ifndef ROUTE_H
#define ROUTE_H

#include "models.h"

int findRoutes(
    int source,
    int destination,
    int maxStops,
    Route routes[],
    int capacity
);

double calculateFuelUsed(
    double distanceKm,
    double mileageKmPerLitre
);

void displayRoutes(
    const Route routes[],
    int routeCount,
    const Vehicle *selectedVehicle
);

void sortRoutesByDistance(Route routes[], int routeCount);
void sortRoutesByTime(Route routes[], int routeCount);

int saveRouteReport(
    const Route routes[],
    int routeCount,
    const Vehicle *selectedVehicle,
    int source,
    int destination,
    int maxStops
);

int saveRouteReport(
    const Route routes[],
    int routeCount,
    const Vehicle *selectedVehicle,
    int source,
    int destination,
    int maxStops
);
#endif
