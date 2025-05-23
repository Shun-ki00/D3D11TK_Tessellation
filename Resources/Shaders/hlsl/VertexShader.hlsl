#include "Header.hlsli"

// ���_�V�F�[�_�[�{��
// ���_�V�F�[�_�[����n���V�F�[�_�[��
VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;

   // ���[�J����� �� ���[���h���
    float4 worldPos = mul(float4(input.position, 1.0f), matWorld);
    
    // ���[���h��� �� �r���[��� �� �N���b�v���
    output.position = mul(worldPos, matView);
    output.position = mul(output.position, matProj);

    
    //output.position = float4(input.position, 1.0f);
    
    // ���[�J����� �� ���[���h���
    output.position = mul(float4(input.position, 1.0f), matWorld);
    // ���_�F��UV���W�����̂܂ܓn��
    output.uv = input.uv;

    return output;
}