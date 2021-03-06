#pragma once

/**********************************************************************
    class: SubSystemManager (subsystem_manager.h)

    author: S. Hau
    date: October 11, 2017

    See below for a class description.

**********************************************************************/

#include "Time/durations.h"
#include "Graphics/enums.hpp"

namespace Graphics { class IRenderer; }
namespace Assets   { class AssetManager; }

namespace Core {

    //----------------------------------------------------------------------
    // Forward Declarations
    //----------------------------------------------------------------------
    namespace MemoryManagement  { class MemoryManager; }
    namespace Config            { class ConfigurationManager; }
    namespace Threading         { class ThreadManager; }
    namespace Profiling         { class Profiler; }
    namespace Input             { class InputManager; }
    namespace Resources         { class ResourceManager; }
    namespace Debug             { class DebugManager; }
    namespace Audio             { class AudioManager; }
    class IInGameConsole;
    class SceneManager;

    //*********************************************************************
    // Manages every Sub-System in the engine.
    // Creates, initializes, deinitializes & destroys them.
    //*********************************************************************
    class SubSystemManager
    {
    public:
        SubSystemManager();

        //----------------------------------------------------------------------
        void init(Graphics::API api);
        void shutdown();

    private:
        //----------------------------------------------------------------------
        // Every Sub-System is enumerated here
        //----------------------------------------------------------------------
        MemoryManagement::MemoryManager*    m_memoryManager     = nullptr;
        Config::ConfigurationManager*       m_configManager     = nullptr;
        Threading::ThreadManager*           m_threadManager     = nullptr;
        Profiling::Profiler*                m_profiler          = nullptr;
        Input::InputManager*                m_inputManager      = nullptr;
        IInGameConsole*                     m_inGameConsole     = nullptr;
        Graphics::IRenderer*                m_renderer          = nullptr;
        SceneManager*                       m_sceneManager      = nullptr;
        Resources::ResourceManager*         m_resourceManager   = nullptr;
        Assets::AssetManager*               m_assetManager      = nullptr;
        Debug::DebugManager*                m_debugManager      = nullptr;
        Audio::AudioManager*                m_audioManager      = nullptr;

        //----------------------------------------------------------------------
        void _InitVirtualFilePaths(Graphics::API api);
        void _ClearVirtualFilePaths();

        //----------------------------------------------------------------------
        template <typename T>
        T* initializeSubSystem(T* system);

        template <typename T>
        void shutdownSubSystem(T*& system);

        NULL_COPY_AND_ASSIGN(SubSystemManager)
    };

    //**********************************************************************
    // IMPLEMENTATION
    //**********************************************************************

    template <typename T>
    T* SubSystemManager::initializeSubSystem( T* system )
    {
        Locator::provide( system );
        system->init();

        return system;
    }

    template <typename T>
    void SubSystemManager::shutdownSubSystem( T*& system )
    {
        system->shutdown();
        Locator::provide( (T*) nullptr );

        delete system;
        system = nullptr;
    }


} // end namespaces