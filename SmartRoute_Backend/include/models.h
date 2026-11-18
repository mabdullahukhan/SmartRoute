
#ifndef MODELS_H
#define MODELS_H

#define MAX_LOCATIONS 8
#define MAX_ROUTES 100

typedef enum {
    ROAD_NORMAL,
    ROAD_BUSY,
    ROAD_CONGESTED
} RoadCondition;

typedef struct {
    int id;
    char name[50];
} Location;

typedef struct {
    int id;
    char name[20];
    double mileageKmPerLitre;
} Vehicle;

typedef struct {
    int exists;
    double distanceKm;
    double baseTimeMin;
    RoadCondition condition;
    int isClosed;
} Road;

typedef struct {
    int locationIds[MAX_LOCATIONS];
    int locationCount;
    double totalDistanceKm;
    double totalBaseTimeMin;
    double totalTravelTimeMin;
} Route;

#endif
