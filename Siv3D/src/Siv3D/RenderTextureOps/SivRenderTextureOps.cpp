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

# include <Siv3D/RenderTextureOps.hpp>
# include <Siv3D/RenderTexture.hpp>
# include <Siv3D/TextureRegion.hpp>
# include <Siv3D/Error.hpp>
# include <Siv3D/Texture/ITexture.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include "TextureProcessing.hpp"
# include <algorithm>
# include <cmath>
# include <cassert>

namespace s3d
{
	namespace detail
	{
		TextureProcessingBinding MakeTextureProcessingBinding(const TextureProcessingPass& pass,
			const Size sourceSize, const Size secondarySize)
		{
			TextureProcessingBinding result{};
			auto& constants = result.constants;
			const auto setRegion = [](Float4& uv, Float4& bounds, const Rect& rect, const Size size)
			{
				const float x = (1.0f / size.x);
				const float y = (1.0f / size.y);
				uv = Float4{ rect.x * x, rect.y * y, rect.w * x, rect.h * y };
				bounds = Float4{ (rect.x + 0.5f) * x, (rect.y + 0.5f) * y,
					(rect.x + rect.w - 0.5f) * x, (rect.y + rect.h - 0.5f) * y };
			};
			setRegion(constants.sourceUV, constants.sourceBounds, pass.sourceRect, sourceSize);
			const bool secondary = (pass.secondary != Texture::IDType::Null());
			if (secondary)
			{
				setRegion(constants.secondaryUV, constants.secondaryBounds, pass.secondaryRect, secondarySize);
			}
			constants.parameters = Float4{ pass.step.x / sourceSize.x, pass.step.y / sourceSize.y,
				pass.blend, static_cast<float>(pass.samples.size()) };
			std::copy(pass.samples.begin(), pass.samples.end(), constants.samples.begin());
			const bool bounded = (pass.sourceRect != Rect{ sourceSize })
				|| (secondary && (pass.secondaryRect != Rect{ secondarySize }));
			result.shader = static_cast<EnginePS>(static_cast<uint32>(EnginePS::ProcessingResample)
				+ (static_cast<uint32>(pass.kernel) - 1) * 2 + static_cast<uint32>(bounded));
			return result;
		}
	}

	namespace RenderTextureOps
	{
		namespace
		{
			using Pass = detail::TextureProcessingPass;
			using Kernel = detail::TextureProcessingKernel;
			constexpr size_t MaxLevels = 14;

			void ValidateImage(const Texture& texture)
			{
				if (not texture)
				{
					throw Error{ U"RenderTextureOps: empty texture" };
				}

				const auto format = texture.format().value();
				if ((format < TexturePixelFormat::R8_Unorm) || (TexturePixelFormat::R32G32B32A32_Float < format))
				{
					throw Error{ U"RenderTextureOps: an uncompressed color format is required" };
				}
			}

			void ValidateRect(const Rect& rect, const Size size)
			{
				if ((rect.x < 0) || (rect.y < 0) || (rect.w < 0) || (rect.h < 0)
					|| (rect.x > size.x) || (rect.y > size.y) || (rect.w > (size.x - rect.x)) || (rect.h > (size.y - rect.y)))
				{
					throw Error{ U"RenderTextureOps: rectangle is outside the texture" };
				}
			}

			bool ValidateRegions(const Texture& source, const Rect& sourceRect, const RenderTexture& destination, const Rect& destinationRect)
			{
				ValidateImage(source);
				ValidateImage(destination);
				ValidateRect(sourceRect, source.size());
				ValidateRect(destinationRect, destination.size());
				const bool empty = ((destinationRect.w == 0) || (destinationRect.h == 0));
				if (not empty && ((sourceRect.w == 0) || (sourceRect.h == 0)))
				{
					throw Error{ U"RenderTextureOps: nonempty output requires nonempty input" };
				}

				return empty;
			}

			Rect GetSourceRect(const TextureRegion& source)
			{
				ValidateImage(source.texture);
				const Size size = source.texture.size();
				const auto coordinate = [](const float uv, const int32 extent)
				{
					const double value = (static_cast<double>(uv) * extent);
					const double rounded = std::round(value);
					// At the maximum texture extent this also accepts normalized float UV roundoff.
					constexpr double Tolerance = (1.0 / 512.0);
					if (not std::isfinite(value) || (std::abs(value - rounded) > Tolerance)
						|| (rounded < 0) || (extent < rounded))
					{
						throw Error{ U"RenderTextureOps: TextureRegion must have integer pixel boundaries (within 1/512 pixel)" };
					}

					return static_cast<int32>(rounded);
				};
				const int32 x = coordinate(source.uvRect.left, size.x);
				const int32 y = coordinate(source.uvRect.top, size.y);
				const int32 right = coordinate(source.uvRect.right, size.x);
				const int32 bottom = coordinate(source.uvRect.bottom, size.y);
				if ((right < x) || (bottom < y))
				{
					throw Error{ U"RenderTextureOps: flipped TextureRegion is not supported" };
				}

				return Rect{ x, y, (right - x), (bottom - y) };
			}

			void ValidateSameSize(const Rect& sourceRect, const Rect& destinationRect)
			{
				if (sourceRect.size != destinationRect.size)
				{
					throw Error{ U"RenderTextureOps: input and output regions must have the same size" };
				}
			}

			void ValidateAmount(const double value, const double maximum)
			{
				if (not std::isfinite(value) || (value < 0) || (maximum < value))
				{
					throw Error{ U"RenderTextureOps: blur amount is outside the supported range" };
				}
			}

			bool IsFloat32(const TextureFormat format)
			{
				return (format == TextureFormat::R32_Float) || (format == TextureFormat::R32G32_Float)
					|| (format == TextureFormat::R32G32B32A32_Float);
			}

			TextureFormat GetIntermediateFormat(const Texture& source, const RenderTexture& destination)
			{
				return ((IsFloat32(source.format()) || IsFloat32(destination.format()))
					? TextureFormat::R32G32B32A32_Float : TextureFormat::R16G16B16A16_Float);
			}

			void EnsureRenderTexture(RenderTexture& texture, const Size required, const TextureFormat format)
			{
				Size capacity = required;
				if (texture && (texture.format() == format))
				{
					const Size old = texture.size();
					if ((required.x <= old.x) && (required.y <= old.y))
					{
						return;
					}
					capacity = Size{ (required.x <= old.x) ? old.x : std::min(16384, required.x + required.x / 4),
						(required.y <= old.y) ? old.y : std::min(16384, required.y + required.y / 4) };
					// Do not combine a previous very wide texture with a new very tall one.
					if ((int64{ capacity.x } * capacity.y) > 2 * std::max(int64{ old.x } * old.y, int64{ required.x } * required.y))
					{
						capacity = required;
					}
				}

				RenderTexture next{ capacity, format };
				if (not next)
				{
					throw Error{ U"RenderTextureOps: failed to allocate a working texture" };
				}
				texture = std::move(next);
			}

			struct Plan
			{
				std::array<Pass, detail::MaxProcessingPasses> passes;
				size_t count = 0;

				Pass& add(const Kernel kernel, const Texture& source, const Rect& sourceRect,
					const RenderTexture& destination, const Rect& destinationRect, const bool preserve = false)
				{
					assert(count < passes.size());
					auto& pass = passes[count++];
					pass.kernel = kernel;
					pass.source = source.id();
					pass.sourceRect = sourceRect;
					pass.destination = destination.id();
					pass.destinationRect = destinationRect;
					pass.preserveDestination = preserve;
					return pass;
				}

				void submit() const
				{
					if (count)
					{
						SIV3D_ENGINE(Texture)->processRenderTextures(std::span{ passes.data(), count });
					}
				}
			};

			void AddDownsample(Plan& plan, const Texture& source, const Rect& sourceRect,
				const RenderTexture& destination, const Rect& destinationRect, const bool preserve)
			{
				// Exact 1x/2x reductions are a single bilinear box sample.
				const bool simple = ((sourceRect.w == destinationRect.w) || (sourceRect.w == 2 * destinationRect.w))
					&& ((sourceRect.h == destinationRect.h) || (sourceRect.h == 2 * destinationRect.h));
				auto& pass = plan.add(simple ? Kernel::Resample : Kernel::Downsample, source, sourceRect, destination, destinationRect, preserve);
				pass.step = Float2{ (sourceRect.w == destinationRect.w) ? 0.0f : (0.25f * sourceRect.w / destinationRect.w),
					(sourceRect.h == destinationRect.h) ? 0.0f : (0.25f * sourceRect.h / destinationRect.h) };
			}
		}

		class Workspace::WorkspaceDetail
		{
		public:

			RenderTexture gaussian;
			RenderTexture alias;
			RenderTexture transition;
			std::array<RenderTexture, MaxLevels> downsample;
			std::array<RenderTexture, MaxLevels> dual;
			std::array<Float4, detail::MaxGaussianSamples> gaussianSamples{};
			size_t sampleCount = 0;
			double previousSigma = -1;

			void prepareGaussian(const double sigma)
			{
				if (sigma == previousSigma)
				{
					return;
				}

				const int32 radius = static_cast<int32>(std::ceil(3 * sigma));
				std::array<double, 97> weights{};
				weights[0] = 1;
				double sum = 1;
				for (int32 i = 1; i <= radius; ++i)
				{
					const double x = (i / sigma);
					weights[i] = std::exp(-0.5 * x * x);
					sum += (2 * weights[i]);
				}
				gaussianSamples[0] = Float4{ static_cast<float>(1 / sum), 0, 0, 0 };
				sampleCount = 1;
				for (int32 i = 1; i <= radius; i += 2)
				{
					const double next = ((i < radius) ? weights[i + 1] : 0);
					const double pair = (weights[i] + next);
					if (pair == 0)
					{
						continue;
					}
					gaussianSamples[sampleCount++] = Float4{ static_cast<float>(pair / sum),
						static_cast<float>(i + next / pair), 0, 0 };
				}
				previousSigma = sigma;
			}
		};
	}

	namespace detail
	{
		struct RenderTextureOpsAccess
		{
			static auto& Get(RenderTextureOps::Workspace& work)
			{
				if (not work.pImpl)
				{
					work.pImpl = std::make_unique<RenderTextureOps::Workspace::WorkspaceDetail>();
				}

				return *work.pImpl;
			}
		};
	}

	namespace RenderTextureOps
	{
		namespace
		{
			void CopyOrConvert(const Texture& source, const Rect& sourceRect, const RenderTexture& destination,
				const Rect& destinationRect, Workspace& work)
			{
				Plan plan;
				const bool same = (source.id() == destination.id());
				if (same && (sourceRect == destinationRect))
				{
					return;
				}

				const Kernel kernel = ((source.format() == destination.format()) ? Kernel::Copy : Kernel::Resample);
				if (same)
				{
					auto& temp = detail::RenderTextureOpsAccess::Get(work).alias;
					EnsureRenderTexture(temp, sourceRect.size, destination.format());
					plan.add(kernel, source, sourceRect, temp, Rect{ sourceRect.size });
					plan.add(Kernel::Copy, temp, Rect{ sourceRect.size }, destination, destinationRect, true);
				}
				else
				{
					// A same-size format conversion needs one texel, not linear filtering.
					auto& pass = plan.add(kernel, source, sourceRect, destination, destinationRect, true);
					pass.filter = TextureFilter::Nearest;
				}

				plan.submit();
			}
		}

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		Workspace::Workspace() = default;

		Workspace::Workspace(Workspace&& other) noexcept = default;

		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		Workspace::~Workspace() = default;

		////////////////////////////////////////////////////////////////
		//
		//	operator =
		//
		////////////////////////////////////////////////////////////////

		Workspace& Workspace::operator =(Workspace&& other) noexcept = default;

		////////////////////////////////////////////////////////////////
		//
		//	release
		//
		////////////////////////////////////////////////////////////////

		void Workspace::release() noexcept
		{
			pImpl.reset();
		}

		////////////////////////////////////////////////////////////////
		//
		//	Copy
		//
		////////////////////////////////////////////////////////////////

		void Copy(const Texture& source, const RenderTexture& destination)
		{
			Copy(source, Rect{ source.size() }, destination, Rect{ destination.size() });
		}

		void Copy(const Texture& source, const Rect& sourceRect, const RenderTexture& destination, const Rect& destinationRect)
		{
			const bool empty = ValidateRegions(source, sourceRect, destination, destinationRect);
			ValidateSameSize(sourceRect, destinationRect);
			if (source.format() != destination.format())
			{
				throw Error{ U"RenderTextureOps::Copy: formats must match" };
			}

			if (source.id() == destination.id())
			{
				if (sourceRect == destinationRect)
				{
					return;
				}
				throw Error{ U"RenderTextureOps::Copy: different regions of the same image are not supported" };
			}

			if (empty)
			{
				return;
			}

			Plan plan;
			plan.add(Kernel::Copy, source, sourceRect, destination, destinationRect, true);
			plan.submit();
		}

		////////////////////////////////////////////////////////////////
		//
		//	Resample
		//
		////////////////////////////////////////////////////////////////

		void Resample(const Texture& source, const RenderTexture& destination, const TextureFilter filter)
		{
			Resample(source, Rect{ source.size() }, destination, Rect{ destination.size() }, filter);
		}

		void Resample(const TextureRegion& source, const RenderTexture& destination, const Rect& destinationRect, const TextureFilter filter)
		{
			Resample(source.texture, GetSourceRect(source), destination, destinationRect, filter);
		}

		void Resample(const Texture& source, const Rect& sourceRect, const RenderTexture& destination, const Rect& destinationRect, const TextureFilter filter)
		{
			const bool empty = ValidateRegions(source, sourceRect, destination, destinationRect);
			if ((filter != TextureFilter::Nearest) && (filter != TextureFilter::Linear))
			{
				throw Error{ U"RenderTextureOps::Resample: invalid filter" };
			}

			if (source.id() == destination.id())
			{
				throw Error{ U"RenderTextureOps::Resample: input and output must be different images" };
			}

			if (empty)
			{
				return;
			}

			Plan plan;
			auto& pass = plan.add(Kernel::Resample, source, sourceRect, destination, destinationRect, true);
			pass.filter = filter;
			plan.submit();
		}

		////////////////////////////////////////////////////////////////
		//
		//	Downsample
		//
		////////////////////////////////////////////////////////////////

		void Downsample(const Texture& source, const RenderTexture& destination, Workspace& work)
		{
			Downsample(source, Rect{ source.size() }, destination, Rect{ destination.size() }, work);
		}

		void Downsample(const TextureRegion& source, const RenderTexture& destination, const Rect& destinationRect, Workspace& work)
		{
			Downsample(source.texture, GetSourceRect(source), destination, destinationRect, work);
		}

		void Downsample(const Texture& source, const Rect& sourceRect, const RenderTexture& destination, const Rect& destinationRect, Workspace& work)
		{
			const bool empty = ValidateRegions(source, sourceRect, destination, destinationRect);
			if ((sourceRect.w < destinationRect.w) || (sourceRect.h < destinationRect.h))
			{
				throw Error{ U"RenderTextureOps::Downsample: output must not be larger than input" };
			}

			if (empty)
			{
				return;
			}

			if (sourceRect.size == destinationRect.size)
			{
				CopyOrConvert(source, sourceRect, destination, destinationRect, work);
				return;
			}

			auto& state = detail::RenderTextureOpsAccess::Get(work);
			const TextureFormat format = GetIntermediateFormat(source, destination);
			Plan plan;
			const Texture* current = &source;
			Rect region = sourceRect;
			size_t level = 0;
			while ((region.w > 2 * destinationRect.w) || (region.h > 2 * destinationRect.h))
			{
				const Size size{ std::max(destinationRect.w, (region.w + 1) / 2), std::max(destinationRect.h, (region.h + 1) / 2) };
				auto& next = state.downsample[level++];
				EnsureRenderTexture(next, size, format);
				AddDownsample(plan, *current, region, next, Rect{ size }, false);
				current = &next;
				region = Rect{ size };
			}

			if (current->id() == destination.id())
			{
				EnsureRenderTexture(state.alias, destinationRect.size, destination.format());
				AddDownsample(plan, *current, region, state.alias, Rect{ destinationRect.size }, false);
				plan.add(Kernel::Copy, state.alias, Rect{ destinationRect.size }, destination, destinationRect, true);
			}
			else
			{
				AddDownsample(plan, *current, region, destination, destinationRect, true);
			}

			plan.submit();
		}

		////////////////////////////////////////////////////////////////
		//
		//	GaussianBlur
		//
		////////////////////////////////////////////////////////////////

		void GaussianBlur(const Texture& source, const RenderTexture& destination, const double sigma, Workspace& work)
		{
			GaussianBlur(source, Rect{ source.size() }, destination, Rect{ destination.size() }, sigma, work);
		}

		void GaussianBlur(const TextureRegion& source, const RenderTexture& destination, const Rect& destinationRect, const double sigma, Workspace& work)
		{
			GaussianBlur(source.texture, GetSourceRect(source), destination, destinationRect, sigma, work);
		}

		void GaussianBlur(const Texture& source, const Rect& sourceRect, const RenderTexture& destination, const Rect& destinationRect, const double sigma, Workspace& work)
		{
			const bool empty = ValidateRegions(source, sourceRect, destination, destinationRect);
			ValidateSameSize(sourceRect, destinationRect);
			ValidateAmount(sigma, 32);
			if (empty)
			{
				return;
			}

			if (sigma == 0)
			{
				CopyOrConvert(source, sourceRect, destination, destinationRect, work);
				return;
			}

			auto& state = detail::RenderTextureOpsAccess::Get(work);
			EnsureRenderTexture(state.gaussian, sourceRect.size, GetIntermediateFormat(source, destination));
			state.prepareGaussian(sigma);
			Plan plan;
			auto& horizontal = plan.add(Kernel::Gaussian, source, sourceRect, state.gaussian, Rect{ sourceRect.size });
			horizontal.step = Float2{ 1, 0 };
			horizontal.samples = std::span{ state.gaussianSamples.data(), state.sampleCount };
			auto& vertical = plan.add(Kernel::Gaussian, state.gaussian, Rect{ sourceRect.size }, destination, destinationRect, true);
			vertical.step = Float2{ 0, 1 };
			vertical.samples = horizontal.samples;
			plan.submit();
		}

		////////////////////////////////////////////////////////////////
		//
		//	DualKawaseBlur
		//
		////////////////////////////////////////////////////////////////

		void DualKawaseBlur(const Texture& source, const RenderTexture& destination, const double radius, Workspace& work)
		{
			DualKawaseBlur(source, Rect{ source.size() }, destination, Rect{ destination.size() }, radius, work);
		}

		void DualKawaseBlur(const TextureRegion& source, const RenderTexture& destination, const Rect& destinationRect, const double radius, Workspace& work)
		{
			DualKawaseBlur(source.texture, GetSourceRect(source), destination, destinationRect, radius, work);
		}

		void DualKawaseBlur(const Texture& source, const Rect& sourceRect, const RenderTexture& destination, const Rect& destinationRect, const double radius, Workspace& work)
		{
			const bool empty = ValidateRegions(source, sourceRect, destination, destinationRect);
			ValidateSameSize(sourceRect, destinationRect);
			ValidateAmount(radius, 256);
			if (empty)
			{
				return;
			}

			if ((radius == 0) || (sourceRect.size == Size{ 1, 1 }))
			{
				CopyOrConvert(source, sourceRect, destination, destinationRect, work);
				return;
			}

			auto& state = detail::RenderTextureOpsAccess::Get(work);
			const TextureFormat format = GetIntermediateFormat(source, destination);
			std::array<Size, MaxLevels + 1> sizes;
			sizes[0] = sourceRect.size;
			int32 maximum = 0;
			while (sizes[maximum] != Size{ 1, 1 })
			{
				const Size size = sizes[maximum];
				sizes[++maximum] = Size{ (size.x + 1) / 2, (size.y + 1) / 2 };
			}

			const double strength = std::log2(radius + 1);
			const int32 whole = static_cast<int32>(std::floor(strength));
			const float fraction = static_cast<float>(strength - whole);
			const bool transition = ((fraction > 0) && (whole < maximum));
			const int32 depth = std::min(maximum, whole + static_cast<int32>(transition));
			if (depth == 0)
			{
				CopyOrConvert(source, sourceRect, destination, destinationRect, work);
				return;
			}

			Plan plan;
			const Texture* current = &source;
			Rect region = sourceRect;
			for (int32 level = 1; level <= depth; ++level)
			{
				auto& next = state.dual[level - 1];
				EnsureRenderTexture(next, sizes[level], format);
				// Area-aware reduction avoids accumulating phase-dependent bright-point gain
				// when odd dimensions keep aligning a pixel with the center of every level.
				AddDownsample(plan, *current, region, next, Rect{ sizes[level] }, false);
				current = &next;
				region = Rect{ sizes[level] };
			}

			for (int32 level = depth - 1; level >= 0; --level)
			{
				const bool blend = (transition && (level == depth - 1));
				const Texture* secondary = blend ? ((level == 0) ? &source : &state.dual[level - 1]) : nullptr;
				const RenderTexture* output = (level == 0) ? &destination : &state.dual[level - 1];
				Rect outputRect = (level == 0) ? destinationRect : Rect{ sizes[level] };
				if (blend && (level != 0))
				{
					EnsureRenderTexture(state.transition, sizes[level], format);
					output = &state.transition;
				}

				const bool alias = (level == 0) && secondary && (secondary->id() == destination.id());
				if (alias)
				{
					EnsureRenderTexture(state.alias, destinationRect.size, destination.format());
					output = &state.alias;
					outputRect = Rect{ destinationRect.size };
				}

				auto& pass = plan.add(blend ? Kernel::DualBlend : Kernel::DualUp, *current, region,
					*output, outputRect, (level == 0) && not alias);
				pass.step = Float2{ 1, 1 };
				if (blend)
				{
					pass.secondary = secondary->id();
					pass.secondaryRect = (level == 0) ? sourceRect : Rect{ sizes[level] };
					pass.blend = fraction;
				}

				if (alias)
				{
					plan.add(Kernel::Copy, state.alias, Rect{ destinationRect.size }, destination, destinationRect, true);
				}
				current = output;
				region = outputRect;
			}

			plan.submit();
		}
	}
}
