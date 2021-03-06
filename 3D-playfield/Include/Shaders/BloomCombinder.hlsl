#include "Constants.h"


SamplerState sState : register(s0);

texture2D backBuffer : register(t0);
texture2D bloomTexture : register(t1);
RWTexture2D<unorm float4> output : register(u0);

float3 adjustSaturation(float3 color, float saturation)
{
    float grey = dot(color, float3(0.3, 0.59, 0.11));
   
    return lerp(grey, color, saturation);
}


[numthreads(16, 16, 1)]
void CS( uint3 DTid : SV_DispatchThreadID )
{
    float2 uv;
    uv.x = DTid.x / 1920.f;
    uv.y = DTid.y / 1080.f;

    float3 color = backBuffer.SampleLevel(sState, uv, 0);
    float3 bloomColor = bloomTexture.SampleLevel(sState, uv, 0);

    color = adjustSaturation(color, ORIGINAL_SATURATION) * ORIGINAL_INTENSITY;
    bloomColor = adjustSaturation(bloomColor, BLOOM_SATURATION) * BLOOM_INTENSITY;
    color *= (1 - saturate(bloomColor));

    float3 finalColor = saturate(color + bloomColor);

    output[DTid.xy] = float4(finalColor, 1);
}

