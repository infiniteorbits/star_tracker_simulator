# star_tracker_simulator
A model to simulate the view of a star tracker with star information based on the Hipparcos-2 star catalogue. The simulator parses the Hipparcos catalog, and uses the information to build an OpenGL environment of the celestial sphere.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for testing purposes.

### Prerequisites

This project requires OpenGL and CMake to be installed. 

```
This project is compatible with OpenGL 2.0 or later.
This project is compatible with CMake 3.0 or later.
```

### Installing

#### Linux/MacOS/Unix

Download or clone this repository.
Unzip if necessary.

```
cd star_tracker_simulator
cmake .
sudo make
```

## Running

Run the executable

```
./star_tracker_input
```

## User Manual

Use the arrow keys to look around
* ARROW UP/DOWN to increase/decrease declination
* ARROW LEFT/RIGHT to change right ascension

Press
* ESC to close the window
* S key to capture an image
