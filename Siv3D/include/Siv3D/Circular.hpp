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
# include "Common.hpp"
# include "PointVector.hpp"
# include "FastMath.hpp"
# include "PredefinedNamedParameter.hpp"
# include "Interpolation.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	CircularBase
	//
	////////////////////////////////////////////////////////////////

	/// @brief 円座標
	template <Concept::FloatingPoint Float, int32 Oclock = 0>
	struct CircularBase
	{
		using value_type = Float;

		using position_type = Vector2D<value_type>;

		/// @brief 半径
		value_type r;

		/// @brief 角度（ラジアン）
		value_type theta;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		CircularBase() = default;

		/// @brief 円座標を作成します。
		/// @param _r 中心からの距離
		/// @param _theta 角度（ラジアン）
		[[nodiscard]]
		constexpr CircularBase(value_type _r, value_type _theta) noexcept;

		/// @brief 円座標を作成します。
		/// @param _r 中心からの距離
		/// @param _theta 角度（ラジアン）
		[[nodiscard]]
		constexpr CircularBase(Concept::Arithmetic auto _r, value_type _theta) noexcept;

		/// @brief 円座標を作成します。
		/// @param _r 中心からの距離
		/// @param _theta 角度（ラジアン）
		[[nodiscard]]
		constexpr CircularBase(Arg::r_<value_type> _r, Arg::theta_<value_type> _theta) noexcept;

		/// @brief 円座標を作成します。
		/// @param _theta 角度（ラジアン）
		/// @param _r 中心からの距離
		[[nodiscard]]
		constexpr CircularBase(Arg::theta_<value_type> _theta, Arg::r_<value_type> _r) noexcept;

		/// @brief 直交座標から変換して円座標を作成します。
		/// @param v 円座標に変換する直交座標
		[[nodiscard]]
		CircularBase(position_type v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator +
		//
		////////////////////////////////////////////////////////////////

		/// @brief そのままの円座標を返します。
		/// @return 自身のコピー
		[[nodiscard]]
		constexpr CircularBase operator +() const noexcept;

		/// @brief 円座標を通常の直交座標に変換し、指定したベクトルを加算した結果を返します。
		/// @param v 加算するベクトル
		/// @return 加算結果
		[[nodiscard]]
		position_type operator +(position_type v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator -
		//
		////////////////////////////////////////////////////////////////

		/// @brief 180° 反対の位置にある円座標を返します。
		/// @return 180° 反対の位置にある円座標
		[[nodiscard]]
		constexpr CircularBase operator -() const noexcept;

		/// @brief 円座標を通常の直交座標に変換し、指定したベクトルを減算した結果を返します。
		/// @param v 減算するベクトル
		/// @return 減算結果
		[[nodiscard]]
		position_type operator -(position_type v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withR
		//
		////////////////////////////////////////////////////////////////

		/// @brief r 成分のみを変更した自身のコピーを返します。
		/// @param _r r 成分
		/// @return r 成分を変更したコピー
		[[nodiscard]]
		constexpr CircularBase withR(value_type _r) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withTheta
		//
		////////////////////////////////////////////////////////////////

		/// @brief theta 成分のみを変更した自身のコピーを返します。
		/// @param _theta theta 成分
		/// @return theta 成分を変更したコピー
		[[nodiscard]]
		constexpr CircularBase withTheta(value_type _theta) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotated
		//
		////////////////////////////////////////////////////////////////

		/// @brief 角度を時計回りに回転させた円座標を返します。
		/// @param angle 回転する角度（ラジアン）
		/// @return 回転後の円座標
		[[nodiscard]]
		constexpr CircularBase rotated(value_type angle) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotate
		//
		////////////////////////////////////////////////////////////////

		/// @brief 自身を指定した角度だけ時計回りに回転させます。
		/// @param angle 回転する角度（ラジアン）
		/// @return *this
		constexpr CircularBase& rotate(value_type angle) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	lerp
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの円座標間を線形補間した円座標を返します。
		/// @param other もう一方の円座標
		/// @param f 補間係数
		/// @return 補間結果
		[[nodiscard]]
		CircularBase lerp(const CircularBase& other, double f) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toFloat2, toVec2
		//
		////////////////////////////////////////////////////////////////

		/// @brief 円座標を通常の直交座標に変換します。
		/// @return 直交座標
		[[nodiscard]]
		Float2 toFloat2() const noexcept;

		/// @brief 円座標を通常の直交座標に変換します。
		/// @return 直交座標
		[[nodiscard]]
		Vec2 toVec2() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	fastToFloat2, fastToVec2
		//
		////////////////////////////////////////////////////////////////

		/// @brief 円座標を通常の直交座標に近似で変換します。
		/// @return 直交座標
		/// @remark 小さな誤差が生じる場合があります。
		[[nodiscard]]
		Float2 fastToFloat2() const noexcept;

		/// @brief 円座標を通常の直交座標に近似で変換します。
		/// @return 直交座標
		/// @remark 小さな誤差が生じる場合があります。
		[[nodiscard]]
		Vec2 fastToVec2() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toPosition
		//
		////////////////////////////////////////////////////////////////

		/// @brief 円座標を通常の直交座標に変換します。
		/// @return 直交座標
		[[nodiscard]]
		position_type toPosition() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator position_type
		//
		////////////////////////////////////////////////////////////////

		/// @brief 円座標を通常の直交座標に変換します。
		/// @return 直交座標
		[[nodiscard]]
		operator position_type() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hash
		//
		////////////////////////////////////////////////////////////////

		/// @brief ハッシュ値を返します。
		/// @return ハッシュ値
		[[nodiscard]]
		uint64 hash() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const CircularBase& value)
		{
			return output << CharType('(')
				<< value.r << CharType(',') << CharType(' ')
				<< value.theta << CharType(')');
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator >>
		//
		////////////////////////////////////////////////////////////////

		template <class CharType>
		friend std::basic_istream<CharType>& operator >>(std::basic_istream<CharType>& input, CircularBase& value)
		{
			CharType unused;
			return input >> unused
				>> value.r >> unused
				>> value.theta >> unused;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, const CircularBase& value)
		{
			Formatter(formatData, position_type{ value.r, value.theta });
		}

	private:

		[[nodiscard]]
		static constexpr value_type ClampAngle(value_type theta) noexcept;

		[[nodiscard]]
		static constexpr value_type Clock() noexcept;

		[[nodiscard]]
		static constexpr value_type Offset(value_type theta) noexcept;
	};

	using Circular	= CircularBase<double, 0>;
	using Circular0	= CircularBase<double, 0>;
	using Circular3	= CircularBase<double, 3>;
	using Circular6	= CircularBase<double, 6>;
	using Circular9	= CircularBase<double, 9>;

	using CircularF		= CircularBase<float, 0>;
	using Circular0F	= CircularBase<float, 0>;
	using Circular3F	= CircularBase<float, 3>;
	using Circular6F	= CircularBase<float, 6>;
	using Circular9F	= CircularBase<float, 9>;
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

template <s3d::Concept::FloatingPoint Float, s3d::int32 Oclock>
struct fmt::formatter<s3d::CircularBase<Float, Oclock>, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx)
	{
		return s3d::FmtHelper::GetFormatTag(tag, ctx);
	}

	s3d::BufferContext::iterator format(const s3d::CircularBase<Float, Oclock>& value, s3d::BufferContext& ctx)
	{
		if (tag.empty())
		{
			return format_to(ctx.out(), U"({}, {})", value.r, value.theta);
		}
		else
		{
			const std::u32string format
				= (U"({:" + tag + U"}, {:" + tag + U"})");
			return format_to(ctx.out(), format, value.r, value.theta);
		}
	}
};

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <s3d::Concept::FloatingPoint Float, s3d::int32 Oclock>
struct std::hash<s3d::CircularBase<Float, Oclock>>
{
	[[nodiscard]]
	size_t operator ()(const s3d::CircularBase<Float, Oclock>& value) const noexcept
	{
		return value.hash();
	}
};

# include "detail/Circular.ipp"
