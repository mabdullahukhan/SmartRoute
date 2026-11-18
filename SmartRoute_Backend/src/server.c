
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "models.h"
#include "data.h"
#include "route.h"

#define SERVER_PORT 8080
#define REQUEST_SIZE 8192
#define RESPONSE_SIZE 65536

static int sendAll(SOCKET client, const char *data, int length)
{
    int totalSent = 0;

    while (totalSent < length)
    {
        int sent = send(client, data + totalSent,
                        length - totalSent, 0);

        if (sent == SOCKET_ERROR || sent == 0)
            return 0;

        totalSent += sent;
    }

    return 1;
}

static void sendResponse(SOCKET client, const char *status,
                         const char *contentType, const char *body)
{
    char header[512];

    int bodyLength = (int)strlen(body);

    snprintf(header, sizeof(header),
             "HTTP/1.1 %s\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %d\r\n"
             "Connection: close\r\n"
             "Cache-Control: no-store\r\n"
             "\r\n",
             status, contentType, bodyLength);

    sendAll(client, header, (int)strlen(header));
    sendAll(client, body, bodyLength);
}

static void sendJson(SOCKET client, const char *json)
{
    sendResponse(client, "200 OK", "application/json; charset=utf-8", json);
}

static int getQueryInt(const char *target, const char *key, int *value)
{
    const char *question = strchr(target, '?');

    if (question == NULL)
        return 0;

    question++;

    char query[2048];
    snprintf(query, sizeof(query), "%s", question);

    char *token = strtok(query, "&");

    while (token != NULL)
    {
        char expected[64];
        snprintf(expected, sizeof(expected), "%s=", key);

        if (strncmp(token, expected, strlen(expected)) == 0)
        {
            char *number = token + strlen(expected);
            char *end = NULL;

            long parsed = strtol(number, &end, 10);

            if (number == end || *end != '\0')
                return 0;

            if (parsed < -2147483647L || parsed > 2147483647L)
                return 0;

            *value = (int)parsed;
            return 1;
        }

        token = strtok(NULL, "&");
    }

    return 0;
}

static int appendText(char *buffer, size_t capacity,
                      size_t *used, const char *format, ...)
{
    if (*used >= capacity)
        return 0;

    va_list args;
    va_start(args, format);

    int written = vsnprintf(buffer + *used,
                            capacity - *used,
                            format, args);

    va_end(args);

    if (written < 0 || (size_t)written >= capacity - *used)
        return 0;

    *used += (size_t)written;
    return 1;
}

static void sendError(SOCKET client, const char *message)
{
    char json[512];

    snprintf(json, sizeof(json),
             "{\"success\":false,\"message\":\"%s\"}", message);

    sendResponse(client, "400 Bad Request",
                 "application/json; charset=utf-8", json);
}

static void handleRoutes(SOCKET client, const char *target)
{
    int source, destination, maxStops, vehicleId;

    if (!getQueryInt(target, "source", &source) ||
        !getQueryInt(target, "destination", &destination) ||
        !getQueryInt(target, "stops", &maxStops) ||
        !getQueryInt(target, "vehicle", &vehicleId))
    {
        sendError(client, "Missing or invalid route parameters.");
        return;
    }

    if (!isValidLocationId(source) ||
        !isValidLocationId(destination) ||
        source == destination ||
        maxStops < 0 || maxStops >= MAX_LOCATIONS ||
        !isValidVehicleId(vehicleId))
    {
        sendError(client, "Please provide valid route settings.");
        return;
    }

    Route routes[MAX_ROUTES];

    int routeCount = findRoutes(source, destination,
                                maxStops, routes, MAX_ROUTES);

    if (routeCount <= 0)
    {
        sendJson(client,
                 "{\"success\":true,\"count\":0,\"routes\":[],"
                 "\"message\":\"No available routes found. Check road closures or increase the maximum stops.\"}");
        return;
    }

    sortRoutesByDistance(routes, routeCount);

    const Vehicle *vehicle = &vehicles[vehicleId - 1];

    char json[RESPONSE_SIZE];
    size_t used = 0;

    if (!appendText(json, sizeof(json), &used,
                    "{\"success\":true,\"count\":%d,\"routes\":[",
                    routeCount))
    {
        sendError(client, "Could not prepare route results.");
        return;
    }

    for (int i = 0; i < routeCount; i++)
    {
        Route *route = &routes[i];

        if (i > 0)
        {
            if (!appendText(json, sizeof(json), &used, ","))
                break;
        }

        if (!appendText(json, sizeof(json), &used,
                        "{\"number\":%d,"
                        "\"distance\":%.2f,"
                        "\"baseTime\":%.2f,"
                        "\"travelTime\":%.2f,"
                        "\"fuel\":%.2f,"
                        "\"locations\":[",
                        i + 1,
                        route->totalDistanceKm,
                        route->totalBaseTimeMin,
                        route->totalTravelTimeMin,
                        calculateFuelUsed(route->totalDistanceKm,
                                          vehicle->mileageKmPerLitre)))
        {
            sendError(client, "Route response is too large.");
            return;
        }

        for (int j = 0; j < route->locationCount; j++)
        {
            int locationId = route->locationIds[j];

            if (j > 0)
            {
                if (!appendText(json, sizeof(json), &used, ","))
                {
                    sendError(client, "Route response is too large.");
                    return;
                }
            }

            if (!appendText(json, sizeof(json), &used,
                            "{\"id\":%d,\"name\":\"%s\"}",
                            locationId, locations[locationId].name))
            {
                sendError(client, "Route response is too large.");
                return;
            }
        }

        if (!appendText(json, sizeof(json), &used, "]}"))
        {
            sendError(client, "Route response is too large.");
            return;
        }
    }

    if (!appendText(json, sizeof(json), &used, "]}"))
    {
        sendError(client, "Route response is too large.");
        return;
    }

    sendJson(client, json);
}

static void handleRoads(SOCKET client)
{
    char json[RESPONSE_SIZE];
    size_t used = 0;
    int first = 1;

    appendText(json, sizeof(json), &used,
               "{\"success\":true,\"roads\":[");

    for (int i = 0; i < MAX_LOCATIONS; i++)
    {
        for (int j = i + 1; j < MAX_LOCATIONS; j++)
        {
            if (!roads[i][j].exists)
                continue;

            if (!first)
                appendText(json, sizeof(json), &used, ",");

            appendText(json, sizeof(json), &used,
                       "{\"from\":%d,\"to\":%d,"
                       "\"fromName\":\"%s\",\"toName\":\"%s\","
                       "\"distance\":%.2f,\"condition\":%d,\"closed\":%d}",
                       i, j,
                       locations[i].name, locations[j].name,
                       roads[i][j].distanceKm,
                       (int)roads[i][j].condition,
                       roads[i][j].isClosed);

            first = 0;
        }
    }

    appendText(json, sizeof(json), &used, "]}");
    sendJson(client, json);
}

static void handleRoadUpdate(SOCKET client, const char *target)
{
    int from, to, condition, closed;

    if (!getQueryInt(target, "from", &from) ||
        !getQueryInt(target, "to", &to) ||
        !getQueryInt(target, "condition", &condition) ||
        !getQueryInt(target, "closed", &closed))
    {
        sendError(client, "Missing or invalid road settings.");
        return;
    }

    if (!isValidLocationId(from) ||
        !isValidLocationId(to) ||
        from == to ||
        condition < ROAD_NORMAL ||
        condition > ROAD_CONGESTED ||
        (closed != 0 && closed != 1) ||
        !roads[from][to].exists)
    {
        sendError(client, "The selected road or condition is invalid.");
        return;
    }

    /* Update both directions of the two-way road. */
    roads[from][to].condition = (RoadCondition)condition;
    roads[to][from].condition = (RoadCondition)condition;

    roads[from][to].isClosed = closed;
    roads[to][from].isClosed = closed;

    sendJson(client,
             "{\"success\":true,\"message\":\"Road settings updated successfully.\"}");
}

static void handleReport(SOCKET client, const char *target)
{
    int source, destination, maxStops, vehicleId;

    if (!getQueryInt(target, "source", &source) ||
        !getQueryInt(target, "destination", &destination) ||
        !getQueryInt(target, "stops", &maxStops) ||
        !getQueryInt(target, "vehicle", &vehicleId))
    {
        sendError(client, "Missing report settings.");
        return;
    }

    if (!isValidLocationId(source) ||
        !isValidLocationId(destination) ||
        source == destination ||
        maxStops < 0 || maxStops >= MAX_LOCATIONS ||
        !isValidVehicleId(vehicleId))
    {
        sendError(client, "Invalid report settings.");
        return;
    }

    Route routes[MAX_ROUTES];

    int routeCount = findRoutes(source, destination,
                                maxStops, routes, MAX_ROUTES);

    if (routeCount <= 0)
    {
        sendError(client, "No routes available to save.");
        return;
    }

    sortRoutesByDistance(routes, routeCount);

    int saved = saveRouteReport(routes, routeCount,
                                &vehicles[vehicleId - 1],
                                source, destination, maxStops);

    if (!saved)
    {
        sendResponse(client, "500 Internal Server Error",
                     "application/json; charset=utf-8",
                     "{\"success\":false,\"message\":\"Could not save the report.\"}");
        return;
    }

    sendJson(client,
             "{\"success\":true,\"message\":\"Report saved to reports/SmartRoute_Report.txt\"}");
}

static void serveStaticFile(SOCKET client, const char *path)
{
    const char *filePath = NULL;
    const char *contentType = "text/plain; charset=utf-8";

    if (strcmp(path, "/") == 0 || strcmp(path, "/index.html") == 0)
    {
        filePath = "SmartRoute_Frontend/index.html";
        contentType = "text/html; charset=utf-8";
    }
    else if (strcmp(path, "/styles.css") == 0)
    {
        filePath = "SmartRoute_Frontend/styles.css";
        contentType = "text/css; charset=utf-8";
    }
    else if (strcmp(path, "/app.js") == 0)
    {
        filePath = "SmartRoute_Frontend/app.js";
        contentType = "application/javascript; charset=utf-8";
    }

    if (filePath == NULL)
    {
        sendResponse(client, "404 Not Found",
                     "text/plain; charset=utf-8",
                     "File not found.");
        return;
    }

    FILE *file = fopen(filePath, "rb");

    if (file == NULL)
    {
        sendResponse(client, "500 Internal Server Error",
                     "text/plain; charset=utf-8",
                     "Could not open SmartRoute_Frontend/ file. Run the server from the project root folder.");
        return;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    if (fileSize < 0 || fileSize > 2000000)
    {
        fclose(file);
        sendResponse(client, "500 Internal Server Error",
                     "text/plain; charset=utf-8",
                     "Invalid file size.");
        return;
    }

    char *content = malloc((size_t)fileSize + 1);

    if (content == NULL)
    {
        fclose(file);
        sendResponse(client, "500 Internal Server Error",
                     "text/plain; charset=utf-8",
                     "Memory allocation failed.");
        return;
    }

    size_t bytesRead = fread(content, 1, (size_t)fileSize, file);
    fclose(file);

    char header[512];

    snprintf(header, sizeof(header),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %zu\r\n"
             "Connection: close\r\n"
             "Cache-Control: no-store\r\n"
             "\r\n",
             contentType, bytesRead);

    sendAll(client, header, (int)strlen(header));
    sendAll(client, content, (int)bytesRead);

    free(content);
}

static void handleClient(SOCKET client)
{
    char request[REQUEST_SIZE];

    int received = recv(client, request, sizeof(request) - 1, 0);

    if (received <= 0)
        return;

    request[received] = '\0';

    char method[16];
    char target[2048];

    if (sscanf(request, "%15s %2047s", method, target) != 2)
    {
        sendResponse(client, "400 Bad Request",
                     "text/plain; charset=utf-8",
                     "Invalid HTTP request.");
        return;
    }

    if (strcmp(method, "GET") != 0)
    {
        sendResponse(client, "405 Method Not Allowed",
                     "text/plain; charset=utf-8",
                     "Only GET requests are supported.");
        return;
    }

    char path[2048];
    snprintf(path, sizeof(path), "%s", target);

    char *question = strchr(path, '?');

    if (question != NULL)
        *question = '\0';

    if (strcmp(path, "/api/health") == 0)
    {
        sendJson(client,
                 "{\"success\":true,\"message\":\"C17 backend is running.\"}");
    }
    else if (strcmp(path, "/api/routes") == 0)
    {
        handleRoutes(client, target);
    }
    else if (strcmp(path, "/api/roads") == 0)
    {
        handleRoads(client);
    }
    else if (strcmp(path, "/api/road/update") == 0)
    {
        handleRoadUpdate(client, target);
    }
    else if (strcmp(path, "/api/report") == 0)
    {
        handleReport(client, target);
    }
    else
    {
        serveStaticFile(client, path);
    }
}

int main(void)
{
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("Winsock initialization failed.\n");
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (serverSocket == INVALID_SOCKET)
    {
        printf("Could not create server socket.\n");
        WSACleanup();
        return 1;
    }

    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVER_PORT);
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(serverSocket, (struct sockaddr *)&serverAddress,
             sizeof(serverAddress)) == SOCKET_ERROR)
    {
        printf("Could not use port %d. It may already be in use.\n",
               SERVER_PORT);
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        printf("Could not start listening.\n");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    initializeRoadNetwork();

    printf("\n========================================\n");
    printf("       KARACHI SMARTROUTE SERVER\n");
    printf("========================================\n");
    printf("C17 backend: ONLINE\n");
    printf("Road network: INITIALIZED\n");
    printf("SmartRoute_Frontend/: http://127.0.0.1:%d\n", SERVER_PORT);
    printf("\nKeep this terminal open while using SmartRoute.\n");
    printf("Press CTRL+C to stop the server.\n\n");

    while (1)
    {
        SOCKET client = accept(serverSocket, NULL, NULL);

        if (client == INVALID_SOCKET)
            continue;

        handleClient(client);
        closesocket(client);
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
