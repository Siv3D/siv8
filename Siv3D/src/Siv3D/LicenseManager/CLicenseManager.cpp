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

# include <Siv3D/Keyboard.hpp>
# include <Siv3D/LicenseManager.hpp>
# include <Siv3D/FileSystem.hpp>
# include "CLicenseManager.hpp"
# include "LicenseList.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	CLicenseManager::CLicenseManager()
		: m_licenses(std::begin(detail::licenses), std::end(detail::licenses))
		, m_applicationName{ FileSystem::BaseName(FileSystem::ModulePath()) } {}

	////////////////////////////////////////////////////////////////
	//
	//	update
	//
	////////////////////////////////////////////////////////////////

	void CLicenseManager::update()
	{
		if (m_openLicenseWithF1Key && KeyF1.down())
		{
			LicenseManager::ShowInBrowser();
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	setApplicationLicense
	//
	////////////////////////////////////////////////////////////////

	void CLicenseManager::setApplicationLicense(const String& applicationName, const LicenseInfo& license)
	{
		if (not m_hasApplicationLicense)
		{
			m_licenses.push_front(license);

			++m_customLicenseCount;
			
			m_hasApplicationLicense = true;
		}
		else
		{
			m_licenses.front() = license;
		}

		m_applicationName = applicationName;
	}

	////////////////////////////////////////////////////////////////
	//
	//	addLicense
	//
	////////////////////////////////////////////////////////////////

	void CLicenseManager::addLicense(const LicenseInfo& license)
	{
		m_licenses.insert((m_licenses.begin() + m_customLicenseCount), license);

		++m_customLicenseCount;
	}

	////////////////////////////////////////////////////////////////
	//
	//	enumLicenses
	//
	////////////////////////////////////////////////////////////////

	const Array<LicenseInfo>& CLicenseManager::enumLicenses() const noexcept
	{
		return m_licenses;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getApplicationName
	//
	////////////////////////////////////////////////////////////////

	const String& CLicenseManager::getApplicationName() const noexcept
	{
		return m_applicationName;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setDefaultTriggerEnabled
	//
	////////////////////////////////////////////////////////////////

	void CLicenseManager::setDefaultTriggerEnabled(const bool enabled) noexcept
	{
		m_openLicenseWithF1Key = enabled;
	}
}
