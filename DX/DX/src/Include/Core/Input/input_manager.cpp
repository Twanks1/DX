#include "input_manager.h"
/**********************************************************************
    class: InputManager (input_manager.cpp)

    author: S. Hau
    date: November 18, 2017
**********************************************************************/

#include "Core/locator.h"

namespace Core { namespace Input {

    //----------------------------------------------------------------------
    void InputManager::init()
    {
        // Subscribe for OnTick() event
        Locator::getCoreEngine().subscribe( this );

        // Create Input Devices
        OS::Window& window = Locator::getWindow();
        m_mouse      = new Mouse( &window );
        m_keyboard   = new Keyboard( &window );
        m_controller = new Controller();

        m_actionMapper = new ActionMapper( m_keyboard, m_mouse, m_controller );
        m_axisMapper   = new AxisMapper( m_keyboard, m_mouse, m_controller );

        // Register some default axes
        F64 acceleration = 4.0;
        F64 damping = 5.0;
        m_axisMapper->registerAxis( "Vertical", Key::W, Key::S, acceleration, damping );
        m_axisMapper->registerAxis( "Horizontal", Key::A, Key::D, acceleration, damping );
        m_axisMapper->registerAxis( "Up", Key::Space, Key::Shift, acceleration, damping );
    }

    //----------------------------------------------------------------------
    void InputManager::OnTick( Time::Seconds delta )
    {
        m_keyboard->_UpdateInternalState();
        m_mouse->_UpdateInternalState();
        m_actionMapper->_UpdateInternalState();
        m_axisMapper->_UpdateInternalState( delta.value );
        m_controller->_UpdateInternalState();
    }

    //----------------------------------------------------------------------
    void InputManager::shutdown()
    {
        delete m_axisMapper;
        delete m_actionMapper;
        delete m_keyboard;
        delete m_mouse;
        delete m_controller;
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void InputManager::setChannels( EInputChannels channels )
    {
        m_keyboard->setChannels( channels );
        m_mouse->setChannels( channels );
        m_controller->setChannels( channels );
    }

    //----------------------------------------------------------------------
    void InputManager::restoreLastChannelMask()
    {
        m_keyboard->restoreLastChannelMask();
        m_mouse->restoreLastChannelMask();
        m_controller->restoreLastChannelMask();
    }

} }
