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
# include <Siv3D/Common.hpp>
# include <Siv3D/HashTable.hpp>
# include <Siv3D/Renderer/Metal/Metal.hpp>
# include "PipelineStateDesc.hpp"

namespace s3d
{
	class CShader_Metal;

	class MetalRenderPipelineState
	{
	public:

		void init(MTL::Device* device, CShader_Metal* pShader);
		
		const MTL::RenderPipelineState* get(const PipelineStateDesc& desc);

	private:
	
		MTL::Device* m_device			= nullptr;

		CShader_Metal* m_pShader		= nullptr;

		HashTable<PipelineStateDesc, NS::SharedPtr<MTL::RenderPipelineState>> m_pipelineStates;
	};
}
