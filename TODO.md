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

### 基本形状

- `Mesh3D::Capsule()` を、円柱部分の長さを明示する API で追加する。
- open surface の `Mesh3D::Hemisphere()` を追加し、将来の底面オプションを検討する。
- 長方形の底面に対応する `Mesh3D::Pyramid()` を追加する。
- tetrahedron、octahedron、icosahedron、dodecahedron を、外接球半径を共通のサイズ規約として追加する。
- icosahedron の再帰分割と球面投影による `Mesh3D::IcoSphere()` を追加する。
- UV と tangent の規約を設計して `Mesh3D::RoundedBox()` を追加する。
- 補完候補として、double-sided plane、hollow cylinder、wedge を評価する。

### 汎用形状生成

- Torus、Capsule、Hemisphere で蓄積した実装を基に、2D プロファイルの回転体を作る `Lathe()` を設計する。
- 穴を含む `Polygon` の押し出し形状を作る `Extrude()` を設計する。
- parallel-transport frame を使い、3D 経路に沿うチューブを作る `Tube()` を設計する。
- `Grid<float>` や画像の標高値から地形を作る height field API を設計する。
- 応用候補として、torus arc、torus knot、superellipsoid、chamfered box、rounded cylinder を評価する。

### OBJ 入力

- Wavefront OBJ の入力 API を設計して実装する。
- OBJ の位置、UV、法線の独立したインデックスを `Vertex3D` の単一インデックスへ統合する規約を決める。
- 材質ライブラリ、複数オブジェクト、材質割り当てを扱う前に、材質情報を持たない単一メッシュである `Mesh3D` とは別のモデルまたはシーン層を設計する。

### 変形・編集

- taper、twist、bend など、トポロジを維持する非線形変形 API と法線・接線の扱いを検討する。
- 頂点溶接、フラット・スムーズ境界の分割、細分割、UV 変換などの編集機能を検討する。
