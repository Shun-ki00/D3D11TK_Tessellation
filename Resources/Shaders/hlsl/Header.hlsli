// ���͍\����
struct VS_INPUT
{
    float3 position : POSITION; // ���_�ʒu
    float2 uv       : TEXCOORD; // UV���W
};

// �o�͍\����
struct VS_OUTPUT
{
    float4 position : SV_POSITION; // �N���b�v��ԍ��W
    float2 uv       : TEXCOORD;    // UV���W
};


// ���́F�n���V�F�[�_�[�ɓn�����p�b�`�̒��_
struct HS_INPUT
{
    float4 position : SV_POSITION; // ���_�̈ʒu
    float2 uv       : TEXCOORD;    // UV���W
};
// �o�́F�e�b�Z���[�^�[�ɓn�����f�[�^
struct HS_OUTPUT
{
    float4 position : SV_POSITION; // ���_�̈ʒu
    float2 uv       : TEXCOORD;    // UV���W
};
// �e�b�Z���[�V�����萔�i�G�b�W�Ɠ����̕������x���j���`����\����
struct HS_CONSTANT_OUTPUT
{
    float EdgeTess[4]   : SV_TessFactor;       // �e�G�b�W�̃e�b�Z���[�V�����W��
    float InsideTess[2] : SV_InsideTessFactor; // �����̃e�b�Z���[�V�����W��
};
// �h���C���V�F�[�_�[�ւ̓���
struct DS_INPUT
{
    float2 uvw : SV_DomainLocation; // �e�b�Z���[�^�[�Ő������ꂽ�o�����[�p�����[�^�iUV��ԁj
};
// �h���C���V�F�[�_�[�̏o��
struct DS_OUTPUT
{
    float4 position : SV_POSITION; // �ŏI�I�Ȉʒu�i�N���b�v��ԍ��W�j
    float2 uv       : TEXCOORD;    // UV���W
};


struct GS_INPUT
{
    float4 position : SV_POSITION; // �ŏI�I�Ȉʒu�i�N���b�v��ԍ��W�j
    float2 uv       : TEXCOORD; // UV���W
};


// ���́F���_�V�F�[�_�[���瑗���Ă���f�[�^
struct PS_INPUT
{
    float4 position : SV_POSITION; // �N���b�v��ԍ��W
    float2 uv       : TEXCOORD;    // UV���W�i����͎g���܂��񂪁A�K�v�ɉ����Ďg�p�\�j
};

// �萔�o�b�t�@
cbuffer ConstBuffer : register(b1)
{
    matrix matWorld;
    matrix matView;
    matrix matProj;
    float4 diffuse;
    float4 TessellationFactor; // �e�b�Z���[�V�����̌W���i�ו����̓x�����j
};
