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
# include <Siv3D/Common.hpp>
# include <Siv3D/LicenseInfo.hpp>
# include <Siv3D/Array.hpp>

namespace s3d
{
	class SIV3D_NOVTABLE ISiv3DLicenseManager
	{
	public:

		[[nodiscard]]
		static ISiv3DLicenseManager* Create();

		virtual ~ISiv3DLicenseManager() = default;

		virtual void update() = 0;

		virtual void setApplicationLicense(const String& applicationName, const LicenseInfo& license) = 0;

		virtual void addLicense(const LicenseInfo& license) = 0;

		virtual const Array<LicenseInfo>& enumLicenses() const noexcept = 0;

		virtual const String& getApplicationName() const noexcept = 0;

		virtual void setDefaultTriggerEnabled(bool enabled) noexcept = 0;
	};
}
