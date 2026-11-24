//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include "../Siv3D/src/Siv3D/Renderer2D/Renderer2DStateUpdate.hpp"
# include <functional>

# if SIV3D_PLATFORM(WINDOWS)
# include "../Siv3D/src/Siv3D-Platform/WindowsDesktop/Siv3D/Renderer2D/D3D11/D3D11Renderer2DCommandManager.hpp"
# elif SIV3D_PLATFORM(MACOS)
# include "../Siv3D/src/Siv3D-Platform/macOS/Siv3D/Renderer2D/Metal/MetalRenderer2DCommandManager.hpp"
# endif

namespace
{
	enum class TestCommand : uint32
	{
		Value = 1,
		Other = 2,
	};

	struct RecordedValue
	{
		int32 value = 1;
		mutable uint32 reads = 0;

		const int32& back() const
		{
			++reads;
			return value;
		}
	};
}

TEST_CASE("Renderer2D.state_update_lazy_recorded_value")
{
	BatchStateTracker<TestCommand> tracker;
	tracker.set(TestCommand::Other);
	RecordedValue recorded;
	int32 current = 1;

	UpdateRenderer2DState(tracker, TestCommand::Value, 1, current, recorded);
	CHECK(current == 1);
	CHECK_FALSE(tracker.has(TestCommand::Value));
	CHECK(recorded.reads == 0);

	UpdateRenderer2DState(tracker, TestCommand::Value, 2, current, recorded);
	CHECK(current == 2);
	CHECK(tracker.has(TestCommand::Value));
	CHECK(recorded.reads == 0);

	UpdateRenderer2DState(tracker, TestCommand::Value, 3, current, recorded);
	CHECK(current == 3);
	CHECK(tracker.has(TestCommand::Value));
	CHECK(recorded.reads == 1);

	UpdateRenderer2DState(tracker, TestCommand::Value, 1, current, recorded);
	CHECK(current == 1);
	CHECK_FALSE(tracker.has(TestCommand::Value));
	CHECK(recorded.reads == 2);
	CHECK(tracker.has(TestCommand::Other));
}

# if SIV3D_PLATFORM(WINDOWS) || SIV3D_PLATFORM(MACOS)

namespace
{
# if SIV3D_PLATFORM(WINDOWS)
	using CommandManager = D3D11Renderer2DCommandManager;
	using CommandType = D3D11Renderer2DCommandType;
# else
	using CommandManager = MetalRenderer2DCommandManager;
	using CommandType = MetalRenderer2DCommandType;
# endif

	template <class State, class Push, class Get>
	void CheckCoalescing(const CommandType command, const State& initial, const State& second,
		const State& third, Push push, Get get)
	{
		CAPTURE(FromEnum(command));
		CommandManager manager;
		const auto set = [&](const State& value) { std::invoke(push, manager, value); };
		set(initial);
		manager.flush();
		const size_t first = manager.getCommands().size();

		manager.pushDraw(3);
		set(initial);
		set(second);
		set(second);
		set(third);
		set(initial);
		manager.pushDraw(6);
		set(second);
		set(third);
		manager.pushDraw(12);
		manager.flush();

		const auto& commands = manager.getCommands();
		REQUIRE(commands.size() == (first + 3));
		REQUIRE(commands[first].type == CommandType::Draw);
		CHECK(manager.getDraw(commands[first].index).indexCount == 9);
		REQUIRE(commands[first + 1].type == command);
		CHECK(std::invoke(get, manager, commands[first + 1].index) == third);
		REQUIRE(commands[first + 2].type == CommandType::Draw);
		CHECK(manager.getDraw(commands[first + 2].index).indexCount == 12);

		set(third);
		set(third);
		manager.flush();
		CHECK(commands.size() == (first + 3));

		set(initial);
		manager.flush();
		REQUIRE(commands.size() == (first + 4));
		REQUIRE(commands.back().type == command);
		CHECK(std::invoke(get, manager, commands.back().index) == initial);
	}
}

TEST_CASE("Renderer2D.state_update_coalescing")
{
	CheckCoalescing<Float4>(CommandType::ColorMul, { 1, 1, 1, 1 }, { 1, 0, 0, 1 }, { 0, 1, 0, 0.5f },
		&CommandManager::pushColorMul, &CommandManager::getColorMul);
	CheckCoalescing<Float3>(CommandType::ColorAdd, { 0, 0, 0 }, { 1, 0, 0 }, { 0, 0.5f, 1 },
		&CommandManager::pushColorAdd, &CommandManager::getColorAdd);

	std::array<Float4, 4> patternA{}, patternB{}, patternC{};
	patternB[0].x = 1;
	patternC[3].w = 2;
	CheckCoalescing(CommandType::PatternParameters, patternA, patternB, patternC,
		&CommandManager::pushPatternParameter,
		[](const CommandManager& manager, uint32 index) { return manager.getPatternParameter(index); });

	CheckCoalescing<BlendState>(CommandType::BlendState, BlendState::Default2D, BlendState::Additive, BlendState::Opaque,
		&CommandManager::pushBlendState, &CommandManager::getBlendState);
	CheckCoalescing<RasterizerState>(CommandType::RasterizerState, RasterizerState::Default2D,
		RasterizerState::SolidCullBack, RasterizerState::WireframeCullNone,
		&CommandManager::pushRasterizerState, &CommandManager::getRasterizerState);
	CheckCoalescing<Optional<Rect>>(CommandType::ScissorRect, none, Rect{ 10, 20, 30, 40 }, Rect{ 40, 30, 20, 10 },
		&CommandManager::pushScissorRect, &CommandManager::getScissorRect);
	CheckCoalescing<Optional<Rect>>(CommandType::Viewport, none, Rect{ 0, 0, 320, 240 }, Rect{ 20, 10, 160, 120 },
		&CommandManager::pushViewport, &CommandManager::getViewport);

	std::array<Float4, 3> sdfA{}, sdfB{}, sdfC{};
	sdfB[0].x = 0.5f;
	sdfC[2].w = 1;
	CheckCoalescing(CommandType::SDFParams, sdfA, sdfB, sdfC,
		&CommandManager::pushSDFParameters, &CommandManager::getSDFParameters);
	CheckCoalescing(CommandType::SetVS, VertexShader::IDType::Invalid(), VertexShader::IDType{ 1 }, VertexShader::IDType{ 2 },
		&CommandManager::pushEngineVS, &CommandManager::getVS);
	CheckCoalescing(CommandType::SetPS, PixelShader::IDType::Invalid(), PixelShader::IDType{ 1 }, PixelShader::IDType{ 2 },
		&CommandManager::pushEnginePS, &CommandManager::getPS);

	for (uint32 slot = 0; slot < Graphics::TextureSlotCount; ++slot)
	{
		CAPTURE(slot);
		CheckCoalescing<SamplerState>(ToEnum<CommandType>(FromEnum(CommandType::VSSamplerState0) + slot),
			SamplerState::Default2D, SamplerState::RepeatNearest, SamplerState::MirrorLinear,
			[=](CommandManager& manager, const SamplerState& value) { manager.pushVSSamplerState(value, slot); },
			[=](const CommandManager& manager, uint32 index) { return manager.getVSSamplerState(slot, index); });
		CheckCoalescing<SamplerState>(ToEnum<CommandType>(FromEnum(CommandType::PSSamplerState0) + slot),
			SamplerState::Default2D, SamplerState::RepeatNearest, SamplerState::MirrorLinear,
			[=](CommandManager& manager, const SamplerState& value) { manager.pushPSSamplerState(value, slot); },
			[=](const CommandManager& manager, uint32 index) { return manager.getPSSamplerState(slot, index); });
	}
}

TEST_CASE("Renderer2D.state_update_independent_sampler_slots")
{
	CommandManager manager;
	const size_t first = manager.getCommands().size();
	manager.pushDraw(6);
	for (uint32 slot = 0; slot < Graphics::TextureSlotCount; ++slot)
	{
		manager.pushVSSamplerState(SamplerState::RepeatNearest, slot);
		manager.pushPSSamplerState(SamplerState::RepeatNearest, slot);
	}
	for (uint32 slot = 0; slot < Graphics::TextureSlotCount; ++slot)
	{
		if ((slot % 2) == 0) manager.pushVSSamplerState(SamplerState::Default2D, slot);
		else manager.pushPSSamplerState(SamplerState::Default2D, slot);
	}
	manager.pushDraw(6);
	manager.flush();

	const auto& commands = manager.getCommands();
	REQUIRE(commands.size() == (first + Graphics::TextureSlotCount + 2));
	REQUIRE(commands[first].type == CommandType::Draw);
	REQUIRE(commands.back().type == CommandType::Draw);
	CHECK(manager.getDraw(commands[first].index).indexCount == 6);
	CHECK(manager.getDraw(commands.back().index).indexCount == 6);
	std::array<bool, Graphics::TextureSlotCount> seenVS{}, seenPS{};
	for (size_t i = (first + 1); i < (commands.size() - 1); ++i)
	{
		const auto& command = commands[i];
		const uint32 type = FromEnum(command.type);
		if ((FromEnum(CommandType::VSSamplerState0) <= type) && (type <= FromEnum(CommandType::VSSamplerState7)))
		{
			const uint32 slot = (type - FromEnum(CommandType::VSSamplerState0));
			CHECK_FALSE(seenVS[slot]);
			seenVS[slot] = true;
			CHECK(manager.getVSSamplerState(slot, command.index) == SamplerState{ SamplerState::RepeatNearest });
		}
		else
		{
			REQUIRE(FromEnum(CommandType::PSSamplerState0) <= type);
			REQUIRE(type <= FromEnum(CommandType::PSSamplerState7));
			const uint32 slot = (type - FromEnum(CommandType::PSSamplerState0));
			CHECK_FALSE(seenPS[slot]);
			seenPS[slot] = true;
			CHECK(manager.getPSSamplerState(slot, command.index) == SamplerState{ SamplerState::RepeatNearest });
		}
	}
	for (uint32 slot = 0; slot < Graphics::TextureSlotCount; ++slot)
	{
		CAPTURE(slot);
		CHECK(seenVS[slot] == ((slot % 2) != 0));
		CHECK(seenPS[slot] == ((slot % 2) == 0));
	}
}

# endif
