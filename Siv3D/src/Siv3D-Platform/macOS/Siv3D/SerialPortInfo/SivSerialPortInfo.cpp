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

# include <Siv3D/Common.hpp>
# include <Siv3D/SerialPortInfo.hpp>
# include <Siv3D/FormatLiteral.hpp>
# include <CoreFoundation/CoreFoundation.h>
# include <IOKit/IOKitLib.h>
# include <IOKit/serial/IOSerialKeys.h>

namespace s3d
{
	namespace detail
	{
		// CFStringRef を Siv3D の String (UTF-32) に安全に変換するヘルパー
		static String CFStringToString(CFStringRef cfString)
		{
			if (!cfString)
			{
				return String();
			}

			// UTF-8 のポインタを直接取得できる場合
			if (const char* ptr = CFStringGetCStringPtr(cfString, kCFStringEncodingUTF8))
			{
				return Unicode::FromUTF8(ptr);
			}

			// 取得できない場合はバッファを用意してコピー
			CFIndex length = CFStringGetLength(cfString);
			CFIndex maxSize = CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8) + 1;
			std::string buffer(maxSize, '\0');

			if (CFStringGetCString(cfString, buffer.data(), maxSize, kCFStringEncodingUTF8))
			{
				return Unicode::FromUTF8(buffer.c_str());
			}

			return String();
		}

		// IORegistry のツリーを親に向かって再帰的に探索し、文字列プロパティを取得する
		static String GetStringProperty(io_registry_entry_t entry, CFStringRef key)
		{
			// kIORegistryIterateRecursively | kIORegistryIterateParents を指定することで、
			// クラス名(IOUSBDevice等)に依存せず、確実に親のUSBデバイスノードからプロパティを抽出できます。
			CFTypeRef cfType = ::IORegistryEntrySearchCFProperty(
				entry,
				kIOServicePlane,
				key,
				kCFAllocatorDefault,
				kIORegistryIterateRecursively | kIORegistryIterateParents
			);

			String result;
			if (cfType)
			{
				if (CFGetTypeID(cfType) == CFStringGetTypeID())
				{
					result = CFStringToString(static_cast<CFStringRef>(cfType));
				}
				CFRelease(cfType);
			}
			return result.trimmed();
		}

		// IORegistry のツリーから整数（16ビット）プロパティを取得する
		static Optional<uint16> GetUint16Property(io_registry_entry_t entry, CFStringRef key)
		{
			CFTypeRef cfType = ::IORegistryEntrySearchCFProperty(
				entry,
				kIOServicePlane,
				key,
				kCFAllocatorDefault,
				kIORegistryIterateRecursively | kIORegistryIterateParents
			);

			Optional<uint16> result = none;
			if (cfType)
			{
				if (CFGetTypeID(cfType) == CFNumberGetTypeID())
				{
					uint16_t value = 0;
					if (CFNumberGetValue(static_cast<CFNumberRef>(cfType), kCFNumberSInt16Type, &value))
					{
						result = value;
					}
				}
				CFRelease(cfType);
			}
			return result;
		}
	}

	namespace System
	{
		Array<SerialPortInfo> EnumerateSerialPorts()
		{
			Array<SerialPortInfo> ports;

			// BSD シリアルポートを検索するための辞書を作成
			CFMutableDictionaryRef classesToMatch = ::IOServiceMatching(kIOSerialBSDServiceValue);
			if (!classesToMatch)
			{
				return ports;
			}

			::CFDictionarySetValue(classesToMatch, CFSTR(kIOSerialBSDTypeKey), CFSTR(kIOSerialBSDAllTypes));

			io_iterator_t serialPortIterator;
			if (KERN_SUCCESS != ::IOServiceGetMatchingServices(kIOMainPortDefault, classesToMatch, &serialPortIterator))
			{
				return ports;
			}

			io_object_t serialPort;
			while ((serialPort = ::IOIteratorNext(serialPortIterator)))
			{
				SerialPortInfo info;

				// 1. デバイスパス (ポート名: 例 "/dev/cu.usbserial-xxx")
				CFTypeRef calloutPath = ::IORegistryEntryCreateCFProperty(
					serialPort,
					CFSTR(kIOCalloutDeviceKey),
					kCFAllocatorDefault,
					0
				);

				if (calloutPath)
				{
					if (CFGetTypeID(calloutPath) == CFStringGetTypeID())
					{
						info.port = detail::CFStringToString(static_cast<CFStringRef>(calloutPath)).trimmed();
					}
					CFRelease(calloutPath);
				}

				// ポート名が取得できない場合はスキップ
				if (info.port.isEmpty())
				{
					::IOObjectRelease(serialPort);
					continue;
				}

				// 2. 親の USB デバイスから VID と PID を取得
				info.vendorID  = detail::GetUint16Property(serialPort, CFSTR("idVendor"));
				info.productID = detail::GetUint16Property(serialPort, CFSTR("idProduct"));

				// 3. メーカー名とプロダクト名を取得して結合
				info.manufacturer  = detail::GetStringProperty(serialPort, CFSTR("USB Vendor Name"));
				String productName = detail::GetStringProperty(serialPort, CFSTR("USB Product Name"));

				if (info.manufacturer && productName)
				{
					info.description = info.manufacturer + U" " + productName;
				}
				else if (productName)
				{
					info.description = productName;
				}
				else
				{
					info.description = U"n/a";
				}

				// 4. ハードウェア ID の生成 (Siv3D の format を使用)
				if (info.vendorID && info.productID)
				{
					String serialNum = detail::GetStringProperty(serialPort, CFSTR("USB Serial Number"));
					if (serialNum.isEmpty())
					{
						serialNum = U"None";
					}

					info.hardwareID = U"USB VID:PID={:04x}:{:04x} SNR={}"_fmt(*info.vendorID, *info.productID, serialNum);
				}
				else
				{
					info.hardwareID = U"n/a";
				}

				ports.push_back(info);
				::IOObjectRelease(serialPort);
			}

			::IOObjectRelease(serialPortIterator);

			return ports;
		}
	}
}
