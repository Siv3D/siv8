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

# pragma once
# include "IAudioEncoder.hpp"

namespace s3d
{
	class CAudioEncoder final : public ISiv3DAudioEncoder
	{
	public:

		CAudioEncoder() = default;

		~CAudioEncoder() override;

		void init() override;

		bool add(std::unique_ptr<IAudioEncoder> encoder) override;

		void remove(StringView encoderName) override;

		const Array<std::unique_ptr<IAudioEncoder>>& enumEncoder() const noexcept override;

		String getEncoderNameFromExtension(StringView extension) const override;

		bool save(const Wave& wave, StringView encoderName, FilePathView path) const override;

		bool encode(const Wave& wave, StringView encoderName, IWriter& writer) const override;

		Blob encode(const Wave& wave, StringView encoderName) const override;

	private:

		Array<std::unique_ptr<IAudioEncoder>> m_encoders;
	};
}
