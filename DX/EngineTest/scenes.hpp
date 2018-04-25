#pragma once
#include <DX.h>
#include "components.hpp"

class SceneCameras : public IScene
{
    GameObject* go;
    GameObject* go2;

    Components::Camera* cam;

public:
    SceneCameras() : IScene("SceneCameras") {}

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

        auto sphere = Assets::MeshGenerator::CreateUVSphere(30,30);
        ArrayList<Color> sphereColors;
        for (U32 i = 0; i < sphere->getVertexCount(); i++)
            sphereColors.push_back(Math::Random::Color());
        sphere->setColors(sphereColors);

        go2 = createGameObject("Mesh");
        go2->addComponent<Components::MeshRenderer>(sphere, RESOURCES.getColorMaterial());
        go2->addComponent<ConstantRotation>(0.0f, 20.0f, 0.0f);

        auto& viewport = cam->getViewport();
        viewport.width  = 0.5f;
        viewport.height = 0.5f;

        {
            // CAMERA 2
            auto go3 = createGameObject("Camera2");
            auto cam2 = go3->addComponent<Components::Camera>();
            go3->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -5);
            cam2->setClearMode(Components::Camera::EClearMode::NONE);

            auto& viewport2 = cam2->getViewport();
            viewport2.topLeftX = 0.5f;
            viewport2.width = 0.5f;
            viewport2.height = 0.5f;

            // CAMERA 3
            auto go4 = createGameObject("Camera3");
            auto cam3 = go4->addComponent<Components::Camera>();
            go4->getComponent<Components::Transform>()->position = Math::Vec3(0, 5, 5);
            go4->getComponent<Components::Transform>()->lookAt(Math::Vec3(0));
            cam3->setClearMode(Components::Camera::EClearMode::NONE);

            auto& viewport3 = cam3->getViewport();
            viewport3.topLeftY = 0.5f;
            viewport3.width = 0.5f;
            viewport3.height = 0.5f;

            // CAMERA 4
            auto go5 = createGameObject("Camera4");
            auto cam4 = go5->addComponent<Components::Camera>();
            go5->getComponent<Components::Transform>()->position = Math::Vec3(0, -5, -5);
            go5->getComponent<Components::Transform>()->lookAt(Math::Vec3(0));
            cam4->setClearMode(Components::Camera::EClearMode::NONE);

            auto& viewport4 = cam4->getViewport();
            viewport4.topLeftY = 0.5f;
            viewport4.topLeftX = 0.5f;
            viewport4.width = 0.5f;
            viewport4.height = 0.5f;
        }

        //bool removed = go2->removeComponent( c );
        //bool destroyed = go2->removeComponent<Components::Transform>();

        LOG("SceneCameras initialized!", Color::RED);
    }

    void tick(Time::Seconds delta)
    {
        if(KEYBOARD.wasKeyPressed(Key::T))
            go2->removeComponent<Components::MeshRenderer>();
    }

    void shutdown() override
    {
        LOG("SceneCameras Shutdown!", Color::RED);
    }
};

class VertexGenScene : public IScene
{
    GameObject* goModel;

public:
    VertexGenScene() : IScene("VertexGenScene"){}

    void init() override
    {
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0,10,-25);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 10.0f, 0.3f);
        go->addComponent<AutoOrbiting>(15.0f);

        auto worldGO = createGameObject("World");
        worldGO->getComponent<Components::Transform>()->position = Math::Vec3(0, 3, 0);
        worldGO->addComponent<WorldGeneration>();

        auto wavesGO = createGameObject("Waves");
        wavesGO->addComponent<VertexGeneration>();

        auto cube = Assets::MeshGenerator::CreateCube(1.0f);
        cube->setColors(cubeColors);

        auto plane = Assets::MeshGenerator::CreatePlane();
        plane->setColors(planeColors);

        auto sphere = Assets::MeshGenerator::CreateUVSphere(10,10);
        ArrayList<Color> sphereColors;
        for(U32 i = 0; i < sphere->getVertexCount(); i++)
            sphereColors.push_back(Math::Random::Color());
        sphere->setColors(sphereColors);

        {
            goModel = createGameObject("Test");
            goModel->addComponent<ConstantRotation>(0.0f, 20.0f, 20.0f);
            auto mr = goModel->addComponent<Components::MeshRenderer>(cube, RESOURCES.getColorMaterial());

            GameObject* goModel2 = createGameObject("Test");
            goModel2->getComponent<Components::Transform>()->position = {5,0,0};
            goModel2->addComponent<ConstantRotation>(20.0f, 20.0f, 0.0f);
            mr = goModel2->addComponent<Components::MeshRenderer>(sphere, RESOURCES.getColorMaterial());

            GameObject* goModel3 = createGameObject("Test");
            goModel3->getComponent<Components::Transform>()->position = { -5,0,0 };
            goModel3->addComponent<ConstantRotation>(0.0f, 0.0f, 20.0f);
            mr = goModel3->addComponent<Components::MeshRenderer>(plane, RESOURCES.getColorMaterial());
        }

        {
            //auto cam2GO = createGameObject("Camera2");
            //cam2GO->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, 10);
            //cam2GO->getComponent<Components::Transform>()->lookAt(Math::Vec3(0));
            //auto cam2 = cam2GO->addComponent<Components::Camera>();
            //cam2->setClearMode(Components::Camera::EClearMode::NONE);
            //cam2->getViewport().topLeftX = 0.5f;
            //cam2->getViewport().width = 0.5f;
        }

        LOG("VertexGenScene initialized!", Color::RED);
    }

    void tick( Time::Seconds d ) override
    {
        static F32 speed = 50.0f;
        F32 delta = (F32)d.value;
        if (KEYBOARD.isKeyDown(Key::Add))
            goModel->getComponent<Components::Transform>()->scale.y += speed * delta;
        if (KEYBOARD.isKeyDown(Key::Subtract))
            goModel->getComponent<Components::Transform>()->scale.y -= speed * delta;
    }

    void shutdown() override { LOG("VertexGenScene Shutdown!", Color::RED); }

};

class ManyObjectsScene : public IScene
{
    U32 m_numObjects;

public:
    ManyObjectsScene(U32 numObjects) : IScene("MaterialTestScene"), m_numObjects(numObjects) {}

    void init() override
    {
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 10.0f, 0.3f);

        // MESH
        auto cube = Assets::MeshGenerator::CreateCube(1.0f);
        cube->setColors(cubeColors);

        U32 sq = (U32)sqrt(m_numObjects);

        for (U32 i = 0; i < m_numObjects; i++)
        {
            auto go = createGameObject("Test");
            go->addComponent<Components::MeshRenderer>(cube, RESOURCES.getColorMaterial());
            go->getComponent<Components::Transform>()->position = Math::Random::Vec3(-1,1).normalized() * sqrtf((F32)m_numObjects);
        }

        // GAMEOBJECTs
        //for (U32 i = 0; i < sq; i++)
        //{
        //    for (U32 j = 0; j < sq; j++)
        //    {
        //        auto go = createGameObject("Test");
        //        go->addComponent<Components::MeshRenderer>(cube);
        //        go->getComponent<Components::Transform>()->position = Math::Vec3(i * 3.0f, 0, j * 3.0f);
        //    }
        //}
        LOG("ManyObjectsScene initialized!", Color::RED);
    }

    void shutdown() override { LOG("ManyObjectsScene Shutdown!", Color::RED); }
};

class MaterialTestScene : public IScene
{
    MaterialPtr material;

    Texture2DPtr tex;
    Texture2DPtr tex2;

public:
    MaterialTestScene() : IScene("MaterialTestScene") {}

    void init() override
    {
        // Camera
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 10.0f, 0.3f);
        //go->addComponent<AutoOrbiting>(10.0f);

        auto cubemap = ASSETS.getCubemap("/cubemaps/tropical_sunny_day/Left.png", "/cubemaps/tropical_sunny_day/Right.png",
            "/cubemaps/tropical_sunny_day/Up.png", "/cubemaps/tropical_sunny_day/Down.png",
            "/cubemaps/tropical_sunny_day/Front.png", "/cubemaps/tropical_sunny_day/Back.png");
        go->addComponent<Components::Skybox>(cubemap);

        // Camera 2
        auto renderTex = RESOURCES.createRenderTexture();
        renderTex->create(400, 400, 24, Graphics::TextureFormat::BGRA32);
        auto cam2GO = createGameObject("Camera2");
        cam2GO->getComponent<Components::Transform>()->position = Math::Vec3(0, 3, -10);
        cam2GO->addComponent<AutoOrbiting>(10.0f);

        auto cam2 = cam2GO->addComponent<Components::Camera>();
        cam2->setRenderTarget(renderTex);
        cam2->setClearColor(Color::GREEN);

        auto grid = createGameObject("Grid");
        grid->addComponent<GridGeneration>(20);

        // MESH
        auto cube = Assets::MeshGenerator::CreateCubeUV();
        cube->setColors(cubeColors);

        auto plane = Assets::MeshGenerator::CreatePlane();
        plane->setColors(planeColors);

        // SHADER
        auto texShader = RESOURCES.createShader( "TexShader", "/shaders/texVS.hlsl", "/shaders/texPS.hlsl");

        // TEXTURES
        tex = RESOURCES.createTexture2D(4, 4, Graphics::TextureFormat::RGBA32);
        for (U32 x = 0; x < tex->getWidth(); x++)
            for (U32 y = 0; y < tex->getHeight(); y++)
                tex->setPixel( x, y, Math::Random::Color() );
        tex->apply();
        tex->setFilter(Graphics::TextureFilter::Point);

        tex2 = ASSETS.getTexture2D("/textures/nico.jpg");
        auto dirt = ASSETS.getTexture2D("/textures/dirt.jpg");

        // MATERIAL
        material = RESOURCES.createMaterial();
        material->setShader(texShader);
        material->setTexture( SID("tex0"), tex2);
        material->setTexture( SID("tex1"), dirt );
        material->setFloat( SID("mix"), 0.0f);
        material->setColor( SID("tintColor"), Color::WHITE );

        auto dirtMaterial = RESOURCES.createMaterial();
        dirtMaterial->setShader(texShader);
        dirtMaterial->setTexture(SID("tex0"), dirt);
        dirtMaterial->setColor(SID("tintColor"), Color::WHITE);

        auto customTexMaterial = RESOURCES.createMaterial();
        customTexMaterial->setShader(texShader);
        customTexMaterial->setTexture(SID("tex0"), renderTex);
        customTexMaterial->setFloat(SID("mix"), 0.0f);
        customTexMaterial->setColor(SID("tintColor"), Color::WHITE);

        // GAMEOBJECT
        auto goModel = createGameObject("Test");
        //goModel->addComponent<ConstantRotation>(0.0f, 20.0f, 20.0f);
        auto mr = goModel->addComponent<Components::MeshRenderer>(plane, material);

        auto go2 = createGameObject("Test2");
        go2->addComponent<Components::MeshRenderer>(cube, dirtMaterial);
        go2->getComponent<Components::Transform>()->position = Math::Vec3(3, 0, 0);

        auto go3 = createGameObject("Test3");
        go3->addComponent<Components::MeshRenderer>(plane, customTexMaterial);
        go3->getComponent<Components::Transform>()->position = Math::Vec3(0, 2, 0);

        go->addComponent<Components::MeshRenderer>(cube, dirtMaterial);

        LOG("MaterialTestScene initialized!", Color::RED);
    }

    void tick(Time::Seconds d) override
    {
        if (KEYBOARD.wasKeyPressed(Key::NumPad1))
            RESOURCES.setGlobalAnisotropicFiltering(1);
        if (KEYBOARD.wasKeyPressed(Key::NumPad2))
            RESOURCES.setGlobalAnisotropicFiltering(4);
        if (KEYBOARD.wasKeyPressed(Key::NumPad3))
            RESOURCES.setGlobalAnisotropicFiltering(8);
    }

    void shutdown() override
    {
        LOG("MaterialTestScene Shutdown!", Color::RED);
    }
};

class CubemapScene : public IScene
{
public:
    CubemapScene() : IScene("CubemapScene") {}

    void init() override
    {
        // Camera
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 10.0f, 0.3f);
        //go->addComponent<AutoOrbiting>(10.0f);
        //go->addComponent<Components::Skybox>(cubemap);

        auto grid = createGameObject("Grid");
        grid->addComponent<GridGeneration>(20);

        // MESH
        auto sphere = Assets::MeshGenerator::CreateCubeUV();

        // SHADER
        auto texShader = RESOURCES.createShader("Skybox", "/shaders/skyboxVS.hlsl", "/shaders/skyboxPS.hlsl");
        texShader->setRasterizationState({ Graphics::FillMode::Solid, Graphics::CullMode::None });

        // MATERIAL
        auto material = RESOURCES.createMaterial();
        material->setShader(texShader);
        material->setTexture(SID("Cubemap"), RESOURCES.getDefaultCubemap());
        material->setColor(SID("tintColor"), Color::WHITE);

        // GAMEOBJECT
        auto go2 = createGameObject("Test2");
        go2->addComponent<Components::MeshRenderer>(sphere, material);
        go2->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, 0);

        LOG("CubemapScene initialized!", Color::RED);
    }

    void shutdown() override { LOG("CubemapScene Shutdown!", Color::RED); }
};

class TexArrayScene : public IScene
{
    Texture2DPtr        tex;

public:
    TexArrayScene() : IScene("TexArrayScene") {}

    void init() override
    {
        // Camera
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 10.0f, 0.3f);
        //go->addComponent<Components::Skybox>(cubemap);

        createGameObject("Grid")->addComponent<GridGeneration>(20);

        // MESH
        auto plane = Assets::MeshGenerator::CreatePlane();

        // SHADER
        auto texShader = RESOURCES.createShader("TextureArray", "/shaders/arrayTexVS.hlsl", "/shaders/arrayTexPS.hlsl");

        // TEXTURES
        Color cols[] = { Color::RED, Color::BLUE, Color::GREEN };

        const I32 slices = 3;
        const I32 size = 512;

        auto texArr = RESOURCES.createTexture2DArray(size, size, slices, Graphics::TextureFormat::RGBA32, false);
        for (I32 slice = 0; slice < slices; slice++)
            texArr->setPixels(slice, ArrayList<Color>(size * size, cols[slice]).data());
        texArr->apply();
        texArr->setFilter(Graphics::TextureFilter::Point);

        // MATERIAL
        auto material = RESOURCES.createMaterial();
        material->setShader(texShader);
        material->setTexture("texArray", texArr);
        material->setInt("texIndex", 0);

        auto material2 = RESOURCES.createMaterial();
        material2->setShader(texShader);
        material2->setTexture("texArray", texArr);
        material2->setInt("texIndex", 1);

        auto material3 = RESOURCES.createMaterial();
        material3->setShader(texShader);
        material3->setTexture("texArray", texArr);
        material3->setInt("texIndex", 2);

        // GAMEOBJECT
        auto go2 = createGameObject("Test2");
        go2->addComponent<Components::MeshRenderer>(plane, material);

        auto go3 = createGameObject("Test3");
        go3->addComponent<Components::MeshRenderer>(plane, material2);
        go3->getComponent<Components::Transform>()->position.x = -2;

        auto go4 = createGameObject("Test4");
        go4->addComponent<Components::MeshRenderer>(plane, material3);
        go4->getComponent<Components::Transform>()->position.x = 2;

        LOG("TexArrayScene initialized!", Color::RED);
    }

    void shutdown() override { LOG("TexArrayScene Shutdown!", Color::RED); }
};

class AsyncLoadScene : public IScene
{
    MaterialPtr material;

public:
    AsyncLoadScene() : IScene("AsyncLoadScene") {}

    void init() override
    {
        // Camera
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 10.0f, 0.3f);

        auto go3 = createGameObject("Camera2");
        auto cam2 = go3->addComponent<Components::Camera>();
        go3->getComponent<Components::Transform>()->position = Math::Vec3(0, 5, -10);
        go3->addComponent<AutoOrbiting>(10.0f);

        cam2->setClearMode(Components::Camera::EClearMode::NONE);
        cam2->setZFar(20.0f);
        cam2->getViewport().width = 0.25f;
        cam2->getViewport().height = 0.25f;

        go3->addComponent<DrawFrustum>();

        createGameObject("Grid")->addComponent<GridGeneration>(20);

        // SHADER
        auto texShader = RESOURCES.createShader("TexShader", "/shaders/texVS.hlsl", "/shaders/texPS.hlsl");

        // MATERIAL
        material = RESOURCES.createMaterial();
        material->setShader(texShader);
        material->setTexture(SID("tex0"), ASSETS.getTexture2D("/textures/dirt.jpg"));
        material->setTexture(SID("tex1"), ASSETS.getTexture2D("/textures/nico.jpg"));
        material->setFloat(SID("mix"), 0.0f);
        material->setColor(SID("tintColor"), Color::WHITE);

        // MESH
        auto mesh = Assets::MeshGenerator::CreateCubeUV();
        mesh->setColors(cubeColors);

        // GAMEOBJECT
        auto go2 = createGameObject("Test2");
        go2->addComponent<Components::MeshRenderer>(mesh, material);

        LOG("AsyncLoadScene initialized!", Color::RED);
    }

    void tick(Time::Seconds d) override
    {
        if (KEYBOARD.wasKeyPressed(Key::L))
            ASSETS.getTexture2DAsync("/textures/4k.jpg", true, [=](Texture2DPtr tex) { material->setTexture(SID("tex0"), tex); });
    }

    void shutdown() override { LOG("AsyncLoadScene Shutdown!", Color::RED); }
};

class SceneGraphScene : public IScene
{
    GameObject* parent;
    GameObject* child;
    GameObject* child2;

public:
    SceneGraphScene() : IScene("SceneGraphScene") {}

    void init() override
    {
        // Camera
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 10.0f, 0.3f);

        createGameObject("Grid")->addComponent<GridGeneration>(20);

        // SHADER
        auto texShader = RESOURCES.createShader("TexShader", "/shaders/texVS.hlsl", "/shaders/texPS.hlsl");

        // MATERIAL
        auto material = RESOURCES.createMaterial();
        material->setShader(texShader);
        material->setTexture(SID("tex0"), ASSETS.getTexture2D("/textures/dirt.jpg"));
        material->setTexture(SID("tex1"), ASSETS.getTexture2D("/textures/nico.jpg"));
        material->setFloat(SID("mix"), 0.0f);
        material->setColor(SID("tintColor"), Color::WHITE);

        // MATERIAL
        auto material2 = RESOURCES.createMaterial();
        material2->setShader(texShader);
        material2->setTexture(SID("tex0"), ASSETS.getTexture2D("/textures/dirt.jpg"));
        material2->setTexture(SID("tex1"), ASSETS.getTexture2D("/textures/nico.jpg"));
        material2->setFloat(SID("mix"), 1.0f);
        material2->setColor(SID("tintColor"), Color::WHITE);

        // MESH
        auto mesh = Assets::MeshGenerator::CreateCubeUV();
        mesh->setColors(cubeColors);

        // GAMEOBJECT
        parent = createGameObject("Test2");
        parent->addComponent<Components::MeshRenderer>(mesh, material);
        parent->addComponent<ConstantRotation>(5.0f, 25.0f, 0.0f);

        child = createGameObject("Test3");
        child->addComponent<Components::MeshRenderer>(mesh, material2);
        child->getTransform()->position.x = 5;
        child->getTransform()->position.y = 1;

        child2 = createGameObject("Test4");
        child2->addComponent<Components::MeshRenderer>(mesh, material2);
        child2->getTransform()->position.y = 3;
        child2->getTransform()->position.z = 2;

        parent->getTransform()->addChild(child->getTransform());
        parent->getTransform()->addChild(child2->getTransform());

        LOG("SceneGraphScene initialized!", Color::RED);
    }

    void tick(Time::Seconds d) override
    {
        if (KEYBOARD.wasKeyPressed(Key::F))
            child2->getTransform()->setParent(nullptr);

        if (KEYBOARD.wasKeyPressed(Key::G))
            child2->getTransform()->setParent(parent->getTransform());
    }

    void shutdown() override { LOG("SceneGraphScene Shutdown!", Color::RED); }
};