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
# include "IImageEncoder.hpp"

namespace s3d
{
	class CImageEncoder final : public ISiv3DImageEncoder
	{
	public:

		CImageEncoder() = default;

		~CImageEncoder() override;

		void init() override;

		bool add(std::unique_ptr<IImageEncoder>&& encoder) override;

		void remove(StringView encoderName) override;

		const Array<std::unique_ptr<IImageEncoder>>& enumEncoder() const noexcept override;

		String getEncoderNameFromExtension(StringView extension) const override;

		bool save(const Image& image, StringView encoderName, FilePathView path) const override;

		bool encode(const Image& image, StringView encoderName, IWriter& writer) const override;

		Blob encode(const Image& image, StringView encoderName) const override;

	private:

		Array<std::unique_ptr<IImageEncoder>> m_encoders;
	};
}
