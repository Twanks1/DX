#pragma once
/**********************************************************************
    class: EventDispatcher (event_dispatcher.h)

    author: S. Hau
    date: April 2, 2018
**********************************************************************/

#include "event.h"
#include "event_names.hpp"

namespace Events {

    //**********************************************************************
    class EventDispatcher {
    public:
        //----------------------------------------------------------------------
        // Get an existing event. Warning if the event does not exist.
        // @Params:
        //  "eventName": The name of the event.
        // @Return:
        //  The event, possibly created with this call.
        //----------------------------------------------------------------------
        static Event& GetEvent(StringID eventName);

        //----------------------------------------------------------------------
        // Clears all existing events.
        //----------------------------------------------------------------------
        static void Clear();

    private:
        static HashMap<StringID, Event> m_eventMap;

        NULL_COPY_AND_ASSIGN(EventDispatcher)
    };

}