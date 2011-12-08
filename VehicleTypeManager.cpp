

#include "VehicleTypeManager.h"
#include "VehicleType.h"
#include "ConfigFile.h"
#include "ConfigDirectory.h"
#include "stdafx.h"
#include "LoadingThread.h"

VehicleTypeManager* VehicleTypeManager::vehicleTypeManager = 0;

void VehicleTypeManager::initialize()
{
    if (vehicleTypeManager == 0)
    {
        vehicleTypeManager = new VehicleTypeManager();
    }
}

void VehicleTypeManager::finalize()
{
    if (vehicleTypeManager)
    {
        delete vehicleTypeManager;
        vehicleTypeManager = 0;
    }
}

VehicleTypeManager::VehicleTypeManager()
    : vehicleTypeMap()
{
    read();
}

VehicleTypeManager::~VehicleTypeManager()
{
    for (vehicleTypeMap_t::iterator it = vehicleTypeMap.begin();
         it != vehicleTypeMap.end();
         it++)
    {
        delete it->second;
    }
    vehicleTypeMap.clear();
}

void VehicleTypeManager::read()
{

#if 0
    ConfigFile cf;
    cf.load("data/vehicles/vehicletypes.cfg");

    dprintf(MY_DEBUG_NOTE, "Read vehicle types file:\n");
    // Go through all sections & settings in the file
    ConfigFile::SectionIterator seci = cf.getSectionIterator();

    std::string objectName, vehicleTypeName, vehicleTypeFilename;
    while (seci.hasMoreElements())
    {
        objectName = seci.peekNextKey();
        dprintf(MY_DEBUG_NOTE, "\tObject: %s\n", objectName.c_str());
        ConfigFile::SettingsMultiMap *settings = seci.getNext();
        for (ConfigFile::SettingsMultiMap::iterator i = settings->begin(); i != settings->end(); ++i)
        {
            vehicleTypeName = i->first;
            vehicleTypeFilename = i->second;
            
            if (vehicleTypeName != "" && vehicleTypeFilename != "")
            {
                bool ret = false;
                VehicleType* vehicleType = new VehicleType(vehicleTypeName, vehicleTypeFilename, ret);
                if (!ret)
                {
                    delete vehicleType;
                }
                else
                {
                    vehicleTypeMap[vehicleTypeName] = vehicleType;
                }
            }
        }
        
    }
#else
    ConfigDirectory::fileList_t fileList;
    
    dprintf(MY_DEBUG_NOTE, "Read vehicle types directory:\n");

    bool ret = ConfigDirectory::load("data/vehicles", "", fileList);
    
    if (!ret)
    {
        dprintf(MY_DEBUG_WARNING, "unable to read vehicle directory\n");
        return;
    }
    
    LoadingThread::getInstance()->setSmallStepCount(fileList.size());

    for (ConfigDirectory::fileList_t::const_iterator it = fileList.begin();
         it != fileList.end();
         it++)
    {
        std::string vehicleTypeName = it->c_str();
        std::string vehicleTypeDir = std::string("data/vehicles/")+vehicleTypeName;
        std::string vehicleTypeFilename = vehicleTypeDir+"/"+vehicleTypeName+".cfg";
        VehicleType* vehicleType = new VehicleType(vehicleTypeName, vehicleTypeFilename, vehicleTypeDir, ret);
        if (!ret)
        {
            delete vehicleType;
        }
        else
        {
            vehicleTypeMap[vehicleTypeName] = vehicleType;
            LoadingThread::getInstance()->stepSmall();
        }
    }
//    assert(0);
#endif
}

