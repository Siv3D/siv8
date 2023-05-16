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
# include <Siv3D/Byte.hpp>
# include <Siv3D/Hash.hpp>
//# include <Siv3D/Step.hpp>
//# include <Siv3D/Step2D.hpp>
//# include <Siv3D/InfiniteList.hpp>
# include <Siv3D/ScopeExit.hpp>
# include <Siv3D/UniqueResource.hpp>
# include <Siv3D/Demangle.hpp>
# include <Siv3D/KahanSummation.hpp>
//# include <Siv3D/SIMD.hpp>

# include <Siv3D/NonNull.hpp>
# include <Siv3D/Memory.hpp>

# include <Siv3D/Optional.hpp>
# include <Siv3D/Expected.hpp>
# include <Siv3D/FunctionRef.hpp>

# include <Siv3D/Array.hpp>
//# include <Siv3D/Grid.hpp>
//# include <Siv3D/HeterogeneousLookupHelper.hpp>
//# include <Siv3D/OrderedTable.hpp>
//# include <Siv3D/HashTable.hpp>
//# include <Siv3D/HashSet.hpp>
//# include <Siv3D/KDTree.hpp>
//# include <Siv3D/DisjointSet.hpp>



# include <Siv3D/Thread.hpp>
# include <Siv3D/AsyncTask.hpp>
# include <Siv3D/TaskFlow.hpp>
//# include <Siv3D/ChildProcess.hpp>

# include <Siv3D/Char.hpp>
# include <Siv3D/StringView.hpp>
# include <Siv3D/String.hpp>
# include <Siv3D/Unicode.hpp>
# include <Siv3D/LetterCase.hpp>
//# include <Siv3D/UnicodeConverter.hpp>
# include <Siv3D/MatchResults.hpp>
# include <Siv3D/RegExp.hpp>
# include <Siv3D/Obfuscation.hpp>

# include <Siv3D/Error.hpp>

# include <Siv3D/BoolToString.hpp>
# include <Siv3D/IntToString.hpp>
# include <Siv3D/FloatToString.hpp>
# include <Siv3D/IntFormatter.hpp>
# include <Siv3D/FloatFormatter.hpp>
# include <Siv3D/FormatData.hpp>
# include <Siv3D/Formatter.hpp>
# include <Siv3D/RangeFormatter.hpp>
# include <Siv3D/TupleFormatter.hpp>
# include <Siv3D/Format.hpp>
# include <Siv3D/FormatLiteral.hpp>
# include <Siv3D/FormatUtility.hpp>
# include <Siv3D/ThousandSeparate.hpp>

# include <Siv3D/ParseErrorReason.hpp>
# include <Siv3D/ParseBool.hpp>
# include <Siv3D/ParseInt.hpp>
# include <Siv3D/ParseFloat.hpp>
# include <Siv3D/Parse.hpp>

# include <Siv3D/Number.hpp>
# include <Siv3D/Int128.hpp>
//# include <Siv3D/BigInt.hpp>
//# include <Siv3D/BigFloat.hpp>
//# include <Siv3D/CommonFloat.hpp>

# include <Siv3D/Duration.hpp>
# include <Siv3D/DayOfWeek.hpp>
# include <Siv3D/Date.hpp>
# include <Siv3D/DateTime.hpp>
# include <Siv3D/Time.hpp>
# include <Siv3D/ISteadyClock.hpp>
# include <Siv3D/Stopwatch.hpp>
//# include <Siv3D/VariableSpeedStopwatch.hpp>
//# include <Siv3D/Timer.hpp>

//# include <Siv3D/Profiler.hpp>
//# include <Siv3D/MillisecClock.hpp>
//# include <Siv3D/MicrosecClock.hpp>
//# include <Siv3D/RDTSCClock.hpp>
//# include <Siv3D/TimeProfiler.hpp>

# include <Siv3D/IReader.hpp>
# include <Siv3D/IWriter.hpp>
# include <Siv3D/SpecialFolder.hpp>
# include <Siv3D/CopyOption.hpp>
# include <Siv3D/FileSystem.hpp>
//# include <Siv3D/ResourceOption.hpp>
//# include <Siv3D/Resource.hpp>
//# include <Siv3D/FileAction.hpp>
//# include <Siv3D/DirectoryWatcher.hpp>
//# include <Siv3D/MemoryViewReader.hpp>
//# include <Siv3D/MemoryReader.hpp>
//# include <Siv3D/MemoryMappedFileView.hpp>
//# include <Siv3D/MemoryMappedFile.hpp>
//# include <Siv3D/Blob.hpp>
//# include <Siv3D/MemoryWriter.hpp>
# include <Siv3D/OpenMode.hpp>
# include <Siv3D/BinaryReader.hpp>
# include <Siv3D/BinaryWriter.hpp>
//# include <Siv3D/Base64.hpp>
//# include <Siv3D/MD5Value.hpp>
//# include <Siv3D/MD5.hpp>
//# include <Siv3D/Crypto.hpp>
//# include <Siv3D/ArchivedFileReader.hpp>
//# include <Siv3D/FileArchive.hpp>


//# include <Siv3D/Zlib.hpp>
//# include <Siv3D/Compression.hpp>
//# include <Siv3D/ZIPReader.hpp>
////# include <Siv3D/ZIPWriter.hpp> // [Siv3D ToDo]

//# include <Siv3D/TextEncoding.hpp>
//# include <Siv3D/TextReader.hpp>
//# include <Siv3D/TextWriter.hpp>
//# include <Siv3D/CSV.hpp>
//# include <Siv3D/INI.hpp>
//# include <Siv3D/JSON.hpp>
//# include <Siv3D/JSONValidator.hpp>
//# include <Siv3D/XMLReader.hpp>
//# include <Siv3D/TOMLReader.hpp>
//# include <Siv3D/GeoJSON.hpp>
//# include <Siv3D/HTMLWriter.hpp>

# include <Siv3D/LogType.hpp>
//# include <Siv3D/Logger.hpp>
//# include <Siv3D/LicenseInfo.hpp>
//# include <Siv3D/LicenseManager.hpp>
//# include <Siv3D/MathConstants.hpp>
//# include <Siv3D/Math.hpp>
//# include <Siv3D/FastMath.hpp>
//# include <Siv3D/Interpolation.hpp>
//# include <Siv3D/Easing.hpp>
//# include <Siv3D/EasingAB.hpp>
//# include <Siv3D/SimpleAnimation.hpp>
//# include <Siv3D/Transition.hpp>
//# include <Siv3D/Periodic.hpp>
//# include <Siv3D/PoissonDisk2D.hpp>
//# include <Siv3D/Spline.hpp>
//# include <Siv3D/SIMDMath.hpp>
//# include <Siv3D/HalfFloat.hpp>
//# include <Siv3D/SIMD_Float4.hpp>
//# include <Siv3D/MathParser.hpp>
//# include <Siv3D/Statistics.hpp>
//# include <Siv3D/PrimeNumber.hpp>

//# include <Siv3D/Noise.hpp>
//# include <Siv3D/PerlinNoise.hpp>

//# include <Siv3D/ColorHSV.hpp>
//# include <Siv3D/Color.hpp>
//# include <Siv3D/ColorF.hpp>
//# include <Siv3D/HSV.hpp>
//# include <Siv3D/Palette.hpp>x
//# include <Siv3D/Colormap.hpp>



# include <Siv3D/System.hpp>

# include <Siv3D/WindowState.hpp>
# include <Siv3D/Window.hpp>

# include <Siv3D/MonitorInfo.hpp>
# include <Siv3D/Monitor.hpp>

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
