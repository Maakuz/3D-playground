Texture2D inputTexture : register(t0);
RWTexture2D<unorm float4> output : register(u);

[numthreads(16, 8, 1)]
void CS( uint3 DTid : SV_DispatchThreadID )
{
    float threshold = 0.4;


    float3 sample = inputTexture.Load(DTid);


    output[DTid.xy] = float4(saturate((sample - threshold) / (1 - threshold)), 1);

}