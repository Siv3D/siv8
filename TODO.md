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

### 部品範囲と部品情報

- 部品範囲の長期的な所有先を決める。
  - 用途は部品単位の検査、選択、材質割り当て、OBJ 等への group / object 情報の引き渡し。
  - 単発の範囲は `Mesh3DAddResult` で扱えるが、名前、材質、階層を含む永続的な部品情報は builder と別のモデル層も候補とする。
- 複数部品を傾いたローカル座標系へ配置する際の、手動の基底ベクトル計算を減らす方法を評価する。
  - scoped transform、明示的な placement、builder の外側の補助型などを候補とし、transform の合成順序と明示的な `Mat4x4` overload との関係を先に決める。
  - `Anchor` や Y 区間指定を各 primitive へ個別追加して overload を増やす前に、直交性のある配置機能で吸収できるか検討する。

### 端面制御

- `Cylinder`、`Cone`、`ConicalFrustum`、`Extrude`、`Loft`、開路の `Tube` / `Sweep` で、始端・終端を選択的に生成しない用途を評価する。
- `BoxShell::openFaces`、部分 `Revolve` の `CloseEnds`、`CloseRing`、`CloseBottom` はそれぞれ異なる位相操作である。名前だけを統一せず、各操作の意味と組み合わせを整理してから型を設計する。
- `CloseRing` は経路の閉鎖だけを表す既存の意味を維持する。開路の片端だけを閉じる機能を追加する場合は別の端面指定型を使い、`CloseRing` の意味を拡張しない。
- 独立した端面・断面変形オプションが増える generator では、引数順を変えただけの overload を増殖させず、options 型にまとめる。既存の `SweepOptions` に端面指定を加えるか、Tube / Sweep / Loft で共有する端面指定型を導入するかを決める。
- 最初の対象は開路の Tube / Sweep とし、両端、始端のみ、終端のみ、端面なしを表す型と既定値を設計する。閉路で端面指定を受け付けるか、無効な組み合わせとして拒否するかも統一する。
- 追加する場合は cap の winding、法線、UV、hard edge、頂点・三角形数、および Builder の失敗時非変更保証を既存規約に合わせる。

### 利用例

- 2 点間を結ぶ柱・梁には `Tube({ from, to }, radius, sides)` を使えることを示し、専用 `Cylinder(from, to)` overload の必要性はその後に再評価する。
- 2D 立面図を world XY、押し出し方向を world Z として使う、`Extrude()` + `Quaternion::RotateX(90_deg)` の例を追加する。
- `HeightField()` の `[y][x]` ループ、OBJ / MTL で相対テクスチャパスを使う例は、Doxygen と将来の manual test のどちらに置くか決める。
- `Cylindrical` / `Spherical` の配置例、および接合部には `Box`、露出部には `ChamferedBox` / `RoundedBox` を使う指針は、サンプル拡充時の候補とする。

### 入力形式と追加候補

- 端面制御の契約を固めた後の生成 API 候補として、`HeightField()` は `Image` 専用 overload より先に、グリッド座標から高さを返す callable overload を評価する。
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

- Tube の経路点別 radius と Sweep の経路点別 scale / twist で表現できない、生成後メッシュに対する汎用変形の責務を再評価する。法線・接線の再計算、部品範囲との関係、および bend 用経路生成との役割分担を先に決める。
- 頂点溶接、フラット・スムーズ境界の分割、細分割などの編集機能を検討する。
