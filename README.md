# D3D11TK_Tessellation
<img width="1277" height="721" alt="スクリーンショット 2025-07-22 103849" src="https://github.com/user-attachments/assets/4340312b-25ac-43da-9184-3ca8c7bc621a" />

## 概要

このプロジェクトは、**Direct3D 11 の全シェーダーステージ**（頂点・ハル・ドメイン・ジオメトリ・ピクセル）を使用したテッセレーションのサンプルプログラムです。

`Scene` クラス一つで全ての描画処理を管理しており、**各シェーダーステージの使い方や連携方法を把握しやすい構成**になっています。

---

## 特徴

- 頂点シェーダー（VS）
- ハルシェーダー（HS）
- ドメインシェーダー（DS）
- ジオメトリシェーダー（GS）
- ピクセルシェーダー（PS）

すべてのシェーダーステージを **一つの描画パスに統合**して動作。

---

## デモ実行ファイル
最新版の実行ファイルはこちらからダウンロードできます： [Download D3D11TK_Tessellation v1.0.0](https://github.com/Shun-ki00/D3D11TK_Tessellation/releases/latest)


## 動作環境
・OS：Windows 10 / 11  
・GPU : Direct3D 11対応グラフィックカード  
・開発環境：Visual Studio 2022  
・ビルド対象：x64  

## 使用ライブラリ
・Direct3D 11  
・DirectXTK  
・Windows SDK   
