#ifndef VEHICLE_H
#define VEHICLE_H

#include <string>
#include <irrlicht.h>
#include <vector>
#include "hk.h"
#include "OffsetObject.h"

#include <Physics/Vehicle/AeroDynamics/Default/hkpVehicleDefaultAerodynamics.h>
#include <Physics/Vehicle/DriverInput/Default/hkpVehicleDefaultAnalogDriverInput.h>
#include <Physics/Vehicle/Brake/Default/hkpVehicleDefaultBrake.h>
#include <Physics/Vehicle/Engine/Default/hkpVehicleDefaultEngine.h>
#include <Physics/Vehicle/VelocityDamper/Default/hkpVehicleDefaultVelocityDamper.h>
#include <Physics/Vehicle/Steering/Default/hkpVehicleDefaultSteering.h>
#include <Physics/Vehicle/Suspension/Default/hkpVehicleDefaultSuspension.h>
#include <Physics/Vehicle/Transmission/Default/hkpVehicleDefaultTransmission.h>
#include <Physics/Vehicle/WheelCollide/RayCast/hkpVehicleRayCastWheelCollide.h>
#include <Physics/Vehicle/TyreMarks/hkpTyremarksInfo.h>


class VehicleType;
class VehicleTypeTyre;
class MySound;
class Smoke;
class VehicleCollisionListener;


// -------------------------------------------------------
//                     VehicleTyre
// -------------------------------------------------------

class Vehicle;
class VehicleTyre
{
public:
    VehicleTyre(VehicleTypeTyre* p_vehicleTypeTyre, const irr::core::vector3df& apos, Vehicle* vehicle, unsigned int tyreNum);
    ~VehicleTyre();

private:
    Vehicle*                vehicle;
    unsigned int            tyreNum;
    irr::core::matrix4      localMatrix;
    irr::scene::ISceneNode* node;
    VehicleTypeTyre*        tyreType;
    OffsetObject*           offsetObject;
    hkpRigidBody*           hitBody;
    float                   width;
    float                   radius;

    friend class Vehicle;
    friend class FrictionMapVehicleRaycastWheelCollide;
};
typedef std::vector<VehicleTyre*> tyreVector_t;


// -------------------------------------------------------
//                       Vehicle
// -------------------------------------------------------

class Vehicle : public OffsetObjectUpdateCB
{
public:
    /* constructor:
            suspensionSpringModifier and damperModifier must be between -20 .. 20,
    */
    Vehicle(const std::string& vehicleTypeName, const irr::core::vector3df& apos, const irr::core::vector3df& rotation,
        bool manual = false, bool sequential = true, float suspensionSpringModifier = 0.0f, float suspensionDamperModifier = 0.0f);
    ~Vehicle();

    void reset(const irr::core::vector3df& pos);
    float getAngle() const;
    int getGear() const;
    float getSpeed() const {return hkVehicle->calcKMPH();}
    float getRPM() const {return hkVehicle->calcRPM();}
    const irr::core::vector3df& getLinearVelocity() {return linearVelocity;}
    VehicleType* getVehicleType() {return vehicleType;}
    const irr::core::matrix4& getViewPos(unsigned int num) const;
    const irr::core::matrix4& getViewDest(unsigned int num) const;
    const irr::core::matrix4& getMatrix() const {return matrix;}
    float getDistance() const {return distance;}
    bool getGearShifting() const;
    bool getGearShiftingSequential() const;

    void setSteer(float value);
    void setTorque(float value);
    void setHandbrake(float value);
    void setClutch(float value) {clutch = value;}
    void setDistance(float distance) {this->distance = distance;}
    void setGearShifting(bool manual, bool sequential);
    void setGear(char gear);
    void incGear();
    void decGear();

    /* constructor:
            suspensionSpringModifier must be between -20 .. 20,
    */
    void modifySuspensionSpring(float suspensionSpringModifier = 0.0f);
    /* constructor:
            damperModifier must be between -20 .. 20,
    */
    void modifySuspensionDamper(float suspensionDamperModifier = 0.0f);
    
    void pause();
    void resume();

    void setNameText(irr::scene::ITextSceneNode* nameText) {this->nameText = nameText;}

private:
    virtual void handleUpdatePos(bool phys);
    void updateToMatrix();
    void addSmoke(const float speed, const irr::core::vector3df& pos, float offset);
    void updateSmoke();

private:
    VehicleType*                vehicleType;
    VehicleCollisionListener*   collisionListener;

    irr::core::matrix4          matrix;
    irr::scene::ISceneNode*     node;
    hkpRigidBody*               hkBody;
    OffsetObject*               offsetObject;
    tyreVector_t                tyres;
    hkpVehicleInstance*         hkVehicle;
    MySound*                    engineSound;
    irr::core::vector3df        linearVelocity;
    float                       distance;
    irr::core::vector3df        lastPos;
    Smoke**                     smokes;
    unsigned int                physUpdates;
    float                       clutch;
    float                       suspensionSpringModifier;
    float                       suspensionDamperModifier;
    irr::scene::ITextSceneNode* nameText;


    friend class FrictionMapVehicleRaycastWheelCollide;
    friend class VehicleTyre;
    friend class VehicleTransmission;
    friend class VehicleCollisionListener;
};

#endif // VEHICLE_H
