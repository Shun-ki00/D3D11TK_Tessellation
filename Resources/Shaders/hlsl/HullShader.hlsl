#include "Header.hlsli"

// �n���V�F�[�_�[�{��
HS_CONSTANT_OUTPUT HS_ConstantFunction(InputPatch<HS_INPUT, 4> patch, uint patchID : SV_PrimitiveID)
{
    
    HS_CONSTANT_OUTPUT output;

    // �G�b�W�̍ו������x����ݒ�
    output.EdgeTess[0] = TessellationFactor.x; // ��
    output.EdgeTess[1] = TessellationFactor.x; // ��
    output.EdgeTess[2] = TessellationFactor.x; // �E
    output.EdgeTess[3] = TessellationFactor.x; // ��

    // �����̍ו������x���iU������V�����j
    output.InsideTess[0] = TessellationFactor.x; // U����
    output.InsideTess[1] = TessellationFactor.x; // V����

  
    return output;
}



// �e����_�̏o��
[domain("quad")] // �p�b�`�̌`��i"tri"�͎O�p�`�j"quad"�i�l�p�`�j"isoline"�i����j
[partitioning("integer")] // �ו����̕����i�����e�b�Z���[�V�����j"fractional_odd"�i������j"fractional_even"�i���������j
[outputtopology("triangle_cw")] // �o�̓g�|���W�[�i���v���̎O�p�`�j�C�ӂ̐��̐����l�i�ʏ��3�i�O�p�`�j�܂���4�i�l�p�`�j�j
[outputcontrolpoints(4)] // �o�͂��鐧��_�̐��i3�_�p�b�`�j
[patchconstantfunc("HS_ConstantFunction")] // �e�b�Z���[�V�����萔�v�Z�֐��̎w��
HS_OUTPUT main(InputPatch<HS_INPUT, 4> patch, uint controlPointID : SV_OutputControlPointID)
{
    HS_OUTPUT output;

    // ���͂����̂܂܏o�́i����_�����H���Ȃ��ꍇ�j
    output.position = patch[controlPointID].position;
    output.uv = patch[controlPointID].uv;

    return output;
}