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


class VehicleCollisionCB
{
protected:
    virtual ~VehicleCollisionCB() {}

    /*
        called when the driven vehicle caused the collision and
        result in penalty
    */
    virtual void handleHardCollision(float w) = 0;

    /*
        the vehicle participate in collision, but
        not result in penalty
    */
    virtual void handleSoftCollision(float w) = 0;


    friend class VehicleCollisionListener;
};


// -------------------------------------------------------
//                       Vehicle
// -------------------------------------------------------

class Vehicle : public OffsetObjectUpdateCB
{
public:
    /* constructor:
            suspensionSpringModifier and damperModifier must be between -20 .. 20,
            brakeBalance: 0.0 .. 1.0
    */
    Vehicle(const std::string& vehicleTypeName, const irr::core::vector3df& apos, const irr::core::vector3df& rotation,
        bool manual = false, bool sequential = true, float suspensionSpringModifier = 0.0f, float suspensionDamperModifier = 0.0f,
        float brakeBalance = 0.2f);
    ~Vehicle();

    void reset(const irr::core::vector3df& pos);
    void addStartImpulse(float initialSpeed, const irr::core::vector3df& dir);
    float getAngle() const;
    int getGear() const;
    float getSpeed() const {return hkVehicle->calcKMPH();}
    float getRPM() const {return hkVehicle->calcRPM();}
    const irr::core::vector3df& getLinearVelocity() {return linearVelocity;}
    VehicleType* getVehicleType() {return vehicleType;}
    const irr::core::matrix4& getViewPos(unsigned int num) const;
    const irr::core::matrix4& getViewDest(unsigned int num) const;
    const irr::core::matrix4& getMatrix() const {return matrix;}
    const irr::core::vector3df& getRotationDegrees() const {return rot;}
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
    
    /* constructor:
            damperModifier must be between 0.0f .. 1.0f,
    */
    void modifyBrakeBalance(float brakeBalance = 0.2f);
    
    void pause();
    void resume();

    void setNameText(irr::scene::ITextSceneNode* nameText) {this->nameText = nameText; updateNameTextPos();}
    void setVehicleCollisionCB(VehicleCollisionCB* vehicleCollisionCB) {this->vehicleCollisionCB = vehicleCollisionCB;}

private:
    virtual void handleUpdatePos(bool phys);
    void updateToMatrix();
    void addSmoke(const float speed, const irr::core::vector3df& pos, float offset);
    void updateSmoke();
    void updateNameTextPos();
    void updateAngle();

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
    irr::core::vector3df        rot;
    irr::core::vector3df        soundPosAdjustment;
    float                       angle;
    Smoke**                     smokes;
    unsigned int                physUpdates;
    float                       clutch;
    float                       suspensionSpringModifier;
    float                       suspensionDamperModifier;
    float                       brakeBalance;
    irr::scene::ITextSceneNode* nameText;
    VehicleCollisionCB*         vehicleCollisionCB;


    friend class FrictionMapVehicleRaycastWheelCollide;
    friend class VehicleTyre;
    friend class VehicleTransmission;
    friend class VehicleCollisionListener;
};

#endif // VEHICLE_H
