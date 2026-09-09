# Implementation TODO

実装を保留している API や、設計判断が必要な項目を記録する作業補助ファイルです。

- 公開ヘッダごとに、関連する項目をトピック単位でまとめます。
- 実装に着手できるよう、保留理由と決めるべき契約を記録します。
- 実装とテストが完了したトピックは、このファイルから削除します。
- 完了項目の履歴は残さず、未完了項目だけを維持します。

## `Siv3D/include/Siv3D/BinaryFileReader.hpp`

### プラットフォーム検証

- macOS で共通実装の変更後のビルド・全自動テストを実行する。

## `Siv3D/include/Siv3D/TextFileReader.hpp`

### UTF-8 読み込みの段階的な高速化

- `readAll(String&)` の一時 UTF-8 領域と CR 除去の走査を見直す。[UTF-8 ベンチマーク](docs/development/text-file-reader-benchmark.md) で既存の simdutf 一括変換との比較を行い、変換エラー時の挙動を暗黙に変更しない。
- macOS で共通実装のビルド・全自動テストと UTF-8 ベンチマークを実行する。

## `Siv3D/include/Siv3D/FileSystem.hpp`

### プラットフォーム検証

- Windows で、OEM コードページが UTF-8 以外の環境での Unicode 相対パス、夏時間のあるタイムゾーンでのファイル日時、移動先を使用する環境での Downloads 取得を確認する。
- Linux のビルド・自動テスト手順を整備し、macOS と共通の実装を検証する。

### 基盤の見直し

- [基盤見直し案](docs/filesystem/proposals/foundation-review.md) に沿って、列挙の重複した変換や属性取得を減らす。代表的なアセット一覧・属性取得を同じ Release 条件で比較する。
- macOS で存在確認と I/O が異なる対象を指す区切り文字の扱い、および列挙でリンク名が失われる挙動を見直す。契約と Reader / Writer・資源一覧への影響を確認してから変更する。
- [診断プログラム](Test/Manual/FileSystemReview.md) を必要に応じて Windows でも実行し、変更に関係する OS 差を照合する。

## `Siv3D/include/Siv3D/Array.hpp`

### テストのプラットフォーム・計測範囲

- Windows / MSVC で `Test_Array.cpp`、`Test_ArrayException.cpp`、`Test_ArrayModel.cpp` を実行し、特に `Array<bool>` の領域切り替えとアロケータ伝播を確認する。
- 通常の API・並列処理テストも含めたサニタイザ／カバレッジ計測を整備する。`tools/run-array-checks.sh` の対象は例外・モデルテストであり、未実体化のテンプレートメンバはカバレッジの分母に含まれない。

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

### API 表面の整理

- `Plane` / `Grid` の UV、Box 系の scalar / vector と UV mapping の組み合わせを options へ集約するか、実利用コードと宣言数を比較して決める。単に型数を増やすだけの options 化は行わない。

### Builder の配置入力と失敗

- `Mesh3DPlacement` 付き add 関数は、形状生成後に配置変換を適用している。`Vec3`、offset + quaternion、`Mat4x4` から作られた placement について、非有限 offset、非単位 quaternion、非アフィンまたは非有限 matrix を、既存内容を変更する前に拒否する契約を検討する。
  - `NumericRange` は非有限値や float 範囲外、`InvalidArgument` は非単位 quaternion や非アフィン matrix に使用できる。新しい error code は、呼び出し側が既存分類と区別して処理する必要が確認された場合だけ追加する。
  - 全 transformed add に検証を加える実行時コストを測り、頻繁な追加処理へ例外入力対策の分岐を追加してよいか判断する。
- 大量の add 結果を逐次検査する用途向けに、最初の失敗を収集する外部ヘルパ、builder の診断 sink、現状の `Mesh3DAddResult` のどれが最小かを実利用コードで比較する。builder が暗黙に失敗状態を持つ設計は、再利用時の状態を増やすため慎重に扱う。

### 点列入力の準備と厳密性

- 計算上 0 を意図した `Revolve` の微小な正の端点半径は開口リングになり、float 演算で縮退三角形を生む場合がある。生成三角形の追加検証を行うか、厳密な 0 を要求する現契約と警告だけに留めるか、実行時コストを含めて決める。
- `Polygon` / `Loft` 断面の winding 正規化、サンプリングした曲線の連続重複点除去、円形断面生成を、Mesh3D 固有 API ではなく 2D geometry / 点列 utility として提供できるかを先に調べる。
- `Extrude`、`Loft`、`Sweep` で連続重複点を一律に自動除去する案は、入力ミスを隠すこと、属性対応をずらすこと、追加コストを伴うため採用しない。必要なら明示的な前処理 API とする。

### 端面制御

- `Cylinder`、`Cone`、`ConicalFrustum`、`Extrude` で、始端・終端を選択的に生成しない用途を評価する。
- `BoxShell::openFaces`、部分 `Revolve` の `CloseEnds`、`CloseRing`、`CloseBottom` はそれぞれ異なる位相操作である。名前だけを統一せず、各操作の意味と組み合わせを整理してから型を設計する。
- `Mesh3DEndCaps` を他の generator へ流用する場合、経路の始端・終端という意味が底面・上面などの固有名より明確かを確認する。名前だけを統一するためには使わない。
- 追加する場合は cap の winding、法線、UV、hard edge、頂点・三角形数、および Builder の失敗時非変更保証を既存規約に合わせる。

### 利用例

- `HeightField()` の `Grid<float>` 入力と callable 入力の使い分け、OBJ / MTL で相対テクスチャパスを使う例は、Doxygen と将来の manual test のどちらに置くか決める。
- `Cylindrical` / `Spherical` の配置例、および接合部には `Box`、露出部には `ChamferedBox` / `RoundedBox` を使う指針は、サンプル拡充時の候補とする。

### 入力形式と追加候補

- `Image` overload を追加する場合は、チャネルまたは輝度変換、正規化範囲、Y scale / offset、行方向、HDR 入力の範囲を決める。
- 扇形・扇形環は既存の `Circle::pieAsPolygon()` / `arcAsPolygon()` を優先する。未三角形化の円弧点列が必要な用途だけを、`outer()`、`PointsPerCircle`、`QualityFactor` と比較して評価する。円弧単体の分割数指定、角度の基準、全周と開弧の端点規約の不足が実証されるまで新 API は追加しない。
- double-sided plane、torus arc、torus knot、superellipsoid は利用例が明確になった時点で評価する。
- `RegularPrism` は `Extrude()`、rounded cylinder は `Revolve()` での代替を優先する。

### レンダリング層への依存

- 3D レンダリング側の設計が固まるまで、`Mesh3D::draw()` は追加しない。
- レンダリング統合時に `Vertex3D` の GPU レイアウト、頂点カラー、index の上限と primitive-restart 値、CPU メッシュと GPU リソースの責務を再確認する。
  - 1 モデル内の複数色・材質という実利用要求を、頂点カラー、submesh / material range、別のモデル層のどこで表現するか決める。`Vertex3D` への color 追加を前提にしない。
### 低優先度のレビュー残件

- `append()` が入力ごとに O(triangle count) の `validate()` を行うコストを、信頼済みメッシュを大量合成する実例が出た時に再評価する。
- default 引数と scalar overload、`Shape2D` からの Extrude / Sweep は、実利用上の不足が確認された場合に API 全体として設計する。

### OBJ 入力

- Wavefront OBJ の入力 API を設計して実装する。
- OBJ の位置、UV、法線の独立したインデックスを `Vertex3D` の単一インデックスへ統合する規約を決める。
- 材質ライブラリ、複数オブジェクト、材質割り当てを扱う前に、材質情報を持たない単一メッシュである `Mesh3D` とは別のモデルまたはシーン層を設計する。

### 変形・編集

- Tube の経路点別 radius と Sweep の経路点別 scale / twist で表現できない、生成後メッシュに対する汎用変形の責務を再評価する。法線・接線の再計算、部品範囲との関係、および bend 用経路生成との役割分担を先に決める。
- 頂点溶接、フラット・スムーズ境界の分割、細分割などの編集機能を検討する。

## `Siv3D/include/Siv3D/Mesh3DAssembly.hpp`

### 組立データの利用と編集

- 登録順に依存しない reparent や部品削除は、実利用で必要性を評価してから追加する。ID の安定性、古い ID の扱い、子の扱い、再利用ストレージと階層走査のコストを同時に設計する。
- 形状内の側面・端面などへの材質割り当ては v0.8.1 以降に再評価する。三角形への材質割り当てを先に設計し、generator の面役割はその後に検討する。
- 部品 ID と入力輪郭・経路区間を関連づける明示的な診断、および接触・間隔の期待値を指定する開発時検査を設計する。

### モデリング支援の機能候補

API 名や実装方針は未確定。寸法・配置・配色の変更時に、記述量と修正漏れが減るかを評価する。

- **取り付け座標系の未対応用途。** 空間曲線の端面について、経路に沿った断面軸の輸送と取り付け座標系の受け渡しを評価する。平面経路のレシピで使う一定の断面軸を一般化しない。専用の AnchorID や文字列検索は、通常の戻り値構造体と既存の Align / SweepOptions で不足する契約が実証されてから検討する。`setMesh()` による自動追従や常時評価する制約機構は前提にしない。
- **優先: モデリング用の診断。** [プレビューガイド](docs/mesh3d/preview.md) の接写・部品選択を使い、意図した接触・支持・通行空間の検査を評価する。部品の同一性は名前でなく PartID で保持する。頂点の存在だけで開口を判定せず、対象の面や線分との交差を使う検査を、既知の正常・異常入力で確認する。Siv3D の描画 API を先行確定せず、開発用の補助ツールとして設計する。
- **次点: 別 Assembly への部品一式の組み込み。** 同一 Assembly 内の再利用には `cloneSubtree()` を使う。別モデルへの組み込みは、まず通常の C++ 関数で Assembly に部品を追加し、根と関節の ID を返すレシピを試す。不足する場合に、別 Assembly からの取り込み、MeshID / MaterialID / PartID の対応、繰り返し取り込む際の共有データの重複防止と材質の置換範囲を設計する。単体メッシュの登録・配置を一度で済ませる便利関数は、既存の共有形状利用を損なわないかを見てから判断する。
- **v0.8.1 以降: 1 つの形状内の面ごとの材質。** 立体的な装飾は独立した部品として表現し、単なる塗り分けでは重ねた薄板を必要としない表現を検討する。箱の面、面取り部分、円柱の側面・端面を題材とし、三角形の番号に依存せず材質を割り当て、分割数変更後も意味を保てることを評価する。generator の面識別と、Assembly / bake / OBJ の材質範囲の責務を合わせて設計する。

色違いは現在の MaterialID と `setMaterial()`、等間隔配置は通常のループ、左右反転は親 Part の鏡映で表現できる。専用のパレット型・配列配置 API・対称モデリング API は、これらで不足する具体例が出るまで追加しない。

## Mesh3D モデリング支援の設計検討

- 開いた格子曲面の公開契約は未確定。[格子曲面の試作案](docs/mesh3d/proposals/open-grid-surface.md)を基に、出力領域の再利用、対角線、UV、hard edge、HeightField との共有範囲を検討する。閉曲面・極点・自己交差修復を同時に扱わない。
- 面の材質割り当ては v0.8 の作業対象から外し、v0.8.1 以降に再評価する。再開時は [面ごとの材質設計案](docs/mesh3d/proposals/face-materials.md) の比較ケースで内部表現を評価する。三角形への割り当てと generator の面役割を分離し、形状差し替え時と bake / export の契約を決めてから公開型を追加する。
- 軸からのフレーム生成、誤差による分割数指定、形状登録と部品追加の一体化は保留する。既存の quaternion / quality API とサンプルのヘルパーで不足する根拠を先に確認する。Builder の暗黙の失敗状態は追加しない。
