
#include "VehicleType.h"
#include "Vehicle.h"
#include "VehicleManager.h"
#include "VehicleTypeManager.h"
#include "ObjectPoolManager.h"
#include "MySound.h"
#include "stdafx.h"

#include <Physics/Vehicle/WheelCollide/RayCast/hkpVehicleRayCastWheelCollide.h>
#include <Physics/Vehicle/hkpVehicleInstance.h>
#include <Physics/Collide/Query/CastUtil/hkpWorldRayCastOutput.h>


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

Vehicle::Vehicle(const std::string& vehicleTypeName, const irr::core::vector3df& apos, const irr::core::vector3df& rotation)
    : vehicleType(0),
      matrix(),
      node(0),
      hkBody(0),
      offsetObject(0),
      tyres(),
      hkVehicle(0),
      engineSound(0),
      linearVelocity(),
      distance(0.0f),
      lastPos()
{
    dprintf(MY_DEBUG_NOTE, "Vehicle::Vehicle(): %p, [%s]\n", this, vehicleTypeName.c_str());
    vehicleType = VehicleTypeManager::getInstance()->getVehicleType(vehicleTypeName);
    assert(vehicleType);

    offsetObject = ObjectPoolManager::getInstance()->getObject(vehicleType->objectName, apos);
    offsetObject->setUpdateCB(this);
    node = offsetObject->getNode();
    hkBody = offsetObject->getBody();

    lastPos = node->getPosition();

    node->setMaterialTexture(0, vehicleType->texture);
    node->setRotation(rotation);

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
    hkVehicle->m_transmission = new hkpVehicleDefaultTransmission;
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
    hkVehicle->m_data->m_torqueRollFactor = 0.625f;
    hkVehicle->m_data->m_torquePitchFactor = 0.5f;
    hkVehicle->m_data->m_torqueYawFactor = 0.35f;

    hkVehicle->m_data->m_chassisUnitInertiaYaw = 1.0f;
    hkVehicle->m_data->m_chassisUnitInertiaRoll = 1.0f;
    hkVehicle->m_data->m_chassisUnitInertiaPitch = 1.0f;

    // Adds or removes torque around the yaw axis 
    // based on the current steering angle.  This will 
    // affect steering.
    hkVehicle->m_data->m_extraTorqueFactor = -0.5f;
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
    driverInput->m_slopeChangePointX = 0.8f;
    driverInput->m_initialSlope = 0.7f;
    driverInput->m_deadZone = 0.0f;
    driverInput->m_autoReverse = true;

    // ------------------------------------------------
    // setupComponent( *hkVehicle->m_data, *static_cast< hkpVehicleDefaultSteering*>(hkVehicle->m_steering));
    // ------------------------------------------------
    hkpVehicleDefaultSteering* steering = static_cast< hkpVehicleDefaultSteering*>(hkVehicle->m_steering);
    steering->m_doesWheelSteer.setSize(hkVehicle->m_data->m_numWheels);
    steering->m_maxSteeringAngle = vehicleType->maxSteerAngle * ( HK_REAL_PI / 180 );
    // [mph/h] The steering angle decreases linearly 
    // based on your overall max speed of the vehicle. 
    steering->m_maxSpeedFullSteeringAngle = 70.0f * (1.605f / 3.6f);
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
    hkpVehicleDefaultTransmission* transmission = static_cast< hkpVehicleDefaultTransmission*>(hkVehicle->m_transmission);
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
    for (unsigned int i = 0; i < tyres.size(); i++)
    {
        if (tyres[i]->tyreType->steerable)
        {
            transmission->m_wheelsTorqueRatio[i] = 0.2f;
        }
        else
        {
            transmission->m_wheelsTorqueRatio[i] = 0.3f;
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
    for (unsigned int i = 0; i < tyres.size(); i++)
    {
        brake->m_wheelBrakingProperties[i].m_maxBreakingTorque = vehicleType->maxBrakeForce;
        brake->m_wheelBrakingProperties[i].m_isConnectedToHandbrake = tyres[i]->tyreType->handbrakeable;
        brake->m_wheelBrakingProperties[i].m_minPedalInputToBlock = 0.9f;
    }
    brake->m_wheelsMinTimeToBlock = 1000.0f;
    
    // ------------------------------------------------
    // setupComponent( *hkVehicle->m_data, *static_cast< hkpVehicleDefaultSuspension*>(hkVehicle->m_suspension) );
    // ------------------------------------------------
    hkpVehicleDefaultSuspension* suspension = static_cast< hkpVehicleDefaultSuspension*>(hkVehicle->m_suspension);
    suspension->m_wheelParams.setSize(hkVehicle->m_data->m_numWheels);
    suspension->m_wheelSpringParams.setSize(hkVehicle->m_data->m_numWheels);

    const hkVector4 downDirection(0.0f, -1.0f, 0.0f);
    const hkVector4 downDirectionp(0.0f, -1.0f, 0.1f);
    const hkVector4 downDirectionm(0.0f, -1.0f, -0.1f);
    for (unsigned int i = 0; i < tyres.size(); i++)
    {
        suspension->m_wheelParams[i].m_length = tyres[i]->tyreType->suspensionLength;
        suspension->m_wheelSpringParams[i].m_strength = tyres[i]->tyreType->suspensionSpring;
        suspension->m_wheelSpringParams[i].m_dampingCompression = tyres[i]->tyreType->suspensionDamper;
        suspension->m_wheelSpringParams[i].m_dampingRelaxation = tyres[i]->tyreType->suspensionDamper;
        suspension->m_wheelParams[i].m_hardpointChassisSpace.set(tyres[i]->tyreType->localPos.X, -0.05f, tyres[i]->tyreType->localPos.Z); 
        /*
        if (m_tires[i]->tirePosition.Z > 0.0f)
        {
            suspension.m_wheelParams[i].m_directionChassisSpace = downDirectionp;
        }
        else
        {
            suspension.m_wheelParams[i].m_directionChassisSpace = downDirectionm;
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
    //assert(0);
}

Vehicle::~Vehicle()
{
    dprintf(MY_DEBUG_NOTE, "Vehicle::~Vehicle(): %p\n", this);
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
}

void Vehicle::handleUpdatePos(bool phys)
{
    if (phys)
    {
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

        float speed = getSpeed();
        float angularVelocity = fabsf(speed);
        float ssSpeed = angularVelocity/80.f+1.0f;
        float ssGear = (hkVehicle->calcRPM() * 1.5f) / vehicleType->maxTorque + 1.0f;

        if (ssGear > 2.5f) ssGear = 2.5f;

        const irr::core::vector3df soundPos = node->getPosition(); //(matrix*soundMatrix).getTranslation();
        if (engineSound)
        {
            engineSound->setPosition(soundPos);
            engineSound->setPlaybackSpeed(ssGear);
            engineSound->setVelocity(linearVelocity);
        }
        
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
            }
        }
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
    dprintf(MY_DEBUG_NOTE, "reset car:  mod rot: %f %f %f\n", rot.X, rot.Y, rot.Z);
    matrix.setTranslation(pos);
    // vector3df(0.f, rot.Y, 0.f)
    matrix.setRotationDegrees(rot);
    //hkBody->setLinearVelocity(hkVector4());
    updateToMatrix();
}

float Vehicle::getAngle() const
{
    irr::core::vector3df rot = matrix.getRotationDegrees();
    //dprintf(printf("reset car: orig rot: %f %f %f\n", rot.X, rot.Y, rot.Z));
    if (fabsf(rot.Z-180.f) < 90.f)
    {
        if (rot.Y < 90.f)
            rot.Y = 180.f - rot.Y;
        if (rot.Y > 270.f)
            rot.Y = 540.f - rot.Y;
    }
    rot.Z = rot.X = 0.f;
    //dprintf(printf("reset car:  mod rot: %f %f %f\n", rot.X, rot.Y, rot.Z));
    return rot.Y;
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
    deviceStatus->m_positionY = hkMath::clamp(value, -1.0f, 1.0f);
}

void Vehicle::setHandbrake(float value)
{
    hkpVehicleDriverInputAnalogStatus* deviceStatus = (hkpVehicleDriverInputAnalogStatus*)hkVehicle->m_deviceStatus;
    deviceStatus->m_handbrakeButtonPressed = value > 0.1f;
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

