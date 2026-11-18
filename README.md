# Karachi SmartRoute

### A C-Based Smart Route Selection and Traffic Simulation System

Karachi SmartRoute is a Programming Fundamentals semester project developed for the Department of Computer Science at NED University of Engineering and Technology, Karachi.

The project is designed to demonstrate how a route selection system can help users compare different routes between predefined locations in Karachi. It uses predefined road-network data, route-finding logic, simulated traffic conditions, road closures, and vehicle-related fuel calculations to provide route information.

The project combines a **C17 backend** with a lightweight web-based frontend. It also provides a console-based interface for running the backend independently.

> **Project Note:** This is an educational simulation. Locations, road connections, travel times, and traffic conditions are predefined or simulated. The application does not provide live GPS navigation, real-time traffic information, or live map services.

---

## 1. Project Information

| Detail | Description |
|---|---|
| Project Name | Karachi SmartRoute |
| Course | Programming Fundamentals |
| Course Code | CT-175 |
| Department | Computer Science |
| University | NED University of Engineering and Technology |
| Programming Language | C (C17) |
| Frontend | HTML, CSS, JavaScript |
| Backend | C |
| Server | C Winsock HTTP Server |
| Compiler | GCC |
| Development Environment | Visual Studio Code |
| Operating System | Windows |

---

## 2. Project Overview

Karachi is a large city with many roads and possible routes between different areas. Choosing a suitable route can depend on several factors, including distance, estimated travel time, traffic conditions, road closures, and vehicle fuel consumption.

Karachi SmartRoute demonstrates a simplified solution to this problem using programming fundamentals and basic route-selection algorithms.

The system works with a predefined network of locations and roads. It identifies possible routes, evaluates route information, and presents the available results to the user.

The project is developed to demonstrate how fundamental programming concepts can be applied to a practical, real-world-inspired problem.

---

## 3. Main Features

### Route Selection
- Uses predefined Karachi locations and road connections.
- Finds possible routes between selected locations.
- Uses route-searching logic to explore available paths.
- Allows route information to be compared.

### Traffic Simulation
- Includes simulated traffic conditions.
- Represents different traffic levels.
- Demonstrates how traffic conditions can affect estimated travel time.

### Road Closure Simulation
- Supports predefined or simulated road closures.
- Excludes unavailable roads from route selection.
- Demonstrates alternative route exploration.

### Vehicle and Fuel Information
- Includes vehicle-related fuel calculations.
- Estimates fuel expenses using the project's predefined values.
- Helps demonstrate how vehicle information can contribute to route comparison.

### Route Sorting and Comparison
- Organizes route results using sorting logic.
- Makes it easier to compare available route options.

### Report Generation
- Supports saving route-related report information to a text file.
- Provides a record of generated results.

### Console Application
- Provides a console-based interface.
- Allows the backend to run independently.

### Web-Based Interface
- Includes a frontend built using HTML, CSS, and JavaScript.
- Communicates with the C backend through a lightweight local HTTP server.
- Runs locally in a web browser.

---

## 4. Technologies Used

| Technology | Purpose |
|---|---|
| C (C17) | Core backend logic and route processing |
| HTML5 | Frontend structure |
| CSS3 | Frontend styling |
| JavaScript | Frontend interaction |
| GCC | Compiling C source files |
| Winsock | Local HTTP server communication on Windows |
| Visual Studio Code | Development environment |
| Git and GitHub | Version control and project hosting |

No database or large web framework is required for this project.

---

## 5. Project Folder Structure

```text
SmartRoute/
│
├── .gitignore
│
└── SmartRoute_Backend/
    │
    ├── bin/
    │   ├── SmartRoute.exe
    │   ├── SmartRouteServer.exe
    │   ├── main.o
    │   └── smartroute
    │
    ├── include/
    │   ├── data.h
    │   ├── models.h
    │   └── route.h
    │
    ├── reports/
    │   └── SmartRoute_Report.txt
    │
    ├── SmartRoute_Frontend/
    │   ├── app.js
    │   ├── index.html
    │   ├── README.txt
    │   └── styles.css
    │
    ├── src/
    │   ├── data.c
    │   ├── main.c
    │   ├── route.c
    │   └── server.c
    │
    ├── Makefile
    └── README.md
```

---

## 6. Requirements

To compile and run the project, the following are required:

- Windows operating system.
- GCC compiler.
- Visual Studio Code or another C development environment.
- A modern web browser for the frontend.
- Winsock support, which is available on Windows.

The project uses the GCC compiler located at:

```text
C:\msys64\ucrt64\bin\gcc.exe
```

If GCC is installed in a different location, update the compiler path in the commands below.

---

## 7. How to Run the Project

### Step 1: Clone the Repository

Open PowerShell and run:

```powershell
git clone https://github.com/mabdullahukhan/SmartRoute.git
```

Move into the backend directory:

```powershell
cd SmartRoute\SmartRoute_Backend
```

### Step 2: Compile the Console Backend

Run the following commands in PowerShell:

```powershell
$gcc = "C:\msys64\ucrt64\bin\gcc.exe"
$oldPath = $env:PATH

try {
    $env:PATH = "C:\msys64\ucrt64\bin;C:\Windows\System32;C:\Windows"

    & $gcc -std=c17 -Wall -Wextra -Wpedantic -Iinclude `
        src/main.c src/data.c src/route.c `
        -o bin/SmartRoute.exe

    if ($LASTEXITCODE -ne 0) {
        throw "Backend compilation failed."
    }
}
finally {
    $env:PATH = $oldPath
}
```

### Step 3: Run the Console Application

```powershell
.\bin\SmartRoute.exe
```

The console application will start.

### Step 4: Compile the Web Server

To run the web-based interface, compile the C server:

```powershell
$gcc = "C:\msys64\ucrt64\bin\gcc.exe"
$oldPath = $env:PATH

try {
    $env:PATH = "C:\msys64\ucrt64\bin;C:\Windows\System32;C:\Windows"

    & $gcc -std=c17 -Wall -Wextra -Wpedantic -Iinclude `
        src/server.c src/data.c src/route.c `
        -lws2_32 -o bin/SmartRouteServer.exe

    if ($LASTEXITCODE -ne 0) {
        throw "Server compilation failed."
    }
}
finally {
    $env:PATH = $oldPath
}
```

### Step 5: Start the Web Server

```powershell
.\bin\SmartRouteServer.exe
```

Keep the PowerShell window open while using the application.

### Step 6: Open the Frontend

Open your browser and visit:

```text
http://127.0.0.1:8080
```

The local server provides the frontend and handles communication with the C backend.

---

## 8. Programming Fundamentals Concepts

The project demonstrates the practical application of Programming Fundamentals concepts, including:

- **Variables and Data Types:** Storing locations, route information, traffic values, and other data.
- **Input and Output:** Receiving user input and displaying application results.
- **Operators:** Performing arithmetic, relational, and logical operations.
- **Conditional Statements:** Making decisions based on route and traffic conditions.
- **Switch Statements:** Handling menu choices and application options where applicable.
- **Loops:** Repeating operations and processing route information.
- **Arrays:** Managing collections of predefined locations and road information.
- **Strings:** Handling textual information and location names.
- **Functions:** Dividing the program into smaller, reusable modules.
- **Structures:** Organizing related information into structured data types.
- **Pointers:** Working with memory addresses and passing data between functions.
- **Recursion:** Exploring possible routes through recursive route-searching logic.
- **Searching:** Finding routes and processing available road connections.
- **Sorting:** Organizing route results for comparison.
- **File Handling:** Saving generated report information.
- **Error Handling:** Managing invalid input and other application conditions.

These concepts are incorporated where they are relevant to the system's functionality.

---

## 9. Project Modules

The backend is organized into separate source and header files to keep the code modular and maintainable.

| File | Responsibility |
|---|---|
| `main.c` | Console application entry point |
| `data.c` | Predefined project data |
| `route.c` | Route-processing and route-selection logic |
| `server.c` | Local HTTP server and frontend communication |
| `models.h` | Data structures and model definitions |
| `data.h` | Data-related declarations |
| `route.h` | Route-related declarations |

The frontend is organized separately:

| File | Responsibility |
|---|---|
| `index.html` | Frontend page structure |
| `styles.css` | Visual styling |
| `app.js` | Frontend interaction and backend communication |

---

## 10. Limitations

Karachi SmartRoute is an educational project developed for demonstrating Programming Fundamentals concepts.

Its current limitations include:

- It uses predefined locations and road-network information.
- Traffic conditions are simulated rather than retrieved from live traffic services.
- It does not provide actual GPS tracking.
- It does not provide turn-by-turn navigation.
- Travel times and fuel estimates depend on the project's predefined data and calculation logic.
- The local web server is intended for educational demonstration.

---

## 11. Future Improvements

Possible future improvements include:

- Expanding the predefined Karachi road network.
- Adding more locations and route alternatives.
- Improving route comparison and selection.
- Adding more vehicle categories and fuel options.
- Improving report generation.
- Enhancing the frontend interface.
- Exploring integration with real mapping and traffic services in a future version.

---

## 12. Project Report and Documentation

The project documentation and supporting materials are maintained alongside the source code.

The final submission may include:

- Project report in PDF format.
- System flowchart.
- Application screenshots.
- Source code.
- Supporting documentation.

---

## 13. Academic Purpose

Karachi SmartRoute is developed as a semester project for the Programming Fundamentals course (CT-175) at NED University of Engineering and Technology.

The main objective is to apply fundamental programming concepts to a practical problem through a modular C-based application.

---

## 14. Authors

**Developed by:**  
NED University of Engineering and Technology  
Department of Computer Science

**Group Members:**

1. [Muhammad Abdullah Mohsin Khan] — [CT-26279]
2. [Muhammad Hamza Kashif] — [CT-26282]
3. [Muhammad Mustafa Afnan] — [CT-26295]

---

## 15. Repository

GitHub Repository:

https://github.com/mabdullahukhan/SmartRoute

---

**Karachi SmartRoute — Programming Fundamentals Project (CT-175)**

*NED University of Engineering and Technology, Karachi*
