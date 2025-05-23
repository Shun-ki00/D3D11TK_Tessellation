#include "pch.h"
#include "Game/Scene.h"
#include "Framework/CommonResources.h"
#include "Framework/DebugCamera.h"
#include "imgui/ImGuizmo.h"
#include "Framework/Microsoft/ReadData.h"
#include "WICTextureLoader.h"

///	<summary>
///	�C���v�b�g���C�A�E�g
///	</summary>
const std::vector<D3D11_INPUT_ELEMENT_DESC> Scene::INPUT_LAYOUT =
{
	{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,    0, sizeof(DirectX::SimpleMath::Vector3), D3D11_INPUT_PER_VERTEX_DATA, 0 },
};


/// <summary>
/// �R���X�g���N�^
/// </summary>
Scene::Scene()
{
	// �C���X�^���X���擾����
	m_commonResources = CommonResources::GetInstance();
	m_device          = CommonResources::GetInstance()->GetDeviceResources()->GetD3DDevice();
	m_context         = CommonResources::GetInstance()->GetDeviceResources()->GetD3DDeviceContext();
}

/// <summary>
/// ����������
/// </summary>
void Scene::Initialize()
{
	// �J�����̍쐬
	m_camera = std::make_unique<DebugCamera>();
	m_camera->Initialize(1280, 720);

	// �V�F�[�_�[�A�o�b�t�@�̍쐬
	this->CreateShaderAndBuffer();
	// �u�����h�X�e�[�g�̍쐬
	this->CreateBlendState();
	// �[�x�X�e���V���X�e�[�g�̍쐬
	this->CreateDepthStencilState();
	// ���X�^���C�U�[�X�e�[�g�̍쐬
	this->CreateRasterizerState();
}

/// <summary>
/// �X�V����
/// </summary>
/// <param name="elapsedTime">�o�ߎ���</param>
void Scene::Update(const float& elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	// �J�����̍X�V����
	auto io = ImGui::GetIO();
	if (!io.WantCaptureMouse)
	{
		
	}

	m_camera->Update();
	m_commonResources->SetViewMatrix(m_camera->GetViewMatrix());

}

/// <summary>
/// �`�揈��
/// </summary>
void Scene::Render()
{

	DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(2.0f);

	//	�V�F�[�_�[�ɓn���ǉ��̃o�b�t�@���쐬����B(ConstBuffer�j
	ConstBuffer cbuff;
	cbuff.matView = m_commonResources->GetViewMatrix().Transpose();
	cbuff.matProj = m_commonResources->GetProjectionMatrix().Transpose();
	cbuff.matWorld = world.Transpose();
	cbuff.Diffuse = DirectX::SimpleMath::Vector4(1, 1, 1, 1);
	cbuff.TessellationFactor = DirectX::SimpleMath::Vector4(5.0f, 0.0f, 0.0f, 0.0f);

	//	�󂯓n���p�o�b�t�@�̓��e�X�V(ConstBuffer����ID3D11Buffer�ւ̕ϊ��j
	m_context->UpdateSubresource(m_constantBuffer.Get(), 0, NULL, &cbuff, 0, 0);

	// ���_�o�b�t�@��ݒ�
	ID3D11Buffer* buffers[] = { m_vertexBuffer.Get() };
	UINT stride[] = { sizeof(DirectX::VertexPositionTexture) };
	UINT offset[] = { 0 };
	m_context->IASetVertexBuffers(0, 1, buffers, stride, offset);


	//	�V�F�[�_�[�Ƀo�b�t�@��n��
	ID3D11Buffer* cb[1] = { m_constantBuffer.Get() };
	m_context->VSSetConstantBuffers(1, 1, cb);
	m_context->HSSetConstantBuffers(1, 1, cb);
	m_context->DSSetConstantBuffers(1, 1, cb);
	m_context->GSSetConstantBuffers(1, 1, cb);
	m_context->PSSetConstantBuffers(1, 1, cb);

	// �T���v���[�X�e�[�g���s�N�Z���V�F�[�_�[�ɐݒ�
	ID3D11SamplerState* sampler[1] = { m_commonResources->GetCommonStates()->LinearWrap()};
	m_context->PSSetSamplers(0, 1, sampler);

	// �u�����h�X�e�[�g��ݒ� (�������`��p)
	m_context->OMSetBlendState(m_blendState.Get(), nullptr, 0xFFFFFFFF);
	// �[�x�X�e���V���X�e�[�g��ݒ� (�[�x�o�b�t�@�̏������݂ƎQ��)
	//context->OMSetDepthStencilState(m_depthStencilState.Get(), 0);

	// ���̓��C�A�E�g��ݒ�
	m_context->IASetInputLayout(m_inputLayout.Get());

	// �v���~�e�B�u�g�|���W�[��ݒ� (�|�C���g���X�g)
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

	//	�[�x�o�b�t�@�ɏ������ݎQ�Ƃ���
	m_context->OMSetDepthStencilState(m_commonResources->GetCommonStates()->DepthDefault(), 0);

	// ���X�^���C�U�[�X�e�[�g�̐ݒ�
	m_context->RSSetState(m_rasterizerState.Get());

	//	�V�F�[�_���Z�b�g����
	m_context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	m_context->HSSetShader(m_hullShader.Get(), nullptr, 0);
	m_context->DSSetShader(m_domainShader.Get(), nullptr, 0);
	m_context->GSSetShader(m_geometryShader.Get(), nullptr, 0);
	m_context->PSSetShader(m_pixelShader.Get(), nullptr, 0);

	// �s�N�Z���V�F�[�_�[�Ƀe�N�X�`�����\�[�X��ݒ�
	//m_context->PSSetShaderResources(0, 1, m_texture.GetAddressOf());

	// �`��R�[��
	m_context->Draw(4, 0);

	//	�V�F�[�_�̓o�^���������Ă���
	m_context->VSSetShader(nullptr, nullptr, 0);
	m_context->HSSetShader(nullptr, nullptr, 0);
	m_context->DSSetShader(nullptr, nullptr, 0);
	m_context->GSSetShader(nullptr, nullptr, 0);
	m_context->PSSetShader(nullptr, nullptr, 0);
}

/// <summary>
/// �I������
/// </summary>
void Scene::Finalize() {}


/// <summary>
/// �V�F�[�_�[�A�o�b�t�@�̍쐬
/// </summary>
void Scene::CreateShaderAndBuffer()
{

	// �e�N�X�`���̃��[�h
	/*DirectX::CreateWICTextureFromFile(
		m_device, L"Resources\\Textures\\water5.png", nullptr, m_texture.ReleaseAndGetAddressOf());*/

	// �V�F�[�_��ǂݍ��ނ��߂�blob
	std::vector<uint8_t> blob;

	// ���_�V�F�[�_�����[�h����
	blob = DX::ReadData(L"Resources/Shaders/cso/VertexShader.cso");
	DX::ThrowIfFailed(
		m_device->CreateVertexShader(blob.data(), blob.size(), nullptr, m_vertexShader.ReleaseAndGetAddressOf())
	);

	//	�C���v�b�g���C�A�E�g�̍쐬
	m_device->CreateInputLayout(&INPUT_LAYOUT[0],
		static_cast<UINT>(INPUT_LAYOUT.size()),
		blob.data(), blob.size(),
		m_inputLayout.GetAddressOf());

	// �n���V�F�[�_�[�����[�h����
	blob = DX::ReadData(L"Resources/Shaders/cso/HullShader.cso");
	DX::ThrowIfFailed(
		m_device->CreateHullShader(blob.data(), blob.size(), nullptr, m_hullShader.ReleaseAndGetAddressOf())
	);

	// �h���C���V�F�[�_�����[�h����
	blob = DX::ReadData(L"Resources/Shaders/cso/DomainShader.cso");
	DX::ThrowIfFailed(
		m_device->CreateDomainShader(blob.data(), blob.size(), nullptr, m_domainShader.ReleaseAndGetAddressOf())
	);

	// �h���C���V�F�[�_�����[�h����
	blob = DX::ReadData(L"Resources/Shaders/cso/GeometryShader.cso");
	DX::ThrowIfFailed(
		m_device->CreateGeometryShader(blob.data(), blob.size(), nullptr, m_geometryShader.ReleaseAndGetAddressOf())
	);

	// �s�N�Z���V�F�[�_�����[�h����
	blob = DX::ReadData(L"Resources/Shaders/cso/PixelShader.cso");
	DX::ThrowIfFailed(
		m_device->CreatePixelShader(blob.data(), blob.size(), nullptr, m_pixelShader.ReleaseAndGetAddressOf())
	);


	//	�V�F�[�_�[�Ƀf�[�^��n�����߂̃R���X�^���g�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	m_device->CreateBuffer(&bd, nullptr, &m_constantBuffer);

	// ���_�f�[�^���`�i4�̐���_�j
	DirectX::VertexPositionTexture vertex[4] =
	{
		{ DirectX::XMFLOAT3(-5.0f,  0.0f, 5.0f),  DirectX::XMFLOAT2(0.0f, 0.0f) }, // ����
		{ DirectX::XMFLOAT3(5.0f,  0.0f, 5.0f), DirectX::XMFLOAT2(1.0f, 0.0f) }, // �E��
		{ DirectX::XMFLOAT3(5.0f, 0.0f, -5.0f),  DirectX::XMFLOAT2(1.0f, 1.0f) }, // �E��
		{ DirectX::XMFLOAT3(-5.0f, 0.0f, -5.0f),  DirectX::XMFLOAT2(0.0f, 1.0f) }  // ����
	};

	// ���_�o�b�t�@�̐���
	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(DirectX::VertexPositionTexture) * _countof(vertex); // ���_�f�[�^�S�̂̃T�C�Y
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;

	// ���_�f�[�^�̏�����
	D3D11_SUBRESOURCE_DATA vertexData = {};
	vertexData.pSysMem = vertex;

	// ���_�o�b�t�@�̍쐬
	m_device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
}


/// <summary>
/// �u�����h�X�e�[�g�̍쐬
/// </summary>
void Scene::CreateBlendState()
{
	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = FALSE;  // �J�o���b�W���A���t�@�Ɋ�Â��ėL��������
	blendDesc.IndependentBlendEnable = FALSE; // �����̃����_�[�^�[�Q�b�g��Ɨ����Đݒ肷��

	D3D11_RENDER_TARGET_BLEND_DESC rtBlendDesc = {};
	rtBlendDesc.BlendEnable = TRUE;              // �u�����h��L����
	rtBlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;        // �\�[�X�̃A���t�@
	rtBlendDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;    // �t�A���t�@
	rtBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;           // ���Z�u�����h
	rtBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;              // �A���t�@�l�̃\�[�X
	rtBlendDesc.DestBlendAlpha = D3D11_BLEND_ZERO;             // �A���t�@�l�̃f�X�e�B�l�[�V����
	rtBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;           // �A���t�@�l�̉��Z
	rtBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL; // RGBA�S�Ă�L��

	blendDesc.RenderTarget[0] = rtBlendDesc;

	// �u�����h�X�e�[�g���쐬
	m_device->CreateBlendState(&blendDesc, &m_blendState);
}

/// <summary>
/// �[�x�X�e���V���X�e�[�g�̍쐬
/// </summary>
void Scene::CreateDepthStencilState()
{
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = TRUE;                          // �[�x�e�X�g��L����
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; // �[�x�o�b�t�@�̏������݂�L����
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;      // �[�x�e�X�g���� (�������ꍇ�̂ݕ`��)
	depthStencilDesc.StencilEnable = FALSE;                       // �X�e���V���e�X�g�𖳌���

	// �[�x�X�e���V���X�e�[�g���쐬
	m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
}

/// <summary>
/// ���X�^���C�U�[�X�e�[�g�̍쐬
/// </summary>
void Scene::CreateRasterizerState()
{
	D3D11_RASTERIZER_DESC rasterDesc = {};
	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;      // �h��Ԃ� (�܂��� D3D11_FILL_WIREFRAME)D3D11_FILL_SOLID
	rasterDesc.CullMode = D3D11_CULL_NONE; // �J�����O�Ȃ� (�܂��� D3D11_CULL_FRONT / D3D11_CULL_BACK)
	rasterDesc.FrontCounterClockwise = FALSE;    // ���v���̒��_������\�ʂƂ��ĔF��
	rasterDesc.DepthClipEnable = TRUE;           // �[�x�N���b�s���O��L����

	// ���X�^���C�U�[�X�e�[�g�̍쐬
	m_device->CreateRasterizerState(&rasterDesc, &m_rasterizerState);
}