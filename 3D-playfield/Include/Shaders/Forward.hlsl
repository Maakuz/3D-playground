
struct VS_IN
{   
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : UV;

    float4x4 transform : TRANSFORM;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : UV;
};

cbuffer camera : register(b0)
{
    float4x4 VP;
    float4 camPos;
};



PS_IN VS(VS_IN input)
{
    PS_IN output = (PS_IN) 0;

    output.pos = float4(input.pos, 1);

    output.pos = mul(input.transform, output.pos);
    output.pos = mul(VP, output.pos);

    output.normal = input.normal;
    output.uv = input.uv;


    return output;
}




float4 PS(PS_IN input) : SV_Target0
{
    return float4(1, 1, 1, 1);
}