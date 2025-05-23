// 入力構造体
struct VS_INPUT
{
    float3 position : POSITION; // 頂点位置
    float2 uv       : TEXCOORD; // UV座標
};

// 出力構造体
struct VS_OUTPUT
{
    float4 position : SV_POSITION; // クリップ空間座標
    float2 uv       : TEXCOORD;    // UV座標
};


// 入力：ハルシェーダーに渡されるパッチの頂点
struct HS_INPUT
{
    float4 position : SV_POSITION; // 頂点の位置
    float2 uv       : TEXCOORD;    // UV座標
};
// 出力：テッセレーターに渡されるデータ
struct HS_OUTPUT
{
    float4 position : SV_POSITION; // 頂点の位置
    float2 uv       : TEXCOORD;    // UV座標
};
// テッセレーション定数（エッジと内部の分割レベル）を定義する構造体
struct HS_CONSTANT_OUTPUT
{
    float EdgeTess[4]   : SV_TessFactor;       // 各エッジのテッセレーション係数
    float InsideTess[2] : SV_InsideTessFactor; // 内側のテッセレーション係数
};
// ドメインシェーダーへの入力
struct DS_INPUT
{
    float2 uvw : SV_DomainLocation; // テッセレーターで生成されたバリューパラメータ（UV空間）
};
// ドメインシェーダーの出力
struct DS_OUTPUT
{
    float4 position : SV_POSITION; // 最終的な位置（クリップ空間座標）
    float2 uv       : TEXCOORD;    // UV座標
};


struct GS_INPUT
{
    float4 position : SV_POSITION; // 最終的な位置（クリップ空間座標）
    float2 uv       : TEXCOORD; // UV座標
};


// 入力：頂点シェーダーから送られてくるデータ
struct PS_INPUT
{
    float4 position : SV_POSITION; // クリップ空間座標
    float2 uv       : TEXCOORD;    // UV座標（今回は使いませんが、必要に応じて使用可能）
};

// 定数バッファ
cbuffer ConstBuffer : register(b1)
{
    matrix matWorld;
    matrix matView;
    matrix matProj;
    float4 diffuse;
    float4 TessellationFactor; // テッセレーションの係数（細分化の度合い）
};
