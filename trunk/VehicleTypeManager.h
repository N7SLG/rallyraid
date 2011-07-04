
#ifndef VEHICLETYPEMANAGER_H
#define VEHICLETYPEMANAGER_H

#include <string>
#include <map>
#include <irrlicht.h>

class VehicleType;

class VehicleTypeManager
{
public:
    static void initialize();
    static void finalize();
    
    static VehicleTypeManager* getInstance() {return vehicleTypeManager;}

    typedef std::map<std::string, VehicleType*> vehicleTypeMap_t;

private:
    static VehicleTypeManager* vehicleTypeManager;

private:
    VehicleTypeManager();
    ~VehicleTypeManager();

    void read();

public:
    VehicleType* getVehicleType(const std::string& vehicleTypeName) {return vehicleTypeMap[vehicleTypeName];}
    const vehicleTypeMap_t& getVehicleTypeMap() {return vehicleTypeMap;}

private:
    vehicleTypeMap_t vehicleTypeMap;
};

#endif // VEHICLETYPEMANAGER_H
