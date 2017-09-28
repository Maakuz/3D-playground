
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

    float4x4 transform : TRANSFORM;
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
    float4 flashLightPos;
    float4 flashLightDir;
    int flashLightOn;
};

GS_IN VS(VS_IN input)
{
    GS_IN output = (GS_IN) 0;

    output.wPos = float4(input.pos, 1);

    output.wPos = mul(input.transform, output.wPos);
    output.pos = mul(VP, output.wPos);

    output.normal = normalize(mul(input.transform, float4(input.normal, 0)));
    output.uv = input.uv;

    output.transform = input.transform;

    return output;
}

//Credits to Ipren CAN YOU DO THIS BEFORE STARTING AND STORE THEM????
[maxvertexcount(3)]
void GS(triangle GS_IN input[3] : SV_POSITION, inout TriangleStream<PS_IN> output)
{
     //calculats tangent and bitangent for normal mapping
    float3 e0 = input[1].wPos - input[0].wPos;
    float3 e1 = input[2].wPos - input[0].wPos;
    float2 dUV0 = input[1].uv - input[0].uv;
    float2 dUV1 = input[2].uv - input[0].uv;

        // 1 diveded by determinant constant
    float c = 1.0f / (dUV0.x * dUV1.y - dUV1.x * dUV0.y);

        //tangent
    float3 t;
    t.x = c * (dUV1.y * e0.x - dUV0.y * e1.x);
    t.y = c * (dUV1.y * e0.y - dUV0.y * e1.y);
    t.z = c * (dUV1.y * e0.z - dUV0.y * e1.z);
    t = normalize(t);

        //bitangent
    float3 bt;
    bt.x = c * (-dUV1.x * e0.x + dUV0.x * e1.x);
    bt.y = c * (-dUV1.x * e0.y + dUV0.x * e1.y);
    bt.z = c * (-dUV1.x * e0.z + dUV0.x * e1.z);
    bt = normalize(bt);


    for (uint i = 0; i < 3; i++)
    {
        PS_IN element;
        element.wPos = input[i].wPos;
        element.pos = input[i].pos;
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
texture2D specularTexture : register(t2);

float3 sampleNormal(PS_IN input)
{
    float3 normalSample = normalTexture.Sample(sState, input.uv);
    
    normalSample = normalize(normalSample * 2.0 - 1);

    input.tangent = normalize(input.tangent - dot(input.tangent, input.normal) * input.normal);

    float3x3 tangentMatrix = float3x3(input.tangent, input.biTangent, input.normal);


    return normalize(mul(normalSample, tangentMatrix));
}

float4 PS(PS_IN input) : SV_Target0
{
    float3 finalNormal = sampleNormal(input);
    
    float3 lightDir = normalize(float3(0, -0.5, 0.5));
    float3 lightColor = 1;
    
    float3 directionalDiffuse = saturate(dot(finalNormal, -lightDir)) * lightColor;

    float3 reflectThingDir = normalize(-lightDir + (camPos.xyz - input.wPos.xyz));
    float3 directionalSpecularity = pow(saturate(dot(finalNormal, reflectThingDir)), 500) * lightColor;


    ///FlashLight
    float outerCone = 0.8f;
    float innerCone = 0.95f;
    float innerMinusOuter = innerCone - outerCone;
    float3 flashLightToPos = input.wPos.xyz - flashLightPos.xyz;


    float cosAngle = dot(normalize(flashLightToPos), normalize(flashLightDir.xyz));

    float falloff = flashLightOn ? saturate((cosAngle - outerCone) / innerMinusOuter) : 0;

    //Test both and see difference
    //float attenuation = saturate(1.f / (0.01 * max(20, pow(length(flashLightToPos.xyz), 2))));
    float attenuation = smoothstep(40, 0, length(flashLightToPos));

    float3 flashLightDiffuseComponent = dot(finalNormal, normalize(-flashLightToPos)) * attenuation * falloff;
    flashLightDiffuseComponent = saturate(flashLightDiffuseComponent);

    float3 diffuseSample = diffuseTexture.Sample(sState, input.uv);
    float3 specularSample = specularTexture.Sample(sState, input.uv);

    float3 totalDiffuse = saturate(directionalDiffuse + flashLightDiffuseComponent) * diffuseSample;
    directionalSpecularity *= specularSample;

    

    float3 totalIllumination = totalDiffuse + directionalSpecularity;

    //return float4(diffuseComponent, 1);
    return float4(totalIllumination, 1);
}