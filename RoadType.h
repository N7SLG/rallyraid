
#ifndef ROADTYPE_H
#define ROADTYPE_H

#include <string>
#include <vector>
#include <irrlicht.h>

class RoadType
{
public:
    RoadType(FILE* f, bool& ret);
    ~RoadType();

    const std::string& getName(); // inline

private:
    bool read(FILE* f);

private:
    std::string                         name;

    std::vector<irr::core::vector2df>   slicePoints;
    std::vector<unsigned int>           sliceIndices;
    irr::video::ITexture*               texture;
    float                               frictionMulti;
    float                               tRate;

    friend class Road;
    friend class VisualRoad;
};


inline const std::string& RoadType::getName()
{
    return name;
}

#endif // ROADTYPE_H
