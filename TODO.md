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

### 座標規約とドキュメント

- `Polygon` の外周・穴、および `Loft()` の断面の winding を、画面上の「時計回り」だけでなく格納された `(x, y)` に対する符号付き面積で定義する。
  - 現行契約は `Σ(x[i] * y[i+1] - x[i+1] * y[i])` が外周と Loft 断面では正、穴では負。
  - `Polygon.hpp`、Mesh3D のクラス説明、各 Loft overload に、同じ定義と有効な最小例を重複しすぎない形で配置する。
- 部分 `Revolve()` の正の角度が `+X` から `+Z`、`Cylindrical::phi` / `Spherical::phi` / `Quaternion::RotateY()` が `+X` から `-Z` である不一致を、v0.8 の API 固定前に再検討する。
  - どちらかへ統一するか、現状を維持して相互参照を追加するかを決める。
  - 変更する場合は winding、端面、接線、U 座標、既存の完全回転 overload への影響をまとめて確認する。
- `Revolve()`、`Loft()`、`Sweep()` などの点列入力について、連続する同一点が無効である契約を揃えて記述し、未検証の経路には失敗テストを追加する。
  - 現行 `Revolve()` は float 変換後に長さ 0 となる profile segment を拒否しており、縮退三角形を生成する仕様ではない。

### Builder の追加範囲と部品情報

- 直前または任意区間の add で追加された頂点・三角形の範囲を取得する API が必要か評価する。
  - 用途は部品単位の検査、選択、材質割り当て、OBJ 等への group / object 情報の引き渡し。
  - 現状は add 前後の `getMesh().vertexCount()` / `triangleCount()` の差分で取得できるが、利用者ごとに同じラッパが必要になる。
  - 各 add の戻り値変更、stateful な `lastAdded*()`、checkpoint / scope、builder と別のモデル層のいずれが適切かは未決定とする。
- 複数部品を傾いたローカル座標系へ配置する際の、手動の基底ベクトル計算を減らす方法を評価する。
  - scoped transform、明示的な placement、builder の外側の補助型などを候補とし、transform の合成順序と明示的な `Mat4x4` overload との関係を先に決める。
  - `Anchor` や Y 区間指定を各 primitive へ個別追加して overload を増やす前に、直交性のある配置機能で吸収できるか検討する。

### 端面制御

- `Cylinder`、`Cone`、`ConicalFrustum`、`Extrude`、`Loft`、開路の `Tube` / `Sweep` で、始端・終端を選択的に生成しない用途を評価する。
- `BoxShell::openFaces`、部分 `Revolve` の `CloseEnds`、`CloseRing`、`CloseBottom` はそれぞれ異なる位相操作である。名前だけを統一せず、各操作の意味と組み合わせを整理してから型を設計する。
- 追加する場合は cap の winding、法線、UV、hard edge、頂点・三角形数、および Builder の失敗時非変更保証を既存規約に合わせる。

### 利用例

- 2 点間を結ぶ柱・梁には `Tube({ from, to }, radius, sides)` を使えることを示し、専用 `Cylinder(from, to)` overload の必要性はその後に再評価する。
- 2D 立面図を world XY、押し出し方向を world Z として使う、`Extrude()` + `Quaternion::RotateX(90_deg)` の例を追加する。
- `HeightField()` の `[y][x]` ループ、OBJ / MTL で相対テクスチャパスを使う例は、Doxygen と将来の manual test のどちらに置くか決める。
- `Cylindrical` / `Spherical` の配置例、および接合部には `Box`、露出部には `ChamferedBox` / `RoundedBox` を使う指針は、サンプル拡充時の候補とする。

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
- `Extrude` / `Loft` の断面として使う扇形・扇形環を、手書きの三角関数ループなしで `Polygon` または `Shape2D` として生成する API を評価する。配置先は Mesh3D ではなく 2D geometry API を優先する。
- double-sided plane、torus arc、torus knot、superellipsoid は利用例が明確になった時点で評価する。
- `RegularPrism` は `Extrude()`、rounded cylinder は `Revolve()` での代替を優先する。

### レンダリング層と境界体積への依存

- 3D レンダリング側の設計が固まるまで、`Mesh3D::draw()` は追加しない。
- レンダリング統合時に `Vertex3D` の GPU レイアウト、頂点カラー、index の上限と primitive-restart 値、CPU メッシュと GPU リソースの責務を再確認する。
  - 1 モデル内の複数色・材質という実利用要求を、頂点カラー、submesh / material range、別のモデル層のどこで表現するか決める。`Vertex3D` への color 追加を前提にしない。
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
