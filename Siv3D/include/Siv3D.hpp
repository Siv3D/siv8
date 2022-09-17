//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2022 Ryo Suzuki
//	Copyright (c) 2016-2022 OpenSiv3D Project
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
//----------------------------------------------------------------------------------------

# pragma once 

// Siv3D.hpp をインクルードすると定義されるマクロ
# define SIV3D_INCLUDED

//////////////////////////////////////////////////
//
//	ユーティリティ | Utilities
//
//////////////////////////////////////////////////

// プラットフォーム | Platform
# include <Siv3D/Platform.hpp>

// ライブラリのバージョン | Library version
# include <Siv3D/Version.hpp>

// 基本型 | Types
# include <Siv3D/Types.hpp>

// コンセプト | Concepts
# include <Siv3D/Concepts.hpp>

// ユーティリティー | Utilities
# include <Siv3D/Utility.hpp>


//// プレースホルダー | Place holder
//# include <Siv3D/PlaceHolder.hpp>
//
//// コピー禁止 Mixin | Non-copyable mixin
//# include <Siv3D/Uncopyable.hpp>
//
//// YesNo タグ | Yes/No tag
//# include <Siv3D/YesNo.hpp>
//
//// 定義済み YesNo タグ | Predefined Yes/No tag
//# include <Siv3D/PredefinedYesNo.hpp>
//
//// 名前付き引数 | Named parameter
//# include <Siv3D/NamedParameter.hpp>
//
//// 定義済み名前付き引数 | Predefined named parameters
//# include <Siv3D/PredefinedNamedParameter.hpp>
//
//// 範囲をインデックス値とセットで列挙するユーティリティー | Indexed iterator
//# include <Siv3D/Indexed.hpp>
//
//// メタ関数 | Meta
//# include <Siv3D/Meta.hpp>
//
//// ファンクター | Functor
//# include <Siv3D/Functor.hpp>
//
//// エンディアン | Endian
//# include <Siv3D/Endian.hpp>
//
//// バイト | Byte
//# include <Siv3D/Byte.hpp>
//
//// ハッシュ | Hash
//# include <Siv3D/Hash.hpp>
//
//// 範囲 | Range
//# include <Siv3D/Step.hpp>
//
//// 2D 範囲 | 2D range
//# include <Siv3D/Step2D.hpp>
//
//// 無限リスト | Infinite list
//# include <Siv3D/InfiniteList.hpp>
//
//// スコープ・ガード | Scope guard
//# include <Siv3D/ScopeGuard.hpp>
//
//// ユニーク・リソース | Unique resource
//# include <Siv3D/UniqueResource.hpp>
//
//// エラー | Error
//# include <Siv3D/Error.hpp>
//
//// デマングル | Demangle
//# include <Siv3D/Demangle.hpp>
//
//// Kahan の加算アルゴリズム | Kahan summation algorithm
//# include <Siv3D/KahanSummation.hpp>
//
//// SIMD | SIMD
//# include <Siv3D/SIMD.hpp>

//////////////////////////////////////////////////
//
//	メモリ | Memory
//
//////////////////////////////////////////////////

//// Null でないポインタ | Non-null pointer
//# include <Siv3D/NonNull.hpp>

// メモリアロケーション | Memory allocation
# include <Siv3D/Memory.hpp>

//////////////////////////////////////////////////
//
//	Optional クラス | Optional
//
//////////////////////////////////////////////////
//
//// 無効値 | None_t
//# include <Siv3D/None.hpp>
//
//// 無効値を持つ値 | Optional
//# include <Siv3D/Optional.hpp>
//
//// 特に指定しないことを表す無効値 | Representation of an unspecified value
//# include <Siv3D/Unspecified.hpp>

//////////////////////////////////////////////////
//
//	データ構造 | Data Structures
//
//////////////////////////////////////////////////

//// 動的配列（一次元）| Array
//# include <Siv3D/Array.hpp>
//
//// 動的配列（二次元）| 2D array
//# include <Siv3D/Grid.hpp>
//
//// 文字列ルックアップヘルパー | Heterogeneous lookup helper
//# include <Siv3D/HeterogeneousLookupHelper.hpp>
//
//// ハッシュテーブル | Hash table
//# include <Siv3D/HashTable.hpp>
//
//// ハッシュセット | Hash set
//# include <Siv3D/HashSet.hpp>
//
//// kd 木 | kd-tree
//# include <Siv3D/KDTree.hpp>
//
//// Disjoint-set (Union-find) | Disjoint-set (Union–find)
//# include <Siv3D/DisjointSet.hpp>

//////////////////////////////////////////////////
//
//	並列・並行処理 | Parallel and Concurrent
//
//////////////////////////////////////////////////

//// スレッド | Thread
//# include <Siv3D/Threading.hpp>
//
//// 非同期タスク | Asynchronous task
//# include <Siv3D/AsyncTask.hpp>
//
//// 子プロセス | Child process
//# include <Siv3D/ChildProcess.hpp>

//////////////////////////////////////////////////
//
//	文字と文字列 | Characters and String
//
//////////////////////////////////////////////////

//// 文字 | Character
//# include <Siv3D/Char.hpp>

// 文字列ビュー | String view
# include <Siv3D/StringView.hpp>

// 文字列 | String
# include <Siv3D/String.hpp>

// 文字コードの変換 | Character Code Conversion
# include <Siv3D/Unicode.hpp>

//// Unicode の逐次変換 | Unicode converter
//# include <Siv3D/UnicodeConverter.hpp>
//
//// アルファベットの大文字・小文字 | Letter case
//# include <Siv3D/LetterCase.hpp>
//
//// 正規表現のマッチ結果 | Regular expression matching
//# include <Siv3D/MatchResults.hpp>
//
//// 正規表現 | Regular expression
//# include <Siv3D/RegExp.hpp>
//
//// 実行ファイルに埋める文字列の難読化 | String literal obfuscation
//# include <Siv3D/Obfuscation.hpp>




# include <Siv3D/ConsoleBuffer.hpp>
# include <Siv3D/Console.hpp>







//////////////////////////////////////////////////
//
//	プラットフォーム | Platforms
//
//////////////////////////////////////////////////

// Windows ビルド用の静的リンクライブラリ | Static link libraries for Windows
# include <Siv3D/Windows/Libraries.hpp>

//////////////////////////////////////////////////
//
//	名前空間 | Name space
//
//////////////////////////////////////////////////

# if (!defined(SIV3D_LIBRARY_BUILD) && !defined(NO_S3D_USING))

	using namespace s3d;
	using namespace std::literals;

# endif
