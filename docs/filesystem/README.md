# FileSystem

公開契約は [FileSystem.hpp](../../Siv3D/include/Siv3D/FileSystem.hpp)、
自動テストは [Test_FileSystem.cpp](../../Test/Test_FileSystem.cpp) を参照してください。
このディレクトリでは、実装の評価と設計判断の根拠を管理します。

- [基盤見直し案](proposals/foundation-review.md): 通常のファイル操作に影響する課題と対応順。
- [メモリマップトファイル](memory-mapping.md): API の役割、所有関係、実装と検証の境界。
- [Blob とバイナリファイル入出力](binary-file-io.md): 容量の再利用、読み込み結果の置換、保存エラーの検証。
- [挙動・タイミング確認プログラム](../../Test/Manual/FileSystemReview.md):
  OS 間の挙動比較と性能調査に使う診断。
- [macOS の Trash 確認](../../Test/Manual/FileSystemRemoveContentsTrash.md)。

未完了の作業は [TODO.md](../../TODO.md) で管理します。

## ディレクトリ監視の実装と検証

macOS の `DirectoryWatcher` は監視ごとの serial dispatch queue で FSEvents を
受け取り、作成元スレッドの RunLoop に依存しません。終了時は stream を停止・無効化
してから queue 上のコールバックを待ち、監視状態を解放します。コールバックから
ユーザーの処理を直接呼ばず、変更一覧は mutex を介して受け渡します。

[Test_DirectoryWatcher.cpp](../../Test/Test_DirectoryWatcher.cpp) は、未開始・無効なパス、
macOS での作成・変更・削除通知、拡張子フィルター、通知を伴う監視の反復終了を検査します。
macOS の通知検査は `System::Update()` や RunLoop を回さずに実行します。
ファイル通知は OS によってまとめられるため、通知の件数・到着順序を固定しません。

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
