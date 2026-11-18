
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>

#include "data.h"
#include "route.h"

/* Safely read an integer within an allowed range. */
static int readInt(const char *prompt, int min, int max, int *value)
{
    char input[100];
    char *endPointer;
    long number;

    while (1) {

        printf("%s", prompt);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            return 0;
        }

        /* Detect an excessively long input line. */
        if (strchr(input, '\n') == NULL && !feof(stdin)) {

            int ch;

            while ((ch = getchar()) != '\n' && ch != EOF) {
                /* Discard remaining characters. */
            }

            printf("Input is too long. Try again.\n");
            continue;
        }

        errno = 0;
        number = strtol(input, &endPointer, 10);

        if (endPointer == input || errno == ERANGE) {
            printf("Invalid input. Enter a valid number.\n");
            continue;
        }

        while (isspace((unsigned char)*endPointer)) {
            endPointer++;
        }

        if (*endPointer != '\0') {
            printf("Invalid input. Enter numbers only.\n");
            continue;
        }

        if (number < min || number > max) {
            printf("Please enter a value between %d and %d.\n",
                   min, max);
            continue;
        }

        *value = (int)number;
        return 1;
    }
}

/* Safely read Y/N input. */
static int readYesNo(const char *prompt, char *choice)
{
    char input[100];

    while (1) {

        printf("%s", prompt);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            return 0;
        }

        if (strchr(input, '\n') == NULL && !feof(stdin)) {

            int ch;

            while ((ch = getchar()) != '\n' && ch != EOF) {
                /* Discard remaining characters. */
            }

            printf("Input is too long. Enter Y or N.\n");
            continue;
        }

        char *start = input;

        while (isspace((unsigned char)*start)) {
            start++;
        }

        char selected = *start;

        if (selected == 'Y' || selected == 'y' ||
            selected == 'N' || selected == 'n') {

            start++;

            while (isspace((unsigned char)*start)) {
                start++;
            }

            if (*start == '\0') {
                *choice = selected;
                return 1;
            }
        }

        printf("Invalid input. Enter Y or N.\n");
    }
}

static void displayRoads(void)
{
    printf("\n==============================================\n");
    printf("             AVAILABLE ROADS                 \n");
    printf("==============================================\n");

    for (int i = 0; i < MAX_LOCATIONS; i++) {

        for (int j = i + 1; j < MAX_LOCATIONS; j++) {

            if (roads[i][j].exists) {

                printf("%d. %s <-> %s | %.1f km\n",
                       i,
                       locations[i].name,
                       locations[j].name,
                       roads[i][j].distanceKm);
            }
        }
    }
}

static int manageTraffic(void)
{
    int choice;

    while (1) {

        displayRoads();

        printf("\n========== TRAFFIC MANAGEMENT ==============\n");
        printf("1. Change traffic condition\n");
        printf("2. Close a road\n");
        printf("3. Reopen a road\n");
        printf("0. Continue to route search\n");

        if (!readInt("Enter choice: ", 0, 3, &choice)) {
            return 0;
        }

        if (choice == 0) {
            return 1;
        }

        int from, to;

        if (!readInt("Enter first location ID (0-7): ",
                     0, MAX_LOCATIONS - 1, &from)) {
            return 0;
        }

        if (!readInt("Enter second location ID (0-7): ",
                     0, MAX_LOCATIONS - 1, &to)) {
            return 0;
        }

        if (from == to || !roads[from][to].exists) {
            printf("No direct road exists between these locations.\n");
            continue;
        }

        if (choice == 1) {

            int condition;

            printf("\n1. Normal\n");
            printf("2. Busy\n");
            printf("3. Congested\n");

            if (!readInt("Select traffic condition: ",
                         1, 3, &condition)) {
                return 0;
            }

            RoadCondition selectedCondition;

            switch (condition) {

                case 1:
                    selectedCondition = ROAD_NORMAL;
                    break;

                case 2:
                    selectedCondition = ROAD_BUSY;
                    break;

                default:
                    selectedCondition = ROAD_CONGESTED;
                    break;
            }

            roads[from][to].condition = selectedCondition;
            roads[to][from].condition = selectedCondition;

            printf("Traffic condition updated successfully.\n");
        }

        else if (choice == 2) {

            if (roads[from][to].isClosed) {
                printf("This road is already closed.\n");
                continue;
            }

            roads[from][to].isClosed = 1;
            roads[to][from].isClosed = 1;

            printf("Road closed successfully.\n");
        }

        else if (choice == 3) {

            if (!roads[from][to].isClosed) {
                printf("This road is already open.\n");
                continue;
            }

            roads[from][to].isClosed = 0;
            roads[to][from].isClosed = 0;

            printf("Road reopened successfully.\n");
        }
    }
}

int main(void)
{
    initializeRoadNetwork();

    printf("==============================================\n");
    printf("                 SMARTROUTE                   \n");
    printf("       INTELLIGENT ROUTE PLANNING SYSTEM      \n");
    printf("==============================================\n\n");

    printf("Backend foundation initialized successfully.\n\n");

    printf("Available locations:\n");

    for (int i = 0; i < MAX_LOCATIONS; i++) {
        printf("  %d. %s\n", locations[i].id, locations[i].name);
    }

    printf("\nAvailable vehicles:\n");

    for (int i = 0; i < 3; i++) {
        printf("  %d. %-12s Mileage: %.1f km/L\n",
               vehicles[i].id,
               vehicles[i].name,
               vehicles[i].mileageKmPerLitre);
    }

    printf("\nRoad network: initialized\n");
    printf("==============================================\n");

    if (!manageTraffic()) {
        printf("\nInput ended. Program terminated safely.\n");
        return 0;
    }

    int source, destination, maxStops;
    int vehicleChoice;
    int sortChoice;

    printf("\n              ROUTE SEARCH                   \n");

    if (!readInt("\nEnter source location ID (0-7): ",
                 0, MAX_LOCATIONS - 1, &source)) {
        return 0;
    }

    while (1) {

        if (!readInt("Enter destination location ID (0-7): ",
                     0, MAX_LOCATIONS - 1, &destination)) {
            return 0;
        }

        if (destination == source) {
            printf("Source and destination cannot be the same.\n");
            continue;
        }

        break;
    }

    if (!readInt("Enter maximum intermediate stops (0-6): ",
                 0, MAX_LOCATIONS - 2, &maxStops)) {
        return 0;
    }

    printf("\nSELECT YOUR VEHICLE:\n");

    for (int i = 0; i < 3; i++) {
        printf("%d. %-12s %.1f km/L\n",
               vehicles[i].id,
               vehicles[i].name,
               vehicles[i].mileageKmPerLitre);
    }

    if (!readInt("Enter vehicle ID (1-3): ",
                 1, 3, &vehicleChoice)) {
        return 0;
    }

    const Vehicle *selectedVehicle = &vehicles[vehicleChoice - 1];

    Route routes[MAX_ROUTES];

    int routeCount = findRoutes(
        source,
        destination,
        maxStops,
        routes,
        MAX_ROUTES
    );

    if (routeCount > 0) {

        printf("\nSORT ROUTES BY:\n");
        printf("1. Shortest Distance\n");
        printf("2. Fastest Travel Time\n");

        if (!readInt("Enter choice: ", 1, 2, &sortChoice)) {
            return 0;
        }

        if (sortChoice == 1) {
            sortRoutesByDistance(routes, routeCount);
            printf("\nRoutes ranked by shortest distance.\n");
        }

        else {
            sortRoutesByTime(routes, routeCount);
            printf("\nRoutes ranked by fastest travel time.\n");
        }
    }

    displayRoutes(routes, routeCount, selectedVehicle);

    char saveChoice;

    if (!readYesNo(
            "\nWould you like to save this route report? (Y/N): ",
            &saveChoice)) {
        return 0;
    }

    if (saveChoice == 'Y' || saveChoice == 'y') {

        saveRouteReport(
            routes,
            routeCount,
            selectedVehicle,
            source,
            destination,
            maxStops
        );

    } else {
        printf("\nReport was not saved.\n");
    }

    return 0;
}
