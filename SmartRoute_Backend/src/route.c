#include <time.h>
#include <stdio.h>
#include "data.h"
#include "route.h"
#include <time.h>

/* Return the simulated traffic multiplier */
static double getTrafficMultiplier(RoadCondition condition)
{
    switch (condition) {

        case ROAD_NORMAL:
            return 1.0;

        case ROAD_BUSY:
            return 1.5;

        case ROAD_CONGESTED:
            return 2.0;

        default:
            return 1.0;
    }
}

double calculateFuelUsed(
    double distanceKm,
    double mileageKmPerLitre
)
{
    if (distanceKm < 0 || mileageKmPerLitre <= 0) {
        return 0.0;
    }

    return distanceKm / mileageKmPerLitre;
}

static void dfs(
    int current,
    int destination,
    int maxStops,
    int path[],
    int pathLength,
    int visited[],
    Route routes[],
    int *routeCount,
    int capacity
)
{
    if (*routeCount >= capacity) {
        return;
    }

    if (current == destination) {

        Route *route = &routes[*routeCount];

        route->locationCount = pathLength;
        route->totalDistanceKm = 0.0;
        route->totalBaseTimeMin = 0.0;
        route->totalTravelTimeMin = 0.0;

        for (int i = 0; i < pathLength; i++) {
            route->locationIds[i] = path[i];
        }

        for (int i = 0; i < pathLength - 1; i++) {

            int from = path[i];
            int to = path[i + 1];

            Road road = roads[from][to];

            route->totalDistanceKm += road.distanceKm;

            route->totalBaseTimeMin += road.baseTimeMin;

            route->totalTravelTimeMin +=
                road.baseTimeMin *
                getTrafficMultiplier(road.condition);
        }

        (*routeCount)++;
        return;
    }

    for (int next = 0; next < MAX_LOCATIONS; next++) {

        if (*routeCount >= capacity) {
            return;
        }

        if (!roads[current][next].exists) {
            continue;
        }

        if (roads[current][next].isClosed) {
            continue;
        }

        if (visited[next]) {
            continue;
        }

        if (next != destination &&
            pathLength - 1 >= maxStops) {
            continue;
        }

        visited[next] = 1;
        path[pathLength] = next;

        dfs(
            next,
            destination,
            maxStops,
            path,
            pathLength + 1,
            visited,
            routes,
            routeCount,
            capacity
        );

        visited[next] = 0;
    }
}

int findRoutes(
    int source,
    int destination,
    int maxStops,
    Route routes[],
    int capacity
)
{
    if (!isValidLocationId(source) ||
        !isValidLocationId(destination) ||
        source == destination ||
        maxStops < 0 ||
        routes == NULL ||
        capacity <= 0) {

        return 0;
    }

    int path[MAX_LOCATIONS] = {0};
    int visited[MAX_LOCATIONS] = {0};
    int routeCount = 0;

    path[0] = source;
    visited[source] = 1;

    dfs(
        source,
        destination,
        maxStops,
        path,
        1,
        visited,
        routes,
        &routeCount,
        capacity
    );

    return routeCount;
}

void displayRoutes(
    const Route routes[],
    int routeCount,
    const Vehicle *selectedVehicle
)
{
    if (routeCount == 0) {
        printf("\nNo routes found within the stop limit.\n");
        return;
    }

    if (selectedVehicle == NULL) {
        printf("\nVehicle information unavailable.\n");
        return;
    }

    printf("\n==============================================\n");
    printf("             AVAILABLE ROUTES                \n");
    printf("==============================================\n");

    printf("\nSelected vehicle: %s\n", selectedVehicle->name);
    printf("Vehicle mileage: %.2f km/L\n",
           selectedVehicle->mileageKmPerLitre);

    for (int i = 0; i < routeCount; i++) {

        printf("\nRoute %d:\n", i + 1);

        for (int j = 0; j < routes[i].locationCount; j++) {

            printf("%s",
                locations[routes[i].locationIds[j]].name);

            if (j < routes[i].locationCount - 1) {
                printf(" -> ");
            }
        }

        printf("\nDistance: %.2f km",
               routes[i].totalDistanceKm);

        printf("\nBase travel time: %.2f minutes",
               routes[i].totalBaseTimeMin);

        printf("\nEstimated travel time: %.2f minutes",
               routes[i].totalTravelTimeMin);

        printf("\nEstimated fuel consumption: %.2f litres\n",
               calculateFuelUsed(
                   routes[i].totalDistanceKm,
                   selectedVehicle->mileageKmPerLitre
               ));
    }

    printf("\nTotal routes found: %d\n", routeCount);
    printf("==============================================\n");
}

void sortRoutesByDistance(Route routes[], int routeCount)
{
    for (int i = 0; i < routeCount - 1; i++) {

        for (int j = 0; j < routeCount - i - 1; j++) {

            if (routes[j].totalDistanceKm >
                routes[j + 1].totalDistanceKm) {

                Route temp = routes[j];
                routes[j] = routes[j + 1];
                routes[j + 1] = temp;
            }
        }
    }
}

void sortRoutesByTime(Route routes[], int routeCount)
{
    for (int i = 0; i < routeCount - 1; i++) {

        for (int j = 0; j < routeCount - i - 1; j++) {

            if (routes[j].totalTravelTimeMin >
                routes[j + 1].totalTravelTimeMin) {

                Route temp = routes[j];
                routes[j] = routes[j + 1];
                routes[j + 1] = temp;
            }
        }
    }
}


int saveRouteReport(
    const Route routes[],
    int routeCount,
    const Vehicle *selectedVehicle,
    int source,
    int destination,
    int maxStops
)
{
    FILE *file = fopen("reports/SmartRoute_Report.txt", "a");

    if (file == NULL) {
        printf("\nError: Could not open report file.\n");
        printf("Make sure the reports folder exists.\n");
        return 0;
    }

    time_t currentTime = time(NULL);
    struct tm *localTime = localtime(&currentTime);
    char dateTime[50] = "Date/time unavailable";

    if (localTime != NULL) {
        strftime(
            dateTime,
            sizeof(dateTime),
            "%d-%m-%Y %H:%M:%S",
            localTime
        );
    }

    fprintf(file, "\n==============================================\n");
    fprintf(file, "              SMARTROUTE REPORT               \n");
    fprintf(file, "==============================================\n");

    fprintf(file, "Generated: %s\n", dateTime);

    fprintf(file, "Source: %s\n", locations[source].name);
    fprintf(file, "Destination: %s\n", locations[destination].name);
    fprintf(file, "Maximum intermediate stops: %d\n", maxStops);

    if (selectedVehicle != NULL) {
        fprintf(file, "Vehicle: %s\n", selectedVehicle->name);
        fprintf(
            file,
            "Vehicle mileage: %.2f km/L\n",
            selectedVehicle->mileageKmPerLitre
        );
    }

    fprintf(file, "\nTotal routes found: %d\n", routeCount);

    if (selectedVehicle != NULL) {
        for (int i = 0; i < routeCount; i++) {

            fprintf(file, "\nRoute %d:\n", i + 1);

            for (int j = 0; j < routes[i].locationCount; j++) {

                fprintf(
                    file,
                    "%s",
                    locations[routes[i].locationIds[j]].name
                );

                if (j < routes[i].locationCount - 1) {
                    fprintf(file, " -> ");
                }
            }

            fprintf(
                file,
                "\nDistance: %.2f km\n",
                routes[i].totalDistanceKm
            );

            fprintf(
                file,
                "Base travel time: %.2f minutes\n",
                routes[i].totalBaseTimeMin
            );

            fprintf(
                file,
                "Estimated travel time: %.2f minutes\n",
                routes[i].totalTravelTimeMin
            );

            fprintf(
                file,
                "Estimated fuel consumption: %.2f litres\n",
                calculateFuelUsed(
                    routes[i].totalDistanceKm,
                    selectedVehicle->mileageKmPerLitre
                )
            );
        }
    }

    fprintf(file, "==============================================\n\n");

    if (fclose(file) != 0) {
        printf("\nError: Could not properly close report file.\n");
        return 0;
    }

    printf("\nReport saved successfully!\n");
    printf("File: reports/SmartRoute_Report.txt\n");

    return 1;
}

