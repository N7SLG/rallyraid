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
    
    MySound* play3D(const std::string& soundFileName, const irr::core::vector3df& pos, bool playLooped = false,
                    bool startPaused = false, bool track = false);
    MySound* play2D(const std::string& soundFileName, bool playLooped = false,
                    bool startPaused = false, bool track = false); // inline
    void setListenerPosition(const irr::core::vector3df& pos,
                             const irr::core::vector3df& lookdir,
                             const irr::core::vector3df& upVector = irr::core::vector3df(0, 1, 0),
                             const irr::core::vector3df& velPerSecond = irr::core::vector3df(0, 0, 0)
                             ); // inline

    const irr::core::vector3df& getListenerPosition() const; // inline
    
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

inline MySound* MySoundEngine::play2D(const std::string& soundFileName, bool playLooped,
                               bool startPaused, bool track)
{
    return play3D(soundFileName, listenerPos, playLooped, startPaused, track);
}

inline void MySoundEngine::setListenerPosition(const irr::core::vector3df& pos,
                                        const irr::core::vector3df& lookdir,
                                        const irr::core::vector3df& upVector,
                                        const irr::core::vector3df& velPerSecond)
{
    listenerPos = -pos;
    listenerVel = velPerSecond;
    listenerDir = lookdir;
    listenerUp = upVector;
    
    updateListener();
}

inline const irr::core::vector3df& MySoundEngine::getListenerPosition() const
{
    return listenerPos;
}

class MySound
{
public:
    MySound(unsigned int newBuffer, bool plooped, const irr::core::vector3df& pos);
    ~MySound();
    
    void setVolume(float newVolume);
    float getVolume() const; // inline

    void setPlaybackSpeed(float newPlaybackSpeed);
    float getPlaybackSpeed() const; // inline
    
    void setIsLooped(bool newLoop);
    bool isLooped() const; // inline
    
    void setPosition(const irr::core::vector3df& newPos);
    const irr::core::vector3df& getPosition() const; // inline
    
    void setVelocity(const irr::core::vector3df& newVelocity);
    const irr::core::vector3df& getVelocity() const; // inline
    
    void setMaxDistance(float newMaxDistance);
    float getMaxDistance() const; // inline
    void setMinDistance(float newMinDistance);
    float getMinDistance() const; // inline
    
    void play();
    void stop();
    
    void setIsPaused(bool newPaused);
    bool isPaused() const; // inline
    
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

    static const irr::core::vector3df zeroVector;
    
};

inline float MySound::getVolume() const
{
    if (!this || !initialized) return 0.f;
    return volume;
}

inline float MySound::getPlaybackSpeed() const
{
    if (!this || !initialized) return 0.f;
    return playbackSpeed;
}

inline bool MySound::isLooped() const
{
    if (!this || !initialized) return false;
    return looped;
}

inline const irr::core::vector3df& MySound::getPosition() const
{
    if (!this || !initialized) return zeroVector;
    return soundSourcePos;
}

inline const irr::core::vector3df& MySound::getVelocity() const
{
    if (!this || !initialized) return zeroVector;
    return soundSourceVel;
}

inline float MySound::getMaxDistance() const
{
    if (!this || !initialized) return 0.f;
    return maxDistance;
}

inline float MySound::getMinDistance() const
{
    if (!this || !initialized) return 0.f;
    return minDistance;
}

inline bool MySound::isPaused() const
{
    if (!this || !initialized) return false;
    return paused;
}

#endif // MYSOUND_H
