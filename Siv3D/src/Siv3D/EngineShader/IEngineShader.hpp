﻿//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <Siv3D/Common.hpp>
# include <Siv3D/VertexShader.hpp>
# include <Siv3D/PixelShader.hpp>

namespace s3d
{
	enum class EngineVS
	{
		FullScreenTriangle,
		Shape2D,
		QuadWarp,
	};

	enum class EnginePS
	{
		FullScreenTriangle,
		Shape2D,
		Texture2D,
		QuadWarp,
		LineDot,
		LineDash,
		LineLongDash,
		LineDashDot,
		LineRoundDot,
		PatternPolkaDot,
		PatternStripe,
		PatternGrid,
		PatternChecker,
		PatternTriangle,
		PatternHexGrid,
	};

	class SIV3D_NOVTABLE ISiv3DEngineShader
	{
	public:

		[[nodiscard]]
		static ISiv3DEngineShader* Create();

		virtual ~ISiv3DEngineShader() = default;

		virtual void init() = 0;

		virtual const VertexShader& getVS(EngineVS vs) const = 0;

		virtual const PixelShader& getPS(EnginePS ps) const = 0;
	};
}
