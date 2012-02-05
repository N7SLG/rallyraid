
#include "hk.h"
#include "stdafx.h"

#include <stdio.h>
#include <assert.h>
#include <Physics/Dynamics/World/hkpWorldCinfo.h>
#include <Common/Base/System/hkBaseSystem.h>
#include <Common/Base/Memory/System/hkMemorySystem.h>
#include <Common/Base/Memory/Allocator/Malloc/hkMallocAllocator.h>

#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>
#include <Common/Base/System/Error/hkDefaultError.h>
#include <Common/Base/Memory/MemoryClasses/hkMemoryClassDefinitions.h>

#include <Common/Base/Thread/Job/ThreadPool/Cpu/hkCpuJobThreadPool.h>
#include <Common/Base/Thread/Job/ThreadPool/Spu/hkSpuJobThreadPool.h>
#include <Common/Base/Thread/JobQueue/hkJobQueue.h>

#include <Physics/Collide/Dispatch/hkpAgentRegisterUtil.h>

#include <Common/Base/keycode.cxx>

#undef HK_FEATURE_PRODUCT_AI
#undef HK_FEATURE_PRODUCT_ANIMATION
#undef HK_FEATURE_PRODUCT_CLOTH
#undef HK_FEATURE_PRODUCT_DESTRUCTION
#undef HK_FEATURE_PRODUCT_BEHAVIOR
#define HK_EXCLUDE_LIBRARY_hkgpConvexDecomposition
#define HK_FEATURE_REFLECTION_PHYSICS
#define HK_CLASSES_FILE <Common/Serialize/Classlist/hkClasses.h>
#include <Common/Base/Config/hkProductFeatures.cxx>

/*
#ifdef _MSC_VER
#pragma comment(lib, "hkBase.lib")
#pragma comment(lib, "hkSerialize.lib")
#pragma comment(lib, "hkSceneData.lib")
#pragma comment(lib, "hkInternal.lib")
#pragma comment(lib, "hkGeometryUtilities.lib")
#pragma comment(lib, "hkVisualize.lib")
#pragma comment(lib, "hkCompat.lib")
#pragma comment(lib, "hkpCollide.lib")
#pragma comment(lib, "hkpConstraintSolver.lib")
#pragma comment(lib, "hkpDynamics.lib")
#pragma comment(lib, "hkpInternal.lib")
#pragma comment(lib, "hkpUtilities.lib")
#pragma comment(lib, "hkpVehicle.lib")
#pragma comment(lib, "hkaAnimation.lib")
#pragma comment(lib, "hkaInternal.lib")
#pragma comment(lib, "hkaRagdoll.lib")
#pragma comment(lib, "hkgBridge.lib")
#pragma comment(lib, "hkgCommon.lib")
#pragma comment(lib, "hkgDx9.lib")
#pragma comment(lib, "hkgDx9s.lib")
#pragma comment(lib, "hkgDx10.lib")
#pragma comment(lib, "hkgOgl.lib")
#pragma comment(lib, "hkgOgls.lib")
#pragma comment(lib, "hksCommon.lib")
#pragma comment(lib, "hksXAudio2.lib")
#endif // _MSC_VER
*/

hkpWorld* hk::hkWorld = 0;
hkJobThreadPool* hk::threadPool;
int hk::totalNumThreadsUsed;
hkJobQueue* hk::jobQueue;
MyLock* hk::hkLock = 0;

static void HK_CALL errorReportFunction(const char* str, void*)
{
    printf("%s", str);
}

#include <Common/Base/UnitTest/hkUnitTest.h>

extern hkTestEntry* hkUnitTestDatabase;
hkTestEntry* hkUnitTestDatabase;

hkBool HK_CALL hkTestReport(hkBool32 cond, const char* desc, const char* file, int line)
{
    printf("hkTestReport: %s (%d)\n\t%s\n", file, line, desc);
    return cond != false;
}



void hk::initialize()
{
    dprintf(MY_DEBUG_NOTE, "initialize Havok\n");
    dprintf(MY_DEBUG_NOTE, "initialize Havok: initPlatform\n");
#if !defined(HK_PLATFORM_WIN32)
    extern void initPlatform();
    initPlatform();
#endif
    hkMemorySystem::FrameInfo frameInfo(6*1024*1024);

    dprintf(MY_DEBUG_NOTE, "initialize Havok: memory list\n");
    hkMemoryRouter* memoryRouter = hkMemoryInitUtil::initDefault(hkMallocAllocator::m_defaultMallocAllocator, frameInfo);

    dprintf(MY_DEBUG_NOTE, "initialize Havok: memory\n");
    hkResult res = hkBaseSystem::init(memoryRouter, errorReportFunction);
    dprintf(MY_DEBUG_NOTE, "initialize Havok: memory: %s\n", res == HK_SUCCESS?"Ok":"Not Ok");
    assert(res == HK_SUCCESS);

    // Get the number of physical threads available on the system
    dprintf(MY_DEBUG_NOTE, "initialize Havok: get HW info\n");
    hkHardwareInfo hwInfo;
    hkGetHardwareInfo(hwInfo);
    totalNumThreadsUsed = hwInfo.m_numThreads;
    dprintf(MY_DEBUG_NOTE, "initialize Havok: get HW info: threads: %d\n", totalNumThreadsUsed);

    // We use one less than this for our thread pool, because we must also use this thread for our simulation
    hkCpuJobThreadPoolCinfo threadPoolCinfo;
    threadPoolCinfo.m_numThreads = totalNumThreadsUsed - 1;

    // This line enables timers collection, by allocating 200 Kb per thread.  If you leave this at its default (0),
    // timer collection will not be enabled.
    //threadPoolCinfo.m_timerBufferPerThreadAllocation = 200000;
    dprintf(MY_DEBUG_NOTE, "initialize Havok: new job thread\n");
    threadPool = new hkCpuJobThreadPool( threadPoolCinfo );

    // We also need to create a Job queue. This job queue will be used by all Havok modules to run multithreaded work.
    // Here we only use it for physics.
    hkJobQueueCinfo info;
    info.m_jobQueueHwSetup.m_numCpuThreads = totalNumThreadsUsed;
    dprintf(MY_DEBUG_NOTE, "initialize Havok: new job queue\n");
    jobQueue = new hkJobQueue(info);

    //
    // Enable monitors for this thread.
    //

    // Monitors have been enabled for thread pool threads already (see above comment).
    //printf("initialize Havok: monitor stream resize\n");
    //hkMonitorStream::getInstance().resize(200000);

    dprintf(MY_DEBUG_NOTE, "initialize Havok: world\n");
    hkpWorldCinfo winfo;
    //winfo.m_simulationType = hkpWorldCinfo::SIMULATION_TYPE_DISCRETE;
    winfo.m_simulationType = hkpWorldCinfo::SIMULATION_TYPE_MULTITHREADED;
    winfo.m_enableDeactivation = false;
    winfo.m_collisionTolerance = 0.1f;
    winfo.m_gravity.set(0.0f, -9.8f, 0.0f);
    winfo.setBroadPhaseWorldSize(20050.0f);
    winfo.setupSolverInfo(hkpWorldCinfo::SOLVER_TYPE_4ITERS_MEDIUM);
    hkWorld = new hkpWorld(winfo);

    //hkLock = new MyLock();

    hk::lock();
// Register all agents.
    hkpAgentRegisterUtil::registerAllAgents(hkWorld->getCollisionDispatcher());
    hkWorld->registerWithJobQueue(jobQueue);
    hkpGroupFilter* filter = new hkpGroupFilter();
    filter->disableCollisionsBetween(materialType::vehicleId, materialType::wheelId);
    hkWorld->setCollisionFilter(filter);
    filter->removeReference();
    hk::unlock();

    dprintf(MY_DEBUG_NOTE, "initialize Havok end\n");
}

void hk::finalize()
{
    hk::lock();
    if (hkWorld)
    {
        hkBaseSystem::quit();
        hkResult result = hkMemoryInitUtil::quit();
        hkWorld = 0;
    }
    //delete hkLock;
//    delete jobQueue;
//    threadPool->removeReference();
    //printf("Base system shut down.\n");
    //return result;
}

void hk::step(float step_sec)
{
    //hkWorld->stepDeltaTime(step_sec);
    //hkLock->lock();
    hkWorld->stepMultithreaded(jobQueue, threadPool, step_sec);
    //hkLock->unlock();
}
