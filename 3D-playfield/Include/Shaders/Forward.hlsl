
struct VS_IN
{   
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : UV;

    float4x4 transform : TRANSFORM;
};

struct GS_IN
{
    float4 pos : POSITION;
    float4 wPos : WORLDPOSITION;
    float3 normal : NORMAL;
    float2 uv : UV;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 wPos : WORLDPOSITION;
    float3 normal : NORMAL;
    float2 uv : UV;
    float3 tangent : TANGENT;
    float3 biTangent : BITANGENT;
};

cbuffer camera : register(b0)
{
    float4x4 VP;
    float4 camPos;
};

cbuffer flashLight : register(b1)
{
    float4 lightPos;
    float4 lightDir;
    bool lightOn;
};

GS_IN VS(VS_IN input)
{
    GS_IN output = (GS_IN) 0;

    output.wPos = float4(input.pos, 1);

    output.wPos = mul(input.transform, output.wPos);
    output.pos = mul(VP, output.wPos);

    output.normal = normalize(mul(input.transform, float4(input.normal, 0)));
    output.uv = input.uv;


    return output;
}

//Credits to Ipren
[maxvertexcount(3)]
void GS(triangle GS_IN input[3] : SV_POSITION, inout TriangleStream<PS_IN> output)
{
     //calculats tangent and bitangent for normal mapping
    float3 e1 = input[1].wPos - input[0].wPos;
    float3 e2 = input[2].wPos - input[0].wPos;
    float2 dUV1 = input[1].uv - input[0].uv;
    float2 dUV2 = input[2].uv - input[0].uv;

        // 1 diveded by determinant constant
    float c = 1.0f / (dUV1.x * dUV2.y - dUV2.x * dUV1.y);

        //tangent
    float3 t;
    t.x = c * (dUV2.y * e1.x - dUV1.y * e2.x);
    t.y = c * (dUV2.y * e1.y - dUV1.y * e2.y);
    t.z = c * (dUV2.y * e1.z - dUV1.y * e2.z);
    t = normalize(t);

        //bitangent
    float3 bt;
    bt.x = c * (-dUV2.x * e1.x + dUV1.x * e2.x);
    bt.y = c * (-dUV2.x * e1.y + dUV1.x * e2.y);
    bt.z = c * (-dUV2.x * e1.z + dUV1.x * e2.z);
    bt = normalize(bt);


    for (uint i = 0; i < 3; i++)
    {
        PS_IN element;
        element.pos = input[i].pos;
        element.wPos = input[i].wPos;
        element.normal = input[i].normal;
        element.uv = input[i].uv;
        element.tangent = t;
        element.biTangent = bt;
        output.Append(element);
    }
    output.RestartStrip();
}

SamplerState sState : register(s0);
texture2D diffuseTexture : register(t0);
texture2D normalTexture : register(t1);

float4 PS(PS_IN input) : SV_Target0
{
    float3 lightDir = normalize(float3(0.3, -1, 0.3));
    float3 lightColor = 1;

    ////NORMAL MAPPING
    float3 normalSample = normalTexture.Sample(sState, input.uv);

    float3x3 tangentMatrix = float3x3(input.tangent, input.biTangent, input.normal);
    normalSample = normalize(normalSample * 2.0 - 1);


    float3 finalNormal = normalize(mul(normalSample, tangentMatrix));
    ///////////



    float3 diffuseComponent = dot(finalNormal, -lightDir) * lightColor;

    float3 color = diffuseTexture.Sample(sState, input.uv);


    return float4(color * diffuseComponent, 1);
}