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

# include <Siv3D/ScopeExit.hpp>
# include "EffectData.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	EffectData::EffectData(Null)
		: EffectData{ Effect::DefaultMaxLifeTimeSec, false } {}

	EffectData::EffectData(const double maxLifeTimeSec, const bool sortingEnabled)
		: m_maxLifeTimeSec{ Max(maxLifeTimeSec, 0.0) }
		, m_sortingEnabled{ sortingEnabled }
		, m_sortDirty{ sortingEnabled } {}

	////////////////////////////////////////////////////////////////
	//
	//	isInitialized
	//
	////////////////////////////////////////////////////////////////

	bool EffectData::isInitialized() const noexcept
	{
		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	add
	//
	////////////////////////////////////////////////////////////////

	void EffectData::add(std::unique_ptr<IEffect> effect, const double initialElapsedTimeSec)
	{
		assert(effect != nullptr);

		if (m_maxLifeTimeSec <= initialElapsedTimeSec)
		{
			return;
		}

		if (m_sortingEnabled)
		{
			m_sortDirty = true;
		}

		EffectHolder holder
		{
			.effect = std::move(effect),
			.elapsedTimeSec = initialElapsedTimeSec
		};

		if (not m_locked)
		{
			m_effects.emplace_back(std::move(holder));
		}
		else // エフェクトの更新中に追加された場合はペンディングに回す
		{
			m_pendingEffects.emplace_back(std::move(holder));
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	size
	//
	////////////////////////////////////////////////////////////////

	size_t EffectData::size() const noexcept
	{
		return (m_effects.size() + m_pendingEffects.size());
	}

	////////////////////////////////////////////////////////////////
	//
	//	pause
	//
	////////////////////////////////////////////////////////////////

	void EffectData::pause() noexcept
	{
		m_paused = true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	isPaused
	//
	////////////////////////////////////////////////////////////////

	bool EffectData::isPaused() const noexcept
	{
		return m_paused;
	}

	////////////////////////////////////////////////////////////////
	//
	//	resume
	//
	////////////////////////////////////////////////////////////////

	void EffectData::resume() noexcept
	{
		m_paused = false;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setSpeed, getSpeed
	//
	////////////////////////////////////////////////////////////////

	void EffectData::setSpeed(const double speed) noexcept
	{
		m_timeScale = speed;
	}

	double EffectData::getSpeed() const noexcept
	{
		return m_timeScale;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setMaxLifeTime, getMaxLifeTime
	//
	////////////////////////////////////////////////////////////////

	void EffectData::setMaxLifeTime(const double maxLifeTimeSec) noexcept
	{
		m_maxLifeTimeSec = Max(maxLifeTimeSec, 0.0);
	}

	double EffectData::getMaxLifeTime() const noexcept
	{
		return m_maxLifeTimeSec;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setSortingEnabled, isSortingEnabled
	//
	////////////////////////////////////////////////////////////////

	void EffectData::setSortingEnabled(const bool enabled) noexcept
	{
		if ((not m_sortingEnabled) && enabled)
		{
			m_sortDirty = true;
		}

		m_sortingEnabled = enabled;
	}

	bool EffectData::isSortingEnabled() const noexcept
	{
		return m_sortingEnabled;
	}

	////////////////////////////////////////////////////////////////
	//
	//	advanceTime
	//
	////////////////////////////////////////////////////////////////

	void EffectData::advanceTime(const uint64 deltaUs) noexcept
	{
		if (m_paused)
		{
			return;
		}

		m_lastDeltaSec += (deltaUs / 1'000'000.0 * m_timeScale);
	}

	////////////////////////////////////////////////////////////////
	//
	//	update
	//
	////////////////////////////////////////////////////////////////

	void EffectData::update()
	{
		m_locked = true;
		const ScopeExit unlock{ [this]()
		{
			m_locked = false;
			
			if (m_clearRequested)
			{
				clear();
			}
		}};

		// ペンディングエフェクトの移動
		if (m_pendingEffects)
		{
			m_effects.insert(
				m_effects.end(),
				std::make_move_iterator(m_pendingEffects.begin()),
				std::make_move_iterator(m_pendingEffects.end())
			);

			m_pendingEffects.clear();

			if (m_sortingEnabled)
			{
				m_sortDirty = true;
			}
		}

		// 経過時間の更新
		{
			for (auto& effect : m_effects)
			{
				effect.elapsedTimeSec += m_lastDeltaSec;
			}

			m_lastDeltaSec = 0.0;
		}

		// エフェクトのソート
		if (m_sortingEnabled && m_sortDirty)
		{
			m_effects.sort_by([](const auto& a, const auto& b)
			{
				return (a.effect->getOrder() < b.effect->getOrder());
			});

			m_sortDirty = false;
		}

		// エフェクトの実行・不要なエフェクトの削除
		{
			m_effects.remove_if([&](const auto& effect)
			{
				if (m_maxLifeTimeSec <= effect.elapsedTimeSec)
				{
					return true;
				}

				return (not effect.effect->update(effect.elapsedTimeSec));
			});
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	clear
	//
	////////////////////////////////////////////////////////////////

	void EffectData::clear()
	{
		if (m_locked)
		{
			m_clearRequested = true;
			return;
		}

		m_effects.clear();
		m_pendingEffects.clear();
		m_sortDirty = false;
		m_clearRequested = false;
	}
}
