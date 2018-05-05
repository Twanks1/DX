#pragma once
/**********************************************************************
    class: MeshRenderer (mesh_renderer.h)

    author: S. Hau
    date: December 19, 2017

    Used to render models.
    A Mesh-Renderer consists of a mesh (with possibly submeshes) and
    a list of materials. Each material belongs to the corresponding
    submesh in the mesh-class e.g. SubMesh 1 gets rendered with Material 1.
**********************************************************************/

#include "i_render_component.hpp"
#include "Graphics/i_mesh.h"
#include "Graphics/i_material.h"

namespace Components {

    //**********************************************************************
    class MeshRenderer : public IRenderComponent
    {
    public:
        MeshRenderer() = default;
        MeshRenderer(MeshPtr mesh, MaterialPtr material = nullptr);

        MeshPtr                                 getMesh()                   const   { return m_mesh; }
        MaterialPtr                             getMaterial(U32 index = 0)  const   { return m_materials[index]; }
        const ArrayList<MaterialPtr>&           getMaterials()              const   { return m_materials; }
        U32                                     getMaterialCount()          const   { return static_cast<U32>( m_materials.size() ); }

        void setMesh(MeshPtr mesh);
        void setMaterial(MaterialPtr m, U32 index = 0);

    private:
        MeshPtr                 m_mesh;
        ArrayList<MaterialPtr>  m_materials;

        //----------------------------------------------------------------------
        // IComponent Interface
        //----------------------------------------------------------------------
        void addedToGameObject(GameObject* go) override;

        //----------------------------------------------------------------------
        // IRendererComponent Interface
        //----------------------------------------------------------------------
        void recordGraphicsCommands(Graphics::CommandBuffer& cmd, F32 lerp) override;
        bool cull(const Camera& camera) override;


        //inline bool _IsVisible(const Camera& camera, const DirectX::XMMATRIX& modelMatrix);

        //----------------------------------------------------------------------
        MeshRenderer(const MeshRenderer& other)               = delete;
        MeshRenderer& operator = (const MeshRenderer& other)  = delete;
        MeshRenderer(MeshRenderer&& other)                    = delete;
        MeshRenderer& operator = (MeshRenderer&& other)       = delete;
    };

}