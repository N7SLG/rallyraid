
#ifndef SHADERS_H
#define SHADERS_H

#include <irrlicht.h>
#include <map>
#include <string>


class Shaders
{
public:
    static void initialize();
    static void finalize();
    
    static Shaders* getInstance() {return shaders;}

private:
    static Shaders* shaders;
    
public:
    typedef std::map<std::string, irr::video::E_MATERIAL_TYPE> materialMap_t;

protected:
    Shaders();
    ~Shaders();

public:
    unsigned int getSupportedSMVersion() {return supportedSMVersion;}

    static irr::video::E_MATERIAL_TYPE stringToBaseType(const std::string& baseMaterialName);

private:
    void loadBaseMaterials();
    
protected:
    irr::video::IGPUProgrammingServices*    gpu;
    unsigned int                            supportedSMVersion;

// materials
public:
    materialMap_t                           materialMap;

};

#endif // SHADERS_H
