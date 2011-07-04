
#ifndef AIPOINT_H
#define AIPOINT_H

#include <irrlicht.h>
#include "ObjectWireGlobalObject.h"
#include <list>



class AIPoint : public ObjectWireGlobalObject
{
public:
    typedef std::list<AIPoint*> AIPointList_t;

public:
    AIPoint(const irr::core::vector3df& apos,
        float globalDistance,
        float localDistance);
    virtual ~AIPoint();

    static void readAIPointList(const std::string& AIPointListFilename, AIPointList_t& AIPointList);
    static bool writeAIPointList(const std::string& AIPointListFilename, const AIPointList_t& AIPointList);
    static void clearAIPointList(AIPointList_t& AIPointList);

    virtual void editorRender(bool last);

    float getGlobalDistance() const; // inline
    float getLocalDistance() const; // inline
    
private:
    //virtual void updateVisible();
    static void editorRenderAIPointList(const AIPointList_t& AIPointList);

private:
    float                   globalDistance;
    float                   localDistance;


    friend class MenuPageEditor;
};


inline float AIPoint::getGlobalDistance() const
{
    return globalDistance;
}

inline float AIPoint::getLocalDistance() const
{
    return localDistance;
}


#endif // AIPOINT_H
