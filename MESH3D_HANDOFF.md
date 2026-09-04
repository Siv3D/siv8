# Mesh3D 開発引き継ぎ

更新日: 2026-09-04

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
- 生成失敗は理由を `LOG_FAIL` へ出力する。factory は空メッシュ、builder は `false` を返し、builder の既存内容は変更しない。

## 実装済みの主要機能

- 合成・編集: `append()`、`reserve()`、`clear()`、各種 transform、`transformUV()`、法線・接線の再計算
- 出力: OBJ、および `Material` を伴う OBJ / MTL の保存・エンコード
- Box 系: `BoxFace` による面選択、`BoxShell`、`BoxFrame`、`RoundedBox`、`ChamferedBox` など
- 汎用生成: `Extrude`、完全・部分 `Revolve`、開路・閉路 `Tube` / `Sweep`、`HeightField`、runtime / compile-time `Loft`
- その他の基本プリミティブ一式。UV 球の API 名は `Sphere` とする。
- `Mesh3DBuilder` は既存メッシュと上記 generator を直接追加でき、通常形状には base、offset、offset + rotation、`Mat4x4` の配置体系を持つ。
- `Sweep` の初期断面方向は `Arg::initialXAxis` で指定する。

正確な overload、既定値、端面、巻き順、UV、異常入力の契約はヘッダを参照する。

## 実利用レビューからの申し送り

初見の利用者が 4 種類の構造物を作成・検査した記録が [`Claude outputs/REPORT.md`](<Claude outputs/REPORT.md>) にある。最終生成物では、生成失敗、z-fighting、部品間の隙間、縮退三角形、裏返り、意図しない孤立部品は検出されず、現在の builder とクラス冒頭の Doxygen は実用上よく機能した。

次のセッションへ引き継ぐ価値がある課題は `TODO.md` に整理した。特に次の 3 点は、形状追加とは独立して検討できる。

- `Polygon` / `Loft` の winding を「時計回り」だけでなく符号付き面積と最小例で一意に説明する。
- 部分 `Revolve` の正角方向が `Cylindrical` / `Spherical` / `Quaternion::RotateY()` と逆である点を、統一するか明示的な差として残すか決める。
- `Mesh3DBuilder` で追加した部品の頂点・三角形範囲を、検査や export 用に低コストで取得できる設計が必要か評価する。

報告中の `Quaternion::RotateX/Y/Z` の説明不足は現行 Doxygen ですでに解消済みである。また、現行 `Revolve` は連続する同一点を縮退面として生成せず、生成失敗として拒否する。この点は実装不具合として扱わず、入力契約の説明とテストの不足として評価する。

## 次の候補と保留事項

- 次の形状生成候補は Variable Tube、その次に Variable Sweep。詳細は `TODO.md` に集約する。
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
