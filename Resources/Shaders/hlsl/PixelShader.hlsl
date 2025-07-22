#include "Header.hlsli"

// テクスチャ
Texture2D tex : register(t0);
// サンプラー
SamplerState sam : register(s0);

// ピクセルシェーダー本体
float4 main(PS_INPUT input) : SV_TARGET
{ 
    float output = float4(1.0f, 1.0f, 1.0f, 1.0f);
    
    float4 texColor = tex.Sample(sam, input.uv);
   
    // 白色（RGBA）を出力
    return texColor; // R, G, B, A = 1.0 (白色)
}