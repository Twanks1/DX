// ----------------------------------------------
#Fill			Solid
#Cull 			None
#ZWrite 		On
#ZTest 			Less
#Queue 			Geometry

// ----------------------------------------------
#shader vertex

#include "includes/engineVS.hlsl"

struct VertexIn
{
    float3 PosL : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float2 Tex : TEXCOORD0;
	float3 Normal : NORMAL;
	float3 WorldPos : POSITION;
};

VertexOut main(VertexIn vin)
{
    VertexOut OUT;

	OUT.WorldPos 	= TO_WORLD_SPACE( vin.PosL );
    OUT.PosH 		= TO_CLIP_SPACE( vin.PosL );
	OUT.Tex 		= vin.tex;
	OUT.Normal 		= TRANSFORM_NORMAL( vin.normal );
	
    return OUT;
}

// ----------------------------------------------
#shader fragment  

#include "includes/enginePS.hlsl"
#include "includes/blinn_phong.hlsl"

cbuffer cbPerMaterial
{
	float uvScale;
} 

struct FragmentIn
{
    float4 PosH : SV_POSITION;
	float2 Tex : TEXCOORD0;
	float3 Normal : NORMAL;
	float3 WorldPos : POSITION;
};

Texture2D albedo;
SamplerState sampler0;

static const float ALPHA_THRESHOLD = 0.1f;

float4 main(FragmentIn fin) : SV_Target
{
	float4 textureColor = albedo.Sample(sampler0, fin.Tex * uvScale);
	
	//return float4(fin.Normal,1);
	if (textureColor.a < ALPHA_THRESHOLD)
		discard;
	
	return APPLY_LIGHTING( textureColor, fin.WorldPos, fin.Normal ); 
}