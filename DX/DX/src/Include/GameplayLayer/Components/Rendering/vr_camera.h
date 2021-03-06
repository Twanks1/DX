#pragma once
/**********************************************************************
    class: VRCamera + VRTouch

    author: S. Hau
    date: August 1, 2018
**********************************************************************/

#include "../i_component.h"
#include "Events/event.h"
#include "Graphics/enums.hpp"
#include "Graphics/forward_declarations.hpp"

namespace Components {

    class Camera;
    class Transform;

    //----------------------------------------------------------------------
    enum class ScreenDisplay // What will be displayed on the screen
    {
        None,
        LeftEye,
        RightEye,
        BothEyes
    };

    //**********************************************************************
    // Consists of two basic camera components, which renders into each eye.
    //**********************************************************************
    class VRCamera : public IComponent
    {
    public:
        VRCamera(ScreenDisplay screenDisplay = ScreenDisplay::LeftEye, Graphics::MSAASamples sampleCount = Graphics::MSAASamples::Four, bool hdr = false);

        //----------------------------------------------------------------------
        Components::Camera& getCameraForEye(Graphics::VR::Eye eye) { return *m_eyeCameras[eye]; }
        F32 getWorldScale() const;

        //----------------------------------------------------------------------
        // @Params:
        // "sd": What will be displayed on the regular screen/monitor (not the hmd).
        //----------------------------------------------------------------------
        void setScreenDisplay(ScreenDisplay sd);

        //----------------------------------------------------------------------
        // @Return: Local Position + Rotation from the HMD.
        //----------------------------------------------------------------------
        Transform* getHeadTransform();

        //----------------------------------------------------------------------
        // @Return: Direction in which the HMD is facing in World Space
        //----------------------------------------------------------------------
        Math::Vec3 getLookDirection();

        //----------------------------------------------------------------------
        // Set the clear color for both eye cameras
        //----------------------------------------------------------------------
        void setClearColor(Color color);

        //----------------------------------------------------------------------
        void setWorldScale(F32 newWorldScale);

        //----------------------------------------------------------------------
        // IComponent Interface
        //----------------------------------------------------------------------
        void addedToGameObject(GameObject* go) override;
        void setActive(bool active);

    private:
        GameObject*             m_eyeGameObjects[2];
        Components::Camera*     m_eyeCameras[2];
        Events::EventListener   m_frameBeginListener;

        void _OnFrameBegin();

        NULL_COPY_AND_ASSIGN(VRCamera)
    };


    //**********************************************************************
    // Updates the transform from the attached gameobject with the data
    // from the touch controller.
    //**********************************************************************
    class VRTouch : public IComponent
    {
    public:
        VRTouch(Graphics::VR::Hand hand);
        ~VRTouch() = default;

    private:
        Graphics::VR::Hand m_hand;
        Events::EventListener m_frameBeginListener;

        NULL_COPY_AND_ASSIGN(VRTouch)
    };

    //**********************************************************************
    // Custom component which sets up touch controller real quickly for testing.
    //**********************************************************************
    class VRBasicTouch : public IComponent
    {
    public:
        VRBasicTouch(const MeshPtr& mesh, const MaterialPtr& material);
        VRBasicTouch(const MeshPtr& leftHandMesh, const MeshPtr& rightHandMesh, const MaterialPtr& material);

        //----------------------------------------------------------------------
        // IComponent Interface
        //----------------------------------------------------------------------
        void addedToGameObject(GameObject* go) override;

    private:
        GameObject* m_handGameObject[2];
        Events::EventListener m_hmdFocusGainedListener;
        Events::EventListener m_hmdFocusLostListener;

        NULL_COPY_AND_ASSIGN(VRBasicTouch)
    };
}