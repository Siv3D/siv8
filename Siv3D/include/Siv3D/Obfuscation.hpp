//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <ThirdParty/Obfuscate/obfuscate.h>

# ifndef SIV3D_OBFUSCATE_DEFAULT_KEY
#	define SIV3D_OBFUSCATE_DEFAULT_KEY AY_OBFUSCATE_DEFAULT_KEY
# endif

/// @brief 文字列は難読化されたバイナリデータとして実行ファイルに格納されます。 | The string is stored in the executable file as obfuscated binary data.
# define SIV3D_OBFUSCATE(s) AY_OBFUSCATE_KEY(s, SIV3D_OBFUSCATE_DEFAULT_KEY)

/// @brief 文字列は難読化されたバイナリデータとして実行ファイルに格納されます。 | The string is stored in the executable file as obfuscated binary data.
# define SIV3D_OBFUSCATE_WITH_KEY(s, key) AY_OBFUSCATE_KEY(s, key)
