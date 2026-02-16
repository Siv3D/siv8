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

# pragma once
# include "../2DShapes.hpp"
# include "../JitterEndpointMode.hpp"
# include "../JitterEndpointSettings.hpp"
# include "../JitterSettings.hpp"

namespace s3d
{
	namespace Geometry2D
	{
		////////////////////////////////////////////////////////////////
		//
		//	Jitter
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		LineString Jitter(const Line& g, const JitterSettings& settings = {},
			const JitterEndpointSettings& endPoint = JitterEndpointSettings::Free());

		[[nodiscard]]
		LineString Jitter(const Line& g, const JitterSettings& settings,
			const JitterEndpointSettings& start, const JitterEndpointSettings& end);

		[[nodiscard]]
		LineString Jitter(const RectF& g, const JitterSettings& settings,
			const JitterEndpointSettings& endPoint = JitterEndpointSettings::Free());

		[[nodiscard]]
		LineString Jitter(const Triangle& g, const JitterSettings& settings,
			const JitterEndpointSettings& endPoint = JitterEndpointSettings::Free());

		[[nodiscard]]
		LineString Jitter(const Quad& g, const JitterSettings& settings,
			const JitterEndpointSettings& endPoint = JitterEndpointSettings::Free());

		[[nodiscard]]
		LineString Jitter(const LineString& g, const JitterSettings& settings = {},
			const JitterEndpointSettings& endPoint = JitterEndpointSettings::Free());

		[[nodiscard]]
		LineString Jitter(const LineString& g, const JitterSettings& settings,
			const JitterEndpointSettings& start, const JitterEndpointSettings& end);

		////////////////////////////////////////////////////////////////
		//
		//	JitterClosed
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		LineString JitterClosed(const LineString& g, const JitterSettings& settings = {},
			const JitterEndpointSettings& seam = JitterEndpointSettings::Free());
	}
}
