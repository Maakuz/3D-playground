#include "Constants.h"

Texture2D inputTexture : register(t0);
RWTexture2D<unorm float4> output : register(u0);

[numthreads(16, 16, 1)]
void CS( uint3 DTid : SV_DispatchThreadID )
{



    float3 sample = inputTexture.Load(DTid);


    output[DTid.xy] = float4(saturate((sample - BLOOM_THRESHOLD) / (1 - BLOOM_THRESHOLD)), 1);

}