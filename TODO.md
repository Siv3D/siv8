# Implementation TODO

実装を保留している API や、設計判断が必要な項目を記録する作業補助ファイルです。

- 公開ヘッダごとに、関連する項目をトピック単位でまとめます。
- 実装に着手できるよう、保留理由と決めるべき契約を記録します。
- 実装とテストが完了したトピックは、このファイルから削除します。
- 完了項目の履歴は残さず、未完了項目だけを維持します。

## `Siv3D/include/Siv3D/LineString.hpp`

### 可変幅・先細りの GPU 描画（Trail 系の知見待ち）

- Trail 系の実装で幅指定・接合・半透明・頂点生成を評価してから、LineString / Bezier2/3 / Spline2D の可変幅描画 API を設計する。
- 幅配列と点列の対応、弧長と時間による補間の違い、幅 0・負の幅、重複点、接合部と線端、閉じ目の契約を決める。内側の反転や半透明の重複をどこまで許容するか、テクスチャ座標と再利用可能な生成処理も含めて評価する。
- 既存の一定幅描画へ分岐や幅補間の費用を加えず、再利用可能な作業領域と直接頂点生成を使う。

## 2D 描画の拡張検討

- [拡張提案](docs/geometry2d/proposals/drawing-extensions.md)の用途・設計条件を基に採否を判断する。対象リリースと実装時期は未定。
- 四隅別角丸枠や Shape2D / Polygon 枠の LineStyle、楕円弧の矢印、LineString / 図形枠の JoinStyle は、自然な退化境界と半透明の見た目、描画の準備費用を比較する。
- 形状生成や複合パスは、保存形状と既存の変換・点列生成で不足する具体的な用途を確認してから設計する。

## RenderTexture / DepthTexture

- Windows / D3D11 で `ScopedRenderTarget2D.*` とフルスイートを実行し、ネスト・復元、各色フォーマットの描画、Scene の MSAA・解像度変換、読み書き競合、スコープ内の読み戻し・ミップ生成を検証する。[描画先とリソースの設計](docs/renderer2d/render-texture.md)を維持する。
- 深度は独立した `DepthTexture` として設計する。最初は単一サンプルの D32_Float を候補とし、色と独立したクリア、シェーダ参照、read-only 利用、reversed-Z の投影・比較・クリア値をそろえる。
- MRT は色の配列と任意の深度 1 枚を組み合わせる。シェーダ出力、各色出力のフォーマット・ブレンド、サイズ・サンプル数の適合性を検証する。
- `MSRenderTexture` は MSAA 用と resolve 後の単一サンプル用の色テクスチャを 2 枚内包する方針とする。深度は別に持ち、継承関係、resolve のタイミング、各操作の対象は実装前に決める。深度の resolve・Hi-Z の min/max 縮小を色のミップ生成と混同しない。

## NinePatch / 9-slice

- v0.6.16 との互換性を前提にせず、[新クラスの設計候補](docs/geometry2d/proposals/drawing-extensions.md#ninepatch9-slice)から再開する。Texture / TextureRegion を使う値型、分割位置と表示時の枠倍率の分離、小さい描画先での連続した縮小を検討する。
- 再開時に初期範囲とタイリングの採否・方式を決める。独立テクスチャへの切り出し、幾何分割、専用シェーダの準備費用・メモリ・フィルタリング品質を比較し、旧版の RenderTexture キャッシュをそのまま引き継ぐ前提にはしない。

## 2D 図形の Image 書き込み

今後の検証と機能拡張を扱う。採用済みの単色描画の仕様・API 対応表・再現可能な測定方法は [Image 書き込みガイド](docs/geometry2d/image-writing.md)を参照。
`.draw()` との差は機能単位で扱い、画素一致のために Image 書き込みの速度を落とさない。
再開時は継続検証を優先する。機能拡張・追加最適化の候補は、具体的な用途や測定結果に基づいて別途着手する。

### 継続検証

- **手動確認**。[共通ギャラリー](Test/Manual/ImageWriting.md)で CPU / GPU の輪郭と alpha を確認する。Bezier / Spline の品質・分割上限、Spline の閉じ目と線端、Triangle / Quad の内外枠と中央配置の JoinStyle も含め、実際に操作・確認した条件を記録する。自動テストやサンプルの構文検査と区別する。

### 将来の機能拡張候補

- **Line の破線・LineStyle**。最初の範囲を Line の paint / overwrite に限定し、各パターン、位相、端点、短い区間の仕様と最小 API を決める。線端と破線断片の重複を一度だけ合成し、既存の実線の性能も比較する。その後、LineString / 曲線、図形の枠へ段階的に広げ、曲線の分割点配置に依存しない長さ基準と閉じ目の位相を検討する。
- **LineString などへの JoinStyle 拡張**。最初は開いた折れ線・閉じた輪郭を対象に、既定値と線端・結合部の合成規則を決める。その後、Polygon / Shape2D の枠、Bezier / Spline の折り返しや cusp への適用を検討する。真の角と曲線近似の分割点を区別し、分割品質の変更で不自然な角が現れないようにする。Image と GPU は幾何的な意味を共有し、長い点列の一時確保、自己交差、穴、多重合成の費用を個別に評価する。

### 用途・測定結果に応じて選ぶ候補

- **変換付き描き込み・wireframe・添字付き三角形**。Polygon 等の変換済みコピーを作らない描き込みや、Shape2D の GPU と同じ添字付きメッシュを描く用途を評価する。Shape2D の頂点列を輪郭として塗る既存契約を保ち、添字付きメッシュは別 API として三角形の重複・共有辺・半透明の合成規則を決める。
- **グラデーション・頂点色・パターン・影**。利用頻度と補間・座標・alpha の仕様を先に決め、一種類ずつ採否を判断する。単色描画の共通経路へ一律に費用を加えない。
- **実用途に基づく追加最適化**。同じ画像への反復描画に加え、初期 alpha の異なる描画先やアプリ相当の呼び出し列を測定する。短い区間を多数連ねた Spline は、[曲線統合ベンチマーク](docs/geometry2d/bezier-drawing.md#spline-and-image-integration-costs)で密度分割の変換費用が頂点削減の効果を上回るケースを優先して切り分ける。CPU の準備・ラスタライズ・合成、一時確保、GPU 実行時間を区別する。再利用可能な公開コンテキストやキャッシュは、既存の [分離計測](tools/image-stroke-profile/profile.cpp)で費用と再利用の効果を確認してから検討する。

### GPU 側の追加調査

- **Circle arc の線端重複**。[共通ギャラリー](Test/Manual/ImageWriting.md)に記載された GPU キャップの重複を、短い弧・全周直前などで検証し、修正範囲を判断する。Image の一度だけの合成を参照しつつ、GPU 側の独立した作業として扱う。

## `Siv3D/include/Siv3D/JSON.hpp`

### 数値変換と入力検証

- `get<T>()` / `getOpt<T>()` / `getOr<T>()` は数値の範囲を検査しない。`256` → `uint8` は `0`、浮動小数点数から整数への範囲外変換は未定義動作になり得る。既存変換を互換性のため残すか、検査付き API を別に設けるか決める。頻繁に使われる取得処理へ範囲・非有限値の分岐を追加する前に明示的な承認を得る。
- 厳密な JSON 構文検査を必要とする用途には、コメントの受理と重複キーの扱いを選べる API が必要。深さ・入力サイズ制限とストリーミング対応は、想定する非信頼入力の規模から設計する。通常の DOM 読み込みとバリデータを混同しない。

### 保存失敗の報告

- `save()` / `saveMinified()` はファイルを開けない場合は false を返すが、`TextFileWriter::writeUTF8()` と `close()` が書き込み失敗を呼び出し側へ返さない。`BinaryFileWriter::close()` の結果を TextFileWriter 経由で JSON に伝播する設計が必要。原子的な置換・既存ファイルの保持まで保証するかも決める。JSON 内で保存後に再読込する回避策は採らない。

### BSON の互換性と検証

- 同梱ライブラリは `INT64_MAX` を超える `uint64` を BSON の型コード `0x11` で出力するが、標準では Timestamp を意味する。既存の往復互換性と外部 BSON 実装との相互運用を分け、拒否・文字列化・明示的拡張のいずれにするか決める。
- BSON reader は document length を読み捨て、文字列終端や boolean のバイト値の検証も十分でない。厳密なバイナリ入力検証を保証するなら、上流の対応状況を確認し、基盤 parser の更新または修正として扱う。ラッパ側で別の BSON parser を重ねない。

### 参照と例外の契約

- 子要素は親を所有しない参照。親の寿命終了や object/array の構造変更後に使うと無効になる。安全な長期保持は `clone()` で行えるが、所有する `JSON` と借用する view を型で分ける設計は別途検討する。常時 shared ownership を追加する前に費用と既存 API への影響を評価する。
- 編集・flatten・書式化・バイナリ出力などは基盤ライブラリの例外を返す一方、Parse/Load/get 等は `Error` へ変換する。例外体系を統一する場合はユーザー定義 serializer / reader の例外も含めた境界を決める。

## `Siv3D/include/Siv3D/TOML.hpp`

### Windows Debug の不正入力テスト

- テストアプリをクリーンビルドした Debug 全テストが `TOML.Load.invalid` で CRT アサート（`isctype.cpp:36`、`c >= -1 && c <= 255`）に停止する。どの入力・文字分類呼び出しが原因か、エンジンと依存ライブラリを含めて切り分ける。Release 全テストは通過する。警告整理とは別件として保留し、不正入力の拒否を検証するテストやバイト単位の fixture を弱めない。

## `Siv3D/include/Siv3D/Keyboard.hpp` / macOS の入力

- JIS / US の記号キーの対応を配列に合わせる。現在の変換表には `;`、`:`、`=`、`^`、`@`、括弧などの不整合があり、`0xDE` は未割り当て。`0xDC` の backslash / yen の統合も、JIS の `]` と yen の区別までは解決しない。物理配列と入力ソースのどちらで判定するか、切り替え・複数キーボードをどう扱うかを先に決める。既存 Input コードの意味と保存済みキー設定との互換性を維持する。
- Caps Lock の物理的な押下とロック状態の公開契約を分けて検討する。Fn / Globe は通常のキーイベントと同じではなく、`flagsChanged:` と OS のショートカット設定を含めた設計が必要。単にトークンを追加して対応済みとはしない。
- 1 フレーム内の押下・解放の取りこぼし対策は、イベント蓄積と `down()` / `up()` の同時成立の契約を決めてから行う。入力ソース変更後のキー名更新、IME の文字入力、メディア・輝度キーの収集も別途設計する。

## `Siv3D/include/Siv3D/Window.hpp` / macOS のスクリーン座標

- 全画面中の OS 表示倍率・解像度・リフレッシュレート変更と、物理的なモニター切断・再接続を検証する。残った画面で表示・操作を続けられ、再接続後も全画面往復と設定維持ができること。native detach の自動検査とは区別する。
- 最小サイズ設定後に異種 DPI 間を手動移動し、枠ドラッグの下限が backing pixel 指定を維持することを確認する。上下・左右配置の連続ドラッグとポインター座標も目視確認する。手順は [手動テスト](Test/Manual/SceneHighResolution.md) に集約する。

## `Siv3D/include/Siv3D/Scene.hpp`

### 描画の境界・失敗時の検証

- 有効な MSAA 列挙値が GPU 非対応となる環境で、`SceneMSAA.invalid_or_unsupported_preserves_scene` による `Error` と以前の状態の維持を確認する。
- Intel iGPU の `ScenePresentation.D3D11.scaling_filter_and_buffer_reuse` では、2×2 シーンを 750×750 に Nearest 拡大した境界が 1 ピクセル左に寄る。D3D11 のテクスチャ座標精度と照合し、厳密一致を保証するか、観測位置を見直すか決める。Linear・MSAA resolve・バッファ再利用とは切り分け、許容差を一律に緩めない。
- GPU 確保失敗後の状態維持と再試行を、D3D11 の確保段階と実際に確保が失敗する環境で検証する。Metal の [Configure / MSAA](Test/Test_SceneAllocationFailure.cpp)・[Window → Scene](Test/Test_WindowSceneSync.cpp) の失敗注入検査とは区別する。

### 残る手動確認

[共通プログラム](Test/Manual/SceneHighResolution.md) を使い、確認した設定・操作・モニター条件だけを完了扱いにする。描画、ホバー・クリック、復帰位置・寸法、SceneConfig の維持を確認する。

| 対象 | 未確認の条件・組み合わせ |
| --- | --- |
| Windows / D3D11 | 全 6 設定 × API / Alt+Enter 往復・混在の目視網羅。特に Fixed / SceneSize と PixelArt の異種 DPI 移動、最大化・最小化復元。真上・真下の独立配置、全画面中の複数回モニター移動後の復帰 |
| macOS / Metal | 異種 DPI 移動、最大化・最小化復元、全画面。実キーの Control+Command+F、メニューと緑ボタンの実クリック（緑ボタンは最大化） |
| 両方 | OS 倍率変更、横長・縦長・シーンより小さいウィンドウへの連続リサイズ、右ドラッグ中の解像度方針切り替え |

## Renderer2D / Triangle・Quad の JoinStyle

- Windows / D3D11 で [手動ギャラリー](Test/Manual/Renderer2DJoinStyle.md) の半透明・パターン・拡大表示を確認する。
- 凹形状・自己交差・内側輪郭の消失は Boost の topology 処理に残す。内外幅指定では領域の intersection / difference / union も含む。拡張する場合は非重複な領域分割と使用頻度を先に評価する。
- 実アプリの描画負荷で CPU コマンド投入と GPU 実行を個別に計測する。形状生成のマイクロベンチマーク倍率を FPS や GPU の速度向上率として扱わない。

## Renderer2D / 組み込みシェーダ最適化

- WARP の `Pattern.wave_rendering` / `Pattern.ripple_rendering` で、半透明の幅 0 の背景色と単色描画の比較に赤成分 1/255 の差が出る。端点色の補間・PMA・UNORM 変換の丸めを切り分け、必要な描画契約と画像比較の許容差を決める。画像全体の比較を一律に緩めない。
- WARP の `MSDFFont.atlas_dimensions` で、同じグリフを幅 768 のアトラスに移すと輪郭描画は最大 2/255、Print 用シェーダは最大 11/255 の差が出る。幅 128 / 512 と比較し、非 2 冪アトラスの UV・フィルタリング・微分・coverage 計算を切り分ける。既存の 1 階調許容を根拠なく拡大しない。
- [D3D11 / Metal 統合計画](docs/renderer2d/proposals/shader-optimization-plan.md)の初期 3 段階（A2 Truchet、A1 Pattern 色加算、A4 MSDF 除算）は、D3D11 の変更前後の描画 A/B、両バックエンドの GPU 時間評価を残している。中間命令の削減を実行時間の改善率とみなさず、描画比較と安定した反復計測を踏まえて最終判断する。新たな最適化の着手前には方針・変更箇所・期待結果を説明して承認を得る。
- ScopedQuadWarp2D: Windows で [ポストカードの手動テスト](Test/Manual/ScopedQuadWarp2D.md) の固定姿勢・Space 比較を目視確認する。自動テストの画像比較と区別する。
- Truchet の配置分岐は D3D11 の `[branch]` を先に測り、Metal に同じ変更が必要とは仮定しない。
- Triangle の skew 共通化と Weave の微分共有は画質差を評価してから判断する。Pattern UV の VS 移動、背景色・MSDF 寸法の CPU 前計算、Metal の half / アドレス空間変更は、残る負荷と互換性・状態管理の費用を根拠に再評価する。
- Windows の通常起動の全シェーダ再コンパイルを外す場合は、配布バイナリの生成・更新漏れを防ぐ手順を同時に整える。Metal の初回 PSO 費用は別途計測し、頻出組み合わせの事前生成、必要なら Binary Archive を検討する。

## `Siv3D/include/Siv3D/BigInt.hpp` / `Siv3D/include/Siv3D/BigFloat.hpp`

### 数値文字列の受理範囲

- 入力互換性に関わるため、以下を拒否するか、既存の受理範囲として公開するかを決める。
  - 両型は埋め込み NUL 以降を無視する。例えば `std::string_view{ "12\0x", 4 }` は 12 になる。
  - BigInt は空文字列、`"-"`、`"0x"` を 0 として受理する。非 ASCII の UTF-32 入力（例: `U"１２"`、`U"12あ"`）も 0 になる。
  - BigFloat は `"nanX"` など NaN 表記の後ろに文字が付いた入力を NaN として受理する。
- BigInt の基数接頭辞・先頭 0（16 進・8 進）や、両型の符号・空白の扱いも含め、コンストラクタと文字列代入で共通の文法を定める。
- 検査を追加する場合は入力走査のコストを確認し、正常入力・不完全な表記・埋め込み NUL・UTF-32 入力を両型でテストする。

### 不正な文字列を代入したときの状態

- 現在は BigInt が例外後に 0 になり、BigFloat は元の値を保持する。両型とも再代入は可能。
- BigInt も元の値を保持する契約へ揃えるかを決める。変更する場合は確保・コピーのコストと例外時の保証を確認する。

## `Siv3D/include/Siv3D/Quaternion.hpp`

### Squad 補間

- `squad()` と制御点生成 API の引数構成を設計する。
- 毎回制御点を再計算せず、補間区間ごとに制御点を再利用できる形にする。
- DirectXMath の `XMQuaternionSquad()` と `XMQuaternionSquadSetup()` の利用を前提にする。
- 単位クォータニオンの事前条件、符号の選択、補間係数をクランプするかを明文化する。
- 出力参照形式にするか、制御点を保持する専用型を導入するかを決める。

## `Siv3D/include/Siv3D/Mat3x3.hpp`

### 3D 回転した平面の射影行列生成

- [ポストカードの手動テスト](Test/Manual/ScopedQuadWarp2D.md) では、矩形を Quaternion で回転し、四隅を透視投影して `TryHomography()` に渡す処理を呼び出し側で組み立てている。矩形・3D 回転・焦点距離・表示中心から行列を生成するヘルパーの必要性を評価する。API 名・配置・採用は未決定で、具体案は `docs/renderer2d/proposals/` にまとめて採否を判断する。
- `ScopedQuadWarp2D` の描画状態管理と行列生成の責務を分け、回転中心、座標系・回転順、焦点距離、同次座標 W が 0 以下になる場合や退化時の失敗契約を決める。四隅からホモグラフィを解く方法と回転・射影から直接行列を作る方法を、計算量と既存 API の再利用の観点で比較する。頻繁に呼ばれる処理へ例外入力対策を追加する場合は事前に承認を得る。

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
