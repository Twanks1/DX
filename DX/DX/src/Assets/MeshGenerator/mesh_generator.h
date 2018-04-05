#pragma once
/**********************************************************************
    class: MeshGenerator (mesh_generator.h)

    author: S. Hau
    date: March 11, 2018
**********************************************************************/

#include "Graphics/i_mesh.h"

namespace Assets { 

    //*********************************************************************
    class MeshGenerator
    {
    public:
        //----------------------------------------------------------------------
        // Generates a new cube mesh.
        // @Params:
        // "size": The size/radi of the cube.
        // "color" [opt]: The vertex-color for each vertex.
        //----------------------------------------------------------------------
        static Graphics::Mesh* CreateCube(F32 size = 1.0f);
        static Graphics::Mesh* CreateCube(F32 size, Color color);
        static Graphics::Mesh* CreateCube(const Math::Vec3& min, const Math::Vec3& max);

        //----------------------------------------------------------------------
        // Generates a new cube mesh with uv coordinates (no colors!).
        // @Params:
        // "size": The size/radi of the cube.
        //----------------------------------------------------------------------
        static Graphics::Mesh* CreateCubeUV(F32 size = 1.0f);

        //----------------------------------------------------------------------
        // Generates a new plane mesh in the X/Y Plane with 4 vertices.
        // @Params:
        // "size": The size of the plane.
        // "color" [opt]: The vertex-color for each vertex.
        //----------------------------------------------------------------------
        static Graphics::Mesh* CreatePlane(F32 size = 1.0f);
        static Graphics::Mesh* CreatePlane(F32 size, Color color);

        //----------------------------------------------------------------------
        // Generates a new plane mesh in the X/Y Plane with width * height vertices.
        // @Params:
        // "width": Amount of vertices in the x-plane.
        // "height": Amount of vertices in the y-plane.
        // "color" [opt]: The vertex-color for each vertex.
        //----------------------------------------------------------------------
        static Graphics::Mesh* CreatePlane(U32 width, U32 height);
        static Graphics::Mesh* CreatePlane(U32 width, U32 height, Color color);

        //----------------------------------------------------------------------
        // Generates a new sphere mesh.
        // @Params:
        // "size": The size of the sphere.
        // "center": Center of the sphere mesh.
        // "radius": Radius of the sphere mesh.
        // "color" [opt]: The vertex-color for each vertex.
        //----------------------------------------------------------------------
        static Graphics::Mesh* CreateUVSphere(U32 meridians, U32 parallels);
        static Graphics::Mesh* CreateUVSphere(U32 meridians, U32 parallels, Color color);
        static Graphics::Mesh* CreateUVSphere(const Math::Vec3& center, F32 radius, U32 meridians, U32 parallels);

        //----------------------------------------------------------------------
        // Generates a new grid mesh in the XZ plane.
        // @Params:
        // "size": The size of the grid. Generates size + size lines.
        //----------------------------------------------------------------------
        static Graphics::Mesh* CreateGrid(U32 size);

        //----------------------------------------------------------------------
        // Generates a new perspective frustum mesh (Lines only).
        //----------------------------------------------------------------------
        static Graphics::Mesh* CreateFrustum(const Math::Vec3& pos, const Math::Vec3& up, const Math::Vec3& right, const Math::Vec3& forward,
                                             F32 fovAngleYRad, F32 zNear, F32 zFar, F32 aspectRatio, Color color = Color::WHITE);

    private:
        //----------------------------------------------------------------------
        MeshGenerator()                                         = delete;
        MeshGenerator(const MeshGenerator& other)               = delete;
        MeshGenerator& operator = (const MeshGenerator& other)  = delete;
        MeshGenerator(MeshGenerator&& other)                    = delete;
        MeshGenerator& operator = (MeshGenerator&& other)       = delete;
    };

}