# FileSystem

公開契約は [FileSystem.hpp](../../Siv3D/include/Siv3D/FileSystem.hpp)、
自動テストは [Test_FileSystem.cpp](../../Test/Test_FileSystem.cpp) を参照してください。
このディレクトリでは、実装の評価と設計判断の根拠を管理します。

- [基盤見直し案](proposals/foundation-review.md): 通常のファイル操作に影響する課題と対応順。
- [メモリマップトファイル](memory-mapping.md): API の役割、所有関係、実装と検証の境界。
- [挙動・タイミング確認プログラム](../../Test/Manual/FileSystemReview.md):
  OS 間の挙動比較と性能調査に使う診断。
- [macOS の Trash 確認](../../Test/Manual/FileSystemRemoveContentsTrash.md)。

未完了の作業は [TODO.md](../../TODO.md) で管理します。

## スクリーンショットの保存先

スクリーンショットの公開契約は
[ScreenCapture.hpp](../../Siv3D/include/Siv3D/ScreenCapture.hpp)、
保存先と画像の往復テストは
[Test_ScreenCapture.cpp](../../Test/Test_ScreenCapture.cpp) を参照してください。
既定保存先への出力、macOS の権限ダイアログ、Windows の UNC 保存先は
[手動確認プログラム](../../Test/Manual/ScreenCapture.md) で確認できます。

```cpp
ScreenCapture::SaveCurrentFrame(); // ピクチャ/Screenshot/ に自動命名
ScreenCapture::SaveCurrentFrame(U"boss-clear.png"); // 同じフォルダに名前を付けて保存
ScreenCapture::SaveCurrentFrameTo(U"output/boss-clear.png"); // 作業ディレクトリからの相対パス
```

任意の出力先を渡していたコードは `SaveCurrentFrameTo(path)` に移します。
`SetScreenshotDirectory()` で設定したフォルダへの名前付き保存は、引き続き
`SaveCurrentFrame(fileName)` を使います。空の名前をメモリ取得に使っていたコードは
`RequestCurrentFrame()` に移します。

Windows の既定保存先もピクチャフォルダ内になりました。実行ファイル横や作業
ディレクトリへ出力したいアプリは `SetScreenshotDirectory()` で明示します。
相対指定した保存ディレクトリや予約済みの保存先は、後から作業ディレクトリを
変更しても移動しません。
