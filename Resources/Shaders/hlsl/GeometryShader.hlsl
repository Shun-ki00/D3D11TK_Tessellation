#include "Header.hlsli"


[maxvertexcount(3)]
void main(triangle GS_INPUT input[3], inout TriangleStream<PS_INPUT> outStream)
{
    float3 center = (input[0].position + input[1].position + input[2].position) / 3.0f;
    
    // 疑似ランダムな方向をノイズなしで生成（中心座標の変換）
    float3 dir = normalize(float3(
        sin(dot(center, float3(12.9898, 78.233, 45.164))),
        cos(dot(center, float3(39.3467, 11.135, 93.873))),
        sin(dot(center, float3(73.124, 52.643, 14.753)))
    ));

    float3 offset = dir * (2.0f * 2.0f);

    PS_INPUT v = (PS_INPUT)0;
    
    for (int i = 0; i < 3; ++i)
    {
     
        v.position = input[i].position + float4(offset, 1.0f);
        
        v.position = mul(v.position, matView); 
        v.position = mul(v.position, matProj);
        
        v.uv = input[i].uv;
        
        outStream.Append(v);
    }

    outStream.RestartStrip();
}