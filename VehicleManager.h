#ifndef VEHICLEMANAGER_H
#define VEHICLEMANAGER_H

#include <string>
#include <irrlicht.h>
#include <set>

class Vehicle;

typedef std::set<Vehicle*> vehicleSet_t;

class VehicleManager
{
public:
    static void initialize();
    static void finalize();
    
    static VehicleManager* getInstance() {return vehicleManager;}

private:
    static VehicleManager* vehicleManager;

public:
    VehicleManager();
    ~VehicleManager();
    
    void addVehicle(Vehicle* vehicle);
    void removeVehicle(Vehicle* vehicle);
    
    void pause();
    void resume();

private:
    vehicleSet_t        vehicleSet;
};

#endif // VEHICLEMANAGER_H
