//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# pragma once
# include <array>
# include "../ColorRenderTarget.hpp"
# include "../Optional.hpp"

namespace s3d::detail
{
	// Shared by the renderer and its scopes. Zero outputs represents Scene.
	// Disengaged slots do not construct Texture handles or allocate asset storage.
	struct RenderTargetState
	{
		static constexpr uint32 MaxCount = 8;

		std::array<Optional<ColorRenderTarget>, MaxCount> colors;

		uint32 count = 0;

		explicit operator bool() const noexcept { return (count != 0); }

		Size size() const { return colors[0]->size(); }

		friend bool operator ==(const RenderTargetState& a, const RenderTargetState& b) noexcept
		{
			if (a.count != b.count) { return false; }
			for (uint32 i = 0; i < a.count; ++i)
			{
				if (a.colors[i] != b.colors[i]) { return false; }
			}
			return true;
		}
	};
}
