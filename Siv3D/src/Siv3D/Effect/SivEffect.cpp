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

# include <Siv3D/Effect.hpp>
# include <Siv3D/Effect/IEffect.hpp>
# include <Siv3D/Troubleshooting/Troubleshooting.hpp>
# include <Siv3D/AssetMonitor/IAssetMonitor.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace
	{
		static void CheckEngine(const StringView type = U"Effect")
		{
			if (Siv3DEngine::isNull())
			{
				Troubleshooting::Show(Troubleshooting::Error::AssetInitializationBeforeEngineStartup, type);
				std::exit(EXIT_FAILURE);
			}
		}
	}

	template <>
	AssetIDWrapper<AssetHandle<Effect>>::AssetIDWrapper()
	{
		CheckEngine();
	}

	template <>
	AssetIDWrapper<AssetHandle<Effect>>::~AssetIDWrapper()
	{
		if (not Siv3DEngine::isAvailable())
		{
			return;
		}

		SIV3D_ENGINE(Effect)->release(m_id);
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	Effect::Effect(const SortingEnabled sortingEnabled)
		: Effect{ 10.0, sortingEnabled } {}

	Effect::Effect(const double maxLifeTimeSec, const SortingEnabled sortingEnabled)
		: AssetHandle{ (CheckEngine(), std::make_shared<AssetIDWrapperType>(SIV3D_ENGINE(Effect)->create(maxLifeTimeSec, sortingEnabled.getBool()))) }
	{
		SIV3D_ENGINE(AssetMonitor)->reportAssetCreation();
	}

	Effect::Effect(const Duration& maxLifeTimeSec, const SortingEnabled sortingEnabled)
		: Effect{ maxLifeTimeSec.count(), sortingEnabled } {}

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	Effect::~Effect() {}

	////////////////////////////////////////////////////////////////
	//
	//	add
	//
	////////////////////////////////////////////////////////////////

	const Effect& Effect::add(std::unique_ptr<IEffect>&& effect, const double initialElapsedTimeSec) const
	{
		if (effect)
		{
			SIV3D_ENGINE(Effect)->add(m_handle->id(), std::move(effect), initialElapsedTimeSec);
		}

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator bool
	//
	////////////////////////////////////////////////////////////////

	Effect::operator bool() const
	{
		return (size() != 0);
	}

	////////////////////////////////////////////////////////////////
	//
	//	isEmpty
	//
	////////////////////////////////////////////////////////////////

	bool Effect::isEmpty() const
	{
		return (size() == 0);
	}

	////////////////////////////////////////////////////////////////
	//
	//	size
	//
	////////////////////////////////////////////////////////////////

	size_t Effect::size() const
	{
		return SIV3D_ENGINE(Effect)->size(m_handle->id());
	}

	////////////////////////////////////////////////////////////////
	//
	//	pause
	//
	////////////////////////////////////////////////////////////////

	void Effect::pause() const
	{
		SIV3D_ENGINE(Effect)->pause(m_handle->id());
	}

	////////////////////////////////////////////////////////////////
	//
	//	isPaused
	//
	////////////////////////////////////////////////////////////////

	bool Effect::isPaused() const
	{
		return SIV3D_ENGINE(Effect)->isPaused(m_handle->id());
	}

	////////////////////////////////////////////////////////////////
	//
	//	resume
	//
	////////////////////////////////////////////////////////////////

	void Effect::resume() const
	{
		SIV3D_ENGINE(Effect)->resume(m_handle->id());
	}

	////////////////////////////////////////////////////////////////
	//
	//	setSpeed
	//
	////////////////////////////////////////////////////////////////

	const Effect& Effect::setSpeed(const double speed) const
	{
		SIV3D_ENGINE(Effect)->setSpeed(m_handle->id(), speed);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getSpeed
	//
	////////////////////////////////////////////////////////////////

	double Effect::getSpeed() const
	{
		return SIV3D_ENGINE(Effect)->getSpeed(m_handle->id());
	}

	////////////////////////////////////////////////////////////////
	//
	//	setMaxLifeTime
	//
	////////////////////////////////////////////////////////////////

	const Effect& Effect::setMaxLifeTime(const double maxLifeTimeSec) const
	{
		SIV3D_ENGINE(Effect)->setMaxLifeTime(m_handle->id(), maxLifeTimeSec);
		return *this;
	}

	const Effect& Effect::setMaxLifeTime(const Duration& maxLifeTimeSec) const
	{
		return setMaxLifeTime(maxLifeTimeSec.count());
	}

	////////////////////////////////////////////////////////////////
	//
	//	getMaxLifeTime
	//
	////////////////////////////////////////////////////////////////

	double Effect::getMaxLifeTime() const
	{
		return SIV3D_ENGINE(Effect)->getMaxLifeTime(m_handle->id());
	}

	////////////////////////////////////////////////////////////////
	//
	//	setSortingEnabled
	//
	////////////////////////////////////////////////////////////////

	const Effect& Effect::setSortingEnabled(const bool enabled) const
	{
		SIV3D_ENGINE(Effect)->setSortingEnabled(m_handle->id(), enabled);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	isSortingEnabled
	//
	////////////////////////////////////////////////////////////////

	bool Effect::isSortingEnabled() const
	{
		return SIV3D_ENGINE(Effect)->isSortingEnabled(m_handle->id());
	}

	////////////////////////////////////////////////////////////////
	//
	//	update
	//
	////////////////////////////////////////////////////////////////

	void Effect::update() const
	{
		SIV3D_ENGINE(Effect)->updateEffect(m_handle->id());
	}

	////////////////////////////////////////////////////////////////
	//
	//	clear
	//
	////////////////////////////////////////////////////////////////

	void Effect::clear() const
	{
		SIV3D_ENGINE(Effect)->clear(m_handle->id());
	}

	////////////////////////////////////////////////////////////////
	//
	//	swap
	//
	////////////////////////////////////////////////////////////////

	void Effect::swap(Effect& other) noexcept
	{
		m_handle.swap(other.m_handle);
	}
}
