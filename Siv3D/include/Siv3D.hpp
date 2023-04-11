//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
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
# define SIV3D_INCLUDED

# include <Siv3D/Version.hpp>
# include <Siv3D/Platform.hpp>
# include <Siv3D/CompilerVersion.hpp>
# include <Siv3D/Types.hpp>
# include <Siv3D/Concepts.hpp>
# include <Siv3D/Uncopyable.hpp>
# include <Siv3D/YesNo.hpp>
# include <Siv3D/PredefinedYesNo.hpp>
# include <Siv3D/NamedParameter.hpp>
# include <Siv3D/PredefinedNamedParameter.hpp>
//# include <Siv3D/Indexed.hpp>
# include <Siv3D/Utility.hpp>
//# include <Siv3D/Endian.hpp>
//# include <Siv3D/Byte.hpp>
# include <Siv3D/Hash.hpp>
//# include <Siv3D/Step.hpp>
//# include <Siv3D/Step2D.hpp>
//# include <Siv3D/InfiniteList.hpp>
//# include <Siv3D/ScopeGuard.hpp>
//# include <Siv3D/UniqueResource.hpp>
//# include <Siv3D/Error.hpp>
//# include <Siv3D/Demangle.hpp>
//# include <Siv3D/KahanSummation.hpp>
//# include <Siv3D/SIMD.hpp>

//# include <Siv3D/NonNull.hpp>
//# include <Siv3D/Memory.hpp>
//# include <Siv3D/Allocator.hpp>

# include <Siv3D/Optional.hpp>


//# include <Siv3D/Array.hpp>
//# include <Siv3D/Grid.hpp>
//# include <Siv3D/HeterogeneousLookupHelper.hpp>
//# include <Siv3D/OrderedTable.hpp>
//# include <Siv3D/HashTable.hpp>
//# include <Siv3D/HashSet.hpp>
//# include <Siv3D/KDTree.hpp>
//# include <Siv3D/DisjointSet.hpp>

//# include <Siv3D/Threading.hpp>
//# include <Siv3D/AsyncTask.hpp>
//# include <Siv3D/ChildProcess.hpp>

# include <Siv3D/Char.hpp>
# include <Siv3D/StringView.hpp>
//# include <Siv3D/String.hpp>
//# include <Siv3D/Unicode.hpp>
//# include <Siv3D/UnicodeConverter.hpp>
//# include <Siv3D/LetterCase.hpp>
//# include <Siv3D/MatchResults.hpp>
//# include <Siv3D/RegExp.hpp>
//# include <Siv3D/Obfuscation.hpp>

//# include <Siv3D/FormatBool.hpp>
//# include <Siv3D/FormatInt.hpp>
//# include <Siv3D/FormatFloat.hpp>
//# include <Siv3D/FormatData.hpp>
//# include <Siv3D/Formatter.hpp>
//# include <Siv3D/Format.hpp>
//# include <Siv3D/FormatLiteral.hpp>
//# include <Siv3D/FormatUtility.hpp>

//# include <Siv3D/ParseBool.hpp>
//# include <Siv3D/ParseInt.hpp>
//# include <Siv3D/ParseFloat.hpp>
//# include <Siv3D/Parse.hpp>


# include <Siv3D/ConsoleBuffer.hpp>
# include <Siv3D/Console.hpp>

# if !defined(SIV3D_LIBRARY_BUILD) && SIV3D_PLATFORM(WINDOWS)

	// Windows 用の静的リンクライブラリ | Static link libraries for Windows
	# include <Siv3D/Windows/Libraries.hpp>

# endif

# if (!defined(SIV3D_LIBRARY_BUILD) && !defined(NO_S3D_USING))

	using namespace s3d;
	using namespace std::literals;

# endif
