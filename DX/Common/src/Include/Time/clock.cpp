#include "clock.h"
/**********************************************************************
    class: Clock (clock.cpp)
    
    author: S. Hau
    date: October 27, 2017

    @Considerations:
       - Sophisticated ID generator
       - Better data structure for callback-functions
         + sorted linked-list and check only next elem? (Save index where you were at last frame)
**********************************************************************/

#include "Logging/logging.h"

namespace Time {

    //----------------------------------------------------------------------
    // Check if a given value is between the two boundaries b1 and b2.
    // @Return:
    //   True if [b1 <= val <= b2] or 
    //     WRAP-CASE (b1 < b2): [val >= b1 || val <= v2]
    //----------------------------------------------------------------------
    bool isBetweenCircular( Milliseconds b1, Milliseconds val, Milliseconds b2 )
    {
        if (b1 <= b2)
            return ( ( val >= b1 ) && ( val <= b2 ) );
        else
            return ( ( val >= b1 ) || ( val <= b2 ) );
    }

    //----------------------------------------------------------------------
    static U64 NextID()
    {
        static U64 callbackIDs = 0;
        callbackIDs++;
        return callbackIDs;
    }

    //----------------------------------------------------------------------
    Clock::Clock( Milliseconds duration, bool isLooping )
        : m_curTime( 0, duration ), m_isLooping( isLooping )
    {
    }

    //----------------------------------------------------------------------
    bool Clock::tick( Seconds delta )
    {
        m_lastTime = m_curTime.value();
        m_curTime += (delta * m_tickModifier);

        // Check whether we finished one cycle
        bool cycle = ticksBackwards() ? (m_curTime.value() > m_lastTime) : (m_curTime.value() < m_lastTime);
        if (not isLooping() && cycle)
        {
            // Reset time back to max or min
            m_curTime.setValue( ticksBackwards() ? m_curTime.getLowerBound() : m_curTime.getUpperBound() );
            return false;
        }

        _CheckCallbacks();
        return true;
    }

    //----------------------------------------------------------------------
    Seconds Clock::getDelta() const 
    { 
        if (ticksBackwards())
        {
            if (m_curTime.value() > m_lastTime)
                return m_lastTime - m_curTime.value() - (Time::Milliseconds)getDuration();
            else
                return m_lastTime - m_curTime.value();
        }
        else
        {
            if (m_curTime.value() < m_lastTime)
                return m_curTime.value() + (Time::Milliseconds)getDuration() - m_lastTime;
            else
                return m_curTime.value() - m_lastTime;
        }
    }

    //----------------------------------------------------------------------
    CallbackID Clock::attachCallback( const std::function<void()>& func, Milliseconds ms, ECallFrequency freq )
    {
        AttachedCallback newFunc;
        newFunc.callback   = func;
        newFunc.id         = NextID();
        newFunc.freq       = freq;
        newFunc.time       = m_curTime.getValueInRange( ms );

        m_attachedCallbacks.push_back( newFunc );

        return newFunc.id;
    }

    //----------------------------------------------------------------------
    void Clock::clearCallback(CallbackID id)
    {
        if (id == INVALID_CALLBACK_ID)
            return;

        for (auto it = m_attachedCallbacks.begin(); it != m_attachedCallbacks.end(); it++)
        {
            if (it->id == id)
            {
                m_attachedCallbacks.erase( it );
                return;
            }
        }

        LOG_WARN( "Clock::clearCallback(): Given CallbackID #" + TS( id ) + " does not exist." );
    }

    //----------------------------------------------------------------------
    void Clock::_CheckCallbacks()
    {
        for (auto it = m_attachedCallbacks.begin(); it != m_attachedCallbacks.end(); )
        {
            // Reverse min and max if the clock ticks backwards
            Milliseconds min = ticksBackwards() ? m_curTime.value() : m_lastTime;
            Milliseconds max = ticksBackwards() ? m_lastTime        : m_curTime.value();

            // Check if the current tick passed the time of the callback. If so, call it.
            if ( isBetweenCircular( min, it->time, max ) )
            {
                it->callback();
                switch ( it->freq )
                {
                    case ECallFrequency::ONCE:
                    {
                        it = m_attachedCallbacks.erase( it );
                        continue;
                    }
                    case ECallFrequency::REPEAT:
                        // do nothing
                        break;
                }
            }
            it++;
        }
    }


} // end namespaces