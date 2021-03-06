#include "thread.h"
/**********************************************************************
    class: Thread (thread.cpp)

    author: S. Hau
    date: October 21, 2017
**********************************************************************/

namespace OS {

    //----------------------------------------------------------------------
    U32 Thread::s_threadCounter = 0;

    //----------------------------------------------------------------------
    Thread::Thread( JobQueue& jobQueue )
        : m_jobQueue( jobQueue )
    {
    }

    //----------------------------------------------------------------------
    Thread::~Thread()
    {
        _Join();
    }

    //----------------------------------------------------------------------
    void Thread::_ThreadLoop()
    {
        while (true)
        {
            // Try to grab a job. The thread will be put to sleep if no job is available
            m_currentJob = m_jobQueue.grabJob();

            // If the job is null, terminate this thread
            if ( m_currentJob == nullptr )
                break;

            // Execute the job
            (*m_currentJob)();

            m_currentJob = nullptr;
        }
    }

    //----------------------------------------------------------------------
    void Thread::_Join()
    {
        if ( m_thread.joinable() )
        {
            m_thread.join();
        }
    }


} // end namespaces