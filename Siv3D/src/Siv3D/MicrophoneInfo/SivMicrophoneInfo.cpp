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

# include <Siv3D/MicrophoneInfo.hpp>
# include <Siv3D/Audio/IAudio.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <ThirdParty/miniaudio/miniaudio.h>

namespace s3d
{
	namespace System
	{
		Array<MicrophoneInfo> EnumerateMicrophones()
		{
			::ma_context* context = SIV3D_ENGINE(Audio)->getContext();
			::ma_device_info* pCaptureInfos = nullptr;
			::ma_uint32 captureCount = 0;

			const ::ma_result result = ::ma_context_get_devices(context, nullptr, nullptr, &pCaptureInfos, &captureCount);		
			if (result != MA_SUCCESS)
			{
				return{};
			}

			Array<MicrophoneInfo> infos;

			for (::ma_uint32 i = 0; i < captureCount; ++i)
			{
				const auto& src = pCaptureInfos[i];
				
				MicrophoneInfo info;
				{
					info.name = Unicode::FromUTF8(src.name);

					switch (context->backend)
					{
					case ma_backend_wasapi:
						info.id = Unicode::FromWstring(src.id.wasapi);
						break;
					case ma_backend_coreaudio:
						info.id = Unicode::FromUTF8(src.id.coreaudio);
						break;
					}

					info.isDefault = src.isDefault;
				}

				infos.push_back(info);
			}

			return infos;
		}
	}
}
