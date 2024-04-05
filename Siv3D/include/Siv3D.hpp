//--------------------------------------------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files(the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions :
//	
//	The above copyright notice and this permission notice shall be included in
//	all copies or substantial portions of the Software.
//	
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//	THE SOFTWARE.
//
//--------------------------------------------------------------------------------

# pragma once 
# define SIV3D_INCLUDED

////////////////////////////////////////////////////////////////
//
//	ユーティリティ | Utilities
//
////////////////////////////////////////////////////////////////

// ライブラリのバージョン | Library version
# include <Siv3D/Version.hpp>

// プラットフォーム | Platform
# include <Siv3D/Platform.hpp>

// コンパイラのバージョン | Compiler version
# include <Siv3D/CompilerVersion.hpp> // ToDo

// 基本型のエイリアス | Type aliases
# include <Siv3D/Types.hpp>

// コンセプト | Concepts
# include <Siv3D/Concepts.hpp>

// Yes/No タグ | Yes/No tag
# include <Siv3D/YesNo.hpp>

// 定義済み Yes/No タグ | Predefined Yes/No tag
# include <Siv3D/PredefinedYesNo.hpp> // ToDo

// 名前付き引数 | Named parameter
# include <Siv3D/NamedParameter.hpp>

// 定義済み名前付き引数 | Predefined named parameter
# include <Siv3D/PredefinedNamedParameter.hpp> // ToDo

// ユーティリティー | Utilities
# include <Siv3D/Utility.hpp>

// 1 バイトのデータを表現する型 | Type representing 1-byte data
# include <Siv3D/Byte.hpp>

// エンディアン変換　| Endian conversion
# include <Siv3D/Endian.hpp>

// ハッシュ関数 | Hash function
# include <Siv3D/Hash.hpp>

// スコープガード | Scope guard
# include <Siv3D/ScopeExit.hpp>

// リソースハンドルの RAII ラッパー | RAII wrapper for resource handle
# include <Siv3D/UniqueResource.hpp>

// Null にならないことを表すポインタ型ラッパー | Non-null pointer wrapper
# include <Siv3D/NonNull.hpp>

// メモリ | Memory
# include <Siv3D/Memory.hpp>

// アライメント指定付きのメモリアロケータ | Aligned allocator
# include <Siv3D/AlignedAllocator.hpp>

// デマングル | Demangle
# include <Siv3D/Demangle.hpp>

# include <Siv3D/SIMD.hpp>

// 所有権を持たない関数ラッパー | Non-owning function wrapper
# include <Siv3D/FunctionRef.hpp>

////////////////////////////////////////////////////////////////
//
//	エラー | Error
//
////////////////////////////////////////////////////////////////

// エラー型 | Error type
# include <Siv3D/Error.hpp>

// パースエラー | Parse error
# include <Siv3D/Error/ParseError.hpp>

// エンジン内部エラー | Internal engine error
# include <Siv3D/Error/InternalEngineError.hpp>

////////////////////////////////////////////////////////////////
//
//	range とビュー | Range and View
//
////////////////////////////////////////////////////////////////

// インデックスと要素をペアにしたビュー | Indexed view
# include <Siv3D/Indexed.hpp>

# include <Siv3D/Step.hpp>

# include <Siv3D/Step2D.hpp>

# include <Siv3D/Step3D.hpp>

////////////////////////////////////////////////////////////////
//
//	文字と文字列 | Characters and String
//
////////////////////////////////////////////////////////////////

// 文字 | Character
# include <Siv3D/Char.hpp>

// 文字列ビュー | String view
# include <Siv3D/StringView.hpp>

// 文字列 | String
# include <Siv3D/String.hpp>

// Unicode | Unicode
# include <Siv3D/Unicode.hpp>

// Unicode の逐次変換 | Unicode converter
# include <Siv3D/UnicodeConverter.hpp>

// アルファベットの大文字・小文字 | Letter case
# include <Siv3D/LetterCase.hpp>

//// 正規表現のマッチ結果 | Regular expression matching
//# include <Siv3D/MatchResults.hpp>
//
//// 正規表現 | Regular expression
//# include <Siv3D/RegExp.hpp>

// 実行ファイルに格納される文字列の難読化 | String obfuscation
# include <Siv3D/Obfuscation.hpp>

// Base64 | Base64
# include <Siv3D/Base64Value.hpp>

////////////////////////////////////////////////////////////////
//
//	データ構造 | Data Structures
//
////////////////////////////////////////////////////////////////

// オプショナル型 | Optional
// 値とその有無を表します。 | Represents a value and its presence.
# include <Siv3D/Optional.hpp>

// 成功/失敗の結果型 | Expected
// 成功時は値を、失敗時はエラーを保持します。 | Holds a value in case of success, or an error in case of failure.
# include <Siv3D/Result.hpp>

// 動的配列（一次元）| Array
# include <Siv3D/Array.hpp>

// 動的配列（二次元）| 2D array
# include <Siv3D/Grid.hpp> // ToDo

// 文字列ルックアップヘルパー | Heterogeneous lookup helper
# include <Siv3D/HeterogeneousLookupHelper.hpp>

// 順序付けセット | Ordered set
# include <Siv3D/OrderedSet.hpp>

// 順序付けテーブル | Ordered table
# include <Siv3D/OrderedTable.hpp>

// ハッシュセット | Hash set
# include <Siv3D/HashSet.hpp>

// ハッシュテーブル | Hash table
# include <Siv3D/HashTable.hpp>

//// kd 木 | kd-tree
//# include <Siv3D/KDTree.hpp>

// Disjoint-set (Union-find) | Disjoint-set (Union–find)
# include <Siv3D/DisjointSet.hpp>

//// モートン順序 | Morton order
//# include <Siv3D/Morton.hpp>

////////////////////////////////////////////////////////////////
//
//	並列・並行処理 | Parallel and Concurrent
//
////////////////////////////////////////////////////////////////

// スレッド | Thread
# include <Siv3D/Threading.hpp>

// 非同期タスク | Asynchronous task
# include <Siv3D/AsyncTask.hpp>

//// 子プロセス | Child process
//# include <Siv3D/ChildProcess.hpp>

////////////////////////////////////////////////////////////////
//
//	文字列フォーマット | Formatting
//
////////////////////////////////////////////////////////////////

// bool 型から文字列への変換 | Converting bool to string
# include <Siv3D/BoolToString.hpp>

// 整数型から文字列への変換 | Converting integer to string
# include <Siv3D/IntToString.hpp>
# include <Siv3D/IntFormatter.hpp>

// 浮動小数点数型から文字列への変換 | Converting floating point number to string
# include <Siv3D/FloatToString.hpp>
# include <Siv3D/FloatFormatter.hpp>

// Format 用のデータ | Data for Format
# include <Siv3D/FormatData.hpp>

// 基本的なフォーマッタ | Basic formatters
# include <Siv3D/Formatter.hpp>

// 配列のフォーマッタ | Array formatter
# include <Siv3D/SequenceFormatter.hpp>

// タプルのフォーマッタ | Tuple formatter
# include <Siv3D/TupleFormatter.hpp>

// マップのフォーマッタ | Map formatter
# include <Siv3D/MapFormatter.hpp>

// フォーマット | Format
# include <Siv3D/Format.hpp>

// フォーマットリテラル | Format literal
# include <Siv3D/FormatLiteral.hpp>

// Fmt の拡張の補助 | Fmt helper
# include <Siv3D/FmtHelper.hpp>

// Fmt の拡張 | Fmt extension
# include <Siv3D/FmtExtension.hpp>

// フォーマットユーティリティ | Format utility
# include <Siv3D/FormatUtility.hpp>

// 桁区切り | Digit separator
# include <Siv3D/ThousandSeparate.hpp>

////////////////////////////////////////////////////////////////
//
//	文字列パーサ | Parser
//
////////////////////////////////////////////////////////////////

// パースエラーの原因 | Parse error reason
# include <Siv3D/ParseErrorReason.hpp>

// 文字列 → bool 変換 | Text-to-bool conversion
# include <Siv3D/ParseBool.hpp>
# include <Siv3D/ParseBoolExtended.hpp>

// 文字列 → 整数 変換 | Text-to-integer conversion
# include <Siv3D/ParseInt.hpp>

// 文字列 → 浮動小数点数 変換 | Text-to-float conversion
# include <Siv3D/ParseFloat.hpp>

// 文字列 → 値 変換 | Text-to-value conversion
# include <Siv3D/Parse.hpp>

////////////////////////////////////////////////////////////////
//
//	数値 | Number
//
////////////////////////////////////////////////////////////////

// 数値のプロパティ | Properties of numbers
# include <Siv3D/Number.hpp>

// 128-bit 整数 | 128-bit integer
# include <Siv3D/Int128.hpp>

// 多倍長整数 | Arbitrary precision integer type
# include <Siv3D/BigInt.hpp>

// 多倍長浮動小数点数 | 100 decimal digits precision floating point type
# include <Siv3D/BigFloat.hpp>

// 浮動小数点数型の共通型 | Common Float
# include <Siv3D/CommonFloat.hpp>

////////////////////////////////////////////////////////////////
//
//	時刻と時間 | Time and Duration
//
////////////////////////////////////////////////////////////////

// 時間の表現 | Duration
# include <Siv3D/Duration.hpp>

// 曜日 | Day of week
# include <Siv3D/DayOfWeek.hpp>

// 日付 | Date
# include <Siv3D/Date.hpp>

// 日付と時刻 | Date and time
# include <Siv3D/DateTime.hpp>

// 時間 | Time
# include <Siv3D/Time.hpp>

////////////////////////////////////////////////////////////////
//
//	タイマー | Timer
//
////////////////////////////////////////////////////////////////

// タイムポイント提供インタフェース | Time point provider interface
# include <Siv3D/ISteadyClock.hpp>

// 経過時間の計測 | Stopwatch
# include <Siv3D/Stopwatch.hpp>

// スピードを変更可能なストップウォッチ | Stopwatch with variable speed
# include <Siv3D/VariableSpeedStopwatch.hpp>

// カウントダウンタイマー | Countdown timer
# include <Siv3D/Timer.hpp>

////////////////////////////////////////////////////////////////
//
//	プロファイラー | Profiler
//
////////////////////////////////////////////////////////////////

// プロファイラーの統計情報 | Profiler statistics
# include <Siv3D/ProfilerStat.hpp> // ToDo

// プロファイラー | Profiler
# include <Siv3D/Profiler.hpp> // ToDo

// 処理にかかった時間の測定 | Clock counter in milliseconds
# include <Siv3D/MillisecClock.hpp>

// 処理にかかった時間の測定 | Clock counter in microseconds
# include <Siv3D/MicrosecClock.hpp>

// 処理にかかった CPU サイクル数の測定 | Clock counter in CPU cycles
# include <Siv3D/CycleClock.hpp>

//// 時間の測定 | Time profiler
//# include <Siv3D/TimeProfiler.hpp>

////////////////////////////////////////////////////////////////
//
//	ファイル I/O | File I/O
//
////////////////////////////////////////////////////////////////

// IReader インタフェース | IReader interface
# include <Siv3D/IReader.hpp>

// IWriter インタフェース | IWriter interface
# include <Siv3D/IWriter.hpp>

// 特殊フォルダ | Special folders
# include <Siv3D/SpecialFolder.hpp>

// ファイルコピーオプション | File copy option
# include <Siv3D/CopyOption.hpp>

// ファイルとディレクトリ | Files and directories
# include <Siv3D/FileSystem.hpp>

//// リソースファイルの設定 | Resource file option
//# include <Siv3D/ResourceOption.hpp>
//
//// リソースファイルの管理 | Resource files
//# include <Siv3D/Resource.hpp>

// ファイル操作のイベント | File action
# include <Siv3D/FileAction.hpp>

//// ファイルとディレクトリの変更の監視 | Directory watcher
//# include <Siv3D/DirectoryWatcher.hpp>

// 所有権を持たない、メモリ範囲への IReader インタフェース | IReader Wrapper for Non-Owning Memory Range 
# include <Siv3D/MemoryViewReader.hpp>

// メモリ範囲への IReader インタフェース | IReader Wrapper for Memory Range 
# include <Siv3D/MemoryReader.hpp>

// マップされた読み込み専用メモリ | Mapped memory
# include <Siv3D/MappedMemory.hpp>

// 読み込み専用メモリマップトファイル | Read-only memory mapped file
# include <Siv3D/MemoryMappedFileView.hpp>

// マップされた書き込み可能なメモリ | Mapped memory
# include <Siv3D/WritableMappedMemory.hpp>

// 書き込み可能なメモリマップトファイル | Writable memory mapped file
# include <Siv3D/MemoryMappedFile.hpp>

// メモリ上のバッファ | Binary Large Object
# include <Siv3D/Blob.hpp>

// 書き込み用バッファ | Memory writer
# include <Siv3D/MemoryWriter.hpp>

// ファイルオープンモード / File open mode
# include <Siv3D/OpenMode.hpp>

// 読み込み専用バイナリファイル | Binary file reader
# include <Siv3D/BinaryReader.hpp>

// 書き込み専用バイナリファイル | Binary file writer
# include <Siv3D/BinaryWriter.hpp>

// アーカイブファイルからの読み込み | Archived file reader
//# include <Siv3D/ArchivedFileReader.hpp>

// アーカイブファイル | File archive
//# include <Siv3D/FileArchive.hpp>

////////////////////////////////////////////////////////////////
//
//	暗号法 | Cryptography
//
////////////////////////////////////////////////////////////////

// MD5 ハッシュ | MD5 hash
# include <Siv3D/MD5Value.hpp>

// 暗号化 | Cryptography
//# include <Siv3D/Crypto.hpp>

////////////////////////////////////////////////////////////////
//
//	データ圧縮 | Data Compression
//
////////////////////////////////////////////////////////////////

//// zlib 方式による可逆圧縮 | Lossless compression with zlib format
//# include <Siv3D/Zlib.hpp>

// Zstandard 方式による可逆圧縮 | Lossless compression with Zstandard algorithm
# include <Siv3D/Compression.hpp>

//// ZIP 圧縮ファイルの読み込み | ZIP reader
//# include <Siv3D/ZIPReader.hpp>
//
//// ZIP 圧縮ファイルの書き出し | ZIP writer
////# include <Siv3D/ZIPWriter.hpp>

////////////////////////////////////////////////////////////////
//
//	テキストファイル | Text File
//
////////////////////////////////////////////////////////////////

// 文字列エンコード | Text encoding
# include <Siv3D/TextEncoding.hpp>

// 読み込み専用テキストファイル | Text file reader
# include <Siv3D/TextReader.hpp>

// 書き込み専用テキストファイル | Text file writer
# include <Siv3D/TextWriter.hpp>

////////////////////////////////////////////////////////////////
//
//	設定ファイル | Configuration File
//
////////////////////////////////////////////////////////////////

//// CSV データの読み書き | CSV reader/writer
//# include <Siv3D/CSV.hpp>
//
//// INI データの読み書き | INI reader/writer
//# include <Siv3D/INI.hpp>
//
//// JSON データの読み書き | JSON reader/writer
//# include <Siv3D/JSON.hpp>
//
//// JSON データの検証 | JSON validation
//# include <Siv3D/JSONValidator.hpp>
//
//// TOML ファイルの読み込み | TOML reader/writer
//# include <Siv3D/TOML>
//
//// XML ファイルの読み込み | XML parser
//# include <Siv3D/XMLReader.hpp>
//
//// GeoJSON ファイルの読み込み | GeoJSON parser
//# include <Siv3D/GeoJSON.hpp>
//
//// HTML ファイルの書き出し | HTML document writer
//# include <Siv3D/HTMLWriter.hpp>

////////////////////////////////////////////////////////////////
//
//	ログ出力 | Logging
//
////////////////////////////////////////////////////////////////

// ログの種類 | The type of the log message
# include <Siv3D/LogType.hpp>

# include <Siv3D/LoggerBuffer.hpp>

// ロガー | Logger
# include <Siv3D/Logger.hpp>

// ライセンス情報 | License information
# include <Siv3D/LicenseInfo.hpp>

// ライセンス出力の管理 | License management
# include <Siv3D/LicenseManager.hpp>

////////////////////////////////////////////////////////////////
//
//	数学 | Math
//
////////////////////////////////////////////////////////////////

// 数学定数 | Math constants
# include <Siv3D/MathConstants.hpp>

// カハンの加算アルゴリズム | Kahan summation algorithm
# include <Siv3D/KahanSummation.hpp>

//// 数学関数 | Math functions
//# include <Siv3D/Math.hpp>

// 数学関数 (BigInt, BigFloat) | Math functions (BigInt, BigFloat)
# include <Siv3D/BigNumMath.hpp>

// 精度を落とした高速な数学関数 | Fast math functions
# include <Siv3D/FastMath.hpp>

//// 線形補間 | Interpolation
//# include <Siv3D/Interpolation.hpp>
//
//// イージング関数 | Easing
//# include <Siv3D/Easing.hpp>
//
//// イージングによる往復 | Bidirectional easing
//# include <Siv3D/EasingAB.hpp>
//
//// キーフレームによるアニメーション | Keyframe animation
//# include <Siv3D/SimpleAnimation.hpp>
//
//// トランジション | Transition
//# include <Siv3D/Transition.hpp>
//
//// 周期関数 | Periodic functions
//# include <Siv3D/Periodic.hpp>
//
//// Poisson Disk 2D | Poisson disk 2D
//# include <Siv3D/PoissonDisk2D.hpp>
//
//// スプライン | Spline
//# include <Siv3D/Spline.hpp>
//
//// SIMD 演算 | SIMD
//# include <Siv3D/SIMDMath.hpp>
//
//// 半精度浮動小数点数 | Half-precision floating-point
//# include <Siv3D/HalfFloat.hpp>
//
//// SIMD 対応 Float4 | SIMD Float4
//# include <Siv3D/SIMD_Float4.hpp>
//
//// 数式パーサ | Math parser
//# include <Siv3D/MathParser.hpp>
//
//// 統計 | Statistics
//# include <Siv3D/Statistics.hpp>
//
//// 素数判定 | Primality test
//# include <Siv3D/PrimeNumber.hpp>

////////////////////////////////////////////////////////////////
//
//	色 | Color
//
////////////////////////////////////////////////////////////////

// 色 | Colors
# include <Siv3D/ColorHSV.hpp>

// 色 (uint8 x 4) | Color (uint8 * 4)
# include <Siv3D/Color.hpp>

// 色 (double * 4) | Color (double * 4)
# include <Siv3D/ColorF.hpp>

// HSV カラー | HSV color
# include <Siv3D/HSV.hpp>

// 色定数 | Color constants
# include <Siv3D/Palette.hpp>

//// カラーマップ | Colormap
//# include <Siv3D/Colormap.hpp>

////////////////////////////////////////////////////////////////
//
//	乱数と分布 | Random and Distribution
//
////////////////////////////////////////////////////////////////

// 非決定的乱数生成器 | Non-deterministic random number generator
# include <Siv3D/HardwareRNG.hpp>

// 擬似乱数発生器 | Pseudorandom number generator
# include <Siv3D/PRNG.hpp>

// 乱数の分布 | Random number distribution
# include <Siv3D/Distribution.hpp>

// 離散確率分布 | Discrete probability distribution
# include <Siv3D/DiscreteDistribution.hpp>

// 乱数 | Random number
# include <Siv3D/Random.hpp>

// シャッフル | Random shuffle
# include <Siv3D/Shuffle.hpp>

// ランダムな時間 | Random duration
# include <Siv3D/RandomDuration.hpp>

//// ランダムな Point の作成 | Random Point generation
//# include <Siv3D/RandomPoint.hpp>
//
//// ランダムな Vec2 の作成 | Random Vec2 generation
//# include <Siv3D/RandomVec2.hpp>
//
//// ランダムな Vec3 の作成 | Random Vec3 generation
//# include <Siv3D/RandomVec3.hpp>
//
//// ランダムな Vec4 の作成 | Random Vec4 generation
//# include <Siv3D/RandomVec4.hpp>
//
//// ランダムな色の作成 | Random Color generation
//# include <Siv3D/RandomColor.hpp>
//
//// ランダムなサンプリング | Random sampling
//# include <Siv3D/Sample.hpp>

//// Improved Perlin ノイズ | Improved Perlin noise
//# include <Siv3D/PerlinNoise.hpp>

////////////////////////////////////////////////////////////////
//
//	2D 図形 | 2D Geometry
//
////////////////////////////////////////////////////////////////

// ベクトル | Vectors
# include <Siv3D/PointVector.hpp>

// 整数の 2 次元ベクトル | 2D vector (integer)
# include <Siv3D/Point.hpp> // ToDo

// 整数の 3 次元ベクトル | 3D vector (integer)
# include <Siv3D/Point3D.hpp> // ToDo

// 2 次元ベクトル | 2D vector
# include <Siv3D/Vector2D.hpp> // ToDo

// 3 次元ベクトル | 3D vector
# include <Siv3D/Vector3D.hpp> // ToDo

// 4 次元ベクトル | 4D vector
# include <Siv3D/Vector4D.hpp> // ToDo

//// ベクトルクラステンプレートの計算補助 | Vector class templates support
//# include <Siv3D/CommonVector.hpp>
//
//// 2D 描画の頂点 | Vertex elements for 2D rendering
//# include <Siv3D/Vertex2D.hpp>
//
//// 円座標 | Polar coordinates system (2D)
//# include <Siv3D/Circular.hpp>
//
//// オフセット付き円座標 | Polar coordinates system with offset (2D)
//# include <Siv3D/OffsetCircular.hpp>
//
//// 円柱座標 | Cylindrical coordinate system
//# include <Siv3D/Cylindrical.hpp>
//
//// 球面座標 | Spherical coordinate system
//# include <Siv3D/Spherical.hpp>
//
//// 3x2 行列 | 3x2 matrix
//# include <Siv3D/Mat3x2.hpp>
//
//// 3x3 行列 | 3x3 matrix
//# include <Siv3D/Mat3x3.hpp>
//
//// 2D 図形コレクション | 2D shape collection
//# include <Siv3D/2DShapes.hpp>
//
//// 線のスタイル | Line style
//# include <Siv3D/LineStyle.hpp>
//
//// 線分 | Line segment
//# include <Siv3D/Line.hpp>

// 長方形 (int32) | Rectangle (int32)
# include <Siv3D/Rect.hpp> // ToDo

//// 長方形 (double) | Rectangle (double)
//# include <Siv3D/RectF.hpp>
//
//// 円 | Circle
//# include <Siv3D/Circle.hpp>
//
//// 楕円 | Ellipse
//# include <Siv3D/Ellipse.hpp>
//
//// 三角形 | Triangle
//# include <Siv3D/Triangle.hpp>
//
//// 凸四角形 | Convex quadrilateral
//# include <Siv3D/Quad.hpp>
//
//// 角丸長方形 | Rounded rectangle
//# include <Siv3D/RoundRect.hpp>
//
//// 点の集合（とそれをつないで表現される線分） | A sequence of points (and the line segments connecting them)
//# include <Siv3D/LineString.hpp>
//
//// 多角形 | Polygon
//# include <Siv3D/Polygon.hpp>
//
//// 複数の多角形 | Multi-polygon
//# include <Siv3D/MultiPolygon.hpp>
//
//// 2 次ベジェ曲線 | Quadratic Bézier curve
//# include <Siv3D/Bezier2.hpp>
//
//// 3 次ベジェ曲線 | Cubic Bézier curve
//# include <Siv3D/Bezier3.hpp>
//
//// Catmull-Rom スプライン曲線 | Catmull-Rom spline curves
//# include <Siv3D/Spline2D.hpp>
//
//// float 型の長方形 | Rectangle (float)
//# include <Siv3D/FloatRect.hpp>
//
//// float 型の四角形 | Quad (float)
//# include <Siv3D/FloatQuad.hpp>
//
//// 2D 形状コレクション | 2D shapes collection
//# include <Siv3D/Shape2D.hpp>
//
//// 2D 描画バッファ | Native 2D drawing buffer
//# include <Siv3D/Buffer2D.hpp>
//
//// 2D 幾何 | 2D geometry processing
//# include <Siv3D/Geometry2D.hpp>
//
//// 長方形詰込み | Rectangle packing
//# include <Siv3D/RectanglePacking.hpp>
//
//// 平面分割 | Planar subdivisions
//# include <Siv3D/Subdivision2D.hpp>

////////////////////////////////////////////////////////////////
//
//	システム | System
//
////////////////////////////////////////////////////////////////

//// 環境変数 | Environment variable
//# include <Siv3D/EnvironmentVariable.hpp>
//
//// コマンドライン引数 | Command-line arguments
//# include <Siv3D/CommandLine.hpp>

// CPU 情報 | CPU information
# include <Siv3D/CPUInfo.hpp>

// 画面解像度の定数 | Display resolution constants
# include <Siv3D/DisplayResolution.hpp>

// モニターの情報 | Monitor information
# include <Siv3D/MonitorInfo.hpp>

// ユーザアクション | User action
# include <Siv3D/UserAction.hpp>

// システム | System
# include <Siv3D/System.hpp> // ToDo

//// クリップボード | Clipboard
//# include <Siv3D/Clipboard.hpp>
//
//// ドライブの種類 | Drive type
//# include <Siv3D/DriveType.hpp>
//
//// ドライブの情報 | Drive information
//# include <Siv3D/DriveInfo.hpp>

// UUID | UUID
# include <Siv3D/UUIDValue.hpp>

////////////////////////////////////////////////////////////////
//
//	コンソール入出力 | Console input/output
//
////////////////////////////////////////////////////////////////

# include <Siv3D/ConsoleBuffer.hpp>

// コンソール入出力 | Console input/output
# include <Siv3D/Console.hpp>

////////////////////////////////////////////////////////////////
//
//	ネットワーク | Network
//
////////////////////////////////////////////////////////////////

//// Twitter | Twitter
//# include <Siv3D/Twitter.hpp>
//
//// IPv4 アドレス | IPv4 address
//# include <Siv3D/IPv4Address.hpp>
//
//// ネットワーク | Networking
//# include <Siv3D/Network.hpp>

////////////////////////////////////////////////////////////////
//
//	TCP 通信 | TCP Communication
//
////////////////////////////////////////////////////////////////

//// TCP 通信のエラー | TCP connection errors
//# include <Siv3D/TCPError.hpp>
//
//// TCP サーバ | TCP server
//# include <Siv3D/TCPServer.hpp>
//
//// TCP クライアント | TCP client
//# include <Siv3D/TCPClient.hpp>

////////////////////////////////////////////////////////////////
//
//	OSC 通信 | OSC Communication
//
////////////////////////////////////////////////////////////////

//// OSC 型タグ
//# include <Siv3D/OSCTypeTag.hpp>
//
//// OSC 送信メッセージ
//# include <Siv3D/OSCMessage.hpp>
//
//// OSC 送信クラス
//# include <Siv3D/OSCSender.hpp>
//
//// OSC 受信メッセージの引数
//# include <Siv3D/OSCArgument.hpp>
//
//// OSC 受信メッセージ
//# include <Siv3D/ReceivedOSCMessage.hpp>
//
//// OSC 受信クラス
//# include <Siv3D/OSCReceiver.hpp>

////////////////////////////////////////////////////////////////
//
//	HTTP 通信 | HTTP Communication
//
////////////////////////////////////////////////////////////////

//// URL | URL
//# include <Siv3D/URL.hpp>
//
//# include <Siv3D/URLView.hpp>
//
//# include <Siv3D/HTTPStatusCode.hpp>
//
//# include <Siv3D/HTTPResponse.hpp>
//
//# include <Siv3D/HTTPAsyncStatus.hpp>
//
//# include <Siv3D/HTTPProgress.hpp>
//
//# include <Siv3D/AsyncHTTPTask.hpp>
//
//// シンプルな HTTP 通信ライブラリ | Simple HTTP library
//# include <Siv3D/SimpleHTTP.hpp>

////////////////////////////////////////////////////////////////
//
//	シリアル通信 | Serial Communication
//
////////////////////////////////////////////////////////////////

//// シリアルポートの情報 | Serial port information
//# include <Siv3D/SerialPortInfo.hpp>
//
//// シリアル通信 | Serial communication
//# include <Siv3D/Serial.hpp>

////////////////////////////////////////////////////////////////
//
//	パイプ | Pipe
//
////////////////////////////////////////////////////////////////

//// パイプ通信 | Interprocess communication (Pipe)
//# include <Siv3D/Pipe.hpp>

////////////////////////////////////////////////////////////////
//
//	ウィンドウ | Window
//
////////////////////////////////////////////////////////////////

// ウィンドウスタイル | Window Style
# include <Siv3D/WindowStyle.hpp> // ToDo

// ウィンドウステート | Window State
# include <Siv3D/WindowState.hpp> // ToDo

// ウィンドウ | Window
# include <Siv3D/Window.hpp> // ToDo

////////////////////////////////////////////////////////////////
//
//	ダイアログ | Dialog
//
////////////////////////////////////////////////////////////////

//// ファイルダイアログ | File dialog
//# include <Siv3D/Dialog.hpp>
//
//// ファイル拡張子フィルタ | File extension filter
//# include <Siv3D/FileFilter.hpp>


////////////////////////////////////////////////////////////////
//
//	電源の状態 | Power Status
//
////////////////////////////////////////////////////////////////

//// 電源の接続状態 | AC line status
//# include <Siv3D/ACLineStatus.hpp>
//
//// バッテリーの残量 | Battery status
//# include <Siv3D/BatteryStatus.hpp>
//
//// 電源の状態 | Power status
//# include <Siv3D/PowerStatus.hpp>

////////////////////////////////////////////////////////////////
//
//	文章読み上げ | Text to Speech
//
////////////////////////////////////////////////////////////////

//// 言語コード | Language code
//# include <Siv3D/LanguageCode.hpp>
//
//// 文章読み上げ | Text-to-speech
//# include <Siv3D/TextToSpeech.hpp>
//
//// 文章読み上げオブジェクト | Text to speech object
//# include <Siv3D/Say.hpp>
//
//// KlattTTS のパラメータ | Parameters for KlattTTS
//# include <Siv3D/KlattTTSParameters.hpp>
//
//// Klatt 方式による音声読み上げ | Text-to-speech using Klatt's method
//# include <Siv3D/KlattTTS.hpp>

////////////////////////////////////////////////////////////////
//
//	ドラッグ & ドロップ | Drag & Drop
//
////////////////////////////////////////////////////////////////

//// ドラッグするアイテムの種類 | Dragged item type
//# include <Siv3D/DragItemType.hpp>
//
//// ドラッグの状態 | Dragging status
//# include <Siv3D/DragStatus.hpp>
//
//// ドロップされたファイルパスの情報 | Dropped file path
//# include <Siv3D/DroppedFilePath.hpp>
//
//// ドロップされたテキストの情報 | Dropped text
//# include <Siv3D/DroppedText.hpp>
//
//// ドラッグ＆ドロップ | Drag & drop
//# include <Siv3D/DragDrop.hpp>

////////////////////////////////////////////////////////////////
//
//	メッセージボックス | Message Box
//
////////////////////////////////////////////////////////////////

//// メッセージボックスの結果 | Result from messagebox
//# include <Siv3D/MessageBoxResult.hpp>
//
//// メッセージボックスのスタイル | Message box style
//# include <Siv3D/MessageBoxStyle.hpp>
//
//// メッセージボックス | Message box
//# include <Siv3D/MessageBox.hpp>

////////////////////////////////////////////////////////////////
//
//	トースト通知 | Toast Notification
//
////////////////////////////////////////////////////////////////

//// トースト通知アイテム | Toast notification item
//# include <Siv3D/ToastNotificationItem.hpp>
//
//// トースト通知の状態 | Toast notification state
//# include <Siv3D/ToastNotificationState.hpp>
//
//// トースト通知 | Toast notification
//# include <Siv3D/ToastNotification.hpp>

////////////////////////////////////////////////////////////////
//
//	画像処理 | Image Processing
//
////////////////////////////////////////////////////////////////

//// 画像のフォーマット | Image format
//# include <Siv3D/ImageFormat.hpp>
//
//// ピクセルフォーマット | Pixel format
//# include <Siv3D/ImagePixelFormat.hpp>
//
//// 画像の情報 | Image information
//# include <Siv3D/ImageInfo.hpp>
//
//// ピクセルアドレスモード | Image address mode
//# include <Siv3D/ImageAddressMode.hpp>
//
//// 境界線の種類 | Border type
//# include <Siv3D/BorderType.hpp>
//
//// 適応的閾値処理の手法 | Adaptive thresholding method
//# include <Siv3D/AdaptiveThresholdMethod.hpp>
//
//// 塗りつぶし処理の境界処理手法 | Flood fill method
//# include <Siv3D/FloodFillConnectivity.hpp>
//
//// 拡大縮小時の補間アルゴリズム | Interpolation algorithms for image scaling
//# include <Siv3D/InterpolationAlgorithm.hpp>
//
//// エッジ保持フィルタ | Edge preserving filter
//# include <Siv3D/EdgePreservingFilterType.hpp>
//
//// ボックスフィルタ | Box filter
//# include <Siv3D/BoxFilterSize.hpp>

// 画像 | Image
# include <Siv3D/Image.hpp> // ToDo

//// 追加の画像処理 | Extra image processing
//# include <Siv3D/ImageProcessing.hpp>
//
//// カスケード分類器 | Cascade classifier
//# include <Siv3D/CascadeClassifier.hpp>
//
//// 画像の関心領域 | Image region of interest
//# include <Siv3D/ImageROI.hpp>
//
//// 背景抽出のフラグ | Grab cut flag
//# include <Siv3D/GrabCutClass.hpp>
//
//// 背景抽出 | Grab cut
//# include <Siv3D/GrabCut.hpp>

////////////////////////////////////////////////////////////////
//
//	画像形式 | Image Encoding
//
////////////////////////////////////////////////////////////////

//# include <Siv3D/IImageDecoder.hpp>
//# include <Siv3D/IImageEncoder.hpp>
//
//# include <Siv3D/ImageEncoder.hpp>
//# include <Siv3D/ImageDecoder.hpp>
//
//# include <Siv3D/PNGFilter.hpp>
//# include <Siv3D/ImageFormat/PNGDecoder.hpp>
//# include <Siv3D/ImageFormat/PNGEncoder.hpp>
//
//# include <Siv3D/ImageFormat/JPEGDecoder.hpp>
//# include <Siv3D/ImageFormat/JPEGEncoder.hpp>
//# include <Siv3D/Exif.hpp>
//
//# include <Siv3D/ImageFormat/BMPDecoder.hpp>
//# include <Siv3D/ImageFormat/BMPEncoder.hpp>
//
//# include <Siv3D/WebPMethod.hpp>
//# include <Siv3D/ImageFormat/WebPDecoder.hpp>
//# include <Siv3D/ImageFormat/WebPEncoder.hpp>
//
//# include <Siv3D/ImageFormat/GIFDecoder.hpp>
//# include <Siv3D/ImageFormat/GIFEncoder.hpp>
//
//# include <Siv3D/ImageFormat/TIFFDecoder.hpp>
////# include <Siv3D/ImageFormat/TIFFEncoder.hpp>
//
//# include <Siv3D/ImageFormat/TGADecoder.hpp>
//# include <Siv3D/ImageFormat/TGAEncoder.hpp>
//
//# include <Siv3D/PPMType.hpp>
//# include <Siv3D/ImageFormat/PPMDecoder.hpp>
//# include <Siv3D/ImageFormat/PPMEncoder.hpp>
//
//# include <Siv3D/ImageFormat/SVGDecoder.hpp>
//
//// SVG 画像 | SVG image
//# include <Siv3D/SVG.hpp>
//
//// アニメーション　GIF の読み込み | Animated GIF reader
//# include <Siv3D/AnimatedGIFReader.hpp>
//
//// アニメーション GIF の書き出し | Animated GIF writer
//# include <Siv3D/AnimatedGIFWriter.hpp>
//
////# include <Siv3D/ImageFormat/DDSDecoder.hpp>
////# include <Siv3D/ImageFormat/DDSEncoder.hpp>
////# include <Siv3D/ImageFormat/JPEG2000Decoder.hpp>
////# include <Siv3D/ImageFormat/JPEG2000Encoder.hpp>


















# if SIV3D_PLATFORM(WINDOWS)

	// Windows 用の静的リンクライブラリ | Static link libraries for Windows
	# include <Siv3D/Windows/Libraries.hpp>

# endif

# if (!defined(SIV3D_LIBRARY_BUILD) && !defined(NO_S3D_USING))

	using namespace s3d;
	using namespace std::literals;

# endif
