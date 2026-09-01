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

# include "Siv3DTest.hpp"

namespace
{
	static_assert(requires
	{
		static_cast<bool (Mesh3D::*)(FilePathView) const>(&Mesh3D::saveOBJ);
		static_cast<bool (Mesh3D::*)(FilePathView, const Material&) const>(&Mesh3D::saveOBJ);
	});
}

TEST_CASE("Material defaults")
{
	const Material material;

	CHECK_EQ(material.name, U"Material");
	CHECK_EQ(material.baseColor, ColorF{ 1.0 });
	CHECK_EQ(material.metallic, 0.0);
	CHECK_EQ(material.roughness, 1.0);
	CHECK_EQ(material.emissive, ColorF{ 0.0 });
	CHECK_EQ(material.alphaMode, MaterialAlphaMode::Opaque);
	CHECK_EQ(material.alphaCutoff, 0.5);
	CHECK_FALSE(material.doubleSided);
	CHECK_FALSE(material.baseColorTexture.has_value());
	CHECK_FALSE(material.metallicRoughnessTexture.has_value());
	CHECK_FALSE(material.normalTexture.has_value());
	CHECK_EQ(material.normalScale, 1.0);
	CHECK_FALSE(material.occlusionTexture.has_value());
	CHECK_EQ(material.occlusionStrength, 1.0);
	CHECK_FALSE(material.emissiveTexture.has_value());
}

TEST_CASE("MaterialTexture")
{
	const MaterialTexture texture{
		.path = U"texture.png",
		.uvIndex = 2,
	};

	CHECK_EQ(texture.path, U"texture.png");
	CHECK_EQ(texture.uvIndex, uint32{ 2 });
}
