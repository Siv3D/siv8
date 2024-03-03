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

//# include <Siv3D/Step2D.hpp> // ToDo

//# include <Siv3D/InfiniteList.hpp> // ToDo

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
# include <Siv3D/String.hpp> // ToDo

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

//// Base64 | Base64
//# include <Siv3D/Base64.hpp>

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
# include <Siv3D/Array.hpp> // ToDo

//// 動的配列（二次元）| 2D array
//# include <Siv3D/Grid.hpp>

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
# include <Siv3D/BigInt.hpp> // ToDo

//// 多倍長浮動小数点数 | 100 decimal digits precision floating point type
//# include <Siv3D/BigFloat.hpp>
//
//// 浮動小数点数型の共通型 | Common Float
//# include <Siv3D/CommonFloat.hpp>

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

//// スピードを変更可能なストップウォッチ | Stopwatch with variable speed
//# include <Siv3D/VariableSpeedStopwatch.hpp>
//
//// カウントダウンタイマー | Countdown timer
//# include <Siv3D/Timer.hpp>

////////////////////////////////////////////////////////////////
//
//	プロファイラー | Profiler
//
////////////////////////////////////////////////////////////////

//// プロファイラー | Profiler
//# include <Siv3D/Profiler.hpp>
//
//// 処理にかかった時間の測定 | Clock counter in milliseconds
//# include <Siv3D/MillisecClock.hpp>
//
//// 処理にかかった時間の測定 | Clock counter in microseconds
//# include <Siv3D/MicrosecClock.hpp>
//
//// Clock counter in CPU clocks | 処理にかかった CPU サイクル数の測定
//# include <Siv3D/RDTSCClock.hpp>
//
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
//
//// 所有権を持たない、メモリ範囲への IReader インタフェース | IReader Wrapper for Non-Owning Memory Range 
//# include <Siv3D/MemoryViewReader.hpp>
//
//// メモリ範囲への IReader インタフェース | IReader Wrapper for Memory Range 
//# include <Siv3D/MemoryReader.hpp>
//
//// メモリマップトファイル | Memory mapped file
//# include <Siv3D/MemoryMappedFileView.hpp>
//
//// 書き込み可能なメモリマップトファイル | Writable memory mapped file
//# include <Siv3D/MemoryMappedFile.hpp>

// メモリ上のバッファ | Binary Large Object
# include <Siv3D/Blob.hpp> // ToDo

//// 書き込み用バッファ | Memory writer
//# include <Siv3D/MemoryWriter.hpp>

// ファイルオープンモード / File open mode
# include <Siv3D/OpenMode.hpp>

// 読み込み専用バイナリファイル | Binary file reader
# include <Siv3D/BinaryReader.hpp> // ToDo

// 書き込み専用バイナリファイル | Binary file writer
# include <Siv3D/BinaryWriter.hpp> // ToDo

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
//
//// Zstandard 方式による可逆圧縮 | Lossless compression with Zstandard algorithm
//# include <Siv3D/Compression.hpp>
//
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

//// 書き込み専用テキストファイル | Text file writer
//# include <Siv3D/TextWriter.hpp>

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

//// ライセンス情報 | License information
//# include <Siv3D/LicenseInfo.hpp>
//
//// ライセンス出力の管理 | License management
//# include <Siv3D/LicenseManager.hpp>

////////////////////////////////////////////////////////////////
//
//	数学 | Math
//
////////////////////////////////////////////////////////////////

// 数学定数 | Math constants
# include <Siv3D/MathConstants.hpp>

// カハンの加算アルゴリズム | Kahan summation algorithm
# include <Siv3D/KahanSummation.hpp>

// 精度を落とした高速な数学関数 | Fast math functions
# include <Siv3D/FastMath.hpp>


////////////////////////////////////////////////////////////////
//
//	ノイズ | Noise
//
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
//
//	色 | Color
//
////////////////////////////////////////////////////////////////

// 色 | Colors
# include <Siv3D/ColorHSV.hpp> // ToDo

// 色 (uint8 x 4) | Color (uint8 * 4)
# include <Siv3D/Color.hpp> // ToDo

// 色 (double * 4) | Color (double * 4)
# include <Siv3D/ColorF.hpp> // ToDo

// HSV カラー | HSV color
# include <Siv3D/HSV.hpp> // ToDo

// 色定数 | Color constants
# include <Siv3D/Palette.hpp> // ToDo

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

//// ランダムな時間 | Random duration
//# include <Siv3D/RandomDuration.hpp>
//
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
//
// UUID | UUID
# include <Siv3D/UUIDValue.hpp>







////////////////////////////////////////////////////////////////
//
//	2D 図形 | 2D Geometry
//
////////////////////////////////////////////////////////////////

// ベクトル | Vectors
# include <Siv3D/PointVector.hpp> // ToDo

// 整数の 2 次元ベクトル | 2D vector (integer)
# include <Siv3D/Point.hpp> // ToDo

// 2 次元ベクトル | 2D vector
# include <Siv3D/Vector2D.hpp> // ToDo

//// 3 次元ベクトル | 3D vector
//# include <Siv3D/Vector3D.hpp>
//
//// 4 次元ベクトル | 4D vector
//# include <Siv3D/Vector4D.hpp>
//
//// 浮動小数点数の計算補助 | Floating point support
//# include <Siv3D/CommonFloat.hpp>
//
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





















# include <Siv3D/ConsoleBuffer.hpp>
# include <Siv3D/Console.hpp>


# if SIV3D_PLATFORM(WINDOWS)

	// Windows 用の静的リンクライブラリ | Static link libraries for Windows
	# include <Siv3D/Windows/Libraries.hpp>

# endif

# if (!defined(SIV3D_LIBRARY_BUILD) && !defined(NO_S3D_USING))

	using namespace s3d;
	using namespace std::literals;

# endif
