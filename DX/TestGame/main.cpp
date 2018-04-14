#include <DX.h>
#define DISPLAY_CONSOLE 1


//**********************************************************************
class MyScene : public IScene
{
    GameObject*             go;
    Components::Camera*     cam;

    MeshPtr         mesh;
    AudioClipPtr    clip;

public:
    MyScene() : IScene("MyScene"){}

    void init() override
    {
        go = createGameObject("Camera");
        cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0,0,-10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 10.0f, 0.3f);
        go->addComponent<Components::AudioListener>();

        clip = ASSETS.getAudioClip( "/audio/doki.wav" );

        auto go2 = createGameObject("Sound");
        auto as = go2->addComponent<Components::AudioSource>(clip, true);

        go2->addComponent<Components::MeshRenderer>(Core::Assets::MeshGenerator::CreateCube(0.5f, Color::BLUE));

        auto go3 = createGameObject("Sound");
        go3->getComponent<Components::Transform>()->position = Math::Vec3(50, 0, 0);
        //go3->addComponent<Components::AudioSource>(ASSETS.getAudioClip("/audio/test.wav"));
        go3->addComponent<Components::MeshRenderer>(Core::Assets::MeshGenerator::CreateCube(0.5f, Color::RED));

        mesh = Core::Assets::MeshGenerator::CreateGrid(100.0f);
        createGameObject("Grid")->addComponent<Components::MeshRenderer>(mesh);
    }

    void tick(Time::Seconds delta) override
    {
        if (KEYBOARD.wasKeyPressed(Key::F))
        {
            static bool play = true;
            play ? clip->play() : clip->pause();
            play = !play;
        }

        if (KEYBOARD.wasKeyPressed(Key::H))
            DEBUG.drawSphere({}, 10.0f, Color::GREEN, 5000);
        if (KEYBOARD.wasKeyPressed(Key::G))
            DEBUG.drawSphere({}, 20.0f, Color::RED, 5000);

        if (KEYBOARD.wasKeyPressed(Key::E))
            clip->play();

        if (KEYBOARD.wasKeyPressed(Key::R))
            clip->pause();

        if (KEYBOARD.wasKeyPressed(Key::T))
            clip->resume();

        if (KEYBOARD.wasKeyPressed(Key::Z))
            clip->stop();

        static F32 pitch = 1.0f;
        static F32 volume = 1.0f;
        static F32 speed = 0.5f;
        if (KEYBOARD.isKeyDown(Key::Up))
            volume += 2 * speed * delta.value;
        if (KEYBOARD.isKeyDown(Key::Down))
            volume -= 2 * speed * delta.value;
        if (KEYBOARD.isKeyDown(Key::Left))
            pitch -= speed * delta.value;
        if (KEYBOARD.isKeyDown(Key::Right))
            pitch += speed * delta.value;

        if (clip)
        {
            clip->setBasePitch(pitch);
            clip->setVolume(volume);
        }
    }

    void shutdown() override
    {
    }
};

class Game : public IGame
{
public:
    //----------------------------------------------------------------------
    void init() override 
    {
        LOG( "Init game..." );
        gLogger->setSaveToDisk( false );

        // Print time and delta every 1000ms
        Locator::getEngineClock().setInterval([] {
           LOG( "Time: " + TS( Locator::getEngineClock().getTime().value ) + " FPS: " + TS( Locator::getProfiler().getFPS() ) );
        }, 1000);

        getWindow().setCursor( "../dx/res/internal/cursors/Areo Cursor Red.cur" );
        getWindow().setIcon( "../dx/res/internal/icon.ico" );
        Locator::getSceneManager().LoadSceneAsync( new MyScene() );
    }

    //----------------------------------------------------------------------
    void tick(Time::Seconds delta) override
    {
    }

    //----------------------------------------------------------------------
    void shutdown() override 
    {
        LOG( "Shutdown game..." );
    }
};

#ifdef _DEBUG
    const char* gameName = "[DEBUG] Test Game";
#else
    const char* gameName = "[RELEASE] Test Game";
#endif

#if DISPLAY_CONSOLE

    int main()
    {
        Game game;
        game.start( gameName, 800, 600 );

        //system("pause");
        return 0;
    }

#else

    #undef ERROR
    #include <Windows.h>

    int APIENTRY WinMain(HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPTSTR    lpCmdLine,
        int       nCmdShow)
    {
        Game game;
        game.start( gameName, 800, 600 );

        return 0;
    }

#endif