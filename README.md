# Elevator Scheduler Operating System

## Project Overview
This project is an **Elevator Scheduler Operating System** designed to efficiently manage the movement of elevators in a building using advanced scheduling algorithms. The scheduler interacts with the simulation environment using asynchronous API calls and handles real-time requests from passengers to optimize elevator operations. The project is built in C++ with multithreading to ensure high performance and concurrency.

## Features
- **Efficient Elevator Scheduling**: Implements a scheduling system that manages elevator requests, optimizing for factors like elevator load, proximity, and historical wait times.
- **Multithreading**: Uses multithreading to manage concurrent requests and ensure smooth operation across multiple elevators.
- **API Integration**: Communicates with the simulation environment and operating system via asynchronous API calls to manage real-time elevator operations.
- **Configurable Setup**: Reads building configuration and people movement data from input files for flexibility in testing different scenarios.
- **External Libraries**: Uses `pthread` for multithreading and `libcurl` for network communication.

## Requirements
- **g++ Compiler**: Ensure you have `g++` installed for compiling C++ programs.
- **Libraries**: The project requires `pthread` for multithreading and `libcurl` for network communication.
- **C++11**: The project uses the C++11 standard, so ensure your compiler supports this version.

## Installation and Setup
1. Clone the repository:
   ```bash
2. Navigate to the project directory:
3. Compile the project:
 .make
 .This will create an executable named scheduler_os in the project directory.
4.To clean up the compiled files:
 make clean

## Usage

1. **Prepare the input files**:
   - **Building Configuration File**: Define the building’s elevator configuration, such as the number of elevators, capacity, and floor range.
   - **People Configuration File**: Specify the movement of people between floors and the time tick for each action.

2. **Run the scheduler**:
   ```bash
   ./scheduler_os


## Example Input Format
- Building Configuration File:

- HotelBayA 1 8 4 10
- This example defines an elevator bay named HotelBayA with a floor range from 1 to 8, currently on floor 4, and a capacity of 10.

- People Configuration File:
- Eric_R 1 2 0
- This defines a person named Eric_R moving from floor 1 to floor 2 at time tick 0.
