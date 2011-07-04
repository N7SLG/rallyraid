#ifndef MYTHREAD_H
#define MYTHREAD_H

#ifdef __linux__
# include <pthread.h>
#else
# include <windows.h>
#endif

class MyThread
{
public:
    MyThread();
    ~MyThread();
    
    void execute();
    
    void kill();
    void terminateAndWait();
    
protected:
    virtual void run() = 0;
    void run_in();

private:
    void lock();
    void unlock();

    volatile bool underTermination;
    volatile bool terminated;

#ifdef __linux__
    static void* ThreadEntry(void* arg);
    pthread_t thread;
//    pthread_mutex_t mutex[2];
//    volatile int currentMutex;
    pthread_mutex_t mutex;
//    pthread_spinlock_t spinLock;
#else
    static DWORD WINAPI ThreadEntry (void* pArg);
    HANDLE handle;
    //CRITICAL_SECTION critSection[2];
    //volatile int currentCritSection;
    DWORD tid;
#endif
};

#endif // MYTHREAD_H
