
#include "RoadType.h"
#include "TheGame.h"

RoadType::RoadType(FILE* f, bool& ret)
    : name(),
      slicePoints(),
      sliceIndices(),
      texture(0),
      frictionMulti(0.0f),
      tRate(0.0f)
{
    ret = read(f) && !name.empty();
}

RoadType::~RoadType()
{
}

bool RoadType::read(FILE* f)
{
    char cname[256];
    char textureName[256];
    float f1, f2;
    unsigned int num;
    int ret = 0;
    unsigned int ind;

    slicePoints.clear();
    sliceIndices.clear();
    tRate = 0.f;
    memset(cname, 0, sizeof(cname));
    memset(textureName, 0, sizeof(textureName));

    ret = fscanf_s(f, "name: %s\n", cname, 255);
    if (ret < 1)
    {
        printf("error reading name from road types file\n");
        return false;
    }
    name = cname;

    ret = fscanf_s(f, "texture: %s\n", textureName, 255);
    if (ret < 1)
    {
        printf("error reading texture name from road types file\n");
        return false;
    }

    if (strcmp(textureName, "null") == 0)
    {
        return true;
    }

    TheGame::getInstance()->getDriver()->setTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS, false);
    texture = TheGame::getInstance()->getDriver()->getTexture(textureName);
    TheGame::getInstance()->getDriver()->setTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS, true);

    ret = fscanf_s(f, "friction_multi: %f\n", &frictionMulti);
    if (ret < 1)
    {
        printf("error reading firction multiplier from road types file\n");
        return false;
    }
    
    ret = fscanf_s(f, "num_of_slice_points: %u\n", &num);
    if (ret < 1)
    {
        printf("error reading num of slice points from road types file\n");
        return false;
    }

    slicePoints.resize(num);
    for (unsigned int i = 0; i < num; i++)
    {
        ret = fscanf_s(f, "%f %f\n", &f1, &f2);
        if (ret < 2 )
        {
            printf("error reading %u. slice point from road types file\n", i);
            return false;
        }
        slicePoints[i] = irr::core::vector2df(f1, f2);
        if (0 < i) tRate += (slicePoints[i] - slicePoints[i-1]).getLength();
    }
    
    ret = fscanf_s(f, "num_of_slice_indices: %u\n", &num);
    if (ret < 1)
    {
        printf("error reading num of slice indices from road types file\n");
        return false;
    }
    
    sliceIndices.resize(num);
    for (unsigned int i = 0; i < num; i++)
    {
        ret = fscanf_s(f, "%u\n", &ind);
        if (ret < 1 )
        {
            printf("error reading %u. slice index from road types file\n", i);
            return false;
        }
        sliceIndices[i] = ind;
    }
    return true;
}

