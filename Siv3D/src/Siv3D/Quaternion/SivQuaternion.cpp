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

# include <Siv3D/Quaternion.hpp>
# include <Siv3D/FormatData.hpp>
# include <Siv3D/FloatFormatter.hpp>
# include <Siv3D/Mat4x4.hpp>
# include <cmath>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	canonicalize
	//
	////////////////////////////////////////////////////////////////

	Quaternion& SIV3D_VECTOR_CALL Quaternion::canonicalize() & noexcept
	{
		const Float4 components = value.toFloat4();
		const bool shouldNegate = ((components.w < 0.0f)
			|| ((components.w == 0.0f) && ((components.x < 0.0f)
				|| ((components.x == 0.0f) && ((components.y < 0.0f)
					|| ((components.y == 0.0f) && (components.z < 0.0f)))))));

		if (shouldNegate)
		{
			value.vec = DirectX::XMVectorNegate(value.vec);
		}

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	FromUnitVectors
	//
	////////////////////////////////////////////////////////////////

	Quaternion SIV3D_VECTOR_CALL Quaternion::FromUnitVectors(const Vec3& from, const Vec3& to) noexcept
	{
		const double real = (from.dot(to) + 1.0);
		Vec4 result;

		if (real < 1e-7)
		{
			if ((std::abs(from.x) <= std::abs(from.y)) && (std::abs(from.x) <= std::abs(from.z)))
			{
				result = Vec4{ 0.0, -from.z, from.y, 0.0 };
			}
			else if (std::abs(from.y) <= std::abs(from.z))
			{
				result = Vec4{ -from.z, 0.0, from.x, 0.0 };
			}
			else
			{
				result = Vec4{ -from.y, from.x, 0.0, 0.0 };
			}
		}
		else
		{
			result = Vec4{ from.cross(to), real };
		}

		return Quaternion{ SimdFloat4{ result.normalized() } };
	}

	////////////////////////////////////////////////////////////////
	//
	//	FromVectors
	//
	////////////////////////////////////////////////////////////////

	Quaternion SIV3D_VECTOR_CALL Quaternion::FromVectors(const Vec3& from, const Vec3& to) noexcept
	{
		return FromUnitVectors(from.normalized(), to.normalized());
	}

	////////////////////////////////////////////////////////////////
	//
	//	FromUnitVectorPairs
	//
	////////////////////////////////////////////////////////////////

	Quaternion SIV3D_VECTOR_CALL Quaternion::FromUnitVectorPairs(
		const std::pair<Vec3, Vec3>& from, const std::pair<Vec3, Vec3>& to) noexcept
	{
		const Vec3& primaryFrom = from.first;
		const Vec3& secondaryFrom = from.second;
		const Vec3& primaryTo = to.first;
		const Vec3& secondaryTo = to.second;

		const Quaternion primaryRotation = FromUnitVectors(primaryFrom, primaryTo);
		const Float3 secondaryToInSourceSpaceFloat = primaryRotation.inverseRotate(Float3{ secondaryTo });
		const Vec3 secondaryToInSourceSpace{ secondaryToInSourceSpaceFloat };
		const Vec3 projectedFrom = secondaryFrom.projectOnPlane(primaryFrom);
		const Vec3 projectedTo = secondaryToInSourceSpace.projectOnPlane(primaryFrom);

		double angle = projectedFrom.angleTo(projectedTo);
		if (projectedTo.dot(primaryFrom.cross(secondaryFrom)) < 0.0)
		{
			angle = -angle;
		}

		const Quaternion twist = RotationNormal(Float3{ primaryFrom }, static_cast<float>(angle));
		return (twist * primaryRotation);
	}

	////////////////////////////////////////////////////////////////
	//
	//	FromRotationMatrix
	//
	////////////////////////////////////////////////////////////////

	Quaternion SIV3D_VECTOR_CALL Quaternion::FromRotationMatrix(const Mat4x4& rotationMatrix) noexcept
	{
		return Quaternion{ SimdFloat4{ DirectX::XMQuaternionRotationMatrix(rotationMatrix.value) } };
	}

	void Formatter(FormatData& formatData, const Quaternion& value)
	{
		const Float4 xyzw = value.toFloat4();

		formatData.string.push_back(U'(');
		detail::AppendFloat(formatData.string, xyzw.x);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, xyzw.y);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, xyzw.z);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, xyzw.w);
		formatData.string.push_back(U')');
	}
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

fmt::format_context::iterator fmt::formatter<s3d::Quaternion>::format(const s3d::Quaternion& value, fmt::format_context& ctx) const
{
	const s3d::Float4 xyzw = value.toFloat4();

	if (tag.empty())
	{
		return fmt::format_to(ctx.out(), "({}, {}, {}, {})", xyzw.x, xyzw.y, xyzw.z, xyzw.w);
	}
	else
	{
		const std::string format
			= ("({:" + tag + "}, {:" + tag + "}, {:" + tag + "}, {:" + tag + "})");
		return fmt::vformat_to(ctx.out(), format, fmt::make_format_args(xyzw.x, xyzw.y, xyzw.z, xyzw.w));
	}
}

s3d::ParseContext::iterator fmt::formatter<s3d::Quaternion, s3d::char32>::parse(s3d::ParseContext& ctx)
{
	return s3d::FmtHelper::GetFormatTag(tag, ctx);
}

s3d::BufferContext::iterator fmt::formatter<s3d::Quaternion, s3d::char32>::format(const s3d::Quaternion& value, s3d::BufferContext& ctx) const
{
	const s3d::Float4 xyzw = value.toFloat4();

	if (tag.empty())
	{
		return format_to(ctx.out(), U"({}, {}, {}, {})", xyzw.x, xyzw.y, xyzw.z, xyzw.w);
	}
	else
	{
		const std::u32string format
			= (U"({:" + tag + U"}, {:" + tag + U"}, {:" + tag + U"}, {:" + tag + U"})");
		return format_to(ctx.out(), format, xyzw.x, xyzw.y, xyzw.z, xyzw.w);
	}
}
