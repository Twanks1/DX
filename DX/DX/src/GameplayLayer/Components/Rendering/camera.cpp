#include "camera.h"
/**********************************************************************
    class: Camera (camera.cpp)

    author: S. Hau
    date: March 4, 2018
**********************************************************************/

#include "GameplayLayer/Components/transform.h"
#include "GameplayLayer/gameobject.h"
#include "Graphics/command_buffer.h"

namespace Components {

    //----------------------------------------------------------------------
    ArrayList<Camera*> Camera::s_cameras;

    //----------------------------------------------------------------------
    Camera::Camera( F32 fovAngleYInDegree, F32 zNear, F32 zFar )
    {
        m_cameraMode = EMode::PERSPECTIVE;
        setPerspectiveParams( fovAngleYInDegree, zNear, zFar );
        s_cameras.push_back( this );
    }

    //----------------------------------------------------------------------
    Camera::Camera( F32 left, F32 right, F32 bottom, F32 top, F32 zNear, F32 zFar )
    {
        m_cameraMode = EMode::ORTHOGRAPHIC;
        setOrthoParams( left, right, bottom, top, zNear, zFar );
        s_cameras.push_back( this );
    }

    //----------------------------------------------------------------------
    Camera::~Camera()
    {
        std::remove( s_cameras.begin(), s_cameras.end(), this );
    }

    //----------------------------------------------------------------------
    void Camera::recordGraphicsCommands( Graphics::CommandBuffer& cmd, F32 lerp )
    {
        // Set camera parameters
        cmd.setRenderTarget( getRenderTarget(), getClearColor() );

        auto transform = getGameObject()->getComponent<Components::Transform>();
        auto& transformation = transform->getTransformationMatrix();
        auto view = DirectX::XMMatrixTranspose( transformation );

        switch ( m_cameraMode )
        {
        case Components::Camera::PERSPECTIVE:
            cmd.setCameraPerspective( view, getFOV(), getZNear(), getZFar() );
            break;
        case Components::Camera::ORTHOGRAPHIC:
            cmd.setCameraOrtho( view, getLeft(), getRight(), getBottom(), getTop(), getZNear(), getZFar() );
            break;
        default:
            WARN_RENDERING( "UNKNOWN CAMERA MODE" );
        }
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void Camera::setOrthoParams( F32 left, F32 right, F32 bottom, F32 top, F32 zNear, F32 zFar )
    {
        m_left = left; m_right = right; m_bottom = bottom; m_top = top; m_zNear = zNear; m_zFar = zFar;
    }

    //----------------------------------------------------------------------
    void Camera::setPerspectiveParams( F32 fovAngleYInDegree, F32 zNear, F32 zFar )
    {
        m_fov = fovAngleYInDegree; m_zNear = zNear; m_zFar = zFar;
    }

    ////----------------------------------------------------------------------
    //F32 Camera::getAspecRatio() const
    //{
    //    // if (m_renderTexture == nullptr)
    //    OS::Window& window = Locator::getWindow();
    //    return window.getWidth() / (F32) window.getHeight();
    //}

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    //void Camera::_UpdateProjectionMatrix()
    //{
    //    switch (m_cameraMode)
    //    {
    //        case EMode::PERSPECTIVE:
    //            m_projection = DirectX::XMMatrixPerspectiveFovLH( DirectX::XMConvertToRadians( m_fov ), getAspecRatio(), m_zNear, m_zFar );
    //            break;
    //        case EMode::ORTHOGRAPHIC:
    //            m_projection = DirectX::XMMatrixOrthographicOffCenterLH( m_left, m_right, m_bottom, m_top, m_zNear, m_zFar );
    //            break;
    //    }
    //}


}