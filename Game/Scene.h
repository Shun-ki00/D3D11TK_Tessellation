#pragma once
#include <future>
#include "Framework/DebugCamera.h"

class CommonResources;
class DebugCamera;

class Scene
{
private:

	// インプットレイアウト
	static const std::vector<D3D11_INPUT_ELEMENT_DESC> INPUT_LAYOUT;

	//	データ受け渡し用コンスタントバッファ(送信側)
	struct ConstBuffer
	{
		DirectX::SimpleMath::Matrix		matWorld;
		DirectX::SimpleMath::Matrix		matView;
		DirectX::SimpleMath::Matrix		matProj;
		DirectX::SimpleMath::Vector4	Diffuse;
		DirectX::SimpleMath::Vector4    TessellationFactor;
	};

public:

	// コンストラクタ
	Scene();
	// デストラクタ
	~Scene() = default;

public:

	// 初期化処理
	void Initialize();
	// 更新処理
	void Update(const float& elapsedTime);
	// 描画処理
	void Render();
	// 終了処理
	void Finalize();

private:

	// シェーダー、バッファの作成
	void CreateShaderAndBuffer();
	// ブレンドステートの作成
	void CreateBlendState();
	// 深度ステンシルステートの作成
	void CreateDepthStencilState();
	// ラスタライザーステートの作成
	void CreateRasterizerState();



private:

	// 共有リソース
	CommonResources* m_commonResources;

	// デバッグカメラ
	std::unique_ptr<DebugCamera> m_camera;

	// デバイス
	ID3D11Device1* m_device;
	// コンテキスト
	ID3D11DeviceContext1* m_context;
	

	// 頂点シェーダー
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	// ハルシェーダー
	Microsoft::WRL::ComPtr<ID3D11HullShader> m_hullShader;
	// ドメインシェーダー
	Microsoft::WRL::ComPtr<ID3D11DomainShader> m_domainShader;
	// ジオメトリシェーダー
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_geometryShader;
	// ピクセルシェーダー
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;

	// 入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	// 定数バッファ用のバッファオブジェクト
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;
	// 頂点バッファ用のバッファオブジェクト
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;

	
	// ブレンドステート
	Microsoft::WRL::ComPtr<ID3D11BlendState> m_blendState;
	// 深度ステンシルステート
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;
	// ラスタライザーステート
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerState;

	// テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
};