//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/Interpolation.hpp>
# include <Siv3D/Math.hpp>

namespace s3d
{
	namespace
	{
		template <class VectorType, class ValueType>
		VectorType Damp_impl(const VectorType& a, const VectorType& b, const ValueType r, const ValueType dt) noexcept
		{
			return Math::Lerp(a, b, static_cast<ValueType>(1) - Math::Exp(-r * dt));
		}

		template <class VectorType, class ValueType>
		VectorType SmoothDamp_impl(const VectorType& from, const VectorType& to, VectorType& velocity, const ValueType smoothTime, const ValueType deltaTime, const Optional<ValueType>& maxSpeed)
		{
			const ValueType omega = (static_cast<ValueType>(2.0) / smoothTime);
			const ValueType x = (omega * deltaTime);
			const ValueType exp = (static_cast<ValueType>(1.0) / (static_cast<ValueType>(1.0) + x + static_cast<ValueType>(0.48) * x * x + static_cast<ValueType>(0.235) * x * x * x));
			VectorType change = (from - to);

			// 速度を maxSpeed に制限
			if (maxSpeed)
			{
				const ValueType maxChange = (*maxSpeed * smoothTime);

				if constexpr (std::is_arithmetic_v<VectorType>)
				{
					change = Clamp(change, -maxChange, maxChange);
				}
				else
				{
					change = change.setMaxLength(maxChange);
				}
			}

			const VectorType temp = ((velocity + change * omega) * deltaTime);
			velocity = ((velocity - temp * omega) * exp);

			VectorType result = ((from - change) + (change + temp) * exp);

			// 行き過ぎを防ぐ
			if constexpr (std::is_arithmetic_v<VectorType>)
			{
				if ((0 < (to - from)) == (to < result))
				{
					result = to;
					velocity = ((result - to) / deltaTime);
				}
			}
			else
			{
				if (0 < (to - from).dot(result - to))
				{
					result = to;
					velocity = ((result - to) / deltaTime);
				}
			}

			return result;
		}
	}

	namespace Math
	{
		////////////////////////////////////////////////////////////////
		//
		//	MoveTowards
		//
		////////////////////////////////////////////////////////////////

		Float2 MoveTowards(const Float2& current, const Float2& target, const float maxSpeed) noexcept
		{
			return (current + (target - current).setMaxLength(maxSpeed));
		}

		Vec2 MoveTowards(const Vec2& current, const Vec2& target, const double maxSpeed) noexcept
		{
			return (current + (target - current).setMaxLength(maxSpeed));
		}

		Float3 MoveTowards(const Float3& current, const Float3& target, const float maxSpeed) noexcept
		{
			return (current + (target - current).setMaxLength(maxSpeed));
		}

		Vec3 MoveTowards(const Vec3& current, const Vec3& target, const double maxSpeed) noexcept
		{
			return (current + (target - current).setMaxLength(maxSpeed));
		}

		Float4 MoveTowards(const Float4& current, const Float4& target, const float maxSpeed) noexcept
		{
			return (current + (target - current).setMaxLength(maxSpeed));
		}

		Vec4 MoveTowards(const Vec4& current, const Vec4& target, const double maxSpeed) noexcept
		{
			return (current + (target - current).setMaxLength(maxSpeed));
		}

		ColorF MoveTowards(const ColorF& current, const ColorF& target, const double maxSpeed) noexcept
		{
			return ColorF{ MoveTowards(current.toVec4(), target.toVec4(), maxSpeed) };
		}

		HSV MoveTowards(const HSV& current, const HSV& target, const double maxSpeed) noexcept
		{
			return HSV{ MoveTowards(current.hsva(), target.hsva(), maxSpeed) };
		}

		////////////////////////////////////////////////////////////////
		//
		//	Damp
		//
		////////////////////////////////////////////////////////////////

		float Damp(const float current, const float target, const float r, const float dt)
		{
			return Damp_impl(current, target, r, dt);
		}

		double Damp(const double current, const double target, const double r, const double dt)
		{
			return Damp_impl(current, target, r, dt);
		}

		Float2 Damp(const Float2& current, const Float2& target, const float r, const float dt)
		{
			return Damp_impl(current, target, r, dt);
		}

		Vec2 Damp(const Vec2& current, const Vec2& target, const double r, const double dt)
		{
			return Damp_impl(current, target, r, dt);
		}

		Float3 Damp(const Float3& current, const Float3& target, const float r, const float dt)
		{
			return Damp_impl(current, target, r, dt);
		}

		Vec3 Damp(const Vec3& current, const Vec3& target, const double r, const double dt)
		{
			return Damp_impl(current, target, r, dt);
		}

		Float4 Damp(const Float4& current, const Float4& target, const float r, const float dt)
		{
			return Damp_impl(current, target, r, dt);
		}

		Vec4 Damp(const Vec4& current, const Vec4& target, const double r, const double dt)
		{
			return Damp_impl(current, target, r, dt);
		}

		ColorF Damp(const ColorF& current, const ColorF& target, const double r, const double dt)
		{
			return Damp_impl(current, target, r, dt);
		}

		HSV Damp(const HSV& current, const HSV& target, const double r, const double dt)
		{
			return Damp_impl(current, target, r, dt);
		}

		////////////////////////////////////////////////////////////////
		//
		//	SmoothDamp
		//
		////////////////////////////////////////////////////////////////

		float SmoothDamp(const float current, const float target, float& velocity, const float smoothTime, const Optional<float>& maxSpeed, const float deltaTime)
		{
			return SmoothDamp_impl(current, target, velocity, smoothTime, deltaTime, maxSpeed);
		}

		double SmoothDamp(const double current, const double target, double& velocity, const double smoothTime, const Optional<double>& maxSpeed, const double deltaTime)
		{
			return SmoothDamp_impl(current, target, velocity, smoothTime, deltaTime, maxSpeed);
		}

		Float2 SmoothDamp(const Float2& current, const Float2& target, Float2& velocity, const float smoothTime, const Optional<float>& maxSpeed, const float deltaTime)
		{
			return SmoothDamp_impl(current, target, velocity, smoothTime, deltaTime, maxSpeed);
		}

		Vec2 SmoothDamp(const Vec2& current, const Vec2& target, Vec2& velocity, const double smoothTime, const Optional<double>& maxSpeed, const double deltaTime)
		{
			return SmoothDamp_impl(current, target, velocity, smoothTime, deltaTime, maxSpeed);
		}

		Float3 SmoothDamp(const Float3& current, const Float3& target, Float3& velocity, const float smoothTime, const Optional<float>& maxSpeed, const float deltaTime)
		{
			return SmoothDamp_impl(current, target, velocity, smoothTime, deltaTime, maxSpeed);
		}

		Vec3 SmoothDamp(const Vec3& current, const Vec3& target, Vec3& velocity, const double smoothTime, const Optional<double>& maxSpeed, const double deltaTime)
		{
			return SmoothDamp_impl(current, target, velocity, smoothTime, deltaTime, maxSpeed);
		}

		Float4 SmoothDamp(const Float4& current, const Float4& target, Float4& velocity, const float smoothTime, const Optional<float>& maxSpeed, const float deltaTime)
		{
			return SmoothDamp_impl(current, target, velocity, smoothTime, deltaTime, maxSpeed);
		}

		Vec4 SmoothDamp(const Vec4& current, const Vec4& target, Vec4& velocity, const double smoothTime, const Optional<double>& maxSpeed, const double deltaTime)
		{
			return SmoothDamp_impl(current, target, velocity, smoothTime, deltaTime, maxSpeed);
		}

		ColorF SmoothDamp(const ColorF& current, const ColorF& target, ColorF& velocity, const double smoothTime, const Optional<double>& maxSpeed, const double deltaTime)
		{
			Vec4 vel = velocity.toVec4();

			const Vec4 result = SmoothDamp_impl(current.toVec4(), target.toVec4(), vel, smoothTime, deltaTime, maxSpeed);

			velocity = ColorF{ vel };

			return ColorF{ result };
		}

		HSV SmoothDamp(const HSV& current, const HSV& target, HSV& velocity, const double smoothTime, const Optional<double>& maxSpeed, const double deltaTime)
		{
			const double resultH = SmoothDamp_impl(current.h, target.h, velocity.h, smoothTime, deltaTime, maxSpeed.and_then([](double t)-> Optional<double> { return (t * 360); }));

			Vec3 velocitySVA = velocity.sva();
			const Vec3 resultSVA = SmoothDamp_impl(current.sva(), target.sva(), velocitySVA, smoothTime, deltaTime, maxSpeed);
			velocity.s = velocitySVA.x;
			velocity.v = velocitySVA.y;
			velocity.a = velocitySVA.z;

			return HSV{ resultH, resultSVA.x, resultSVA.y, resultSVA.z };
		}
	}
}
