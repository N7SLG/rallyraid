
#include "VehicleType.h"
#include "Vehicle.h"
#include "VehicleManager.h"
#include "VehicleTypeManager.h"
#include "ObjectPoolManager.h"
#include "MySound.h"
#include "stdafx.h"
#include "Shaders.h"
#include "TheEarth.h"
#include "TheGame.h"
#include "Settings.h"

#include <Physics/Vehicle/WheelCollide/RayCast/hkpVehicleRayCastWheelCollide.h>
#include <Physics/Vehicle/hkpVehicleInstance.h>
#include <Physics/Collide/Query/CastUtil/hkpWorldRayCastOutput.h>
#include <Physics/Dynamics/Collide/ContactListener/hkpContactListener.h>

// normalize angle between 0 and 360
static float normalizeAngle(float &angle)
{
    while (angle >= 360.f) angle -= 360.f;
    while (angle < 0.f) angle += 360.f;
    return angle;
}

static float normalizeAngle180(float &angle)
{
    while (angle > 180.f) angle -= 360.f;
    while (angle < -180.f) angle += 360.f;
    return angle;
}

// -------------------------------------------------------
//                   Collision handler
// -------------------------------------------------------

class VehicleCollisionListener : public hkReferencedObject, public hkpContactListener
{
public:
    VehicleCollisionListener(Vehicle* vehicle)
        : vehicle(vehicle), hitSound(0), hitSoundGround(0), lastTick(0), lastTickVehicle(0)
    {
        hitSound = MySoundEngine::getInstance()->play3D("data/sounds/puff_02.wav", irr::core::vector3df(), false, true, true);
        if (hitSound)
        {
            hitSound->setVolume(0.2f);
            hitSound->setMinDistance(3.0f);
        }
        hitSoundGround = MySoundEngine::getInstance()->play3D("data/sounds/puff.wav", irr::core::vector3df(), false, true, true);
        if (hitSoundGround)
        {
            hitSoundGround->setVolume(0.2f);
            hitSoundGround->setMinDistance(3.0f);
        }
    }
    
    virtual ~VehicleCollisionListener()
    {
        if (hitSound)
        {
            delete hitSound;
            hitSound = 0;
        }
        if (hitSoundGround)
        {
            delete hitSoundGround;
            hitSoundGround = 0;
        }
    }
    
private:
    virtual void contactPointCallback( const hkpContactPointEvent& event ) 
    {
        assert(event.m_source != hkpCollisionEvent::SOURCE_WORLD && "Do not add this listener to the world.");

        if (event.m_source != hkpCollisionEvent::SOURCE_A) return;

        unsigned int tick = TheGame::getInstance()->getTick();

        if (tick - lastTick < 500) return;
        
        irr::core::vector3df point(event.m_contactPoint->getPosition()(0),
            event.m_contactPoint->getPosition()(1),
            event.m_contactPoint->getPosition()(2));
        irr::core::vector3df normal(event.m_contactPoint->getNormal()(0),
            event.m_contactPoint->getNormal()(1),
            event.m_contactPoint->getNormal()(2));
        float fourth = event.m_contactPoint->getPosition()(3);
        float distance = event.m_contactPoint->getNormal()(3);

        dprintf(MY_DEBUG_NOTE, "Vehicle collision:\n" \
            "\tpoint: %f %f %f\n" \
            "\tnormal: %f %f %f\n" \
            "\tfourth: %f\n" \
            "\tdistance: %f\n" \
            "\tsource: %u\n" \
            "\ttick: %u\n" \
            "\tlastTick: %u\n"
            , point.X, point.Y, point.Z
            , normal.X, normal.Y, normal.Z,
            fourth,
            distance,
            event.m_source,
            tick,
            lastTick
            );

        lastTick = tick;
        assert(event.m_source == hkpCollisionEvent::SOURCE_A);

        hkpRigidBody* body = event.getBody(event.m_source);
        assert(vehicle->hkBody == body);

        hkpRigidBody* otherBody = event.getBody(1-event.m_source);
        Vehicle* otherVehicle = 0;

        float speed = 0.0f;
        MySound* sound = hitSound;
        if (otherBody->hasProperty(hk::materialType::vehicleId))
        {
            if (otherBody->areContactListenersAdded())
            {
                otherVehicle = ((VehicleCollisionListener*)otherBody->getContactListeners()[0])->vehicle;
            }
        }
        else if (otherBody->hasProperty(hk::materialType::terrainId))
        {
            sound = hitSoundGround;
        }

        if (otherVehicle)
        {
            speed = (otherVehicle->getLinearVelocity()-vehicle->getLinearVelocity()).getLength() * 3.6f;
            otherVehicle->collisionListener->lastTick = tick;
        }
        else
        {
            speed = vehicle->getLinearVelocity().getLength() * 3.6f;
        }
        
        if (sound)
        {
            // play sound
            float soundVolume = speed / 130.f;
            if (soundVolume > 1.0f) soundVolume = 1.0f;
            sound->setVolume(soundVolume);
            sound->setPosition(point);
            sound->play();
        }

        // call vehicle collision
        irr::core::matrix4 mat = vehicle->matrix;
        mat.makeInverse();
        irr::core::vector3df localPoint;
        irr::core::vector3df localNormal;
        irr::core::vector3df localLinearVelocity;

        mat.transformVect(localPoint, point);
        mat.setTranslation(irr::core::vector3df());
        mat.transformVect(localNormal, normal);
        mat.transformVect(localLinearVelocity, vehicle->getLinearVelocity());

        dprintf(MY_DEBUG_NOTE, "local collision data:\n" \
            "\tpoint: %f %f %f\n" \
            "\tnormal: %f, %f, %f\n" \
            "\tspeed: %f\n"
            , localPoint.X, localPoint.Y, localPoint.Z
            , localNormal.X, localNormal.Y, localNormal.Z,
            speed
            );

        // check for penalties
        if (vehicle && otherVehicle &&
            tick - lastTickVehicle > 5000 &&
            tick - otherVehicle->collisionListener->lastTickVehicle > 5000)
        {
            assert(vehicle != otherVehicle);
            lastTickVehicle = otherVehicle->collisionListener->lastTickVehicle = tick;
            checkTwoVehicleCollision(vehicle, localPoint, localLinearVelocity, otherVehicle, point, speed);
        }
    }

private:
    /*
        first edition:
        
            1. determine the hit points for both vehicle in their local space
            2. if the hitpoint vector's angle is smaller than 45 degrees and larger
               than -45 degrees, the vehicle is the blame. so need to give penalty
            3. if both blame, then the w is 0, which means the player will get the
               penalty
    */

    static void checkTwoVehicleCollision(Vehicle* vehicle, const irr::core::vector3df& localPoint, const irr::core::vector3df& localLinearVelocity,
        Vehicle* otherVehicle, const irr::core::vector3df& point, float hit)
    {
        // do not use speed and direction, use instead the linear velocity
        if (vehicle->vehicleCollisionCB)
        {
            vehicle->vehicleCollisionCB->handleSoftCollision(1.0f);
        }
        if (otherVehicle->vehicleCollisionCB)
        {
            otherVehicle->vehicleCollisionCB->handleSoftCollision(-1.0f);
        }

        float w = hit / 130.f;
        if (w > 1.0f) w = 1.0f;

        irr::core::matrix4 mat = otherVehicle->matrix;
        mat.makeInverse();
        irr::core::vector3df otherLocalPoint;
        mat.transformVect(otherLocalPoint, point);

        irr::core::vector2df localPoint2D(localPoint.X, localPoint.Z);
        irr::core::vector2df otherLocalPoint2D(otherLocalPoint.X, otherLocalPoint.Z);

        bool blame = false;
        bool otherBlame = false;
        int blameCnt = 0;

        float angle = (float)localPoint2D.getAngle();
        float otherAngle = (float)otherLocalPoint2D.getAngle();
        normalizeAngle180(angle);
        normalizeAngle180(otherAngle);
        angle = fabsf(angle);
        otherAngle = fabsf(otherAngle);

        dprintf(MY_DEBUG_NOTE, "collision vehicles, angle: %f, other angle: %f\n", angle, otherAngle);

        //if (angle < 45.f) {blame = true; blameCnt++;}
        //if (otherAngle < 45.f) {otherBlame = true; blameCnt++;}
        if (angle < 45.f && (angle < 10.f || angle <= otherAngle)) {blame = true; blameCnt++;}
        if (otherAngle < 45.f && (otherAngle < 10.f || angle > otherAngle)) {otherBlame = true; blameCnt++;}

        if (blameCnt > 1) w = 0.0f;
        //w /= blameCnt;

        if (blame && vehicle->vehicleCollisionCB)
        {
            dprintf(MY_DEBUG_NOTE, "vehicle collision, w: %f\n", w);
            vehicle->vehicleCollisionCB->handleHardCollision(w);
        }
        if (otherBlame && otherVehicle->vehicleCollisionCB)
        {
            dprintf(MY_DEBUG_NOTE, "other vehicle collision, w: %f\n", w);
            otherVehicle->vehicleCollisionCB->handleHardCollision(w);
        }
    }

    /*
        second edition
    */
/*
    static void checkTwoVehicleCollision(Vehicle* vehicle, const irr::core::vector3df& localPoint, const irr::core::vector3df& localLinearVelocity,
        Vehicle* otherVehicle, const irr::core::vector3df& point, float hit)
    {
        // do not use speed and direction, use instead the linear velocity
        if (vehicle->vehicleCollisionCB)
        {
            vehicle->vehicleCollisionCB->handleSoftCollision(1.0f);
        }
        if (otherVehicle->vehicleCollisionCB)
        {
            otherVehicle->vehicleCollisionCB->handleSoftCollision(-1.0f);
        }

        float w = hit / 130.f;
        if (w > 1.0f) w = 1.0f;

        irr::core::matrix4 mat = otherVehicle->matrix;
        mat.makeInverse();
        irr::core::vector3df otherLocalPoint;
        mat.transformVect(otherLocalPoint, point);

        irr::core::vector3df otherLocalLinearVelocity;
        mat.setTranslation(irr::core::vector3df());
        mat.transformVect(otherLocalLinearVelocity, otherVehicle->getLinearVelocity());

        const irr::core::vector2df localPoint2D(localPoint.X, localPoint.Z);
        const irr::core::vector2df otherLocalPoint2D(otherLocalPoint.X, otherLocalPoint.Z);
        const irr::core::vector2df localLinearVelocity2D(localLinearVelocity.X, localLinearVelocity.Z);
        const irr::core::vector2df otherLocalLinearVelocity2D(otherLocalLinearVelocity.X, otherLocalLinearVelocity.Z);

        bool blame = false;
        bool otherBlame = false;
        int blameCnt = 0;

        float angle = (float)(localPoint2D.getAngle() - localLinearVelocity2D.getAngle());
        float otherAngle = (float)(otherLocalPoint2D.getAngle() - otherLocalLinearVelocity2D.getAngle());
        normalizeAngle180(angle);
        normalizeAngle180(otherAngle);
        angle = fabsf(angle);
        otherAngle = fabsf(otherAngle);

        dprintf(MY_DEBUG_NOTE, "collision vehicles, angle: %f, other angle: %f\n", angle, otherAngle);

        if (angle < 60.f && (angle < 10.f || angle <= otherAngle)) {blame = true; blameCnt++;}
        if (otherAngle < 60.f && (otherAngle < 10.f || angle > otherAngle)) {otherBlame = true; blameCnt++;}

        if (blameCnt > 1) w = 0.0f;
        //w /= blameCnt;

        if (blame && vehicle->vehicleCollisionCB)
        {
            dprintf(MY_DEBUG_NOTE, "vehicle collision, w: %f\n", w);
            vehicle->vehicleCollisionCB->handleHardCollision(w);
        }
        if (otherBlame && otherVehicle->vehicleCollisionCB)
        {
            dprintf(MY_DEBUG_NOTE, "other vehicle collision, w: %f\n", w);
            otherVehicle->vehicleCollisionCB->handleHardCollision(w);
        }
    }
*/
private:
    Vehicle*        vehicle;
    MySound*        hitSound;
    MySound*        hitSoundGround;
    unsigned int    lastTick;
    unsigned int    lastTickVehicle;
};

// -------------------------------------------------------
//                   Friction helper
// -------------------------------------------------------

extern const class hkClass FrictionMapVehicleRaycastWheelCollideClass;

class FrictionMapVehicleRaycastWheelCollide : public hkpVehicleRayCastWheelCollide
{
public:
    HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE);

private:
    virtual void wheelCollideCallback(const hkpVehicleInstance* hkVehicle, hkUint8 wheelIndex, CollisionDetectionWheelOutput& cdInfo)
    {
        Vehicle* vehicle = reinterpret_cast<Vehicle*>(hkVehicle->getUserData());
        vehicle->tyres[wheelIndex]->hitBody = cdInfo.m_contactBody;
    }

// Serialization.
public:
    // By adding HK_DECLARE_REFLECTION, we enable objects of this class to be serialized.
    // However, the class does need to be registered with the type registry. The following code can be used:
    // hkBuiltinTypeRegistry::getInstance().addType( &FrictionMapVehicleRaycastWheelCollideTypeInfo, &FrictionMapVehicleRaycastWheelCollideClass );
    HK_DECLARE_REFLECTION();

    FrictionMapVehicleRaycastWheelCollide() { }

    FrictionMapVehicleRaycastWheelCollide(hkFinishLoadedObjectFlag f) : hkpVehicleRayCastWheelCollide(f) {}
};

// -------------------------------------------------------
//                   Transmission
// -------------------------------------------------------

extern const class hkClass VehicleTransmissionClass;

class VehicleTransmission : public hkpVehicleDefaultTransmission
{
public:
    HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE);
    HK_DECLARE_REFLECTION();
    
    /// Default constructor
    VehicleTransmission(Vehicle* vehicle, bool manual = false, bool sequential = true)
        : hkpVehicleDefaultTransmission(), vehicle(vehicle), manual(manual), sequential(sequential), gear(0)
    {
    }
        
    VehicleTransmission(hkFinishLoadedObjectFlag f, Vehicle* vehicle, bool manual = false, bool sequential = true)
        : hkpVehicleDefaultTransmission(f), vehicle(vehicle), manual(manual), sequential(sequential), gear(0)
    {
    }

    /// Calculates information about the effects of transmission on the vehicle.
    virtual void calcTransmission(const hkReal deltaTime, const hkpVehicleInstance* hkVehicle, TransmissionOutput& TransmissionOutputOut)
    {
        if (manual)
        {
            // TODO
            if (gear < 0)
            {
                TransmissionOutputOut.m_isReversing = true;
                TransmissionOutputOut.m_currentGear = 0;
            }
            else
            {
                TransmissionOutputOut.m_isReversing = false;
                if (gear)
                {
                    TransmissionOutputOut.m_currentGear = gear - 1;
                }
                else
                {
                    TransmissionOutputOut.m_currentGear = 0;
                }
            }
            
            hkReal transTorque = 0.0f;
            if (gear != 0)
            {
                transTorque = calcMainTransmittedTorque(hkVehicle, TransmissionOutputOut) * (1.0f - vehicle->clutch);
            }
            TransmissionOutputOut.m_mainTransmittedTorque = transTorque;

            hkReal rpm = calcTransmissionRPM(hkVehicle, TransmissionOutputOut);
            TransmissionOutputOut.m_transmissionRPM = rpm;
            
            assert(m_wheelsTorqueRatio.getSize() == TransmissionOutputOut.m_numWheelsTramsmittedTorque);
            
            for (char i = 0; i < TransmissionOutputOut.m_numWheelsTramsmittedTorque; i++)
            {
                TransmissionOutputOut.m_wheelsTransmittedTorque[i] = transTorque * m_wheelsTorqueRatio[i];
            }

            TransmissionOutputOut.m_delayed = false;
            TransmissionOutputOut.m_clutchDelayCountdown = 0.0f;
        }
        else
        {
            //hkpVehicleDefaultTransmission::calcTransmission(deltaTime, hkVehicle, TransmissionOutputOut);

            hkBool isRev = calcIsReversing(hkVehicle, TransmissionOutputOut);   
            TransmissionOutputOut.m_isReversing = isRev;

            hkReal transTorque = calcMainTransmittedTorque(hkVehicle, TransmissionOutputOut);
            TransmissionOutputOut.m_mainTransmittedTorque = transTorque;

            hkReal rpm = calcTransmissionRPM(hkVehicle, TransmissionOutputOut);
            TransmissionOutputOut.m_transmissionRPM = rpm;
            
            assert(m_wheelsTorqueRatio.getSize() == TransmissionOutputOut.m_numWheelsTramsmittedTorque);
            
            for (char i = 0; i < TransmissionOutputOut.m_numWheelsTramsmittedTorque; i++)
            {
                TransmissionOutputOut.m_wheelsTransmittedTorque[i] = transTorque * m_wheelsTorqueRatio[i];
            }
 
            updateCurrentGear(deltaTime, hkVehicle, TransmissionOutputOut);
        }
    }
    

public:
    Vehicle*    vehicle;
    bool        manual;
    bool        sequential;
    hkInt8      gear;
};

// -------------------------------------------------------
//                   Smoke class
// -------------------------------------------------------
#define MAX_SMOKES 50

class Smoke
{
public:
    Smoke(const float speed, const irr::core::vector3df& pos, float offset/*, float p_waterHeight*/);
    void renew(const float speed, const irr::core::vector3df& pos, float offset/*, float p_waterHeight*/);
    
private:
    irr::scene::IBillboardSceneNode*    node;
    float                               speed;
    int                                 animePhase;
    OffsetObject*                       offsetObject;


    friend class Vehicle;
};


// -------------------------------------------------------
//                     VehicleTyre
// -------------------------------------------------------

VehicleTyre::VehicleTyre(VehicleTypeTyre* vehicleTypeTyre, const irr::core::vector3df& apos, Vehicle* vehicle, unsigned int tyreNum)
    : vehicle(vehicle),
      tyreNum(tyreNum),
      localMatrix(),
      node(0),
      tyreType(vehicleTypeTyre),
      offsetObject(0),
      hitBody(0)
{
    offsetObject = ObjectPoolManager::getInstance()->getObject(tyreType->objectName, apos, tyreType->scale);
    node = offsetObject->getNode();
    
    node->setMaterialTexture(0, tyreType->texture);

    localMatrix.setTranslation(tyreType->localPos);

    irr::core::aabbox3d<float> bbox = node->getBoundingBox();
    width = (bbox.MaxEdge.Z-bbox.MinEdge.Z) * tyreType->scale.Z;
    radius = (bbox.MaxEdge.Y-bbox.MinEdge.Y) * tyreType->scale.Y * 0.5f;

    if (tyreType->steerable)
    {
        vehicle->hkVehicle->m_data->m_wheelParams[tyreNum].m_axle = 0;
    }
    else
    {
        vehicle->hkVehicle->m_data->m_wheelParams[tyreNum].m_axle = 1;
    }
    vehicle->hkVehicle->m_data->m_wheelParams[tyreNum].m_friction = tyreType->friction;
    vehicle->hkVehicle->m_data->m_wheelParams[tyreNum].m_slipAngle = 0.0f;
    vehicle->hkVehicle->m_data->m_wheelParams[tyreNum].m_radius = radius;
    vehicle->hkVehicle->m_data->m_wheelParams[tyreNum].m_width = width;
    vehicle->hkVehicle->m_data->m_wheelParams[tyreNum].m_mass = tyreType->mass;
    vehicle->hkVehicle->m_data->m_wheelParams[tyreNum].m_viscosityFriction = 0.25f;
    vehicle->hkVehicle->m_data->m_wheelParams[tyreNum].m_maxFriction = 2.0f * tyreType->friction;
    vehicle->hkVehicle->m_data->m_wheelParams[tyreNum].m_forceFeedbackMultiplier = 0.1f;
    vehicle->hkVehicle->m_data->m_wheelParams[tyreNum].m_maxContactBodyAcceleration = hkReal(vehicle->hkVehicle->m_data->m_gravity.length3()) * 2.0f;
}

VehicleTyre::~VehicleTyre()
{
    node = 0;

    ObjectPoolManager::getInstance()->putObject(offsetObject);
    offsetObject = 0;
}


// -------------------------------------------------------
//                       Vehicle
// -------------------------------------------------------

Vehicle::Vehicle(const std::string& vehicleTypeName, const irr::core::vector3df& apos, const irr::core::vector3df& rotation,
    bool manual, bool sequential, float suspensionSpringModifier, float suspensionDamperModifier, float brakeBalance)
    : vehicleType(0),
      collisionListener(0),
      matrix(),
      node(0),
      hkBody(0),
      offsetObject(0),
      tyres(),
      hkVehicle(0),
      engineSound(0),
      linearVelocity(),
      distance(0.0f),
      lastPos(),
      smokes(new Smoke*[MAX_SMOKES]),
      physUpdates(0),
      clutch(0.0f),
      suspensionSpringModifier(suspensionSpringModifier),
      suspensionDamperModifier(suspensionDamperModifier),
      brakeBalance(brakeBalance),
      nameText(0)
{
    dprintf(MY_DEBUG_NOTE, "Vehicle::Vehicle(): %p, [%s]\n", this, vehicleTypeName.c_str());
    vehicleType = VehicleTypeManager::getInstance()->getVehicleType(vehicleTypeName);
    assert(vehicleType);

    offsetObject = ObjectPoolManager::getInstance()->getObject(vehicleType->objectName, apos, irr::core::vector3df(1.f, 1.f, 1.f), rotation);
    offsetObject->setUpdateCB(this);
    node = offsetObject->getNode();
    hkBody = offsetObject->getBody();

    lastPos = node->getPosition();

    node->setMaterialTexture(0, vehicleType->texture);
    //node->setRotation(rotation);

    matrix.setTranslation(node->getPosition());
    matrix.setRotationDegrees(rotation);

    tyres.resize(vehicleType->vehicleTypeTyreMap.size(), 0);

    // ------------------------------------------------
    // Create hkVehicle
    // ------------------------------------------------
    dprintf(MY_DEBUG_NOTE, "Vehicle::Vehicle(): %p, create hkVehicle\n", this);
    hkVehicle = new hkpVehicleInstance(hkBody);

    hkVehicle->m_data = new hkpVehicleData;
    hkVehicle->m_driverInput = new hkpVehicleDefaultAnalogDriverInput;
    hkVehicle->m_steering = new hkpVehicleDefaultSteering;
    hkVehicle->m_engine = new hkpVehicleDefaultEngine;
    hkVehicle->m_transmission = new VehicleTransmission(this, manual, sequential);//new hkpVehicleDefaultTransmission;
    hkVehicle->m_brake = new hkpVehicleDefaultBrake;
    hkVehicle->m_suspension = new hkpVehicleDefaultSuspension;
    hkVehicle->m_aerodynamics = new hkpVehicleDefaultAerodynamics;
    hkVehicle->m_velocityDamper = new hkpVehicleDefaultVelocityDamper;
    hkVehicle->m_wheelCollide = new FrictionMapVehicleRaycastWheelCollide;
    hkVehicle->m_tyreMarks = new hkpTyremarksInfo(*hkVehicle->m_data, 128);
    hkVehicle->m_deviceStatus = new hkpVehicleDriverInputAnalogStatus;

    hkVehicle->setUserData(reinterpret_cast<hkUlong>(this));

    hkVehicle->m_data->m_gravity = hk::hkWorld->getGravity();

    // The coordinates of the chassis system, used for steering the vehicle.
    //                                                      up              forward             right
    hkVehicle->m_data->m_chassisOrientation.setCols(hkVector4(0, 1, 0), hkVector4(1, 0, 0), hkVector4(0, 0, 1));

    hkVehicle->m_data->m_frictionEqualizer = 0.5f;


    // Inertia tensor for each axis is calculated by using :
    // (1 / chassis_mass) * (torque(axis)Factor / chassisUnitInertia)
    // roll - x, yaw - y, pitch - z
//    hkVehicle->m_data->m_torquePitchFactor = 0.5f;
////    hkVehicle->m_data->m_torqueYawFactor = 0.35f;
////    hkVehicle->m_data->m_torqueRollFactor = 0.8f;
////    hkVehicle->m_data->m_torquePitchFactor = 0.6f;
//    hkVehicle->m_data->m_torqueYawFactor = 0.425f;
///    hkVehicle->m_data->m_torqueRollFactor = 0.625f;
///    hkVehicle->m_data->m_torquePitchFactor = 0.6f;
///    hkVehicle->m_data->m_torqueYawFactor = 0.6f;
//    hkVehicle->m_data->m_torqueRollFactor = 0.675f;
//    hkVehicle->m_data->m_torquePitchFactor = 0.675f;
//    hkVehicle->m_data->m_torqueYawFactor = 0.8f;
    hkVehicle->m_data->m_torqueRollFactor = 1.0f;
    hkVehicle->m_data->m_torquePitchFactor = 1.0f;
    hkVehicle->m_data->m_torqueYawFactor = 1.0f;

    hkVehicle->m_data->m_chassisUnitInertiaYaw = 1.0f;
    hkVehicle->m_data->m_chassisUnitInertiaRoll = 1.0f;
    hkVehicle->m_data->m_chassisUnitInertiaPitch = 1.0f;

    // Adds or removes torque around the yaw axis 
    // based on the current steering angle.  This will 
    // affect steering.
    hkVehicle->m_data->m_extraTorqueFactor = -2.0f;// -0.5f;
    hkVehicle->m_data->m_maxVelocityForPositionalFriction = 0.0f;

    // ------------------------------------------------
    // Wheel specifications
    // ------------------------------------------------
    hkVehicle->m_data->m_numWheels = tyres.size();
    hkVehicle->m_data->m_wheelParams.setSize(hkVehicle->m_data->m_numWheels);

    // ------------------------------------------------
    // create wheels
    // ------------------------------------------------
    for (unsigned int i = 0; i < tyres.size(); i++)
    {
        tyres[i] = new VehicleTyre(vehicleType->vehicleTypeTyreMap[i], apos, this, i);
        irr::core::matrix4 tyreMatGlob = matrix * tyres[i]->localMatrix;
        tyres[i]->node->setPosition(tyreMatGlob.getTranslation());
        tyres[i]->node->setRotation(tyreMatGlob.getRotationDegrees());
    }

    // ------------------------------------------------
    // setupComponent( *hkVehicle->m_data, *static_cast< hkpVehicleDefaultAnalogDriverInput* >(hkVehicle->m_driverInput) );
    // ------------------------------------------------
    hkpVehicleDefaultAnalogDriverInput* driverInput = static_cast< hkpVehicleDefaultAnalogDriverInput* >(hkVehicle->m_driverInput);
    driverInput->m_slopeChangePointX = 0.99f;
    driverInput->m_initialSlope = 1.0f;
    driverInput->m_deadZone = Settings::getInstance()->joystickDeadZone*Settings::getInstance()->joystickDeadZone;//0.0f;
    driverInput->m_autoReverse = true;

    // ------------------------------------------------
    // setupComponent( *hkVehicle->m_data, *static_cast< hkpVehicleDefaultSteering*>(hkVehicle->m_steering));
    // ------------------------------------------------
    hkpVehicleDefaultSteering* steering = static_cast< hkpVehicleDefaultSteering*>(hkVehicle->m_steering);
    steering->m_doesWheelSteer.setSize(hkVehicle->m_data->m_numWheels);
    steering->m_maxSteeringAngle = vehicleType->maxSteerAngle * ( HK_REAL_PI / 180 );
    // [mph/h] The steering angle decreases linearly 
    // based on your overall max speed of the vehicle. 
    steering->m_maxSpeedFullSteeringAngle = /*70.0f*/(vehicleType->maxSpeed * 0.5f) * (1.605f / 3.6f);
    for (unsigned int i = 0; i < tyres.size(); i++)
    {
        steering->m_doesWheelSteer[i] = tyres[i]->tyreType->steerable;
    }

    // ------------------------------------------------
    // setupComponent( *hkVehicle->m_data, *static_cast< hkpVehicleDefaultEngine*>(hkVehicle->m_engine) );
    // ------------------------------------------------
    hkpVehicleDefaultEngine* engine = static_cast< hkpVehicleDefaultEngine*>(hkVehicle->m_engine);
    engine->m_maxTorque = vehicleType->maxTorqueRate /*500.0f*/;
    engine->m_minRPM = vehicleType->maxTorque / 8.0f /*1000.0f*/;
    engine->m_optRPM = (vehicleType->maxTorque * 3.0f) / 4.0f /*5500.0f*/;
    // This value is also used to calculate the m_primaryTransmissionRatio.
    engine->m_maxRPM = vehicleType->maxTorque /*7500.0f*/;
    engine->m_torqueFactorAtMinRPM = 0.8f;
    engine->m_torqueFactorAtMaxRPM = 0.8f;
    engine->m_resistanceFactorAtMinRPM = 0.1f;
    engine->m_resistanceFactorAtOptRPM = 0.3f;
    engine->m_resistanceFactorAtMaxRPM = 0.8f;


    // ------------------------------------------------
    // setupComponent( *hkVehicle->m_data, *static_cast< hkpVehicleDefaultTransmission*>(hkVehicle->m_transmission) );
    // ------------------------------------------------
    VehicleTransmission* transmission = static_cast<VehicleTransmission*>(hkVehicle->m_transmission);
    transmission->m_gearsRatio.setSize(vehicleType->gearMap.size());
    transmission->m_wheelsTorqueRatio.setSize(hkVehicle->m_data->m_numWheels);

    transmission->m_downshiftRPM = vehicleType->maxTorque / 2.5f /*3500.0f*/;
    transmission->m_upshiftRPM = (vehicleType->maxTorque * 4.0f) / 5.0f /*6500.0f*/;

    for (unsigned int i = 0; i < vehicleType->gearMap.size(); i++)
    {
        transmission->m_gearsRatio[i] = vehicleType->gearMap[i+1];
        dprintf(MY_DEBUG_NOTE, "\t%d. gear ration: %f\n", i, transmission->m_gearsRatio[i]);
    }

    transmission->m_clutchDelayTime = vehicleType->changeGearTime / 50.0f;
    transmission->m_reverseGearRatio = 1.5f;
    /*
    transmission.m_gearsRatio[0] = 2.0f;
    transmission.m_gearsRatio[1] = 1.5f;
    transmission.m_gearsRatio[2] = 1.0f;
    transmission.m_gearsRatio[3] = 0.75f;
    */
    float torqueableTyreNum = 0.0f;
    for (unsigned int i = 0; i < tyres.size(); i++)
    {
        if (tyres[i]->tyreType->torqueable)
        {
            if (tyres[i]->tyreType->steerable)
            {
                torqueableTyreNum += 2.0f;
            }
            else
            {
                torqueableTyreNum += 3.0f;
            }
        }
        /*
        if (tyres[i]->tyreType->steerable)
        {
            transmission->m_wheelsTorqueRatio[i] = 0.2f;
        }
        else
        {
            transmission->m_wheelsTorqueRatio[i] = 0.3f;
        }
        */
    }
    
    float torqueableTyreRate = torqueableTyreNum > 0.0f ? 1.0f / torqueableTyreNum : 0.0f;
    for (unsigned int i = 0; i < tyres.size(); i++)
    {
        if (tyres[i]->tyreType->torqueable)
        {
            if (tyres[i]->tyreType->steerable)
            {
                transmission->m_wheelsTorqueRatio[i] = torqueableTyreRate * 2.0f;
            }
            else
            {
                transmission->m_wheelsTorqueRatio[i] = torqueableTyreRate * 3.0f;
            }
        }
        else
        {
            transmission->m_wheelsTorqueRatio[i] = 0.0f;
        }
    }
    /*
    transmission.m_wheelsTorqueRatio[0] = 0.2f;
    transmission.m_wheelsTorqueRatio[1] = 0.2f;
    transmission.m_wheelsTorqueRatio[2] = 0.3f;
    transmission.m_wheelsTorqueRatio[3] = 0.3f;
    */
    transmission->m_primaryTransmissionRatio = hkpVehicleDefaultTransmission::calculatePrimaryTransmissionRatio(
        vehicleType->maxSpeed / 1.605f,
        tyres[0]->radius,
        vehicleType->maxTorque,
        transmission->m_gearsRatio[vehicleType->gearMap.size() - 1]);

    // ------------------------------------------------
    // setupComponent( *hkVehicle->m_data, *static_cast< hkpVehicleDefaultBrake*>(hkVehicle->m_brake) );
    // ------------------------------------------------
    hkpVehicleDefaultBrake* brake = static_cast< hkpVehicleDefaultBrake*>(hkVehicle->m_brake);
    brake->m_wheelBrakingProperties.setSize(hkVehicle->m_data->m_numWheels);

    modifyBrakeBalance(brakeBalance);

    for (unsigned int i = 0; i < tyres.size(); i++)
    {
        brake->m_wheelBrakingProperties[i].m_isConnectedToHandbrake = tyres[i]->tyreType->handbrakeable;
        brake->m_wheelBrakingProperties[i].m_minPedalInputToBlock = 0.1f;
    }
    brake->m_wheelsMinTimeToBlock = 1000.0f;
    
    // ------------------------------------------------
    // setupComponent( *hkVehicle->m_data, *static_cast< hkpVehicleDefaultSuspension*>(hkVehicle->m_suspension) );
    // ------------------------------------------------
    hkpVehicleDefaultSuspension* suspension = static_cast< hkpVehicleDefaultSuspension*>(hkVehicle->m_suspension);
    suspension->m_wheelParams.setSize(hkVehicle->m_data->m_numWheels);
    suspension->m_wheelSpringParams.setSize(hkVehicle->m_data->m_numWheels);

    modifySuspensionSpring(suspensionSpringModifier);
    modifySuspensionDamper(suspensionDamperModifier);

    const hkVector4 downDirection(0.0f, -1.0f, 0.0f);
    const hkVector4 downDirectionp(0.0f, -1.0f, 0.5f);
    const hkVector4 downDirectionm(0.0f, -1.0f, -0.5f);
    for (unsigned int i = 0; i < tyres.size(); i++)
    {
        //suspension->m_wheelParams[i].m_length = tyres[i]->tyreType->suspensionLength;
        //suspension->m_wheelSpringParams[i].m_strength = tyres[i]->tyreType->suspensionSpring;
        //suspension->m_wheelSpringParams[i].m_dampingCompression = tyres[i]->tyreType->suspensionDamper;
        //suspension->m_wheelSpringParams[i].m_dampingRelaxation = tyres[i]->tyreType->suspensionDamper;
        suspension->m_wheelParams[i].m_hardpointChassisSpace.set(tyres[i]->tyreType->localPos.X, -0.05f, tyres[i]->tyreType->localPos.Z); 
        /*
        if (tyres[i]->tyreType->localPos.Z > 0.0f)
        {
            suspension->m_wheelParams[i].m_directionChassisSpace = downDirectionp;
        }
        else
        {
            suspension->m_wheelParams[i].m_directionChassisSpace = downDirectionm;
        }
        */
        suspension->m_wheelParams[i].m_directionChassisSpace = downDirection;
    }
    
    // ------------------------------------------------
    // setupComponent( *hkVehicle->m_data, *static_cast< hkpVehicleDefaultAerodynamics*>(hkVehicle->m_aerodynamics) );
    // ------------------------------------------------
    hkpVehicleDefaultAerodynamics* aerodynamics = static_cast< hkpVehicleDefaultAerodynamics*>(hkVehicle->m_aerodynamics);
    aerodynamics->m_airDensity = 1.3f;
    // In m^2.
    aerodynamics->m_frontalArea = 1.0f;
    aerodynamics->m_dragCoefficient = 0.7f;
    aerodynamics->m_liftCoefficient = -0.3f;
    // Extra gavity applies in world space (independent of m_chassisCoordinateSystem).
    aerodynamics->m_extraGravityws.set(0.0f, -5.0f, 0.0f);
    
    // ------------------------------------------------
    // setupComponent( *hkVehicle->m_data, *static_cast< hkpVehicleDefaultVelocityDamper*>(hkVehicle->m_velocityDamper) );
    // ------------------------------------------------
    hkpVehicleDefaultVelocityDamper* velocityDamper = static_cast< hkpVehicleDefaultVelocityDamper*>(hkVehicle->m_velocityDamper);
    // Caution: setting negative damping values will add energy to system. 
    // Setting the value to 0 will not affect the angular velocity. 

    // Damping the change of the chassis angular velocity when below m_collisionThreshold. 
    // This will affect turning radius and steering.
    velocityDamper->m_normalSpinDamping = 0.0f; 
    // Positive numbers dampen the rotation of the chassis and 
    // reduce the reaction of the chassis in a collision. 
    velocityDamper->m_collisionSpinDamping = 4.0f; 
    // The threshold in m/s at which the algorithm switches from 
    // using the normalSpinDamping to the collisionSpinDamping. 
    velocityDamper->m_collisionThreshold   = 1.0f; 
    
    // ------------------------------------------------
    // setupWheelCollide(hkWorld, *hkVehicle, *static_cast< hkpVehicleRayCastWheelCollide*>(hkVehicle->m_wheelCollide) );
    // ------------------------------------------------
    hkpVehicleRayCastWheelCollide* wheelCollide = static_cast< hkpVehicleRayCastWheelCollide*>(hkVehicle->m_wheelCollide);
    // Set the wheels to have the same collision filter info as the chassis.
    //wheelCollide->m_wheelCollisionFilterInfo = /*vehicle.getChassis()*/hkBody->getCollisionFilterInfo();
    wheelCollide->m_wheelCollisionFilterInfo = hkpGroupFilter::calcFilterInfo(hk::materialType::wheelId);

    // ------------------------------------------------
    // setupTyremarks( *hkVehicle->m_data, *static_cast< hkpTyremarksInfo*>(hkVehicle->m_tyreMarks) );
    // ------------------------------------------------
    hkpTyremarksInfo* tyreMarks = static_cast< hkpTyremarksInfo*>(hkVehicle->m_tyreMarks);
    tyreMarks->m_minTyremarkEnergy = 100.0f;
    tyreMarks->m_maxTyremarkEnergy  = 1000.0f;

    assert(hkBody == hkVehicle->getChassis());

    // ------------------------------------------------
    // finalize
    // ------------------------------------------------
    hkVehicle->init();
    //printf("2\n");
    hk::lock();
    //printf("2a\n");
    hkVehicle->addToWorld(hk::hkWorld);
    //printf("3\n");
    hk::hkWorld->addAction(hkVehicle);
    hk::unlock();
    
    //hkBody->setLinearVelocity(hkVector4());
    //hkBody->setAngularVelocity(hkVector4());
    setSteer(0.0f);
    setTorque(0.0f);
    setHandbrake(0.0f);
    
    if (!vehicleType->engineSoundFilename.empty())
    {
        engineSound = MySoundEngine::getInstance()->play3D(vehicleType->engineSoundFilename, irr::core::vector3df(), true, false, true);
    }
    
    if (engineSound)
    {
        engineSound->setMinDistance(4.0f);
        // engineSound->setIsPaused(true);
    }
    VehicleManager::getInstance()->addVehicle(this);
    memset(smokes, 0, MAX_SMOKES*sizeof(Smoke*));
    
    collisionListener = new VehicleCollisionListener(this);
    hkBody->addContactListener(collisionListener);
    //assert(0);
}

Vehicle::~Vehicle()
{
    dprintf(MY_DEBUG_NOTE, "Vehicle::~Vehicle(): %p\n", this);
    
    if (collisionListener)
    {
        hkBody->removeContactListener(collisionListener);
        collisionListener->removeReference();
        collisionListener = 0;
    }
    
    vehicleType = 0;
    node = 0;
    hkBody = 0;
    
    pause();
    if (engineSound)
    {
        delete engineSound;
        engineSound = 0;
    }

    VehicleManager::getInstance()->removeVehicle(this);

    ObjectPoolManager::getInstance()->putObject(offsetObject);
    offsetObject = 0;

    for (unsigned int i = 0; i < tyres.size(); i++)
    {
        delete tyres[i];
    }
    tyres.clear();

    //printf("r1\n");
    hk::lock();
    hk::hkWorld->removeAction(hkVehicle);
    //printf("r2\n");
    hkVehicle->removeFromWorld();
    //printf("r3\n");
    hkVehicle->removeReference();
    //printf("r4\n");
    hkVehicle = 0;
    hk::unlock();

    if (smokes)
    {
        for(int ind = 0;ind<MAX_SMOKES;ind++)
        {
            if (smokes[ind])
            {
                //printf("delete smoke\n");
                smokes[ind]->node->remove();
                delete smokes[ind];
                smokes[ind] = 0;
            }
        }
        delete [] smokes;
        smokes = 0;
    }
}

void Vehicle::handleUpdatePos(bool phys)
{
    if (phys)
    {
        physUpdates++;
        const bool actual = ((physUpdates % 3) == 0);

        hkVector4 hkLV = hkBody->getLinearVelocity();
        hkVector4 hkPos = /*hkVehicle->getChassis()*/hkBody->getPosition();
        hkQuaternion hkQuat = /*hkVehicle->getChassis()*/hkBody->getRotation();
        irr::core::quaternion(hkQuat.m_vec(0), hkQuat.m_vec(1), hkQuat.m_vec(2), hkQuat.m_vec(3)).getMatrix(matrix,
            irr::core::vector3df(hkPos(0), hkPos(1), hkPos(2)));
        const irr::core::vector3df newPos = matrix.getTranslation();
        distance += newPos.getDistanceFrom(lastPos);
        lastPos = newPos;
        node->setPosition(newPos);
        node->setRotation(matrix.getRotationDegrees());
        
        linearVelocity.X = hkLV(0);
        linearVelocity.Y = hkLV(1);
        linearVelocity.Z = hkLV(2);

        const float speed = getSpeed();
        const float angularVelocity = fabsf(speed);
        const float ssSpeed = angularVelocity/80.f+1.0f;
        const float rpm = hkVehicle->calcRPM() / vehicleType->maxTorque;
        float ssGear = rpm * 1.5f + 1.0f;
        const float soundVolume = rpm*0.5f + 0.5f;

        if (ssGear > 2.5f) ssGear = 2.5f;

        const irr::core::vector3df soundPos = node->getPosition(); //(matrix*soundMatrix).getTranslation();
        if (engineSound)
        {
            engineSound->setPosition(soundPos);
            engineSound->setPlaybackSpeed(ssGear);
            engineSound->setVelocity(linearVelocity);
            engineSound->setVolume(soundVolume);
        }
        updateSmoke();
        
        // tyre update
        irr::core::matrix4 tyreMatrix;
        bool onGround = false;
        for (unsigned int i = 0; i < tyres.size(); i++)
        {
            hkVector4    hkPos;
            hkQuaternion hkQuat;
            //
            // XXX Check if this is the same value as the m_hardPointWS in wheelsInfo
            //
            //
            hkVehicle->calcCurrentPositionAndRotation(
                hkVehicle->getChassis(),
                hkVehicle->m_suspension,
                i,
                hkPos, hkQuat);
            irr::core::quaternion(hkQuat.m_vec(0), hkQuat.m_vec(1), hkQuat.m_vec(2), hkQuat.m_vec(3)).getMatrix(
                tyreMatrix,
                irr::core::vector3df(hkPos(0), hkPos(1), hkPos(2)));
            tyres[i]->node->setPosition(tyreMatrix.getTranslation());
            tyres[i]->node->setRotation(tyreMatrix.getRotationDegrees());

            if (tyres[i]->hitBody)
            {
                onGround = true;
                if (tyres[i]->hitBody->hasProperty(hk::materialType::terrainId) && actual && speed > 5.f)
                {
                    addSmoke(speed*0.4f, tyres[i]->node->getPosition(), tyres[i]->radius);
                }
            }
        }

        /*
            Debug
        */
        /*
        hkpVehicleTransmission::TransmissionOutput to;
        memset(&to, 0, sizeof(to));
        to.m_numWheelsTramsmittedTorque = 4;
        to.m_wheelsTransmittedTorque = new hkReal[to.m_numWheelsTramsmittedTorque];
        memset(to.m_wheelsTransmittedTorque, 0, sizeof(hkReal)*to.m_numWheelsTramsmittedTorque);
        hkVehicle->m_transmission->calcTransmission(0.1f, hkVehicle, to);
        printf("TransmissionOutput:\n");
        printf("\tm_transmissionRPM:            %f\n", to.m_transmissionRPM);
        printf("\tm_mainTransmittedTorque:      %f\n", to.m_mainTransmittedTorque);
        printf("\tm_numWheelsTramsmittedTorque: %hhu\n", to.m_numWheelsTramsmittedTorque);
        for (char i = 0; i < to.m_numWheelsTramsmittedTorque; i++)
        {
            printf("\t\t%hhu: %f\n", i, to.m_wheelsTransmittedTorque[i]);
        }
        printf("\tm_isReversing (bool):         %u\n", to.m_isReversing);
        printf("\tm_currentGear:                %hhu\n", to.m_currentGear);
        printf("\tm_delayed (bool):             %u\n", to.m_delayed);
        printf("\tm_clutchDelayCountdown:       %f\n", to.m_clutchDelayCountdown);
        delete [] to.m_wheelsTransmittedTorque;
        */
    }
    else
    {
        const irr::core::vector3df soundPos = node->getPosition(); //(matrix*soundMatrix).getTranslation();
        lastPos = soundPos;
        if (engineSound)
        {
            engineSound->setPosition(soundPos);
        }
    }
    updateNameTextPos();
}

void Vehicle::updateNameTextPos()
{
    if (nameText)
    {
        irr::core::vector3df pos = node->getPosition();
        float dist = TheGame::getInstance()->getCamera()->getPosition().getDistanceFrom(pos) / 1000.f;
        pos.Y += (4.0f + (140.f * dist * dist));
        nameText->setPosition(pos);
    }
}

void Vehicle::reset(const irr::core::vector3df& pos)
{
    irr::core::vector3df rot = matrix.getRotationDegrees();// - m_trafo.getRotationDegrees();
    dprintf(MY_DEBUG_NOTE, "reset car: orig rot: %f %f %f\n", rot.X, rot.Y, rot.Z);
    /*
    if (fabsf(rot.Z-180.f) < 90.f)
    {
        if (rot.Y < 90.f)
            rot.Y = 180.f - rot.Y;
        if (rot.Y > 270.f)
            rot.Y = 540.f - rot.Y;
    }
    */
    if (fabsf(rot.Z-180.f) < 90.f)
    //if (rot.Z > 180.f)
    {
        rot.Y = rot.Y - 180.f;
    }
    else
    {
        rot.Y = -rot.Y;
        if (rot.Y < 0) rot.Y+=360.f;
    }
    rot.Z = rot.X = 0.f;
    dprintf(MY_DEBUG_NOTE, "reset car:  mod rot: %f %f %f, action-world: %p\n", rot.X, rot.Y, rot.Z, hkVehicle->getWorld());
    matrix.setTranslation(pos);
    // vector3df(0.f, rot.Y, 0.f)
    matrix.setRotationDegrees(rot);
    //hkBody->setLinearVelocity(hkVector4());
    hkBody->activate();
    
    //hkVehicle->
    //hkBody->applyLinearImpulse(/*hkVector4(-hkBody->getLinearVelocity()(0), -hkBody->getLinearVelocity()(1), -hkBody->getLinearVelocity()(2))*/hkVector4(0.0f, 1.0f, 0.0f));
    //hkBody->applyLinearImpulse(hkVector4(10000.0f, 0.0f, 0.0f));
    updateToMatrix();
}

void Vehicle::addStartImpulse(float initialSpeed, const irr::core::vector3df& dir)
{
    float v = initialSpeed / 3.6f; // km/h -> m/s
    float m = hkBody->getMass();
    hkBody->applyLinearImpulse(hkVector4(dir.X*m*v, dir.Y*m*v, dir.Z*m*v));
}

float Vehicle::getAngle() const
{
    irr::core::vector3df rot = matrix.getRotationDegrees();
    if (fabsf(rot.Z-180.f) < 90.f)
    {
        if (rot.Y < 90.f)
            rot.Y = 180.f - rot.Y;
        if (rot.Y > 270.f)
            rot.Y = 540.f - rot.Y;
    }
    //rot.Z = rot.X = 0.f;
    return rot.Y;
}

int Vehicle::getGear() const
{
    if (((VehicleTransmission*)hkVehicle->m_transmission)->manual)
    {
        return ((VehicleTransmission*)hkVehicle->m_transmission)->gear;
    }
    else
    {
        return hkVehicle->m_currentGear+1;
    }
}

const irr::core::matrix4& Vehicle::getViewPos(unsigned int num) const
{
    assert(vehicleType && num < VIEW_SIZE);
    return vehicleType->viewPos[num];
}

const irr::core::matrix4& Vehicle::getViewDest(unsigned int num) const
{
    assert(vehicleType && num < VIEW_SIZE);
    return vehicleType->viewDest[num];
}

bool Vehicle::getGearShifting() const
{
    return ((VehicleTransmission*)hkVehicle->m_transmission)->manual;
}

bool Vehicle::getGearShiftingSequential() const
{
    return ((VehicleTransmission*)hkVehicle->m_transmission)->sequential;
}

void Vehicle::updateToMatrix()
{
    const irr::core::vector3df pos = matrix.getTranslation();
    irr::core::quaternion quat(matrix);
    hkBody->setPositionAndRotation(hkVector4(pos.X, pos.Y, pos.Z), hkQuaternion(quat.X, quat.Y, quat.Z, quat.W));

    node->setPosition(pos);
    node->setRotation((matrix).getRotationDegrees());
    lastPos = pos;
}

void Vehicle::setSteer(float value)
{
    //hkVehicle->getChassis()->activate();
    hkpVehicleDriverInputAnalogStatus* deviceStatus = (hkpVehicleDriverInputAnalogStatus*)hkVehicle->m_deviceStatus;
    deviceStatus->m_positionX = hkMath::clamp(-value, -1.0f, 1.0f);
}

void Vehicle::setTorque(float value)
{
    hkpVehicleDriverInputAnalogStatus* deviceStatus = (hkpVehicleDriverInputAnalogStatus*)hkVehicle->m_deviceStatus;
    if (((VehicleTransmission*)hkVehicle->m_transmission)->manual && ((VehicleTransmission*)hkVehicle->m_transmission)->gear < 0) value *= -1.0f;
    deviceStatus->m_positionY = hkMath::clamp(value, -1.0f, 1.0f);
}

void Vehicle::setHandbrake(float value)
{
    hkpVehicleDriverInputAnalogStatus* deviceStatus = (hkpVehicleDriverInputAnalogStatus*)hkVehicle->m_deviceStatus;
    deviceStatus->m_handbrakeButtonPressed = value > 0.1f;
}

void Vehicle::setGearShifting(bool manual, bool sequential)
{
    ((VehicleTransmission*)hkVehicle->m_transmission)->manual = manual;
    ((VehicleTransmission*)hkVehicle->m_transmission)->gear = 0;
}

void Vehicle::setGear(char gear)
{
    if (gear >= -1 && gear <= ((VehicleTransmission*)hkVehicle->m_transmission)->m_gearsRatio.getSize())
    {
        ((VehicleTransmission*)hkVehicle->m_transmission)->gear = gear;
    }
    else
    {
        ((VehicleTransmission*)hkVehicle->m_transmission)->gear = 0;
    }
}

void Vehicle::incGear()
{
    if (((VehicleTransmission*)hkVehicle->m_transmission)->gear+1 <= ((VehicleTransmission*)hkVehicle->m_transmission)->m_gearsRatio.getSize())
    {
        ((VehicleTransmission*)hkVehicle->m_transmission)->gear++;
    }
}

void Vehicle::decGear()
{
    if (((VehicleTransmission*)hkVehicle->m_transmission)->gear-1 >= -1)
    {
        ((VehicleTransmission*)hkVehicle->m_transmission)->gear--;
    }
}

void Vehicle::modifySuspensionSpring(float suspensionSpringModifier)
{
    if (suspensionSpringModifier < -20.f) suspensionSpringModifier = -20.f;
    if (suspensionSpringModifier > 20.f) suspensionSpringModifier = 20.f;
    this->suspensionSpringModifier = suspensionSpringModifier;
    for (unsigned int i = 0; i < tyres.size(); i++)
    {
        ((hkpVehicleDefaultSuspension*)hkVehicle->m_suspension)->m_wheelSpringParams[i].m_strength = 
            tyres[i]->tyreType->suspensionSpring = 30.f + suspensionSpringModifier;

        float suspensionLengthModifier = ((suspensionSpringModifier) * 0.015f);
        if (suspensionLengthModifier > (tyres[i]->tyreType->suspensionLength*0.25f)) suspensionLengthModifier = (tyres[i]->tyreType->suspensionLength*0.25f);
        ((hkpVehicleDefaultSuspension*)hkVehicle->m_suspension)->m_wheelParams[i].m_length =
            tyres[i]->tyreType->suspensionLength - suspensionLengthModifier;

        tyres[i]->tyreType->suspensionDamper = (tyres[i]->tyreType->suspensionSpring * (30.f - suspensionDamperModifier)) / 100.f;
        ((hkpVehicleDefaultSuspension*)hkVehicle->m_suspension)->m_wheelSpringParams[i].m_dampingCompression = 
            ((hkpVehicleDefaultSuspension*)hkVehicle->m_suspension)->m_wheelSpringParams[i].m_dampingRelaxation = 
            tyres[i]->tyreType->suspensionDamper;
    }
}

void Vehicle::modifySuspensionDamper(float suspensionDamperModifier)
{
    if (suspensionDamperModifier < -20.f) suspensionDamperModifier = -20.f;
    if (suspensionDamperModifier > 20.f) suspensionDamperModifier = 20.f;
    this->suspensionDamperModifier = suspensionDamperModifier;
    for (unsigned int i = 0; i < tyres.size(); i++)
    {
        tyres[i]->tyreType->suspensionDamper = (tyres[i]->tyreType->suspensionSpring * (30.f - suspensionDamperModifier)) / 100.f;
        ((hkpVehicleDefaultSuspension*)hkVehicle->m_suspension)->m_wheelSpringParams[i].m_dampingCompression = 
            ((hkpVehicleDefaultSuspension*)hkVehicle->m_suspension)->m_wheelSpringParams[i].m_dampingRelaxation = 
            tyres[i]->tyreType->suspensionDamper;
    }
}

void Vehicle::modifyBrakeBalance(float brakeBalance)
{
    if (brakeBalance < 0.0f) brakeBalance = 0.0f;
    if (brakeBalance > 1.0f) brakeBalance = 1.0f;
    this->brakeBalance = brakeBalance;
    for (unsigned int i = 0; i < tyres.size(); i++)
    {
        if (tyres[i]->tyreType->steerable)
        {
            static_cast<hkpVehicleDefaultBrake*>(hkVehicle->m_brake)->m_wheelBrakingProperties[i].m_maxBreakingTorque = vehicleType->maxBrakeForce*(2.0f*(1.0f - brakeBalance));
        }
        else
        {
            static_cast<hkpVehicleDefaultBrake*>(hkVehicle->m_brake)->m_wheelBrakingProperties[i].m_maxBreakingTorque = vehicleType->maxBrakeForce*(2.0f*brakeBalance);
        }
    }
}

void Vehicle::pause()
{
    if (engineSound)
    {
        engineSound->setPlaybackSpeed(1.f);
        engineSound->setVelocity(irr::core::vector3df());
        engineSound->setIsPaused(true);
    }
}

void Vehicle::resume()
{
    if (engineSound)
    {
        engineSound->setIsPaused(false);
    }
}

void Vehicle::addSmoke(const float speed, const irr::core::vector3df& pos, float offset)
{
    int ind = 0;
    
    if (!smokes) return;
    //printf("add smoke\n");
    for(;ind<MAX_SMOKES;ind++)
    {
        if (smokes[ind]==0 || smokes[ind]->animePhase==-1) break;
    }
    
    if (ind==MAX_SMOKES)
    {
        //printf("add smoke end 1\n");
        return;
    }
    
    if (smokes[ind]==0)
    {
        smokes[ind] = new Smoke(speed, pos, offset/*, waterHeight*/);
    }
    else
    {
        smokes[ind]->renew(speed, pos, offset/*, waterHeight*/);
    }
    //printf("add smoke end 2\n");
}

void Vehicle::updateSmoke()
{
    int ind = 0;

    if (!smokes) return;
    //printf("update smokes\n");
    
    for(;ind<MAX_SMOKES;ind++)
    {
        if (smokes[ind] && smokes[ind]->animePhase >= 0)
        {
            if (smokes[ind]->animePhase < MAX_SMOKES/3)
            {
                //printf("update smoke ind: %d, ap: %d\n", ind, smokes[ind]->animePhase);
                irr::core::dimension2df size(1.f,1.f);
                float scale = (smokes[ind]->speed/90.f);

                //size = smokes[ind]->node->getSize();
                size *= smokes[ind]->animePhase * scale;
                smokes[ind]->animePhase++;
                smokes[ind]->node->setSize(size);
                smokes[ind]->node->getMaterial(0).MaterialTypeParam2 = 1.0f - ((float)smokes[ind]->animePhase/(float)(MAX_SMOKES/3));
            }
            else
            {
                //printf("delete smoke\n");
                //smokes[ind]->node->remove();
                //delete smokes[ind];
                //smokes[ind] = 0;
                smokes[ind]->animePhase = -1;
                smokes[ind]->node->setVisible(false);
            }
        }
    }
    //printf("update smokes end\n");
}

Smoke::Smoke(const float speed, const irr::core::vector3df& pos, float offset/*, float p_waterHeight*/)
    : speed(speed),
      animePhase(0),
      offsetObject(0)
{
    //printf("new smoke\n");
    float addrX = (float)((rand()%20) - 10) / 20.0f;
    float addrZ = (float)((rand()%20) - 10) / 20.0f;

    node = TheGame::getInstance()->getSmgr()->addBillboardSceneNode(0, irr::core::dimension2df(0.2f, 0.2f), irr::core::vector3df(pos.X+addrX, pos.Y, pos.Z+addrZ));
    //node->setMaterialFlag(video::EMF_LIGHTING, false);
    node->setMaterialFlag(irr::video::EMF_LIGHTING, false);
    node->setMaterialFlag(irr::video::EMF_TEXTURE_WRAP, true);
    node->setMaterialFlag(irr::video::EMF_BLEND_OPERATION, true);
    node->setMaterialType(TheEarth::getInstance()->getSmokeMaterial()/*Shaders::getInstance()->materialMap["normal_no_light_t"]*/);
    node->getMaterial(0).MaterialTypeParam2 = 1.0f;
    //if (pos.Y - offset > p_waterHeight)
    //{
       node->setMaterialTexture(0, TheEarth::getInstance()->getSmokeTexture());
    //}
    //else
    //{
    //   node->setMaterialTexture(0, smokeWaterTexture);
    //}
}

void Smoke::renew(const float speed, const irr::core::vector3df& pos, float offset/*, float p_waterHeight*/)
{
    //printf("renew smoke\n");
    float addrX = (float)((rand()%20) - 10) / 20.0f;
    float addrZ = (float)((rand()%20) - 10) / 20.0f;
    
    this->speed = speed;
    animePhase = 0;

    node->getMaterial(0).MaterialTypeParam2 = 1.0f;
    node->setSize(irr::core::dimension2df(0.2f, 0.2f));
    node->setPosition(irr::core::vector3df(pos.X+addrX, pos.Y, pos.Z+addrZ));
    node->setVisible(true);
    //if (pos.Y - offset > p_waterHeight)
    //{
       node->setMaterialTexture(0, TheEarth::getInstance()->getSmokeTexture());
    //}
    //else
    //{
    //   node->setMaterialTexture(0, smokeWaterTexture);
    //}
}
