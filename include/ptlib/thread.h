/*
 * thread.h
 *
 * Executable thread encapsulation class (pre-emptive if OS allows).
 *
 * Portable Tools Library
 *
 * Copyright (c) 1993-1998 Equivalence Pty. Ltd.
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is Portable Windows Library.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Portions are Copyright (C) 1993 Free Software Foundation, Inc.
 * All Rights Reserved.
 *
 * Contributor(s): ______________________________________.
 *
 * $Revision: 30585 $
 * $Author: rjongbloed $
 * $Date: 2013-09-30 01:51:43 -0500 (Mon, 30 Sep 2013) $
 */

#ifndef PTLIB_THREAD_H
#define PTLIB_THREAD_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#ifdef Priority
#undef Priority
#endif

#include <ptlib/mutex.h>
#include <set>


class PSemaphore;


///////////////////////////////////////////////////////////////////////////////
// PThread

/** This class defines a thread of execution in the system. A <i>thread</i> is
   an independent flow of processor instructions. This differs from a
   <i>process</i> which also embodies a program address space and resource
   allocation. So threads can share memory and resources as they run in the
   context of a given process. A process always contains at least one thread.
   This is reflected in this library by the <code>PProcess</code> class being
   descended from the <code>PThread</code> class.

   The implementation of a thread is platform dependent, but it is
   assumed that the platform has some support for native threads.
   Previous versions of PTLib/PWLib have some support for co-operative
   threads, but this has been removed.
 */
class PThread : public PObject
{
  PCLASSINFO(PThread, PObject);

  public:
  /**@name Construction */
  //@{
    /// Codes for thread priorities.
    enum Priority {
      LowestPriority,  ///< Will only run if all other threads are blocked.

      LowPriority,     ///< Runs approximately half as often as normal.

      NormalPriority,  ///< Normal priority for a thread.

      HighPriority,    ///< Runs approximately twice as often as normal.

      HighestPriority, ///< Is only thread that will run, unless blocked.

      NumPriorities
    };

    /// Codes for thread autodelete flag
    enum AutoDeleteFlag {
      /// Automatically delete thread object on termination.
      AutoDeleteThread,   

      /// Don't delete thread as it may not be on heap.
      NoAutoDeleteThread  
    };

    /** Create a new thread instance. Unless the <code>startSuspended</code>
       parameter is <code>true</code>, the threads <code>Main()</code> function is called to
       execute the code for the thread.
       
       Note that the exact timing of the execution of code in threads can
       never be predicted. Thus you you can get a race condition on
       intialising a descendent class. To avoid this problem a thread is
       always started suspended. You HAVE TO call the <code>Resume()</code> function after
       your descendent class construction is complete.

       If synchronization is required between threads then the use of
       semaphores is essential.

       If the <code>deletion</code> is set to <code>AutoDeleteThread</code>
       then the <code>PThread</code> is assumed to be allocated with the new operator and
       may be freed using the delete operator as soon as the thread is
       terminated or executes to completion (usually the latter).

       The stack size argument is highly platform specific and should alamost
       always be zero for the default.
     */
    PThread(
      PINDEX stack,  ///< Stack size on some platforms, 0 == default
      AutoDeleteFlag deletion = AutoDeleteThread,
        ///< Automatically delete PThread instance on termination of thread.
      Priority priorityLevel = NormalPriority,  ///< Initial priority of thread.
      const PString & threadName = PString::Empty() ///< The name of the thread (for Debug/Trace)
    );

    /** Destroy the thread, this simply calls the <code>Terminate()</code> function
       with all its restrictions and penalties. See that function for more
       information.

       NOTE that the correct way for a thread to finalize is to return from
       the <code>Main()</code> function.
     */
    ~PThread();
  //@}

  /**@name Overrides from <code>PObject</code> */
  //@{
    /**Standard stream print function.
       The <code>PObject</code> class has a << operator defined that calls this function
       polymorphically.
      */
    void PrintOn(
      ostream & strm    ///< Stream to output text representation
    ) const;
  //@}

  /**@name Control functions */
  //@{
    /** Restart a terminated thread using the same stack priority etc that
       was current when the thread terminated.

       If the thread is still running then this function is ignored.
     */
    virtual void Restart();

    /** Terminate the thread. It is highly recommended that THIS IS NOT USED
       except in abnormal abort situations as not all clean up of resources
       allocated to the thread will be executed. This is especially true in
       C++ as the destructors of objects that are automatic variables are not
       called causing at the very least the possiblity of memory leaks.

       Note that the correct way for a thread to terminate is to return from
       the <code>Main()</code> function or self terminate by calling
       <code>Terminate()</code> within the context of the thread which can then
       assure that all resources are cleaned up.
     */
    virtual void Terminate();

    /** Determine if the thread has been terminated or ran to completion.

       @return
       <code>true</code> if the thread has been terminated.
     */
    virtual PBoolean IsTerminated() const;

    /** Block and wait for the thread to terminate.
     */
    void WaitForTermination() const;

    /** Block and wait for the thread to terminate.

       @return
       <code>false</code> if the thread has not terminated and the timeout has expired, <code>true</code> otherwise.
     */
    PBoolean WaitForTermination(
      const PTimeInterval & maxWait  ///< Maximum time to wait for termination.
    ) const;

    /** Suspend or resume the thread.
    
       If <code>susp</code> is <code>true</code> this increments an internal count of
       suspensions that must be matched by an equal number of calls to
       <code>Resume()</code> or <code>Suspend(false)</code> before the
       thread actually executes again.

       If <code>susp</code> is <code>false</code> then this decrements the internal count of
       suspensions. If the count is <= 0 then the thread will run. Note that
       the thread will not be suspended until an equal number of
       <code>Suspend(true)</code> calls are made.
     */
    virtual void Suspend(
      PBoolean susp = true    ///< Flag to suspend or resume a thread.
    );

    /** Resume thread execution, this is identical to
       <code>Suspend(false)</code>.

      The Resume() method MUST NOT be called from the constructor of
      descendant classes!
     */
    virtual void Resume();

    /** Determine if the thread is currently suspended. This checks the
       suspension count and if greater than zero returns <code>true</code> for a suspended
       thread.

       @return
       <code>true</code> if thread is suspended.
     */
    virtual PBoolean IsSuspended() const;

    /// Suspend the current thread for the specified amount of time.
    static void Sleep(
      const PTimeInterval & delay   ///< Time interval to sleep for.
    );

    /** Set the priority of the thread relative to other threads in the current
       process.
     */
    virtual void SetPriority(
      Priority priorityLevel    ///< New priority for thread.
    );

    /** Get the current priority of the thread in the current process.

       @return
       current thread priority.
     */
    virtual Priority GetPriority() const;

    /** Set the flag indicating thread object is to be automatically deleted
        when the thread ends.
     */
    virtual void SetAutoDelete(
      AutoDeleteFlag deletion = AutoDeleteThread  ///< New auto delete setting.
    );

    /** Reet the flag indicating thread object is to be automatically deleted
        when the thread ends.
     */
    void SetNoAutoDelete() { SetAutoDelete(NoAutoDeleteThread); }

    /** Get the name of the thread. Thread names are a optional debugging aid.

       @return
       current thread name.
     */
    virtual PString GetThreadName() const;

    /** Change the name of the thread. Thread names are a optional debugging aid.

       @return
       current thread name.
     */
    virtual void SetThreadName(
      const PString & name        ///< New name for the thread.
    );
  //@}

  /** @name Miscellaneous */
  /// @{
    /** Get operating system specific thread identifier for this thread.
      * Note that the return value from these functions is only valid
      * if called by the owning thread. Calling this function for another
      * thread that may be terminating is a very bad idea.
      */
    virtual PThreadIdentifier GetThreadId() const { return m_threadId; }
    static PThreadIdentifier GetCurrentThreadId();

    /// Times for execution of the thread.
    struct Times
    {
      PTimeInterval m_real;     ///< Total real time since thread start in milliseconds.
      PTimeInterval m_kernel;   ///< Total kernel CPU time in milliseconds.
      PTimeInterval m_user;     ///< Total user CPU time in milliseconds.
      friend ostream & operator<<(ostream & strm, const Times & times);
    };

    /** Get the thread execution times.
     */
    bool GetTimes(
      Times & times   ///< Times for thread execution.
    );

    /** User override function for the main execution routine of the thread. A
       descendent class must provide the code that will be executed in the
       thread within this function.

       NOTE that the correct way for a thread to terminate is to return from
       this function.
     */
    virtual void Main() = 0;

    /** Get the currently running thread object instance. It is possible, even
       likely, that the smae code may be executed in the context of differenct
       threads. Under some circumstances it may be necessary to know what the
       current codes thread is and this static function provides that
       information.

       @return
       pointer to current thread.
     */
    static PThread * Current();

    /** Yield to another thread without blocking.
        This duplicates the implicit thread yield that may occur on some
        I/O operations or system calls.

        This may not be implemented on all platforms.
     */
    static void Yield();

    /** Create a simple thread executing the specified notifier.
       This creates a simple <code>PThread</code> class that automatically executes the
       function defined by the <code>PNotifier</code> in the context of a new thread.
      */
    static PThread * Create(
      const PNotifier & notifier,     ///< Function to execute in thread.
      P_INT_PTR parameter = 0,              ///< Parameter value to pass to notifier.
      AutoDeleteFlag deletion = AutoDeleteThread,
        ///< Automatically delete PThread instance on termination of thread.
      Priority priorityLevel = NormalPriority,  ///< Initial priority of thread.
      const PString & threadName = PString::Empty(), ///< The name of the thread (for Debug/Trace)
      PINDEX stackSize = 0            ///< Stack size on some platforms, 0 == default
    );

    static PThread * Create(
      const PNotifier & notifier,     ///< Function to execute in thread.
      const PString & threadName      ///< The name of the thread (for Debug/Trace)
    )
    {
        return Create(notifier, 0, NoAutoDeleteThread, NormalPriority, threadName);
    }
  /// @}

    bool IsAutoDelete() const { return m_type == e_IsAutoDelete || m_type == e_IsExternal; }

    /// Thread local storage base class, see PThreadLocalStorage for template.
    class LocalStorageBase
    {
      public:
        virtual ~LocalStorageBase() { }
      protected:
        LocalStorageBase() { }
        void StorageDestroyed();
        virtual void * Allocate() const = 0;
        virtual void Deallocate(void * ptr) const = 0;
        virtual void * GetStorage() const;
      private:
        void ThreadDestroyed(PThread * thread) const;
        typedef std::map<PThread *, void *> StorageMap;
        mutable StorageMap m_storage;
        PMutex m_mutex;
      friend class PThread;
    };
    friend class LocalStorageBase;

  private:
    PThread(bool isProcess);
    // Create a new thread instance as part of a <code>PProcess</code> class.

    void InternalDestroy();

    friend class PProcess;
    friend class PExternalThread;
    // So a PProcess can get at PThread() constructor but nothing else.

    PThread(const PThread &) : PObject () { }
    // Empty constructor to prevent copying of thread instances.

    PThread & operator=(const PThread &) { return *this; }
    // Empty assignment operator to prevent copying of thread instances.

  protected:
    enum Type { e_IsAutoDelete, e_IsManualDelete, e_IsProcess, e_IsExternal } m_type;

    PINDEX m_originalStackSize;

    PString m_threadName; // Give the thread a name for debugging purposes.
    PMutex  m_threadNameMutex;

    PThreadIdentifier m_threadId;

    typedef std::list<const LocalStorageBase *> LocalStorageList;
    LocalStorageList m_localStorage;

// Include platform dependent part of class
#ifdef _WIN32
#include "msos/ptlib/thread.h"
#else
#include "unix/ptlib/thread.h"
#endif
};


/** Define some templates to simplify the declaration
  * of simple <code>PThread</code> descendants with one or two paramaters.
  *
  * Please take into account documentation on PThread::Resume method.
  */

/*
   This template automates calling a global function using a functor
   It is used as follows:

   struct Functor {
     void operator()(PThread & thread) { ... code in here }
   }

   ...
   Functor arg;
   PThread * thread = new PThreadFunctor<Functor>(arg)
   thread->Resume();
 */
template<typename Functor>
class PThreadFunctor : public PThread
{
    PCLASSINFO(PThreadFunctor, PThread);
  public:
    PThreadFunctor(Functor & funct, bool autoDel = false)
      : PThread(10000, autoDel ? PThread::AutoDeleteThread : PThread::NoAutoDeleteThread)
      , m_funct(funct)
    {
    }

    virtual void Main()
    {
      m_funct(*this);
    }

  protected:
    Functor & m_funct;
};

/*
   This template automates calling a member function with no arguments within it's own thread.
   It is used as follows:

   class Example {
     public:
      void Function()
      {
      }
   };

   ...
   Example ex;
   PThread * thread = new PThreadObj<Example>(ex, &Example::Function)
   thread->Resume();
 */

template <typename ObjType>
class PThreadObj : public PThread
{
    PCLASSINFO(PThreadObj, PThread);
  public:
    typedef void (ObjType::*ObjTypeFn)(); 

    PThreadObj(
      ObjType & obj,
      ObjTypeFn function,
      bool autoDel = false,
      const char * name = NULL,
      PThread::Priority priority = PThread::NormalPriority
    ) : PThread(10000,
                autoDel ? PThread::AutoDeleteThread : PThread::NoAutoDeleteThread,
                priority,
                name)
      , m_object(obj)
      , m_function(function)
    {
    }

    void Main()
    {
      (m_object.*m_function)();
    }

  protected:
    ObjType & m_object;
    ObjTypeFn P_ALIGN_FIELD(m_function, 8);
};

///////////////////////////////////////////////////////////////////////////////
//
// PThreadLocalStorage
//

template <class Storage_T>
class PThreadLocalStorage : public PThread::LocalStorageBase
{
  public:
    typedef Storage_T * Ptr_T;

    ~PThreadLocalStorage()        { this->StorageDestroyed(); }
    Ptr_T Get() const             { return  (Ptr_T)this->GetStorage(); }
    operator Ptr_T() const        { return  (Ptr_T)this->GetStorage(); }
    Ptr_T operator->() const      { return  (Ptr_T)this->GetStorage(); }
    Storage_T & operator*() const { return *(Ptr_T)this->GetStorage(); }

  protected:
    virtual void * Allocate() const           { return new Storage_T(); }
    virtual void Deallocate(void * ptr) const { delete (Ptr_T)ptr; }
};


#define P_HAS_THREADLOCAL_STORAGE 1  // For backward compatbility


#endif // PTLIB_THREAD_H

// End Of File ///////////////////////////////////////////////////////////////
