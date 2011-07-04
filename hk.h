#ifndef HK_H
#define HK_H

#include <Physics/Dynamics/World/hkpWorld.h>
#include <Physics/Dynamics/Entity/hkpRigidBody.h>
#include <Physics/Collide/Shape/HeightField/SampledHeightField/hkpSampledHeightFieldShape.h>
//#include <Physics/Collide/Shape/hkpShape.h>
#include <Physics/Collide/Shape/Convex/Box/hkpBoxShape.h>
#include <Physics/Collide/Shape/Convex/Cylinder/hkpCylinderShape.h>
#include <Physics/Collide/Shape/Convex/ConvexVertices/hkpConvexVerticesShape.h>
#include <Common/Internal/ConvexHull/hkGeometryUtility.h>
#include <Physics/Vehicle/hkpVehicleInstance.h>
#include <Physics/Vehicle/hkpVehicle.h>
#include <Physics/Collide/Filter/Group/hkpGroupFilter.h>

class hk
{
public:
    class materialType
    {
    public:
        const static unsigned int vehicleId     = 1;
        const static unsigned int wheelId       = 2;
        const static unsigned int treeId        = 3;
        const static unsigned int terrainId     = 4;
    };
public:
    static void initialize();
    static void finalize();
    static void step(float step_sec);

    static void lock()
    {
        //hkWorld->lock();
        hkWorld->markForWrite();
    }

    static void unlock()
    {
        hkWorld->unmarkForWrite();
        //hkWorld->unlock();
    }

    static void lockRead()
    {
        //hkWorld->lock();
        hkWorld->markForRead();
    }

    static void unlockRead()
    {
        hkWorld->unmarkForRead();
        //hkWorld->unlock();
    }

public:
    static hkpWorld*        hkWorld;

private:
    static hkJobThreadPool* threadPool;
    static int              totalNumThreadsUsed;
    static hkJobQueue*      jobQueue;
};

#endif // HK_H
