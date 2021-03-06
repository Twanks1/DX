#include "core_engine.h"
/**********************************************************************
    class: CoreEngine (core_engine.cpp)

    author: S. Hau
    date: October 27, 2017

    @Considerations:
      - Tick physics subsystem in a different rate
**********************************************************************/

#include "Core/locator.h"
#include "Logging/logging.h"
#include "Events/event_dispatcher.h"
#include "render_system.h"

namespace Core {

    //----------------------------------------------------------------------
    void CoreEngine::start( const char* title, U32 width, U32 height, Graphics::API api )
    {
        m_api = api;

        while (m_restart)
        {
            m_restart = false;
            m_frameCounter = 0;
            _Init( title, width, height, m_api );
            _RunCoreGameLoop();
            _Shutdown();
        }
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void CoreEngine::subscribe( ISubSystem* subSystem, bool insertFront )
    { 
        if (insertFront)
            m_subscribers.insert( m_subscribers.begin(), subSystem );
        else
            m_subscribers.push_back( subSystem ); 
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void CoreEngine::_Init( const char* title, U32 width, U32 height, Graphics::API api )
    {
        // Check for Math-Library support
        if ( not DirectX::XMVerifyCPUSupport() )
            LOG_ERROR( "DirectX-Math not supported on this system, but is required!" );

        // Set core engine instance in the locator class
        Locator::setCoreEngine( this );

        // Create Window & Attach window resize event
        m_window.create( title, width, height );
        m_window.setCallbackSizeChanged([](U16 w, U16 h) { Events::EventDispatcher::GetEvent(EVENT_WINDOW_RESIZE).invoke(); });

        // Provide engine clock and window to the locator class
        Locator::provide( &m_engineClock );
        Locator::provide( &m_window );

        // Initialize all subsystems
        m_subSystemManager.init( api );

        F32 ambient = 0.2f;
        if ( auto amb = CONFIG.getEngineIni()["General"]["Ambient"] )
            ambient = amb;
        Locator::getRenderer().setGlobalFloat( SID("_Ambient"), ambient );

        // Invoke game start event
        Events::EventDispatcher::GetEvent( EVENT_GAME_START ).invoke();

        // Quit app when HMD requests it
        static Events::EventListener hmdShouldQuitListener;
        hmdShouldQuitListener = Events::EventDispatcher::GetEvent( EVENT_HMD_SHOULD_QUIT ).addListener([this]{ terminate(); });

        // Call virtual init function for game class
        init();
    }

    //----------------------------------------------------------------------
    void CoreEngine::_RunCoreGameLoop()
    {
        const Time::Seconds TICK_RATE_IN_SECONDS  = (1.0f / GAME_TICK_RATE);
        const           U8  MAX_TICKS_PER_FRAME   = 5; // Prevents the "spiral of death" if the cpu is too slow

        Time::Seconds gameTickAccumulator = 0;

        m_isRunning = true;
        while ( m_isRunning && not m_window.shouldBeClosed() )
        {
            Time::Seconds delta = m_engineClock._Update();
            if (delta > 0.5f) delta = 0.5f;

            switch (m_gameLoopTechnique)
            {
            case EGameLoopTechnique::Fixed:
            {
                _NotifyOnUpdate( delta );

                // Tick game in fixed intervals
                U8 ticksPerFrame = 0;
                gameTickAccumulator += delta;
                while ( (gameTickAccumulator >= TICK_RATE_IN_SECONDS) && (ticksPerFrame++ != MAX_TICKS_PER_FRAME))
                {
                    _NotifyOnTick( TICK_RATE_IN_SECONDS );

                    tick( TICK_RATE_IN_SECONDS );
                    gameTickAccumulator -= TICK_RATE_IN_SECONDS;
                }
                _Render();
            }
            break;
            case EGameLoopTechnique::Variable:
                _NotifyOnUpdate( delta );
                _NotifyOnTick( delta );
                tick( delta );
                _Render();
                break;
            }

            m_window.processOSMessages();
        }
    }

    //----------------------------------------------------------------------
    void CoreEngine::_Render()
    {
        auto& graphicsEngine = Locator::getRenderer();

        // Update global buffer
        static StringID TIME_NAME = SID( "_Time" );
        graphicsEngine.setGlobalFloat( TIME_NAME, (F32)TIME.getTime() );

        Events::EventDispatcher::GetEvent( EVENT_FRAME_BEGIN ).invoke();

        RenderSystem::Instance().execute();

        Events::EventDispatcher::GetEvent( EVENT_FRAME_END ).invoke();

        // Present backbuffer(s) to screen
        graphicsEngine.present();

        m_frameCounter++;
    }

    //----------------------------------------------------------------------
    void CoreEngine::_Shutdown()
    {
        // Invoke game end event
        Events::EventDispatcher::GetEvent( EVENT_GAME_SHUTDOWN ).invoke();

        // Deinitialize game class
        shutdown();

        LOG( " ~ Goodbye! ~ ", Color::GREEN );

        // Deinitialize every subsystem
        m_subSystemManager.shutdown();

        // Clear all subscribers and callbacks attached to the engine clock
        m_subscribers.clear();
        m_engineClock.clearAllCallbacks();

        // Destroy window
        m_window.destroy();
    }

    //----------------------------------------------------------------------
    void CoreEngine::_NotifyOnTick( Time::Seconds delta )
    {
        for (auto& subscriber : m_subscribers)
            subscriber->OnTick( delta );
    }

    //----------------------------------------------------------------------
    void CoreEngine::_NotifyOnUpdate( Time::Seconds delta )
    {
        for (auto& subscriber : m_subscribers)
            subscriber->OnUpdate( delta );
    }

} // end namespaces