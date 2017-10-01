
Texture2D positionTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D randomNormalTexture : register(t2);
RWTexture2D<unorm float4> output: register(u0);

#define RANDOM_TEXTURE_DIMENSION 64

//playing values
#define sampleRadius 0.5
#define intensity 1
#define scale 1
#define bias  0.4f

float doAmbientOcclusion(in float2 tcoord, in float2 uv, in float3 pos, in float3 normal)
{
    float3 diff = positionTexture.Load(int3(tcoord + uv, 0)).xyz - pos;
    const float3 direction = normalize(diff);
    const float distance = length(diff) * scale;
    float nDotD = dot(normal, direction);
    return max(0.0, nDotD - bias) * (1.0 / (1.0 + distance)) * intensity;
}

[numthreads(16, 16, 1)]
void CS( uint3 DTid : SV_DispatchThreadID )
{
    float4 color = 1;
    const float2 vectors[4] = {1,0,    -1,0,    0,1,    0,-1 };
    float3 pos = positionTexture.Load(int3(DTid.xy, 0)).xyz;
    float3 normal = normalize(normalTexture.Load(int3(DTid.xy, 0)) * 2.0f - 1.0f);
    float2 randomNormal = normalize(randomNormalTexture.Load(int3(DTid.xy / RANDOM_TEXTURE_DIMENSION, 0)).xy * 2.0f - 1.0f);
    float occlusion = 0;

    float radius = sampleRadius / pos.z;
    float magicValue = 0.707;

    int iterations = 4;
    for (int i = 0; i < iterations; i++)
    {
        float2 coord0 = reflect(vectors[i], randomNormal.xy)*radius;
        float2 coord1 = float2(
        coord0.x * magicValue - coord0.y * magicValue,
        coord0.x * magicValue + coord0.y * magicValue);

        occlusion += doAmbientOcclusion(DTid.xy, coord0*0.25, pos, normal);
        occlusion += doAmbientOcclusion(DTid.xy, coord1*0.5, pos, normal);
        occlusion += doAmbientOcclusion(DTid.xy, coord0*0.75, pos, normal);
        occlusion += doAmbientOcclusion(DTid.xy, coord1, pos, normal);
    }

    occlusion /= (float)iterations * 4.0;

    output[DTid.xy] = occlusion;
}