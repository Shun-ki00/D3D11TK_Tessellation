#include "pch.h"
#include "Game/Scene.h"
#include "Framework/CommonResources.h"
#include "Framework/DebugCamera.h"
#include "imgui/ImGuizmo.h"
#include "Framework/Microsoft/ReadData.h"
#include "WICTextureLoader.h"

///	<summary>
///	インプットレイアウト
///	</summary>
const std::vector<D3D11_INPUT_ELEMENT_DESC> Scene::INPUT_LAYOUT =
{
	{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,    0, sizeof(DirectX::SimpleMath::Vector3), D3D11_INPUT_PER_VERTEX_DATA, 0 },
};


/// <summary>
/// コンストラクタ
/// </summary>
Scene::Scene()
{
	// インスタンスを取得する
	m_commonResources = CommonResources::GetInstance();
	m_device          = CommonResources::GetInstance()->GetDeviceResources()->GetD3DDevice();
	m_context         = CommonResources::GetInstance()->GetDeviceResources()->GetD3DDeviceContext();
	m_commonStates    = CommonResources::GetInstance()->GetCommonStates();
}

/// <summary>
/// 初期化処理
/// </summary>
void Scene::Initialize()
{
	// カメラの作成
	m_camera = std::make_unique<DebugCamera>();
	m_camera->Initialize(1280, 720);

	m_index = 5.0f;
	m_offset = 0.0f;

	// ベーステクスチャ
	DirectX::CreateWICTextureFromFile(
		m_device, L"Resources/Texture/PlayerIcon.png", nullptr, m_texture.ReleaseAndGetAddressOf());

	// シェーダー、バッファの作成
	this->CreateShaderAndBuffer();
}

/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">経過時間</param>
void Scene::Update(const float& elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	// カメラの更新処理
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
/// 描画処理
/// </summary>
void Scene::Render()
{

	DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(1.0f) *
		DirectX::SimpleMath::Matrix::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::UnitY , DirectX::XMConvertToRadians(180.0f));

	//	シェーダーに渡す追加のバッファを作成する。(ConstBuffer）
	ConstBuffer cbuff;
	cbuff.matView = m_commonResources->GetViewMatrix().Transpose();
	cbuff.matProj = m_commonResources->GetProjectionMatrix().Transpose();
	cbuff.matWorld = world.Transpose();
	cbuff.Diffuse = DirectX::SimpleMath::Vector4(1, 1, 1, 1);
	cbuff.TessellationFactor = DirectX::SimpleMath::Vector4(m_index, m_offset, 0.0f, 0.0f);

	//	受け渡し用バッファの内容更新(ConstBufferからID3D11Bufferへの変換）
	m_context->UpdateSubresource(m_constantBuffer.Get(), 0, NULL, &cbuff, 0, 0);

	// 頂点バッファを設定
	ID3D11Buffer* buffers[] = { m_vertexBuffer.Get() };
	UINT stride[] = { sizeof(DirectX::VertexPositionTexture) };
	UINT offset[] = { 0 };
	m_context->IASetVertexBuffers(0, 1, buffers, stride, offset);


	//	シェーダーにバッファを渡す
	ID3D11Buffer* cb[1] = { m_constantBuffer.Get() };
	m_context->VSSetConstantBuffers(1, 1, cb);
	m_context->HSSetConstantBuffers(1, 1, cb);
	m_context->DSSetConstantBuffers(1, 1, cb);
	m_context->GSSetConstantBuffers(1, 1, cb);
	m_context->PSSetConstantBuffers(1, 1, cb);

	// サンプラーステートをピクセルシェーダーに設定
	ID3D11SamplerState* sampler[1] = { m_commonResources->GetCommonStates()->LinearWrap()};
	m_context->PSSetSamplers(0, 1, sampler);

	// ブレンドステートを設定 (半透明描画用)
	m_context->OMSetBlendState(m_commonStates->AlphaBlend(), nullptr, 0xFFFFFFFF);
	// 深度ステンシルステートを設定 (深度バッファの書き込みと参照)
	//context->OMSetDepthStencilState(m_depthStencilState.Get(), 0);

	// 入力レイアウトを設定
	m_context->IASetInputLayout(m_inputLayout.Get());

	// プリミティブトポロジーを設定 (ポイントリスト)
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

	//	深度バッファに書き込み参照する
	m_context->OMSetDepthStencilState(m_commonResources->GetCommonStates()->DepthDefault(), 0);

	// ラスタライザーステートの設定
	m_context->RSSetState(m_commonStates->CullNone());

	//	シェーダをセットする
	m_context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	m_context->HSSetShader(m_hullShader.Get(), nullptr, 0);
	m_context->DSSetShader(m_domainShader.Get(), nullptr, 0);
	m_context->GSSetShader(m_geometryShader.Get(), nullptr, 0);
	m_context->PSSetShader(m_pixelShader.Get(), nullptr, 0);

	// ピクセルシェーダーにテクスチャリソースを設定
	m_context->PSSetShaderResources(0, 1, m_texture.GetAddressOf());

	// 描画コール
	m_context->Draw(4, 0);

	//	シェーダの登録を解除しておく
	m_context->VSSetShader(nullptr, nullptr, 0);
	m_context->HSSetShader(nullptr, nullptr, 0);
	m_context->DSSetShader(nullptr, nullptr, 0);
	m_context->GSSetShader(nullptr, nullptr, 0);
	m_context->PSSetShader(nullptr, nullptr, 0);
}

/// <summary>
/// 終了処理
/// </summary>
void Scene::Finalize() {}


/// <summary>
/// シェーダー、バッファの作成
/// </summary>
void Scene::CreateShaderAndBuffer()
{

	// テクスチャのロード
	/*DirectX::CreateWICTextureFromFile(
		m_device, L"Resources\\Textures\\water5.png", nullptr, m_texture.ReleaseAndGetAddressOf());*/

	// シェーダを読み込むためのblob
	std::vector<uint8_t> blob;

	// 頂点シェーダをロードする
	blob = DX::ReadData(L"Resources/Shaders/cso/VertexShader.cso");
	DX::ThrowIfFailed(
		m_device->CreateVertexShader(blob.data(), blob.size(), nullptr, m_vertexShader.ReleaseAndGetAddressOf())
	);

	//	インプットレイアウトの作成
	m_device->CreateInputLayout(&INPUT_LAYOUT[0],
		static_cast<UINT>(INPUT_LAYOUT.size()),
		blob.data(), blob.size(),
		m_inputLayout.GetAddressOf());

	// ハルシェーダーをロードする
	blob = DX::ReadData(L"Resources/Shaders/cso/HullShader.cso");
	DX::ThrowIfFailed(
		m_device->CreateHullShader(blob.data(), blob.size(), nullptr, m_hullShader.ReleaseAndGetAddressOf())
	);

	// ドメインシェーダをロードする
	blob = DX::ReadData(L"Resources/Shaders/cso/DomainShader.cso");
	DX::ThrowIfFailed(
		m_device->CreateDomainShader(blob.data(), blob.size(), nullptr, m_domainShader.ReleaseAndGetAddressOf())
	);

	// ドメインシェーダをロードする
	blob = DX::ReadData(L"Resources/Shaders/cso/GeometryShader.cso");
	DX::ThrowIfFailed(
		m_device->CreateGeometryShader(blob.data(), blob.size(), nullptr, m_geometryShader.ReleaseAndGetAddressOf())
	);

	// ピクセルシェーダをロードする
	blob = DX::ReadData(L"Resources/Shaders/cso/PixelShader.cso");
	DX::ThrowIfFailed(
		m_device->CreatePixelShader(blob.data(), blob.size(), nullptr, m_pixelShader.ReleaseAndGetAddressOf())
	);


	//	シェーダーにデータを渡すためのコンスタントバッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	m_device->CreateBuffer(&bd, nullptr, &m_constantBuffer);

	// 頂点データを定義（4つの制御点）
	DirectX::VertexPositionTexture vertex[4] =
	{
		{ DirectX::XMFLOAT3(-5.0f,  0.0f, 5.0f),  DirectX::XMFLOAT2(0.0f, 0.0f) }, // 左上
		{ DirectX::XMFLOAT3(5.0f,  0.0f, 5.0f), DirectX::XMFLOAT2(1.0f, 0.0f) }, // 右上
		{ DirectX::XMFLOAT3(5.0f, 0.0f, -5.0f),  DirectX::XMFLOAT2(1.0f, 1.0f) }, // 右下
		{ DirectX::XMFLOAT3(-5.0f, 0.0f, -5.0f),  DirectX::XMFLOAT2(0.0f, 1.0f) }  // 左下
	};

	// 頂点バッファの説明
	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(DirectX::VertexPositionTexture) * _countof(vertex); // 頂点データ全体のサイズ
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;

	// 頂点データの初期化
	D3D11_SUBRESOURCE_DATA vertexData = {};
	vertexData.pSysMem = vertex;

	// 頂点バッファの作成
	m_device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
}
