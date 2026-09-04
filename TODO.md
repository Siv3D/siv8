# Implementation TODO

実装を保留している API や、設計判断が必要な項目を記録する作業補助ファイルです。

- 公開ヘッダごとに、関連する項目をトピック単位でまとめます。
- 実装に着手できるよう、保留理由と決めるべき契約を記録します。
- 実装とテストが完了したトピックは、このファイルから削除します。
- 完了項目の履歴は残さず、未完了項目だけを維持します。

## `Siv3D/include/Siv3D/Quaternion.hpp`

### Squad 補間

- `squad()` と制御点生成 API の引数構成を設計する。
- 毎回制御点を再計算せず、補間区間ごとに制御点を再利用できる形にする。
- DirectXMath の `XMQuaternionSquad()` と `XMQuaternionSquadSetup()` の利用を前提にする。
- 単位クォータニオンの事前条件、符号の選択、補間係数をクランプするかを明文化する。
- 出力参照形式にするか、制御点を保持する専用型を導入するかを決める。

## `Siv3D/include/Siv3D/Mat4x4.hpp`

### 値返しの行列分解

- `decompose()` の結果を値で返す API を検討する。
- scale, rotation, translation と成功・失敗を表現する戻り値用の型を設計する。
- 既存の出力参照形式を canonical implementation とするかを決める。

### 法線変換

- 線形部分の逆転置による法線変換 API を設計する。
- 特異行列の場合の結果と、変換後に正規化するかを決める。
- 単一法線とストリーム変換の両方が必要かを検討する。

### View / Projection 行列

- Look-at、透視投影、正射影の生成 API を設計する。
- 左手・右手座標系、深度範囲、Reverse-Z の規約を Camera3D と合わせて決める。

### 平面に対する反射

- `InfinitePlane` の実装後に `Reflect()` の追加を検討する。
- 平面の正規化を事前条件にするか、関数内で正規化するかを決める。

### 平面への影

- 点光源と平行光源を区別する `Shadow()` API を設計する。
- DirectXMath に渡す光源ベクトルの w 成分を、点光源では 1、平行光源では 0 とする契約を明文化する。

## `Siv3D/include/Siv3D/Mesh3D.hpp`

### 優先する生成 API

1. 経路点ごとに半径を指定する Variable Tube を追加する。
   - `radii.size() == path.size()` を基本契約とし、開路・閉路の両方を扱う。
   - 既存 Tube の parallel-transport frame、UV、端面、失敗時の非変更保証を維持する。
   - `Mesh3DBuilder` の offset、offset + rotation、`Mat4x4` overload も同時に設計する。
2. Sweep の断面 scale と回転を経路点ごとに指定する Variable Sweep を設計する。
   - `Vec2 scale` と twist 角を持つ設定型、および閉路の twist seam の契約を決める。
   - Variable Tube と内部生成処理を共有し、一定断面版を二重実装しない。

### 入力形式と追加候補

- `HeightField()` は `Image` 専用 overload より先に、グリッド座標から高さを返す callable overload を評価する。
- `Image` overload を追加する場合は、チャネルまたは輝度変換、正規化範囲、Y scale / offset、行方向、HDR 入力の範囲を決める。
- `IcoSphere()` は subdivision 上限、overflow、UV seam を持つ構成と UV を持たない構成のどちらを公開するか決める。
- double-sided plane、torus arc、torus knot、superellipsoid は利用例が明確になった時点で評価する。
- `RegularPrism` は `Extrude()`、rounded cylinder は `Revolve()` での代替を優先する。

### レンダリング層と境界体積への依存

- 3D レンダリング側の設計が固まるまで、`Mesh3D::draw()` は追加しない。
- レンダリング統合時に `Vertex3D` の GPU レイアウト、頂点カラー、index の上限と primitive-restart 値、CPU メッシュと GPU リソースの責務を再確認する。
- bounding box / bounding sphere は `s3d::Box` / `s3d::Sphere` の実装後に設計する。

### 低優先度のレビュー残件

- `append()` が入力ごとに O(triangle count) の `validate()` を行うコストを、信頼済みメッシュを大量合成する実例が出た時に再評価する。
- Loft の端面生成で作る一時 `Polygon` と、DirectXMesh 由来の normals 実装の配置を、関連コードを変更する機会に整理する。
- default 引数と scalar overload、`Shape2D` からの Extrude / Sweep は、実利用上の不足が確認された場合に API 全体として設計する。

### OBJ 入力

- Wavefront OBJ の入力 API を設計して実装する。
- OBJ の位置、UV、法線の独立したインデックスを `Vertex3D` の単一インデックスへ統合する規約を決める。
- 材質ライブラリ、複数オブジェクト、材質割り当てを扱う前に、材質情報を持たない単一メッシュである `Mesh3D` とは別のモデルまたはシーン層を設計する。

### 変形・編集

- Variable Sweep の設計後に、汎用 taper、twist、bend と重複しない責務を再評価する。
- 頂点溶接、フラット・スムーズ境界の分割、細分割などの編集機能を検討する。
