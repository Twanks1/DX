// ----------------------------------------------
#shader vertex

cbuffer cbPerCamera
{
    float4x4 gViewProj;
};

cbuffer cbPerObject
{
    float4x4 gWorld;
};

struct VertexIn
{
    float3 PosL : POSITION;
	float2 tex : TEXCOORD0;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float2 tex : TEXCOORD0;
};

VertexOut main(VertexIn vin)
{
    VertexOut OUT;

    float4x4 mvp = mul(gViewProj, gWorld);
    OUT.PosH = mul(mvp, float4(vin.PosL, 1.0f));
	OUT.tex = vin.tex;
	
    return OUT;
}

// ----------------------------------------------
#shader fragment

cbuffer cbPerMaterial
{
	float4 color;
};

struct FragmentIn
{
    float4 PosH : SV_POSITION;
	float2 tex : TEXCOORD0;
};

Texture2D tex;
SamplerState sampler0;

float4 main(FragmentIn fin) : SV_Target
{
	float4 textureColor = tex.Sample(sampler0, fin.tex);
	return textureColor * color;
}