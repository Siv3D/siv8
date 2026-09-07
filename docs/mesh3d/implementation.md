# Mesh3D 実装方針

CPU 側の形状生成・組み立て・出力を開発するときの設計方針をまとめる。公開 API の正本はヘッダの Doxygen とし、未完了の作業は [TODO](../../TODO.md) で管理する。

資料の入口は [Mesh3D](README.md)、共通の開発手順は [開発ガイド](../development/README.md) を参照。

## 正本

- 公開 API と座標・UV・頂点属性・失敗時動作の契約: [Siv3D/include/Siv3D/Mesh3D.hpp](../../Siv3D/include/Siv3D/Mesh3D.hpp)、[Mesh3DBuilder.hpp](../../Siv3D/include/Siv3D/Mesh3DBuilder.hpp)、[Mesh3DAssembly.hpp](../../Siv3D/include/Siv3D/Mesh3DAssembly.hpp) の Doxygen
- 未実装項目と設計判断: [TODO.md](../../TODO.md)
- リポジトリの作業・検証規則: [AGENTS.md](../../AGENTS.md)
- 実装の振る舞い: `Test/Test_Mesh3D*.cpp` と [Test/Mesh3DTestHelper.hpp](../../Test/Mesh3DTestHelper.hpp)

この文書には、すぐ古くなる commit hash、worktree の状態、テスト件数、全ファイル一覧を記録しない。作業開始時に Git とプロジェクトファイルから確認する。

## 現在の設計方針

- `Mesh3D` は CPU 側のメッシュデータ、編集、生成、OBJ 出力を担当する。3D レンダリング側が未設計の間は `.draw()` を追加しない。
- 単体生成には `Mesh3D` の static factory、複数形状の直接合成には `Mesh3DBuilder` を使う。
- Builder の完成メッシュは `builder.obtainMesh()` でストレージごと取り出す。取得後の Builder は容量も含めて初期状態に戻り、再利用できる。`getMesh()` は const 参照、`clear()` は容量を保持した消去。旧 `std::move(builder).build()` は廃止した。
- 全体へのアフィン変換と負のスケールは表裏を維持する。Mesh3D の transform / scale / append、Builder の配置、Assembly の bake は鏡映時に接線の w と三角形の巻き順を反転する。意図的な表裏反転には invert() を使う。旧仕様に合わせた鏡映後の reverseWinding() は取り除く。API 横断の回帰テストは [Test/Test_Mesh3DTransform.cpp](../../Test/Test_Mesh3DTransform.cpp)。Loft の断面ごとの向き・進行方向の条件は別に維持する。
- `Mesh3DAssembly` は共有形状、材質、名前と親子配置を持つ部品を所有する CPU 側の組立データとする。形状なしの部品はヒンジなどの座標系に使える。形状の差し替えや焼き込みで部品 ID は変化しない。
- Assembly の親は子より先に登録し、`local * parentWorld` で配置を合成する。`bake(destination)` は出力配列を再利用し、部品ごとの範囲と材質の独立したスナップショットを返す。頂点・三角形の予算超過では出力を変更しない。鏡映では法線・接線に加えて巻き順を反転し、表裏を維持する。
- `Mesh3DPlacement{ offset, rotation, scale }` は Vec3 の非一様スケール → 回転 → 平行移動。2 引数版は単位スケールで同じ実装へ委譲する。構築時の追加検査はなく、負・0 スケールの使用可否は各利用先の既存契約に従う。
- `Mesh3DPlacement::Align(sourceFrame, targetFrame)` は取り付け座標系を一致させる `source.inverse() * target` を返す。配置先は親のローカル座標とし、形状と取り付け座標系は同じ寸法入力からレシピで生成する。逆変換可能性などの事前条件は検査せず、自動追従の状態は持たない。
- `cloneSubtree(root, placement, parent)` は呼び出し時点の根と子孫の部品だけをコピーし、形状・材質 ID を共有する。新しい根の配置と親を明示し、内部の親参照を付け替える。返される `ClonedSubtree::find()` で元の部品 ID から複製先を取得できる。Blender の Linked Duplicate と同様に、配置は独立し、共有データの編集は両方に反映する。専用テストは [Test/Test_Mesh3DPlacement.cpp](../../Test/Test_Mesh3DPlacement.cpp) と [Test/Test_Mesh3DAssemblyClone.cpp](../../Test/Test_Mesh3DAssemblyClone.cpp)、利用例と参考資料は Assembly の manual test を参照する。
- Assembly と BakedMesh の `saveOBJ()` は、部品の group と材質割り当てを 1 組の OBJ / MTL に保存する。BakedMesh は writer への `encodeOBJ()` も持つ。出力名は ID と UTF-8 バイトの可逆な percent encoding を組み合わせ、重複名・空白・日本語を扱う。材質未指定の面には既定材質を明示する。OBJ に階層や形状共有は保存しない。
- factory と builder は `Mesh3DGenerators.hpp` に宣言する内部の destination-writing generator を共有する。プリミティブの生成本体は Mesh3D 側の形状群別ファイルに置き、Builder は入力の転送と配置を担当する。サイズ検査・追加先の拡張・Result 用エラー生成は `Mesh3DCommon.hpp` に集約する。
- 汎用 generator と、頻出形状向けの効率的な specialization を組み合わせる。建築部材名を無制限に増やさない。
- 公開形状パラメータは原則 `double`、`Vec2` / `SizeF`、`Vec3` とする。Mesh3D の translate / scale も double / Vec3 で受け、変更用の実装の入口で float / Float3 に一度変換する。頂点ごとの計算・格納と符号判定は従来の float のままとし、値返し版は変更用の実装へ委譲する。
- 生成失敗時は既存の頂点・三角形の内容を保持する。Loft などは出力の拡張前に検証を終える。Tube / Sweep は書き込み中の生成値検証も行い、失敗時に追加前の配列サイズへ戻す。容量・data ポインタの保持やメモリ確保例外に対する非変更は保証しない。
- UV の負の拡大率には接線方向と handedness も追従する。0 の UV 軸は正方向として扱う。Plane / Grid、HeightField、Tube / Sweep、Revolve、Loft の横断テストは [Test/Test_Mesh3DUV.cpp](../../Test/Test_Mesh3DUV.cpp)。生成後の transformUV() は UV だけを編集し、必要な接線再計算は computeTangents() で明示する。
- Mesh3DRange::isEmpty() は両 count が 0、Mesh3D::isEmpty() はいずれかの配列が空、validate() は頂点数と index 範囲だけの検査とする。意味は Doxygen と境界テストで固定し、別名の同義 API は増やさない。
- 法線・接線の内部実装は Mesh3DDetail に閉じ込める。DirectXMesh 由来の法線計算には元の著作権表示を残し、未使用のプラットフォーム補助コードを混在させない。
- 生成失敗は理由を `LOG_FAIL` へ出力する。factory は空メッシュを返し、builder の既存内容は変更しない。
- `Mesh3DBuilder` の全 add 関数は `Mesh3DAddResult` を返す。成功時は追加した頂点・三角形の連続範囲、失敗時は `InvalidArgument`、`InvalidGeometry`、`NumericRange`、`SizeLimit` に分類されたエラーを取得できる。
- Y 軸周りの正角は `Quaternion::RotateY()`、`Cylindrical`、`Spherical` と共通で、`+X` から `-Z` へ進む。完全・部分 `Revolve` の頂点順、接線、U 座標もこの規約に従う。
- `Polygon` の外周と `Loft` の断面は、格納された `(x, y)` に対する符号付き面積が正、`Polygon` の穴は負とする。閉じた輪郭の先頭点を末尾へ重複させない。
- 点列は float 変換後の幾何を基準に検証する。`Revolve` の閉じた profile だけは先頭・末尾の一致を閉鎖表現として使い、`Tube` / `Sweep` の閉路と `Loft` の断面では始点を末尾に重複させない。
- `Tube` / `Sweep` の生成設定は `TubeOptions` / `SweepOptions` に集約する。factory は `std::span` / initializer-list と一定値 / 経路点別値の組み合わせだけを overload とし、builder の配置 overload では options を末尾に置く。
- `Extrude` の側面の平滑化は `ExtrudeOptions::smoothingAngle` で指定する。factory は `Extrude(polygon, height, options)`、Builder は `addExtrude(polygon, height, options)` と `addExtrude(polygon, height, placement, options)`。旧 scalar smoothingAngle overload は廃止した。既定のハードエッジ・端面・UV は維持する。
- `Revolve` の回転範囲、分割数、法線補間、UV 変換、回転方向の端面設定は `RevolveOptions` に集約する。factory は `std::span` / initializer-list の 2 overload、builder は initializer-list、配置なし、`Mesh3DPlacement` 付きの 3 overload とする。
- builder のすべての配置 overload は `Mesh3DPlacement` に集約する。配置なしを独立させ、配置ありの offset、offset + rotation、`Mat4x4` を 1 overload にした。`Vec3` と `Mat4x4` は従来と同じ記述、回転と平行移動は `{ offset, rotation }` で指定できるため、API 宣言数を減らしても利用コードを肥大化させない。
- `HeightField` の UV 設定は `HeightFieldOptions` に集約する。高さの入力は `Grid<float>`、または頂点数と格子点 `Point` から高さを返す callable の 2 系統とし、callable は行優先で評価する。
- `LoftSection` は借用輪郭と既存の Mesh3DPlacement を組み合わせる。旧 sections / heights 入力は廃止し、factory は span / initializer-list、builder は配置なし span / initializer-list と配置あり span の計 5 overload にする。Mesh3DPlacement は独立ヘッダにある。
- Loft の断面はローカル `(x, 0, -y)` に frame を適用する。frame は有限のアフィン変換・正の determinant とし、隣接原点の変位が両端の断面正方向へ正の投影を持つ必要がある。輪郭の自動対応・補間・リサンプリングは行わない。
- `LoftOptions` は端面・輪郭方向の smoothingAngle・UV 変換を持つ。側面 V は frame 原点の累積距離、平滑化は各断面の各輪郭頂点で判定する。頂点の辺ごとの複製と UV seam は保持し、要求された端面だけを三角形化する。局所的な縮退などは検証するが、輪郭・側面全体の自己交差は保証しない。
- `IcoSphere` は UV seam を作らず頂点を共有する軽量な球とする。細分化回数は 0～8、既定値は 2 で、法線は球面方向、UV は常に `(0, 0)` とする。テクスチャマッピング用途には `Sphere` を使う。
- `Mesh3D::computeBoundingBox()` / `computeBoundingSphere()` は、インデックス参照の有無によらず格納された全頂点を対象とする。DirectXCollision に `Vertex3D::pos` を stride 付きで直接渡し、中間配列と動的メモリ確保を避ける。空メッシュは原点・大きさ 0 の境界体積を返し、境界球は近似解とする。
- `CloseRing` は経路の末尾と先頭を接続する指定であり、端面の選択指定ではない。開路の `Tube` / `Sweep` は既定で両端面を生成する。部分 `Revolve` の `CloseEnds` と Hemisphere の `CloseBottom` もそれぞれ固有の面を制御する。
- `Tube` / `Sweep` は `Mesh3DEndCaps` で始端、終端、両端、端面なしを選択できる。未指定時は開路で両端、閉路で端面なしとなり、閉路に端面を明示する矛盾は `InvalidArgument` とする。

## 実装済みの主要機能

- 合成・編集: `append()`、`reserve()`、`clear()`、各種 transform、`transformUV()`、法線・接線の再計算
- 出力: OBJ、および `Material` を伴う OBJ / MTL の保存・エンコード
- Box 系: `BoxFace` による面選択、`BoxShell`、`BoxFrame`、`RoundedBox`、`ChamferedBox` など
- 汎用生成: `Extrude`、UV 変換に対応する完全・部分 `Revolve`、一定半径・経路点別半径の開路・閉路 `Tube`、一定断面および経路点別 scale / twist の開路・閉路 `Sweep`、`HeightField`、`IcoSphere`、輪郭とフレームで指定する `Loft`
- その他の基本プリミティブ一式。UV 球の API 名は `Sphere` とする。
- `Mesh3DBuilder` は既存メッシュと上記 generator を直接追加でき、配置なしと `Mesh3DPlacement` 付きの体系を持つ。
- `Sweep` の初期断面方向は一定断面・経路点別変換のどちらも `SweepOptions::initialXAxis` で指定する。

正確な overload、既定値、端面、巻き順、UV、異常入力の契約はヘッダを参照する。

## 次の候補と保留事項

- `HeightField()` の `Image` 固有 overload は入力変換の契約が固まるまで保留する。次の生成候補は [TODO.md](../../TODO.md) の残件から、既存 generator で代替できない具体的用途を基準に選ぶ。
- レンダリング統合時に、`Vertex3D` の GPU レイアウト、頂点カラー、index 上限、CPU / GPU リソースの責務を決める。
- 利用方法は [Loft ガイド](loft.md)、検証の期待結果は manual test を参照する。曲面主体の検証例は [Test/Manual/Mesh3DLoftExamples.md](../../Test/Manual/Mesh3DLoftExamples.md)。断面形状と向きが変わるダクト、平滑化、共有形状の分割品質変更、Align と鏡映複製を含む。
- OBJ / MTL の形状・配置・base color の確認には [プレビューの使い方](preview.md) の汎用プレビューを使える。PartID または領域での接写は周囲の遮蔽を保持し、単独表示・強調表示・部品色表示は独立に指定する。ID と名前・範囲・色の対応は JSON に出力する。実行手順、独立検証、描画上の制限は同文書を参照する。

## 実装時の共通条件

- winding、outward normal、UV、tangent、hard / smooth edge、seam の頂点複製を既存品質に合わせる。
- 非有限値、float 表現可能性、退化形状、個数 overflow、`MaxVertexCount` を出力変更前に検査する。
- 頂点数と三角形数は `CheckedAdd` / `CheckedMultiply` で計算し、三角関数のサンプルなど共通処理は `Mesh3DCommon.hpp` を再利用する。
- 新しい public API には Doxygen と、正常・境界・失敗ケースの専用テストを追加する。
- shared file を追加・削除・改名した場合は macOS / Windows のプロジェクトファイルを同時に更新する。

### 値型への委譲と共通化の境界

部品生成の都合によらず意味が定まる処理は、基礎となる値型へ委譲する。

| 処理 | 使用する API / 配置先 |
| --- | --- |
| 各成分の有限性 | `Vector2D` / `Vector3D` / `Vector4D`、`ColorF`、`FloatRect` の `isFinite()`。行列・回転は既存の `Mat4x4` / `Quaternion::isFinite()` |
| ゼロベクトル・成分の完全一致・最小成分 | 既存の `isZero()`、`operator ==`、`minComponent()`。接線の共有判定に epsilon 比較を持ち込まない |
| float 格納範囲 | `Mesh3DCommon.hpp` の `IsFloatRepresentable()`。有限な double でも float の範囲を超え得るため、`isFinite()` とは別の検査 |
| 生成個数の overflow・追加先の確保 | `Mesh3DCommon.hpp` の `CheckedAdd()` / `CheckedMultiply()` / `ResizeForAddition()` |

有限性は各成分だけを検査する。長さ・成分の和による代用は、大きな有限値で overflow するため使わない。判定の移譲に伴って、呼び出し元へ新しい検査を追加しない。

正規化の失敗条件は Loft / Tube などで異なり、断面・接線の退化判定にも関係する。既存の `normalized()` / `normalized_or()` と契約を比較し、見た目の類似だけで統合しない。輪郭の境界計算も、float の三角形分割結果を使うか double の元輪郭を使うか、別の処理と同じ走査で求めるかを維持する。

値型の有限性は [Test_Finite.cpp](../../Test/Test_Finite.cpp) で独立に検証する。Mesh3D のテスト側の成分別検査は、実装と同じ判定に依存しない確認として残す。

## macOS での検証

```sh
./macOS/run-tests.sh '--test-case=*Mesh3D*'
./macOS/run-tests.sh
./macOS/validate-projects.sh
git diff --check
```

shared code を変更した場合は focused test だけで終えず、ホスト向けの full suite を実行する。

## モデリング支援の試作

- [モデリングガイド](modeling.md) に既存 API と取り付け座標系の知見、[格子曲面の試作案](proposals/open-grid-surface.md) に設計上の保留事項を置く。実行コードと検査は [manual test](../../Test/Manual/Mesh3DModelingStudies.md) に置く。新たな Anchor / Surface 公開 API は追加していない。
- 柱と梁は寸法変更後の座標系一致、平面経路の管は SweepOptions::initialXAxis と FromUnitVectorPairs による端面多角形の一致を検査する。空間曲線への一般化は未検証。
- 面ごとの材質は [面ごとの材質設計案](proposals/face-materials.md) に内部表現・差し替え・bake / OBJ の比較案を記録する。実装済みの API 契約ではない。

## v0.8 の区切り

- Mesh3D のモデリング機能は現在の複数部品と部品ごとの材質を一区切りとし、当面は既存機能の不具合修正・検証・文書整備を中心にする。
- 面ごとの材質割り当ての内部試作・公開 API は v0.8.1 以降の再評価候補に回す。[面ごとの材質設計案](proposals/face-materials.md) は再開時の検討材料として保持し、実装予定の確約とはしない。
- Anchor / Surface の試作も公開 API 化を保留したまま保持する。
