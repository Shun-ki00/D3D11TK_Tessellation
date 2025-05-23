#include "Header.hlsli"


[domain("quad")] // �l�p�`�p�b�`
DS_OUTPUT main(
    DS_INPUT input, // �e�b�Z���[�^�[����̓���
    const OutputPatch<HS_OUTPUT, 4> patch, // �n���V�F�[�_�[����̐���_�f�[�^
    const HS_CONSTANT_OUTPUT patchConstants // �n���V�F�[�_�[����̃e�b�Z���[�V�����萔
)
{
    DS_OUTPUT output;
    
    // UV���W����
    float2 uv =
        patch[0].uv * (1.0f - input.uvw.x) * (1.0f - input.uvw.y) +
        patch[1].uv * input.uvw.x * (1.0f - input.uvw.y) +
        patch[2].uv * input.uvw.x * input.uvw.y +
        patch[3].uv * (1.0f - input.uvw.x) * input.uvw.y;
     
    // ����_���Ԃ��ă��[���h��Ԃ̒��_�ʒu���v�Z
    float3 worldPosition =
        patch[0].position * (1.0f - input.uvw.x) * (1.0f - input.uvw.y) +
        patch[1].position * input.uvw.x * (1.0f - input.uvw.y) +
        patch[2].position * input.uvw.x * input.uvw.y +
        patch[3].position * (1.0f - input.uvw.x) * input.uvw.y;


    //// ���[���h��� �� �N���b�v��Ԃɕϊ�
    //output.position = mul(float4(worldPosition, 1.0f), matView);
    //output.position = mul(output.position, matProj);
    
    output.position = float4(worldPosition, 1.0f);
    
    output.uv = uv;

    return output;
}