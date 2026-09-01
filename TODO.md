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

### 次に検討する生成 API

1. `Image` の画素値から地形を作る `HeightField()` オーバーロードを設計する。
   - 使用する画素チャネル、または RGB からの輝度変換を決める。
   - 画素値の正規化範囲と Y scale / offset の引数構成を決める。
   - Image の上から下への行順を、既存 `HeightField(Grid<float>)` の `+Z` から `-Z` への行順と一致させるか確認する。
   - alpha、16-bit、HDR 相当の入力を初期 API に含めるか決める。
2. icosahedron の再帰分割と球面投影による `Mesh3D::IcoSphere()` を追加する。
   - `subdivisions` の上限、頂点・三角形数の overflow、`MaxVertexCount` 超過を事前検出する。
   - 均一な三角形を優先して UV を持たせない構成と、球面 UV seam のために頂点を複製する構成を比較する。
3. 建築用途向けに、平面で面取りした `Mesh3D::ChamferedBox()` を検討する。
   - bevel の有効範囲と最大値を決める。
   - corner topology と hard / smooth normal の境界を決める。
   - `BoxUVMapping` を面取り面へどう拡張するか決める。

`Image` 版 HeightField の仕様決定を保留する場合は、依存が少なく仕様を固めやすい `IcoSphere()` を先に進める。

### 低優先度の形状候補

- double-sided plane が必要か評価する。
- torus arc、torus knot、superellipsoid を評価する。
- `RegularPrism` は `Extrude()`、rounded cylinder は `Revolve()` での代替を先に評価する。

### OBJ 入力

- Wavefront OBJ の入力 API を設計して実装する。
- OBJ の位置、UV、法線の独立したインデックスを `Vertex3D` の単一インデックスへ統合する規約を決める。
- 材質ライブラリ、複数オブジェクト、材質割り当てを扱う前に、材質情報を持たない単一メッシュである `Mesh3D` とは別のモデルまたはシーン層を設計する。

### 変形・編集

- taper、twist、bend など、トポロジを維持する非線形変形 API と法線・接線の扱いを検討する。
- 頂点溶接、フラット・スムーズ境界の分割、細分割、UV 変換などの編集機能を検討する。
