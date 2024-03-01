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

/// @brief ライブラリのメジャーバージョン | Library major version
# define SIV3D_VERSION_MAJOR			0

/// @brief ライブラリのマイナーバージョン | Library minor version
# define SIV3D_VERSION_MINOR			8

/// @brief ライブラリのリビジョンバージョン | Library revision version
# define SIV3D_VERSION_REVISION			0

/// @brief プレリリースタグ | Pre-release tag
# define SIV3D_VERSION_PRERELEASE_TAG	U"dev"

/// @brief ライブラリのバージョン | Library version
# define SIV3D_VERSION			((SIV3D_VERSION_MAJOR * 100 * 100) + (SIV3D_VERSION_MINOR * 100) + (SIV3D_VERSION_REVISION))

/// @brief ライブラリのバージョンの文字列（簡易版） | Short Library version
# define SIV3D_VERSION_SHORT_STRING	U"0.8.0"

/// @brief ライブラリのバージョンの文字列 | Library version
# define SIV3D_VERSION_STRING	U"0.8.0" SIV3D_VERSION_PRERELEASE_TAG

/// @brief ライブラリのバージョン名 | Library version name
# define SIV3D_VERSION_NAME		U"Siv3D v8"
