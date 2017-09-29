//Credits to someone in my large game assignment group. Thanks!
struct VS_IN
{
    uint vertexId : SV_VertexID;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float2 uv : UV;
};

PS_IN VS(VS_IN input)
{
    PS_IN output;
    
    if (input.vertexId == 0)
    {
        output.pos = float4(-1, 3, 0, 1.0);
        output.uv = float2(0, -1);
    }
    else if (input.vertexId == 2)
    {
        output.pos = float4(-1, -1, 0, 1.0);
        output.uv = float2(0, 1);
    }
    else if (input.vertexId == 1)
    {
        output.pos = float4(3, -1, 0, 1.0);
        output.uv = float2(2, 1);
    }

    return output;
}

Texture2D sampleTexture : register(t0);
SamplerState sState;

float4 PS(PS_IN input) : SV_Target0
{
    return float4(sampleTexture.Sample(sState, input.uv).xyz, 1.f);
};