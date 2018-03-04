
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
    float3 Color : COLOR;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float4 Color : COLOR;
};

VertexOut main(VertexIn vin)
{
    VertexOut OUT;

    //matrix mvp = mul(projectionMatrix, mul(viewMatrix, worldMatrix));
    //OUT.PosH = mul(gWorldViewProj, float4(vin.PosL, 1.0f));

    float4x4 mvp = mul(gViewProj, gWorld);
    OUT.PosH = mul(mvp, float4(vin.PosL, 1.0f));
    OUT.Color = float4(vin.Color, 1.0f);

    return OUT;
}