#include <string.h>
#include "data.h"

const Location locations[MAX_LOCATIONS] = {
    {0, "NED University"},
    {1, "Gulshan-e-Iqbal"},
    {2, "University Road"},
    {3, "Bahadurabad"},
    {4, "Shahrah-e-Faisal"},
    {5, "Saddar"},
    {6, "Clifton"},
    {7, "Karachi Airport"}
};

const Vehicle vehicles[3] = {
    {1, "Car", 14.0},
    {2, "Motorcycle", 40.0},
    {3, "Bus", 5.0}
};

Road roads[MAX_LOCATIONS][MAX_LOCATIONS];

static void addDirectedRoad(int from, int to, double distanceKm, double timeMin)
{
    roads[from][to].exists = 1;
    roads[from][to].distanceKm = distanceKm;
    roads[from][to].baseTimeMin = timeMin;
    roads[from][to].condition = ROAD_NORMAL;
    roads[from][to].isClosed = 0;
}

static void addTwoWayRoad(int first, int second, double distanceKm, double timeMin)
{
    addDirectedRoad(first, second, distanceKm, timeMin);
    addDirectedRoad(second, first, distanceKm, timeMin);
}

void initializeRoadNetwork(void)
{
    memset(roads, 0, sizeof(roads));

    /* Sample network only: distances and times are illustrative. */
    addTwoWayRoad(0, 1, 3.0, 10.0);
    addTwoWayRoad(0, 2, 4.0, 12.0);
    addTwoWayRoad(1, 2, 2.0, 6.0);
    addTwoWayRoad(1, 3, 5.0, 14.0);
    addTwoWayRoad(2, 3, 4.0, 11.0);
    addTwoWayRoad(2, 4, 7.0, 18.0);
    addTwoWayRoad(3, 4, 3.0, 8.0);
    addTwoWayRoad(3, 5, 6.0, 16.0);
    addTwoWayRoad(4, 5, 5.0, 13.0);
    addTwoWayRoad(4, 7, 8.0, 17.0);
    addTwoWayRoad(5, 6, 4.0, 12.0);
    addTwoWayRoad(6, 7, 12.0, 25.0);
    addTwoWayRoad(1, 7, 15.0, 30.0);
}

int isValidLocationId(int locationId)
{
    return locationId >= 0 && locationId < MAX_LOCATIONS;
}

int isValidVehicleId(int vehicleId)
{
    return vehicleId >= 1 && vehicleId <= 3;
}
