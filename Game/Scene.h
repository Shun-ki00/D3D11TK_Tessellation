#pragma once
#include <future>
#include "Framework/DebugCamera.h"

class CommonResources;
class DebugCamera;

class Scene
{
private:

	// �C���v�b�g���C�A�E�g
	static const std::vector<D3D11_INPUT_ELEMENT_DESC> INPUT_LAYOUT;

	//	�f�[�^�󂯓n���p�R���X�^���g�o�b�t�@(���M��)
	struct ConstBuffer
	{
		DirectX::SimpleMath::Matrix		matWorld;
		DirectX::SimpleMath::Matrix		matView;
		DirectX::SimpleMath::Matrix		matProj;
		DirectX::SimpleMath::Vector4	Diffuse;
		DirectX::SimpleMath::Vector4    TessellationFactor;
	};

public:

	// �R���X�g���N�^
	Scene();
	// �f�X�g���N�^
	~Scene() = default;

public:

	// ����������
	void Initialize();
	// �X�V����
	void Update(const float& elapsedTime);
	// �`�揈��
	void Render();
	// �I������
	void Finalize();

private:

	// �V�F�[�_�[�A�o�b�t�@�̍쐬
	void CreateShaderAndBuffer();
	// �u�����h�X�e�[�g�̍쐬
	void CreateBlendState();
	// �[�x�X�e���V���X�e�[�g�̍쐬
	void CreateDepthStencilState();
	// ���X�^���C�U�[�X�e�[�g�̍쐬
	void CreateRasterizerState();



private:

	// ���L���\�[�X
	CommonResources* m_commonResources;

	// �f�o�b�O�J����
	std::unique_ptr<DebugCamera> m_camera;

	// �f�o�C�X
	ID3D11Device1* m_device;
	// �R���e�L�X�g
	ID3D11DeviceContext1* m_context;
	

	// ���_�V�F�[�_�[
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	// �n���V�F�[�_�[
	Microsoft::WRL::ComPtr<ID3D11HullShader> m_hullShader;
	// �h���C���V�F�[�_�[
	Microsoft::WRL::ComPtr<ID3D11DomainShader> m_domainShader;
	// �W�I���g���V�F�[�_�[
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_geometryShader;
	// �s�N�Z���V�F�[�_�[
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;

	// ���̓��C�A�E�g
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	// �萔�o�b�t�@�p�̃o�b�t�@�I�u�W�F�N�g
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;
	// ���_�o�b�t�@�p�̃o�b�t�@�I�u�W�F�N�g
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;

	
	// �u�����h�X�e�[�g
	Microsoft::WRL::ComPtr<ID3D11BlendState> m_blendState;
	// �[�x�X�e���V���X�e�[�g
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;
	// ���X�^���C�U�[�X�e�[�g
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerState;

	// �e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
};