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

# include <Siv3D/2DShapes.hpp>
# include <Siv3D/FloatQuad.hpp>
# include <Siv3D/Pattern/PatternParameters.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	const Quad& Quad::draw(const ColorF& color) const
	{
		SIV3D_ENGINE(Renderer2D)->addQuad(FloatQuad{ *this }, color.toFloat4());
		return *this;
	}

	const Quad& Quad::draw(const ColorF& color0, const ColorF& color1, const ColorF& color2, const ColorF& color3) const
	{
		const Float4 colors[4] = { color0.toFloat4(), color1.toFloat4(), color2.toFloat4(), color3.toFloat4() };
		SIV3D_ENGINE(Renderer2D)->addQuad(FloatQuad{ *this }, colors);
		return *this;
	}

	const Quad& Quad::draw(const PatternParameters& pattern) const
	{
		SIV3D_ENGINE(Renderer2D)->addQuad(FloatQuad{ *this }, pattern);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	void Quad::ThrowTriangleAtIndexOutOfRange()
	{
		throw std::out_of_range{ "Quad::triangleAtIndex() index out of range" };
	}
}
