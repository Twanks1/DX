#pragma once
/**********************************************************************
    class: IGame (i_game.h)

    author: S. Hau
    date: October 27, 2017

    Inherit from this class and call start()
    to begin initializing the whole engine and run the core loop.

    Override init(), tick() and shutdown() for each game class.
**********************************************************************/

#include "Core/core_engine.h"
#include "i_scene.h"
#include "gameobject.h"
#include "Components/transform.h"
#include "Components/Rendering/mesh_renderer.h"
#include "Components/Rendering/camera.h"
#include "Components/fps_camera.h"
#include "Components/skybox.h"
#include "Components/audio_listener.h"
#include "Components/audio_source.h"
#include "Components/Rendering/directional_light.h"
#include "Components/Rendering/point_light.h"
#include "Components/Rendering/spot_light.h"
#include "Components/billboard.h"
#include "Components/Rendering/gui.h"
#include "Components/Rendering/particle_system.h"
#include "Components/Rendering/vr_camera.h"
#include "Components/Rendering/skinned_mesh_renderer.h"

class IGame : public Core::CoreEngine
{
public:
    IGame() = default;
    virtual ~IGame() = default;

    //----------------------------------------------------------------------
    // Will be called after every subsystem has been initialized.
    //----------------------------------------------------------------------
    virtual void init() = 0;

    //----------------------------------------------------------------------
    // Tick will be called a fixed amount of times per second. (Default is 60x)
    //----------------------------------------------------------------------
    virtual void tick(Time::Seconds delta) = 0;

    //----------------------------------------------------------------------
    // Called after the main game loop has been terminated.
    //----------------------------------------------------------------------
    virtual void shutdown() = 0;

private:
    NULL_COPY_AND_ASSIGN(IGame)
};
