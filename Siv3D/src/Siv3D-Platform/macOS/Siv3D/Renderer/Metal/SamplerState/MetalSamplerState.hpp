//-----------------------------------------------
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
# include <Siv3D/SamplerState.hpp>
# include <Siv3D/Graphics.hpp>
# include <Siv3D/HashTable.hpp>
# include "../Metal.hpp"

namespace s3d
{
	class MetalSamplerState
	{
	public:

		void init(MTL::Device* device);
		
		void resetStates();

		void setVS(MTL::RenderCommandEncoder* renderCommandEncoder, uint32 slot, const SamplerState& state);

		void setPS(MTL::RenderCommandEncoder* renderCommandEncoder, uint32 slot, const SamplerState& state);
	
	private:
	
		using StateTable = HashTable<SamplerState, NS::SharedPtr<MTL::SamplerState>>;
		
		MTL::Device* m_device = nullptr;

		StateTable m_states;

		std::array<SamplerState, Graphics::TextureSlotCount> m_currentVSStates;

		std::array<SamplerState, Graphics::TextureSlotCount> m_currentPSStates;

		StateTable::iterator create(const SamplerState& state);
	};
}
