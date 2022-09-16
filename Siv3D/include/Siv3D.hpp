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

// 基本型 | Types
# include <Siv3D/Types.hpp>




//////////////////////////////////////////////////
//
//	文字と文字列 | Characters and String
//
//////////////////////////////////////////////////

// 文字列ビュー | String view
# include <Siv3D/StringView.hpp>

// 文字列 | String
# include <Siv3D/String.hpp>

// Unicode | Unicode
# include <Siv3D/Unicode.hpp>





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
