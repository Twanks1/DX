#include "OS/PlatformTimer/platform_timer.h"
#include "Memory/Allocators/include.hpp"
#include "Core/subsystem_manager.h"

#include "OS/FileSystem/file_system.h"
#include "OS/Threading/thread_pool.h"
#include "OS/FileSystem/file.h"
#include "i_game.hpp"
#include "Time/clock.h"

#include "locator.h"
#include "Common/color.h"
#include "GameplayLayer/Components/fps_camera.h"

#include "Assets/MeshGenerator/mesh_generator.h"
#include "Math/random.h"

using namespace Core;

class AutoClock
{
    I64 begin;

public:
    AutoClock()
    {
        begin = OS::PlatformTimer::getTicks();
    }

    ~AutoClock()
    {
        I64 elapsedTicks = OS::PlatformTimer::getTicks() - begin;

        F64 elapsedSeconds = OS::PlatformTimer::ticksToSeconds(elapsedTicks);
        LOG("Seconds: " + TS(elapsedSeconds));

        F64 elapsedMillis = OS::PlatformTimer::ticksToMilliSeconds(elapsedTicks);
        LOG("Millis: " + TS(elapsedMillis));

        F64 elapsedMicros = OS::PlatformTimer::ticksToMicroSeconds(elapsedTicks);
        LOG("Micros: " + TS(elapsedMicros));

        F64 elapsedNanos = OS::PlatformTimer::ticksToNanoSeconds(elapsedTicks);
        LOG("Nanos: " + TS(elapsedNanos));
    }
};

class MyScene2 : public IScene
{
    GameObject* go;
    GameObject* go2;

    Components::Camera* cam;

public:
    MyScene2() : IScene("MyScene2") {}

    void init() override
    {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
        go = createGameObject("Camera");
        cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 10.0f, 0.3f);
        cam->setCameraMode(Components::Camera::ORTHOGRAPHIC);
        F32 size = 5.0f;
        cam->setOrthoParams(-size, size, -size, size, 0.1f, 100.0f);

        go2 = createGameObject("Box");
        go2->addComponent<Components::MeshRenderer>();

        //auto& viewport = cam->getViewport();
        //viewport.width  = 0.5f;
        //viewport.height = 0.5f;

        {
            //go2 = createGameObject("Camera2");
            //auto cam2 = go2->addComponent<Components::Camera>();
            //go2->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -5);
            //cam2->setClearMode(Components::Camera::EClearMode::NONE);

            //auto& viewport2 = cam2->getViewport();
            //viewport2.topLeftX = 0.5f;
            //viewport2.topLeftY = 0;
            //viewport2.width = 0.5f;
            //viewport2.height = 0.5f;
        }

        GameObject* go2 = findGameObject("Test");
        if (go2 != nullptr)
            LOG("Found GameObject!", Color::GREEN);

        //bool removed = go2->removeComponent( c );
        //bool destroyed = go2->removeComponent<Transform>();

        LOG("MyScene2 initialized!", Color::RED);
    }

    void shutdown() override
    {
        LOG("MyScene2 Shutdown!", Color::RED);
    }
};

ArrayList<Color> cubeColors =
{
    Color(0, 0, 0),
    Color(0, 255, 0),
    Color(255, 255, 0),
    Color(255, 0, 0),
    Color(0, 0, 255),
    Color(0, 255, 255),
    Color(255, 255, 255),
    Color(255, 0, 255)
};

ArrayList<Color> planeColors =
{
    Color(0, 0, 0),
    Color(0, 255, 0),
    Color(255, 255, 0),
    Color(255, 0, 0)
};

class ConstantRotation : public Components::IComponent
{
    Math::Vec3 m_speeds = Math::Vec3(0.0f);
    Math::Vec3 m_curDegrees = Math::Vec3(0.0f);
public:
    // Attached gameobject rotates "{degree,degree,degree}" per second around the respective axis.
    ConstantRotation(F32 pitchSpeed, F32 yawSpeed, F32 rollSpeed) 
        : m_speeds(Math::Vec3{ pitchSpeed, yawSpeed, rollSpeed }) {}

    void Tick( Time::Seconds delta ) override
    {
        auto t = getGameObject()->getComponent<Components::Transform>();
        t->rotation = Math::Quat::FromEulerAngles( m_curDegrees );
        m_curDegrees += m_speeds * (F32)delta.value;
    }
};

class WorldGeneration : public Components::IComponent
{
    Graphics::Mesh* mesh;
    Components::MeshRenderer* mr;

public:
    void AddedToGameObject(GameObject* go) override
    {
        mesh = Assets::MeshGenerator::CreatePlane();
        mesh->setColors(planeColors);

        mr = go->addComponent<Components::MeshRenderer>();
        mr->setMesh(mesh);
    }

    void Tick(Time::Seconds delta)
    {
        auto newVertices = mesh->getVertices();
        auto indices = mesh->getIndices();

        int i = 0;
        while (i < newVertices.size())
        {
            F32 newZ = newVertices[i].z = (F32)sin(TIME.getTime().value);
            newVertices[i].z = i % 2 == 0 ? newZ : -newZ;
            i++;
        }

        auto newColors = mesh->getColors();
        for(auto& color : newColors)
            color.setRed( (Byte) ( (sin(TIME.getTime().value) + 1) / 2 * 255 ) );

        mesh->clear();
        mesh->setVertices( newVertices );
        mesh->setIndices( indices );
        mesh->setColors( newColors );
    }
};

class MyScene : public IScene
{
    GameObject* goModel;

public:
    MyScene() : IScene("MyScene"){}

    void init() override
    {
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0,0,-10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 10.0f, 0.3f);

        auto worldGO = createGameObject("World");
        worldGO->getComponent<Components::Transform>()->position = Math::Vec3(0, 3, 0);
        worldGO->addComponent<WorldGeneration>();

        auto box = Assets::MeshGenerator::CreateCube(1.0f, Color::RED);
        box->setColors(cubeColors);

        auto m2 = Assets::MeshGenerator::CreateUVSphere(10,10);
        ArrayList<Color> sphereColors;
        for(U32 i = 0; i < m2->getVertexCount(); i++)
            sphereColors.push_back(Math::Random::Color());
        m2->setColors(sphereColors);
        //m2->setColors(planeColors);

        // Create 3D-Model or load it... How to manage resources?
        // Mesh* m = Assets::loadMesh("/models/test.obj");

        goModel = createGameObject("Test");
        goModel->addComponent<ConstantRotation>(0.0f, 20.0f, 20.0f);
        auto mr = goModel->addComponent<Components::MeshRenderer>(box);

        //auto cam2GO = createGameObject("Camera2");
        //cam2GO->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, 10);
        //cam2GO->getComponent<Components::Transform>()->lookAt(Math::Vec3(0));
        //auto cam2 = cam2GO->addComponent<Components::Camera>();
        //cam2->setClearMode(Components::Camera::EClearMode::NONE);
        //cam2->getViewport().topLeftX = 0.5f;
        //cam2->getViewport().width = 0.5f;

        {
            GameObject* goModel2 = createGameObject("Test");
            goModel2->getComponent<Components::Transform>()->position = {5,0,0};
            goModel2->addComponent<ConstantRotation>(20.0f, 20.0f, 0.0f);
            mr = goModel2->addComponent<Components::MeshRenderer>(box);

            GameObject* goModel3 = createGameObject("Test");
            goModel3->getComponent<Components::Transform>()->position = { -5,0,0 };
            goModel3->addComponent<ConstantRotation>(0.0f, 0.0f, 20.0f);
            mr = goModel3->addComponent<Components::MeshRenderer>(m2);
        }

        LOG("MyScene initialized!", Color::RED);
    }

    void tick( Time::Seconds d ) override
    {
        static F32 speed = 50.0f;
        F32 delta = (F32)d.value;
        if (KEYBOARD.isKeyDown(Key::Add))
        {
            //cam->setFOV(cam->getFOV() + speed * (F32)delta.value);
            goModel->getComponent<Components::Transform>()->scale.y += speed * delta;
        }
        if (KEYBOARD.isKeyDown(Key::Subtract))
        {
            //cam->setFOV(cam->getFOV() - speed * delta);
            goModel->getComponent<Components::Transform>()->scale.y -= speed * delta;
        }
    }

    void shutdown() override
    {
        LOG("MyScene Shutdown!", Color::RED);
    }

};


class Game : public IGame
{
    const F64 duration = 1000;
    Time::Clock clock;

public:
    Game() : clock( duration ) {}

    //----------------------------------------------------------------------
    void init() override 
    {
        LOG( "Init game..." );
        getWindow().setCursor( "/cursors/Areo Cursor Red.cur" );
        getWindow().setIcon( "/internal/icon.ico" );
        gLogger->setSaveToDisk( false );

        Locator::getEngineClock().setInterval([=] {
            //terminate();
            U32 fps = PROFILER.getFPS();
            F64 delta = (1000.0 / fps);
            LOG("Time: " + TS( TIME.getTime().value ) + " FPS: " + TS( fps ) + " Delta: " + TS( delta ) + " ms" );
            //LOG("Num Scenes: " + TS(Locator::getSceneManager().numScenes()));
        }, 1000);

        Locator::getSceneManager().LoadSceneAsync(new MyScene);
    }

    //----------------------------------------------------------------------
    void tick(Time::Seconds delta) override
    {
        getWindow().setTitle( PROFILER.getUpdateDelta().toString().c_str() );
        static U64 ticks = 0;
        ticks++;

        clock.tick( delta );
        //LOG( TS( clock.getTime().value ) );
        auto time = clock.getTime();

        static bool inState = false;
        if (KEYBOARD.wasKeyPressed(Key::Q)) {
            inState = !inState;
        }
        if (inState){
        }

        if (KEYBOARD.wasKeyPressed(Key::E))
        {
            auto g = SCENE.findGameObject("Test");
            static bool enabled = true;
            enabled = !enabled;
            g->setActive(enabled);
            LOG(TS(enabled), Color::BLUE);
        }

        if (KEYBOARD.wasKeyPressed(Key::R))
            auto go = SCENE.createGameObject("LOL");

        if (KEYBOARD.wasKeyPressed(Key::One))
            Locator::getSceneManager().LoadSceneAsync(new MyScene);
        if (KEYBOARD.wasKeyPressed(Key::Two))
            Locator::getSceneManager().LoadSceneAsync(new MyScene2);

        if (KEYBOARD.wasKeyPressed(Key::M))
            _ChangeMultiSampling();

        if( KEYBOARD.isKeyDown( Key::Escape ) )
            terminate();
    }

    //----------------------------------------------------------------------
    void _ChangeMultiSampling()
    {
        static int index = 0;
        int mscounts[]{ 1,4,8 };
        int newmscount = mscounts[index];
        index = (index + 1) % (sizeof(mscounts) / sizeof(int));
        Locator::getRenderer().setMultiSampleCount(newmscount);
        LOG("New Multisample-Count: " + TS(newmscount), Color::GREEN);
    }

    //----------------------------------------------------------------------
    void shutdown() override 
    {
        LOG( "Shutdown game..." );
    }
};




int main()
{
    Game game;
    game.start( "Awesome Game!", 800, 600 );

    system("pause");
    return 0;
}



