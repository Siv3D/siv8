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
# include <Siv3D/Texture.hpp>
# include <Siv3D/Rect.hpp>
# include <Siv3D/TextureFilter.hpp>
# include <Siv3D/EngineShader/IEngineShader.hpp>
# include <array>
# include <span>

namespace s3d
{
	namespace detail
	{
		enum class TextureProcessingKernel : uint8
		{
			Copy,
			Resample,
			Downsample,
			Gaussian,
			DualUp,
			DualBlend,

			Count,
		};

		inline constexpr size_t MaxProcessingPasses = 32;
		inline constexpr size_t MaxGaussianSamples = 49;

		struct TextureProcessingPass
		{
			TextureProcessingKernel kernel = TextureProcessingKernel::Copy;
			Texture::IDType source = Texture::IDType::Null();
			Texture::IDType destination = Texture::IDType::Null();
			Texture::IDType secondary = Texture::IDType::Null();
			Rect sourceRect{ 0, 0, 0, 0 };
			Rect destinationRect{ 0, 0, 0, 0 };
			Rect secondaryRect{ 0, 0, 0, 0 };
			Float2 step{ 0, 0 };
			float blend = 1;
			TextureFilter filter = TextureFilter::Linear;
			bool preserveDestination = true;
			std::span<const Float4> samples;
		};

		// Shared VS/PS slot 0 layout with texture_processing.hlsl / texture_processing.metal.
		// Gaussian coefficients use a separate PS slot 1; only the active samples are uploaded.
		struct alignas(16) TextureProcessingConstants
		{
			Float4 sourceUV;
			Float4 sourceBounds;
			Float4 secondaryUV;
			Float4 secondaryBounds;
			Float4 parameters; // step in normalized UV, transition blend, Gaussian sample count
		};

		static_assert(sizeof(TextureProcessingConstants) == 80);

		struct TextureProcessingBinding
		{
			TextureProcessingConstants constants;
			EnginePS shader;
		};

		[[nodiscard]]
		TextureProcessingBinding MakeTextureProcessingBinding(const TextureProcessingPass& pass,
			Size sourceSize, Size secondarySize);
	}
}
