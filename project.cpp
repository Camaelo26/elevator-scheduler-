/* Elevator scheduler 
*C++ program that will act as the scheduler for an Elevator Operating System.
*
*/



#include <chrono>
#include <condition_variable>
#include <curl/curl.h>
#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <queue>
#include <sstream>
#include <thread>



////strutctss declarations////
// Elevator data structure
struct elevat {
  std::string name_elevator;
  int lowFloor;
  int topFloor;
  int actuallevel;
  int capacity;
  int direction; // -1 for down, 0 for no move, 1 for up
  
  // Convert those and put them in the elevator struct 
  static elevat strr(const std::string &line) {
    std::istringstream iss(line);
    elevat datas;
    iss >> datas.name_elevator >> datas.lowFloor >> datas.topFloor >> datas.actuallevel >> datas.capacity;
    datas.direction = 0; // Initially, the elevator is static
    return datas;
  }
};

// Person data structure
struct Person {
  std::string id;
  int startFloor;
  int endFloor;
};
////end of struct/////

//////////////
std::vector<elevat> elevators;
std::map<std::string, int>num_in_elevat; // Map to track the number of person in each elevator
std::queue<Person> people_coming;         // Queue for incoming person requests
std::mutex queueMutex;                 // Mutex for input queue
std::condition_variable inputcond; // Condition variable for input data
std::queue<std::pair<std::string, std::string>>Apiqueue;    // Queue for API tasks
std::mutex outmutex;        // Mutex for output queue
std::condition_variable outcond; // Condition variable for output data
std::map<std::string, std::chrono::steady_clock::time_point>requestTimestamps; // Tracking request times
////////////

//////////
/** manage CURL response**/
static size_t managedata(void *a, size_t x, size_t y,void *b) {
  // Get the size of the data coming
  size_t s = x * y;
  // Chabge pointer to  std::string pointer
  std::string *data = static_cast<std::string *>(b);
  // Append data to the string provided bc it s received as byte
  data->append(static_cast<char *>(a), s);

  // Return the number of bytes processedma
  return s;
}

/** API call function**/
std::string callapi(const std::string &url,const std::string &method = "GET",const std::string &data = "") {
  CURL *manag;
  CURLcode datas;
  std::string getda;
  // Initialize global curl
  curl_global_init(CURL_GLOBAL_ALL);
  manag = curl_easy_init();
  if (manag) {
    // put  URL for the API request
    curl_easy_setopt(manag, CURLOPT_URL, url.c_str());
    curl_easy_setopt(manag, CURLOPT_WRITEFUNCTION, managedata);
    curl_easy_setopt(manag, CURLOPT_WRITEDATA, &getda);
    if (method == "PUT") {
      curl_easy_setopt(manag, CURLOPT_CUSTOMREQUEST, "PUT");
      curl_easy_setopt(manag, CURLOPT_POSTFIELDS, data.c_str());
    }
    // Execute the API request
    datas = curl_easy_perform(manag);
  }
  return getda;
}

/** condition for a person to be assigned to an elevator **/
bool givethiselevator(const Person &person, const elevat &elevator) {
  //the person start floor is greater than or equal to the low floor the elevator
  //the person start floor is less than or equal to the top floor the elevator
  //the person end floor is greater than or equal to the low floor the elevator can serve
  //the person end floor is less than or equal to the top floor the elevator
  //check elevator suitability based on direction 
  return person.startFloor >= elevator.lowFloor && 
         person.startFloor <= elevator.topFloor &&
         person.endFloor >= elevator.lowFloor &&
         person.endFloor <= elevator.topFloor;
         (elevator.direction == 0 ||
          (elevator.direction == 1 && person.startFloor > elevator.actuallevel) ||
          (elevator.direction == -1 && person.startFloor < elevator.actuallevel));
}

/** Hybrid function to select an elevator considering both distance and wait time ,people in , and direction it s going to **/
std::string select1(const Person &person,const std::vector<elevat> &elevators) {
  std::string takethiselevator;
  double better = std::numeric_limits<double>::max();
  auto now = std::chrono::steady_clock::now();

  for (const auto &elevator : elevators) {
    if (givethiselevator(person, elevator) &&num_in_elevat[elevator.name_elevator] < elevator.capacity) {
      int distance = std::abs(elevator.actuallevel - person.startFloor);// calculate distance from elevator actual floor to person start floor
      double waitTime = std::chrono::duration_cast<std::chrono::seconds>(now - requestTimestamps[person.id]).count();// wait time since the elevator was requested by this person
      double loadFactor =1.0 - (double)num_in_elevat[elevator.name_elevator] / elevator.capacity;// see how full the elevator is
      double directionBonus = elevator.direction == 0 ? 0 : (elevator.direction == 1 && person.endFloor > elevator.actuallevel) || (elevator.direction == -1 && person.endFloor < elevator.actuallevel) ? 0 : 10;
      double score = distance + 0.5 * waitTime - loadFactor + directionBonus;





      // If the score for this elevator is better than the best score found ,
      // update the best score and the choice of elevator
      if (score < better) {
        better = score;
        takethiselevator = elevator.name_elevator;
      }
    }
  }
  return takethiselevator;
}

/**select elevator based on only short distance first as an alternate issue**/
std::string select2(const Person &person,const std::vector<elevat> &elevators) {
  std::string takethiselevator;
  int minDistance = std::numeric_limits<int>::max();

  for (const auto &elevator : elevators) {
    // Check if the person start floor is in the elevator  range
    if (person.startFloor >= elevator.lowFloor &&
        person.startFloor <= elevator.topFloor) {
      // distance between the elevator current floor and the person start floor
      int distance = std::abs(elevator.actuallevel - person.startFloor);
      // If this distance is the smallest found so far, upd it and the elevator
      if (distance < minDistance) {
        minDistance = distance;
        takethiselevator = elevator.name_elevator;
      }
    }
  }
  return takethiselevator;
}
/** function to get an elevator by ID**/
elevat *getelevaid(const std::string &elevatorID) {
  for (auto &elevator : elevators) {
    if (elevator.name_elevator == elevatorID) {
      return &elevator;
    }
  }
  return nullptr; // Return 
}



////////////////////thread functions ///////////////////////
/** Scheduler function to compute elevator assignments**/

void scheduler() {
  while (true) {
  // get a lock on the queue 
    std::unique_lock<std::mutex> lock(queueMutex);
    //  till at least one person waiting
    inputcond.wait(lock, [] { return !people_coming.empty(); });
// Process each person in the queue
    while (!people_coming.empty()) {
      Person person = people_coming.front();
      people_coming.pop();
   // Try to select the best elevator based on the first way
      std::string elevatorID = select1(person, elevators);
      if (!elevatorID.empty()) {
      // Retrieve the elevator  ID
        elevat *elevator = getelevaid(elevatorID);
        //  if the elevator has place
        if (elevator && num_in_elevat[elevatorID] < elevator->capacity) {
          num_in_elevat[elevatorID]++;
          // Lock the output queue to safely add the person to it
          std::lock_guard<std::mutex> outputLock(outmutex);
          Apiqueue.push({person.id, elevatorID});
          outcond.notify_one();// Signal any waiting threads 
          std::cout << "Scheduler: Assigning " << person.id << " to "<< elevatorID << std::endl;
        }
      } else {
        // If no elevator was found for a peerson ,use second way 
        elevatorID = select2(person, elevators);
        if (!elevatorID.empty()) {
          std::lock_guard<std::mutex> outputLock(outmutex);
          Apiqueue.push({person.id, elevatorID});
          outcond.notify_one();
          std::cout << "alternate scheduler: Assigning " << person.id << " to "<< elevatorID << " based on second one" << std::endl;
        } else {
          std::cerr<< "alternate  Scheduler: No elevator available even for SDF for "<< person.id << std::endl;
        }
      }
    }
    lock.unlock(); // Allow other operations to proceed
    std::this_thread::sleep_for(std::chrono::milliseconds(200)); //small wait
  }
}


/** output communication thread  function to communicate   with /AddPersonToElevator  **/
void outThread() {
  while (true) {
    std::pair<std::string, std::string> dataparamm;//keep the person and elevator data
     // safe access the shared queue using a mutex
    {
      std::unique_lock<std::mutex> lock(outmutex);// get mutex for queue access
      outcond.wait(lock, [] { 
      return !Apiqueue.empty(); 
      }
      );// Wait for at least one data in the queue
      dataparamm = std::move(Apiqueue.front());// Move the front data from the queue and pop it
      Apiqueue.pop();
    } // lock is released 

    std::string url = "http://localhost:5432/AddPersonToElevator/" + dataparamm.first + "/" + dataparamm.second;
    std::string response = callapi(url, "PUT");// Call the API and store the response(oerson id and elevator id)
    std::cout << "putting " << dataparamm.first << " to elevator " << dataparamm.second << ". Response: " << response << std::endl;// output the person id and the elevator sent to
  }
}


//** Input Communication Thread to communicate with the /NextInput API. **/
void inthread() {
  while (true) {
    //  API for the next input
    std::string response = callapi("http://localhost:5432/NextInput");

    // if response is not none
    if (response != "NONE") {
      std::istringstream iss(response); //parse the response
      Person person;

      // get person ID
      std::getline(iss, person.id, '|');

      // get start floor and end floor 
      std::string floorStr;
      std::getline(iss, floorStr, '|'); // Extract start floor
      person.startFloor = std::stoi(floorStr);
      
      std::getline(iss, floorStr); // Extract end floor
      person.endFloor = std::stoi(floorStr);

      // Safely add the person to the shared queue
      {
        std::lock_guard<std::mutex> lock(queueMutex); //acquire mutex
        people_coming.push(person); // Push into the queue
        inputcond.notify_one(); // Notify one waiting thread
      } // Mutex released 
    }

    //  small wait
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}


//////main

int main(int argc, char *argv[]) {
std::cout << "Macarthur Diby" <<std::endl;
std::cout << "R11763241" <<std::endl;
  
  std::ifstream file(argv[1]);
  std::string line;
  while (getline(file, line)) {
    elevat ec = elevat::strr(line);
    elevators.push_back(ec);
    num_in_elevat[ec.name_elevator] = 0; // Initialize number of person in a current elevator
  }

  std::string startResponse = callapi("http://localhost:5432/Simulation/start", "PUT");//start simulation
  std::cout << "Start Simulation: " << startResponse << std::endl;
//thread
  std::thread inputThread(inthread);
  std::thread schedulerThread(scheduler);
  std::thread outputThread(outThread);

  inputThread.join();
  schedulerThread.join();
  outputThread.join();

  return 0;
}