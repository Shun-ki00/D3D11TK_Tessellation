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
	m_commonStates    = CommonResources::GetInstance()->GetCommonStates();
}

/// <summary>
/// ����������
/// </summary>
void Scene::Initialize()
{
	// �J�����̍쐬
	m_camera = std::make_unique<DebugCamera>();
	m_camera->Initialize(1280, 720);

	m_index = 5.0f;
	m_offset = 0.0f;

	// �x�[�X�e�N�X�`��
	DirectX::CreateWICTextureFromFile(
		m_device, L"Resources/Texture/PlayerIcon.png", nullptr, m_texture.ReleaseAndGetAddressOf());

	// �V�F�[�_�[�A�o�b�t�@�̍쐬
	this->CreateShaderAndBuffer();
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
		m_camera->Update();
		m_commonResources->SetViewMatrix(m_camera->GetViewMatrix());
	}

	if (InputManager::GetInstance()->OnKeyDown(DirectX::Keyboard::Up))
	{
		m_index++;
	}
	else if (InputManager::GetInstance()->OnKeyDown(DirectX::Keyboard::Down))
	{
		m_index--;
	}

	if (InputManager::GetInstance()->OnKey(DirectX::Keyboard::Right))
	{
		m_offset += 0.5f * elapsedTime;
	}
	else if (InputManager::GetInstance()->OnKey(DirectX::Keyboard::Left))
	{
		m_offset -= 0.5f * elapsedTime;
	}
}

/// <summary>
/// �`�揈��
/// </summary>
void Scene::Render()
{

	DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(1.0f) *
		DirectX::SimpleMath::Matrix::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::UnitY , DirectX::XMConvertToRadians(180.0f));

	//	�V�F�[�_�[�ɓn���ǉ��̃o�b�t�@���쐬����B(ConstBuffer�j
	ConstBuffer cbuff;
	cbuff.matView = m_commonResources->GetViewMatrix().Transpose();
	cbuff.matProj = m_commonResources->GetProjectionMatrix().Transpose();
	cbuff.matWorld = world.Transpose();
	cbuff.Diffuse = DirectX::SimpleMath::Vector4(1, 1, 1, 1);
	cbuff.TessellationFactor = DirectX::SimpleMath::Vector4(m_index, m_offset, 0.0f, 0.0f);

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
	m_context->OMSetBlendState(m_commonStates->AlphaBlend(), nullptr, 0xFFFFFFFF);
	// �[�x�X�e���V���X�e�[�g��ݒ� (�[�x�o�b�t�@�̏������݂ƎQ��)
	//context->OMSetDepthStencilState(m_depthStencilState.Get(), 0);

	// ���̓��C�A�E�g��ݒ�
	m_context->IASetInputLayout(m_inputLayout.Get());

	// �v���~�e�B�u�g�|���W�[��ݒ� (�|�C���g���X�g)
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

	//	�[�x�o�b�t�@�ɏ������ݎQ�Ƃ���
	m_context->OMSetDepthStencilState(m_commonResources->GetCommonStates()->DepthDefault(), 0);

	// ���X�^���C�U�[�X�e�[�g�̐ݒ�
	m_context->RSSetState(m_commonStates->CullNone());

	//	�V�F�[�_���Z�b�g����
	m_context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	m_context->HSSetShader(m_hullShader.Get(), nullptr, 0);
	m_context->DSSetShader(m_domainShader.Get(), nullptr, 0);
	m_context->GSSetShader(m_geometryShader.Get(), nullptr, 0);
	m_context->PSSetShader(m_pixelShader.Get(), nullptr, 0);

	// �s�N�Z���V�F�[�_�[�Ƀe�N�X�`�����\�[�X��ݒ�
	m_context->PSSetShaderResources(0, 1, m_texture.GetAddressOf());

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
