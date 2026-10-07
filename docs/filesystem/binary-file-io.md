# Blob とバイナリファイル入出力

公開契約は [Blob.hpp](../../Siv3D/include/Siv3D/Blob.hpp) と
[BinaryFileWriter.hpp](../../Siv3D/include/Siv3D/BinaryFileWriter.hpp) を参照する。

## 読み込み先の再利用

Reader を繰り返し消費する場合は、Blob を保持して
`createFromReader(reader)` に渡す。Reader の所有権を保持したまま、
Blob の容量を次の読み込みにも使える。

```cpp
Blob scratch{ Arg::reserve = size_t{ 65536 } };
BinaryFileReader reader{ U"input.bin" };
if (scratch.createFromReader(reader))
{
    // Process scratch. The reader has advanced to the initial end.
}
```

Reader 入力は現在位置から開始する。Reader を受け取るコンストラクタも同じ
読み込み範囲になる。`createFromFile()` が false を返す場合も内容を消去するため、
以前の open 失敗時の内容保持に依存したコードは見直す必要がある。

以前のデータを失敗時にも保持したい場合は、別の Blob で結果を受けてから交換する。
繰り返す処理では、両方の Blob を保持すれば作業用の領域も再利用できる。

```cpp
Blob current;
Blob pending;
// Repeat this block when a new file is available.
if (pending.createFromFile(U"input.bin"))
{
    current.swap(pending);
}
```

## 保存結果の確認

`BinaryFileWriter::write()` が全バイトを受け入れても、そのデータがまだ
バッファ内にあることがある。結果を使う処理ではデストラクタに任せず、
`close()` の結果を確認する。`Blob::save()` はこの確認まで行う。

## 検証

通常の回帰テストは [Test_Blob.cpp](../../Test/Test_Blob.cpp) と
[Test_BinaryFileWriter.cpp](../../Test/Test_BinaryFileWriter.cpp) にある。
Reader の分割読み込み・途中位置・途中終了・例外、ファイルの往復、バッファの
出力と再オープンを検査する。

macOS の故障注入は、現在のテストアプリをビルドしてから実行する。

```sh
./macOS/run-tests.sh
./tools/run-binary-writer-checks.sh
```

[実行スクリプト](../../tools/run-binary-writer-checks.sh) は
[テスト専用 dylib](../../tools/binary-writer-checks/Interpose.cpp) を生成し、
`DYLD_INSERT_LIBRARIES` をテストプロセスだけに指定する。
テストスレッドが指定したファイルの `fwrite()`、`fflush()`、`fclose()` に対して、
失敗または短い書き込みを一度だけ返す。close の注入時も実際のファイルは閉じる。
通常のエンジンには注入コードを組み込まない。

`[.writer-errors]` の非表示ケースが、明示的・自動的な flush、直接書き込み、
クローズの失敗、エラーの保持、再オープンによる復帰、`Blob::save()` の結果を検査する。
Windows / Linux のネイティブ障害注入はこのスクリプトの対象外。

入力ファイルはテストランナーの `Test/output/` に置かれ、ランナーが後始末する。
dylib と `results.txt` は表示された一時ディレクトリに保持する。
スクリプトの第 1 引数で成果物の出力ディレクトリを指定できる。
