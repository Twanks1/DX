
struct FragmentIn
{
    float4 PosH : SV_POSITION;
};

float4 main(FragmentIn fin) : SV_Target
{
    return float4(1, 0, 0, 0.5);
}