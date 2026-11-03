# Texture processing extensions and backend research

**Status: initial RenderTextureOps operations are implemented. This document retains
unadopted extensions, alternative backends, and the diagnostic that informed the
initial implementation. Additional release scope is undecided.**

Use the [RenderTextureOps guide](../render-texture-ops.md) and
[public header](../../../Siv3D/include/Siv3D/RenderTextureOps.hpp) for the adopted API.
The initial release uses native copy and raster filtering; Compute, MPS, ToScene
operations, and per-draw effect shaders are outside that release scope.
Open work belongs in [TODO](../../../TODO.md).

## 他エンジンから取り入れられる設計

| 参照先 | 確認できる特徴 | Siv3D への示唆 |
| --- | --- | --- |
| Unity [CopyTexture](https://docs.unity3d.com/6000.0/Documentation/ScriptReference/Graphics.CopyTexture.html) / [Blit](https://docs.unity3d.com/6000.0/Documentation/ScriptReference/Graphics.Blit.html) | 転送と shader を通す描き込みを分離 | `Copy` と resize/format conversion を区別する。Siv3D では既存の描画状態を復帰する |
| Unity URP [Bloom shader](https://github.com/Unity-Technologies/Graphics/blob/master/Packages/com.unity.render-pipelines.universal/Shaders/PostProcessing/Bloom.shader) | 分離 Gaussian、bilinear による tap 集約、複数の down/up filter | raster 経路も有力。Bloom と汎用 Gaussian は同じ契約にしない |
| Godot [copy effects](https://github.com/godotengine/godot/blob/master/servers/rendering/renderer_rd/effects/copy_effects.cpp) | compute と raster の Gaussian・mipmap 経路を持つ | renderer・機能条件に応じた実装選択。Compute 一律採用を前提にしない |
| Unreal [Render Dependency Graph](https://dev.epicgames.com/documentation/unreal-engine/render-dependency-graph-in-unreal-engine) | パスの依存関係、transient resource、barrier、async compute を管理 | 入出力・上書き範囲・一時寿命を内部パスで表す。簡単な helper の利用者に graph 構築を要求しない |
| bgfx [API](https://bkaradzic.github.io/bgfx/bgfx.html) / [Bloom example](https://github.com/bkaradzic/bgfx/blob/master/examples/38-bloom/bloom.cpp) | blit/compute capability、縮小・拡大 pyramid の効果 | 機能照会と fallback を持つ。view 内で blit/compute が draw より先になる実行規則は、Siv3D の呼び出し順序へそのまま持ち込めない |
| Filament [Bloom downsample](https://github.com/google/filament/blob/main/filament/src/materials/bloom/bloomDownsample.mat) | 重なる領域を重み付きで集約する 13 sample の縮小 | HDR の細い輝点の安定性も評価する。Bloom 向け RGB filter の alpha 規則を汎用 RGBA 処理へ流用しない |

Godot の [Compute の利用条件](https://docs.godotengine.org/en/stable/tutorials/shaders/compute_shaders.html)も
renderer に依存する。v0.8 では Metal / D3D11 を個別に評価し、他エンジンの採用だけを性能根拠にしない。

## ネイティブ機能の使い分け

| 機能 | 利用可能性 | 制約と方針 |
| --- | --- | --- |
| D3D11 `CopySubresourceRegion` / Metal blit copy | 同サイズ・同形式の mip 0 の転送に適する | resize、色変換、alpha 合成はしない。全 mip を対象とする `CopyResource` とは区別 |
| ネイティブ mip 生成 | 後続の縮小 sample が mip を利用する用途 | source の下位 mip を更新する副作用があり、必要以上の段を作る場合もある。別 destination への縮小そのものではない |
| MSAA resolve | 複数 sample を単一 sample へまとめる | 空間的な縮小とは別。既存の明示 resolve 契約を維持 |
| Metal の load/store action | 全面上書き時の不要な旧画素 load を避ける | 部分更新では旧画素が必要。大幅な高速化を保証するものではない |
| Metal Performance Shaders | Gaussian、bilinear/Lanczos resize、min/max、色変換など | OS 固有の kernel・精度・境界規則を揃えられる効果で候補。既存 command buffer を使う |
| Direct2D effect と DXGI surface の相互運用 | Windows の Gaussian、色変換など | device/context、対応形式、alpha、DPI、境界規則の整合が必要。初期は共通 HLSL/MSL 実装を優先 |

ネイティブコピーの制約は [D3D11 CopyResource](https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-copyresource)
と [Metal texture copy](https://developer.apple.com/documentation/metal/mtlblitcommandencoder/copy(from:sourceslice:sourcelevel:sourceorigin:sourcesize:to:destinationslice:destinationlevel:destinationorigin:))
を参照。mip 0 だけを対象とする新しい `Copy` を採用する場合は、両側の mip 数が違う場合も含めて仕様化する。

[MPSImageGaussianBlur](https://developer.apple.com/documentation/metalperformanceshaders/mpsimagegaussianblur)
は高速な近似 Gaussian で、任意に定義した離散 kernel と同一の結果を約束する API ではない。
sigma ごとの kernel 作成も毎回行わず再利用する。
[MPSImageLanczosScale](https://developer.apple.com/documentation/metalperformanceshaders/mpsimagelanczosscale)
は高品質 resize の候補だが、コストと輪郭の ringing を含めて評価する。
MPS の in-place fallback による texture の置き換えは、Siv3D の共有ハンドルへ透過的に適用できるとは限らない。

[Direct2D Gaussian blur](https://learn.microsoft.com/en-us/windows/win32/direct2d/gaussian-blur) は
品質設定による事前縮小や、soft/hard border による異なる境界処理を持つ。
[DXGI surface からの bitmap 作成](https://learn.microsoft.com/en-us/windows/win32/api/d2d1_1/nf-d2d1_1-id2d1devicecontext-createbitmapfromdxgisurface(idxgisurface_constd2d1_bitmap_properties1_id2d1bitmap1))
で GPU 相互運用はできても、Siv3D の Gaussian 契約に自動的に一致するわけではない。

## Gaussian と大きなぼかしのアルゴリズム

### 基本にする 2 パス描画

分離可能な Gaussian は、2 次元の全 tap を読む代わりに水平・垂直の 2 パスに分けられる。
半径 R のとき、1 軸の `2R + 1` tap は、隣接係数をまとめると
`1 + 2 * ceil(R / 2)` 回の線形 sample で表現できる。
この手法は [bilinear Gaussian の解説](https://www.rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/)
と旧実装の双方にある。

改善対象は、固定 kernel の追加だけでなく、全画面 triangle、専用状態、明示 LOD、
中間画像の形式・寿命、準備の再利用、不要な clear/load の省略まで含む。
精度は sample の補間精度と中間形式にも依存する。2 パスの RGBA16F と、
float の共有メモリで中間値を保持する 1 dispatch は、同じ係数でも bit 単位では一致しない。

### 保留した Compute の再検討条件

以下は将来の再検討対象であり、今回の初期実装の必須工程には含めない。
代表的なアプリでボトルネックが確認され、リソース準備・最終コピーまで含む利益が
実装と保守の複雑さに見合う場合に再開する。

1. 小さい画像で render pass の準備費用が相対的に大きい。
2. 複数の段を同じ dispatch にまとめ、中間画像の外部メモリへの書き戻しを減らせる。
3. group 内で周辺画素や水平結果を再利用でき、halo の重複読み込みと shared memory の費用を上回る。
4. histogram、min/max reduction、複数 mip の生成など、画素ごとの描画以外の仕事がある。

[Godot の compute shader](https://github.com/godotengine/godot/blob/master/servers/rendering/renderer_rd/shaders/effects/copy.glsl)
は group shared memory を使う実例。
[FidelityFX Blur](https://gpuopen.com/manuals/fidelityfx_sdk/techniques/blur/) は水平結果を shared memory の
ring buffer に保持し、垂直処理へ再利用する。単純な小タイルより halo の割合を減らす発想を参考にできる。
ただし SDK の shader model・D3D12/Vulkan 基盤を v0.8 の D3D11/Metal へ直接導入する案ではない。

再検討時は SM5/MSL で実装可能な group shared memory と barrier から試し、wave 専用命令を前提にしない。
範囲外 thread も group の barrier までは参加させる。
[D3D11 CS 5.0](https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/registers---cs-5-0)
の shared memory・UAV 数と形式ごとの機能を確認する。
v0.8 の [D3D11 device 選択](../../../Siv3D/src/Siv3D-Platform/WindowsDesktop/Siv3D/Renderer/D3D11/Device/D3D11Misc.cpp)
ではハードウェアに FL11_0 を要求する一方、software driver は FL10_1 も候補になるため fallback が必要。
read-only SRV + write-only UAV で足りる処理に、不要な typed UAV load を要求しない。

全 `RenderTexture` に UAV/ShaderWrite を付けることは避ける。
Metal は [usage によって最適化条件が変わり](https://developer.apple.com/documentation/metal/optimizing-texture-data)、
GPU によっては書き込み可能な texture が圧縮最適化の制約を受ける。
一時 UAV から通常の destination へコピーする方式も、コピーを含めて比較する必要がある。
Compute の導入と、公開 `ComputeShader` / storage buffer API の設計は別の作業とする。
依存する `draw → compute → draw` を並べただけで async compute の利益が得られるとは限らない。

### 強いぼかしには別の選択肢を持つ

大きい sigma を原寸の多数 tap で計算し続けるより、縮小・ぼかし・拡大が有利になりやすい。
ただし縮小・拡大自身の filter も結果へ寄与するため、原寸の Gaussian と同一にはならない。
`GaussianBlur` を黙ってこの方式へ切り替えるより、近似を許す `DualKawaseBlur`、`Bloom`、影などで利用する。

- **Dual Kawase / down-up pyramid**: 初期セットの DualKawaseBlur に使う。背景ぼかしや Bloom のような広いぼかしに使う。
  Gaussian と異なる形、移動する輝点、強さをアニメーションしたときの段数切り替えを評価する。
- **複数回の box / sliding-window blur**: 大半径の候補。RGBA/PMA と境界処理、走査の並列性、精度を検討する。
- **IIR・積分画像・FFT**: 非常に大きい kernel や特殊な反復用途では候補だが、prefix/scan、境界、精度、
  準備費用が増える。初期の汎用 2D helper に採用する根拠は不足している。

参考は [Intel の blur 比較](https://www.intel.com/content/www/us/en/developer/articles/technical/an-investigation-of-fast-real-time-gpu-based-image-blur-algorithms.html)
と [Arm の Dual Filtering 解説](https://community.arm.com/cfs-file/__key/communityserver-blogs-components-weblogfiles/00-00-00-20-66/siggraph2015_2D00_mmg_2D00_marius_2D00_notes.pdf)。
それらの過去の機種・速度比を現在のバックエンドへ外挿しない。

### 縮小と mip pyramid

大幅な縮小を LOD 0 の bilinear sample だけで行うと、寄与しない入力画素が増える。
2 倍縮小には安価な box/tent、任意比率には footprint を覆う area 系や段階縮小を比較する。
奇数サイズの最後の行・列、片方だけ縮める画像、1 texel の軸も通常ケースとして扱う。
高品質縮小と Gaussian 用の前処理では求める kernel が異なるため、一つの固定法で済むとは限らない。

[FidelityFX SPD](https://gpuopen.com/manuals/fidelityfx_sdk/techniques/single-pass-downsampler/) は
複数 mip を 1 dispatch で生成する有力な研究対象だが、最後の group の調停や UAV 管理を伴う。
文書化された奇数サイズの端の扱いも、任意の画像を余さず集約する縮小とは異なる。
全 mip を必要としない単発縮小の初期実装へ直結させず、Bloom pyramid や reduction が必要になった段階で比較する。

## Metal の予備計測

再現用の [standalone 診断ツール](../../../tools/texture-processing-metal-bench.mm) は、エンジンを変更せず
native copy、全画面 triangle、2 パス Compute、小タイルの融合 Compute、縦長領域の融合 Compute、
MPS を比較する。有限範囲の正規化 Gaussian 係数を生成し、同じ source に適用する。

Apple M2、1920×1080、RGBA16Float、単一サンプル、clamp の結果。
表は各条件で独立に 2 回実行した **GPU 時間中央値の範囲**で、単位は ms。
各実行は warm-up 後に 13 標本、1 command buffer に同じ処理を 8 回記録する。
再帰的にぼかす測定ではなく、同じ常駐入力と出力を再利用する測定。

| 経路 | R=4、sigma=1.6 | R=12、sigma=4 | 注意 |
| --- | ---: | ---: | --- |
| native copy | 0.285–0.287 | 0.280–0.281 | kernel 条件に依存しない対照 |
| raster copy | 0.288–0.290 | 0.288–0.289 | RGBA16F の integer read、全面描画 |
| compute copy | 0.323–0.325 | 0.318–0.320 | 書き込み可能 destination |
| Gaussian、2 パス raster | 0.556–0.556 | 0.887–0.889 | bilinear で tap を集約、通常の RT |
| Gaussian、2 パス compute | 0.704–0.728 | 0.959–0.960 | 同じ bilinear kernel |
| Gaussian、8×8 出力の融合 compute | 1.232–1.253 | 12.013–12.352 | source の halo 全体と水平結果を共有 |
| Gaussian、8×32 出力の融合 compute | 0.588–0.590 | 1.359–1.373 | 水平結果を共有。ring buffer の完成形ではない |
| 8×32 compute + 通常 RT への copy | 0.975–0.993 | 1.724–1.991 | destination の制約回避にも費用がある |
| MPS Gaussian | 0.718–0.729 | 0.950–1.131 | 異なる近似、pixel-equivalent 比較ではない |
| 2 パス raster、ShaderWrite 付き RT | 0.709–0.710 | 0.952–0.953 | usage の影響を比較する対照 |
| 2 パス raster、Load 指定 | 0.556–0.559 | 0.888–0.890 | 他の raster 行は DontCare |

256×256、R=4 の反復では、2 パス raster の中央値が 0.076–0.099 ms、
2 パス compute が 0.017–0.053 ms だった。小さい処理では変動幅も大きく、
この値だけから自動切り替えの面積閾値は決めない。
3840×2160、R=12 の単発測定でも raster は約 3.75 ms、2 パス compute は約 4.15 ms だった。

この比較から採用できる判断は次の範囲に限る。

- Compute は現実的だが、dispatch 数の削減だけでは速くならない。小タイルの大半径処理は大幅に遅くなった。
  halo の重複・演算量・shared memory 使用量は説明候補であり、occupancy の原因分析には GPU counter が必要。
- 1080p、R=4 の CPU encode+commit は 2 パス raster が約 13 µs、縦長 compute が約 4 µs だった。
  CPU 提出費用と GPU 時間は別々に評価する。
- 同じ raster kernel でも ShaderWrite 付きの画像では遅くなった。
  圧縮の有無を測った結果ではないが、全 RT の usage 拡張を無償とは扱えない。
- Compute の結果を通常の RT へコピーすると、候補の利益が消えることがある。
  MPS も、同じ Gaussian という名前だけで高速・等価な置き換えにはならない。
- この条件で DontCare と Load の差は小さかった。
  [不要な load を避ける設計](https://developer.apple.com/documentation/metal/setting-load-and-store-actions)の妥当性と、
  特定条件で得られる改善率を分ける。

### 計測の再現と解釈

macOS、Metal/MPS、C++20、`MTLMathModeSafe` を提供する SDK と実行環境（macOS 15 以降）が必要。
Xcode/Metal の操作は [ホストの開発手順](../../development/README.md#build-and-test)に従って sandbox 外で行う。
出力先は `/tmp` など任意の一時ディレクトリとし、`Test/data/` へ書き込まない。

```sh
xcrun clang++ -std=c++20 -O3 -DNDEBUG -fobjc-arc -Wall -Wextra -Werror \
  -framework Foundation -framework Metal -framework MetalPerformanceShaders \
  tools/texture-processing-metal-bench.mm -o /tmp/siv8-texture-processing-metal-bench

# 引数: width height radius sigma iterations samples
/tmp/siv8-texture-processing-metal-bench 1920 1080 4 1.6 8 13 > /tmp/siv8-texture-processing-r4.csv
/tmp/siv8-texture-processing-metal-bench 1920 1080 12 4 8 13 > /tmp/siv8-texture-processing-r12.csv
/tmp/siv8-texture-processing-metal-bench 256 256 4 1.6 8 13 > /tmp/siv8-texture-processing-small.csv
```

標準出力には device・設定、CPU reference との誤差、GPU median/p10/p90 と CPU encode+commit が出る。
初回の shader compile、pipeline/texture 作成、upload、readback は時間測定の外。
係数を定数化した診断用 shader であり、sigma を変え続ける製品 API の費用は測っていない。
GPU 時間は command buffer 全体を反復回数で割る。CPU 時間は完了待ちを含まない。
標本間では診断のため完了を待つが、製品の画像処理 API にその待機を導入する案ではない。
エンジンの通常描画との混在、frame latency、実際の画像効果の全工程は別途測る必要がある。

自作 kernel は 67×53、1×9、11×1 の PMA 入力を CPU の離散 Gaussian と比較する。
R=4 と R=12 で成功し、copy の値の差は 0、ぼかしは最大約 0.00147 だった。
2 パス reference は中間を half に丸め、融合版は最終出力だけ half に丸める。
0.002 の閾値は診断用であり、製品の許容誤差として採用した値ではない。
MPS の差は表示だけで合否に使わない。同じ sigma でも近似と kernel の打ち切りが違う。
RGBA8/sRGB/HDR 値、別 GPU、エンジンの状態復帰や目視品質まで検証したものではない。

次の条件でも完了し、Metal API Validation のエラーは出なかった。
validation 有効時の時間は性能表へ含めない。

```sh
MTL_DEBUG_LAYER=1 /tmp/siv8-texture-processing-metal-bench 64 48 4 1.6 1 3
```

## 効果の提供形

**参考: 描画用シェーダと RT 操作の責務を分けるための検討。現在具体化する API は RT 操作に限定する。**
効果を一度表示する用途で、中間 RT の作成・描き込みを必須にしない。
全効果を両方の形で公開する前提にもせず、まず自然な使い方を用意する。

| 提供形 | 向いている用途 | 描画状態と費用 |
| --- | --- | --- |
| エンジン提供 `PixelShader` + `ScopedCustomShader2D` | 個々の sprite・アイコンの色調整、対応範囲内の UV 効果 | 通常の draw に適用。target・変換・blend 等を維持し、隠れた RT や追加パスを作らない |
| `source → destination RT` の関数 | 処理結果の保持・共有、合成済み画像の処理、resize、blur、Bloom | 明示した RT の実画素数で処理。描画状態を隔離して出力し、中間画像を再利用する |

例えばグレースケール表示だけなら、次の形を第一候補にする。`TextureShader` は対象を明確にするための仮名。
取得関数はエンジンが保持するシェーダを返し、呼び出すたびに compile・pipeline 作成をしない設計とする。

```cpp
// 仮の API。シェーダをフレームループの外で取得する。
const PixelShader psGrayscale = TextureShader::Grayscale();

// フレーム内。通常の Texture / TextureRegion の描画へ適用する。
{
    const ScopedCustomShader2D shader{ psGrayscale };
    icon.draw(100, 100);
    character.draw(300, 100);
}
```

通常の描画なので Scene にも明示した RT にも描ける。ToScene 関数の追加は不要。
低解像度でぼかした RT の最後の表示に色調整を適用すれば、色調整専用 RT とその書き込みを省ける。
ただし Scene の高精細化によって shader が処理する fragment は増える。重なりによる overdraw、
シェーダ・定数の切り替えによる batch 分割も費用になる。固定の結果を何度も使うなら、RT へ一度処理する方が
有利な場合もあり、直接描画が常に高速とはしない。

### 候補ごとの提供形

| 候補 | 個別描画用シェーダ | RT 操作 | 初期の重点 |
| --- | --- | --- | --- |
| Grayscale / Saturation / ColorMatrix | 適する。色だけを変える小さな入口 | 結果の保持・後段処理に有用 | 描画用を優先。効果自体は A |
| Sepia / Invert / HueShift / Posterize / Threshold | 適する。行列で表せるものと非線形処理を区別 | mask 生成や階調処理などに有用 | 共通実装から追加しやすいものを選ぶ |
| ApplyMask / Dissolve / GradientMap / ColorLookup | 適する。mask や LUT を読む方式には補助 texture・座標・sampler が必要 | 複数入力を明示でき、生成結果も再利用できる | 用途と入力設定を先に決める |
| RGBShift / ChromaticAberration / Pixelate / Displacement | 有用。領域の座標と境界を指定できることが条件 | 画像全体を対象にしやすい | atlas と単位の仕様を固めて選ぶ |
| Copy / Resample / Downsample | 通常描画の拡縮で表示できても、画素転送・高品質縮小とは別 | 中心となる提供形 | S の RT 操作 |
| GaussianBlur / DualKawaseBlur / Bloom | 1 軸・小 kernel 等の限定用途は可能 | 複数パスや低解像度処理を管理できる | RT 操作を優先 |
| Outline / Shadow / Glow | 素材の余白・形状拡張・方式によって可能 | 余白を含む画像を生成できる | 大きな半径と余白は RT 側から設計 |

### 既存の scope へ渡すための条件

[ScopedCustomShader2D](../../../Siv3D/include/Siv3D/ScopedCustomShader2D.hpp) が保存・復元するのは
指定した shader stage であり、定数や追加 texture を含む material ではない。
強さを変える Saturation 等では、シェーダに加えてエンジン提供のパラメータ型を用意し、
既存の [ScopedPSConstantBuffer2D](../../../Siv3D/include/Siv3D/ScopedConstantBuffer2D.hpp) で設定する形が土台になる。
定数 scope は構築時の値を保存するため、キャラクターごとに異なる強さを記録する用途にも使える。
最初は専用パラメータ型・slot の規約・短いサンプルをセットで提示し、その呼び出しが煩雑なら、
シェーダと定数をまとめて設定・復元する薄い便利 scope を比較する。
PixelShader 自体に強さや可変の共有状態を持たせたり、強さごとにシェーダを生成したりしない。

通常の Texture 描画は source を slot 0 に設定する。
一方、現在の公開 [Graphics2D](../../../Siv3D/include/Siv3D/Graphics2D.hpp) には任意の補助 texture を設定する API がない。
Mask / Dissolve / LUT / Displacement の提供には、補助 texture・sampler・寿命・復元を扱う入口も必要。
単にシェーダを配れば使えるものとして初期範囲へ数えない。

適用対象はまず標準のテクスチャ描画に限定する。
[描画の shader 選択](../../../Siv3D/src/Siv3D/Renderer2D/Renderer2DDrawSubmission.hpp)では、custom PS があると
図形・模様・破線・テクスチャそれぞれの標準 PS を選ばなくなる。
テクスチャ用 PS を指定しても、図形の塗りや破線の coverage が自動的に維持されるわけではない。
[Font 描画](../../../Siv3D/src/Siv3D/Font/CFont.cpp)は通常、内部で文字用 PS を設定し、明示した custom shader 用の
TextStyle の場合に外側の PS を使う。MSDF の距離復元・縁取りなどを失わずに色を変えるには専用の対応が必要。
「スコープ内のすべての 2D 描画へ同じ効果」を目指すなら、標準描画処理の後へ効果を組み込む別の仕組みとして評価し、
初期の PixelShader 提供とは分ける。通常描画すべてに effect 判定やデータを追加する前提にしない。

Godot の [CanvasItem shader](https://docs.godotengine.org/en/stable/tutorials/shaders/shader_reference/canvas_item_shader.html)
は、テクスチャ・頂点色と fragment の関係、texture の画素サイズ、sprite region を shader 側のインターフェースに含める。
Siv3D でも、個別描画用の効果には色処理だけでなく、入力と領域の契約が必要という参考になる。
同じ汎用の shader language や全描画への統合まで導入する提案ではない。

### 結果と組み合わせの契約

- **描画ごとの効果と、合成結果への効果を区別する。** scope 内の draw はそれぞれ加工されてから blend される。
  複数の画像を重ねた後に threshold・blur 等を一度適用する結果とは一般に異なる。
- **スコープの入れ子は効果の積み重ねではない。** 内側の PS が外側の PS を置き換える。
  組み合わせは合成した ColorMatrix、用意した複合シェーダ、または明示した RT パスで表す。
- **色と alpha の順序をそろえる。** 標準の乗算色・加算色と PMA を維持する。
  source の色加工後に draw の着色を行う案と、着色を含む描画色へ効果を掛ける案を比較する。
  前者では grayscale 素材を赤く着色でき、後者では着色も脱色されるため、単なる内部実装差にはしない。
- **UV 効果には領域と単位が必要。** TextureRegion の UV は親 texture 内の座標であり、各 sprite の 0〜1 ではない。
  region の境界・source texel サイズ・効果の中心を渡す方法を決め、隣の atlas 画像を読まないようにする。
  source texel、描画座標、実画素のどれを使うかを明示し、拡縮・反転時の結果も定義する。
- **PS だけでは描画形状の外側へ描けない。** 外側の Outline / Shadow / Glow は余白や geometry 拡張が必要で、
  texture の境界処理だけでは解決しない。標準で広い余白や重い近傍 sample を全 draw に加えない。

HLSL/MSL では色変換などの共通関数を再利用し、通常描画と RT 用の入出力・状態設定をそれぞれ用意する。
同じ PixelShader バイナリの共有や、両経路の画素単位の一致を無条件には約束しない。
非線形の色加工と補間の順序、中間画像の形式・丸めによっても結果は変わる。

## 操作の候補と重要度

**グレースケールや色収差のような、小さく名前を付けやすい効果も標準提供の候補に含める。**
頻出の演出を短く書け、HLSL/MSL と alpha の扱いを利用者が毎回実装しなくて済む価値がある。
公開される効果名の数と、内部 kernel や render pass の数は分けて設計する。

以下は効果・操作そのものの候補であり、すべてを RT 関数にする一覧ではない。
提供形は[前節](#効果の提供形)に従って選ぶ。色調整や個別描画に向く効果は今回の RT API 設計から外す。
2D ゲーム、UI、インタラクティブ作品を主用途とした重要度の私案であり、全件の採用予定ではない。
**S は基盤、A は標準提供を優先、B は用途に応じて追加、C は当面保留またはサンプル向け**。
利用頻度、既存の描画で代替する手間、複数用途への応用、パラメータと費用の分かりやすさを重視する。
重要度と実装順序は同じではなく、A の Bloom は A の Grayscale より準備と検証が多い。

候補の漏れは [Direct2D の組み込み効果](https://learn.microsoft.com/en-us/windows/win32/direct2d/built-in-effects)と
[Unity URP の効果一覧](https://docs.unity.com/en-us/engine/6000.0/manual/post-processing-and-full-screen-effects/urp/effect-list)
も参照して確認した。両者の機能一覧をそのまま採用する判断ではない。

費用欄は同じ出力サイズでの典型的な raster 実装の構造を示し、時間の実測値ではない。
**1 参照**は出力画素あたりおおむね 1 回の texture sample、**少数参照**は複数画像や近傍の数回の sample、
**近傍参照**は kernel 幅に依存する sample、**複数パス**は中間画像も使う処理を表す。
1 sample の線形補間自体にも複数 texel の参照があり、同じ 1 パスでも算術・形式・帯域によって費用は異なる。
RT 操作は明示した RT の解像度で評価する。個別描画では通常 draw の中に処理を組み込み、
その draw の実際の fragment 数に応じて費用が変わる。Compute と ToScene 系は初期対象外のまま維持する。

### 基盤と画像の準備

| 重要度 | 操作候補 | 代表用途・API としての価値 | 費用の目安 |
| --- | --- | --- | --- |
| S | `Copy` | 同サイズ・同形式の画素転送、結果の保持 | native 転送 |
| S | `Resample` | thumbnail、低解像度処理、指定サイズへの再サンプリング | 通常は 1 パス・1 参照 |
| S | `Downsample` | 強い縮小の aliasing 抑制、blur/Bloom の準備 | 複数参照、縮小率・品質によって複数パス |
| S | `GaussianBlur` | 背景ぼかし、影、発光の基礎 | 通常 2 パス・近傍参照 |
| A | custom PS の `Blit` | 独自効果を同じ入力・明示した RT・状態隔離で扱う拡張口 | PS に依存 |
| B | `GaussianBlurH/V` / 1 軸 filter | streak、独自 separable filter の組み立て | 1 パス・近傍参照 |
| A | 近似の `DualKawaseBlur` | 初期セットに含める案。大きな半径の背景・UI ぼかしを一呼び出しで扱う | Dual Kawase による縮小・拡大の複数パス |
| B | `ExtractChannel` / `ExtractAlpha` | alpha/luma mask、チャンネルの確認と再利用 | 1 パス・1 参照 |
| B | `PackChannels` / `Swizzle` | 複数 mask の集約、数値 texture の準備 | 1 パス・入力数に依存 |
| B | `PremultiplyAlpha` / `UnpremultiplyAlpha` | 外部データ・自作 shader との明示的な変換 | 1 パス・1 参照 |

### 色の調整

| 重要度 | 操作候補 | 代表用途・API としての価値 | 費用の目安 |
| --- | --- | --- | --- |
| A | `Grayscale` | 無効 UI、回想、停止・死亡演出。部分的な脱色も有用 | 1 パス・1 参照 |
| A | `Saturation` | 雰囲気の調整、状態変化。Grayscale と実装を共用できる | 1 パス・1 参照 |
| A | `Brightness` / `Contrast` / `Exposure` | 明暗・メリハリの調整。単純な affine 変換はまとめやすい | 1 パス・1 参照 |
| A | `ColorMatrix` | 複数の affine 色調整をまとめる基盤、channel mixer | 1 パス・1 参照 |
| B | `Sepia` / `Invert` | 回想、ネガ反転、被弾演出。色行列の preset にしやすい | 1 パス・1 参照 |
| B | `HueShift` | 色違い、属性表現、周期的な色変化 | 1 パス・1 参照、色空間の変換方法に依存 |
| B | `Gamma` / `Levels` / `Curves` | 細かい階調調整、明暗域の調整 | 1 パス・算術または LUT 参照 |
| B | `Duotone` / `GradientMap` | 輝度を 2 色または palette に対応、作品の配色統一 | 1 パス・1〜少数参照 |
| B | `ColorLookup` | 既製の色調整を LUT で適用 | 1 パス・LUT 参照を追加 |
| B | `ReplaceColor` | 特定色の差し替え、色分けされた素材の再利用 | 1 パス・1 参照、許容幅と境界を定義 |
| B | `ToneMap` | HDR の結果を表示用範囲へ写す | 1 パス・1 参照、曲線に依存 |
| C | 色温度、white balance、色管理全般 | 写真・動画の編集用途が中心。必要性を確認して追加 | 色空間と精度の仕様が大きい |

固定倍率や単純な RGB 加算を一度表示するだけなら、既存の
[色 scope](../color-scopes.md) と `Texture::draw()` で表せる。
Brightness 等の独立した関数を設ける価値は、処理済み RT を後段へ渡す用途や、他の色調整との一括適用にもある。
倍率、加算値、EV のどれを指定するかは名前と契約で区別する。

### マスクと画像の合成

| 重要度 | 操作候補 | 代表用途・API としての価値 | 費用の目安 |
| --- | --- | --- | --- |
| A | `ApplyMask` | 任意形状の UI、画像の抜き、効果範囲の制限 | 1 パス・画像と mask を参照 |
| A | `Threshold` / `LuminanceToAlpha` | 二値画像、明暗による mask、効果の抽出 | 1 パス・1 参照 |
| A | `CrossFade` | 2 画像の連続的な切り替え | 1 パス・2 画像参照 |
| A | `Composite` | Multiply / Screen 等の合成、画像同士の重ね合わせ | 1 パス・2 画像参照、blend mode に依存 |
| A | `Dissolve` / mask transition | 消滅、出現、場面切り替え。境界の発光も候補 | 1 パス・2〜3 入力参照 |
| B | `Dilate` / `Erode` | mask の拡縮、隙間埋め、Outline の基礎 | 近傍参照、半径によって複数パス |
| B | `Accumulate` / `Afterimage` | 残像、軌跡、前フレームとの混合 | 1 パス・現在と履歴の 2 画像参照 |
| C | `ChromaKey` | カメラ・動画の背景抜き | 1 パスから。色かぶり除去などの品質要件を別途定義 |

CrossFade は両画像の PMA 値を重み付きで混合する操作として設計できる。
通常の source-over 描画を 2 回行うことと、任意の alpha を持つ画像のクロスフェードは同一ではない。
Composite は入力 2 枚を明示する形と、destination へ直接 blend する形を混同しない。
Afterimage は履歴 RT と時間・減衰を呼び出し側が指定する案とし、関数内でフレーム履歴を暗黙に保持しない。

### 輪郭とぼかしを使う効果

| 重要度 | 操作候補 | 代表用途・API としての価値 | 費用の目安 |
| --- | --- | --- | --- |
| A | `Outline` | 選択中の物体、アイコン、sprite の縁取り | alpha の近傍参照。幅・方式により複数パス |
| A | `Shadow` / `Glow` | 任意画像の影・外側の発光 | mask・blur・色付け・合成。通常は複数パス |
| A | `Bloom` | HDR の輝点、発光表現 | 抽出・縮小・拡大合成の複数パス |
| B | `Sharpen` | 縮小後の輪郭強調、画像の鮮明化 | 小 kernel または blur との差分 |
| B | `EdgeDetect` | 輪郭抽出、線画風、画像解析の入口 | 1 パス・近傍参照 |
| B | `DirectionalBlur` / `ZoomBlur` | 速度感、衝撃、ズーム演出 | 通常 1 パス・指定 sample 数に依存 |
| B | 小さい `Convolution` | 独自の sharpen、edge、emboss kernel | 近傍参照。初期は小さい固定上限を検討 |
| C | `Emboss` / 鉛筆画 / 油絵風 | 作風に特化した効果 | kernel preset またはサンプルを優先 |

Outline は画像の alpha 輪郭、EdgeDetect は画像内の明暗・色の変化を使うものとして区別する。
外側の影や輪郭には余白が必要で、出力サイズと位置合わせも公開契約に含める。
大きい半径での費用と品質は小 kernel の延長として無制限に約束しない。
Bloom を A として提供する場合、閾値・HDR・明示した pyramid の解像度も一緒に設計し、
表示環境に応じて暗黙に作業画像を拡大しない。固定の tone map が必要な用途では ToneMap の重要度も上がる。

### 色ずれと座標の変形

| 重要度 | 操作候補 | 代表用途・API としての価値 | 費用の目安 |
| --- | --- | --- | --- |
| A | `RGBShift` | RGB を一定量ずらす、被弾・グリッチ・速度感 | 1 パス・典型的には 3 sample 前後 |
| A | `ChromaticAberration` | 中心から周辺へ色ずれを強めるレンズ風表現 | 簡易版は 1 パス・3 sample 前後 |
| A | `Displacement` | 歪み画像で屈折、熱気、水面、揺らぎ | 1 パス・歪み画像と source を参照 |
| B | `Wave` / `Ripple` / `Swirl` | 波、波紋、渦。Displacement の入力作成や専用式の候補 | 1 パス・座標計算＋source 参照 |
| B | `LensDistortion` | 樽型・糸巻き型の歪み、魚眼風 | 1 パス・座標計算＋source 参照 |
| C | レンズフレア、物理的なレンズモデル | 作風や renderer への依存が大きい | 複数パスや追加情報が必要になる場合がある |

RGBShift と ChromaticAberration は演出上の違いを保つ。
前者は各 channel の一定の offset、後者は中心と距離に応じた offset を扱う簡易モデルを候補とする。
色収差を標準効果として扱う例は [Unity URP の Chromatic Aberration](https://docs.unity.cn/Packages/com.unity.render-pipelines.universal@16.0/manual/post-processing-chromatic-aberration.html)
にもあるが、その intensity の単位や不透明な画面への前提を、そのまま Siv3D の RGBA 処理へ持ち込まない。
分光的に多数回 sample する高品質版は初期案に含めない。

位置ずれの強さは source の texel 単位、中心は正規化座標など、操作に合った単位を明示する。
透明な sprite に適用したとき、別々の位置から取得した RGB と alpha をどうまとめるかが必要になる。
単に中心の alpha を流用すると PMA が崩れる場合があるため、RGB と alpha の対応と、
輪郭を移動させるか保存するかを仕様化する。
offset=0 / intensity=0 は入力相当の結果とし、変形先の境界では clamp・透明外周などの共通規則を使う。

### 画風と画面演出

| 重要度 | 操作候補 | 代表用途・API としての価値 | 費用の目安 |
| --- | --- | --- | --- |
| A | `Pixelate` | モザイク、低解像度演出、段階的な場面切り替え | 代表点 sample なら 1 パス・1 参照 |
| B | `Posterize` | 階調数を減らす、アニメ・印刷風の色 | 1 パス・1 参照 |
| B | `OrderedDither` | 限られた階調や palette の表現、レトロ風 | 1 パス・パターン参照または算術 |
| B | `Vignette` | 周辺減光、視線誘導、ダメージ表現 | 1 パス・1 参照＋位置依存の演算 |
| B | `FilmGrain` / `Noise` | 質感、古い映像、揺らぐ画面 | 1 パス・noise の生成または追加参照 |
| B | `Scanlines` | 走査線、モニター越しの表現 | 1 パス・1 参照＋位置依存の演算 |
| C | `CRT` / `VHS` / `Glitch` の一括効果 | 複数効果を組み合わせた作風 | まず preset・サンプルで提示し、汎用の基本操作を優先 |

Pixelate の代表点 sample と、各 block の平均色を求めるモザイクは費用も結果も異なる。
OrderedDither と、画素間の依存を持つ誤差拡散も別のアルゴリズムとして扱う。
noise・残像・transition の時間や seed は引数で指定できるようにし、同じ入力からの再現と停止を可能にする。

### 細かい効果を公開する粒度

以下は個別描画用シェーダを設計する際の参考であり、今回の RT 操作の初期範囲には含めない。

Grayscale は名前で使える入口として A に置き、まず個別描画用シェーダを優先する。利用者へ色行列の係数を要求せず、
内部では Saturation や ColorMatrix と共通の実装を使える。
既存の [ColorF::grayscale()](../../../Siv3D/include/Siv3D/ColorF.hpp) は 0.299/0.587/0.114 の重みを使うため、
GPU 側もその見た目へ合わせるか、linear-light の輝度を別に定義するかは採用前に決める。
alpha は保持する案を基本とする。

結果の保持・後段処理の需要があれば、RT へ出力する便利関数も改めて検討する。

Grayscale、Saturation、単純な brightness/contrast、Sepia、Invert などの affine 色変換は、
準備段階で行列を合成し、一つの ColorMatrix パスへまとめる候補になる。
複数の便利関数を順に呼んだ場合まで自動的に 1 パスになるとは約束しない。
gamma、tone map、一般の HSV hue shift、threshold は同じ行列だけでは表せない。
複数のよく使う色調整を固定順序で適用する `ColorAdjust` も候補だが、汎用の自動 effect graph は初期要件にしない。

行列を合成しても、中間 RT の丸め・clamp を通る複数パスと必ず同じ結果になるわけではない。
この点は [Direct2D の shader linking と精度](https://learn.microsoft.com/en-us/windows/win32/direct2d/precision-and-clipping-in-effect-graphs)
でも扱われている。Siv3D では一括適用と個別適用の結果の契約を明示し、PMA の bias と alpha の関係もそろえる。

平行移動、回転、反転、単純な tint、opacity、矩形 crop は、既存の TextureRegion・描画・scope で短く書ける。
それぞれに専用の RT 書き込み関数を増やす優先度は低めとし、結果の保存に必要なときは汎用の描き込みを検討する。
Vignette なども最終表示への重ね描きで足りる場合がある一方、処理済み画像をさらに加工する用途には RT 出力の価値がある。

### 小さく始める場合の候補

初期範囲は[5 操作の案](#初期実装に含める-5-操作の案)を基準とする。
S の Copy / Resample / Downsample / GaussianBlur と、A に引き上げた DualKawaseBlur を中心にし、
GaussianBlurH/V、custom Blit、Outline / Shadow / Bloom、mask 生成・合成は同節の追加候補として扱う。
Grayscale 等の描画用シェーダと、その補助 texture API は初期実装の完了条件に含めない。

この順序は採用案を絞るためのたたき台。表の A をすべて初期リリースへ入れる前提にはしない。
depth-aware blur、被写界深度、速度バッファを使う motion blur、TAA、SSAO、FFT、大規模 histogram や自動露出は、
深度・履歴・GPU buffer などの契約が増えるため C とし、今回の画像処理 API から切り離して検討する。
Bloom に必要な単純な明るさの抽出と、自動露出用の画像全体の集計も区別する。

