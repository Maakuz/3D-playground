
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

    output.normal = normalize(mul(input.transform, float4(input.normal, 0)));
    output.uv = input.uv;


    return output;
}


SamplerState sState : register(s0);
texture2D diffuseTexture : register(t0);

float4 PS(PS_IN input) : SV_Target0
{
    float3 lightDir = normalize(float3(0.3, -1, 0.3));
    float3 lightColor = float3(0, 0.7, 0.6);

    float3 diffuseComponent = dot(input.normal, -lightDir) * lightColor;

    float3 color = diffuseTexture.Sample(sState, input.uv);


    return float4(0, input.uv, 1);
    return float4(color * diffuseComponent, 1);
}