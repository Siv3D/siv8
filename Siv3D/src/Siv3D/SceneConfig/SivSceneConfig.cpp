//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <array>
# include <Siv3D/SceneConfig.hpp>
# include <Siv3D/StringView.hpp>
# include <Siv3D/FormatData.hpp>

namespace s3d
{
	void Formatter(FormatData& formatData, const SceneSizeMode value)
	{
		static constexpr std::array names{ U"Fixed"_sv, U"Window"_sv, U"WindowPixels"_sv };
		formatData.string.append(names[FromEnum(value)]);
	}

	void Formatter(FormatData& formatData, const SceneResolution value)
	{
		static constexpr std::array names{ U"SceneSize"_sv, U"Display"_sv };
		formatData.string.append(names[FromEnum(value)]);
	}

	void Formatter(FormatData& formatData, const FullscreenSceneMode value)
	{
		static constexpr std::array names{ U"KeepSize"_sv, U"FollowWindow"_sv };
		formatData.string.append(names[FromEnum(value)]);
	}
}
