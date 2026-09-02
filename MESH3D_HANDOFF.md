# Mesh3D 実装の引き継ぎ

更新日: 2026-09-01

## 新規セッションで最初に確認すること

- リポジトリ: `/Users/siv3d/Applications/Projects/siv8`
- ブランチ: `v8_develop_2026_08`
- この文書更新時の HEAD: `3ba4a377` (`2026-09-01 update`)
- `Extrude`、`Revolve`、`Tube`、`Sweep`、`HeightField`、`Loft` を含む Mesh3D 本実装、テスト、macOS / Windows プロジェクト登録は HEAD までにコミット済み。
- 新しい作業を始める前に `git status --short` と `git log -1 --oneline` を再確認する。

この文書更新時の未コミット項目は次のとおり。

```text
 M macOS/Main.cpp
 M TODO.md
?? MESH3D_HANDOFF.md
?? Mesh3D_OBJ_MTL_Examples_DELETE_AFTER_REVIEW/
```

`macOS/Main.cpp` とレビュー用生成物はユーザーの作業物である。明示的な依頼なしに削除、巻き戻し、コミット対象への混入をしない。

## 未コミットの OBJ / MTL プレビューについて

`macOS/Main.cpp` には、44 個の既存プリミティブを一括した OBJ / MTL を生成する一時 exporter が追加されている。

- `Main()` 冒頭で `ExportMesh3DExamples()` を無条件に呼んでいるため、アプリやテストの起動ごとに生成物が更新される。
- `--export-mesh3d-examples` 用の呼び出しも、テスト専用 early-exit の後に残っている。引数付き実行では 2 回出力される。
- 無条件呼び出しは `[[nodiscard]]` の戻り値を捨てているため、ビルド時に既知の warning が 1 件出る。
- `--test-only` を処理する early-exit ブロックは `macOS/run-tests.sh` が依存しているため、削除・移動しない。
- レビュー終了時は、exporter 全体を削除するか、無条件呼び出しだけを削除するかをユーザーに確認する。

レビュー用ディレクトリ:

```text
Mesh3D_OBJ_MTL_Examples_DELETE_AFTER_REVIEW/
├── .DS_Store
├── all.mtl
├── all.obj
└── example.jpg
```

生成時点の内容は 8 列、44 パターン、OBJ 7,795 頂点 / 11,416 面。MTL の材質名は `Material`、`map_Kd example.jpg` を参照する。

## ユーザーの方針

- 当面は `Mesh3D` の生成関数を増やす。
- 坂道、階段、屋根、柱、壁などを組み合わせ、建築物や構造物を豊かに表現できることを重視する。
- 建築部材名を大量に追加するより、`Extrude`、`Revolve`、`Sweep`、`Loft` のような直交性の高い生成操作を優先する。
- おすすめ順に、適度な粒度で API 外観を確認してから実装する。
- 公開形状パラメータは原則 `double`、`Vec2` / `SizeF`、`Vec3` を使い、利用者に不要な `float` 指定を求めない。
- 公開設定用の型を追加する場合は、必要に応じて独立した public header に置く。
- 公開 API には Doxygen と、正常・境界・失敗ケースの専用テストを追加する。
- リアルタイム用途を考慮し、不要な動的確保、コピー、三角関数や検査の反復を避ける。

## 現在の Mesh3D 到達点

### 合成・編集・出力

- `append(const Mesh3D&)`
- `append(const Mesh3D&, const Mat4x4&)`
  - transformed mesh を一時生成せずに結合する。
  - 位置、法線、tangent を変換する。
  - 線形部分の行列式が負なら tangent の `w` を反転するが、三角形の巻き順は変えない。
- validate、巻き順・法線反転、transform、translate、rotate、scale にテストがある。
- OBJ 単体、または `Material` を伴う OBJ / MTL の保存・エンコードに対応する。

### プリミティブ

- Box 系: `Box`、`RoundedBox`、`Wedge`、`TriangularPrism`、`Stairs`
- 角錐系: `Pyramid`、`RectangularFrustum`
- 正多面体: `Tetrahedron`、`Octahedron`、`Icosahedron`、`Dodecahedron`
- 面・格子: `Plane`、`Grid`、`HeightField`
- 円形・球形: `Torus`、`Capsule`、`UVSphere`、`Hemisphere`、`Disc`、`Annulus`
- 円柱系: `HollowCylinder`、`ConicalFrustum`、`Cylinder`、`Cone`
- 汎用生成: `Extrude`、`Revolve`、`Tube`、`Sweep`、`Loft`

旧 `Frustum` は、矩形版との区別を明確にするため `ConicalFrustum` へ改名済み。互換ラッパーは置いていない。

すべての生成関数は、不正値、非有限値、個数計算の overflow、`Mesh3D::MaxVertexCount` 超過を可能な範囲で検出し、空メッシュを返す方針。

## 共通の座標・UV・tangent 規約

- Siv3D の左手 3D 座標系、Y-up を使う。
- 2D Polygon を水平断面へ移す API では `(x, y)` を `(X, -Z)` に対応させる。
- 内部 UV は画像上端が `V = 0`、下端が `V = 1` の Siv3D / D3D スタイル。
- 球・カプセル・円柱状側面は `+X` を `U = 0` とし、`+Z` 方向へ U が増える。
- 法線と tangent は単位長、互いに直交し、tangent の `w` で bitangent の向きを表す。
- UV seam、ハードエッジ、端面境界では必要に応じて頂点を複製する。
- `uvScale` の負値・0 は、有限で最終 UV が表現可能なら反転・固定指定として有効。

## 汎用 generator の確定仕様

### Extrude

- `Polygon` の外周と穴を押し出し、上下面を閉じる。
- 水平位置は維持し、高さだけを `-height / 2` から `+height / 2` に配置する。
- 上下面は Polygon 全体の bounding box による planar UV。
- 側面 U は外周・各穴の周長ごとに `[0, 1]`、V は上端 0 / 下端 1。
- 2 引数版は輪郭の角をハードエッジにする。`smoothingAngle` 版は指定角以下の隣接側面を補間する。
- 上下面との境界は常にハードエッジ。

### Revolve

- `profile` の X を半径、Y を生成後の Y として Y 軸周りに一周回転する。自動中央揃えはしない。
- 外側輪郭を低い Y から高い Y へ並べると外向き。
- 半径 0 の端点は軸上 fan、正の端点は開口。端面は profile で明示する。
- 先頭と末尾が同一なら閉 profile とし、V seam 用の頂点を複製する。
- 回転方向は常に smooth。profile の折れは既定で hard、`smoothingAngle` 版で補間できる。
- 部分回転と切断面、自己交差検査は初期 API の対象外。

### Tube

- 2 点以上の開経路に一定半径の円形断面を parallel-transport frame で運ぶ。
- 始終端は閉じ、端面と側面の境界は hard。円周方向と経路の折れ点は smooth。
- 連続する同一点、閉経路、180° 折り返しは不正。鋭角部の自己交差は検査しない。
- 側面 U は円周 `[0, 1]`、V は始端からの実距離。`uvScale.y` を単位経路長あたりの repeat 数に使える。

### Sweep

- 穴を含む `Polygon` を一定断面として、Tube と共通の parallel-transport frame で開経路に沿わせる。
- 自動方向版と `initialNormal` 指定版がある。
- 断面位置は維持し、自動中央揃えしない。始終端は Polygon の三角形分割で閉じる。
- 断面の角と端面境界は hard、経路の折れ点は smooth。
- 側面 U は外周・各穴の周長ごとに `[0, 1]`、V は経路の実距離。
- Tube と同じ不正経路条件を使い、鋭角部の自己交差は検査しない。

### HeightField

```cpp
static Mesh3D HeightField(
    const Grid<float>& heights,
    SizeF sizeXZ,
    Vec2 uvScale = Vec2{ 1.0, 1.0 },
    Vec2 uvOffset = Vec2{ 0.0, 0.0 });
```

- 各値を頂点の Y 座標としてそのまま使う。Y scale / offset は行わない。
- XZ 原点中心で、頂点順、巻き順、UV の向きは既存 `Grid()` と同じ。
- 法線と tangent は中央差分、境界は片側差分で計算する。
- 幅・高さ 2 未満、非有限値、不正な大きさや UV、float 化による隣接座標の潰れは空メッシュ。
- `Image` 入力版は未実装。画素チャネル、正規化、Y scale / offset の契約を先に決める。

### Loft

```cpp
template <size_t SectionCount, size_t VertexCount>
    requires ((2 <= SectionCount) && (3 <= VertexCount))
static Mesh3D Loft(
    const std::array<std::array<Vec2, VertexCount>, SectionCount>& sections,
    const std::array<double, SectionCount>& heights,
    Vec2 uvScale = Vec2{ 1.0, 1.0 },
    Vec2 uvOffset = Vec2{ 0.0, 0.0 });
```

- 断面数、各断面の頂点数、高さ数の一致をコンパイル時に保証する。
- 公開テンプレートは断面ごとの `span` をスタック上に作り、private の非テンプレート `LoftImpl()` へ委譲する。
- 各断面は時計回りの閉じた単純輪郭。先頭頂点を末尾へ重複させない。
- 高さは float 化後も厳密な昇順。同じ頂点添字同士を接続し、始終端を閉じる。
- 輪郭の角と端面境界は hard、断面間は smooth。
- 側面 U は最初の断面の周長で `[0, 1]`、V は最初の高さからの実距離。
- 中間断面内と断面間の自己交差は検査しない。

## 主な実装・テストファイル

```text
Siv3D/include/Siv3D/Mesh3D.hpp
Siv3D/include/Siv3D/detail/Mesh3D.ipp
Siv3D/src/Siv3D/Mesh3D/Mesh3D.cpp
Siv3D/src/Siv3D/Mesh3D/Mesh3DPrimitives.cpp
Siv3D/src/Siv3D/Mesh3D/Mesh3DExtrude.cpp
Siv3D/src/Siv3D/Mesh3D/Mesh3DRevolve.cpp
Siv3D/src/Siv3D/Mesh3D/Mesh3DTube.cpp        # Tube と Sweep
Siv3D/src/Siv3D/Mesh3D/Mesh3DHeightField.cpp
Siv3D/src/Siv3D/Mesh3D/Mesh3DLoft.cpp
Siv3D/src/Siv3D/Mesh3D/Mesh3DOBJ.cpp

Test/Test_Mesh3D.cpp
Test/Test_Mesh3DPrimitives.cpp
Test/Test_Mesh3DExtrude.cpp
Test/Test_Mesh3DRevolve.cpp
Test/Test_Mesh3DTube.cpp
Test/Test_Mesh3DSweep.cpp
Test/Test_Mesh3DHeightField.cpp
Test/Test_Mesh3DLoft.cpp
Test/Test_Mesh3DOBJ.cpp
```

新しい shared source / test を追加した場合は、次の 5 ファイルも更新する。

```text
macOS/OpenSiv3D.xcodeproj/project.pbxproj
WindowsDesktop/Siv3D.vcxproj
WindowsDesktop/Siv3D.vcxproj.filters
WindowsDesktop/Siv3D-Test.vcxproj
WindowsDesktop/Siv3D-Test.vcxproj.filters
```

## 最終検証ベースライン

2026-09-01、HEAD `3ba4a377` と上記未コミット exporter が存在する状態で確認済み。

- `Mesh3D::Extrude`: 7 cases / 4,356 assertions
- `Mesh3D::Revolve`: 7 cases / 6,000 assertions
- `Mesh3D::Tube`: 4 cases / 2,345 assertions
- `Mesh3D::Sweep`: 5 cases / 2,103 assertions
- `Mesh3D::HeightField`: 5 cases / 550 assertions
- `Mesh3D::Loft`: 6 cases / 1,256 assertions
- Mesh3D 全体: 83 cases / 65,783 assertions
- macOS 全体: 539 cases / 73,000 assertions
- `./macOS/validate-projects.sh`: passed
- Windows project XML (`xmllint --noout`): passed
- `git diff --check`: passed
- 変更 source の UTF-8 BOM / LF 規約: passed

標準の確認コマンド:

```sh
./macOS/run-tests.sh '--test-case=*Mesh3D*'
./macOS/run-tests.sh
./macOS/validate-projects.sh
git diff --check
```

shared code を変更したら、macOS では focused test だけで終えず `./macOS/run-tests.sh` を実行する。他 OS の workflow で代用しない。

## 次の作業候補

未完了項目の正本は `TODO.md`。Mesh3D の推奨順は次のとおり。

1. `Image` 入力の `HeightField()` の API 外観を検討する。
   - 使用する画素チャネルまたは輝度変換
   - 画素値の正規化範囲
   - Y scale / offset
   - Image の上下方向と Grid の行方向
   - alpha や HDR 相当入力を扱うか
2. 仕様が明快で独立している `IcoSphere()` を実装する。
   - subdivision の上限と overflow
   - UV seam を持つ球面 UVか、均一三角形だけを優先するか
3. 建築用途で既存 generator から代替しにくい `ChamferedBox()` を検討する。
   - bevel の最大値
   - corner topology
   - `BoxUVMapping` との対応

`Image` 版 HeightField の仕様決定を保留する場合は、実装リスクの低い `IcoSphere` を先に進めてよい。

## 専用 API を急がない形状

- Arch、開口壁、歯車、星形、任意断面柱: `Extrude`
- Vase、Bottle、回転屋根、Funnel: `Revolve`
- Spring、Helix、rail、pipe: `Tube`
- 曲線梁、モールディング、角ダクト、ケーブルラック: `Sweep`
- 段階的に断面が変化する塔・ダクト・屋根: `Loft`
- Terrain: `HeightField(Grid<float>)`
- Ellipsoid、楕円柱、軸方向違い、oriented variant: 既存 mesh と `Mat4x4`
- `RegularPrism`: `Extrude` での代替を先に評価する。
- `RoundedCylinder`: `Revolve` で表現できるため専用 API を前倒ししない。
