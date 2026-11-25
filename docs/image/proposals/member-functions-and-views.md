# Image メンバ関数と矩形領域の処理基盤

**状態: view と矩形 copy / fill の基盤は実装済み。残る設計は未採用。
対象は v0.8 の段階的な拡張候補であり、全項目の採用・実装を約束するものではない。**
現行の実装は [Image ガイド](../README.md)、未完了項目は [TODO](../../../TODO.md) を参照。
採用した API の契約は公開ヘッダへ移し、実装時に以下の対応表も更新する。

## 比較範囲

v0.6 は安定版 v0.6.16 の
[Image.hpp](https://github.com/Siv3D/OpenSiv3D/blob/v0.6.16/Siv3D/include/Siv3D/Image.hpp)、
[ImageROI.hpp](https://github.com/Siv3D/OpenSiv3D/blob/v0.6.16/Siv3D/include/Siv3D/ImageROI.hpp)、
[Image 実装](https://github.com/Siv3D/OpenSiv3D/blob/v0.6.16/Siv3D/src/Siv3D/Image/SivImage.cpp)、
[ROI 実装](https://github.com/Siv3D/OpenSiv3D/blob/v0.6.16/Siv3D/src/Siv3D/ImageROI/SivImageROI.cpp)
を基準にする。v0.8 はこのリポジトリの宣言・定義を照合した。
オーバーロードは機能単位でまとめ、コンストラクタ・演算子・static メンバも補足する。
同名メンバの有無と、他の API による機能の有無を区別する。

## 1. v0.8 で実装済み

| 分類 | メンバ・関連演算子 | 現状 |
| --- | --- | --- |
| 作成 | `Image` の各コンストラクタ | 寸法・色、generator / generator0_1、ファイル / Reader、RGB と alpha の別画像、Emoji / Icon、Grid と変換関数 |
| 代入・比較 | コピー / ムーブ代入、`operator ==` | `!=` は C++ の書き換え規則で利用可能。比較は hidden friend |
| 寸法・状態 | `width`, `height`, `size`, `bytesPerRow`, `size_bytes`, `pixelCount`, `isEmpty`, `operator bool`, `horizontalAspectRatio` | 所有画像は行間に padding のない RGBA8 |
| メモリ・コピー | `shrink_to_fit`, `clear`, `release`, `swap`, `cloned` | 左辺値の clone と右辺値の所有権移動を区別 |
| 画素アクセス | `operator []`, `indexInBounds`, `get_if` | `Point`、行、および `[y, x]`。借用は所有する左辺値に限定 |
| 行・列 | `row`, `column` | `column` は `__cpp_lib_ranges_stride` がある環境のみ |
| 生データ・走査 | `data`, `dataAsUint8`, `begin/end`, `cbegin/cend`, `rbegin/rend`, `crbegin/crend` | `data()` の先頭は 32-byte aligned。各行の先頭まで aligned とは限らない |
| 塗りつぶし・格納寸法 | `fill`, `resize`, `resizeHeight` | `resize` は画像内容を補間して拡縮する操作ではない |
| alpha | `premultiplyAlpha`, `unpremultiplyAlpha` | x86 に SSE4.1 / AVX2 の明示実装。ARM は plain 経路で、明示 NEON 実装はない |
| チャンネル順 | `bgraToRGBA` | R/B の交換。SSSE3 / AVX2 / NEON と plain の実装あり |
| 出力 | `save`, `encodePNG` | 汎用保存と PNG の Blob 出力 |
| 画像コピー | `overwrite(Image&, Point)`, `overwrite(MutableImageView, Point)` | 所有画像同士の自己コピー no-op を維持。view を宛先とするコピーは overlap にも対応 |
| 矩形ビュー | `operator ()`（`Rect`、整数座標・寸法、`Point` / `Size`） | `ImageView` / `MutableImageView`。`subview`, `row`, 画素アクセス、`cloned`, `copyTo`, `overwrite`、mutable の `fill` |

旧名からの移行を「未実装」と数えない:

| v0.6 | v0.8 の対応 | 注意 |
| --- | --- | --- |
| `stride()` | `bytesPerRow()` | バイト単位 |
| `num_pixels()` | `pixelCount()` | 戻り値は `size_t` |
| `inBounds()` | `indexInBounds()` | 境界確認 |
| `resizeRows(rows, color)` | `resizeHeight(height, color)` | 高さ変更。色なし overload もある |
| `RGBAtoBGRA()` | `bgraToRGBA()` | 同じ R/B 交換だが、名前・戻り値は非互換 |
| `operator ()(rect)` / 旧 ROI 型 | `operator ()(rect)` / `ImageView`, `MutableImageView` | 矩形参照とコピーを実装済み。旧 ROI の filter メンバは今後追加 |

現行メンバではないが利用できる基盤:

- [ImageProcessing::Resize](../../../Siv3D/src/Siv3D/ImageProcessing/SivImageProcessing.cpp) は
  stb_image_resize2 による拡縮を実装済み。現在は PMA RGBA8、Clamp、既定 filter、
  連続した入出力を使う。`scale` の追加を新規 resampler の開発から始める必要はない。
- [ImageEncoder](../../../Siv3D/include/Siv3D/ImageEncoder.hpp) に汎用 Encode / Save があり、
  PNG / JPEG のエンコーダには個別設定もある。
- [ImagePainting](../../../Siv3D/src/Siv3D/Image/ImagePainting.hpp) に
  `Paint` / `Stamp` の幅・高さ・両側 stride を受ける内部処理がある。
  矩形コピーは [ImageRegion](../../../Siv3D/src/Siv3D/Image/ImageRegion.hpp) に共有した。
  `Paint` / `Stamp` は `Image` の公開メンバとしては未接続。
- 図形の `.paint(image, ...)` / `.overwrite(image, ...)` は別の実装済み API。
  画像同士の合成メンバが実装済みであることを意味しない。

## 2. v0.6 にあり、v0.8 のメンバとして未実装

`negate/negated` のような組は破壊的操作と結果画像を返す操作を表す。値を返す側の `const&` / `&&`
などは旧ヘッダを参照する。優先度は A: 初期候補、B: 次段階、C: 用途を確認して採否判断。
OpenCV 欄は v0.6.16 の実装に基づく。I/O の codec はこの分類に含めない。

| 系統 | 旧メンバ | OpenCV | 優先度・再利用方針 |
| --- | --- | --- | --- |
| 切り出し | `clipped`, `squareClipped` | なし | A。矩形コピーと共有。旧 `clipped` は要求サイズを保ち、画像外を透明で埋める |
| 座標サンプル | `getPixel`, `samplePixel` | なし | A。address mode と bilinear の画素座標を共有 |
| 反転色・輝度 | `negate/negated`, `grayscale/grayscaled` | なし | A。画素 kernel。alpha と輝度式を確定 |
| 色調 | `sepia/sepiaed`, `posterize/posterized`, `brighten/brightened`, `gammaCorrect/gammaCorrected` | なし | A。色変換・LUT の共通基盤 |
| 配置 | `mirror/mirrored`, `flip/flipped`, `rotate180/rotated180` | なし | A。ROI 内で寸法を変えず処理可能 |
| 直角回転 | `rotate90/rotated90`, `rotate270/rotated270` | なし | A。`rotate90(n)` も含む。transpose と共有 |
| 二値化 | `threshold/thresholded` | なし | A。grayscale の規則と共有 |
| 自動二値化 | `threshold_Otsu/thresholded_Otsu` | あり | B。histogram + 閾値選択 + 二値化に分解 |
| 局所二値化 | `adaptiveThreshold/adaptiveThresholded` | あり | B。局所平均 / Gaussian と共有 |
| モザイク | `mosaic/mosaiced` | なし | A。block の集計と塗りつぶし。ROI 原点から block を数える |
| ランダム拡散 | `spread/spreaded` | なし | C。乱数源、seed、参照範囲、再現性を明示する価値を検討 |
| 平均ぼかし | `blur/blurred` | あり | A。box filter の分離・移動和を検討 |
| Gaussian | `gaussianBlur/gaussianBlurred` | あり | A。分離 kernel、係数と作業領域の再利用 |
| メディアン | `medianBlur/medianBlurred` | あり | B。小窓と大窓でアルゴリズムを分ける |
| エッジ保持 | `bilateralFilter/bilateralFiltered` | あり | C。費用・品質・必要な最大半径を評価 |
| 膨張・収縮 | `dilate/dilated`, `erode/eroded` | あり | B。min/max filter。旧仕様は既定 3×3 の反復 |
| 塗りつぶし探索 | `floodFill/floodFilled` | あり | B。scanline 探索。旧実装は seed 基準の RGB 許容差、alpha 保持 |
| 拡縮・収める | `scale/scaled`, `fit/fitted` | あり（fit は scale 経由） | A。stb の既存経路を拡張。旧 interpolation の名称だけで画素一致を約束しない |
| 余白 | `border/bordered` | なし | A。出力寸法計算 + fill + 矩形コピー |
| 任意変換 | `warpAffine`, `rotated`, `warpPerspective` | あり（rotated は affine 経由） | B。逆写像・sampling・出力範囲の基盤を共有 |
| 画像合成 | `paint`, `stamp`, `paintAt`, `stampAt` | なし | A。ただし旧合成の意味を再検討してから接続 |
| コピーの便利形 | `overwrite(dst, x, y)`, `overwriteAt` | なし | A。既存 Point 版と矩形 kernel に委譲 |
| alpha 画像 | `applyAlphaFromRChannel` | なし | B。ファイル版だけでなくメモリ内の画像 / view を正規経路にする |
| 輪郭抽出 | `alphaToPolygon`, `alphaToPolygonCentered`, `alphaToPolygons`, `alphaToPolygonsCentered` | あり | B。mask → 輪郭・穴 → Polygon。SIMD より topology が主課題 |
| 輝度画像の輪郭 | `grayscaleToPolygon`, `grayscaleToPolygonCentered`, `grayscaleToPolygons`, `grayscaleToPolygonsCentered` | あり | B。旧実装は R 成分の二値化。RGB からの輝度変換と混同しない |
| 汎用出力 | `encode` | なし | A。既存 `ImageEncoder::Encode` の薄い wrapper |
| 形式別出力 | `savePNG`, `saveJPEG`, `encodeJPEG` | なし | A。既存 codec の wrapper。`encodePNG` は実装済み |
| 他形式出力 | `savePPM`, `encodePPM`, `saveWebP`, `encodeWebP` | なし | C。対応 codec の導入と分けて判断。現行の標準 encoder 登録にはない |
| ダイアログ | `saveWithDialog` | なし | C。Dialog 側の責務と利用頻度から判断 |
| 汎用走査 | `forEach`（mutable / const） | なし | B。range-for で代替可能。追加時は callable と借用規則を既存コンテナに合わせる |
| コンテナ取得 | `asArray` | なし | C。v0.8 は aligned allocator。旧 `Array<Color>` 参照をそのまま戻せない |
| static 作成 | `Generate`, `Generate0_1` | なし | C。generator コンストラクタで用途は満たせる。旧名復活の必要性は低い |

### 旧 ROI にもあった操作

`ImageROI` は `Image&` と `Rect` を公開メンバで持ち、作成時に矩形を画像内へ clip する。
`ImageConstROI` は const な画像を参照する。全体画像と ROI の操作は多くが別ループで、
`sepia` / `posterize` / `gammaCorrect` / `mosaic` の ROI 定義は旧 `SivImage.cpp` にある。

| 範囲 | v0.6.16 の ROI 対応 |
| --- | --- |
| 点ごとの処理 | negate、grayscale、sepia、posterize、brighten、gammaCorrect、threshold |
| 配置 | mirror、flip、rotate180 |
| 近傍・block | mosaic、spread、blur、medianBlur、gaussianBlur、dilate、erode |
| 他画像への書き込み | paint、stamp、overwrite と各 At 版。const ROI からも可能 |
| 対応していないもの | Otsu、adaptiveThreshold、bilateral は宣言がコメントアウト。floodFill、寸法変更、直角回転、輪郭抽出なども ROI メンバにはない |

旧 ROI の機能一覧を新 view の上限にしない。読み取り処理や所有結果を返す処理は
const view でも使えるようにする。逆に固定領域の寸法を変える破壊的処理を無理に追加しない。

## 3. v0.8 で新たに追加する価値がある候補

以下の名前は仮称。対応する値返し版は必要な操作に揃える。
既存の v0.6 機能を別名にしただけのものは前節に含めた。

| 優先度 | メンバ候補 | 実際の用途・共通化先 |
| --- | --- | --- |
| A | `applyLUT` / `appliedLUT` | パレット風変換、トーンカーブ。gamma・posterize・contrast の準備を再利用 |
| A | `adjustContrast`, `adjustSaturation` と結果画像版 | UI / 写真 / 素材の調整。contrast は LUT、saturation は色変換 kernel |
| A | `tint` / `tinted` | スプライトの色違い。RGB 乗算と alpha の扱いを明示 |
| A | `multiplyAlpha` と結果画像版 | フェードや mask の適用。PMA では RGB も同時に調整 |
| A | `replaceColor` / `replacedColor` | ピクセルアートの厳密な色置換。曖昧一致は別オプションとして検討 |
| A | `alphaBounds`, `trimmed` | 透明余白の除去、素材配置。全透明なら bounds はなし、結果は空という案 |
| B | `applyAlpha` / channel 選択付き overload | 読み込み済み mask / ROI の利用。置換と乗算を区別し、ファイル I/O を避ける |
| B | `transpose/transposed` | 画像軸の交換。90/270 度回転とタイル転置 kernel を共有 |
| B | `histogram`, `meanColor` | 自動閾値、素材の代表色、露出解析。alpha を重みとするか無視するかを指定 |
| B | `sharpen/sharpened` | 縮小素材の視認性。Gaussian と unsharp mask を再利用 |
| B | `convolve/convolved` | 小さな独自 filter。係数の向き、正規化、bias、border、alpha を先に定める |
| B | 主要 filter の出力先指定形 | view の `copyTo` は実装済み。加工でも出力 storage を再利用する。公開形式は `ImageProcessing` の関数とも比較 |
| C | hue、color matrix、morphology の opening / closing | 具体的な利用例があるものから。既存 kernel の合成で実装 |

Sobel / Canny、距離変換、connected components、float / HDR 画像、3D LUT などは、
RGBA8 の `Image` メンバとして一律に増やすより、適切な出力型を持つ
`ImageProcessing` や別型の候補にする。単なるメンバ数の増加を目的にしない。

## 全体画像と矩形領域の共通化案

### 採用済みの view と今後の接続

公開 view の使い方、寿命、コピー、実装経路は [Image ガイド](../README.md) を参照する。
`ImageView` と `MutableImageView` は Image の左辺値から取得し、局所座標と親の行 stride を持つ。
`image(rect)` の互換用別名は未採用。旧 ROI の公開フィールドは引き継いでいない。

今後の色調補正・filter は全体 Image と view の両方から同じ kernel に接続する。
`scaled` / `rotated90` は const view から所有画像を返せる形を検討する。
固定領域の寸法を変える破壊的操作は追加せず、`mirror` / `flip` / `rotate180` から始める。
新しい Image の fluent mutator は既存コンテナと同じ左辺値 / 右辺値規則を検討する。

### 内部で共有する単位

| 単位 | 共有対象 | storage と処理 |
| --- | --- | --- |
| 行 / 矩形 descriptor | すべて | read-only source と mutable destination、個別 stride。全体画像も同じ入力形式 |
| 画素変換 kernel | negate、grayscale、brighten、tint、alpha、channel swap | 行内を SIMD 化。連続した全体画像は一次元の fast path |
| LUT の準備と適用 | gamma、posterize、contrast、任意 LUT | 準備は画像 / 呼び出しごと、行ごと・画素ごとには作らない |
| 矩形転送と clipping | clipped、border、overwrite、合成、view の clone | コピー・合成する範囲を一度決め、行ループへ渡す |
| 配置変換 | mirror、flip、180 度、transpose、90/270 度 | in-place 交換と別出力への直接書き込みを使い分ける |
| 集計 | histogram、Otsu、mean、mosaic、alphaBounds | SIMD による画素計算と集約。再走査や atomic 更新を一律に導入しない |
| 近傍 filter | blur、Gaussian、adaptive、sharpen、convolve、min/max | border、係数、行 buffer / 中間出力を共有。演算自体は専用 kernel |
| resampling | scale、fit、affine、perspective、samplePixel | 座標・border・alpha の意味を共有。高品質縮小を単純 bilinear に置き換えない |
| mask と輪郭 | alpha / grayscale polygon | 二値 mask の生成と輪郭処理を分離。Polygon 生成は専用処理 |

基準は destination-writing kernel。値返し版は出力を確保してそこへ書き、
全体をコピーしてから変換するだけの実装を常に使うことは避ける。
in-place が安全な点処理は同じ kernel に同一範囲を渡せる。
近傍処理は元画素を保持する必要があるため、作業領域と別出力を使い分ける。
全体画像への破壊的処理は一時所有画像との swap が使えるが、view には使えない。

繰り返すフィルタでは係数・中間 buffer・出力の capacity を再利用できるようにする。
内部 workspace から始め、公開するなら対象の filter / resampler に応じた型にする。
mutable global scratch や、最初から全処理に共通の巨大な context は導入しない。
public wrapper の整合性確認、ISA 選択、準備を画素ループの外に置く。

`fill` と矩形 `overwrite` は共通化済み。次の既存処理の移行候補は alpha 変換、
R/B 交換、`ImagePainting` の合成、`ImageProcessing::Resize`。
連続画像の高速経路を保持し、空領域では画素ポインタの算術や SIMD load を行わない。

### 先に決める境界条件

**ROI の外側を読むか。** 第一案は view 自体を独立した画像として扱い、
filter の border も view の端に適用する。これなら view の clone を加工した結果と
対応しやすい。ただし旧 ROI と常に同じ結果にはならない。
旧 `spread` は親画像を sample し、旧 blur / Gaussian / morphology は親の
`cv::Mat` の部分行列を渡して `BORDER_ISOLATED` を指定していない。
OpenCV は [ROI の外側の画素を境界生成に使う仕組み](https://docs.opencv.org/4.x/d2/de8/group__core__array.html)
を持つため、旧 ROI のすべてを「切り出した独立画像と同じ」と解釈しない。

親画像の周囲も使って部分更新したい場合は、source の親画像 / read rectangle と
destination rectangle を別に渡す処理を用意する案とする。単なる view に
`useParent` フラグだけを足しても親の範囲情報は復元できない。
複数段 filter の halo と、書き換え前の周囲を読む順序も定める。

**alpha と色空間。** 現行 `Image` は alpha 表現のメタデータを持たず、ファイル読込は
既定で premultiply、色・generator・Grid からの作成は格納値をそのまま使う。
したがって全画像を常に straight または PMA と仮定できない。

| 操作 | 第一案・確認点 |
| --- | --- |
| RGB の色調補正 | alpha 保持を基本に、入力 alpha 表現を明示する設計を決める。PMA に単純な `255 - RGB` や加算をしてよいわけではない |
| LUT / gamma / posterize | straight RGB に定義する案。PMA 対応は変換と再乗算を kernel 内で融合し、画像全体の往復コピーを避ける。8-bit の往復誤差は残る |
| blur / scale / warp | 透明境界の色にじみを防ぐ PMA での重み付き処理を基準に検討。格納値の独立 channel filter とは意味を分ける |
| median / min-max / morphology | RGBA 各 channel、RGB のみ、alpha mask のどれを処理するかを区別。旧実装の RGBA 処理をそのまま見た目の正解としない |
| tint / opacity / mask | RGB 乗算、alpha の置換、alpha の乗算を区別。PMA では RGB と alpha の整合を保つ |
| image-to-image 合成 | 旧 `paint` は destination alpha を保持、旧 `stamp` は alpha の最大値等を使う。既存の straight-alpha Source Over とは異なる |

現在の [ImagePixel](../../../Siv3D/include/Siv3D/ImagePixel.hpp) は straight-alpha の
Source Over を実装している。新画像合成の意味を合わせる場合はこれを基準にし、
旧 `ImagePainting::Paint/Stamp` を名前だけ復活させない。
alpha 表現の自動推測や画素ごとの状態判定はしない。明示指定の型・既定値を決めてから
色を変える最初の API を実装する。既定の stored-byte 処理と linear-light 処理も区別する。

**重複と出力先。** view の矩形コピーは方向付きの `memmove` を実装済み。
今後の点処理・合成・近傍 filter は同じ実装を機械的に使わず、必要な source の退避を検討する。
destination を resize する前に alias を解決する。既存 Image 同士の自己コピー no-op は維持する。

**自然な端の条件。** 空画像、幅 / 高さ 1、filter 半径 0、一軸だけのぼかし、
ROI の部分交差、透明画像を通常の利用ケースとして定義する。
`blur` / `gaussianBlur` の旧 `size` は半径、`medianBlur` は窓幅であり、同じ意味ではない。
sigma 指定や大きな半径の近似 Gaussian は別途設計し、近似を隠して切り替えない。
非有限値や極端な入力の防御を理由に、既存 hot path へ無承認の分岐は追加しない。

## SIMD とアルゴリズムの選択

以下は実装候補であり、速度測定済みの結論ではない。

| 対象 | 有力な方法 | 注意 |
| --- | --- | --- |
| negate、brighten、channel swap、tint、alpha | byte / word の mask、飽和演算、shuffle、widen multiply | 丸め、alpha mask、scalar tail を揃える。複数画素をまとめて処理 |
| grayscale、threshold、sepia / saturation | channel の unpack、固定小数点 / float の積和 | 係数と丸めを先に確定。旧の各経路も同じ輝度式とは限らない |
| LUT | 小さな table の scalar lookup と SIMD 案を比較 | 256 要素 lookup は SIMD gather にすれば必ず速いわけではない |
| box blur | 水平・垂直の移動和、行 buffer | 大きな半径を全近傍の再集計で処理しない。累積値の幅も設計 |
| Gaussian | 分離畳み込み、係数再利用、垂直方向の SIMD | `O(WH(1+r_x+r_y))`。大半径では別の近似 API の価値を検討 |
| median | 小窓の比較 network、大窓の histogram 系 | 一つの SIMD 実装で全窓を処理する前提を置かない |
| dilation / erosion | 小窓の SIMD min/max、大窓の sliding min/max | kernel 形状と反復の等価性、境界条件を確認 |
| rotate / transpose / copy | タイル処理、連続コピー | メモリアクセスと cache locality が主。stdlib を測定の基準にする |
| Otsu、flood fill、輪郭 | histogram、scanline、輪郭追跡の適切なアルゴリズム | Otsu の閾値選択など小さな仕事の SIMD 化は優先しない |
| scale | 既存 stb_image_resize2 | 独自化の前に stride・alpha・補間条件と全体費用を比較 |

現行の alpha / R-B SIMD kernel には aligned load/store がある。
ROI の先頭や各行にその前提を持ち込まず、unaligned load/store、先頭の scalar 処理、
aligned 専用経路を比較する。画像末尾の確保 padding を ROI の余白と見なさず、
行幅を超えた読み書きや隣の画素の変更をしない。
x86 の SSE / AVX2 と ARM の NEON を個別に評価し、plain の自動 vectorize も比較する。
`useSIMD` のような切替は既存 API とテストを尊重しつつ、新 API へ機械的に増やさない。

## 実装の分け方と検証

view と矩形 copy / fill は採用済み。以降を次の単位で進める。

1. **基本操作**: alpha 契約を決め、negate / grayscale / brighten / LUT 系、
   mirror / flip / 直角回転、clipped / border、既存 resizer を使う scale / fit。
   source → destination、in-place、値返し版を同じ演算へまとめる。
2. **近傍・集計**: box / Gaussian、mosaic、histogram / Otsu、morphology、median。
   作業領域の再利用と ROI 境界を含めて評価する。合成は alpha 契約の決定後に独立して進める。
3. **用途別の拡張**: affine / perspective、flood fill、輪郭、bilateral、追加 codec。
   OpenCV 互換の維持費と Siv3D の用途を機能ごとに比較する。

既存の高速な全体画像処理が回帰しない粒度で移行する。

検証は [ホストの build/test 手順](../../development/README.md#build-and-test) に従う。
新しい view / filter は機能ごとのテストに分け、次を確かめる。

- 全体処理と全領域 view の一致、孤立 ROI と clone の処理結果の一致、ROI 外の画素不変。
- 空、1×1、1×N、奇数幅、SIMD 幅の前後、非整列 x offset、幅と一致しない行間隔、入出力で異なる stride。
- scalar と SIMD の丸め / alpha の一致。浮動小数点 filter は許容誤差を個別に定義。
- 完全 / 部分 overlap、出力再利用、親の右辺値からの借用拒否、constness。
- 半透明境界、透明画素内の RGB、edge mode、半径 0、非正方形の回転と clip。
- 単純な参照実装との結果比較。旧 OpenCV との比較は移植対象の契約確認に使い、
  旧出力との完全一致を無条件に新しい仕様にしない。

Release 測定では小さい sprite、大画像、細い / 非整列 ROI を使い、kernel 単体と
呼び出し全体（準備・確保・copy を含む）、workspace / 出力再利用あり・なしを分ける。
CPU 画像処理なので GPU upload memory の測定は対象経路を変更するときだけ必要。
性能閾値を正しさのテストに固定せず、実行時間、処理画素数、確保と一時メモリを記録する。
半透明素材の見た目は、採用する機能の共通ギャラリーで確認する。
