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

# include <Siv3D/LicenseManager.hpp>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/TextWriter.hpp>
# include <Siv3D/System.hpp>
# include <Siv3D/LicenseManager/ILicenseManager.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/CacheDirectory/CacheDirectory.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static std::string XML_Br_Escape(const std::string_view s)
		{
			std::string result;

			result.reserve(s.size());

			for (const char ch : s)
			{
				switch (ch)
				{
				case U'&':
					result.append("&amp;");
					break;
				case U'<':
					result.append("&lt;");
					break;
				case U'>':
					result.append("&gt;");
					break;
				case U'\"':
					result.append("&quot;");
					break;
				case U'\'':
					result.append("&apos;");
					break;
				case U'\n':
					result.append("<br>");
					break;
				default:
					result.push_back(static_cast<char>(ch));
					break;
				}
			}

			return result;
		}

		constexpr static std::string_view Header =
R"(<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<title>Licenses</title>
<style>
body		{ background-color: #f9f9f9; font-family: 'Segoe UI','Helvetica Neue','Arial','Hiragino Kaku Gothic ProN','Hiragino Sans','Meiryo',sans-serif; }
h3			{ color: #333333; text-align: center; font-size: 28px; }
div			{ font-size: 14px; line-height: 2; word-wrap: break-word; }
div.c0		{ color: #333333; text-align: center; font-size: 20px; }
div.c1		{ padding-bottom: 8px; color: #555555; text-align: center; font-size: 12px; }
div.c2		{ padding-bottom: 24px; color: #888888; text-align: center; font-size: 9px; }
</style>
</head>
<body>
<h3>Licenses</h3>)";

		constexpr static std::string_view Footer =
R"-(<br>
</body>
</html>)-";

		constexpr static std::string_view DivEnd = "</div>\n";

		constexpr static std::string_view LicenseC0 = R"-(<div class="c0">)-";

		constexpr static std::string_view LicenseC1 = R"-(<div class="c1">)-";

		constexpr static std::string_view LicenseC2 = R"-(<div class="c2">)-";
	}

	namespace LicenseManager
	{
		void SetApplicationLicense(const String& applicationName, const LicenseInfo& license)
		{
			SIV3D_ENGINE(LicenseManager)->setApplicationLicense(applicationName, license);
		}

		void AddLicense(const LicenseInfo& license)
		{
			SIV3D_ENGINE(LicenseManager)->addLicense(license);
		}

		const Array<LicenseInfo>& EnumLicenses() noexcept
		{
			return SIV3D_ENGINE(LicenseManager)->enumLicenses();
		}

		void EnableDefaultTrigger() noexcept
		{
			SIV3D_ENGINE(LicenseManager)->setDefaultTriggerEnabled(true);
		}

		void DisableDefaultTrigger() noexcept
		{
			SIV3D_ENGINE(LicenseManager)->setDefaultTriggerEnabled(false);
		}

		void ShowInBrowser()
		{
			const String& applicationName = SIV3D_ENGINE(LicenseManager)->getApplicationName();
			const FilePath path = (CacheDirectory::Apps(applicationName) + U"Licenses.html");

			{
				TextWriter writer{ path };
				writer.writeUTF8(Header);

				for (const auto& license : SIV3D_ENGINE(LicenseManager)->enumLicenses())
				{
					writer.writelnUTF8(LicenseC0);
					writer.writelnUTF8(XML_Br_Escape(license.title));
					writer.writelnUTF8(DivEnd);

					writer.writelnUTF8(LicenseC1);
					writer.writelnUTF8(XML_Br_Escape(license.copyright));
					writer.writelnUTF8(DivEnd);

					writer.writelnUTF8(LicenseC2);
					writer.writelnUTF8(XML_Br_Escape(license.text));
					writer.writelnUTF8(DivEnd);
				}

				writer.writeUTF8(Footer);
			}

			System::LaunchBrowser(path);
		}
	}
}
