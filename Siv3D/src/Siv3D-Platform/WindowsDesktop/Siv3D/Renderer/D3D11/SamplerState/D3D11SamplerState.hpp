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
# include <Siv3D/HashMap.hpp>
# include "../Device/D3D11Device.hpp"

namespace s3d
{
	class D3D11SamplerState
	{
	public:

		void init(const D3D11Device& device);

		void setVS(uint32 slot, const SamplerState& state);

		void setPS(uint32 slot, const SamplerState& state);
	
	private:

		using StateTable = HashMap<SamplerState, ComPtr<ID3D11SamplerState>>;

		ID3D11Device* m_device = nullptr;

		ID3D11DeviceContext* m_context = nullptr;

		StateTable m_states;

		std::array<SamplerState, Graphics::TextureSlotCount> m_currentVSStates;

		std::array<SamplerState, Graphics::TextureSlotCount> m_currentPSStates;

		StateTable::iterator create(const SamplerState& state);
	};
}
