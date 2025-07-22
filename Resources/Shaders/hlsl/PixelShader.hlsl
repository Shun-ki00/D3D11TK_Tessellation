#include "Header.hlsli"

// �e�N�X�`��
Texture2D tex : register(t0);
// �T���v���[
SamplerState sam : register(s0);

// �s�N�Z���V�F�[�_�[�{��
float4 main(PS_INPUT input) : SV_TARGET
{ 
    float output = float4(1.0f, 1.0f, 1.0f, 1.0f);
    
    float4 texColor = tex.Sample(sam, input.uv);
   
    // ���F�iRGBA�j���o��
    return texColor; // R, G, B, A = 1.0 (���F)
}