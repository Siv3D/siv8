# Image

`Image` は CPU 上の RGBA8 画像を所有する。公開 API の契約は
[Image.hpp](../../Siv3D/include/Siv3D/Image.hpp) と
[ImageView.hpp](../../Siv3D/include/Siv3D/ImageView.hpp)、
[MutableImageView.hpp](../../Siv3D/include/Siv3D/MutableImageView.hpp) に置く。

- [メンバ関数の棚卸しと矩形領域の処理基盤案](proposals/member-functions-and-views.md):
  v0.6.16 との対応、v0.8 の追加候補、共通 kernel と SIMD の検討。
  view と矩形 copy / fill の基盤は実装済み。色調補正・filter などは未採用。
- [所有と借用](../array/array-like.md): 要素・行・列のアクセスと寿命。
- [2D 図形の Image 書き込み](../geometry2d/image-writing.md): 図形の rasterize と合成。
- [stb_image_resize2 の保守](../development/stb-image-resize2.md):
  `ImageProcessing::Resize` と mipmap 生成に使う実装。

現行実装の確認先は [Image.ipp](../../Siv3D/include/Siv3D/detail/Image.ipp)、
[SivImage.cpp](../../Siv3D/src/Siv3D/Image/SivImage.cpp)、
[SIMD 実装](../../Siv3D/src/Siv3D/Image/SivImage_SIMD.cpp)、
[Image のテスト](../../Test/Test_Image.cpp)、
[ImageProcessing のテスト](../../Test/Test_ImageProcessing.cpp)。
未完了の実装・設計判断は [TODO](../../TODO.md) で管理する。

## 矩形領域の参照とコピー

`image(rect)` は画像内との交差を参照する。画素を複製せず、局所座標で操作できる。
左上を `x, y` または `Point`、大きさを `w, h` または `Size` で指定できる。
正方形は大きさを 1 つの整数で指定する。いずれも `Rect` 版と同じ領域を返す。
mutable な Image からは `MutableImageView`、const な Image からは `ImageView` を返す。
読み取り専用の引数にはどちらも渡せる。書き込み可能な view 自身の const は、画素の const を意味しない。

```cpp
Image atlas{ 512, 512, Color{ 0, 0 } };
atlas(32, 16, 64, 48).fill(Color{ 255, 128, 64, 160 });

Image tile = atlas(Point{ 32, 16 }, Size{ 64, 48 }).cloned();
Image icon = atlas(Point{ 128, 16 }, 64).cloned();
Image reusable;
atlas(Rect{ 32, 16, 64, 48 }).copyTo(reusable);

// 同じ画像内でも、重なる領域を元の画素からコピーできる。
atlas(Rect{ 32, 16, 64, 48 }).overwrite(atlas, Point{ 40, 20 });
```

`subview` は view の局所座標でさらに領域を選ぶ。行には `row(y)` の span でアクセスする。
行間に親画像の別の画素があるため、`data()` から `pixelCount()` 個を連続走査してはいけない。
ビューを取得した後に親の storage を変える場合は、変更後にビューを取得し直す。
所有する一時画像から `operator()` でビューを取得することはできない。

`overwrite` はコピー先の寸法を変えずに書き込む。`copyTo` は所有画像の寸法を合わせ、
可能な場合は既存の確保を再利用する。自分の部分領域を `copyTo` する場合は、
source が無効になる前に一時画像へコピーする。
既存の `image.overwrite(image, pos)` の自己コピー no-op は維持している。
同じ画像内の移動には上のように view を使う。

## 共通基盤と性能

[ImageRegion.hpp](../../Siv3D/src/Siv3D/Image/ImageRegion.hpp) が clipping、fill、
矩形コピーを共有し、[SivImageView.cpp](../../Siv3D/src/Siv3D/Image/SivImageView.cpp)
が view と所有結果を接続する。処理は RGBA の格納値をそのまま扱い、alpha 表現を変換しない。

- 全体の fill は `std::fill_n` の一回の連続処理。strided view は行ごとに同じ処理を使う。
- 別画像への既存 overwrite は overlap 検査を経ずに `memcpy` を使う。
  入出力がどちらも連続する矩形なら、一回の `memcpy` にまとめる。
- view のコピーは overlap を呼び出しごとに判定し、重なるときだけ方向付きの
  `memmove` を使う。view を Image 由来に限定しているため、同じ storage では stride が一致する。
- fill / overwrite は一時画像を確保しない。`cloned()` は `copyTo()` に委譲する。
  alpha / R-B の SIMD kernel は引き続き全体 Image 専用で、非整列の view へは転用していない。

## 検証と測定

[Test_ImageView.cpp](../../Test/Test_ImageView.cpp) が借用・constness、clipping、異なる stride、
領域外の画素不変、重複コピー、出力再利用を確認する。
[Image の isolated checks](../development/README.md#isolated-image-checks-on-macos) は
view も ASan / UBSan の対象にし、確保回数とコピー先の確保失敗時の保持を検証する。

macOS ではリポジトリルートから、Xcode の build を sandbox 外で実行する。

```sh
./macOS/run-tests.sh '--test-case=~[benchmark]Image.*,~[benchmark]ImageView.*'
CONFIGURATION=Release ./macOS/run-tests.sh '--test-case=ImageView.benchmark.copy_fill'
./macOS/run-tests.sh
```

opt-in benchmark は以前の全体 fill / 行コピーを比較用に残し、小画像・中画像・大画像と
非整列 ROI で呼び出し全体を測る。画像の確保は計測外。出力はテストの標準出力で、
保存する場合は `/tmp/siv8-image-view-benchmark.log` などを明示してリダイレクトする。
ハードウェア依存の時間を correctness の合否条件にしない。
Windows の実行・最適化設定は [開発ガイド](../development/README.md#build-and-test) に従う。
