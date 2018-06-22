#include "gui.h"
/**********************************************************************
    class: GUI (gui.cpp)
    
    author: S. Hau
    date: June 21, 2018
**********************************************************************/

#include "GameplayLayer/gameobject.h"
#include "Core/locator.h"
#include "camera.h"

#define SHADER_GUI_TEX_NAME "tex"

namespace ImGui
{
    void Image(const TexturePtr& tex)
    {
        ImGui::Image((void*)&tex, { (F32)tex->getWidth(), (F32)tex->getHeight() });
    }

    void Image(const TexturePtr& tex, const Math::Vec2& size)
    {
        ImGui::Image((void*)&tex, { size.x, size.y });
    }
}

namespace Components {

    //----------------------------------------------------------------------
    GUI::GUI()  // Listen only on gui-channel.
        : IMouseListener( (Core::Input::InputChannels)Core::Input::EInputChannel::GUI ),
            IKeyListener( (Core::Input::InputChannels)Core::Input::EInputChannel::GUI )
    {
        m_imguiContext = ImGui::CreateContext();
    }

    //----------------------------------------------------------------------
    GUI::~GUI()
    {
        ImGui::DestroyContext( m_imguiContext );
    }

    //**********************************************************************
    // PROTECTED
    //**********************************************************************

    //----------------------------------------------------------------------
    void GUI::addedToGameObject( GameObject* go )
    {
        // Retrieve camera which is mandatory
        m_camera = go->getComponent<Components::Camera>();
        ASSERT( m_camera && "This component requires a camera component attached to the gameobject!" );
        m_camera->addCommandBuffer( &m_cmd, Components::CameraEvent::Overlay );

        // IMGUI
        ImGui::SetCurrentContext( m_imguiContext );
        ImGuiIO& io = ImGui::GetIO();
        io.KeyMap[ImGuiKey_Tab]         = (I32)Key::Tab;
        io.KeyMap[ImGuiKey_LeftArrow]   = (I32)Key::Left;
        io.KeyMap[ImGuiKey_RightArrow]  = (I32)Key::Right;
        io.KeyMap[ImGuiKey_UpArrow]     = (I32)Key::Up;
        io.KeyMap[ImGuiKey_DownArrow]   = (I32)Key::Down;
        io.KeyMap[ImGuiKey_PageUp]      = (I32)Key::PageUp;
        io.KeyMap[ImGuiKey_PageDown]    = (I32)Key::PageDown;
        io.KeyMap[ImGuiKey_Home]        = (I32)Key::Home;
        io.KeyMap[ImGuiKey_End]         = (I32)Key::End;
        io.KeyMap[ImGuiKey_Insert]      = (I32)Key::Insert;
        io.KeyMap[ImGuiKey_Delete]      = (I32)Key::Delete;
        io.KeyMap[ImGuiKey_Backspace]   = (I32)Key::Backspace;
        io.KeyMap[ImGuiKey_Space]       = (I32)Key::Space;
        io.KeyMap[ImGuiKey_Enter]       = (I32)Key::Enter;
        io.KeyMap[ImGuiKey_Escape]      = (I32)Key::Escape;
        io.KeyMap[ImGuiKey_A]           = (I32)Key::A;
        io.KeyMap[ImGuiKey_C]           = (I32)Key::C;
        io.KeyMap[ImGuiKey_V]           = (I32)Key::V;
        io.KeyMap[ImGuiKey_X]           = (I32)Key::X;
        io.KeyMap[ImGuiKey_Y]           = (I32)Key::Y;
        io.KeyMap[ImGuiKey_Z]           = (I32)Key::Z;

        // Build and load the texture atlas into a texture
        unsigned char* pixels = NULL;
        I32 width, height;
        io.Fonts->GetTexDataAsRGBA32( &pixels, &width, &height );
        m_fontAtlas = RESOURCES.createTexture2D( width, height, Graphics::TextureFormat::RGBA32, pixels );
        io.Fonts->TexID = &m_fontAtlas;

        // Mesh containing all the vertex/index data
        m_dynamicMesh = RESOURCES.createMesh();
        m_dynamicMesh->setBufferUsage( Graphics::BufferUsage::Frequently );

        // Retrieve GUI shader
        m_guiShader = ASSETS.getShader( "/engine/shaders/gui.shader" );
        m_guiShader->setName( "GUI" );
    }

    //----------------------------------------------------------------------
    void GUI::preTick( Time::Seconds d )
    {
        // Needs to be in pretick, so it can disable the input early enough if this gui wants to have the input
        _UpdateIMGUI( (F32)d );
    }

    //----------------------------------------------------------------------
    void GUI::lateTick( Time::Seconds d )
    {
        ImGui::SetCurrentContext( m_imguiContext );
        ImGui::NewFrame();
        for (auto renderComponent : getGameObject()->getComponents<ImGUIRenderComponent>())
            renderComponent->OnImGUI();
        ImGui::EndFrame();
        ImGui::Render();

        m_cmd.reset();
        ImDrawData* draw_data = ImGui::GetDrawData();
        auto proj = DirectX::XMMatrixOrthographicOffCenterLH( draw_data->DisplayPos.x, draw_data->DisplayPos.x + draw_data->DisplaySize.x,
                                                              draw_data->DisplayPos.y + draw_data->DisplaySize.y, draw_data->DisplayPos.y,
                                                              -1, 1 );
        static const StringID PROJ_NAME = SID( "_Proj" );
        m_cmd.setCameraMatrix( PROJ_NAME, proj );

        ArrayList<Math::Vec3>   vertices;
        ArrayList<Math::Vec2>   uvs;
        ArrayList<Color>        colors;

        I32 subMesh = 0;
        U32 baseVertex = 0;
        for (I32 n = 0; n < draw_data->CmdListsCount; n++)
        {
            const ImDrawList*   cmd_list   = draw_data->CmdLists[n];
            const ImDrawVert*   vtx_buffer = cmd_list->VtxBuffer.Data;
            const ImDrawIdx*    idx_buffer = cmd_list->IdxBuffer.Data;

            for (I32 v = 0; v < cmd_list->VtxBuffer.Size; v++)
            {
                auto& vertex = vtx_buffer[v];
                vertices.push_back( { vertex.pos.x, vertex.pos.y, 0.0f } );
                uvs.push_back( { vertex.uv.x, vertex.uv.y } );
                colors.push_back( Color(vertex.col, false) );
            }

            for (I32 cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
            {
                const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
                if (pcmd->UserCallback)
                {
                    pcmd->UserCallback( cmd_list, pcmd );
                }
                else
                {
                    ArrayList<U32> indices( pcmd->ElemCount );
                    for (U32 i = 0; i < pcmd->ElemCount; i++)
                        indices[i] = idx_buffer[i];

                    ImVec2 pos = draw_data->DisplayPos;
                    Math::Rect r = { (I32)(pcmd->ClipRect.x - pos.x), (I32)(pcmd->ClipRect.y - pos.y),
                                     (I32)(pcmd->ClipRect.z - pos.x), (I32)(pcmd->ClipRect.w - pos.y) };
                    m_cmd.setScissor( r );

                    // Create a new material and set texture
                    auto mat = RESOURCES.createMaterial( m_guiShader );

                    TexturePtr* texture = static_cast<TexturePtr*>( pcmd->TextureId );
                    mat->setTexture( SHADER_GUI_TEX_NAME, *texture );

                    m_dynamicMesh->setIndices( indices, subMesh, Graphics::MeshTopology::Triangles, baseVertex );
                    m_cmd.drawMesh( m_dynamicMesh, mat, DirectX::XMMatrixIdentity(), subMesh );
                    subMesh++;
                }
                idx_buffer += pcmd->ElemCount;
            }
            baseVertex += cmd_list->VtxBuffer.Size;
        }

        if (vertices.size() > 0)
        {
            m_dynamicMesh->setVertices( vertices );
            m_dynamicMesh->setColors( colors );
            m_dynamicMesh->setUVs( uvs );
        }
    }

    //**********************************************************************
    // LISTENER
    //**********************************************************************

    //----------------------------------------------------------------------
    void GUI::OnMouseMoved( I16 x, I16 y )
    {
        if (not m_camera)
            return;

        ImGui::SetCurrentContext( m_imguiContext );
        ImGuiIO& io = ImGui::GetIO();

        auto& vp = m_camera->getViewport();

        F32 mouseX = (F32)MOUSE.getMousePos().x - m_camera->getRenderTarget()->getWidth() * vp.topLeftX;
        F32 mouseY = (F32)MOUSE.getMousePos().y - m_camera->getRenderTarget()->getHeight() * vp.topLeftY;

        // @TODO: Transform camera ray to render-target if camera dont render to screen
        io.MousePos.x = mouseX * (1.0f/vp.width);
        io.MousePos.y = mouseY * (1.0f/vp.height);
    }

    //----------------------------------------------------------------------
    void GUI::OnMousePressed( MouseKey key, KeyMod mod )
    {
        ImGui::SetCurrentContext( m_imguiContext );
        ImGuiIO& io = ImGui::GetIO();
        switch (key)
        {
        case MouseKey::LButton: io.MouseDown[0] = true; break;
        case MouseKey::RButton: io.MouseDown[1] = true; break;
        case MouseKey::MButton: io.MouseDown[2] = true; break;
        }
    }

    //----------------------------------------------------------------------
    void GUI::OnMouseReleased( MouseKey key, KeyMod mod )
    {
        ImGui::SetCurrentContext( m_imguiContext );
        ImGuiIO& io = ImGui::GetIO();
        switch (key)
        {
        case MouseKey::LButton: io.MouseDown[0] = false; break;
        case MouseKey::RButton: io.MouseDown[1] = false; break;
        case MouseKey::MButton: io.MouseDown[2] = false; break;
        }
    }

    //----------------------------------------------------------------------
    void GUI::OnMouseWheel( I16 delta )
    {
        ImGui::SetCurrentContext( m_imguiContext );
        ImGuiIO& io = ImGui::GetIO();

        io.MouseWheel = delta;
    }

    //----------------------------------------------------------------------
    void GUI::OnKeyPressed( Key key, KeyMod mod )
    {
        ImGui::SetCurrentContext( m_imguiContext );
        ImGuiIO& io = ImGui::GetIO();
        if (mod & KeyModBits::CONTROL)  io.KeyCtrl  = true;
        if (mod & KeyModBits::ALT)      io.KeyAlt   = true;
        if (mod & KeyModBits::SHIFT)    io.KeyShift = true;
        io.KeysDown[ (I32)key ] = true;
    }


    //----------------------------------------------------------------------
    void GUI::OnKeyReleased( Key key, KeyMod mod )
    {
        ImGui::SetCurrentContext( m_imguiContext );
        ImGuiIO& io = ImGui::GetIO();
        if (mod & KeyModBits::CONTROL)  io.KeyCtrl  = false;
        if (mod & KeyModBits::ALT)      io.KeyAlt   = false;
        if (mod & KeyModBits::SHIFT)    io.KeyShift = false;
        io.KeysDown[ (I32)key ] = false;
    }

    //----------------------------------------------------------------------
    void GUI::OnChar( char c )
    {
        ImGui::SetCurrentContext( m_imguiContext );
        ImGuiIO& io = ImGui::GetIO();

        io.AddInputCharacter( c );
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void GUI::_UpdateIMGUI( F32 delta )
    {
        ImGui::SetCurrentContext( m_imguiContext );
        ImGuiIO& io = ImGui::GetIO();
        io.DeltaTime = delta;

        auto& rt = m_camera->getRenderTarget();
        io.DisplaySize.x = (F32)rt->getWidth();
        io.DisplaySize.y = (F32)rt->getHeight();

        // Update mouse-pos @TODO: Transform camera ray to render-target if camera dont render to screen
        auto& vp = m_camera->getViewport();
        F32 mouseX = (F32)MOUSE.getMousePos().x - m_camera->getRenderTarget()->getWidth() * vp.topLeftX;
        F32 mouseY = (F32)MOUSE.getMousePos().y - m_camera->getRenderTarget()->getHeight() * vp.topLeftY;

        io.MousePos.x = mouseX * (1.0f / vp.width);
        io.MousePos.y = mouseY * (1.0f / vp.height);

        using namespace Core::Input;
        bool consoleIsOpen = KEYBOARD.getChannelMask() & (InputChannels)EInputChannel::Console;
        if ( m_camera->isRenderingToScreen() && not consoleIsOpen )
        {
            if (io.WantCaptureMouse) // Disable master channel + enable GUI Channel
            {
                MOUSE.setChannel( EInputChannel::GUI );
                MOUSE.unsetChannel( EInputChannel::Master );
            }
            else
            {
                MOUSE.setChannel( EInputChannel::Master );
                MOUSE.unsetChannel( EInputChannel::GUI );
            }

            if (io.WantCaptureKeyboard) // Disable master channel + enable GUI Channel
            {
                KEYBOARD.setChannel( EInputChannel::GUI );
                KEYBOARD.unsetChannel( EInputChannel::Master );
            }
            else
            {
                KEYBOARD.setChannel( EInputChannel::Master );
                KEYBOARD.unsetChannel( EInputChannel::GUI );
            }
        }
    }

    //**********************************************************************
    // GUI COMPONENTS
    //**********************************************************************

    //----------------------------------------------------------------------
    void GUIFPS::OnImGUI()
    {
        const float DISTANCE = 10.0f;
        static int corner = 0;
        ImVec2 window_pos = ImVec2((corner & 1) ? ImGui::GetIO().DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? ImGui::GetIO().DisplaySize.y - DISTANCE : DISTANCE);
        ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
        if (corner != -1)
            ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        ImGui::SetNextWindowBgAlpha(0.3f); // Transparent background

        static bool p_open = false;
        if (ImGui::Begin("Stats", &p_open, (corner != -1 ? ImGuiWindowFlags_NoMove : 0) | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
        {
            U32 fps = PROFILER.getFPS();
            F64 delta = (1000.0 / fps);
            ImGui::Text("FPS: %u (%.3f ms)", fps, delta);
            static bool vsync = Locator::getRenderer().isVSyncEnabled();
            ImGui::Checkbox("Vsync", &vsync);
            Locator::getRenderer().setVSync(vsync);

            if (ImGui::BeginPopupContextWindow())
            {
                if (ImGui::MenuItem("Custom", NULL, corner == -1)) corner = -1;
                if (ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
                if (ImGui::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
                if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
                if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
                if (p_open && ImGui::MenuItem("Close")) p_open = false;
                ImGui::EndPopup();
            }
            ImGui::End();
        }
    }

} // End namespaces