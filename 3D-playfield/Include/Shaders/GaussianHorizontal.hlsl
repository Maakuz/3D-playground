#include "Constants.h"


Texture2D inputTexture : register(t0);
RWTexture2D<unorm float4> output: register(u0);

[numthreads(16, 16, 1)]
void CS( uint3 DTid : SV_DispatchThreadID )
{

    int3 textureLocation = DTid * 2 - int3(KERNEL_SIZE -1, 0, 0);

    float4 final = { 0.0, 0.0, 0.0, 1.0 };

    for (uint x = 0; x < KERNEL_SIZE; x++)
        final += inputTexture.Load(textureLocation + int3(x * 2, 0, 0)) * gaussianFilter[x];


    output[DTid.xy] = final;
}