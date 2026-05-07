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
# include <Siv3D/Common.hpp>
# include <Siv3D/Effect.hpp>

namespace s3d
{
	class SIV3D_NOVTABLE ISiv3DEffect
	{
	public:

		[[nodiscard]]
		static ISiv3DEffect* Create();

		virtual ~ISiv3DEffect() = default;

		virtual void init() = 0;

		virtual void update() = 0;

		virtual Effect::IDType create(double maxLifeTimeSec, bool sortingEnabled) = 0;

		virtual void release(Effect::IDType handleID) = 0;

		virtual void add(Effect::IDType handleID, std::unique_ptr<IEffect> effect, double initialElapsedTimeSec) = 0;

		virtual size_t size(Effect::IDType handleID) = 0;

		virtual void pause(Effect::IDType handleID) = 0;

		virtual bool isPaused(Effect::IDType handleID) = 0;

		virtual void resume(Effect::IDType handleID) = 0;

		virtual void setSpeed(Effect::IDType handleID, double speed) = 0;

		virtual double getSpeed(Effect::IDType handleID) = 0;

		virtual void setMaxLifeTime(Effect::IDType handleID, double maxLifeTimeSec) = 0;

		virtual double getMaxLifeTime(Effect::IDType handleID) = 0;

		virtual void setOrderSortingEnabled(Effect::IDType handleID, bool enabled) = 0;

		virtual bool isOrderSortingEnabled(Effect::IDType handleID) = 0;

		virtual void updateEffect(Effect::IDType handleID) = 0;

		virtual void clear(Effect::IDType handleID) = 0;
	};
}
