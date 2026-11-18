# SmartRoute — Backend Foundation

A first-semester Programming Fundamentals project using C17.

## Current scope
- Eight illustrative Karachi-area locations
- Three predefined vehicle categories
- Directed road-edge data stored in an adjacency matrix
- Road condition and closure fields reserved for later phases
- Modular source/header organization

## Build with GCC (Windows)
From this directory:

```bash
gcc -std=c17 -Wall -Wextra -Wpedantic -Iinclude src/main.c src/data.c -o bin/smartroute.exe
```

Run:

```bash
./bin/smartroute.exe
```

All sample road distances, times and vehicle mileage values are illustrative, not live measurements.
