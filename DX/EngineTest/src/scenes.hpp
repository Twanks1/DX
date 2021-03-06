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
        go = createGameObject("Camera");
        cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA);
        cam->setCameraMode(Graphics::CameraMode::Perspective);
        F32 size = 5.0f;
        cam->setOrthoParams(-size, size, -size, size, 0.1f, 100.0f);

        auto sphere = Core::MeshGenerator::CreateUVSphere(30,30);
        ArrayList<Color> sphereColors;
        for (U32 i = 0; i < sphere->getVertexCount(); i++)
            sphereColors.push_back(Math::Random::Color());
        sphere->setColors(sphereColors);

        go2 = createGameObject("Mesh");
        go2->addComponent<Components::MeshRenderer>(sphere, ASSETS.getColorMaterial());
        go2->addComponent<ConstantRotation>(0.0f, 20.0f, 0.0f);

        auto& viewport = cam->getViewport();
        viewport.width  = 0.5f;
        viewport.height = 0.5f;

        {
            // CAMERA 2
            auto go3 = createGameObject("Camera2");
            auto cam2 = go3->addComponent<Components::Camera>();
            go3->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -5);
            //cam2->setClearMode(Graphics::CameraClearMode::None);

            auto& viewport2 = cam2->getViewport();
            viewport2.topLeftX = 0.5f;
            viewport2.width = 0.5f;
            viewport2.height = 0.5f;

            // CAMERA 3
            auto go4 = createGameObject("Camera3");
            auto cam3 = go4->addComponent<Components::Camera>();
            go4->getComponent<Components::Transform>()->position = Math::Vec3(0, 5, 5);
            go4->getComponent<Components::Transform>()->lookAt(Math::Vec3(0));
            //cam3->setClearMode(Graphics::CameraClearMode::None);

            auto& viewport3 = cam3->getViewport();
            viewport3.topLeftY = 0.5f;
            viewport3.width = 0.5f;
            viewport3.height = 0.5f;

            // CAMERA 4
            auto go5 = createGameObject("Camera4");
            auto cam4 = go5->addComponent<Components::Camera>();
            go5->getComponent<Components::Transform>()->position = Math::Vec3(0, -5, -5);
            go5->getComponent<Components::Transform>()->lookAt(Math::Vec3(0));
            //cam4->setClearMode(Graphics::CameraClearMode::None);

            auto& viewport4 = cam4->getViewport();
            viewport4.topLeftY = 0.5f;
            viewport4.topLeftX = 0.5f;
            viewport4.width = 0.5f;
            viewport4.height = 0.5f;
        }

        //bool removed = go2->removeComponent( c );
        //bool destroyed = go2->removeComponent<Components::Transform>();
    }

    void tick(Time::Seconds delta)
    {
        if(KEYBOARD.wasKeyPressed(Key::T))
            go2->removeComponent<Components::MeshRenderer>();
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
        //go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA);
        go->addComponent<AutoOrbiting>(15.0f);

        auto wavesGO = createGameObject("Waves");
        wavesGO->addComponent<VertexGeneration>();

        auto cube = Core::MeshGenerator::CreateCube(1.0f);
        cube->setColors(cubeColors);

        auto plane = Core::MeshGenerator::CreatePlane();
        plane->setColors(planeColors);

        auto sphere = Core::MeshGenerator::CreateUVSphere(10,10);
        ArrayList<Color> sphereColors;
        for(U32 i = 0; i < sphere->getVertexCount(); i++)
            sphereColors.push_back(Math::Random::Color());
        sphere->setColors(sphereColors);

        {
            goModel = createGameObject("Test");
            goModel->addComponent<ConstantRotation>(0.0f, 20.0f, 20.0f);
            auto mr = goModel->addComponent<Components::MeshRenderer>(cube, ASSETS.getColorMaterial());

            GameObject* goModel2 = createGameObject("Test");
            goModel2->getComponent<Components::Transform>()->position = {5,0,0};
            goModel2->addComponent<ConstantRotation>(20.0f, 20.0f, 0.0f);
            mr = goModel2->addComponent<Components::MeshRenderer>(sphere, ASSETS.getColorMaterial());

            GameObject* goModel3 = createGameObject("Test");
            goModel3->getComponent<Components::Transform>()->position = { -5,0,0 };
            goModel3->addComponent<ConstantRotation>(0.0f, 0.0f, 20.0f);
            mr = goModel3->addComponent<Components::MeshRenderer>(plane, ASSETS.getColorMaterial());
        }
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

};

class ManyObjectsScene : public IScene
{
    U32 m_numObjects;

public:
    ManyObjectsScene(U32 numObjects = 10000) : IScene("MaterialTestScene"), m_numObjects(numObjects) {}

    void init() override
    {
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -500);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA);

        // MESH
        auto cube = Core::MeshGenerator::CreateCube(1.0f);
        cube->setColors(cubeColors);

        F32 sq = sqrtf((F32)m_numObjects);
        for (U32 i = 0; i < m_numObjects; i++)
        {
            auto go = createGameObject("Test");
            go->addComponent<Components::MeshRenderer>(cube, ASSETS.getColorMaterial());
            go->getComponent<Components::Transform>()->position = Math::Random::Vec3(-1,1).normalized() * sq;
        }
    }
};

class SceneRenderToTexture : public IScene
{
public:
    SceneRenderToTexture() : IScene("SceneRenderToTexture") {}

    void init() override
    {
        // Camera
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA);

        auto cubemap = ASSETS.getCubemap("/cubemaps/tropical_sunny_day/Left.png", "/cubemaps/tropical_sunny_day/Right.png",
            "/cubemaps/tropical_sunny_day/Up.png", "/cubemaps/tropical_sunny_day/Down.png",
            "/cubemaps/tropical_sunny_day/Front.png", "/cubemaps/tropical_sunny_day/Back.png");
        go->addComponent<Components::Skybox>(cubemap);

        // Camera 2
        auto renderTex = RESOURCES.createRenderTexture(1024, 720, Graphics::TextureFormat::D32, Graphics::TextureFormat::BGRA32, 2, Graphics::MSAASamples::Four);
        auto cam2GO = createGameObject("Camera2");
        cam2GO->getComponent<Components::Transform>()->position = Math::Vec3(0, 3, -10);
        cam2GO->getTransform()->lookAt({});
        //cam2GO->addComponent<AutoOrbiting>(10.0f);

        auto cam2 = cam2GO->addComponent<Components::Camera>();
        cam2->setRenderTarget(renderTex);
        cam2->setClearColor(Color::GREEN);
        cam2GO->addComponent<PostProcess<>>(ASSETS.getMaterial("/materials/post processing/color_grading.material"));

        auto grid = createGameObject("Grid");
        grid->addComponent<GridGeneration>(20);

        // MATERIAL
        auto customTexMaterial = RESOURCES.createMaterial(ASSETS.getShader("/shaders/tex.shader"));
        customTexMaterial->setTexture("tex", renderTex);
        customTexMaterial->setColor("tintColor", Color::WHITE);

        // GAMEOBJECT
        auto go3 = createGameObject("Test3");
        go3->addComponent<Components::MeshRenderer>(Core::MeshGenerator::CreatePlane(), customTexMaterial);
        go3->getTransform()->position = Math::Vec3(0, 1.5f, 0);
        go3->getTransform()->scale = { 3 };

        auto player = createGameObject("Player");
        player->addComponent<Components::MeshRenderer>(ASSETS.getMesh("/models/monkey.obj"), ASSETS.getMaterial("/materials/normals.material"));
        player->getTransform()->setParent(go->getTransform(), false);
        player->getTransform()->position = { 0, 0, -0.5f };
        player->getTransform()->rotation *= Math::Quat(Math::Vec3::UP, 180.0f);
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
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA);

        auto grid = createGameObject("Grid");
        grid->addComponent<GridGeneration>(20);

        // MESH
        auto sphere = Core::MeshGenerator::CreateCubeUV(1000.0f);

        // SHADER
        auto texShader = ASSETS.getShader( "/engine/shaders/skybox.shader" );

        // MATERIAL
        auto material = RESOURCES.createMaterial();
        material->setShader(texShader);
        material->setTexture(SID("Cubemap"), ASSETS.getDefaultCubemap());

        // GAMEOBJECT
        auto go2 = createGameObject("Test2");
        go2->addComponent<Components::MeshRenderer>(sphere, material);
        go2->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, 0);
    }
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
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA);

        createGameObject("Grid")->addComponent<GridGeneration>(20);

        // MESH
        auto plane = Core::MeshGenerator::CreatePlane();

        // SHADER
        auto texShader = ASSETS.getShader( "/shaders/textureArray.shader" );

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
    }
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
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA);

        createGameObject("Grid")->addComponent<GridGeneration>(20);

        // SHADER
        auto texShader = ASSETS.getShader("/shaders/tex.shader");

        // MATERIAL
        auto material = RESOURCES.createMaterial();
        material->setShader(texShader);
        material->setTexture("tex", ASSETS.getTexture2D("/textures/dirt.jpg"));
        material->setColor("tintColor", Color::WHITE);

        auto material2 = RESOURCES.createMaterial();
        material2->setShader(texShader);
        material2->setTexture("tex", ASSETS.getTexture2D("/textures/nico.jpg"));
        material2->setColor("tintColor", Color::WHITE);

        // MESH
        auto mesh = Core::MeshGenerator::CreateCubeUV();
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
    }

    void tick(Time::Seconds d) override
    {
        if (KEYBOARD.wasKeyPressed(Key::F))
            child2->getTransform()->setParent(nullptr);

        if (KEYBOARD.wasKeyPressed(Key::G))
            child2->getTransform()->setParent(parent->getTransform());
    }
};


class SceneFrustumVisualization : public IScene
{
public:
    SceneFrustumVisualization() : IScene("SceneFrustumVisualization") {}

    void init() override
    {
        // Camera
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 10.0f, 0.3f, 1.0f);
        go->addComponent<DrawFrustum>();

        auto go3 = createGameObject("Camera2");
        auto rt1 = RESOURCES.createRenderTexture(WINDOW.getWidth(), WINDOW.getHeight(),
                                                 Graphics::TextureFormat::D32, Graphics::TextureFormat::RGBA32,
                                                 Graphics::MSAASamples::Four, true);
        rt1->setDynamicScreenScale(true, 0.25f);
        auto cam2 = go3->addComponent<Components::Camera>(rt1);
        go3->getComponent<Components::Transform>()->position = Math::Vec3(0, 5, -10);
        go3->addComponent<AutoOrbiting>(10.0f);
        cam2->getViewport().width = 0.25f / 1.33f;
        cam2->getViewport().height = 0.25f;
        cam2->setOrthoParams(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 20.0f);
        cam2->setCullingMask(~(LayerMask)Layer::Two); // Do not draw the frustum from it self (causes an artifact at the edge)
        go3->addComponent<DrawFrustum>();
        go3->setLayerMask((LayerMask)Layer::Two);

        createGameObject("Grid")->addComponent<GridGeneration>(20);

        auto mesh = Core::MeshGenerator::CreateCube(1);
        mesh->setColors(cubeColors);

        auto go2 = createGameObject("Obj");
        go2->addComponent<Components::MeshRenderer>(mesh, ASSETS.getColorMaterial());
    }
};


class TransparencyScene : public IScene
{
public:
    TransparencyScene() : IScene("TransparencyScene") {}

    void init() override
    {
        // Camera
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA);

        createGameObject("Grid")->addComponent<GridGeneration>(20);

        auto plane = Core::MeshGenerator::CreatePlane();
        auto mat = ASSETS.getMaterial("/materials/transparent.material");

        auto go2 = createGameObject("Obj");
        go2->addComponent<Components::MeshRenderer>(plane, mat);

        auto go3 = createGameObject("Obj");
        go3->addComponent<Components::MeshRenderer>(plane, mat);
        go3->getTransform()->position.z = 3;

        auto go4 = createGameObject("Obj");
        go4->addComponent<Components::MeshRenderer>(plane, mat);
        go4->getTransform()->position.z = -3;
    }
};

class BlinnPhongLightingScene : public IScene
{
    Components::SpotLight* spot;

public:
    BlinnPhongLightingScene() : IScene("BlinnPhongLightingScene") {}

    void init() override
    {
        // Camera
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA);

        spot = go->addComponent<Components::SpotLight>(2.0f, Color::RED, 25.0f, 20.0f, false);

        createGameObject("Grid")->addComponent<GridGeneration>(20);

        auto mesh = Core::MeshGenerator::CreatePlane(1.0f);

        auto mat = ASSETS.getMaterial("/materials/phong.material");
        auto go2 = createGameObject("Obj");
        go2->addComponent<Components::MeshRenderer>(mesh, mat);
        go2->addComponent<VisualizeNormals>(0.1f, Color::WHITE);
        go2->getTransform()->rotation *= Math::Quat(Math::Vec3::RIGHT, 90);
        go2->getTransform()->scale = { 10,10,10 };

        I32 loop = 2;
        F32 distance = 3.0f;
        for (I32 x = -loop; x <= loop; x++)
        {
            for (I32 y = -loop; y <= loop; y++)
            {
                for (I32 z = -loop; z <= loop; z++)
                {
                    auto gameobject = createGameObject("Obj");
                    gameobject->addComponent<Components::MeshRenderer>(mesh, mat);
                    gameobject->getTransform()->position = Math::Vec3(x * distance, y * distance + 0.01f, z * distance);
                    gameobject->getTransform()->rotation *= Math::Quat(Math::Vec3::RIGHT, 90);
                }
            }
        }

        I32 loop2 = 1;
        F32 distance2 = 3.0f;
        for (I32 x = -loop2; x <= loop2; x++)
        {
            for (I32 z = -loop2; z <= loop2; z++)
            {
                auto gameobject = createGameObject("Obj");
                gameobject->addComponent<Components::PointLight>(2.0f, Math::Random::Color());
                gameobject->getTransform()->position = Math::Vec3(x * distance2, 1.0f, z * distance2);
                gameobject->addComponent<Components::Billboard>(ASSETS.getTexture2D("/engine/textures/pointLight.png"), 0.5f);
            }
        }

        //auto sun = createGameObject("Sun");
        //sun->addComponent<Components::DirectionalLight>(1.0f, Color::WHITE);
        //sun->getTransform()->rotation = Math::Quat::LookRotation(Math::Vec3{ 0,-1, 1 });

        auto pl = createGameObject("PointLight");
        pl->addComponent<Components::PointLight>(2.0f, Color::GREEN);
        pl->getTransform()->position = { 3, 1, 0 };
        pl->addComponent<Components::Billboard>(ASSETS.getTexture2D("/engine/textures/pointLight.png"), 0.5f);
        pl->addComponent<AutoOrbiting>(20.0f);
    }

    void tick(Time::Seconds d) override
    {
        if (KEYBOARD.isKeyDown(Key::Up))
        {
            spot->setAngle(spot->getAngle() + 10.0f * (F32)d);
            LOG(TS(spot->getAngle()));
        }
        if (KEYBOARD.isKeyDown(Key::Down))
        {
            spot->setAngle(spot->getAngle() - 10.0f * (F32)d);
            LOG(TS(spot->getAngle()));
        }

        if (KEYBOARD.isKeyDown(Key::Left))
        {
            spot->setRange(spot->getRange() - 10.0f * (F32)d);
            LOG(TS(spot->getRange()));
        }
        if (KEYBOARD.isKeyDown(Key::Right))
        {
            spot->setRange(spot->getRange() + 10.0f * (F32)d);
            LOG(TS(spot->getRange()));
        }

        if (KEYBOARD.wasKeyPressed(Key::F))
            spot->setActive(!spot->isActive());
    }
};


class BRDFLUTScene : public IScene
{
public:
    BRDFLUTScene() : IScene("BRDFLUTScene") {}

    void init() override
    {
        // Camera
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>(45.0f, 0.1f, 1000.0f, Graphics::MSAASamples::One);
        cam->setClearColor(Color::BLUE);
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA);

        Assets::BRDFLut brdfLut;
        auto planeMat = ASSETS.getMaterial("/materials/texture.material");
        planeMat->setTexture("tex", brdfLut.getTexture());
        auto plane = createGameObject("Plane");
        plane->addComponent<Components::MeshRenderer>(Core::MeshGenerator::CreatePlane(1), planeMat);
    }
};


class ScenePBRSpheres : public IScene
{
public:
    ScenePBRSpheres() : IScene("PBRSpheres") {}

    void init() override
    {
        // Camera
        auto go = createGameObject("Camera");
        go->getTransform()->position = { 0, 2, -10 };
        auto cam = go->addComponent<Components::Camera>(45.0f, 0.1f, 1000.0f, Graphics::MSAASamples::Four, true);
        go->addComponent<Components::FPSCamera>();
        go->addComponent<Tonemap>();

        createGameObject("Grid")->addComponent<GridGeneration>(20);

        auto cubemapHDR = ASSETS.getCubemap("/cubemaps/pine.hdr", 2048, true);

        auto pbrShader = ASSETS.getShader("/engine/shaders/pbr/pbr.shader");

        Assets::EnvironmentMap envMap(cubemapHDR, 128, 512);
        auto diffuse = envMap.getDiffuseIrradianceMap();
        auto specular = envMap.getSpecularReflectionMap();

        auto brdfLut = Assets::BRDFLut().getTexture();
        pbrShader->setReloadCallback([=](Graphics::IShader* shader) {
            shader->setTexture("diffuseIrradianceMap", diffuse);
            shader->setTexture("specularReflectionMap", specular);
            shader->setTexture("brdfLUT", brdfLut);
            shader->setFloat("maxReflectionLOD", F32(specular->getMipCount() - 1));
        });

        auto mesh = ASSETS.getMesh("/models/sphere.obj");
        auto mat = ASSETS.getMaterial("/materials/pbr/test.pbrmaterial");

        auto go2 = createGameObject("Obj");
        go2->addComponent<Components::MeshRenderer>(mesh, mat);
        go2->getTransform()->rotation *= Math::Quat(Math::Vec3::RIGHT, 90);
        go2->getTransform()->scale = { 1.0f };
        go2->getTransform()->position = { 0, 0, -3 };
        go2->addComponent<Components::Skybox>(cubemapHDR);

        I32 num = 7;
        F32 distance = 3.0f;
        for (I32 x = 0; x < num; x++)
        {
            F32 roughness = x / (F32)(num - 1);

            for (I32 y = 0; y < 2; y++)
            {
                auto gameobject = createGameObject("Obj");

                auto material = RESOURCES.createMaterial(pbrShader);
                material->setTexture("albedoMap", ASSETS.getWhiteTexture());
                material->setTexture("normalMap", ASSETS.getNormalTexture());
                material->setColor("color", Color::WHITE);
                material->setFloat("roughness", roughness);
                F32 metallic = (F32)y;
                material->setFloat("metallic", metallic);
                material->setFloat("useRoughnessMap", 0.0f);
                material->setFloat("useMetallicMap", 0.0f);

                gameobject->addComponent<Components::MeshRenderer>(mesh, material);
                gameobject->getTransform()->position = Math::Vec3(x * distance - (num / 2 * distance), y * distance + 0.01f, 0.0f);
            }
        }

        //auto sun = createGameObject("Sun");
        //sun->addComponent<Components::DirectionalLight>(1.0f, Color::WHITE);
        //sun->getTransform()->rotation = Math::Quat::LookRotation(Math::Vec3{ 0,-1, 1 });

        auto pl = createGameObject("PointLight");
        pl->addComponent<Components::PointLight>(3.0f, Color::WHITE);
        pl->getTransform()->position = { 5, 2, 0 };
        pl->addComponent<Components::Billboard>(ASSETS.getTexture2D("/engine/textures/pointLight.png"), 0.5f);
        pl->addComponent<AutoOrbiting>(20.0f);

        F32 intensity = 5.0f;
        F32 range = 30.0f;
        //auto pl2 = createGameObject("PointLight");
        //pl2->addComponent<Components::PointLight>(intensity, Math::Random::Color(), range);
        //pl2->getTransform()->position = { -5, 3, -3 };
        //pl2->addComponent<Components::Billboard>(ASSETS.getTexture2D("/engine/textures/pointLight.png"), 0.5f);

        //auto pl3 = createGameObject("PointLight");
        //pl3->addComponent<Components::PointLight>(intensity, Math::Random::Color(), range);
        //pl3->getTransform()->position = { 5, 3, -3 };
        //pl3->addComponent<Components::Billboard>(ASSETS.getTexture2D("/engine/textures/pointLight.png"), 0.5f);

        //auto pl4 = createGameObject("PointLight");
        //pl4->addComponent<Components::PointLight>(intensity, Math::Random::Color(), range);
        //pl4->getTransform()->position = { -5, -3, -3 };
        //pl4->addComponent<Components::Billboard>(ASSETS.getTexture2D("/engine/textures/pointLight.png"), 0.5f);

        //auto pl5 = createGameObject("PointLight");
        //pl5->addComponent<Components::PointLight>(intensity, Math::Random::Color(), range);
        //pl5->getTransform()->position = { 5, -3, -3 };
        //pl5->addComponent<Components::Billboard>(ASSETS.getTexture2D("/engine/textures/pointLight.png"), 0.3f);
    }
};

class ScenePBRPistol : public IScene
{
    Tonemap* toneMapComponent;

public:
    ScenePBRPistol() : IScene("ScenePBRPistol") {}

    void init() override
    {
        // Camera
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>(45.0f, 0.1f, 1000.0f, Graphics::MSAASamples::Four, true);
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA);
        toneMapComponent = go->addComponent<Tonemap>();

        // Environment map
        auto cubemapHDR = ASSETS.getCubemap("/cubemaps/canyon.hdr", 2048, true);
        auto pbrShader = ASSETS.getShader("/engine/shaders/pbr/pbr.shader");

        Assets::EnvironmentMap envMap(cubemapHDR, 128, 512);
        auto diffuse = envMap.getDiffuseIrradianceMap();
        auto specular = envMap.getSpecularReflectionMap();

        auto brdfLut = Assets::BRDFLut().getTexture();
        pbrShader->setReloadCallback([=](Graphics::IShader* shader) {
            shader->setTexture("diffuseIrradianceMap", diffuse);
            shader->setTexture("specularReflectionMap", specular);
            shader->setTexture("brdfLUT", brdfLut);
            shader->setFloat("maxReflectionLOD", F32(specular->getMipCount() - 1));
        });

        // Skybox
        createGameObject("Skybox")->addComponent<Components::Skybox>(cubemapHDR);

        // Gameobjects
        auto pistolMesh = ASSETS.getMesh("/models/pistol.fbx");
        auto pistol = createGameObject("Pistol");
        pistol->addComponent<Components::MeshRenderer>(pistolMesh, ASSETS.getMaterial("/materials/pbr/pistol.pbrmaterial"));
        pistol->getTransform()->scale = { 0.1f };
        pistol->getTransform()->rotation *= Math::Quat(Math::Vec3::RIGHT, -90.0f);
        pistol->getTransform()->rotation *= Math::Quat(Math::Vec3::UP, -90.0f);
        pistol->getTransform()->position = { 5, 0, 0 };

        auto daggerMesh = ASSETS.getMesh("/models/dagger.obj");
        auto dagger = createGameObject("Dagger");
        dagger->addComponent<Components::MeshRenderer>(daggerMesh, ASSETS.getMaterial("/materials/pbr/dagger.pbrmaterial"));
        dagger->getTransform()->scale = { 0.1f };
        dagger->getTransform()->rotation *= Math::Quat(Math::Vec3::FORWARD, -90.0f);
        dagger->getTransform()->position = { 0, 4, 0 };

        // LIGHTS
        auto sun = createGameObject("Sun");
        sun->addComponent<Components::DirectionalLight>(5.0f, Color::WHITE);
        sun->getTransform()->rotation = Math::Quat::LookRotation(Math::Vec3{ 0,-1, 1 });
    }

    void tick(Time::Seconds d)
    {
        if (KEYBOARD.wasKeyPressed(Key::G))
            toneMapComponent->setActive( not toneMapComponent->isActive() );
    }
};

ArrayList<MaterialPtr> GeneratePBRMaterials(const ShaderPtr& pbrShader, const MeshPtr& mesh, const Assets::MeshMaterialInfo& materials)
{
    ASSERT(materials.isValid());
    const F32 DEFAULT_ROUGHNESS = 1.0f;

    ArrayList<MaterialPtr> pbrMaterials;
    for ( I32 i = 0; i < mesh->getSubMeshCount(); i++ )
    {
        auto pbrMat = RESOURCES.createMaterial( pbrShader );
        pbrMat->setColor( "color", Color::WHITE );
        pbrMat->setTexture( "normalMap", ASSETS.getNormalTexture() );
        pbrMat->setFloat( "useRoughnessMap", 0.0f );
        pbrMat->setFloat( "useMetallicMap", 0.0f );
        pbrMat->setTexture( "roughnessMap", ASSETS.getBlackTexture() );
        pbrMat->setTexture( "metallicMap", ASSETS.getBlackTexture() );
        pbrMat->setFloat( "metallic", 0.0f );
        pbrMat->setFloat( "roughness", DEFAULT_ROUGHNESS );
        pbrMat->setTexture( "albedoMap", ASSETS.getWhiteTexture() );

        auto material = materials[i];
        pbrMat->setColor("color", material.diffuseColor);

        for (auto& texture : material.textures)
        {
            switch (texture.type)
            {
            case Assets::MaterialTextureType::Albedo: pbrMat->setTexture("albedoMap", ASSETS.getTexture2D(texture.filePath)); break;
            case Assets::MaterialTextureType::Normal: pbrMat->setTexture("normalMap", ASSETS.getTexture2D(texture.filePath)); break;
            case Assets::MaterialTextureType::Shininess:
            {
                pbrMat->setFloat("useRoughnessMap", 1.0f);
                pbrMat->setTexture("roughnessMap", ASSETS.getTexture2D(texture.filePath)); 
                break;
            }
            case Assets::MaterialTextureType::Specular:
            {
                pbrMat->setFloat("useMetallicMap", 1.0f);
                pbrMat->setTexture("metallicMap", ASSETS.getTexture2D(texture.filePath)); 
                break;
            }
            }
        }
        pbrMaterials.push_back( pbrMat );
    }
    return pbrMaterials;
}

class SponzaScene : public IScene
{
public:
    SponzaScene() : IScene("SponzaScene") {}

    void init() override
    {
        // Camera
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>(45.0f, 0.1f, 1000.0f, Graphics::MSAASamples::Four, true);
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA);
        go->addComponent<Tonemap>();

        auto cubemapHDR = ASSETS.getCubemap("/cubemaps/canyon.hdr", 2048, true);
        auto pbrShader = ASSETS.getShader("/engine/shaders/pbr/pbr.shader");

        Assets::EnvironmentMap envMap(cubemapHDR, 128, 512);
        auto diffuse = envMap.getDiffuseIrradianceMap();
        auto specular = envMap.getSpecularReflectionMap();

        auto brdfLut = Assets::BRDFLut().getTexture();
        pbrShader->setReloadCallback([=](Graphics::IShader* shader) {
            shader->setTexture("diffuseIrradianceMap", diffuse);
            shader->setTexture("specularReflectionMap", specular);
            shader->setTexture("brdfLUT", brdfLut);
            shader->setFloat("maxReflectionLOD", F32(specular->getMipCount() - 1));
        });

        createGameObject("Skybox")->addComponent<Components::Skybox>(cubemapHDR);

        Assets::MeshMaterialInfo materialImportInfo;
        auto mesh = ASSETS.getMesh( "/models/sponza/sponza.obj", &materialImportInfo );

        auto obj = createGameObject("Obj");
        auto mr = obj->addComponent<Components::MeshRenderer>(mesh, nullptr);
        //obj->addComponent<VisualizeNormals>(1.0f, Color::BLUE);
        //obj->addComponent<VisualizeTangents>(1.0f, Color::RED);
        obj->getTransform()->scale = { 0.05f };

        if ( materialImportInfo.isValid() )
        {
            auto pbrMaterials = GeneratePBRMaterials( pbrShader, mesh, materialImportInfo );
            for ( I32 i = 0; i < pbrMaterials.size(); i++ )
                mr->setMaterial( pbrMaterials[i], i );
        }

        // LIGHTS
        //auto sun = createGameObject("Sun");
        //sun->addComponent<Components::DirectionalLight>(5.0f, Color::WHITE);
        //sun->getTransform()->rotation = Math::Quat::LookRotation(Math::Vec3{ 0,-1, 1 });

        auto pl = createGameObject("PointLight");
        pl->addComponent<Components::PointLight>(15.0f, Color::WHITE, 30.0f);
        pl->getTransform()->position = { 5, 2, 0 };
        pl->addComponent<Components::Billboard>(ASSETS.getTexture2D("/engine/textures/pointLight.png"), 0.5f);
        pl->addComponent<AutoOrbiting>(20.0f);
    }
};

class ScenePostProcessMultiCamera : public IScene
{
    Components::Camera* cam;
public:
    ScenePostProcessMultiCamera() : IScene("ScenePostProcessMultiCamera") {}

    void init() override
    {
        U32 bufferWidth = WINDOW.getWidth() / 2;
        U32 bufferHeight = WINDOW.getHeight() / 2;

        // Camera 1
        auto go = createGameObject("Camera");
        auto rt1 = RESOURCES.createRenderTexture( bufferWidth, bufferHeight,
                                                  Graphics::TextureFormat::D32, Graphics::TextureFormat::RGBAFloat,
                                                  Graphics::MSAASamples::Four, true );
        cam = go->addComponent<Components::Camera>(rt1);
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA);
        cam->getViewport().width = 0.5f;
        cam->getViewport().height = 0.5f;
        go->addComponent<Tonemap>();

        // Camera 2
        auto go2 = createGameObject("Camera2");
        auto rt2 = RESOURCES.createRenderTexture( bufferWidth, bufferHeight,
                                                  Graphics::TextureFormat::D32, Graphics::TextureFormat::RGBAFloat,
                                                  Graphics::MSAASamples::One, true );
        auto cam2 = go2->addComponent<Components::Camera>(rt2);
        cam2->getViewport().width = 0.5f;
        cam2->getViewport().height = 0.5f;
        cam2->getViewport().topLeftX = 0.5f;
        go2->addComponent<Tonemap>();
        go2->addComponent<Fog>();
        go2->getTransform()->setParent(go->getTransform(), false);

        // Camera 3
        auto go3 = createGameObject("Camera3");
        auto rt3 = RESOURCES.createRenderTexture( bufferWidth, bufferHeight,
                                                  Graphics::TextureFormat::D32, Graphics::TextureFormat::RGBAFloat,
                                                  Graphics::MSAASamples::One, true );
        auto cam3 = go3->addComponent<Components::Camera>(rt3);
        cam3->getViewport().width = 0.5f;
        cam3->getViewport().height = 0.5f;
        cam3->getViewport().topLeftY = 0.5f;
        go3->addComponent<Tonemap>();
        go3->addComponent<PostProcess<void>>(ASSETS.getMaterial("/materials/post processing/color_grading.material"));
        go3->getTransform()->setParent(go->getTransform(), false);

        // Camera 4
        auto go4 = createGameObject("Camera4");
        auto rt4 = RESOURCES.createRenderTexture( bufferWidth, bufferHeight,
                                                  Graphics::TextureFormat::D32, Graphics::TextureFormat::RGBAFloat,
                                                  Graphics::MSAASamples::One, true );
        auto cam4 = go4->addComponent<Components::Camera>(rt4);
        cam4->getViewport().width = 0.5f;
        cam4->getViewport().height = 0.5f;
        cam4->getViewport().topLeftY = 0.5f;
        cam4->getViewport().topLeftX = 0.5f;
        go4->addComponent<Tonemap>();
        go4->addComponent<GaussianBlur>();
        go4->getTransform()->setParent(go->getTransform(), false);

        createGameObject("Grid")->addComponent<GridGeneration>(20);

        // PBR
        auto cubemapHDR = ASSETS.getCubemap("/cubemaps/pine.hdr", 2048, true);
        Assets::EnvironmentMap envMap(cubemapHDR, 128, 512);
        auto diffuse = envMap.getDiffuseIrradianceMap();
        auto specular = envMap.getSpecularReflectionMap();

        auto brdfLut = Assets::BRDFLut().getTexture();
        auto pbrShader = ASSETS.getShader("/engine/shaders/pbr/pbr.shader");
        pbrShader->setReloadCallback([=](Graphics::IShader* shader) {
            shader->setTexture("diffuseIrradianceMap", diffuse);
            shader->setTexture("specularReflectionMap", specular);
            shader->setTexture("brdfLUT", brdfLut);
            shader->setFloat("maxReflectionLOD", F32(specular->getMipCount() - 1));
        });

        auto obj = createGameObject("Obj");
        obj->addComponent<Components::MeshRenderer>(ASSETS.getMesh("/models/monkey.obj"), ASSETS.getMaterial("/materials/pbr/gold.pbrmaterial"));
        obj->addComponent<Components::Skybox>(cubemapHDR);
    }
};

class SceneGUI : public IScene
{
public:
    SceneGUI() : IScene("SceneGUI") {}

    void init() override
    {
        // Camera 1
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 3, -8);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 0.1f);

        createGameObject("Grid")->addComponent<GridGeneration>(20);

        go->addComponent<Components::GUI>();
        //go->addComponent<Components::GUIImage>(ASSETS.getTexture2D("/textures/nico.jpg"));
        go->addComponent<Components::GUIDemoWindow>();

        static F32 color[4] = {};
        go->addComponent<Components::GUICustom>([=] {
            ImGui::Begin("Color is cool!");
            if (ImGui::ColorPicker4("Clear color", color))
                cam->setClearColor(Color(color, true));
            ImGui::End();
        });

        //auto go2 = createGameObject("Camera2");
        //auto cam2 = go2->addComponent<Components::Camera>();
        //auto renderTex = RESOURCES.createRenderTexture(1024, 720, Graphics::DepthFormat::None, Graphics::TextureFormat::BGRA32, 2, Graphics::MSAASamples::One);
        //cam2->setRenderTarget(renderTex);
        //go2->addComponent<Components::GUI>();
        //go2->addComponent<Components::GUIDemoWindow>();

        //auto rtGO = createGameObject("GUIScreen");
        //auto guiScreenMat = RESOURCES.createMaterial(ASSETS.getShader("/shaders/tex.shader"));
        //guiScreenMat->setTexture("tex", cam2->getRenderTarget()->getColorBuffer());
        //guiScreenMat->setColor("tintColor", Color::WHITE);
        //rtGO->addComponent<Components::MeshRenderer>(Core::MeshGenerator::CreatePlane(), guiScreenMat);
        //rtGO->getTransform()->position = { 0, 1, 0 };
    }
};

class ShadowScene : public IScene
{
public:
    ShadowScene() : IScene("ShadowScene") {}

    void init() override
    {
        // Camera 1
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 10, -25);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 0.1f);
        cam->setClearColor(Color(66, 134, 244));
        //go->addComponent<AutoOrbiting>(15.0f);

        auto player = createGameObject("Player");
        player->addComponent<Components::MeshRenderer>(ASSETS.getMesh("/models/monkey.obj"), ASSETS.getMaterial("/materials/normals.material"));
        player->getTransform()->setParent(go->getTransform(), false);
        player->getTransform()->position = { 0, 0, -0.5f };
        player->getTransform()->rotation *= Math::Quat(Math::Vec3::UP, 180.0f);

        //auto obj = createGameObject("GO");
        //obj->addComponent<Components::MeshRenderer>(Core::MeshGenerator::CreatePlane(), ASSETS.getMaterial("/materials/blinn_phong/grass.material"));
        //obj->getTransform()->rotation *= Math::Quat(Math::Vec3::RIGHT, 90.0f);
        //obj->getTransform()->scale = { 20,20,20 };
        auto terrainGO = createGameObject("Terrain");
        terrainGO->addComponent<Components::MeshRenderer>(ASSETS.getMesh("/models/terrain.obj"), ASSETS.getMaterial("/materials/blinn_phong/terrain.material"));

        auto obj2 = createGameObject("GO2");
        obj2->addComponent<Components::MeshRenderer>(ASSETS.getMesh("/models/monkey.obj"), ASSETS.getMaterial("/materials/blinn_phong/monkey.material"));
        obj2->getTransform()->position = { 5, 1, 0 };

        auto cubeGO = createGameObject("GO3");
        cubeGO->addComponent<Components::MeshRenderer>(Core::MeshGenerator::CreateCubeUV(0.3f), ASSETS.getMaterial("/materials/blinn_phong/cube.material"));
        cubeGO->getTransform()->position = { -5.0f, 0.3001f, 0.0f };
        cubeGO->addComponent<ConstantRotation>(0.0f, 10.0f, 0.0f);

        auto cubeGO2 = createGameObject("GO3");
        cubeGO2->addComponent<Components::MeshRenderer>(Core::MeshGenerator::CreateCubeUV(0.3f), ASSETS.getMaterial("/materials/blinn_phong/cube.material"));
        cubeGO2->getTransform()->position = { -8.0f, 0.3001f, 0.0f };

        Assets::MeshMaterialInfo matInfo;
        auto treeMesh = ASSETS.getMesh("/models/tree/tree.obj", &matInfo);

        auto treeGO = createGameObject("Tree");
        auto mr = treeGO->addComponent<Components::MeshRenderer>(treeMesh);

        if (matInfo.isValid())
        {
            for (I32 i = 0; i < treeMesh->getSubMeshCount(); i++)
            {
                auto material = RESOURCES.createMaterial(ASSETS.getShader("/shaders/phong_shadow.shader"));
                material->setFloat("uvScale", 1.0f);

                for (auto& texture : matInfo[i].textures)
                {
                    switch (texture.type)
                    {
                    case Assets::MaterialTextureType::Albedo: material->setTexture("_MainTex", ASSETS.getTexture2D(texture.filePath)); break;
                    case Assets::MaterialTextureType::Normal: material->setTexture("normalMap", ASSETS.getTexture2D(texture.filePath)); break;
                    case Assets::MaterialTextureType::Shininess: break;
                    case Assets::MaterialTextureType::Specular: break;
                    }
                }
                material->setReplacementShader(TAG_SHADOW_PASS, ASSETS.getShadowMapShaderAlpha());
                material->setReplacementShader(TAG_SHADOW_PASS_LINEAR, ASSETS.getShadowMapShaderLinearAlpha());
                mr->setMaterial(material, i);
            }
        }

        // LIGHTS
        auto sun = createGameObject("Sun");
        auto dl = sun->addComponent<Components::DirectionalLight>(0.3f, Color::WHITE, Graphics::ShadowType::CSMSoft, ArrayList<F32>{10.0f, 30.0f, 80.0f, 200.0f});
        //auto dl = sun->addComponent<Components::DirectionalLight>(0.0f, Color::WHITE);
        sun->getTransform()->rotation = Math::Quat::LookRotation(Math::Vec3{ 0,-1, 1 });
        //sun->addComponent<ConstantRotation>(5.0f, 0.0f, 0.0f);

        //auto sun2 = createGameObject("Sun2");
        //sun2->addComponent<Components::DirectionalLight>(0.3f, Color::WHITE);
        //sun2->getTransform()->rotation = Math::Quat::LookRotation(Math::Vec3{ 0,-1, -1 });

        auto plg = createGameObject("PL");
        auto pl = plg->addComponent<Components::PointLight>(1.0f, Color::ORANGE, 5.0f, true);
        plg->getTransform()->position = { 3, 2, 0 };
        plg->addComponent<Components::Billboard>(ASSETS.getTexture2D("/engine/textures/pointLight.png"), 0.5f);
        go->addComponent<Components::Skybox>(pl->getShadowMap());

        auto slg = createGameObject("PL");
        auto sl = slg->addComponent<Components::SpotLight>(1.0f, Color::WHITE, 25.0f, 20.0f);
        slg->getTransform()->position = { -5, 2, -2 };
        slg->getTransform()->rotation = Math::Quat::LookRotation(Math::Vec3{ 0,-1, 1 });
        slg->addComponent<Components::Billboard>(ASSETS.getTexture2D("/engine/textures/spotLight.png"), 0.5f);
        //slg->addComponent<DrawFrustum>();

        go->addComponent<Components::GUI>();
        go->addComponent<Components::GUIFPS>();
        go->addComponent<Components::GUICustom>([=] {
            ImGui::Begin("Shadow Demo", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

            static F32 ambient = 0.4f;
            ImGui::SliderFloat("Ambient", &ambient, 0.0f, 1.0f);
            Locator::getRenderer().setGlobalFloat(SID("_Ambient"), ambient);

            obj2->getTransform()->position.x = 5.0f + std::sinf((F32)TIME.getTime());

            if (ImGui::CollapsingHeader("Shadows"))
            {
                CString type[] = { "None", "Hard", "Soft" };
                static I32 type_current = 2;
                if (ImGui::Combo("Type", &type_current, type, 3))
                    CONFIG.setShadowType(Graphics::ShadowType(type_current));

                CString qualities[] = { "Low", "Medium", "High", "Insane" };
                static I32 quality_current = 2;
                if (ImGui::Combo("Quality", &quality_current, qualities, 4))
                    CONFIG.setShadowMapQuality(Graphics::ShadowMapQuality(quality_current));
            }

            if (ImGui::CollapsingHeader("Lights"))
            {
                if (ImGui::TreeNode("Directional Light"))
                {
                    static F32 animateSpeed = 0.0f;
                    ImGui::SliderFloat("Speed", &animateSpeed, -20.0f, 20.0f);
                    dl->getGameObject()->getTransform()->rotation *= Math::Quat(Math::Vec3::RIGHT, animateSpeed * (F32)PROFILER.getDelta());

                    static Math::Vec3 deg{ 45.0f, 0.0f, 0.0f };
                    if (ImGui::SliderFloat2("Rotation", &deg.x, 0.0f, 360.0f))
                        sun->getTransform()->rotation = Math::Quat::FromEulerAngles(deg);

                    static F32 color[4] = { 1,1,1,1 };
                    if (ImGui::ColorEdit4("Color", color))
                        dl->setColor(Color(color));

                    CString type[] = { "None", "Hard", "Soft", "CSM", "CSMSoft" };
                    static I32 type_current = 1;
                    type_current = (I32)dl->getShadowType();
                    if (ImGui::Combo("Shadow Type", &type_current, type, 5))
                        dl->setShadowType((Graphics::ShadowType)(type_current));

                    CString qualities[] = { "Low", "Medium", "High", "Insane" };
                    static I32 quality_current = 2;
                    quality_current = (I32)dl->getShadowMapQuality();
                    if (ImGui::Combo("Quality", &quality_current, qualities, 4))
                        dl->setShadowMapQuality((Graphics::ShadowMapQuality)(quality_current));

                    if (type_current > 0 && type_current <= 2)
                    {
                        static F32 dlRange;
                        dlRange = dl->getShadowRange();
                        if (ImGui::SliderFloat("Shadow Range", &dlRange, 5.0f, 100.0f))
                            dl->setShadowRange(dlRange);
                        ImGui::Image(dl->getShadowMap(), Math::Vec2{ 200, 200 });
                    }
                    else if (type_current > 2)
                    {
                        ImGui::Text("Shadow-maps");
                        for (I32 i = 0; i < dl->getCascadeCount(); ++i)
                            ImGui::Image(dl->getShadowMap(), i, Math::Vec2{ 200, 200 });
                    }
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Point Light"))
                {
                    static Math::Vec3 plPos{ 0, 2.5, -1 };
                    if (ImGui::SliderFloat3("Position", &plPos.x, -3.0f, 3.0f))
                        plg->getTransform()->position = plPos;

                    static F32 plRange;
                    plRange = pl->getRange();
                    if (ImGui::SliderFloat("Range", &plRange, 5.0f, 50.0f))
                        pl->setRange(plRange);

                    static F32 plIntensity = 1.0f;
                    plIntensity = pl->getIntensity();
                    if (ImGui::SliderFloat("Intensity", &plIntensity, 0.1f, 3.0f))
                        pl->setIntensity(plIntensity);

                    CString type[] = { "None", "Hard", "Soft" };
                    static I32 type_current = 1;
                    type_current = (I32)pl->getShadowType();
                    if (ImGui::Combo("Shadow Type", &type_current, type, 3))
                        pl->setShadowType((Graphics::ShadowType)(type_current));

                    CString qualities[] = { "Low", "Medium", "High", "Insane" };
                    static I32 quality_current = 2;
                    quality_current = (I32)pl->getShadowMapQuality();
                    if (ImGui::Combo("Quality", &quality_current, qualities, 4))
                        pl->setShadowMapQuality((Graphics::ShadowMapQuality)(quality_current));
                    static F32 color[4] = { 1,1,1,1 };
                    if (ImGui::ColorEdit4("Color", color))
                        pl->setColor(Color(color));
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Spot Light"))
                {
                    static F32 spotRange;
                    spotRange = sl->getRange();
                    if (ImGui::SliderFloat("Range", &spotRange, 5.0f, 50.0f))
                        sl->setRange(spotRange);

                    static F32 spotIntensity;
                    spotIntensity = sl->getIntensity();
                    if (ImGui::SliderFloat("Intensity", &spotIntensity, 0.1f, 3.0f))
                        sl->setIntensity(spotIntensity);

                    static F32 spotAngle;
                    spotAngle = sl->getAngle();
                    if (ImGui::SliderFloat("Angle", &spotAngle, 5.0f, 90.0f))
                        sl->setAngle(spotAngle);

                    CString type[] = { "None", "Hard", "Soft" };
                    static I32 type_current = 1;
                    type_current = (I32)sl->getShadowType();
                    if (ImGui::Combo("Shadow Type", &type_current, type, 3))
                        sl->setShadowType((Graphics::ShadowType)(type_current));

                    CString qualities[] = { "Low", "Medium", "High", "Insane" };
                    static I32 quality_current = 2;
                    quality_current = (I32)sl->getShadowMapQuality();
                    if (ImGui::Combo("Quality", &quality_current, qualities, 4))
                        sl->setShadowMapQuality((Graphics::ShadowMapQuality)(quality_current));
                    static F32 color[4] = { 1,1,1,1 };
                    if (ImGui::ColorEdit4("Color", color))
                        sl->setColor(Color(color));
                    ImGui::TreePop();
                }
            }
            ImGui::End();
        });
    }
};

class SceneParticleSystem : public IScene
{
    Components::Camera* cam;
    GameObject* go;

public:
    SceneParticleSystem() : IScene("SceneParticleSystem") {}

    void init() override
    {
        // Camera 1
        go = createGameObject("Camera");
        cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 1, -15);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 0.1f);

        createGameObject("Grid")->addComponent<GridGeneration>(20);

        auto psGO = createGameObject("ParticleSystem");
        //psGO->addComponent<ConstantRotation>(0.0f, 15.0f, 0.0f);
        auto ps = psGO->addComponent<Components::ParticleSystem>(ASSETS.getMaterial("/materials/particles/template.material"));
        //ps->setMesh(Core::MeshGenerator::CreateCubeUV());
        ps->setSpawnPositionFunc( Components::ShapeBox{ Math::Vec3{ -1,-1,-1 } * 10, Math::Vec3{ 1,1,1 } * 10 } );
        //ps->setSpawnPositionFunc( Components::ShapeSphere{ Math::Vec3{0}, 10.0f } );

        go->addComponent<Components::GUI>();
        go->addComponent<Components::GUIFPS>();
        go->addComponent<Components::GUICustom>([ps, psGO] () mutable {
            ImGui::Begin("Particle System Demo", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
            {
                { // Load particle file
                    static char buf[256] = { "res/particles/template.ps" };
                    static bool loadFile = false;
                    loadFile = ImGui::InputText("", buf, 256, ImGuiInputTextFlags_EnterReturnsTrue);
                    ImGui::SameLine();
                    loadFile = ImGui::Button("Reload");
                    if (ImGui::Button("Ambient"))
                    {
                        strcpy_s(buf, "res/particles/ambient.ps");
                        loadFile = true;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Snow"))
                    {
                        strcpy_s(buf, "res/particles/snow.ps");
                        loadFile = true;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Rain"))
                    {
                        strcpy_s(buf, "res/particles/rain.ps");
                        loadFile = true;
                    }
                    if (loadFile)
                    {
                        String path = buf;
                        if (path.size() > 1)
                        {
                            psGO->removeComponent<Components::ParticleSystem>();
                            ps = psGO->addComponent<Components::ParticleSystem>(buf);
                        }
                    }
                }

                if (ImGui::CollapsingHeader("Particle System 0", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    static F32 pos[3];
                    ImGui::SliderFloat3("Pos", pos, -10.0f, 10.0f);
                    ps->getGameObject()->getTransform()->position = { pos[0],pos[1],pos[2] };

                    static F32 scale = 1.0f;
                    ImGui::SliderFloat("Scale", &scale, 0.0f, 10.0f);
                    ps->getGameObject()->getTransform()->scale = scale;

                    {
                        if (ImGui::Button("Restart")) ps->play();
                        ImGui::SameLine();
                        if (ImGui::Button("Pause")) ps->pause();
                        ImGui::SameLine();
                        if (ImGui::Button("Resume")) ps->resume();

                        ImGui::Text("Particles: %d", ps->getCurrentParticleCount());
                        static I32 maxParticles;
                        maxParticles = ps->getMaxParticleCount();
                        if (ImGui::SliderInt("Max Particles", &maxParticles, 0, 100000))
                            ps->setMaxParticleCount(maxParticles);

                        static I32 emissionRate;
                        emissionRate = ps->getEmissionRate();
                        if (ImGui::SliderInt("Emission Rate", &emissionRate, 0, 10000))
                            ps->setEmissionRate(emissionRate);
                    }

                    static F32 gravity;
                    gravity = ps->getGravity();
                    if (ImGui::SliderFloat("Gravity", &gravity, -5.0f, 5.0f, "%.2f"))
                        ps->setGravity(gravity);

                    {
                        CString type[] = { "None", "View" };
                        static I32 type_current = 1;
                        type_current = (I32)ps->getParticleAlignment();
                        if (ImGui::Combo("Particle Alignment", &type_current, type, sizeof(type) / sizeof(CString)))
                            ps->setParticleAlignment((Components::ParticleSystem::ParticleAlignment)type_current);
                    }

                    {
                        CString type[] = { "None", "By Distance" };
                        static I32 type_current = 0;
                        type_current = (I32)ps->getSortMode();
                        if (ImGui::Combo("Sort Mode", &type_current, type, sizeof(type) / sizeof(CString)))
                            ps->setSortMode((Components::ParticleSystem::SortMode)type_current);
                    }

                    if (ImGui::TreeNode("Clock"))
                    {
                        ImGui::Text("Clock time: %.2fs", (F32)ps->getClock().getTime());
                        ImGui::SameLine();
                        static bool looping;
                        looping = ps->getClock().isLooping();
                        if (ImGui::Checkbox("Looping", &looping))
                            ps->getClock().setIsLooping(looping);

                        static F32 duration;
                        duration = (F32)ps->getClock().getDuration();
                        if (ImGui::SliderFloat("Duration", &duration, 0.1f, 50.0f, "%.2f"))
                            ps->getClock().setDuration(Time::Seconds(duration));

                        static F32 tickMod = 1.0f;
                        tickMod = ps->getClock().getTickModifier();
                        if (ImGui::SliderFloat("Tick Mod", &tickMod, 0.0f, 5.0f, "%.2f"))
                            ps->getClock().setTickModifier(tickMod);

                        ImGui::TreePop();
                    }

                    if (ImGui::TreeNode("Volume"))
                    {
                        { // Spawn values
                            CString type[] = { "Box", "Sphere" };
                            static I32 type_current = 0;
                            ImGui::Combo("Volume Type", &type_current, type, sizeof(type) / sizeof(CString));

                            switch ((Components::PSShape)type_current)
                            {
                            case Components::PSShape::Box:
                            {
                                static Math::Vec3 min = -10.0f;
                                static Math::Vec3 max = 10.0f;
                                ImGui::SliderFloat3("Min", (float*)&min, -30.0f, 30.0f, "%.2f");
                                ImGui::SliderFloat3("Max", (float*)&max, -30.0f, 30.0f, "%.2f");
                                ps->setSpawnPositionFunc(Components::ShapeBox(min, max));
                                break;
                            }
                            case Components::PSShape::Sphere:
                            {
                                static Math::Vec3 center = 0.0f;
                                static F32 radius = 10.0f;
                                ImGui::SliderFloat3("Center", (float*)&center, -30.0f, 30.0f, "%.2f");
                                ImGui::SliderFloat("Radius", &radius, 0.0f, 30.0f, "%.2f");
                                ps->setSpawnPositionFunc(Components::ShapeSphere(center, radius));
                                break;
                            }
                            }
                        }
                        ImGui::TreePop();
                    }

                    if (ImGui::TreeNode("Lifetime"))
                    {
                        CString type[] = { "Constant", "Random Between Two Constants" };
                        static I32 type_current = 0;
                        ImGui::Combo("Mode", &type_current, type, sizeof(type)/sizeof(CString));

                        switch ((Components::PSValueMode)type_current)
                        {
                        case Components::PSValueMode::Constant:
                        {
                            static F32 lifeTime = 1.0f;
                            ImGui::SliderFloat("Lifetime", &lifeTime, 0.5f, 10.0f, "%.2f");
                            ps->setSpawnLifetimeFnc(Components::Constant(lifeTime));
                            break;
                        }
                        case Components::PSValueMode::RandomBetweenTwoConstants:
                        {
                            static F32 lifeTimeStart = 0.5f;
                            static F32 lifeTimeEnd = 10.0f;
                            if (ImGui::SliderFloat("Min", &lifeTimeStart, 0.5f, 10.0f, "%.2f"))
                                if (lifeTimeStart > lifeTimeEnd) lifeTimeEnd = lifeTimeStart;
                            if (ImGui::SliderFloat("Max", &lifeTimeEnd, 0.5f, 10.0f, "%.2f"))
                                if (lifeTimeEnd < lifeTimeStart) lifeTimeStart = lifeTimeEnd;
                            ps->setSpawnLifetimeFnc(Components::RandomBetweenTwoConstants(lifeTimeStart, lifeTimeEnd));
                            break;
                        }
                        }
                        ImGui::TreePop();
                    }

                    if (ImGui::TreeNode("Size"))
                    {
                        { // Spawn values
                            CString type[] = { "Constant", "Random Between Two Constants" };
                            static I32 type_current = 0;
                            ImGui::Combo("Spawn Mode", &type_current, type, sizeof(type) / sizeof(CString));

                            switch ((Components::PSValueMode)type_current)
                            {
                            case Components::PSValueMode::Constant:
                            {
                                static F32 size = 1.0f;
                                ImGui::SliderFloat("Size", &size, 0.1f, 10.0f, "%.2f");
                                ps->setSpawnScaleFnc(Components::Constant(size));
                                break;
                            }
                            case Components::PSValueMode::RandomBetweenTwoConstants:
                            {
                                static F32 sizeMin = 1.0f;
                                static F32 sizeMax = 1.0f;
                                if (ImGui::SliderFloat("Min", &sizeMin, 0.1f, 10.0f, "%.2f"))
                                    if (sizeMin > sizeMax) sizeMax = sizeMin;
                                if (ImGui::SliderFloat("Max", &sizeMax, 0.1f, 10.0f, "%.2f"))
                                    if (sizeMax < sizeMin) sizeMin = sizeMax;
                                ps->setSpawnScaleFnc(Components::RandomBetweenTwoConstants(sizeMin, sizeMax));
                                break;
                            }
                            }
                        }
                        { // Over lifetime
                            static bool active = false;
                            if (ImGui::Checkbox("Over lifetime", &active))
                                if (!active) ps->setLifetimeScaleFnc(nullptr);

                            if (active)
                            {
                                static F32 start = 1.0f;
                                static F32 mid = 1.0f;
                                static F32 end = 1.0f;
                                ImGui::SliderFloat("Start", &start, 0.1f, 10.0f, "%.2f");
                                ImGui::SliderFloat("Mid", &mid, 0.1f, 10.0f, "%.2f");
                                ImGui::SliderFloat("End", &end, 0.1f, 10.0f, "%.2f");
                                ps->setLifetimeScaleFnc([](F32 lerp) -> F32 {
                                    return lerp < 0.5f ? Math::Lerp(start, mid, lerp * 2) : Math::Lerp(mid, end, (lerp-0.5f) * 2);
                                });
                            }
                        }
                        ImGui::TreePop();
                    }

                    if (ImGui::TreeNode("Rotation"))
                    {
                        { // Spawn values
                            CString type[] = { "Constant", "Random Between Two Constants" };
                            static I32 type_current = 0;
                            ImGui::Combo("Spawn Mode", &type_current, type, sizeof(type) / sizeof(CString));

                            switch ((Components::PSValueMode)type_current)
                            {
                            case Components::PSValueMode::Constant:
                            {
                                static Math::Vec3 rotation = 0.0f;
                                ImGui::SliderFloat3("Rotation", (float*)&rotation, 0.0f, 360.0f, "%.2f");
                                ps->setSpawnRotationFunc(Components::Constant(Math::Quat::FromEulerAngles(rotation)));
                                break;
                            }
                            case Components::PSValueMode::RandomBetweenTwoConstants:
                            {
                                static Math::Vec3 rotationMin = 0.0f;
                                static Math::Vec3 rotationMax = 360.0f;
                                ImGui::SliderFloat3("Min", (float*)&rotationMin, 0.0f, 360.0f, "%.2f");
                                ImGui::SliderFloat3("Max", (float*)&rotationMax, 0.0f, 360.0f, "%.2f");
                                ps->setSpawnRotationFunc([]() { return Math::Quat::FromEulerAngles(Math::Random::Vec3(rotationMin, rotationMax)); });
                                break;
                            }
                            }
                        }
                        { // Over lifetime
                            static bool active = false;
                            if (ImGui::Checkbox("Over lifetime", &active))
                                if (!active) ps->setLifetimeRotationFnc(nullptr);

                            if (active)
                            {
                                static Math::Vec3 rotationStart = 0.0f;
                                static Math::Vec3 rotationMid = 0.0f;
                                static Math::Vec3 rotationEnd = 0.0f;
                                ImGui::SliderFloat3("Start", (float*)&rotationStart, 0.0f, 360.0f, "%.2f");
                                ImGui::SliderFloat3("Mid", (float*)&rotationMid, 0.0f, 360.0f, "%.2f");
                                ImGui::SliderFloat3("End", (float*)&rotationEnd, 0.0f, 360.0f, "%.2f");
                                ps->setLifetimeRotationFnc([](F32 lerp) -> Math::Quat {
                                    auto v = lerp <= 0.5f ? Math::Lerp(rotationStart, rotationMid, lerp*2) : Math::Lerp(rotationMid, rotationEnd, (lerp-0.5f) * 2);
                                    return Math::Quat::FromEulerAngles(v);
                                });
                            }
                        }
                        ImGui::TreePop();
                    }

                    if (ImGui::TreeNode("Color"))
                    {
                        { // Spawn values
                            CString type[] = { "Constant", "Random Between Two Colors" };
                            static I32 type_current = 0;
                            ImGui::Combo("Spawn Mode", &type_current, type, sizeof(type) / sizeof(CString));

                            switch ((Components::PSValueMode)type_current)
                            {
                            case Components::PSValueMode::Constant:
                            {
                                static F32 color[4] = { 1,1,1,1 };
                                ImGui::ColorEdit4("Color", color);
                                ps->setSpawnColorFnc(Components::Constant(Color(color)));
                                break;
                            }
                            case Components::PSValueMode::RandomBetweenTwoConstants:
                            {
                                static F32 colorStart[4] = { 1,1,1,1 };
                                static F32 colorEnd[4] = { 1,1,1,1 };
                                ImGui::ColorEdit4("Min", colorStart);
                                ImGui::ColorEdit4("Max", colorEnd);
                                ps->setSpawnColorFnc(Components::RandomBetweenTwoConstants(Color(colorStart), Color(colorEnd)));
                                break;
                            }
                            }
                        }
                        { // Over lifetime
                            static bool active = false;
                            if (ImGui::Checkbox("Over lifetime", &active))
                                if (!active) ps->setLifetimeColorFnc(nullptr);

                            if (active)
                            {
                                static F32 colorStart[4] = { 1,1,1,1 };
                                static F32 colorMid[4] = { 1,1,1,1 };
                                static F32 colorEnd[4] = { 1,1,1,1 };
                                ImGui::ColorEdit4("Start", colorStart);
                                ImGui::ColorEdit4("Mid", colorMid);
                                ImGui::ColorEdit4("End", colorEnd);
                                ps->setLifetimeColorFnc([](F32 lerp) -> Color {
                                    return lerp <= 0.5f ? Color::Lerp(Color(colorStart), Color(colorMid), lerp * 2) : Color::Lerp(Color(colorMid), Color(colorEnd), (lerp-0.5f)*2);
                                });
                            }
                        }
                        ImGui::TreePop();
                    }

                    if (ImGui::TreeNode("Velocity"))
                    {
                        { // Spawn values
                            CString type[] = { "Constant", "Random Between Two Constants" };
                            static I32 type_current = 0;
                            ImGui::Combo("Spawn Mode", &type_current, type, sizeof(type) / sizeof(CString));

                            switch ((Components::PSValueMode)type_current)
                            {
                            case Components::PSValueMode::Constant:
                            {
                                static Math::Vec3 velocity;
                                ImGui::SliderFloat3("Velocity", (float*)&velocity, -5.0f, 5.0f, "%.2f");
                                ps->setSpawnVelocityFunc(Components::Constant(velocity));
                                break;
                            }
                            case Components::PSValueMode::RandomBetweenTwoConstants:
                            {
                                static Math::Vec3 velocityMin = -1.0f;
                                static Math::Vec3 velocityMax = 1.0f;
                                ImGui::SliderFloat3("Min", (float*)&velocityMin, -5.0f, 5.0f, "%.2f");
                                ImGui::SliderFloat3("Max", (float*)&velocityMax, -5.0f, 5.0f, "%.2f");
                                ps->setSpawnVelocityFunc(Components::ShapeBox(velocityMin, velocityMax));
                                break;
                            }
                            }
                        } 
                        ImGui::TreePop();
                    }
                }
            }
            ImGui::End();
        });
    }
};

class VRScene : public IScene
{
    Components::VRCamera* vrCam = nullptr;
    GameObject* cam2GO = nullptr;

public:
    VRScene() : IScene("TestScene") {}

    void init() override
    {
        // Camera 1
        auto go = createGameObject("Camera");
        go->addComponent<Components::AudioListener>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 1, -1);

        if (RENDERER.hasHMD())
        {
            vrCam = go->addComponent<Components::VRCamera>(Components::ScreenDisplay::LeftEye, Graphics::MSAASamples::Four);
            go->addComponent<Components::VRFPSCamera>();
            //go->addComponent<PostProcess>(ASSETS.getMaterial("/materials/post processing/color_grading.material"));
            go->addComponent<Components::VRBasicTouch>(Core::MeshGenerator::CreateCubeUV(0.1f), ASSETS.getMaterial("/materials/blinn_phong/cube.material"));

            vrCam->getCameraForEye(Graphics::VR::LeftEye).getGameObject()->
            addComponent<Components::MeshRenderer>(Core::MeshGenerator::CreateCubeUV(0.05f), ASSETS.getMaterial("/materials/normals.material"));

            cam2GO = createGameObject("Observer Camera");
            cam2GO->setActive(false);
            cam2GO->addComponent<Components::Camera>();
            cam2GO->getComponent<Components::Transform>()->position = Math::Vec3(5, 4, 0);
            cam2GO->getComponent<Components::Transform>()->lookAt({0});
            cam2GO->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 0.1f);
        }
        else
        {
            LOG_WARN( "VRScene(): VR is disabled or no VR headset found." );
            go->addComponent<Components::Camera>();
            go->getComponent<Components::Transform>()->position = Math::Vec3(0, 1, -3);
            go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 0.1f);
        }

        RENDERER.setGlobalFloat(SID("_Ambient"), 0.15f);

        auto world = createGameObject("World");
        world->addComponent<Components::MeshRenderer>(ASSETS.getMesh("/models/box_n_inside.obj"), ASSETS.getMaterial("/materials/blinn_phong/cellar.material"));
        world->getTransform()->position.y = 10.0f;
        world->getTransform()->scale = 10.0f;

        auto plg = createGameObject("PL");
        plg->addComponent<Components::PointLight>(2.0f, Color::ORANGE, 15.0f);
        plg->getTransform()->position = { 0, 1.5f, 0 };
        plg->addComponent<Components::Billboard>(ASSETS.getTexture2D("/engine/textures/pointLight.png"), 0.5f);

        auto monkey = createGameObject("monkey");
        monkey->addComponent<Components::MeshRenderer>(ASSETS.getMesh("/models/monkey.obj"), ASSETS.getMaterial("/materials/normals.material"));
        auto t = monkey->getTransform();
        t->scale = { 0.2f };
        t->position.y = 0.3f;
        monkey->addComponent<ConstantRotation>(0.0f, 15.0f, 0.0f);
        //monkey->addComponent<Components::AudioSource>(ASSETS.getAudioClip("/audio/start_dash.wav"));

        auto ps = createGameObject("Particles!");
        ps->addComponent<Components::ParticleSystem>("/particles/ambient.ps");

        if (vrCam) // This does not work, because it is not supported by my engine to render into an offscreen FBO and blend that on top of it
        {
            //auto guiCamGO = createGameObject("Camera");
            //auto guiCam = guiCamGO->addComponent<Components::Camera>(45.0f, 0.1f, 1000.0f, Graphics::MSAASamples::One);

            //// Do not render any gameobjects and do not clear the screen.
            ////guiCam->setClearMode(Graphics::CameraClearMode::Color);
            //guiCam->setCullingMask((LayerMask)Layer::None);
            //guiCam->getViewport().width = 0.5f;
            //guiCam->getViewport().height = 0.5f;

            //guiCamGO->addComponent<Components::GUI>();
            //guiCamGO->addComponent<Components::GUICustom>([this]() mutable {
            //    ImGui::Begin("VR Demo", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
            //    { // Spawn values
            //        CString type[] = { "Left Eye", "Right Eye", "Both Eyes" };
            //        static I32 type_current = 0;
            //        ImGui::Combo("Display Mode", &type_current, type, sizeof(type) / sizeof(CString));

            //        switch (type_current)
            //        {
            //        case 0: vrCam->setScreenDisplay(Components::ScreenDisplay::LeftEye); break;
            //        case 1: vrCam->setScreenDisplay(Components::ScreenDisplay::RightEye); break;
            //        case 2: vrCam->setScreenDisplay(Components::ScreenDisplay::BothEyes); break;
            //        }
            //    }
            //    ImGui::End();
            //});
        }

        String info = "<<<< Info >>>>\n"
                      "1: Display Left Eye on Screen\n"
                      "2: Display Right Eye on Screen\n"
                      "3: Display Both Eyes on Screen\n"
                      "4: Display Separate Camera on Screen\n";
        LOG(info, Color::BLUE);
    }

    void tick(Time::Seconds delta) override
    {
        if (vrCam)
        {
            if (KEYBOARD.wasKeyPressed(Key::One))
            {
                vrCam->setScreenDisplay(Components::ScreenDisplay::LeftEye);
                cam2GO->setActive(false);
            }
            if (KEYBOARD.wasKeyPressed(Key::Two))
            {
                vrCam->setScreenDisplay(Components::ScreenDisplay::RightEye);
                cam2GO->setActive(false);
            }
            if (KEYBOARD.wasKeyPressed(Key::Three))
            {
                vrCam->setScreenDisplay(Components::ScreenDisplay::BothEyes);
                cam2GO->setActive(false);
            }
            if (KEYBOARD.wasKeyPressed(Key::Four))
            {
                vrCam->setScreenDisplay(Components::ScreenDisplay::None);
                cam2GO->setActive(true);
            }
        }
    }
};


class SceneSplines : public IScene
{
    ArrayList<Math::Vec3> controlPoints{ {-1, 0, 0 }, { 0, 1, 0 }, { 1, 0, 0 }, { 2, -1, 0 } };
    Math::CatmullRomSpline m_spline;
    MeshPtr m_mesh;

    Components::Transform* m_shipTransform;
    F32 m_shipSpeed = 1.0f;

public:
    SceneSplines() : IScene("SceneSplines"), m_spline(controlPoints, true) {}

    void init() override
    {
        // Camera 1
        auto go = createGameObject("Camera");
        go->addComponent<Components::Camera>(45.0f, 0.1f, 1000.0f, Graphics::MSAASamples::Eight);
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -5);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 0.1f);

        m_mesh = RESOURCES.createMesh();
        m_mesh->setBufferUsage(Graphics::BufferUsage::Frequently);
        createGameObject("Spline")->addComponent<Components::MeshRenderer>(m_mesh, ASSETS.getColorMaterial());

        auto ship = RESOURCES.createMesh();
        ship->setVertices(ArrayList<Math::Vec3>{ { 0, 0.1f, 0 }, { 0, -0.1f, 0 }, { 0.3f, 0, 0 } });
        ship->setIndices(ArrayList<U32>{ 0, 1, 2, 0 }, 0, Graphics::MeshTopology::LineStrip);
        ship->setColors(ArrayList<Color>(ship->getVertexCount(), Color::ORANGE));
        auto shipGO = createGameObject("Ship")->addComponent<Components::MeshRenderer>(ship, ASSETS.getColorMaterial());
        m_shipTransform = shipGO->getGameObject()->getTransform();

        CString message = "<<<< Controls >>>>\n"
            "W/A/S/D: Move selected point.\n"
            "Left/Right: Change selected point.\n"
            "Up/Down: Increase/Decrease vertex count.\n";
        LOG(message, Color::BLUE);
    }

    void tick(Time::Seconds delta) override
    {
        // Create spline vertices and modify mesh
        ArrayList<Math::Vec3> vertices;
        ArrayList<U32> indices;
        U32 index = 0;

        vertices.push_back(m_spline.getPoint(0.0f));

        static F32 steps = 0.05f;
        for (F32 t = steps; t <= controlPoints.size(); t += steps)
        {
            vertices.push_back(m_spline.getPoint(t));
            indices.push_back(index);
            indices.push_back(index + 1);
            index++;
        }
        indices.push_back(index);
        indices.push_back(0);

        m_mesh->setVertices(vertices);
        m_mesh->setIndices(indices, 0, Graphics::MeshTopology::Lines);
        m_mesh->setColors(ArrayList<Color>(vertices.size(), Color::RED));

        // Draw spline points
        static I32 selectedPoint = 0;
        for (I32 i = 0; i < controlPoints.size(); i++)
        {
            if (i == selectedPoint) continue;
            DEBUG.drawSphere(controlPoints[i], 0.05f, Color::GREEN, 0);
        }

        // Draw selected spline point
        DEBUG.drawSphere(controlPoints[selectedPoint], 0.05f, Color::BLUE, 0);

        // Change selected point
        if (KEYBOARD.wasKeyPressed(Key::Right)) selectedPoint = (selectedPoint + 1) % controlPoints.size();
        if (KEYBOARD.wasKeyPressed(Key::Left))  --selectedPoint; if (selectedPoint < 0) selectedPoint += (I32)controlPoints.size();

        // Change step count
        if (KEYBOARD.isKeyDown(Key::Up))    steps = std::max(0.05f, steps - 0.2f * (F32)delta);
        if (KEYBOARD.isKeyDown(Key::Down))  steps += 0.2f * (F32)delta;

        // Print out vertex count if step count has changed
        static F32 prevSteps = 0.0f;
        if (prevSteps != steps)
        {
            LOG( "Vertex count: " + TS(vertices.size()), Color::BLUE );
            prevSteps = steps;
        }

        // Move selected spline point
        F32 strength = 2.0f;
        if (KEYBOARD.isKeyDown(Key::W)) controlPoints[selectedPoint].y += strength * (F32)delta;
        if (KEYBOARD.isKeyDown(Key::A)) controlPoints[selectedPoint].x -= strength * (F32)delta;
        if (KEYBOARD.isKeyDown(Key::S)) controlPoints[selectedPoint].y -= strength * (F32)delta;
        if (KEYBOARD.isKeyDown(Key::D)) controlPoints[selectedPoint].x += strength * (F32)delta;
        m_spline.setControlPoints(controlPoints);

        // Move ship across the spline
        {
            static F32 shipDistanceOnSpline = 0.0f;

            shipDistanceOnSpline += m_shipSpeed * (F32)delta;
            shipDistanceOnSpline = std::fmodf(shipDistanceOnSpline, m_spline.getTotalLength());

            F32 normalizedOffset = m_spline.getNormalisedOffset(shipDistanceOnSpline);

            // Position ship
            auto newShipPos = m_spline.getPoint(normalizedOffset);
            m_shipTransform->position = { newShipPos.x, newShipPos.y, 0 };

            // Rotate ship
            auto gradient = m_spline.getGradient(normalizedOffset);
            F32 roll = Math::Rad2Deg(std::acos(gradient.dot({1,0,0})));
            m_shipTransform->rotation = Math::Quat::FromEulerAngles(0.0f, 0.0f, gradient.y < 0 ? 360-roll : roll);
            //DEBUG.drawLine(m_shipTransform->position - gradient * 0.5f, m_shipTransform->position + gradient*0.5f, Color::GREEN, 0);
        }
    }
};

class AnimationTestScene : public IScene
{
    Components::Camera* cam;

public:
    AnimationTestScene() : IScene("AnimationTestScene") {}

    void init() override
    {
        // Camera
        auto camGO = createGameObject("Camera");
        cam = camGO->addComponent<Components::Camera>(45.0f, 0.1f, 1000.0f, Graphics::MSAASamples::Four);
        cam->setClearColor(Color(175, 181, 191));
        camGO->getComponent<Components::Transform>()->position = Math::Vec3(0, 2, -5);
        camGO->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 0.1f);
        camGO->addComponent<Components::AudioListener>();

        //createGameObject("Grid")->addComponent<GridGeneration>(20);

        auto skelShader = ASSETS.getShader("/shaders/skel_animation.shader");
        auto mat = ASSETS.getMaterial("/models/humanoid/mat.material");

        ArrayList<Animation::AnimationClip> anims;
        Animation::Skeleton skeleton;
        auto mesh = ASSETS.getMesh("/models/humanoid/model.dae", &skeleton, &anims);
        mat->setReplacementShader(TAG_SHADOW_PASS, skelShader);

        auto meshGO = createGameObject("GO");
        auto smr = meshGO->addComponent<Components::SkinnedMeshRenderer>(mesh, skeleton, anims.front(), mat);

        //ArrayList<Animation::AnimationClip> anims;
        //Animation::Skeleton skeleton;
        //Assets::MeshMaterialInfo matInfo;
        //auto mesh = ASSETS.getMesh("/models/mario/mario_galaxy.fbx", &matInfo, &skeleton, &anims);
        //auto meshGO = createGameObject("GO");
        //auto smr = meshGO->addComponent<Components::SkinnedMeshRenderer>(mesh, skeleton, anims.front(), mat);
        //if (matInfo.isValid())
        //{
        //    for (I32 i = 0; i < mesh->getSubMeshCount(); i++)
        //    {
        //        auto material = RESOURCES.createMaterial(ASSETS.getShader("/shaders/skel_animation.shader"));
        //        for (auto& texture : matInfo[i].textures)
        //        {
        //            switch (texture.type)
        //            {
        //            case Assets::MaterialTextureType::Albedo: material->setTexture("tex", ASSETS.getTexture2D(texture.filePath)); break;
        //            case Assets::MaterialTextureType::Normal: material->setTexture("normalMap", ASSETS.getTexture2D(texture.filePath)); break;
        //            }
        //        }
        //        smr->setMaterial(material, i);
        //    }
        //}

        meshGO->getTransform()->rotation *= Math::Quat(Math::Vec3::RIGHT, -90.0f);
        meshGO->getTransform()->rotation *= Math::Quat(Math::Vec3::UP, 180.0f);
        meshGO->getTransform()->scale *= 0.5f;

        camGO->addComponent<Components::GUI>();
        camGO->addComponent<Components::GUICustom>([=] {
            ImGui::Begin("HUD", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
            {
                if (ImGui::CollapsingHeader("Bone Hierarchy", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    for (auto& joint : skeleton.joints)
                    {
                        auto level = Animation::JointLevel(skeleton, joint);
                        String str = "";
                        for (I32 i = 0; i < level; i++)
                            str += " ";
                        if (level != 0)
                            str += "|-";
                        str += joint.name.c_str();
                        ImGui::Text(str.c_str());
                    }
                }

                static bool jointsVisible;
                ImGui::Checkbox("Toggle Joints", &jointsVisible);
                if (jointsVisible)
                {
                    auto worldMatrix = meshGO->getTransform()->getWorldMatrix();
                    auto& jointWorldMatrices = smr->getJointWorldMatrices();
                    for (U32 i = 0; i < skeleton.joints.size(); i++)
                    {
                        auto skinningMatrix = jointWorldMatrices[i] * worldMatrix;
                        DirectX::XMVECTOR s, r, p;
                        DirectX::XMMatrixDecompose(&s, &r, &p, skinningMatrix);

                        Math::Vec3 pos;
                        Math::Quat rot;
                        DirectX::XMStoreFloat3(&pos, p);
                        DirectX::XMStoreFloat4(&rot, r);
                        DEBUG.drawAxes(pos, rot, 0.2f, 0_s, false);
                    }
                }

                static bool skeletonVisible;
                ImGui::Checkbox("Toggle Skeleton", &skeletonVisible);
                if (skeletonVisible)
                {
                    auto worldMatrix = meshGO->getTransform()->getWorldMatrix();
                    auto& jointWorldMatrices = smr->getJointWorldMatrices();
                    for (U32 i = 0; i < skeleton.joints.size(); i++)
                    {
                        if (skeleton.joints[i].parentIndex < 0)
                            continue;

                        Math::Vec3 start, end;
                        DirectX::XMVECTOR s, r, p;

                        auto skinningMatrix = jointWorldMatrices[i] * worldMatrix;
                        DirectX::XMMatrixDecompose(&s, &r, &p, skinningMatrix);
                        DirectX::XMStoreFloat3(&start, p);

                        auto skinningMatrixParent = jointWorldMatrices[skeleton.joints[i].parentIndex] * worldMatrix;
                        DirectX::XMMatrixDecompose(&s, &r, &p, skinningMatrixParent);
                        DirectX::XMStoreFloat3(&end, p);

                        DEBUG.drawLine(start, end, Color::GREEN, 0_s, false);
                    }
                }

                if (ImGui::CollapsingHeader("Animation", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    static bool looping;
                    looping = smr->getClock().isLooping();
                    if (ImGui::Checkbox("Loop", &looping))
                        smr->getClock().setIsLooping(looping);
                    ImGui::SameLine();
                    if (ImGui::Button("Pause"))
                        smr->getClock().setTickModifier(0.0f);
                    ImGui::SameLine();
                    if (ImGui::Button("Resume"))
                        smr->getClock().setTickModifier(1.0f);

                    static F32 time = 1.0f;
                    time = (F32)smr->getClock().getTime();
                    F32 duration = (F32)smr->getClock().getDuration();
                    if (ImGui::SliderFloat("Time", &time, 0.0f, duration, "%.2f"))
                        smr->getClock().setTime(Time::Seconds(time));

                    static F32 tickMod = 1.0f;
                    tickMod = smr->getClock().getTickModifier();
                    if (ImGui::SliderFloat("Speed", &tickMod, -2.0f, 2.0f, "%.2f"))
                        smr->getClock().setTickModifier(tickMod);
                }
            }
            ImGui::End();
        });

        auto obj = createGameObject("GO");
        obj->addComponent<Components::MeshRenderer>(Core::MeshGenerator::CreatePlane(), ASSETS.getMaterial("/materials/blinn_phong/grass.material"));
        obj->getTransform()->rotation *= Math::Quat(Math::Vec3::RIGHT, 90.0f);
        obj->getTransform()->scale = { 20,20,20 };

        auto sun = createGameObject("Sun");
        auto dl = sun->addComponent<Components::DirectionalLight>(0.5f, Color::WHITE, Graphics::ShadowType::CSMSoft, ArrayList<F32>{20.0f, 40.0f, 80.0f, 200.0f});
        sun->getTransform()->rotation = Math::Quat::LookRotation(Math::Vec3{ 0,-1, 1 }, Math::Vec3{ 0, 0, 1 });
        dl->setShadowMapQuality(Graphics::ShadowMapQuality::Insane);

        auto cubemap = ASSETS.getCubemap("/cubemaps/tropical_sunny_day/Left.png", "/cubemaps/tropical_sunny_day/Right.png",
            "/cubemaps/tropical_sunny_day/Up.png", "/cubemaps/tropical_sunny_day/Down.png",
            "/cubemaps/tropical_sunny_day/Front.png", "/cubemaps/tropical_sunny_day/Back.png", true);
        camGO->addComponent<Components::Skybox>(cubemap);
    }

};

class AnimationTestScene2 : public IScene
{
    Components::Camera* cam;

public:
    AnimationTestScene2() : IScene("AnimationTestScene2") {}

    void init() override
    {
        // Camera
        auto camGO = createGameObject("Camera");
        cam = camGO->addComponent<Components::Camera>(45.0f, 0.1f, 1000.0f, Graphics::MSAASamples::Four);
        cam->setClearColor(Color(175, 181, 191));
        camGO->getComponent<Components::Transform>()->position = Math::Vec3(0, 2, -5);
        camGO->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 0.1f);
        camGO->addComponent<Components::AudioListener>();

        auto skelShader = ASSETS.getShader("/shaders/skel_animation.shader");
        auto mat = ASSETS.getMaterial("/models/humanoid/mat.material");

        ArrayList<Animation::AnimationClip> anims;
        Animation::Skeleton skeleton;
        auto mesh = ASSETS.getMesh("/models/humanoid/model.dae", &skeleton, &anims);
        mat->setReplacementShader(TAG_SHADOW_PASS, skelShader);

        for (I32 x = -5; x < 5; x++)
        {
            for (I32 z = -5; z < 5; z++)
            {
                auto meshGO2 = createGameObject("GO2");
                meshGO2->addComponent<Components::SkinnedMeshRenderer>(mesh, skeleton, anims.front(), mat);

                meshGO2->getTransform()->position = { x * 3.0f, 0.0f, z * 3.0f };
                meshGO2->getTransform()->rotation *= Math::Quat(Math::Vec3::RIGHT, -90.0f);
                meshGO2->getTransform()->rotation *= Math::Quat(Math::Vec3::UP, 180.0f);
                meshGO2->getTransform()->scale *= 0.5f;
            }
        }

        camGO->addComponent<Components::GUI>();
        camGO->addComponent<Components::GUIFPS>();

        auto obj = createGameObject("GO");
        obj->addComponent<Components::MeshRenderer>(Core::MeshGenerator::CreatePlane(), ASSETS.getMaterial("/materials/blinn_phong/grass.material"));
        obj->getTransform()->rotation *= Math::Quat(Math::Vec3::RIGHT, 90.0f);
        obj->getTransform()->scale = { 20,20,20 };

        auto sun = createGameObject("Sun");
        auto dl = sun->addComponent<Components::DirectionalLight>(0.5f, Color::WHITE, Graphics::ShadowType::CSMSoft, ArrayList<F32>{20.0f, 40.0f, 80.0f, 200.0f});
        sun->getTransform()->rotation = Math::Quat::LookRotation(Math::Vec3{ 0,-1, 1 }, Math::Vec3{ 0, 0, 1 });
        dl->setShadowMapQuality(Graphics::ShadowMapQuality::Insane);

        auto cubemap = ASSETS.getCubemap("/cubemaps/tropical_sunny_day/Left.png", "/cubemaps/tropical_sunny_day/Right.png",
            "/cubemaps/tropical_sunny_day/Up.png", "/cubemaps/tropical_sunny_day/Down.png",
            "/cubemaps/tropical_sunny_day/Front.png", "/cubemaps/tropical_sunny_day/Back.png", true);
        camGO->addComponent<Components::Skybox>(cubemap);
    }

};