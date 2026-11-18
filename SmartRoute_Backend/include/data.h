#ifndef DATA_H
#define DATA_H

#include "models.h"

extern const Location locations[MAX_LOCATIONS];
extern const Vehicle vehicles[3];
extern Road roads[MAX_LOCATIONS][MAX_LOCATIONS];

void initializeRoadNetwork(void);
int isValidLocationId(int locationId);
int isValidVehicleId(int vehicleId);

#endif
