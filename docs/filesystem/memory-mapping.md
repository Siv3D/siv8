# メモリマップトファイル

## API の役割と設計判断

| 型 | 責務 |
| --- | --- |
| [MappedMemoryView](../../Siv3D/include/Siv3D/MappedMemoryView.hpp) | 読み取り専用のアドレスとバイト数を渡す、所有権のない範囲 |
| [MappedMemory](../../Siv3D/include/Siv3D/MappedMemory.hpp) | 書き込み可能なアドレスとバイト数を渡す、所有権のない範囲 |
| [MemoryMappedFileView](../../Siv3D/include/Siv3D/MemoryMappedFileView.hpp) | 読み取り専用ファイルとマップの寿命を管理する所有者 |
| [MemoryMappedFile](../../Siv3D/include/Siv3D/MemoryMappedFile.hpp) | ファイルの作成・拡張・書き込み・同期を管理する所有者 |

この役割分担と既存の公開メソッドを維持する。範囲のコピーに参照カウントや
動的確保を加えず、所有者はコピー禁止・ムーブ可能とする。ムーブは既存のマップを
移すため、取得済みのアドレスを変更しない。ムーブ元の再オープン時にだけ、
内部状態を必要に応じて確保し直す。

既存のフォント・Exif・Base64 の呼び出しは、所有者が生存している間に取得した
範囲を使う構成で足りている。範囲自身に `close()` / `flush()` を持たせたり、
`MemoryMappedFileView` に読み込み位置を持たせたりする必要はない。
複数の範囲を独立して所有する API は、現在の単一マップの所有関係とは別の設計になる。

`isMapped()`、`asSpan()`、型付きアクセス、独立した `resize()` は今回追加しない。
現状の呼び出しでは、マップ結果の保持、`data` / `size` の組み合わせ、
書き込み範囲による拡張で必要な操作を表現できる。型付きアクセスには、さらに
アラインメント・オブジェクト寿命・ファイル形式のエンディアンの契約が必要になる。

## Reader と組み合わせる

読み込み位置が必要なら、取得した範囲を `MemoryViewReader` に渡せる。
この Reader もメモリを所有しないため、使用中はファイルの所有者を保持する。

```cpp
MemoryMappedFileView file{ U"data.bin" };
if (const auto mapped = file.mapAll())
{
    MemoryViewReader reader{ mapped.data, mapped.size };
    std::array<uint8, 4> prefix{};
    if (reader.read(prefix))
    {
        // prefix を処理する
    }
}
```

範囲や Reader をこの所有者より長く保持する場合は、所有者を一緒に移すか、
`Blob` / `MemoryReader` へデータをコピーする。
結果の `bool` はアドレスの有無を表すだけで、所有者の寿命を追跡しない。
境界条件、再マップ、失敗時の副作用の正確な契約は公開ヘッダを参照する。

## 実装と検証の境界

パスは共通層で所有するフルパスにしてからバックエンドへ渡す。
これにより `open(file.path())` でも、旧ファイルを閉じた際に入力が失われない。
パスの所有権はそのまま内部状態へ移し、同じパスを再度コピーしない。
ファイル種別・サイズの取得とページ境界への調整は OS 層の責務とする。

書き込み可能なマップは、範囲の加算前にファイルサイズで表現可能かを検査する。
読み取り専用マップはファイルの残りサイズを上限とする。
Windows では、マップ用ハンドルの作成とアドレスへのマップは別々に失敗し得る。
途中で取得したハンドルは失敗時に解放し、成功してから現在のマップとして保持する。

拡張とマップの作成はトランザクションではない。Windows の
[CreateFileMappingW](https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-createfilemappingw)
はビューの作成より先にファイルを拡張し得る。拡張部分の初期値もファイルシステムに
依存するため、移植可能なコードでは使用するバイトを明示的に書き込む。

`flush()` は明示的な同期操作として扱う。POSIX ではマップが存在する場合に
`msync(MS_SYNC)` を行い、続いて `fsync()` を行う。Windows ではマップが存在する
場合に `FlushViewOfFile()` を行い、続いて `FlushFileBuffers()` を行う。
ファイルのクローズだけでは同期完了を検査できない
（[Windows のバッファ同期](https://learn.microsoft.com/en-us/windows/win32/fileio/flushing-system-buffered-i-o-data-to-disk)）。

テストは [範囲の値型](../../Test/Test_MappedMemory.cpp)、
[ファイルの状態・ムーブ・書き込み](../../Test/Test_MemoryMappedFile.cpp)、
[読み取り専用範囲・リソース](../../Test/Test_MemoryMappedFileView.cpp) に置く。
OS の I/O 障害や電源断時の永続性は、通常の書き込み・再読込テストでは検証できない。
プラットフォーム固有の未検証事項は [TODO.md](../../TODO.md) で管理する。
