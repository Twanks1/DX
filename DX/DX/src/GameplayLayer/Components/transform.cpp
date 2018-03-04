#include "transform.h"
/**********************************************************************
    class: Transform (transform.cpp)

    author: S. Hau
    date: December 17, 2017
**********************************************************************/

namespace Components {

    //----------------------------------------------------------------------
    void Transform::Tick( Time::Seconds delta )
    {

       // LOG("TICK");

    }

    //----------------------------------------------------------------------
    DirectX::XMMATRIX Transform::getTransformationMatrix()
    {
        DirectX::XMVECTOR s = DirectX::XMLoadFloat3( &scale );
        DirectX::XMVECTOR r = DirectX::XMLoadFloat4( &rotation );
        DirectX::XMVECTOR p = DirectX::XMLoadFloat3( &position );

        return DirectX::XMMatrixAffineTransformation( s, DirectX::XMQuaternionIdentity(), r, p );
    }

    //----------------------------------------------------------------------
    void Transform::setParent(const Transform& t)
    {
        ASSERT(false);
    }

    //----------------------------------------------------------------------
    void Transform::addChild(const Transform& t)
    {
        ASSERT(false);
    }


}