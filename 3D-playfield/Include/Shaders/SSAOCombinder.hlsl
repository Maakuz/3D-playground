#include "Constants.h"


SamplerState sState : register(s0);

texture2D backBuffer : register(t0);
texture2D ssaoTexture : register(t1);
RWTexture2D<unorm float4> output : register(u0);

float3 adjustSaturation(float3 color, float saturation)
{
    // We define gray as the same color we used in the grayscale shader
    float grey = dot(color, float3(0.3, 0.59, 0.11));
   
    return lerp(grey, color, saturation);
}


[numthreads(16, 16, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
    float2 uv;
    uv.x = DTid.x / 1920.f;
    uv.y = DTid.y / 1080.f;

    float3 color = backBuffer.SampleLevel(sState, uv, 0);
    float3 ssaoColor = ssaoTexture.SampleLevel(sState, uv, 0);

    //color = adjustSaturation(color, ORIGINAL_SATURATION) * ORIGINAL_INTENSITY;
    //ssaoColor = adjustSaturation(ssaoColor, BLOOM_SATURATION) * BLOOM_INTENSITY;
    //color *= (1 - saturate(ssaoColor));

    float3 finalColor = saturate( ssaoColor);

    output[DTid.xy] = float4(finalColor, 1);
}

