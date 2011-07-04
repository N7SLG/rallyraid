#ifndef MYSOUND_H
#define MYSOUND_H

#include <AL/alc.h>

#include <irrlicht.h>
#include <map>
#include <string>
// Irrlicht Namespaces

#define NUM_OF_BUFFERS 64
#define NUM_OF_SOURCES 32
#define EMPTY_SLOT 0xFFFFFFFF

class MySound;

//struct SoundFileNameBuffer
//{
//    char filename[256];
//    unsigned int buffer;
//};

class MySoundEngine
{
public:
    static void initialize();
    static void finalize();
    static MySoundEngine* getInstance() {return soundEngine;}

private:
    static MySoundEngine* soundEngine;

public:
    MySoundEngine();
    ~MySoundEngine();
    
    MySound* play3D(const std::string& soundFileName, irr::core::vector3df pos, bool playLooped=false,
                      bool startPaused=false, bool track=false);
    MySound* play2D(const std::string& soundFileName, bool playLooped=false,
                      bool startPaused=false, bool track=false);
    void setListenerPosition(const irr::core::vector3df &pos, const irr::core::vector3df &lookdir,
                             const irr::core::vector3df &upVector=irr::core::vector3df(0, 1, 0),
                             const irr::core::vector3df &velPerSecond=irr::core::vector3df(0, 0, 0)
                             );

    irr::core::vector3df getListenerPosition() const;
    
private:
    typedef std::map<std::string, unsigned int> bufferList_t;
    bufferList_t soundFileNameBuffer;
    
    void updateListener();
    
    bool initialized;
    
    irr::core::vector3df listenerPos;
    irr::core::vector3df listenerVel;
    irr::core::vector3df listenerDir;
    irr::core::vector3df listenerUp;

    ALCdevice* pDevice;
    ALCcontext* pContext;  
};


class MySound
{
public:
    MySound(unsigned int newBuffer, bool plooped, irr::core::vector3df &pos);
    ~MySound();
    
    void setVolume(float newVolume);
    float getVolume() const;

    void setPlaybackSpeed(float newPlaybackSpeed);
    float getPlaybackSpeed() const;
    
    void setIsLooped(bool newLoop);
    bool isLooped() const;
    
    void setPosition(irr::core::vector3df &newPos);
    void setPosition(irr::core::vector3df newPos);
    irr::core::vector3df getPosition() const;
    
    void setVelocity(irr::core::vector3df &newVelocity);
    irr::core::vector3df getVelocity() const;
    
    void setMaxDistance(float newMaxDistance);
    float getMaxDistance() const;
    void setMinDistance(float newMinDistance);
    float getMinDistance() const;
    
    void play();
    void stop();
    
    void setIsPaused(bool newPaused);
    bool isPaused() const;
    
private:
    
    unsigned int source;

    irr::core::vector3df soundSourcePos;
    irr::core::vector3df soundSourceVel;
    
    bool initialized;
    unsigned int buffer;
    bool paused;
    float volume;
    float playbackSpeed;
    float maxDistance;
    float minDistance;
    bool looped;
    bool isStopped;
    
};

#endif // MYSOUND_H
