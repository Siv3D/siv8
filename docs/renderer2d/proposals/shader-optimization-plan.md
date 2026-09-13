# Built-in shader optimization assessment and plan

Status: **Proposed implementation scope; engine changes and performance acceptance pending.**
Release scope: Renderer2D の組み込みシェーダ。公開 API・カスタムシェーダ契約を
維持する局所変更を先に評価し、描画側の状態管理・補間インターフェース・画質を
変える案は別段階で扱う。D3D11 と Metal の調査結果をこの文書に集約する。

## 統合判断

初期実装の候補は **A2 Truchet、A1 Pattern の ColorAdd、A4 影なし MSDF**。
それぞれ独立した差分で評価する。定数配置・varying・描画状態を増やさず、
両コンパイラの中間表現に演算削減が現れるためである。
数学的な等価性は浮動小数点の画像一致を保証しないので、採用は描画比較と計測を経て決める。

| 扱い | 候補 | 判断理由 |
| --- | --- | --- |
| 初期評価 1 | A2 Truchet の距離式 | 両側で sqrt 削減。Metal の限定描画比較は一致。hash と AA を維持できる |
| 初期評価 2 | A1 Pattern の ColorAdd | 全 11 種に適用できる小変更。Metal の限定描画比較では最大 1/255 のチャンネル差 |
| 初期評価 3 | A4 影なし MSDF | 両側で除算削減。実フォントの倍率・変換・アトラス寸法を追加検証する |
| 次段階の設計・試作 | B1 QuadWarp | 大面積描画で期待できるが、補間成分と VS 定数管理が増える |
| D3D11 固有の計測候補 | B2 Truchet の分岐 | DXBC は現行で hash を常時計算。Metal AIR には既に条件分岐がある |
| 保留 | A3 Triangle の skew | Metal の式共通化でも境界に大きな色差を確認。画質判断を先に行う |
| 保留 | M1 Weave の微分共有 | Metal AIR の重複を除けるが、画像は bit 単位で一致しない。D3D11 の差と実効性能は未確認 |
| 負荷の根拠が得られてから | B3 背景色・B5 MSDF 寸法の CPU 前計算 | 追加の状態追従と定数管理を、局所変更だけでは不足する場合に評価する |
| 初期対象外 | B4 Pattern UV の VS 移動 | 公開済み custom shader 契約に対し、派生経路を増やす費用が大きい |

実行時間の改善率は未確定。DXBC の slots と Metal AIR の命令数は相互換算できず、
どちらも最終 GPU 命令・実レジスタ数・occupancy の測定ではない。
候補の削減量は、特記しない限り各案を単独適用した値で、単純加算できない。
未完了の作業一覧は [TODO](../../../TODO.md) が管理する。

## 段階的な作業計画

1. **比較条件を固定する。** 変更前後で描画面積、頂点数、draw call、blend、MSAA、
   sampler を揃える。大面積の少数図形と多数の小図形、状態を固定する描画と頻繁に
   切り替える描画を分ける。CPU 提出時間・定常 GPU 時間・初回生成時間を別々に測る。
   タイミングが安定しない条件では改善率を判定しない。
2. **A2 → A1 → A4 を個別に実装・検証する。** 一つずつ HLSL / MSL を揃え、
   Windows で対応する配布バイナリを再生成する。既存の Pattern / Renderer2D テストを使い、
   実フォントの描画検証が必要なら専用のテストに置く。各変更の境界比較と両ホストの
   全自動テストが通った段階で採用判断する。中間命令の減少だけで高速化を宣言しない。
3. **B1 QuadWarp の責務を設計する。** 専用の float3 補間と VS への定数供給を先に整理し、
   custom VS / PS の片側差し替えを含めた互換性を決める。行列積の VS 移動と UV scale / offset
   の行列合成は別差分にする。小 quad の悪化と CPU 状態更新の費用も採否に含める。
4. **バックエンド固有の費用を独立して評価する。** D3D11 の B2 はまず `[branch]` を試し、
   配置別 PS の増設はその結果を見て判断する。Windows の起動時 HLSL 再生成と Metal の
   初回 PSO 生成は別の問題として計測し、後述の生成・キャッシュ手順を設計する。
5. **保留案は必要性と画質の合意を得てから進める。** Triangle、Weave、half、PolkaDot / MSDF
   の AA 方針変更を局所最適化へ混ぜない。Pattern UV や CPU 前計算も、先行変更後に
   残った負荷を根拠に再評価する。

画像比較では、丸めに由来する最大 1/255 の差も自動的に許容しない。差の位置、件数、
透明合成後の結果、連続フレームでの見え方を確認する。既存の厳密な hash・端点・
状態復帰の検証を緩めて通す変更は避ける。許容差が必要なら当該比較に限定して根拠を記録する。
GPU 計測は温度・クロック変動や他の負荷の影響を受けるため、順序を交互にした反復比較と
ばらつきを確認し、通常のエンジン描画でも再確認する。

## Metal 側の照合

対象は [2d.metal](../../../macOS/App/engine/shader/metal/2d.metal)、
[fullscreen_triangle.metal](../../../macOS/App/engine/shader/metal/fullscreen_triangle.metal)、
それらの描画・定数バインド・パイプライン生成経路。
`metal -O3 -S -emit-llvm` による AIR 比較と、一時的なオフスクリーン描画で候補を照合した。
これは Xcode の製品ビルド全体やエンジン全自動テストの検証ではない。

画像比較は 1024 × 1024、RGBA8Unorm、MSAA なし、PMA blend、半透明の色と ColorAdd、
平行移動・斜交成分を持つ Pattern UV 変換を使った限定条件。
MSDF は合成した距離テクスチャによる比較であり、実フォントのアトラスや描画経路は未検証。
比較結果はこの条件に限られ、全入力での一致を示すものではない。

| 候補 | Metal AIR の差 | 限定描画比較・解釈 |
| --- | --- | --- |
| A1 ColorAdd 集約 | 全 11 種で vector fmul と fadd が各 1 個減る | 最大チャンネル差 1/255。PMA 化の位置は維持 |
| A2 Truchet 距離式 | シェーダ全体の scalar sqrt が 3 → 2 | 比較画像は一致。全 layout・幅端点等は採用前に検証 |
| A3 Triangle skew 共通化 | float2 dot が 8 → 6 | 最大チャンネル差 33/255。step の境界変化を確認 |
| A4 MSDF 除算 | 通常・Outline の vector fdiv が 2 → 1 | 通常 MSDF の比較画像は一致。Outline の画像は追加検証。OutlineShadow の除算数は変わらない |
| B2 Truchet 配置分岐 | 現行 AIR は layout の switch 内の Random ブロックに hash を置く | 最終 GPU コードが同じ分岐を保つかは未確認。D3D11 の常時計算を Metal に一般化しない |
| M1 Weave 微分共有 | float2 fwidth が 2 → 1 | 最大チャンネル差 1/255。uv + 0.5 の丸め差に注意 |

A3 は定数三角関数を維持したまま、中心とオフセットへ skew を分解した実験。
別に行った tan / sin の小数定数化も画像の境界を変えたが、両案は同じ変更ではない。
定数化だけを速度改善として採用しない。

M1 は `PS_PatternWeave` の既存 `fwidth(uv)` から幅を求め、
`Pattern_CheckersFiltered(uv + 0.5, ...)` に渡す案。
数学上は定数平行移動で微分は変わらないが、浮動小数点では完全には一致しない。
Checker 自体の積分式・parity フィルタは維持する。再利用可能な幅を受け取る計算を
共通にし、通常の Checker 呼び出しは幅を計算して渡す形にできる。

GPU 時間の予備比較はばらつきが大きく、速度改善の根拠には採用していない。
使用した命令表示ツールでは最終 GPU コードを逆アセンブルできず、AIR 以降の
除去・定数畳み込み・分岐形態は未確認。ソースや AIR の見た目から実行回数を断定しない。

### Metal 固有の候補

- **頂点配列のアドレス空間。** `VS_Shape` / `VS_QuadWarp` / `VS_Pattern` の
  `constant VSInput*` は vertex ID ごとに異なる項目を読む。
  `const device VSInput*` はアクセス方法に適するが、予備比較では明確な速度差はなかった。
  定数バッファは同じ理由で device へ変更しない。
- **half の限定利用。** 最初の評価対象は画面転送の `texture2d<half>` と出力色。
  UV・位置・微分・MSDF 距離・Truchet の hash 入力は float を維持する。
  一般の色計算へ広げる前に透明合成、階調、値域と既存の shader interface を確認する。
  現段階では画像・速度とも未検証。
- **初回 PSO 生成。** [MetalRenderPipelineState::get](../../../Siv3D/src/Siv3D-Platform/macOS/Siv3D/Renderer/Metal/RenderPipelineState/MetalRenderPipelineState.mm)
  はキャッシュミス時、描画処理から同期生成する。通常利用する組み合わせの事前生成を
  先に検討し、残る費用に応じて Binary Archive を評価する。全組み合わせの列挙や
  大規模な永続キャッシュから始めない。起動時間への移動と定常 GPU 時間の改善を区別する。

アドレス空間・16-bit 型・uniform 計算の扱いは
[Apple の Metal 最適化指針](https://developer.apple.com/videos/play/wwdc2020/10632/)、
事前コンパイルは [Binary Archive](https://developer.apple.com/documentation/metal/metal-binary-archives)
を参照する。描画中に一定の背景色などは Metal のコンパイラも前計算できるため、
B3 の DXBC 削減量をそのまま Metal の費用削減とみなさない。

## D3D11 の比較結果

以下は Windows の D3DCompile と D3DDisassemble による **DXBC 命令列の比較**。
GPU の実行時間、消費電力、実レジスタ数、occupancy の改善を示す数値ではない。

| 候補 | D3D11 で確認した差 | 統合判断の要点 |
| --- | --- | --- |
| A1. Pattern の色加算を補間後へ集約 | 全 11 種で各 1 slot 減。Stripe は 24 → 23、仮想 temp は 3 → 2 | 初期候補。透明色・ColorMul・ColorAdd を確認 |
| A2. Truchet の距離を二乗値で比較 | 59 → 57 slots、scalar sqrt が 3 → 2 | 配置・seed・AA 式を維持できる |
| A3. Triangle の skew を中心とオフセットに分解 | 34 → 31 slots、仮想 temp は 4 → 3 | Metal の境界差を踏まえ初期実装から外す |
| A4. 影なし MSDF の除算をまとめる | 通常 18 → 17、Outline 20 → 19。vector div が 2 → 1 | Shadow/Print は改善しない。精度差を確認 |
| B1. QuadWarp の同次座標を VS へ移す | PS 9 → 6、VS 8 → 10。UV 変換を行列に合成する追加案では PS 5 | varying が float2 → float3。VS 定数の管理が必要 |
| B2. Truchet の uniform branch / 配置別専用化 | 現行は全配置でハッシュ命令を実行。定数専用化では Random 52、Uniform 33、Alternating 39 slots | 分岐またはシェーダ数の増加と比較する |
| B3. Pattern 背景色の CPU 前計算 | 調べた 4 種で各 3 slots 減 | 複数の状態に依存。A1 と削減量が重なる |
| B4. Pattern UV の VS 前計算 | 調べた 4 種で PS が各 2～3 slots 減 | カスタムシェーダとの互換性が最大の制約 |
| B5. MSDF の逆テクスチャ寸法を定数化 | 通常 18 → 16、Outline 20 → 18、Shadow 29 → 26、Print 30 → 27 | アトラス交換・拡張時の状態追従が必要 |

## D3D11 側の調査範囲と根拠

- 主対象は [2d.hlsl](../../../WindowsDesktop/App/engine/shader/d3d11/2d.hlsl)
  と [fullscreen_triangle.hlsl](../../../WindowsDesktop/App/engine/shader/d3d11/fullscreen_triangle.hlsl)。
  example の grayscale は組み込み描画経路の対象から除外。
- [登録・コンパイル処理](../../../Siv3D/src/Siv3D-Platform/WindowsDesktop/Siv3D/EngineShader/D3D11/CEngineShader_D3D11.cpp)
  の 30 エントリーポイント（4 VS、26 PS）を調査。
- [CShader_D3D11](../../../Siv3D/src/Siv3D-Platform/WindowsDesktop/Siv3D/Shader/D3D11/CShader_D3D11.cpp)
  の `vs_4_0` / `ps_4_0`、[HLSL::CompileOption::Default](../../../Siv3D/include/Siv3D/HLSL.hpp)
  の `OptimizationLevel3 | WarningsAreErrors` に合わせた。
  [HLSLCompiler](../../../Siv3D/src/Siv3D-Platform/WindowsDesktop/Siv3D/Shader/D3D11/HLSLCompiler.cpp)
  と同じ system `d3dcompiler_47.dll` を使用し、BOM はテキスト読み取り時に除いた。
- 基準の全エントリーポイントと各実験用の HLSL 文字列をコンパイルした。
  登録済み `.vs` / `.ps` の逆アセンブルも照合し、コメントを除く宣言・命令列は
  全 30 本で基準と一致した。古い配布バイナリによる差を最適化効果に含めていない。
- `slots` は [D3DDisassemble](https://learn.microsoft.com/en-us/windows/win32/api/d3dcompiler/nf-d3dcompiler-d3ddisassemble)
  出力末尾の approximate instruction slots、`temp` は `dcl_temps`。
  後段のドライバによる変換や、スカラ・ベクトル命令の費用差は含まない。
- エンジンの HLSL、C++、配布シェーダバイナリは変更していない。
  D3D11 描画 A/B、GPU timestamp、Windows のエンジン全テストは未実施。
  Metal の限定比較は前節を参照。これは調査・文書化であり、実装完了報告ではない。

## A. 現行インターフェースのまま評価できる案の詳細

### A1. Pattern の ColorAdd を補間後の 1 回にする

対象: `Pattern_BackgroundColor()` と全 `PS_Pattern*`。
現在は foreground と background に別々に `g_colorAdd * alpha` を加え、
その後に `lerp` している。

`A(C) = C + g_colorAdd * C.a` は、描画中に固定された `g_colorAdd` に対する線形写像。
したがって `lerp(A(P), A(B), c) = A(lerp(P, B, c))`。
各色の PMA 化は今の場所に維持し、ColorAdd だけを補間の後へ移す。

```hlsl
// Background helper: color multiplication and PMA conversion only.
return s3d_premultiplyAlpha(g_patternBackgroundColor * g_patternBackgroundColorMul);

// Preserve each pattern's interpolation direction and coverage.
const float4 primary = input.colorPMA;
const float4 background = Pattern_BackgroundColor();
return s3d_shapeColor(lerp(primary, background, c));
```

逆向きの補間を使う Halftone 等は、その向きを維持する。
11 種すべてで 1 slot 減を確認。PolkaDot / Wave / Ripple / Stripe / Grid / Checker は
`dcl_temps` も 3 → 2。追加定数、VS 切り替え、draw call の増加は不要。
浮動小数点の演算順は変わるので、半透明の両色、ColorAdd、色乗算、重ね描き、
大きい色成分の出力を比較する。PMA 化自体を補間後へ移してはいけない。

### A2. Truchet の円弧距離を sqrt 前に比較する

対象: `PS_PatternTruchet` の `distance`。

```hlsl
// Current
min(abs(length(q) - 0.5f), abs(length(q - 1.0f) - 0.5f))

// Candidate
abs(sqrt(min(dot(q, q), dot(q - 1.0f, q - 1.0f))) - 0.5f)
```

`q` は反転後も単位セル内。2 つの円の中心間距離は `sqrt(2)`、半径は `0.5`。
`a = length(q) <= b = length(q - 1)` とすると、`a >= 0.5` では明らかに
`abs(a - 0.5) <= abs(b - 0.5)`。
`a < 0.5` でも三角不等式 `a + b >= sqrt(2) > 1` により同じ大小関係になる。
つまり近い中心の距離を選んでから sqrt と半径との差を計算できる。

AA 用の `length(fwidth(uv))` は維持するため、シェーダ全体の scalar sqrt は
3 → 2。59 → 57 slots、temp は 3 のまま。
セルを対角線で折ってから `length` を取る別案は 58 slots で、上式の方が短い。
A1 と同時適用した実験は 56 slots。

単位セルの端を含む 513 × 513 点で binary64 の数式比較は差 0。
これは GPU float の画像一致の代用にはならない。
全配置、負のセル座標、既存 seed、セル境界、線幅 0 / 最大、回転と縮小で比較する。
セル hash と、その bit pattern に関する既存テストは変更不要。

### A3. Triangle の 4 サンプルに共通する skew を取り出す

対象: `PS_PatternTriangle`。現行は 4 点それぞれで `Pattern_Skew` を計算する。
これは線形変換なので、元の `fw = fwidth(uv) * 0.25` を維持して次の形にできる。

```hlsl
const float2 center = Pattern_Skew(uv);
const float2 offset1 = Pattern_Skew(fw);
const float2 offset2 = Pattern_Skew(float2(-fw.x, fw.y));
const float2 s1 = center - offset1;
const float2 s2 = center + offset1;
const float2 s3 = center + offset2;
const float2 s4 = center - offset2;
```

34 → 31 slots、temp は 4 → 3。4 点のフィルタ方式を維持できる。
ただし後続が `frac` / `step` なので、丸め差が境界のサンプルを反転させることがある。
この場合、出力差は丸め誤差程度とは限らず、1 サンプル分の coverage 変化になり得る。
平行移動アニメーション、セル境界、大きい offset、縮小時のちらつきを重点確認する。

### A4. 影なし MSDF の除算をまとめる

対象: `MSDF_Init`。現在のスケールは、成分ごとに
`(16 / textureSize) * (0.5 / fwidth(uv))` を計算してから加算する。

```hlsl
st.scale = dot(float2(8.0f, 8.0f), rcp(st.textureSize * fwidth(uv)));
```

`st.invTextureSize` が他で不要な通常・Outline では、コンパイラの不要コード除去により
vector div が 2 → 1。各 1 slot 減。
Shadow / OutlineShadow / Print は影の offset に逆寸法を使うため、この方法の効果を
一律に期待しない。実験した Shadow と Print は slots が変わらなかった。
Glow は `MSDF_Init` を使わず、この案の対象外。

バッファやフォント描画側の状態を増やさずに試せる。
フォント倍率、回転、非等方変換、アトラス寸法を変え、輪郭 coverage の丸め差を確認する。
縮小時のフィルタ品質変更と同じ変更に混ぜない。

## B. 描画側との協調が必要な案

### B1. QuadWarp の同次座標だけを VS へ移す

対象: `VS_QuadWarp` / `PS_QuadWarp` と
[D3D11 Renderer2D](../../../Siv3D/src/Siv3D-Platform/WindowsDesktop/Siv3D/Renderer2D/D3D11/CRenderer2D_D3D11.cpp)
の `flush()`、`QuadWarpParameters`。

現行 VS は描画座標 `float2` を渡し、PS が 3 × 3 逆 homography を掛ける。
VS で `mul(float3(input.position, 1), H)` を求め、結果の `float3` を補間し、
PS では **補間した後で** `t.xy / t.z` を計算する。
線形写像と補間は交換できるが、除算とは交換できない。
最終 UV を VS で除算して単純補間する変更は別物で、射影マッピングを壊す。

実験では PS 9 → 6、VS 8 → 10 slots。
さらに `s = g_quadWarpUVTransform.xy`、`o = g_quadWarpUVTransform.zw` を使い、
CPU 側で行列の列を `H'u = s.x * Hu + o.x * Hz`、
`H'v = s.y * Hv + o.y * Hz`、`H'z = Hz` と合成すると、
PS の UV scale / offset の mad も除ける。この PS 単体の実験は 5 slots。

必要な設計変更:

- effect buffer は現在 PS の b1 のみに bind される。同じ内容を VS へ渡す方法と、
  カスタム VS の定数スロット・復帰の扱いを決める。
- varying は float2 → float3。同じ TEXCOORD レジスタ内でも補間成分は 1 つ増える。
  ピクセルが少ない小 quad では、VS と補間の増加が削減を上回る可能性がある。
- shape / texture / Pattern の共通 `PSInput` を一括変更せず、QuadWarp 用の境界を作る。
  `SV_POSITION.w` を変更すると頂点色の補間にも影響するので、現行の位置出力を維持する。
- 大きい座標や斜めの強い quad、UV region、四色 tint、transform / viewport、
  custom VS または PS の片側差し替えを比較する。浮動小数点では数学的等価性だけで
  seam が消えることや画像が一致することは保証できない。

[Pattern 座標設計](../pattern-coordinates.md) で VS_Pattern と VS_QuadWarp を
独立させた境界を活用できる。

### B2. Truchet の通常配置で不要な hash を省く

現行 HLSL の `if (layout == 0u)` は、O3 の DXBC では `if` 命令にならず、
hash と checker 判定の両方を計算して `movc` 等で選択していた。
Uniform / Alternating でも整数 multiply・xor・shift が残る。

最初の `if` のみに `[branch]` を付けた実験では、hash が `if_z` ブロックに入り、
非 Random では回避できる形になった。静的な全体 slots は 59 → 61 に増える。
両側の命令を含む静的合計を、実行する命令数と取り違えないこと。
layout は定数バッファ由来で draw 内では uniform。両分岐の外で行う UV 微分は維持する。

比較用に layout をコンパイル時定数にすると、Random / Uniform / Alternating は
それぞれ 52 / 33 / 39 slots。実装する場合は専用 PS の登録、選択、配布バイナリが増える。
まず `[branch]` の GPU 時間を 3 配置すべてで測り、専用化に見合う差が残るか判断する。
大きく複雑な hash の置換や事前テクスチャ化を先に行う必要はない。

### B3. Pattern の背景色を描画状態変更時に前計算する

`Pattern_BackgroundColor()` は各ピクセルで同じ背景色乗算・PMA 化・色加算を計算する。
その結果を定数として与えた PS の実験では、Stripe / PolkaDot / Weave / Truchet で
各 3 slots 減を確認した。これは CPU 側を含む統合実装ではない。

依存するのは `PatternParameters` だけでなく `ColorMul` と `ColorAdd`。
パターン生成時に確定させず、`flush()` で関係する状態が変わったときに更新する設計が必要。
コマンド順、同じパターンで色だけ変更する draw、フレーム復帰を含める。
[共通定数レイアウト](../../../Siv3D/src/Siv3D/Renderer2D/Renderer2DCommon.hpp) と
[payload の既存検証](../pattern-payload.md) を確認し、生の背景色を読む custom PS の
意味を上書きしない。

A1 を採用した場合、背景色には ColorAdd 前の PMA 色を前計算する案が自然。
A1 の 1 slot とこの 3 slots は削減対象が重なるため、再コンパイルして判断する。
CPU 状態管理の増加に対して、小さい描画では得が少ない可能性がある。

### B4. Pattern_UVTransform を VS へ移す

アフィン変換なので、数学的には描画座標を変換してから補間できる。
変換済み UV を受け取る仮定の PS では、Stripe 24 → 22、PolkaDot 25 → 23、
Weave 54 → 52、Truchet 59 → 56 slots。
削減候補は PS の 2 つの mad を中心とし、VS の処理と定数 bind は増える。

ただし [PatternParameters の公開契約](../../../Siv3D/include/Siv3D/Pattern/PatternParameters.hpp) は、
組み込み Pattern PS と組み合わせる custom VS が変換前描画座標を渡すこと、
PS のみ差し替えた場合も同じ座標が来ることを明記している。
既存の `uv` をそのまま変換済み UV に変える案は契約を壊す。
採用するなら built-in 同士の経路と custom 混在時の経路を管理するか、
公開契約の変更を先に判断する。2～3 slots のために派生版と分岐を大きく増やすのは慎重にする。

### B5. MSDF の GetDimensions と逆寸法計算を定数へ移す

`MSDF_Init` の `GetDimensions` は DXBC の `resinfo` と、その後の vector div として残る。
CPU が保持する逆テクスチャ寸法を専用定数で渡した実験では、通常・Outline が各 2 slots、
Shadow・Print が各 3 slots 減。テクスチャの sample 数は変わらない。
`resinfo` は[リソース寸法を得る命令](https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/resinfo--sm4---asm-)で、
画像 texel を読む追加 sample として数えてはいけない。

[GlyphCacheManager::updateTexture](../../../Siv3D/src/Siv3D/Font/GlyphCache/GlyphCacheManager.cpp)
は画像サイズが変わると DynamicTexture を作り直す。
逆寸法は fontSize や初期アトラス寸法に固定せず、実際に PS t0 へ bind するテクスチャに
追従させる。フォント混在、アトラス拡張、カラーフォント fallback、custom PS の復帰を確認する。
実験では独立した仮の b2 を使っただけで、製品の空きスロットとして提案しているわけではない。
まず A4 の小変更を評価し、状態追加の費用に見合うか測定する。

## 起動時の改善: シェーダ生成を通常起動から外す

`CEngineShader_D3D11::init()` の再コンパイルブロックが `#if 1` のため、
通常起動で全 30 エントリーポイントをソースからコンパイルし、ファイルに保存してから読み込む。
フレーム中の描画性能とは別に、起動時間と配布先の書き込み要件を減らせる候補。

生成・配布するエントリーポイントの一覧を一元化し、開発用の明示的な再生成手順または
ビルド手順でバイナリを用意する。通常起動は配布バイナリのロードにする。
単に `#if 0` にするだけでは、HLSL の変更を反映し忘れる問題が残る。
今回の照合では配布済みバイナリの命令列は一致していたが、将来の生成漏れを防ぐ仕組みは必要。
この案について起動時間の短縮量は測定していない。

## 品質を伴う別判断と、優先しない変更

- **PolkaDot の微分元。** 現在は `length(repeat)` の値を微分し、その微分ベクトルの
  length を取る。円の中心を囲む対称な 2 × 2 quad では距離が全サンプルで等しくなり、
  幾何が潰れていなくても `fw = 0` になり得る。半径がそのサンプル距離に一致すれば
  smoothstep の両端も入力値も等しい。Halftone / Ripple / Truchet の連続 UV 由来の幅を
  参考に品質を確認する価値がある。ただし `length(fwidth(uv))` 等への置換は幅と見え方を変える。
  今回は GPU 画像で再現しておらず、性能最適化に紛れて epsilon を足す変更は提案しない。
- **MSDF の強い縮小。** 現行 `st.scale` に下限はない。
  [msdfgen の参照実装](https://github.com/Chlumsky/msdfgen#using-a-multi-channel-distance-field)
  は screen pixel range を最低 1 としている。小さい文字・大きい縮小変換を画像比較して、
  採用する AA 方針を決める。これは現在の品質が必ず誤りという断定ではなく、A4 / B5 と別の判断。
- **Wave の sin/cos。** 現行でも DXBC は 1 つの `sincos` 命令。
  HLSL を明示的な `sincos()` に書き換えても 32 slots のまま。
- **Triangle の定数三角関数。** `Pattern_Skew` 内の tan / sin は定数畳み込み済み。
  小数リテラル化だけを速度改善として扱わない。
- **基本 Shape / Texture。** PS はそれぞれ 2 / 4 slots。Texture は sample 1 回。
  フラグで機能を切る mega shader や追加の早期 return より、現在の単純な個別 PS を維持する。
- **MSDF の sample。** 通常 / Outline は既に 1 回、Shadow / OutlineShadow / Print は
  異なる UV を使うため 2 回。Outline のために同じ MSDF を二重サンプルしているわけではない。
  Glow の `pow` は可変指数で、生成された MTSDF の alpha を使う別経路。
  [生成処理](../../../Siv3D/src/Siv3D/Font/GlyphRenderer/MSDFGlyphRenderer.cpp) も距離範囲 16 を
  使用しており、`MSDF_PixelRange = 16` は単独で削除・変更すべき定数ではない。
- **全画面表示。** 既に 3 頂点の fullscreen triangle で、PS は sample 1 回。
  [BackBuffer](../../../Siv3D/src/Siv3D-Platform/WindowsDesktop/Siv3D/Renderer/D3D11/BackBuffer/D3D11BackBuffer.cpp)
  は letterbox と Linear / Nearest を扱う。座標分岐のビット演算化や、
  一律の `Load` / copy への置換を優先しない。
- `fwidth` の norm の変更、距離の二乗化に合わせた smoothstep の再設計、half 精度化、
  discard の追加は画質・精度・パイプライン動作を変え得る。等価な式整理とは分ける。
  NaN / Inf / 極端入力の防御演算を頻出経路へ加える場合はリポジトリ指示どおり事前承認が必要。

## 採用前の検証順序

1. 初期候補の A2、A1、A4 を独立した差分として評価する。A3 は画質判断を伴う別段階にする。
   [Pattern tests](../../../Test/Test_Pattern.cpp) の既存検証を利用し、変更箇所に応じて境界を補う。
   [Pattern collection](../../../Test/Manual/PatternCollection.md) で Truchet の各配置と
   Triangle の動く境界も見る。保留中の M1 は Weave の幅・gap・交差部を追加確認する。
2. B1 と B2 は、画面を広く覆う少数の quad と、多数の小さい quad を分けて測る。
   Pattern の模様別、Truchet の配置別、文字の通常 / 影 / outline を混ぜずに基準を取る。
   同じ描画面積、blend、MSAA、sampler、draw call 数で比較し、GPU timestamp の
   disjoint を除外する。CPU 提出時間と GPU 時間を分け、VSync の待ち時間を結果に含めない。
3. B3～B5 は、定数 bind / upload の回数と draw call 数も比較する。
   状態変更の多い描画で CPU の悪化を GPU の減少だけで隠さない。
   [Renderer2D CPU benchmark](../../../Test/Manual/Renderer2DPipelineState.md) は CPU 側の確認に使えるが、
   GPU 時間の代用にはならない。
4. 実装を採用したホストで全自動テストを実行する。
   Windows は `./WindowsDesktop/run-tests.ps1`、macOS は `./macOS/run-tests.sh`。
   custom shader、描画状態復帰、Pattern と QuadWarp の交互描画も確認する。

一般方針として、計算頻度を下げられるものを前段へ移す考え方は
[Microsoft の HLSL 最適化指針](https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-optimize)
とも一致する。本件では既存の呼び出し経路・公開契約・O3 の出力を優先して候補を絞った。
実装時は先頭の統合判断と段階的計画に従い、採用した項目の知識を既存の subject guide へ
反映する。未完了項目は TODO に残し、完了後はこの提案を履歴として複製保存しない。
