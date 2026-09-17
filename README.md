# Algo Visualisierer 

## SDL3 Setup

Dieses Projekt verwendet **SDL3**. Damit das Projekt gebaut werden kann, muss SDL3 auf dem System installiert sein.

### Voraussetzungen

Benötigt werden:

```bash
cmake
g++
SDL3
```

### Installation

#### macOS mit Homebrew

```bash
brew install cmake sdl3
```

### Projekt bauen

Im Projektordner:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

Danach kann das Programm aus dem `build`-Ordner gestartet werden.

Beispiel:

```bash
./MeinProjekt
```

Der Name der ausführbaren Datei hängt davon ab, wie das Target in der `CMakeLists.txt` heißt.

### Beispiel für CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.20)

project(MeinProjekt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL3 REQUIRED)

add_executable(MeinProjekt
    src/main.cpp
)

target_link_libraries(MeinProjekt PRIVATE SDL3::SDL3)
```

### Wichtige Includes

Bei SDL3 wird normalerweise so eingebunden:

```cpp
#include <SDL3/SDL.h>
```

Nicht mehr so:

```cpp
#include <SDL.h>
```

### Häufige Fehler

#### SDL3 wird von CMake nicht gefunden

Dann ist SDL3 entweder nicht installiert oder CMake findet den Installationspfad nicht.

Auf macOS kann helfen:

```bash
brew install sdl3
```



SDL3 ist nicht komplett gleich wie SDL2. Manche Funktionen oder Strukturen wurden geändert. Deshalb muss alter SDL2-Code teilweise angepasst werden.
