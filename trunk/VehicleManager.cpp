#include "VehicleManager.h"
#include "Vehicle.h"
#include "stdafx.h"

VehicleManager* VehicleManager::vehicleManager = 0;

void VehicleManager::initialize()
{
    if (!vehicleManager)
    {
        vehicleManager = new VehicleManager();
    }
}

void VehicleManager::finalize()
{
    if (vehicleManager)
    {
        delete vehicleManager;
        vehicleManager = 0;
    }
}
    

VehicleManager::VehicleManager()
    : vehicleSet()
{
}

VehicleManager::~VehicleManager()
{
    // TODO: remove here or not?
    vehicleSet.clear();
}

void VehicleManager::addVehicle(Vehicle* vehicle)
{
    vehicleSet.insert(vehicle);
}

void VehicleManager::removeVehicle(Vehicle* vehicle)
{
    vehicleSet.erase(vehicle);
}

void VehicleManager::pause()
{
    dprintf(MY_DEBUG_NOTE, "VehicleManager::pause(): vehicle num: %lu\n", vehicleSet.size());

    for (vehicleSet_t::const_iterator vit = vehicleSet.begin();
         vit != vehicleSet.end();
         vit++)
    {
        (*vit)->pause();
    }
}

void VehicleManager::resume()
{
    dprintf(MY_DEBUG_NOTE, "VehicleManager::resume(): vehicle num: %lu\n", vehicleSet.size());

    for (vehicleSet_t::const_iterator vit = vehicleSet.begin();
         vit != vehicleSet.end();
         vit++)
    {
        (*vit)->resume();
    }
}

