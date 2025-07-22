#include "Header.hlsli"


[maxvertexcount(3)]
void main(triangle GS_INPUT input[3], inout TriangleStream<PS_INPUT> outStream)
{
    float3 center = (input[0].position + input[1].position + input[2].position) / 3.0f;

    PS_INPUT v = (PS_INPUT)0;
    
    for (int i = 0; i < 3; ++i)
    {
     
        v.position = input[i].position;
        
        v.position.x += center.x * TessellationFactor.y;
        v.position.z += center.z * TessellationFactor.y;
        
        v.position = mul(v.position, matView); 
        v.position = mul(v.position, matProj);
        
        v.uv = input[i].uv;
        
        outStream.Append(v);
    }

    outStream.RestartStrip();
}