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

namespace s3d
{
	namespace Math
	{
		namespace detail
		{
			//////////////////////////////////////////////////
			//
			//	Fmod_impl
			//
			//////////////////////////////////////////////////
			
			struct Fmod_impl
			{
				[[nodiscard]]
				auto operator ()(const Concept::Arithmetic auto x, const Concept::Arithmetic auto y) const noexcept
				{
					using R = CommonFloat_t<decltype(x), decltype(y)>;

					return std::fmod(static_cast<R>(x), static_cast<R>(y));
				}

				[[nodiscard]]
				float operator ()(const float x, const float y) const noexcept
				{
					return std::fmod(x, y);
				}

				[[nodiscard]]
				double operator ()(const double x, const double y) const noexcept
				{
					return std::fmod(x, y);
				}

				[[nodiscard]]
				Float2 operator ()(const Float2 x, const Float2 y) const noexcept
				{
					return{ std::fmod(x.x, y.x), std::fmod(x.y, y.y) };
				}

				[[nodiscard]]
				Vec2 operator ()(const Vec2 x, const Vec2 y) const noexcept
				{
					return{ std::fmod(x.x, y.x), std::fmod(x.y, y.y) };
				}

				[[nodiscard]]
				Float3 operator ()(const Float3 x, const Float3 y) const noexcept
				{
					return{ std::fmod(x.x, y.x), std::fmod(x.y, y.y), std::fmod(x.z, y.z) };
				}

				[[nodiscard]]
				Vec3 operator ()(const Vec3 x, const Vec3 y) const noexcept
				{
					return{ std::fmod(x.x, y.x), std::fmod(x.y, y.y), std::fmod(x.z, y.z) };
				}

				[[nodiscard]]
				Float4 operator ()(const Float4 x, const Float4 y) const noexcept
				{
					return{ std::fmod(x.x, y.x), std::fmod(x.y, y.y), std::fmod(x.z, y.z), std::fmod(x.w, y.w) };
				}

				[[nodiscard]]
				Vec4 operator ()(const Vec4 x, const Vec4 y) const noexcept
				{
					return{ std::fmod(x.x, y.x), std::fmod(x.y, y.y), std::fmod(x.z, y.z), std::fmod(x.w, y.w) };
				}

				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x, const BigFloat& y) const;
			};

			//////////////////////////////////////////////////
			//
			//	Frexp_impl
			//
			//////////////////////////////////////////////////

			//////////////////////////////////////////////////
			//
			//	Ldexp_impl
			//
			//////////////////////////////////////////////////

			//////////////////////////////////////////////////
			//
			//	Log_impl
			//
			//////////////////////////////////////////////////

			//////////////////////////////////////////////////
			//
			//	Log2_impl
			//
			//////////////////////////////////////////////////

			//////////////////////////////////////////////////
			//
			//	Log10_impl
			//
			//////////////////////////////////////////////////

			//////////////////////////////////////////////////
			//
			//	Modf_impl
			//
			//////////////////////////////////////////////////

			//////////////////////////////////////////////////
			//
			//	Pow_impl
			//
			//////////////////////////////////////////////////

			//////////////////////////////////////////////////
			//
			//	Sign_impl
			//
			//////////////////////////////////////////////////

			

			//////////////////////////////////////////////////
			//
			//	Sqrt_impl
			//
			//////////////////////////////////////////////////

			struct Sqrt_impl
			{
				[[nodiscard]]
				double operator ()(const Concept::Integral auto x) const noexcept
				{
					return std::sqrt(static_cast<double>(x));
				}

				[[nodiscard]]
				float operator ()(const float x) const noexcept
				{
					return std::sqrt(x);
				}

				[[nodiscard]]
				double operator ()(const double x) const noexcept
				{
					return std::sqrt(x);
				}

				[[nodiscard]]
				Float2 operator ()(const Float2 x) const noexcept
				{
					return{ std::sqrt(x.x), std::sqrt(x.y) };
				}

				[[nodiscard]]
				Vec2 operator ()(const Vec2 x) const noexcept
				{
					return{ std::sqrt(x.x), std::sqrt(x.y) };
				}

				[[nodiscard]]
				Float3 operator ()(const Float3 x) const noexcept
				{
					return{ std::sqrt(x.x), std::sqrt(x.y), std::sqrt(x.z) };
				}

				[[nodiscard]]
				Vec3 operator ()(const Vec3 x) const noexcept
				{
					return{ std::sqrt(x.x), std::sqrt(x.y), std::sqrt(x.z) };
				}

				[[nodiscard]]
				Float4 operator ()(const Float4 x) const noexcept
				{
					return{ std::sqrt(x.x), std::sqrt(x.y), std::sqrt(x.z), std::sqrt(x.w) };
				}

				[[nodiscard]]
				Vec4 operator ()(const Vec4 x) const noexcept
				{
					return{ std::sqrt(x.x), std::sqrt(x.y), std::sqrt(x.z), std::sqrt(x.w) };
				}

				[[nodiscard]]
				BigFloat operator ()(const BigInt& x) const;

				[[nodiscard]]
				BigFloat operator ()(const BigFloat& x) const;
			};
		}
	}
}
