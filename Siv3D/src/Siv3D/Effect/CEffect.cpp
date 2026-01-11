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

# include "CEffect.hpp"
# include <Siv3D/Time.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	CEffect::CEffect() = default;

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	CEffect::~CEffect()
	{
		LOG_SCOPED_DEBUG("CEffect::~CEffect()");

		m_effects.destroy();
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void CEffect::init()
	{
		LOG_SCOPED_DEBUG("CEffect::init()");

		// null Effect を管理に登録
		{
			// null Effect を作成
			auto nullEffect = std::make_unique<EffectData>(EffectData::Null{});

			if (not nullEffect->isInitialized()) // もし作成に失敗していたら
			{
				throw InternalEngineError{ "Failed to create a null Effect" };
			}

			// 管理に登録
			m_effects.setNullData(std::move(nullEffect));
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	update
	//
	////////////////////////////////////////////////////////////////

	void CEffect::update()
	{
		const uint64 currentTimeUs = Time::GetMicrosec();

		if (m_previousTimeUs == 0) // 初めての呼び出し
		{
			m_previousTimeUs = currentTimeUs;
		}
		else
		{
			const uint64 deltaTimeUs = (currentTimeUs - std::exchange(m_previousTimeUs, currentTimeUs));

			for (auto& data : m_effects)
			{
				data.second->advanceTime(deltaTimeUs);
			}
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	create
	//
	////////////////////////////////////////////////////////////////

	Effect::IDType CEffect::create(const double maxLifeTimeSec, const bool sortingEnabled)
	{
		// Effect を作成
		auto effect = std::make_unique<EffectData>(maxLifeTimeSec, sortingEnabled);

		if (not effect->isInitialized()) // もし作成に失敗していたら
		{
			return Effect::IDType::Null();
		}

		// Effect を管理に登録
		return m_effects.add(std::move(effect));
	}

	////////////////////////////////////////////////////////////////
	//
	//	release
	//
	////////////////////////////////////////////////////////////////

	void CEffect::release(const Effect::IDType handleID)
	{
		m_effects.erase(handleID);
	}

	////////////////////////////////////////////////////////////////
	//
	//	add
	//
	////////////////////////////////////////////////////////////////

	void CEffect::add(const Effect::IDType handleID, std::unique_ptr<IEffect>&& effect, const double initialElapsedTimeSec)
	{
		m_effects[handleID]->add(std::move(effect), initialElapsedTimeSec);
	}

	////////////////////////////////////////////////////////////////
	//
	//	size
	//
	////////////////////////////////////////////////////////////////

	size_t CEffect::size(const Effect::IDType handleID)
	{
		return m_effects[handleID]->size();
	}

	////////////////////////////////////////////////////////////////
	//
	//	pause
	//
	////////////////////////////////////////////////////////////////

	void CEffect::pause(const Effect::IDType handleID)
	{
		m_effects[handleID]->pause();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isPaused
	//
	////////////////////////////////////////////////////////////////

	bool CEffect::isPaused(const Effect::IDType handleID)
	{
		return m_effects[handleID]->isPaused();
	}

	////////////////////////////////////////////////////////////////
	//
	//	resume
	//
	////////////////////////////////////////////////////////////////

	void CEffect::resume(const Effect::IDType handleID)
	{
		m_effects[handleID]->resume();
	}

	////////////////////////////////////////////////////////////////
	//
	//	setSpeed, getSpeed
	//
	////////////////////////////////////////////////////////////////

	void CEffect::setSpeed(const Effect::IDType handleID, const double speed)
	{
		m_effects[handleID]->setSpeed(speed);
	}

	double CEffect::getSpeed(const Effect::IDType handleID)
	{
		return m_effects[handleID]->getSpeed();
	}

	////////////////////////////////////////////////////////////////
	//
	//	setMaxLifeTime, getMaxLifeTime
	//
	////////////////////////////////////////////////////////////////

	void CEffect::setMaxLifeTime(const Effect::IDType handleID, const double maxLifeTimeSec)
	{
		m_effects[handleID]->setMaxLifeTime(maxLifeTimeSec);
	}

	double CEffect::getMaxLifeTime(const Effect::IDType handleID)
	{
		return m_effects[handleID]->getMaxLifeTime();
	}

	////////////////////////////////////////////////////////////////
	//
	//	setSortingEnabled, isSortingEnabled
	//
	////////////////////////////////////////////////////////////////

	void CEffect::setSortingEnabled(const Effect::IDType handleID, const bool enabled)
	{
		m_effects[handleID]->setSortingEnabled(enabled);
	}

	bool CEffect::isSortingEnabled(const Effect::IDType handleID)
	{
		return m_effects[handleID]->isSortingEnabled();
	}

	////////////////////////////////////////////////////////////////
	//
	//	updateEffect
	//
	////////////////////////////////////////////////////////////////

	void CEffect::updateEffect(const Effect::IDType handleID)
	{
		m_effects[handleID]->update();
	}

	////////////////////////////////////////////////////////////////
	//
	//	clear
	//
	////////////////////////////////////////////////////////////////

	void CEffect::clear(const Effect::IDType handleID)
	{
		m_effects[handleID]->clear();
	}
}
