# Mesh3D 開発引き継ぎ

更新日: 2026-09-05

この文書は、Mesh3D 開発を再開するための短い索引である。実装済み API の仕様書や完了履歴は兼ねない。

## 正本

- 公開 API と座標・UV・頂点属性・失敗時動作の契約: `Siv3D/include/Siv3D/Mesh3D.hpp` と `Mesh3DBuilder.hpp` の Doxygen
- 未実装項目と設計判断: `TODO.md`
- リポジトリの作業・検証規則: `AGENTS.md`
- 実装の振る舞い: `Test/Test_Mesh3D*.cpp` と `Test/Mesh3DTestHelper.hpp`

この文書には、すぐ古くなる commit hash、worktree の状態、テスト件数、全ファイル一覧を記録しない。作業開始時に Git とプロジェクトファイルから確認する。

## 現在の設計方針

- `Mesh3D` は CPU 側のメッシュデータ、編集、生成、OBJ 出力を担当する。3D レンダリング側が未設計の間は `.draw()` を追加しない。
- 単体生成には `Mesh3D` の static factory、複数形状の直接合成には `Mesh3DBuilder` を使う。
- factory と builder は内部の destination-writing generator を共有し、形状生成本体を二重実装しない。
- 汎用 generator と、頻出形状向けの効率的な specialization を組み合わせる。建築部材名を無制限に増やさない。
- 公開形状パラメータは原則 `double`、`Vec2` / `SizeF`、`Vec3` とする。
- 生成失敗は理由を `LOG_FAIL` へ出力する。factory は空メッシュを返し、builder の既存内容は変更しない。
- `Mesh3DBuilder` の全 add 関数は `Mesh3DAddResult` を返す。成功時は追加した頂点・三角形の連続範囲、失敗時は `InvalidArgument`、`InvalidGeometry`、`NumericRange`、`SizeLimit` に分類されたエラーを取得できる。
- Y 軸周りの正角は `Quaternion::RotateY()`、`Cylindrical`、`Spherical` と共通で、`+X` から `-Z` へ進む。完全・部分 `Revolve` の頂点順、接線、U 座標もこの規約に従う。
- `Polygon` の外周と `Loft` の断面は、格納された `(x, y)` に対する符号付き面積が正、`Polygon` の穴は負とする。閉じた輪郭の先頭点を末尾へ重複させない。
- 点列は float 変換後の幾何を基準に検証する。`Revolve` の閉じた profile だけは先頭・末尾の一致を閉鎖表現として使い、`Tube` / `Sweep` の閉路と `Loft` の断面では始点を末尾に重複させない。
- `CloseRing` は経路の末尾と先頭を接続する指定であり、端面の選択指定ではない。開路の `Tube` / `Sweep` は現在、両端面を生成する。部分 `Revolve` の `CloseEnds` と Hemisphere の `CloseBottom` もそれぞれ固有の面を制御する。

## 実装済みの主要機能

- 合成・編集: `append()`、`reserve()`、`clear()`、各種 transform、`transformUV()`、法線・接線の再計算
- 出力: OBJ、および `Material` を伴う OBJ / MTL の保存・エンコード
- Box 系: `BoxFace` による面選択、`BoxShell`、`BoxFrame`、`RoundedBox`、`ChamferedBox` など
- 汎用生成: `Extrude`、完全・部分 `Revolve`、一定半径・経路点別半径の開路・閉路 `Tube`、一定断面および経路点別 scale / twist の開路・閉路 `Sweep`、`HeightField`、runtime / compile-time `Loft`
- その他の基本プリミティブ一式。UV 球の API 名は `Sphere` とする。
- `Mesh3DBuilder` は既存メッシュと上記 generator を直接追加でき、通常形状には base、offset、offset + rotation、`Mat4x4` の配置体系を持つ。
- 一定断面 `Sweep` の初期断面方向は `Arg::initialXAxis`、経路点別変換版では `SweepOptions::initialXAxis` で指定する。

正確な overload、既定値、端面、巻き順、UV、異常入力の契約はヘッダを参照する。

## 実利用レビューからの申し送り

初見の利用者が 4 種類の構造物を作成・検査した記録が [`Claude outputs/REPORT.md`](<Claude outputs/REPORT.md>) にある。最終生成物では、生成失敗、z-fighting、部品間の隙間、縮退三角形、裏返り、意図しない孤立部品は検出されず、現在の builder とクラス冒頭の Doxygen は実用上よく機能した。

次のセッションへ引き継ぐ価値がある課題は `TODO.md` に整理した。新しい経路系 generator へ進む前提となる、角度、winding、点列の閉鎖表現、および失敗分類は公開ヘッダとテストで固定している。

実利用レビューを起点に検討した `Mesh3DAddResult` は全 add 関数へ展開済みである。単発の追加範囲と失敗理由は取得できるため、次は名前、材質、階層を含む永続的な部品情報の所有先を検討する。

報告中の `Quaternion::RotateX/Y/Z` の説明不足は現行 Doxygen ですでに解消済みである。また、現行 `Revolve` は連続する同一点を縮退面として生成せず、生成失敗として拒否する。この点は実装不具合として扱わず、入力契約の説明とテストの不足として評価する。

Gemini によるヘッダと簡略化済みモデリングコードのレビューからは、Builder のローカル座標系、開路や柱状形状の端面制御、複数色・材質と部品範囲の関係、扇形環などの 2D 断面生成を検討候補として採用した。リポジトリ全体を参照していないレビューなので、提案された API 外観は確定案として扱わない。

次の提案は既存 API との重複または前提の不一致があるため、そのまま実装しない。

- 2 点間の円柱は `Tube({ from, to }, radius, sides)` で表現できる。専用 overload より、まずこの用法の発見性を改善する。
- 方向付き `Extrude` は offset + rotation overload で表現できる。まず立面図を押し出す具体例を追加する。
- `addMesh(Mesh3D&&)` は、連続した頂点・index 配列を持つ非空 builder へ一般にゼロコピーで吸収できない。性能上の根拠なしに direct generator の代替としない。
- 3D CSG は topology、coplanar face、UV、tangent、数値的頑健性を伴う別規模の課題であり、今回の Mesh3D 拡張候補には戻さない。

## 次の候補と保留事項

- 次は新しい形状 generator より先に、開路の Tube / Sweep を起点として端面制御と options 型の共通契約を固める。詳細は `TODO.md` に集約する。
- その後の形状生成候補は callable を受け取る `HeightField()` とし、`Image` 固有 overload より先に評価する。
- bounding box / bounding sphere は `s3d::Box` / `s3d::Sphere` の実装後に扱う。
- レンダリング統合時に、`Vertex3D` の GPU レイアウト、頂点カラー、index 上限、CPU / GPU リソースの責務を決める。
- `Test/Manual/` の既存 Mesh3D レビュー資料は API の正本にしない。現行の Mesh3D 改修が一段落した後、必要な manual test をゼロベースで作り直す。

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
