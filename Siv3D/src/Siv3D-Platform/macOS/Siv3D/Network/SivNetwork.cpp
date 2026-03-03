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

# include <Siv3D/Network.hpp>
# include <cstring>
# include <ifaddrs.h>
# include <net/if.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <SystemConfiguration/SystemConfiguration.h>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		constexpr bool IsLoopbackIPv4(uint8 a, uint8, uint8, uint8) noexcept
		{
			return (a == 127);
		}

		[[nodiscard]]
		constexpr bool IsLinkLocalIPv4(uint8 a, uint8 b, uint8, uint8) noexcept
		{
			return (a == 169) && (b == 254);
		}

		[[nodiscard]]
		static bool HasInternetConnectivityImpl() noexcept
		{
			// 0.0.0.0 を指定して「デフォルトルート到達性」を問い合わせる定番パターン
			sockaddr_in zeroAddress{};
			std::memset(&zeroAddress, 0, sizeof(zeroAddress));
			zeroAddress.sin_len = sizeof(zeroAddress);
			zeroAddress.sin_family = AF_INET;

			SCNetworkReachabilityRef reachability =
				::SCNetworkReachabilityCreateWithAddress(
					kCFAllocatorDefault,
					reinterpret_cast<const sockaddr*>(&zeroAddress));

			if (!reachability)
			{
				return false;
			}

			SCNetworkReachabilityFlags flags = 0;
			const bool didRetrieveFlags = (::SCNetworkReachabilityGetFlags(reachability, &flags) != FALSE);

			::CFRelease(reachability);

			if (!didRetrieveFlags)
			{
				return false;
			}

			const bool isReachable = ((flags & kSCNetworkReachabilityFlagsReachable) != 0);
			if (!isReachable)
			{
				return false;
			}

			const bool connectionRequired = ((flags & kSCNetworkReachabilityFlagsConnectionRequired) != 0);
			if (!connectionRequired)
			{
				return true;
			}

			// 接続が「自動で」確立できるケースは到達可能とみなす
			const bool connectionOnDemand  = ((flags & kSCNetworkReachabilityFlagsConnectionOnDemand) != 0);
			const bool connectionOnTraffic = ((flags & kSCNetworkReachabilityFlagsConnectionOnTraffic) != 0);
			const bool interventionRequired = ((flags & kSCNetworkReachabilityFlagsInterventionRequired) != 0);

			if ((connectionOnDemand || connectionOnTraffic) && !interventionRequired)
			{
				return true;
			}

			return false;
		}
	}

	namespace Network
	{
		bool HasInternetConnectivity()
		{
			return HasInternetConnectivityImpl();
		}

		Array<IPv4Address> EnumerateLocalIPv4Addresses()
		{
			Array<IPv4Address> results;

			ifaddrs* ifaList = nullptr;
			if (::getifaddrs(&ifaList) != 0 || !ifaList)
			{
				return results;
			}

			for (const ifaddrs* ifa = ifaList; ifa; ifa = ifa->ifa_next)
			{
				if (!ifa->ifa_addr)
				{
					continue;
				}

				// IPv4 のみ
				if (ifa->ifa_addr->sa_family != AF_INET)
				{
					continue;
				}

				// 有効な NIC のみ（Up）
				if ((ifa->ifa_flags & IFF_UP) == 0)
				{
					continue;
				}

				// ループバック IF を除外
				if ((ifa->ifa_flags & IFF_LOOPBACK) != 0)
				{
					continue;
				}

				const auto* sin = reinterpret_cast<const sockaddr_in*>(ifa->ifa_addr);
				const uint32 v = ntohl(sin->sin_addr.s_addr);

				const uint8 a = static_cast<uint8>((v >> 24) & 0xFF);
				const uint8 b = static_cast<uint8>((v >> 16) & 0xFF);
				const uint8 c = static_cast<uint8>((v >>  8) & 0xFF);
				const uint8 d = static_cast<uint8>( v        & 0xFF);

				// ループバック除外（保険）
				if (IsLoopbackIPv4(a, b, c, d))
				{
					continue;
				}

				// リンクローカル除外
				if (IsLinkLocalIPv4(a, b, c, d))
				{
					continue;
				}

				results.emplace_back(a, b, c, d);
			}

			::freeifaddrs(ifaList);

			results.sort_and_unique();
			return results;
		}
	}
}
