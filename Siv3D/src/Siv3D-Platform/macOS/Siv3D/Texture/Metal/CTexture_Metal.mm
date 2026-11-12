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

# include "CTexture_Metal.hpp"
# include <Siv3D/ImageProcessing.hpp>
# include <Siv3D/Renderer/Metal/CRenderer_Metal.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/ImageFormat/BCnDecoder.hpp>
# include <Siv3D/Texture/TextureUtility.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <cstring>
# include <Siv3D/RenderTextureOps/TextureProcessing.hpp>
# include <Siv3D/ScopeExit.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	CTexture_Metal::CTexture_Metal() {}

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	CTexture_Metal::~CTexture_Metal()
	{
		LOG_SCOPED_DEBUG("CTexture_Metal::~CTexture_Metal()");

		m_textures.destroy();
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void CTexture_Metal::init()
	{
		LOG_SCOPED_DEBUG("CTexture_Metal::init()");

		m_pRenderer		= static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
		m_device		= m_pRenderer->getDevice();
		m_commandQueue	= m_pRenderer->getCommandQueue();
		m_float32Filtering = m_device->supports32BitFloatFiltering();

		// null Texture を管理に登録
		{
			const Image image{ 16, Palette::Yellow };
			const Array<Image> mipmaps = {
				Image{ 8, Palette::Yellow }, Image{ 4, Palette::Yellow },
				Image{ 2, Palette::Yellow }, Image{ 1, Palette::Yellow }
			};

			// null Texture を作成
			auto nullTexture = std::make_unique<MetalTexture>(m_device, m_commandQueue, image, mipmaps, TextureDesc::Mipmap);

			if (not nullTexture->isInitialized()) // もし作成に失敗していたら
			{
				throw InternalEngineError{ "Failed to create a null texture" };
			}

			// 管理に登録
			m_textures.setNullData(std::move(nullTexture));
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	create
	//
	////////////////////////////////////////////////////////////////

	Texture::IDType CTexture_Metal::create(std::unique_ptr<IReader> reader, FilePathView pathHint, const TextureDesc desc)
	{
		if (not reader)
		{
			return Texture::IDType::Null();
		}

		if (not reader->isOpen())
		{
			return Texture::IDType::Null();
		}

		const BCnDecoder bcnDecoder{};
		const bool isBCn = bcnDecoder.getImageInfo(*reader).has_value();

		if (isBCn)
		{
			return create(bcnDecoder.decodeNative(std::move(reader), desc.sRGB, pathHint));
		}
		else
		{
			const Image image{ std::move(reader) };
			return create(image.size(), std::as_bytes(std::span{ image }), (desc.sRGB ? TextureFormat::R8G8B8A8_Unorm_SRGB : TextureFormat::R8G8B8A8_Unorm), desc);
		}
	}

	Texture::IDType CTexture_Metal::create(const Image& image, const std::span<const Image> mipmaps, const TextureDesc desc)
	{
		if (not image)
		{
			return Texture::IDType::Null();
		}

		auto texture = std::make_unique<MetalTexture>(m_device, m_commandQueue, image, mipmaps, desc);

		if (not texture->isInitialized())
		{
			return Texture::IDType::Null();
		}

		const String info = texture->getDesc().toString();
		return m_textures.add(std::move(texture), info);
	}

	Texture::IDType CTexture_Metal::create(const Size& size, const std::span<const Byte> data, const TextureFormat& format, const TextureDesc desc)
	{
		if ((size.x <= 0) || (size.y <= 0))
		{
			return Texture::IDType::Null();
		}
		
		auto texture = std::make_unique<MetalTexture>(m_device, m_commandQueue, size, data, format, desc);

		if (not texture->isInitialized())
		{
			return Texture::IDType::Null();
		}

		const String info = texture->getDesc().toString();
		return m_textures.add(std::move(texture), info);
	}

	Texture::IDType CTexture_Metal::create(const BCnData& bcnData)
	{
		const auto mipmaps = GetValidBCnMipmaps(bcnData);
		if (mipmaps.empty())
		{
			return Texture::IDType::Null();
		}
		
		std::unique_ptr<MetalTexture> texture = std::make_unique<MetalTexture>(m_device, m_commandQueue, bcnData.size, bcnData.format, mipmaps);
		
		if (not texture->isInitialized())
		{
			return Texture::IDType::Null();
		}

		const String info = texture->getDesc().toString();
		return m_textures.add(std::move(texture), info);
	}

	Texture::IDType CTexture_Metal::createDynamic(const Size& size, const std::span<const Byte> data, const TextureFormat& format, const TextureDesc desc)
	{
		if ((size.x <= 0) || (size.y <= 0))
		{
			return Texture::IDType::Null();
		}
		
		auto texture = std::make_unique<MetalTexture>(MetalTexture::Dynamic{}, m_device, m_commandQueue, size, data, format, desc);

		if (not texture->isInitialized())
		{
			return Texture::IDType::Null();
		}
			
		const String info = texture->getDesc().toString();
		return m_textures.add(std::move(texture), info);
	}

	////////////////////////////////////////////////////////////////
	//
	//	RenderTexture
	//
	////////////////////////////////////////////////////////////////

	MTL::CommandBuffer* CTexture_Metal::renderCommands(const bool flushDraws)
	{
		if (auto* commands = m_pRenderer->getFrameContext().getCommandBuffer())
		{
			if (flushDraws)
			{
				SIV3D_ENGINE(Renderer2D)->flush();
			}
			return commands;
		}
		auto* commands = m_commandQueue->commandBuffer();
		if (not commands)
		{
			throw InternalEngineError{ "RenderTexture: commandBuffer() failed" };
		}
		return commands;
	}

	Texture::IDType CTexture_Metal::createRender(const Size& size, const std::span<const Byte> data,
		const TextureFormat& format, TextureDesc desc, const ColorF& color)
	{
		if (not IsValidRenderTexture(size, data, format))
		{
			return Texture::IDType::Null();
		}
		desc.sRGB = format.isSRGB();
		const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		auto* commands = renderCommands(false);
		auto texture = std::make_unique<MetalTexture>(MetalTexture::Render{}, m_device, commands, size, data, format, desc, color);
		if (not m_pRenderer->getFrameContext().getCommandBuffer())
		{
			commands->commit();
		}
		if (not texture->isInitialized())
		{
			return Texture::IDType::Null();
		}
		const String info = texture->getDesc().toString();
		return m_textures.add(std::move(texture), info);
	}

	Texture::IDType CTexture_Metal::createMSRender(const Size& size, const TextureFormat& format,
		TextureDesc desc, const ColorF& color, const uint32 sampleCount)
	{
		if (((sampleCount != 2) && (sampleCount != 4)) || not IsValidRenderTexture(size, {}, format))
		{
			LOG_FAIL("MSRenderTexture: Invalid size, format, or sample count");
			return Texture::IDType::Null();
		}
		desc.sRGB = format.isSRGB();
		const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		auto* commands = renderCommands(false);
		auto texture = std::make_unique<MetalTexture>(MetalTexture::Render{}, m_device, commands, size, std::span<const Byte>{}, format, desc, color, sampleCount);
		if (not m_pRenderer->getFrameContext().getCommandBuffer())
		{
			commands->commit();
		}
		if (not texture->isInitialized())
		{
			LOG_FAIL(fmt::format("MSRenderTexture: Unsupported settings or allocation failure (size = {}x{}, format = {}, samples = {}, mipmaps = {})",
				size.x, size.y, format.name().toUTF8(), sampleCount, desc.hasMipmap));
			return Texture::IDType::Null();
		}
		const String info = texture->getDesc().toString();
		return m_textures.add(std::move(texture), info);
	}

	////////////////////////////////////////////////////////////////
	//
	//	processRenderTextures
	//
	////////////////////////////////////////////////////////////////

	void CTexture_Metal::processRenderTextures(const std::span<const detail::TextureProcessingPass> passes)
	{
		using Kernel = detail::TextureProcessingKernel;
		const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		std::array<detail::TextureProcessingBinding, detail::MaxProcessingPasses> bindings;
		std::array<const MTL::RenderPipelineState*, detail::MaxProcessingPasses> pipelines{};
		auto* shaders = SIV3D_ENGINE(EngineShader);
		const auto validateFiltering = [this](const Texture::IDType id)
		{
			const auto format = getFormat(id);
			if (not m_float32Filtering && format.isFloat32())
			{
				throw Error{ U"RenderTextureOps: this device cannot linearly filter 32-bit float textures" };
			}
		};
		// Prepare every pipeline before the first destination write.
		for (size_t i = 0; i < passes.size(); ++i)
		{
			const auto& pass = passes[i];
			if (pass.kernel == Kernel::Copy)
			{
				continue;
			}
			if (pass.filter == TextureFilter::Linear)
			{
				validateFiltering(pass.source);
				if (pass.secondary != Texture::IDType::Null())
				{
					validateFiltering(pass.secondary);
				}
			}
			bindings[i] = detail::MakeTextureProcessingBinding(pass, getSize(pass.source),
				(pass.secondary == Texture::IDType::Null()) ? Size{ 1, 1 } : getSize(pass.secondary));
			PipelineStateDesc desc{
				.vs = shaders->getVS(EngineVS::TextureProcessing).id(),
				.ps = shaders->getPS(bindings[i].shader).id(),
				.pixelFormats = { static_cast<uint16>(getTexture(pass.destination)->pixelFormat()) },
				.blendState = BlendState::Opaque,
			};
			pipelines[i] = m_pRenderer->getRenderPipelineState().get(desc);
		}
		auto* commands = renderCommands(true);
		const bool standalone = not m_pRenderer->getFrameContext().getCommandBuffer();
		const ScopeExit commit{ [&]
		{
			if (standalone)
			{
				commands->commit();
			}
		} };
		for (size_t i = 0; i < passes.size(); ++i)
		{
			const auto& pass = passes[i];
			if (pass.kernel == Kernel::Copy)
			{
				auto* encoder = commands->blitCommandEncoder();
				if (not encoder)
				{
					throw InternalEngineError{ "RenderTextureOps: blit encoder creation failed" };
				}
				const ScopeExit finish{ [encoder]
				{
					encoder->endEncoding();
				} };
				encoder->copyFromTexture(getTexture(pass.source), 0, 0,
					MTL::Origin{ static_cast<NS::UInteger>(pass.sourceRect.x), static_cast<NS::UInteger>(pass.sourceRect.y), 0 },
					MTL::Size{ static_cast<NS::UInteger>(pass.sourceRect.w), static_cast<NS::UInteger>(pass.sourceRect.h), 1 },
					getTexture(pass.destination), 0, 0,
					MTL::Origin{ static_cast<NS::UInteger>(pass.destinationRect.x), static_cast<NS::UInteger>(pass.destinationRect.y), 0 });
				continue;
			}
			const auto descriptor = NS::TransferPtr(MTL::RenderPassDescriptor::alloc()->init());
			auto* attachment = descriptor->colorAttachments()->object(0);
			attachment->setTexture(getTexture(pass.destination));
			attachment->setLoadAction((pass.preserveDestination && (pass.destinationRect != Rect{ getSize(pass.destination) }))
				? MTL::LoadActionLoad : MTL::LoadActionDontCare);
			attachment->setStoreAction(MTL::StoreActionStore);
			auto* encoder = commands->renderCommandEncoder(descriptor.get());
			if (not encoder)
			{
				throw InternalEngineError{ "RenderTextureOps: render encoder creation failed" };
			}
			const ScopeExit finish{ [encoder]
			{
				encoder->endEncoding();
			} };
			encoder->setRenderPipelineState(pipelines[i]);
			encoder->setViewport(MTL::Viewport{ static_cast<double>(pass.destinationRect.x), static_cast<double>(pass.destinationRect.y),
				static_cast<double>(pass.destinationRect.w), static_cast<double>(pass.destinationRect.h), 0.0, 1.0 });
			encoder->setScissorRect(MTL::ScissorRect{ static_cast<NS::UInteger>(pass.destinationRect.x), static_cast<NS::UInteger>(pass.destinationRect.y),
				static_cast<NS::UInteger>(pass.destinationRect.w), static_cast<NS::UInteger>(pass.destinationRect.h) });
			m_processingConstants = bindings[i].constants;
			encoder->setVertexBytes(m_processingConstants._data(), m_processingConstants.activeSize(), 0);
			encoder->setFragmentBytes(m_processingConstants._data(), m_processingConstants.activeSize(), 0);
			if (pass.kernel == Kernel::Gaussian)
			{
				std::copy(pass.samples.begin(), pass.samples.end(), m_gaussianSamples->begin());
				m_gaussianSamples.setActiveSize(pass.samples.size_bytes());
				encoder->setFragmentBytes(m_gaussianSamples._data(), m_gaussianSamples.activeSize(), 1);
			}
			encoder->setFragmentTexture(getTexture(pass.source), 0);
			if (pass.secondary != Texture::IDType::Null())
			{
				encoder->setFragmentTexture(getTexture(pass.secondary), 1);
			}
			auto& samplers = m_pRenderer->getSamplerState();
			samplers.resetStates();
			samplers.setPS(encoder, 0, (pass.filter == TextureFilter::Nearest) ? SamplerState::ClampNearest : SamplerState::ClampLinear);
			encoder->drawPrimitives(MTL::PrimitiveTypeTriangle, NS::UInteger{ 0 }, NS::UInteger{ 3 });
		}
	}

	void CTexture_Metal::resolveMSRT(const Texture::IDType handleID)
	{
		auto* texture = m_textures[handleID];
		if (texture->getDesc().type != TextureType::MSRender)
		{
			return;
		}
		const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		auto* commands = renderCommands(false);
		texture->resolveMSRT(commands);
		if (not m_pRenderer->getFrameContext().getCommandBuffer())
		{
			commands->commit();
		}
	}

	uint32 CTexture_Metal::getSampleCount(const Texture::IDType handleID)
	{
		return m_textures[handleID]->getDesc().multiSampleCount;
	}

	void CTexture_Metal::clearRT(const Texture::IDType handleID, const ColorF& color)
	{
		const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		auto* commands = renderCommands(true);
		m_textures[handleID]->clearRT(commands, color);
		if (not m_pRenderer->getFrameContext().getCommandBuffer())
		{
			commands->commit();
		}
	}

	bool CTexture_Metal::readRT(const Texture::IDType handleID, const std::span<Byte> destination, const uint32 mipLevel)
	{
		auto* texture = m_textures[handleID];
		const auto& desc = texture->getDesc();
		if (((desc.type != TextureType::Render) && (desc.type != TextureType::MSRender)) || (desc.mipLevels <= mipLevel))
		{
			return false;
		}
		const Size levelSize = ImageProcessing::CalculateMipmapSize(desc.size, mipLevel);
		const uint32 width = levelSize.x;
		const uint32 height = levelSize.y;
		const uint32 rowBytes = static_cast<uint32>(desc.format.packedRowBytes(width));
		if (destination.size_bytes() != (static_cast<size_t>(rowBytes) * height))
		{
			return false;
		}
		const uint32 rowPitch = ((rowBytes + 255) & ~255u);
		const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		auto* commands = renderCommands(true);
		auto* buffer = texture->encodeReadRT(m_device, commands, mipLevel, rowPitch);
		if (not buffer)
		{
			return false;
		}
		if (m_pRenderer->getFrameContext().getCommandBuffer())
		{
			if (not m_pRenderer->finishTextureReadback())
			{
				return false;
			}
		}
		else
		{
			commands->commit();
			commands->waitUntilCompleted();
			if (commands->status() != MTL::CommandBufferStatusCompleted)
			{
				return false;
			}
		}
		const auto* source = static_cast<const Byte*>(buffer->contents());
		for (uint32 y = 0; y < height; ++y)
		{
			std::memcpy((destination.data() + static_cast<size_t>(y) * rowBytes), (source + static_cast<size_t>(y) * rowPitch), rowBytes);
		}
		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	release
	//
	////////////////////////////////////////////////////////////////

	void CTexture_Metal::release(const Texture::IDType handleID)
	{
		m_textures.erase(handleID);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getSize
	//
	////////////////////////////////////////////////////////////////

	Size CTexture_Metal::getSize(const Texture::IDType handleID)
	{
		return m_textures[handleID]->getDesc().size;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getMipLevels
	//
	////////////////////////////////////////////////////////////////

	uint32 CTexture_Metal::getMipLevels(const Texture::IDType handleID)
	{
		return m_textures[handleID]->getDesc().mipLevels;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getDesc
	//
	////////////////////////////////////////////////////////////////

	TextureDesc CTexture_Metal::getDesc(const Texture::IDType handleID)
	{
		const auto& desc = m_textures[handleID]->getDesc();
		return{ desc.hasMipmap, desc.sRGB, desc.isSDF };
	}

	////////////////////////////////////////////////////////////////
	//
	//	getFormat
	//
	////////////////////////////////////////////////////////////////

	TextureFormat CTexture_Metal::getFormat(const Texture::IDType handleID)
	{
		return m_textures[handleID]->getDesc().format;
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasDepth
	//
	////////////////////////////////////////////////////////////////

	bool CTexture_Metal::hasDepth(const Texture::IDType handleID)
	{
		return m_textures[handleID]->hasDepth();
	}

	////////////////////////////////////////////////////////////////
	//
	//	fill
	//
	////////////////////////////////////////////////////////////////

	bool CTexture_Metal::fill(const Texture::IDType handleID, const ColorF& color, const bool wait)
	{
		return m_textures[handleID]->fill(m_commandQueue, m_pRenderer->getFrameContext(), color, wait);
	}

	bool CTexture_Metal::fill(const Texture::IDType handleID, const std::span<const Byte> src, const uint32 srcBytesPerRow, const bool wait)
	{
		return m_textures[handleID]->fill(m_commandQueue, m_pRenderer->getFrameContext(), src, srcBytesPerRow, wait);
	}

	////////////////////////////////////////////////////////////////
	//
	//	fillRegion
	//
	////////////////////////////////////////////////////////////////

	bool CTexture_Metal::fillRegion(const Texture::IDType handleID, const ColorF& color, const Rect& rect, const bool wait)
	{
		return m_textures[handleID]->fillRegion(m_commandQueue, m_pRenderer->getFrameContext(), color, rect, wait);
	}

	bool CTexture_Metal::fillRegion(const Texture::IDType handleID, const std::span<const Byte> src, const uint32 srcBytesPerRow, const Rect& rect, const bool wait)
	{
		return m_textures[handleID]->fillRegion(m_commandQueue, m_pRenderer->getFrameContext(), src, srcBytesPerRow, rect, wait);
	}

	////////////////////////////////////////////////////////////////
	//
	//	generateMips
	//
	////////////////////////////////////////////////////////////////

	void CTexture_Metal::generateMips(const Texture::IDType handleID)
	{
		auto* texture = m_textures[handleID];
		if (texture->getDesc().mipLevels <= 1)
		{
			return;
		}
		const bool flushDraws = ((texture->getDesc().type == TextureType::Render)
			|| (texture->getDesc().type == TextureType::MSRender));
		const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		auto* commands = renderCommands(flushDraws);
		texture->generateMips(commands);
		if (not m_pRenderer->getFrameContext().getCommandBuffer())
		{
			commands->commit();
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getTexture
	//
	////////////////////////////////////////////////////////////////

	MTL::Texture* CTexture_Metal::getTexture(const Texture::IDType handleID)
	{
		return m_textures[handleID]->getTexture();
	}

	MTL::Texture* CTexture_Metal::getRenderTargetTexture(const Texture::IDType handleID)
	{
		return m_textures[handleID]->getRenderTargetTexture();
	}
}
