# Mesh3D 開発引き継ぎ

更新日: 2026-09-07

この文書は、Mesh3D 開発を再開するための短い索引である。実装済み API の仕様書や完了履歴は兼ねない。

## 正本

- 公開 API と座標・UV・頂点属性・失敗時動作の契約: `Siv3D/include/Siv3D/Mesh3D.hpp`、`Mesh3DBuilder.hpp`、`Mesh3DAssembly.hpp` の Doxygen
- 未実装項目と設計判断: `TODO.md`
- リポジトリの作業・検証規則: `AGENTS.md`
- 実装の振る舞い: `Test/Test_Mesh3D*.cpp` と `Test/Mesh3DTestHelper.hpp`

この文書には、すぐ古くなる commit hash、worktree の状態、テスト件数、全ファイル一覧を記録しない。作業開始時に Git とプロジェクトファイルから確認する。

## 現在の設計方針

- `Mesh3D` は CPU 側のメッシュデータ、編集、生成、OBJ 出力を担当する。3D レンダリング側が未設計の間は `.draw()` を追加しない。
- 単体生成には `Mesh3D` の static factory、複数形状の直接合成には `Mesh3DBuilder` を使う。
- `Mesh3DAssembly` は共有形状、材質、名前と親子配置を持つ部品を所有する CPU 側の組立データとする。形状なしの部品はヒンジなどの座標系に使える。形状の差し替えや焼き込みで部品 ID は変化しない。
- Assembly の親は子より先に登録し、`local * parentWorld` で配置を合成する。`bake(destination)` は出力配列を再利用し、部品ごとの範囲と材質の独立したスナップショットを返す。頂点・三角形の予算超過では出力を変更しない。鏡映では法線・接線に加えて巻き順を反転し、表裏を維持する。
- `Mesh3DPlacement::Align(sourceFrame, targetFrame)` は取り付け座標系を一致させる `source.inverse() * target` を返す。配置先は親のローカル座標とし、形状と取り付け座標系は同じ寸法入力からレシピで生成する。逆変換可能性などの事前条件は検査せず、自動追従の状態は持たない。
- `cloneSubtree(root, placement, parent)` は呼び出し時点の根と子孫の部品だけをコピーし、形状・材質 ID を共有する。新しい根の配置と親を明示し、内部の親参照を付け替える。返される `ClonedSubtree::find()` で元の部品 ID から複製先を取得できる。Blender の Linked Duplicate と同様に、配置は独立し、共有データの編集は両方に反映する。専用テストは `Test/Test_Mesh3DPlacement.cpp` と `Test/Test_Mesh3DAssemblyClone.cpp`、利用例と参考資料は Assembly の manual test を参照する。
- Assembly と BakedMesh の `saveOBJ()` は、部品の group と材質割り当てを 1 組の OBJ / MTL に保存する。BakedMesh は writer への `encodeOBJ()` も持つ。出力名は ID と UTF-8 バイトの可逆な percent encoding を組み合わせ、重複名・空白・日本語を扱う。材質未指定の面には既定材質を明示する。OBJ に階層や形状共有は保存しない。
- factory と builder は内部の destination-writing generator を共有し、形状生成本体を二重実装しない。
- 汎用 generator と、頻出形状向けの効率的な specialization を組み合わせる。建築部材名を無制限に増やさない。
- 公開形状パラメータは原則 `double`、`Vec2` / `SizeF`、`Vec3` とする。
- 生成失敗は理由を `LOG_FAIL` へ出力する。factory は空メッシュを返し、builder の既存内容は変更しない。
- `Mesh3DBuilder` の全 add 関数は `Mesh3DAddResult` を返す。成功時は追加した頂点・三角形の連続範囲、失敗時は `InvalidArgument`、`InvalidGeometry`、`NumericRange`、`SizeLimit` に分類されたエラーを取得できる。
- Y 軸周りの正角は `Quaternion::RotateY()`、`Cylindrical`、`Spherical` と共通で、`+X` から `-Z` へ進む。完全・部分 `Revolve` の頂点順、接線、U 座標もこの規約に従う。
- `Polygon` の外周と `Loft` の断面は、格納された `(x, y)` に対する符号付き面積が正、`Polygon` の穴は負とする。閉じた輪郭の先頭点を末尾へ重複させない。
- 点列は float 変換後の幾何を基準に検証する。`Revolve` の閉じた profile だけは先頭・末尾の一致を閉鎖表現として使い、`Tube` / `Sweep` の閉路と `Loft` の断面では始点を末尾に重複させない。
- `Tube` / `Sweep` の生成設定は `TubeOptions` / `SweepOptions` に集約する。factory は `std::span` / initializer-list と一定値 / 経路点別値の組み合わせだけを overload とし、builder の配置 overload では options を末尾に置く。
- `Revolve` の回転範囲、分割数、法線補間、UV 変換、回転方向の端面設定は `RevolveOptions` に集約する。factory は `std::span` / initializer-list の 2 overload、builder は initializer-list、配置なし、`Mesh3DPlacement` 付きの 3 overload とする。
- builder のすべての配置 overload は `Mesh3DPlacement` に集約する。配置なしを独立させ、配置ありの offset、offset + rotation、`Mat4x4` を 1 overload にした。`Vec3` と `Mat4x4` は従来と同じ記述、回転と平行移動は `{ offset, rotation }` で指定できるため、API 宣言数を減らしても利用コードを肥大化させない。add 宣言数は 233 から 117 になった。
- `HeightField` の UV 設定は `HeightFieldOptions` に集約する。高さの入力は `Grid<float>`、または頂点数と格子点 `Point` から高さを返す callable の 2 系統とし、callable は行優先で評価する。
- `IcoSphere` は UV seam を作らず頂点を共有する軽量な球とする。細分化回数は 0～8、既定値は 2 で、法線は球面方向、UV は常に `(0, 0)` とする。テクスチャマッピング用途には `Sphere` を使う。
- `Mesh3D::computeBoundingBox()` / `computeBoundingSphere()` は、インデックス参照の有無によらず格納された全頂点を対象とする。DirectXCollision に `Vertex3D::pos` を stride 付きで直接渡し、中間配列と動的メモリ確保を避ける。空メッシュは原点・大きさ 0 の境界体積を返し、境界球は近似解とする。
- `CloseRing` は経路の末尾と先頭を接続する指定であり、端面の選択指定ではない。開路の `Tube` / `Sweep` は既定で両端面を生成する。部分 `Revolve` の `CloseEnds` と Hemisphere の `CloseBottom` もそれぞれ固有の面を制御する。
- `Tube` / `Sweep` は `Mesh3DEndCaps` で始端、終端、両端、端面なしを選択できる。未指定時は開路で両端、閉路で端面なしとなり、閉路に端面を明示する矛盾は `InvalidArgument` とする。

## 実装済みの主要機能

- 合成・編集: `append()`、`reserve()`、`clear()`、各種 transform、`transformUV()`、法線・接線の再計算
- 出力: OBJ、および `Material` を伴う OBJ / MTL の保存・エンコード
- Box 系: `BoxFace` による面選択、`BoxShell`、`BoxFrame`、`RoundedBox`、`ChamferedBox` など
- 汎用生成: `Extrude`、UV 変換に対応する完全・部分 `Revolve`、一定半径・経路点別半径の開路・閉路 `Tube`、一定断面および経路点別 scale / twist の開路・閉路 `Sweep`、`HeightField`、`IcoSphere`、runtime / compile-time `Loft`
- その他の基本プリミティブ一式。UV 球の API 名は `Sphere` とする。
- `Mesh3DBuilder` は既存メッシュと上記 generator を直接追加でき、配置なしと `Mesh3DPlacement` 付きの体系を持つ。
- `Sweep` の初期断面方向は一定断面・経路点別変換のどちらも `SweepOptions::initialXAxis` で指定する。

正確な overload、既定値、端面、巻き順、UV、異常入力の契約はヘッダを参照する。

## 実利用レビューからの申し送り

初見の利用者が 6 種類、184 部品の構造物を作成・検査した記録が [`Claude outputs/REPORT.md`](<Claude outputs/REPORT.md>) にある。最終生成物では、生成失敗、z-fighting、部品間の隙間、縮退三角形、裏返り、意図しない孤立部品は検出されず、現在の builder とクラス冒頭の Doxygen は実用上よく機能した。

実地評価で誤読が起きた `Sweep` の断面軸、`Extrude` の配置後の軸、quaternion の合成順、部分 `Revolve` の端面範囲、`BoxShell` / `BoxFrame` / `Stairs` の T 字接合は Doxygen へ具体例または注意を追加した。`Revolve` で V を反転するために一時メッシュを経由した事例を受け、UV 変換を `RevolveOptions` に追加した。

次のセッションへ引き継ぐ価値がある課題は `TODO.md` に整理した。新しい経路系 generator へ進む前提となる、角度、winding、点列の閉鎖表現、および失敗分類は公開ヘッダとテストで固定している。

単発の追加範囲と失敗理由は `Mesh3DAddResult`、永続的な名前・材質・階層と共有形状は `Mesh3DAssembly` が扱う。宝箱のヒンジ、材質変更、台車の車輪共有と分割数の差し替えを `Test/Test_Mesh3DAssembly.cpp` で検証する。一括出力、名前、部品範囲の検証、面ごとの材質対応は `Test/Test_Mesh3DAssemblyOBJ.cpp` で検証する。実行可能な一式は `Test/Manual/Mesh3DAssemblyExamples.md` にあり、8 色の base color と共有形状・鏡映配置を使うテクスチャなしの作業ロボットも含む。ロボットの材質は metallic = 0 として、従来の MTL の diffuse color に base color がそのまま伝わるようにしている。

報告中の `Quaternion::RotateX/Y/Z` の説明不足は現行 Doxygen ですでに解消済みである。また、現行 `Revolve` は連続する同一点を縮退面として生成せず、生成失敗として拒否する。この点は実装不具合として扱わず、入力契約の説明とテストの不足として評価する。

配置計算では `Quaternion::rotate(Vec3)` / `inverseRotate(Vec3)` を使用できる。ベクトルを float へ変換せず double で計算するが、クォータニオン自体の保持精度は float のままである。型を省いた `rotate({ ... })` / `inverseRotate({ ... })` は overload が曖昧になるため、`Vec3{ ... }` または `Float3{ ... }` を明示する。

Gemini によるヘッダと簡略化済みモデリングコードのレビューからは、Builder のローカル座標系、開路や柱状形状の端面制御、複数色・材質と部品範囲の関係、扇形環などの 2D 断面生成を検討候補として採用した。リポジトリ全体を参照していないレビューなので、提案された API 外観は確定案として扱わない。

次の提案は既存 API との重複または前提の不一致があるため、そのまま実装しない。

- 2 点間の円柱は `Tube({ from, to }, radius)` で表現でき、断面分割数が必要なら `TubeOptions::sides` で指定できる。この用法は `Tube` / `addTube` の Doxygen を参照する。
- 方向付き `Extrude` は offset + rotation overload で表現できる。まず立面図を押し出す具体例を追加する。
- `addMesh(Mesh3D&&)` は、連続した頂点・index 配列を持つ非空 builder へ一般にゼロコピーで吸収できない。性能上の根拠なしに direct generator の代替としない。
- 3D CSG は topology、coplanar face、UV、tangent、数値的頑健性を伴う別規模の課題であり、今回の Mesh3D 拡張候補には戻さない。

## 次の候補と保留事項

- `HeightField()` の `Image` 固有 overload は入力変換の契約が固まるまで保留する。次の生成候補は `TODO.md` の残件から、既存 generator で代替できない具体的用途を基準に選ぶ。
- レンダリング統合時に、`Vertex3D` の GPU レイアウト、頂点カラー、index 上限、CPU / GPU リソースの責務を決める。
- manual test は利用例であり、API の正本は公開ヘッダとする。新しい生成・編集 API は、宝箱・台車に加えて曲面主体の題材でも評価する。

## 実装時の共通条件

- winding、outward normal、UV、tangent、hard / smooth edge、seam の頂点複製を既存品質に合わせる。
- 非有限値、float 表現可能性、退化形状、個数 overflow、`MaxVertexCount` を出力変更前に検査する。
- 頂点数と三角形数は `CheckedAdd` / `CheckedMultiply` で計算し、三角関数のサンプルなど共通処理は `Mesh3DCommon.hpp` を再利用する。
- 新しい public API には Doxygen と、正常・境界・失敗ケースの専用テストを追加する。
- shared file を追加・削除・改名した場合は macOS / Windows のプロジェクトファイルを同時に更新する。

## macOS での検証

```sh
./macOS/run-tests.sh '--test-case=*Mesh3D*'
./macOS/run-tests.sh
./macOS/validate-projects.sh
git diff --check
```

shared code を変更した場合は focused test だけで終えず、ホスト向けの full suite を実行する。
