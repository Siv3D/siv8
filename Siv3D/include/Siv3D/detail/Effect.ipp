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

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	add
	//
	////////////////////////////////////////////////////////////////

	template <class IEffectType, class... Args>
		requires std::derived_from<IEffectType, IEffect>
	const Effect& Effect::add(Args&&... args) const
	{
		return add(std::make_unique<IEffectType>(std::forward<Args>(args)...));
	}

	template <class Fty>
		requires std::invocable<Fty, double> &&
				 std::same_as<std::invoke_result_t<Fty, double>, bool>
	const Effect& Effect::add(Fty&& f) const
	{
		using Fn = std::decay_t<Fty>;

		class AnonymousEffect final : IEffect
		{
		public:

			explicit AnonymousEffect(Fn&& f)
				: m_function{ std::move(f) } {}

			bool update(double timeSec) override
			{
				return std::invoke(m_function, timeSec);
			}

		private:

			[[no_unique_address]] Fn m_function;
		};

		return add(std::make_unique<AnonymousEffect>(std::forward<Fty>(f)));
	}
}
