#ifndef _SDK_THREAD_H_
#define _SDK_THREAD_H

#ifdef _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

/** 
 * Header Files
 */
#include "windows.h"
#include <deque>
#include "assert.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include <process.h>
#endif
#define EXPORT __declspec(dllexport)

#else
#include "pthread.h"
#define EXPORT
#endif

/**
 * suppress the warning #810 if intel compiler is used.
 */
#if defined(__INTEL_COMPILER)
#pragma warning(disable : 810)
#endif 

//#define PRINT_COND_VAR_ERROR_MSG
#ifdef PRINT_COND_VAR_ERROR_MSG
#define PRINT_ERROR_MSG(errorcode, msg) \
    if(errorcode != 0) \
        printf("%s \n", msg)
#else
#define PRINT_ERROR_MSG(errorcode, msg)    
#endif // PRINT_COND_VAR_ERROR_MSG

/** 
 * namespace streamsdk
 */
namespace streamsdk
{
    /**
	 * Entry point for the thread
     * prototype of the entry point in windows
	 */
    typedef void* (*threadFunc)(void*);

    /**
	 * class ThreadLock
     *  \brief Provides a wrapper for locking primitives used to 
     *   synchronize _CPU_ threads.
     * 
     *  Common usage would be:
     * 
     *     CALLock lock;
     * 
     *     // Critical section begins
     * 
     *     lock.lock();
     *
     * Critical section ends
     *
     *    lock.unlock();
     */

    /**
	 * class ThreadLock
	 */
	class EXPORT ThreadLock
    {
        public:

           /**
		    * Constructor
			*/
           ThreadLock();

           /**
		    * Destructor
			*/
           ~ThreadLock();

           /**
		    * Returns true if the lock is already locked, false otherwise
			*/
           bool isLocked();

           /**
		    * Try to acquire the lock, if available continue, else wait on the lock
			*/
           void lock();

           /**
		    * Try to acquire the lock, if available, hold it, else continue doing something else
			*/
           bool tryLock();

           /**
		    * Unlock the lock and return
			*/
           void unlock();

        private:

           /**
            * Private data members and methods
            */
           
           /**
		    * System specific synchronization primitive
			*/
        #ifdef _WIN32
           CRITICAL_SECTION _cs;
        #else
           pthread_mutex_t _lock;
        #endif
    };


    /**
     * class Condition variable
     * Provides a wrapper for creating a condition variable
     * This class provides a simple wrapper to a condition variable
     */

    class CondVarImpl;
    class EXPORT CondVar
    {
        public:
            /**
			 * constructor and destructor. 
             * Note that condition variable is not initialized in constructor 
             * Separate functions available to initialize and destroy condition variable
			 */
            CondVar();
            ~CondVar();

            /**
			 * Initialize condition variable
			 */
            bool init(unsigned int maxThreadCount);

            /**
			 * Destroy condition variable
			 */
            bool destroy();

            /**
			 * Synchronize threads
			 */
            void syncThreads();


        private:

            /** 
			 * Pointer to Implementation class
			 */
            CondVarImpl* _condVarImpl;

    };


    
    /**
     * \class Thread
     * \brief Provides a wrapper for creating a _CPU_ thread.
     * This class provides a simple wrapper to a CPU thread/
     */
	class EXPORT SDKThread
    {
        public:
           /**
		    * Thread constructor and destructor. Note that the thread is
            * NOT created in the constructor. The thread creation takes
            *  place in the create method
			*/
           SDKThread();

           ~SDKThread();
           
           /**
		    * Wrapper for pthread_create. Pass the thread's entry
            * point and data to be passed to the routine
			*/
           bool create(threadFunc func, void* arg);

           /** 
		    * Wrapper for pthread_join. The calling thread
			* will wait until _this_ thread exits
			*/
           bool join();

           /**
		    * Get the thread data passed by the application
			*/
           void* getData() { return _data; }

           /** 
		    * Get the thread ID 
			*/
           unsigned int getID();

        private:

           /**
            * Private data members and methods
            */

        #ifdef _WIN32
           /**
		    * store the handle
			*/
           HANDLE _tid;
        #else
           pthread_t _tid;
        #endif

           void *_data;

    };


	/**
     * CondVarImpl 
	 * class Implementation of Condition variable class
     * Provides Implementation of condition variable
	 * Note that condition variable is not initialized in constructor 
	 * Separate functions available to initialize and destroy condition variable
     */

    class CondVarImpl
    {
        public:
            /**
			 * Constructor 
			 */
            CondVarImpl();
            
			
			/**
			 * Destructor 
			 */
			~CondVarImpl();

            /**
			 * Initialize condition variable
			 */
            bool init(unsigned int maxThreadCount);

            /**
			 * Destroy condition variable
			 */
            bool destroy();

            /**
			 * Synchronize threads
			 */
            void syncThreads();


        private:
          
            /**
			 * Acquires the lock once. 
			 */
            void beginSynchronized();

            /**
			 * Releases the lock once. 
			 */
            int endSynchronized();

#ifdef _WIN32

            /**
			 * Waits for a notification. 
			 */
            DWORD wait(DWORD dwMillisecondsTimeout = INFINITE, BOOL bAlertable = FALSE);

            /**
			 * Notifies the waiting threads 
			 */
            BOOL broadcast();

            /**
			 * Creates an initially non-signalled auto-reset event and
             * pushes the handle to the event onto the wait set. The
             * return value is the event handle. In case of failure,
             * NULL is returned.
			 */
            HANDLE _push();

            /**
			 * Pops the first handle off the wait set. Returns NULL if the
             * wait set was empty.
			 */
            HANDLE _pop();

            /** 
			 * Checks whether the calling thread is holding the lock.
			 */
            BOOL _lockHeldByCallingThread();


            /**
			 * STL deque that implements the wait set.
			 */
            std::deque<HANDLE> _deqWaitSet;

            /**
			 * Critical section to protect access to wait set.
			 */
            CRITICAL_SECTION _critsecWaitSetProtection;

            /** 
			 * Critical section for external synchronization.
			 */
            CRITICAL_SECTION _condVarLock;

            /**
			 * The monitor must keep track of how many times the lock
			 * has been acquired, because Win32 does not divulge this
             * information to the client programmer.
			 */
            int _nLockCount;

 #else

            /**
			 * condition variable
			 */
            pthread_cond_t _condVar;

            /**
			 * Mutex for condition variable _condVar
			 */
            pthread_mutex_t _condVarLock;

#endif

            /** 
			 * Maximum threads in a group
			 */
            unsigned int _maxThreads;

            /** 
			 * Number of threads waiting 
			 */
            unsigned int _count;

    };
}

#endif // _CPU_THREAD_H_
