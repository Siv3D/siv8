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

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	bitangent
	//
	////////////////////////////////////////////////////////////////

	constexpr Float3 Vertex3D::bitangent() const noexcept
	{
		return (Math::Cross(normal, tangent.xyz()) * tangent.w);
	}
}
