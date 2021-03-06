#pragma once
#include <DX.h>
using namespace Core;

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
    Color(0, 0, 255),
    Color(0, 255, 0),
    Color(255, 0, 0),
    Color(255, 255, 0)
};

//----------------------------------------------------------------------
// COMPONENTS
//----------------------------------------------------------------------

class ConstantRotation : public Components::IComponent
{
    Math::Vec3 m_speeds = Math::Vec3(0.0f);
    Math::Vec3 m_curDegrees = Math::Vec3(0.0f);
public:
    // Attached gameobject rotates "{degree,degree,degree}" per second around the respective axis.
    ConstantRotation(F32 pitchSpeed, F32 yawSpeed, F32 rollSpeed) 
        : m_speeds(Math::Vec3{ pitchSpeed, yawSpeed, rollSpeed }) {}

    void tick( Time::Seconds delta ) override
    {
        auto t = getGameObject()->getComponent<Components::Transform>();
        t->rotation = Math::Quat::FromEulerAngles( m_curDegrees );
        m_curDegrees += m_speeds * (F32)delta.value;
    }
};

class VertexGeneration : public Components::IComponent
{
    MeshPtr                     mesh;
    Components::MeshRenderer*   mr;

    const U32 width  = 20;
    const U32 height = 20;

public:
    void addedToGameObject(GameObject* go) override
    {
        generateMesh();
        auto transform = go->getComponent<Components::Transform>();
        transform->rotation = Math::Quat(Math::Vec3::RIGHT, 90);
        transform->position = Math::Vec3(-(width/2.0f), -2.0f, -(height/2.0f));

        mr = go->addComponent<Components::MeshRenderer>(mesh, ASSETS.getColorMaterial());
    }

    void tick(Time::Seconds delta)
    {
        U32 i = 0;
        for (auto& pos : mesh->getPositionStream())
        {
            F32 newY = (F32)sin(TIME.getTime().value);
            pos.z = (i % 2 == 0 ? newY : -newY);
            i++;
        }

        for (auto& color : mesh->getColorStream())
            color.y = ((sinf((F32)TIME.getTime().value) + 1) / 2);
    }

private:
    void generateMesh()
    {
        mesh = Core::MeshGenerator::CreatePlane(width, height);
        mesh->setBufferUsage(Graphics::BufferUsage::Frequently);

        ArrayList<Color> m_colors;
        for (U32 i = 0; i < mesh->getVertexCount(); i++)
            m_colors.push_back( Math::Random::Color() );
        mesh->setColors( m_colors );
    }
};

class GridGeneration : public Components::IComponent
{
    U32 m_size;

public:
    GridGeneration(U32 size) : m_size(size) {}

    void addedToGameObject(GameObject* go) override
    {
        auto mesh = Core::MeshGenerator::CreateGrid(m_size);
        auto mr = go->addComponent<Components::MeshRenderer>(mesh, ASSETS.getColorMaterial());
        mr->setCastShadows(false);
    }

private:
};

class AutoOrbiting : public Components::IComponent
{
    F32 m_speed;
    F32 m_curDegrees;
    Math::Vec3 m_center;

public:
    // Attached gameobject rotates "yawSpeed" (in degrees) per second around the center.
    AutoOrbiting(F32 yawSpeed, Math::Vec3 center = Math::Vec3())
        : m_speed( yawSpeed ), m_center(center) {}

    void tick(Time::Seconds delta) override
    {
        auto t = getGameObject()->getComponent<Components::Transform>();

        Math::Vec3 vecXZ = t->position;
        vecXZ.y = 0.0f;
        F32 length = vecXZ.magnitude();

        t->position.x = m_center.x + length * cos( Math::Deg2Rad( m_curDegrees ) );
        t->position.z = m_center.z + length * sin( Math::Deg2Rad( m_curDegrees ) );

        t->lookAt(m_center);

        m_curDegrees += m_speed * (F32)delta;
    }
};

class DrawFrustum : public Components::IComponent
{
public:
    void addedToGameObject(GameObject* go) override
    {
        const Graphics::Camera* cam = nullptr;
        if ( auto camComponent = go->getComponent<Components::Camera>() )
            cam = &camComponent->getNativeCamera();
        else if (auto lightComponent = go->getComponent<Components::DirectionalLight>())
            cam = &lightComponent->getNativeCamera();
        else if (auto lightComponent = go->getComponent<Components::SpotLight>())
            cam = &lightComponent->getNativeCamera();

        MeshPtr mesh;
        if (cam->getCameraMode() == Graphics::CameraMode::Perspective)
            mesh = Core::MeshGenerator::CreateFrustum( Math::Vec3(0), Math::Vec3::UP, Math::Vec3::RIGHT, Math::Vec3::FORWARD,
                                                       cam->getFOV(), cam->getZNear(), cam->getZFar(), cam->getAspectRatio(), Color::GREEN );
        else
            mesh = Core::MeshGenerator::CreateFrustum( Math::Vec3(0), cam->getLeft(), cam->getRight(), cam->getBottom(), cam->getTop(), cam->getZNear(), cam->getZFar() );

        auto mr = go->addComponent<Components::MeshRenderer>(mesh, ASSETS.getColorMaterial());
        mr->setCastShadows(false);
    }
};


// Visualizes the normals from the mesh in the attached mesh renderer
class VisualizeNormals : public Components::IComponent
{
    F32 m_length;
    Color m_color;

public:
    VisualizeNormals(F32 length = 0.5f, Color color = Color::BLUE)
        : m_length(length), m_color(color) {}

    void addedToGameObject(GameObject* go) override
    {
        auto mr = getGameObject()->getComponent<Components::MeshRenderer>();
        ASSERT(mr != nullptr && "VisualizeNormals requires a mesh renderer component attached to it!");

        auto mesh = mr->getMesh();
        auto& meshNormals = mesh->getNormals();
        auto& meshVertices = mesh->getVertexPositions();

        ArrayList<Math::Vec3> vertices;
        ArrayList<U32> indices;
        U32 index = 0;
        for (I32 i = 0; i < meshNormals.size(); i++)
        {
            auto start = meshVertices[i];
            auto end = start + meshNormals[i] * m_length;

            vertices.emplace_back(start);
            vertices.emplace_back(end);

            indices.push_back(index++);
            indices.push_back(index++);
        }

        auto normalMesh = RESOURCES.createMesh();
        normalMesh->setVertices(vertices);
        normalMesh->setIndices(indices, 0, Graphics::MeshTopology::Lines);
        normalMesh->setColors(ArrayList<Color>(vertices.size(), m_color));

        auto normalGO = go->getScene()->createGameObject( "NormalsVisualizer" );
        normalGO->getTransform()->setParent( go->getTransform() );
        normalGO->addComponent<Components::MeshRenderer>( normalMesh, ASSETS.getColorMaterial() );
    }
};

// Visualizes the normals from the mesh in the attached mesh renderer
class VisualizeTangents : public Components::IComponent
{
    F32 m_length;
    Color m_color;

public:
    VisualizeTangents(F32 length, Color color)
        : m_length(length), m_color(color) {}

    void addedToGameObject(GameObject* go) override
    {
        auto mr = getGameObject()->getComponent<Components::MeshRenderer>();
        ASSERT(mr != nullptr && "VisualizeNormals requires a mesh renderer component attached to it!");

        auto mesh = mr->getMesh();
        auto& meshTangents = mesh->getTangents();
        auto& meshVertices = mesh->getVertexPositions();

        ArrayList<Math::Vec3> vertices;
        ArrayList<U32> indices;
        U32 index = 0;
        for (I32 i = 0; i < meshTangents.size(); i++)
        {
            auto start = meshVertices[i];
            auto end = start + Math::Vec3(meshTangents[i].x, meshTangents[i].y, meshTangents[i].z) * m_length;

            vertices.emplace_back(start);
            vertices.emplace_back(end);

            indices.push_back(index++);
            indices.push_back(index++);
        }

        auto tangentMesh = RESOURCES.createMesh();
        tangentMesh->setVertices(vertices);
        tangentMesh->setIndices(indices, 0, Graphics::MeshTopology::Lines);
        tangentMesh->setColors(ArrayList<Color>(vertices.size(), m_color));

        auto meshGO = go->getScene()->createGameObject("TangentVisualizer");
        meshGO->getTransform()->setParent(go->getTransform());
        meshGO->addComponent<Components::MeshRenderer>(tangentMesh, ASSETS.getColorMaterial());
    }
};

class GaussianBlur : public Components::IComponent
{
    Graphics::CommandBuffer cmd;
    MaterialPtr horizontalBlur;
    MaterialPtr verticalBlur;

public:
    ~GaussianBlur() { if (auto cam = getGameObject()->getComponent<Components::Camera>()) cam->removeCommandBuffer(&cmd); }

    void addedToGameObject(GameObject* go)
    {
        // Create rendertarget
        auto rt = RESOURCES.createRenderTexture(WINDOW.getWidth(), WINDOW.getHeight(), Graphics::TextureFormat::RGBA32, true);
        auto rt2 = RESOURCES.createRenderTexture(WINDOW.getWidth(), WINDOW.getHeight(), Graphics::TextureFormat::RGBA32, true);

        horizontalBlur = ASSETS.getMaterial("/materials/post processing/gaussian_blur_horizontal.material");
        verticalBlur = ASSETS.getMaterial("/materials/post processing/gaussian_blur_vertical.material");

        // Apply post processing
        cmd.blit(PREVIOUS_BUFFER, rt, horizontalBlur);
        cmd.blit(rt, rt2, verticalBlur);

        // Attach command buffer to camera
        auto cam = go->getComponent<Components::Camera>();
        cam->addCommandBuffer(&cmd, Components::CameraEvent::PostProcess);
    }
};

class Fog : public Components::IComponent
{
    Graphics::CommandBuffer cmd;
public:
    ~Fog() { if (auto cam = getGameObject()->getComponent<Components::Camera>()) cam->removeCommandBuffer(&cmd); }

    void addedToGameObject(GameObject* go)
    {
        auto cam = go->getComponent<Components::Camera>();

        auto shader = ASSETS.getShader("/engine/shaders/post processing/fog.shader");
        shader->setReloadCallback([=](Graphics::Shader* shader) {
            shader->setTexture("depthBuffer", cam->getRenderTarget()->getDepthBuffer());
        });
        shader->invokeReloadCallback();

        // Apply post processing
        auto rt = RESOURCES.createRenderTexture(WINDOW.getWidth(), WINDOW.getHeight(), Graphics::TextureFormat::RGBA32, true);
        cmd.blit(PREVIOUS_BUFFER, rt, ASSETS.getMaterial("/materials/post processing/fog.material"));

        // Attach command buffer to camera
        cam->addCommandBuffer(&cmd, Components::CameraEvent::PostProcess);
    }
};

// Because components can only be added once, using another template this component get be used more than once
template <typename T = void>
class PostProcess : public Components::IComponent
{
    Graphics::CommandBuffer cmd[2];
    MaterialPtr m_material;
    bool m_hdr;
    bool m_addedCommandBuffer = false;

public:
    PostProcess(const MaterialPtr& material, bool hdr = false) : m_material(material), m_hdr(hdr) {}
    PostProcess(const ShaderPtr& shader, bool hdr = false) : m_material(RESOURCES.createMaterial(shader)), m_hdr(hdr) {}
    ~PostProcess() { _RemoveCommandBuffer(); }

    const MaterialPtr& getMaterial() const { return m_material; }

    void addedToGameObject(GameObject* go)
    {
        auto cam = go->getComponent<Components::Camera>();
        auto vrCam = go->getComponent<Components::VRCamera>();
        if (!cam && !vrCam)
            LOG_WARN("Post Process component requires an attached camera (vr or basic) component!");

        if (vrCam)
        {
            auto hmdDesc = RENDERER.getHMD().getDescription();
            for (auto eye : { Graphics::VR::LeftEye, Graphics::VR::RightEye })
            {
                // Create render target
                auto rt = RESOURCES.createRenderTexture(hmdDesc.idealResolution[eye].x, hmdDesc.idealResolution[eye].y, m_hdr ? Graphics::TextureFormat::RGBAFloat : Graphics::TextureFormat::RGBA32, false);
                cmd[eye].blit(PREVIOUS_BUFFER, rt, m_material);
            }
        }
        else if (cam)
        {
            // Create render target
            auto width = cam->getRenderTarget()->getWidth();
            auto height = cam->getRenderTarget()->getHeight();
            auto rt = RESOURCES.createRenderTexture(width, height, m_hdr ? Graphics::TextureFormat::RGBAFloat : Graphics::TextureFormat::RGBA32, true);
            cmd[0].blit(PREVIOUS_BUFFER, rt, m_material);
        }
        _AddCommandBuffer();
    }

    void onInActive() override { _RemoveCommandBuffer(); }
    void onActive() override   { _AddCommandBuffer(); }

private:
    void _AddCommandBuffer()
    {
        if (m_addedCommandBuffer)
            return;

        auto cam = getGameObject()->getComponent<Components::Camera>();
        auto vrCam = getGameObject()->getComponent<Components::VRCamera>();

        if (vrCam)
        {
            auto hmdDesc = RENDERER.getHMD().getDescription();
            for (auto eye : { Graphics::VR::LeftEye, Graphics::VR::RightEye })
                vrCam->getCameraForEye(eye).addCommandBuffer(&cmd[eye], Components::CameraEvent::PostProcess);
            m_addedCommandBuffer = true;
        }
        else if (cam)
        {
            // Attach command buffer to camera
            cam->addCommandBuffer(&cmd[0], Components::CameraEvent::PostProcess);
            m_addedCommandBuffer = true;
        }
    }

    void _RemoveCommandBuffer()
    {
        if (m_addedCommandBuffer)
        {
            if (auto cam = getGameObject()->getComponent<Components::Camera>())
                cam->removeCommandBuffer(&cmd[0]);
            if (auto vrCam = getGameObject()->getComponent<Components::VRCamera>())
                for (auto eye : { Graphics::VR::LeftEye, Graphics::VR::RightEye })
                    vrCam->getCameraForEye(eye).removeCommandBuffer(&cmd[eye]);
            m_addedCommandBuffer = false;
        }
    }
};

class Tonemap : public PostProcess<>
{
public:
    Tonemap() : PostProcess(ASSETS.getMaterial("/materials/post processing/tonemap.material")) {}
};

//**********************************************************************
// Fades scene from complete black in "duration" time in
//**********************************************************************
class FadeIn : public PostProcess<>
{
public:
    FadeIn(Time::Milliseconds duration)
        : PostProcess(ASSETS.getMaterial("/materials/post processing/blend.material")), m_duration(duration) {}

    void tick(Time::Seconds delta) override
    {
        m_time += static_cast<Time::Milliseconds>(delta);
        F64 blend = Math::Clamp( m_time.value / m_duration.value, 0.0, 1.0);
        blend = Math::Lerp(blend, 1.0, 0.05);

        auto& mat = getMaterial();
        mat->setFloat("blend", (F32)blend);

        if (blend == 1.0)
            this->setActive(false);
    }

private:
    Time::Milliseconds m_duration;
    Time::Milliseconds m_time = 0_ms;
};


//**********************************************************************
class GUISceneMenu : public Components::ImGUIRenderComponent
{
    String m_menuName;
public:
    GUISceneMenu(CString menuName) : m_menuName(menuName) {}

    void OnImGUI() override
    {
        ImGui::Begin(m_menuName.c_str());
        for (auto& scene : m_scenes)
            if (ImGui::Button(scene.first.c_str()))
                Locator::getSceneManager().LoadScene(scene.second());
        ImGui::End();
    }

    template <typename T>
    void registerScene(const String& name)
    {
        m_scenes.emplace_back(name, [] { return new T(); });
    }

private:
    ArrayList<std::pair<String, std::function<IScene*()>>> m_scenes;
};

class VisualizeLightRange : public Components::IComponent
{
public:
    void addedToGameObject(GameObject* go) override
    {
        F32 range = 0.0f;
        if (auto lightComponent = go->getComponent<Components::PointLight>())
            range = lightComponent->getRange();
        else if (auto lightComponent = go->getComponent<Components::SpotLight>())
            range = lightComponent->getRange();

        MeshPtr mesh = Core::MeshGenerator::CreateUVSphere({}, range, 10, 10);

        auto mr = go->addComponent<Components::MeshRenderer>(mesh, ASSETS.getWireframeMaterial());
        mr->setCastShadows(false);
    }
};
