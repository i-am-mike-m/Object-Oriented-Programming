#include <iostream>
#include <stack>
#include <vector>
#include <unordered_map>
#include <string>

/* Wouldn't normally use a namespace like this, just making life easy for this single-file practice work. */ 
using namespace std;

enum VehicleSize { sm, med, lg };

class Space {
    public:
        VehicleSize spaceSize;
        char spaceLevel;
        int spaceNumber;
        
        Space(char level, int number, VehicleSize size) {
            spaceLevel = level;
            spaceNumber = number;
            spaceSize = size;
        }
        
        /* Returns a string containing the full space name for this Space object. */
        string getSpaceName() {
            return spaceLevel + to_string(spaceNumber);
        }
};

class Level {
    public:
        char level;
        vector<stack<Space*>> spaceStacks;
        
        Level (char level, int smallSpaceCount, int medSpaceCount, int lgSpaceCount) {
            this->level = level;
            
            stack<Space*> s;
            for (int i = 0; i < 3; i++) spaceStacks.push_back(s);
            
            /*
                Fill each stack with the indicated number of (pointers to) Spaces. Numbering is established such that small spaces
                have the lowest numbers on each floor and that the lowest space number begins on the top of the stack for an empty
                or new garage.
            */
            int count = smallSpaceCount + medSpaceCount + lgSpaceCount;
            for (int i = 0; i < lgSpaceCount; i++) { spaceStacks[2].push(new Space(level, count, lg)); count--; }
            for (int i = 0; i < medSpaceCount; i++) { spaceStacks[1].push(new Space(level, count, med)); count--; }
            for (int i = 0; i < smallSpaceCount; i++) { spaceStacks[0].push(new Space(level, count, sm)); count--; }
        }
};

class Vehicle {
    public:
        string license;
        VehicleSize size;
        
        Vehicle(string license, VehicleSize size) {
            this->license = license;
            this->size = size;
        }
};

class Garage {
    public:
        vector<Level*> levels;
        unordered_map<string, Space*> mapVehicleLicenseToSpace;
        unordered_map<Space*, string> mapSpaceToVehicleLicense;
        
        Garage (int numLevels) {
            for (int i = 0; i < numLevels; i++) {
                int sm, med, lg;
                cout << "Enter number of small spaces on level " << i + 1 << ": ";
                cin >> sm;
                cout << "Enter number of medium spaces on level " << i + 1 << ": ";
                cin >> med;
                cout << "Enter number of large spaces on level " << i + 1 << ": ";
                cin >> lg;
                levels.push_back(new Level(('A' + i), sm, med, lg));
            }
        }
        
        /* Prints current open space count by level and by space size. */
        void PrintOpenSpaceCount() {
            cout << "\r\nPrinting current available space count by level and size:" << endl;
            for (auto each : levels) {
                cout << "On Level " << each->level << ":" << endl;
                cout << "Small Spaces: " << each->spaceStacks[0].size() << endl;
                cout << "Medium Spaces: " << each->spaceStacks[1].size() << endl;
                cout << "Large Spaces: " << each->spaceStacks[2].size() << endl;
            }
        }
        
        /* 
            Receives pointer to vehicle object and assigns Space to Vehicle.  Records space-vehicle and vehicle-space correlation
            in hash set. Prioritizes most suitable space size first, followed by lowest parking level. Vehicles may be placed in
            larger Spaces if necessary but may not be placed in smaller Spaces.
        */
        string getSpace(Vehicle* vehicle) {
            for (int i = vehicle->size; i < 3; i++) {
                for (int level = 0; level < this->levels.size(); level++) {
                    if (!this->levels[level]->spaceStacks[i].empty()) {
                        Space* assignedSpace = this->levels[level]->spaceStacks[i].top();
                        string assignedSpaceName = assignedSpace->getSpaceName();
                        mapVehicleLicenseToSpace[vehicle->license] = assignedSpace;
                        mapSpaceToVehicleLicense[assignedSpace] = vehicle->license;
                        this->levels[level]->spaceStacks[i].pop();
                        return assignedSpaceName;
                    }
                }
            }
            return "No available parking for this vehicle size.";
        }
        
        /* Receives string representing Vehicle license and prints assigned Space or error message. */
        void findVehicleByLicensePlate(string license) {
            if (this->mapVehicleLicenseToSpace.find(license) != this->mapVehicleLicenseToSpace.end()) {
                cout << "Your vehicle is in space: " << mapVehicleLicenseToSpace[license]->getSpaceName() << endl;
            }
            else cout << "No vehicle matching license " << license << " found." << endl;
        }
        
        /* 
            Receives license string. Removes both Vehicle/Space correlations from hash tables and places Space back on proper stack.
            No input verification here as it is assumed this input is generated by the garage's systems upon observing vehicle exit.
            Improvement item would include input verification and perhaps nearest edit-distance checking in the event of poor video
            feed reporting the incorrect vehicle leaving the garage.
        */
        void onVehicleExit(string license) {
            Space* vacating = mapVehicleLicenseToSpace[license];
            int vacatingLevel = (int)(vacating->spaceLevel - 'A');
            mapVehicleLicenseToSpace.erase(license);
            mapSpaceToVehicleLicense.erase(vacating);
            levels[vacatingLevel]->spaceStacks[vacating->spaceSize].push(vacating);
        }
        
        /* Instantiate a vehicle object upon entering garage. Returns pointer to created Vehicle object. */
        Vehicle* createVehicle() {
            string license;
            int size;
            cout << "Enter License Plate: ";
            cin >> license;
            cout << "Enter Vehicle Size (0 - sm / 1- med / 2- lg): ";
            cin >> size;
            Vehicle* newVehicle = new Vehicle(license, (VehicleSize) size);
           return newVehicle;
        }
};

/*
    Prints main program loop for open-ended testing.
*/
void printMainMenu() {
    cout << "\r\nMain Menu" << endl;
    cout << "1. New Vehicle" << endl;
    cout << "2. Exit Vehicle" << endl;
    cout << "3. Find Vehicle" << endl;
    cout << "4. Check Garage Availability" << endl;
    cout << "Enter Selection: ";
}

int main() {
    /* Instantiate a parking garage. Currently using hard-coded number of levels for the garage. */
    Garage garage(2);
    
    /* TEST: Print space availability of newly instantiated garage. */
    /*
    garage.PrintOpenSpaceCount();
    */
    
    /* Instantiate vehicle of each size. */
    Vehicle* testCarSmall = new Vehicle("ABC-1234", sm);
    Vehicle* testCarMedium = new Vehicle("DEF-1234", med);
    Vehicle* testCarLarge = new Vehicle("GHI-1234", lg);
    
    /* TEST: Assign vehicle to first available suitable parking space (if available). */
    cout << "Assigning small vehicle to space: " << garage.getSpace(testCarSmall) << endl;
    cout << "Assigning medium vehicle to space: " << garage.getSpace(testCarMedium) << endl;
    cout << "Assigning large vehicle to space: " << garage.getSpace(testCarLarge) << endl;
    
    /* TEST: Allow customer to determine where they parked by entering their license plate. */
    garage.findVehicleByLicensePlate("ABC-1234");
    garage.findVehicleByLicensePlate("ABC-9999");
    
    /* TEST: Print space availability After space assignment. */
    /*
    garage.PrintOpenSpaceCount();
    */
    
    /* TEST: Initial three vehicles leave garage. Spaces replaced on stack and hash tables updated. */
    garage.onVehicleExit(testCarSmall->license);
    garage.onVehicleExit(testCarMedium->license);
    garage.onVehicleExit(testCarLarge->license);
    /*
    if (garage.mapVehicleLicenseToSpace.find(testCarSmall->license) == garage.mapVehicleLicenseToSpace.end()) cout << testCarSmall->license << " deleted." << endl;
    if (garage.mapVehicleLicenseToSpace.find(testCarMedium->license) == garage.mapVehicleLicenseToSpace.end()) cout << testCarMedium->license << " deleted." << endl;
    if (garage.mapVehicleLicenseToSpace.find(testCarLarge->license) == garage.mapVehicleLicenseToSpace.end()) cout << testCarLarge->license << " deleted." << endl;
    garage.PrintOpenSpaceCount();
    */
    
    /* Main menu loop. Allows unlimited testing of garage features. Some functionality still to be implemented. */
    int selection;
    while (1) {
        printMainMenu();
        cin >> selection;
        switch(selection) {
            case 1: {
                Vehicle* newVehicle = garage.createVehicle();
                cout << "Assigning " << newVehicle->license << " to space: " << garage.getSpace(newVehicle) << endl;
                break;
            }
            case 2: {
                string exitLicense;
                cout << "Enter license of exiting vehicle: ";
                cin >> exitLicense;
                if (garage.mapVehicleLicenseToSpace.find(exitLicense) != garage.mapVehicleLicenseToSpace.end()) {
                    garage.onVehicleExit(exitLicense);
                    if (garage.mapVehicleLicenseToSpace.find(exitLicense) == garage.mapVehicleLicenseToSpace.end()) cout << exitLicense << " deleted." << endl;
                }
                else cout << "Invalid License Provided" << endl;
                break;
            }
            case 3: {
                string licenseSearch;
                cout << "Enter license place: ";
                cin >> licenseSearch;
                garage.findVehicleByLicensePlate(licenseSearch);
                break;
            }
            case 4: {
                garage.PrintOpenSpaceCount();
                break;
            }
            default:
                return 0;
        }
    }
    
    return 0;
}