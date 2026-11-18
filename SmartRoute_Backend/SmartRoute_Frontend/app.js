
// ============================================
// KARACHI SMARTROUTE - FRONTEND APPLICATION
// Dynamic Map + C17 Backend Integration
// ============================================


// --------------------------------------------
// 1. DOM ELEMENTS
// --------------------------------------------

const source = document.getElementById("source");
const destination = document.getElementById("destination");
const routeForm = document.getElementById("routeForm");
const notice = document.getElementById("resultNotice");

const vehicleSelect = document.getElementById("vehicle");
const stopsSelect = document.getElementById("stops");

const mapSvg = document.getElementById("roadMap");

let lastRequest = null;
let roadData = [];
let selectedRouteIds = [];


// --------------------------------------------
// 2. MAP LOCATION COORDINATES
// --------------------------------------------

const mapLocations = [
    { id: 0, name: "NED University", x: 105, y: 325 },
    { id: 1, name: "Gulshan-e-Iqbal", x: 245, y: 205 },
    { id: 2, name: "University Road", x: 365, y: 285 },
    { id: 3, name: "Bahadurabad", x: 485, y: 175 },
    { id: 4, name: "Shahrah-e-Faisal", x: 570, y: 315 },
    { id: 5, name: "Saddar", x: 680, y: 230 },
    { id: 6, name: "Clifton", x: 720, y: 365 },
    { id: 7, name: "Karachi Airport", x: 650, y: 75 }
];


// --------------------------------------------
// 3. DYNAMIC ROUTE RESULTS PANEL
// --------------------------------------------

const resultsPanel = document.createElement("section");

resultsPanel.className = "panel route-results-panel";
resultsPanel.id = "routeResults";

resultsPanel.innerHTML = `
    <div class="panel-heading">
        <div>
            <div class="eyebrow">C17 ROUTE ENGINE</div>
            <h2>Calculated routes</h2>
        </div>
        <span class="status-chip">BACKEND RESULTS</span>
    </div>

    <div id="routesOutput">
        <p class="panel-footnote">
            Choose your journey and select Find best routes.
        </p>
    </div>
`;

document.querySelector(".content-grid")
    .insertAdjacentElement("afterend", resultsPanel);


// --------------------------------------------
// 4. NOTIFICATION SYSTEM
// --------------------------------------------

function showNotice(title, message, type = "info") {

    const icon =
        type === "error" ? "!" :
        type === "success" ? "✓" : "i";

    notice.innerHTML = `
        <span class="note-icon">${icon}</span>

        <div>
            <strong>${escapeHTML(title)}</strong>
            <p>${escapeHTML(message)}</p>
        </div>
    `;
}


// --------------------------------------------
// 5. HTML ESCAPING
// --------------------------------------------

function escapeHTML(value) {

    return String(value).replace(/[&<>"']/g, character => ({
        "&": "&amp;",
        "<": "&lt;",
        ">": "&gt;",
        '"': "&quot;",
        "'": "&#039;"
    })[character]);

}


// --------------------------------------------
// 6. API REQUEST FUNCTION
// --------------------------------------------

async function apiRequest(path) {

    const response = await fetch(path);
    const data = await response.json();

    if (!response.ok || data.success === false) {
        throw new Error(
            data.message || "The server request failed."
        );
    }

    return data;
}


// --------------------------------------------
// 7. BACKEND CONNECTION CHECK
// --------------------------------------------

async function checkBackend() {

    try {

        const data = await apiRequest("/api/health");

        const systemText =
            document.querySelector(".system-card small");

        if (systemText) {
            systemText.textContent = "C backend connected";
        }

        showNotice(
            "C backend connected",
            data.message,
            "success"
        );

    } catch (error) {

        const systemText =
            document.querySelector(".system-card small");

        if (systemText) {
            systemText.textContent = "Backend disconnected";
        }

        showNotice(
            "Backend connection unavailable",
            "Start SmartRouteServer.exe and refresh this page.",
            "error"
        );

    }
}


// ============================================
// 8. DYNAMIC MAP ENGINE
// ============================================


// Create SVG elements safely.

function createSvgElement(tag, attributes = {}) {

    const element = document.createElementNS(
        "http://www.w3.org/2000/svg",
        tag
    );

    Object.entries(attributes).forEach(([key, value]) => {
        element.setAttribute(key, value);
    });

    return element;
}


// Draw roads, traffic conditions and locations.

function renderDynamicMap() {

    if (!mapSvg || !roadData.length) {
        return;
    }

    // Clear previous map drawing.
    mapSvg.replaceChildren();

    const locationById = new Map(
        mapLocations.map(location => [
            location.id,
            location
        ])
    );


    // ----------------------------------------
    // Draw all predefined roads
    // ----------------------------------------

    roadData.forEach(road => {

        const start = locationById.get(Number(road.from));
        const end = locationById.get(Number(road.to));

        if (!start || !end) {
            return;
        }

        let roadColor = "#a9d936";
        let dash = "";

        if (road.closed) {

            roadColor = "#737b89";
            dash = "7 6";

        } else {

            switch (Number(road.condition)) {

                case 1:
                    roadColor = "#f1bd55";
                    break;

                case 2:
                    roadColor = "#f16f75";
                    break;

                default:
                    roadColor = "#a9d936";

            }

        }

        const line = createSvgElement("line", {

            x1: start.x,
            y1: start.y,

            x2: end.x,
            y2: end.y,

            stroke: roadColor,
            "stroke-width": 5,

            "stroke-linecap": "round",

            opacity: road.closed ? 0.65 : 0.85

        });

        if (dash) {
            line.setAttribute("stroke-dasharray", dash);
        }

        // Add road line to SVG.
        mapSvg.appendChild(line);

    });


    // ----------------------------------------
    // Highlight selected route
    // ----------------------------------------

    for (
        let i = 0;
        i < selectedRouteIds.length - 1;
        i++
    ) {

        const fromId = selectedRouteIds[i];
        const toId = selectedRouteIds[i + 1];

        const start = locationById.get(fromId);
        const end = locationById.get(toId);

        if (!start || !end) {
            continue;
        }

        const routeLine = createSvgElement("line", {

            x1: start.x,
            y1: start.y,

            x2: end.x,
            y2: end.y,

            stroke: "#a98aff",

            "stroke-width": 7,

            "stroke-linecap": "round",

            "stroke-dasharray": "10 7",

            filter: "drop-shadow(0 0 5px #9c7aff)"

        });

        mapSvg.appendChild(routeLine);

    }


    // ----------------------------------------
    // Draw all 8 location markers and labels
    // ----------------------------------------

    mapLocations.forEach(location => {

        const isSelected =
            selectedRouteIds.includes(location.id);


        // Location marker.

        const marker = createSvgElement("circle", {

            cx: location.x,
            cy: location.y,

            r: isSelected ? 12 : 9,

            fill: isSelected ? "#8055f6" : "#a9d936",

            stroke: isSelected ? "#d4c5ff" : "#d7f79a",

            "stroke-width": 3

        });

        mapSvg.appendChild(marker);


        // Location name.

        const label = createSvgElement("text", {

            x: location.x,
            y: location.y - 17,

            fill: "#f5f6fa",

            "font-size": 12,

            "font-weight": 600,

            "text-anchor": "middle",

            "paint-order": "stroke",

            stroke: "#090b10",

            "stroke-width": 3,

            "stroke-linejoin": "round"

        });

        label.textContent = location.name;

        mapSvg.appendChild(label);

    });

}


// --------------------------------------------
// 9. LOAD ROAD DATA FROM C BACKEND
// --------------------------------------------

async function loadRoads() {

    try {

        const data = await apiRequest("/api/roads");

        roadData = data.roads;

        renderRoadControls();

        // Refresh dynamic map.
        renderDynamicMap();

    } catch (error) {

        console.error(
            "Could not load roads:",
            error
        );

    }

}


// --------------------------------------------
// 10. ROAD MANAGEMENT DROPDOWN
// --------------------------------------------

function renderRoadControls() {

    const roadSelect =
        document.getElementById("roadSelect");

    if (!roadSelect) {
        return;
    }

    const previousValue = roadSelect.value;

    roadSelect.innerHTML = roadData.map(road => `

        <option value="${road.from},${road.to}">

            ${escapeHTML(road.fromName)}
            ↔
            ${escapeHTML(road.toName)}

            ${road.closed ? "(CLOSED)" : ""}

        </option>

    `).join("");

    // Preserve selected road when possible.
    if (
        previousValue &&
        [...roadSelect.options].some(
            option => option.value === previousValue
        )
    ) {
        roadSelect.value = previousValue;
    }

}


// --------------------------------------------
// 11. POPULATE LOCATION DROPDOWNS
// --------------------------------------------

function populateLocations(select, selectedIndex) {

    select.innerHTML = "";

    const locations = [
        "NED University",
        "Gulshan-e-Iqbal",
        "University Road",
        "Bahadurabad",
        "Shahrah-e-Faisal",
        "Saddar",
        "Clifton",
        "Karachi Airport"
    ];

    locations.forEach((name, index) => {

        const option = document.createElement("option");

        option.value = String(index);

        option.textContent = name;

        if (index === selectedIndex) {
            option.selected = true;
        }

        select.appendChild(option);

    });

}

populateLocations(source, 0);
populateLocations(destination, 7);


// --------------------------------------------
// 12. SWAP SOURCE AND DESTINATION
// --------------------------------------------

document.getElementById("swapBtn")
    .addEventListener("click", () => {

        const oldSource = source.value;

        source.value = destination.value;

        destination.value = oldSource;

    });


// ============================================
// 13. DISPLAY CALCULATED ROUTES
// ============================================

function renderRoutes(data) {

    const output =
        document.getElementById("routesOutput");


    // ----------------------------------------
    // Update selected route for map
    // ----------------------------------------

    selectedRouteIds = [];

    if (data.routes && data.routes.length > 0) {

        selectedRouteIds =
            data.routes[0].locations.map(
                location => Number(location.id)
            );

    }

    // Refresh map with highlighted route.
    renderDynamicMap();


    // ----------------------------------------
    // Handle no available routes
    // ----------------------------------------

    if (
        !data.routes ||
        data.routes.length === 0
    ) {

        output.innerHTML = `

            <div class="empty-routes">

                <strong>No routes available</strong>

                <p>
                    ${escapeHTML(
                        data.message ||
                        "Try another journey or adjust the maximum stops."
                    )}
                </p>

            </div>

        `;

        return;

    }


    // ----------------------------------------
    // Generate route cards
    // ----------------------------------------

    const routeCards = data.routes.map(route => {

        const locationNames = route.locations
            .map(location => escapeHTML(location.name));

        return `

            <article class="route-card">

                <div class="route-card-top">

                    <strong>Route ${route.number}</strong>

                    <span class="route-tag">
                        ${route.number === 1
                            ? "SHORTEST DISTANCE"
                            : "ALTERNATIVE"}
                    </span>

                </div>

                <div class="route-path">

                    ${locationNames.join(
                        '<span class="route-arrow"> → </span>'
                    )}

                </div>

                <div class="route-stats">

                    <div>
                        <small>DISTANCE</small>
                        <strong>
                            ${Number(route.distance).toFixed(2)} km
                        </strong>
                    </div>

                    <div>
                        <small>TRAVEL TIME</small>
                        <strong>
                            ${Number(route.travelTime).toFixed(1)} min
                        </strong>
                    </div>

                    <div>
                        <small>FUEL ESTIMATE</small>
                        <strong>
                            ${Number(route.fuel).toFixed(2)} L
                        </strong>
                    </div>

                </div>

            </article>

        `;

    }).join("");


    // ----------------------------------------
    // Display results
    // ----------------------------------------

    output.innerHTML = `

        <div class="results-summary">

            <strong>${Number(data.count)} route(s) found</strong>

            <span>
                Sorted by shortest distance using C bubble sort
            </span>

        </div>

        ${routeCards}

        <button
            type="button"
            class="primary-button report-save-button"
            id="saveReportBtn">

            Save route report <span>↓</span>

        </button>

    `;


    document.getElementById("saveReportBtn")
        .addEventListener("click", saveReport);

}


// ============================================
// 14. ROUTE SEARCH FORM
// ============================================

routeForm.addEventListener("submit", async event => {

    event.preventDefault();


    // Validate source and destination.

    if (source.value === destination.value) {

        showNotice(
            "Choose two different locations",
            "Your starting point and destination must be different.",
            "error"
        );

        return;

    }


    const request = {

        source: Number(source.value),

        destination: Number(destination.value),

        stops: Number(stopsSelect.value),

        vehicle: Number(vehicleSelect.value)

    };

    lastRequest = request;


    const output =
        document.getElementById("routesOutput");

    output.innerHTML = `

        <p class="panel-footnote">
            Calculating routes with the C DFS engine...
        </p>

    `;


    const button =
        routeForm.querySelector('button[type="submit"]');

    button.disabled = true;

    button.textContent = "Calculating routes...";


    try {

        const params = new URLSearchParams({

            source: request.source,

            destination: request.destination,

            stops: request.stops,

            vehicle: request.vehicle

        });


        const data = await apiRequest(
            `/api/routes?${params.toString()}`
        );


        renderRoutes(data);


        showNotice(
            "Route calculation completed",
            `${data.count} route(s) returned by the C backend.`,
            "success"
        );


    } catch (error) {

        // Clear old highlighted route after a failed search.
        selectedRouteIds = [];

        renderDynamicMap();

        output.innerHTML = `

            <div class="empty-routes">

                <strong>Could not calculate routes</strong>

                <p>${escapeHTML(error.message)}</p>

            </div>

        `;


        showNotice(
            "Route calculation failed",
            error.message,
            "error"
        );


    } finally {

        button.disabled = false;

        button.innerHTML =
            'Find best routes <span>↗</span>';

    }

});


// ============================================
// 15. SAVE ROUTE REPORT
// ============================================

async function saveReport() {

    if (!lastRequest) {
        return;
    }

    const params = new URLSearchParams(lastRequest);

    try {

        const data = await apiRequest(
            `/api/report?${params.toString()}`
        );

        showNotice(
            "Report saved",
            data.message,
            "success"
        );

    } catch (error) {

        showNotice(
            "Report could not be saved",
            error.message,
            "error"
        );

    }

}


// ============================================
// 16. ROAD MANAGEMENT CONTROLS
// ============================================

const trafficPanel =
    document.querySelector(".traffic-panel");


const roadManager = document.createElement("div");

roadManager.className = "road-manager";


roadManager.innerHTML = `

    <div class="eyebrow">BACKEND CONTROLS</div>

    <h3>Manage road conditions</h3>

    <label for="roadSelect">Select road</label>

    <select id="roadSelect"></select>

    <label for="conditionSelect">Traffic condition</label>

    <select id="conditionSelect">

        <option value="0">Normal</option>

        <option value="1">Busy</option>

        <option value="2">Congested</option>

    </select>

    <div class="road-manager-actions">

        <button
            type="button"
            id="updateRoadBtn"
            class="secondary-button">

            Update condition

        </button>

        <button
            type="button"
            id="toggleRoadBtn"
            class="secondary-button">

            Close / reopen

        </button>

    </div>

    <p id="roadManagerMessage" class="panel-footnote">

        Changes are applied to both directions of the selected road.

    </p>

`;


trafficPanel.appendChild(roadManager);


// --------------------------------------------
// 17. UPDATE ROAD CONDITION OR CLOSURE
// --------------------------------------------

async function updateRoad(closed) {

    const selected =
        document.getElementById("roadSelect").value;

    if (!selected) {
        return;
    }

    const [from, to] =
        selected.split(",").map(Number);


    const existingRoad = roadData.find(road =>

        Number(road.from) === from &&
        Number(road.to) === to

    );

    if (!existingRoad) {
        return;
    }


    const condition = Number(
        document.getElementById("conditionSelect").value
    );


    const params = new URLSearchParams({

        from,

        to,

        condition,

        closed: closed === null
            ? Number(existingRoad.closed)
            : Number(closed)

    });


    const message =
        document.getElementById("roadManagerMessage");


    try {

        const data = await apiRequest(
            `/api/road/update?${params.toString()}`
        );


        message.textContent = data.message;


        // Refresh roads and redraw the map.
        // Clear previous route because its road conditions may have changed.
        selectedRouteIds = [];

        await loadRoads();


        showNotice(
            "Road network updated",
            data.message,
            "success"
        );


    } catch (error) {

        message.textContent = error.message;

        showNotice(
            "Road update failed",
            error.message,
            "error"
        );

    }

}


// --------------------------------------------
// 18. ROAD MANAGEMENT BUTTONS
// --------------------------------------------

document.getElementById("updateRoadBtn")
    .addEventListener("click", () => {

        updateRoad(null);

    });


document.getElementById("toggleRoadBtn")
    .addEventListener("click", () => {

        const selected =
            document.getElementById("roadSelect").value;

        if (!selected) {
            return;
        }

        const [from, to] =
            selected.split(",").map(Number);


        const road = roadData.find(item =>

            Number(item.from) === from &&
            Number(item.to) === to

        );


        if (road) {

            updateRoad(!road.closed);

        }

    });


// --------------------------------------------
// 19. LOAD SELECTED ROAD CONDITION
// --------------------------------------------

document.getElementById("roadSelect")
    .addEventListener("change", () => {

        const selected =
            document.getElementById("roadSelect").value;

        if (!selected) {
            return;
        }

        const [from, to] =
            selected.split(",").map(Number);


        const road = roadData.find(item =>

            Number(item.from) === from &&
            Number(item.to) === to

        );


        if (road) {

            document.getElementById("conditionSelect").value =
                String(road.condition);

        }

    });


// ============================================
// 20. INITIALIZE SMARTROUTE
// ============================================

checkBackend();

loadRoads();
