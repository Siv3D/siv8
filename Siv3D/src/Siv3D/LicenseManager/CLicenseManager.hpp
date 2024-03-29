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
# include "ILicenseManager.hpp"

namespace s3d
{
	class CLicenseManager final : public ISiv3DLicenseManager
	{
	public:

		CLicenseManager();

		void update() override;

		void setApplicationLicense(const String& applicationName, const LicenseInfo& license) override;

		void addLicense(const LicenseInfo& license) override;

		const Array<LicenseInfo>& enumLicenses() const noexcept override;

		const String& getApplicationName() const noexcept override;

		void setDefaultTriggerEnabled(bool enabled) noexcept override;

	private:

		Array<LicenseInfo> m_licenses;

		String m_applicationName;

		uint32 m_customLicenseCount = 0;

		bool m_hasApplicationLicense = false;

		bool m_openLicenseWithF1Key = true;
	};
}
