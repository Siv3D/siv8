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

namespace s3d
{
	/// @brief カラーマップ用のカラースケールの種類
	/// @remark https://github.com/yuki-koyama/tinycolormap
	enum class ColorScale
	{
		/// @url https://raw.githubusercontent.com/yuki-koyama/tinycolormap/master/docs/samples/Parula.png
		Parula,

		/// @url https://raw.githubusercontent.com/yuki-koyama/tinycolormap/master/docs/samples/Heat.png
		Heat,

		/// @url https://raw.githubusercontent.com/yuki-koyama/tinycolormap/master/docs/samples/Jet.png
		Jet,

		/// @url https://raw.githubusercontent.com/yuki-koyama/tinycolormap/master/docs/samples/Turbo.png
		Turbo,

		/// @url https://raw.githubusercontent.com/yuki-koyama/tinycolormap/master/docs/samples/Hot.png
		Hot,

		/// @url https://raw.githubusercontent.com/yuki-koyama/tinycolormap/master/docs/samples/Gray.png
		Gray,

		/// @url https://raw.githubusercontent.com/yuki-koyama/tinycolormap/master/docs/samples/Magma.png
		Magma,

		/// @url https://raw.githubusercontent.com/yuki-koyama/tinycolormap/master/docs/samples/Inferno.png
		Inferno,

		/// @url https://raw.githubusercontent.com/yuki-koyama/tinycolormap/master/docs/samples/Plasma.png
		Plasma,

		/// @url https://raw.githubusercontent.com/yuki-koyama/tinycolormap/master/docs/samples/Viridis.png
		Viridis,

		/// @url https://raw.githubusercontent.com/yuki-koyama/tinycolormap/master/docs/samples/Cividis.png
		Cividis,

		/// @url https://raw.githubusercontent.com/yuki-koyama/tinycolormap/master/docs/samples/Github.png
		Github,

		/// @url https://raw.githubusercontent.com/yuki-koyama/tinycolormap/master/docs/samples/Cubehelix.png
		Cubehelix,
	};
}
