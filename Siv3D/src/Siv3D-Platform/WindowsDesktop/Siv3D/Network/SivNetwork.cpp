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
# ifndef NOMINMAX
#	define	NOMINMAX
# endif
# ifndef STRICT
#	define	STRICT
# endif
# ifndef _WIN32_WINNT
#	define	_WIN32_WINNT	_WIN32_WINNT_WIN10
# endif
# ifndef NTDDI_VERSION
#	define	NTDDI_VERSION	NTDDI_WIN10_RS3
# endif
# include <WinSock2.h>
# include <wininet.h>
# include <netlistmgr.h>
# include <IPTypes.h>
# include <iphlpapi.h>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static bool HasInternetConnectivityImpl() noexcept
		{
			INetworkListManager* pNLM = nullptr;

			HRESULT hr = ::CoCreateInstance(CLSID_NetworkListManager, nullptr, CLSCTX_ALL,
				IID_INetworkListManager, reinterpret_cast<void**>(&pNLM));

			if (FAILED(hr) || !pNLM)
			{
				return false;
			}

			VARIANT_BOOL isConnectedToInternet = VARIANT_FALSE;
			hr = pNLM->get_IsConnectedToInternet(&isConnectedToInternet);

			pNLM->Release();

			if (FAILED(hr))
			{
				return false;
			}

			return (isConnectedToInternet == VARIANT_TRUE);
		}

		[[nodiscard]]
		static bool HasInternetConnectivity_WinINet() noexcept
		{
			DWORD flags = 0;
			return (::InternetGetConnectedState(&flags, 0) != FALSE);
		}

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
	}

	namespace Network
	{
		bool HasInternetConnectivity()
		{
			if (HasInternetConnectivityImpl())
			{
				return true;
			}

			// フォールバック
			return HasInternetConnectivity_WinINet();
		}

		Array<IPv4Address> EnumerateLocalIPv4Addresses()
		{
			Array<IPv4Address> results;

			ULONG flags =
				GAA_FLAG_SKIP_ANYCAST |
				GAA_FLAG_SKIP_MULTICAST |
				GAA_FLAG_SKIP_DNS_SERVER;

			ULONG family = AF_INET;

			ULONG bufLen = 0;
			::GetAdaptersAddresses(family, flags, nullptr, nullptr, &bufLen);

			if (bufLen == 0)
			{
				return results;
			}

			std::vector<std::byte> buffer(bufLen);

			IP_ADAPTER_ADDRESSES* addrs = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(buffer.data());
			const ULONG ret = ::GetAdaptersAddresses(family, flags, nullptr, addrs, &bufLen);

			if (ret != NO_ERROR)
			{
				return results;
			}

			for (auto* aa = addrs; aa; aa = aa->Next)
			{
				// ループバック IF を除外
				if (aa->IfType == IF_TYPE_SOFTWARE_LOOPBACK)
				{
					continue;
				}

				// 有効な NIC のみ
				if (aa->OperStatus != IfOperStatusUp)
				{
					continue;
				}

				for (auto* ua = aa->FirstUnicastAddress; ua; ua = ua->Next)
				{
					const SOCKADDR* sa = ua->Address.lpSockaddr;
					if (!sa || sa->sa_family != AF_INET)
					{
						continue;
					}

					const auto* sin = reinterpret_cast<const sockaddr_in*>(sa);
					const uint32 v = ::ntohl(sin->sin_addr.s_addr);

					const uint8 a = static_cast<uint8>((v >> 24) & 0xFF);
					const uint8 b = static_cast<uint8>((v >> 16) & 0xFF);
					const uint8 c = static_cast<uint8>((v >> 8) & 0xFF);
					const uint8 d = static_cast<uint8>(v & 0xFF);

					// ループバック除外
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
			}

			// 重複除去
			results.sort_and_unique();
			return results;
		}
	}
}
