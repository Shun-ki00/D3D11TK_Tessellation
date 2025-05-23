#include "Header.hlsli"


[domain("quad")] // 四角形パッチ
DS_OUTPUT main(
    DS_INPUT input, // テッセレーターからの入力
    const OutputPatch<HS_OUTPUT, 4> patch, // ハルシェーダーからの制御点データ
    const HS_CONSTANT_OUTPUT patchConstants // ハルシェーダーからのテッセレーション定数
)
{
    DS_OUTPUT output;
    
    // UV座標を補間
    float2 uv =
        patch[0].uv * (1.0f - input.uvw.x) * (1.0f - input.uvw.y) +
        patch[1].uv * input.uvw.x * (1.0f - input.uvw.y) +
        patch[2].uv * input.uvw.x * input.uvw.y +
        patch[3].uv * (1.0f - input.uvw.x) * input.uvw.y;
     
    // 制御点を補間してワールド空間の頂点位置を計算
    float3 worldPosition =
        patch[0].position * (1.0f - input.uvw.x) * (1.0f - input.uvw.y) +
        patch[1].position * input.uvw.x * (1.0f - input.uvw.y) +
        patch[2].position * input.uvw.x * input.uvw.y +
        patch[3].position * (1.0f - input.uvw.x) * input.uvw.y;


    //// ワールド空間 → クリップ空間に変換
    //output.position = mul(float4(worldPosition, 1.0f), matView);
    //output.position = mul(output.position, matProj);
    
    output.position = float4(worldPosition, 1.0f);
    
    output.uv = uv;

    return output;
}