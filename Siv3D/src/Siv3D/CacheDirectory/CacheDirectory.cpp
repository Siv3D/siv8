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

# include <Siv3D/SpecialFolder.hpp>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/FormatLiteral.hpp>
# include <Siv3D/Version.hpp>
# include "CacheDirectory.hpp"

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static String GetUniqueID(const StringView applicationName)
		{
			if (const auto name = String{ applicationName }.removed_if([](char32 c)
				{
					return (c == U'\\')
						|| (c == U'/')
						|| (c == U',')
						|| (c == U'.')
						|| (c == U';')
						|| (c == U':')
						|| (c == U'*')
						|| (c == U'?')
						|| (c == U'\"')
						|| (c == U'<')
						|| (c == U'>')
						|| (c == U'|');
				}))
			{
				return name;
			}
			else
			{
				return U"{:0>16X}"_fmt(applicationName.hash());
			}
		}
	}

	namespace CacheDirectory
	{
		////////////////////////////////////////////////////////////////
		//
		//	Engine
		//
		////////////////////////////////////////////////////////////////

		FilePath Engine()
		{
			FilePath path = FileSystem::GetFolderPath(SpecialFolder::LocalAppData);
			path += U"Siv3D/" SIV3D_VERSION_STRING U"/";
			return path;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Apps
		//
		////////////////////////////////////////////////////////////////

		FilePath Apps(const StringView applicationName)
		{
			FilePath path = FileSystem::GetFolderPath(SpecialFolder::LocalAppData);
			path += U"Siv3DApps/" + GetUniqueID(applicationName) + U"/";
			return path;
		}
	}
}
