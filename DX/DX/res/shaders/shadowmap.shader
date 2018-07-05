// ----------------------------------------------
#Fill			Solid
#Cull 			None
#ZWrite 		On
#ZTest 			Less
#Queue 			Geometry
#DepthBias		3
#DBSlopeScaled  3
#DBClamp 		0
#DepthClip		Off

// ----------------------------------------------
#shader vertex

#include "/engine/shaders/includes/engineVS.hlsl"

struct VertexIn
{
    float3 PosL : POSITION;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
};

VertexOut main(VertexIn vin)
{
    VertexOut OUT;

    OUT.PosH = TO_CLIP_SPACE( vin.PosL );
	
    return OUT;
}

// ----------------------------------------------
#shader fragment  

#include "/engine/shaders/includes/enginePS.hlsl"

struct FragmentIn
{
    float4 PosH : SV_POSITION;
};

void main(FragmentIn fin)
{
}