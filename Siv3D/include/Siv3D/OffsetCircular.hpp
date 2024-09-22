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
	//	OffsetCircularBase
	//
	////////////////////////////////////////////////////////////////

	/// @brief オフセット付き円座標
	template <Concept::FloatingPoint Float, int32 Oclock = 0>
	struct OffsetCircularBase
	{
		/// @brief 要素の型
		using value_type	= Float;

		/// @brief 表現する二次元座標の型
		using position_type	= Vector2D<value_type>;

		/// @brief 要素をまとめたベクトル型
		using vector_type	= Vector4D<value_type>;

		/// @brief オフセット座標（中心座標）
		position_type center;

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
		OffsetCircularBase() = default;

		/// @brief オフセット付き円座標を作成します。
		/// @param _center 中心座標
		/// @param _r 中心からの距離
		/// @param _theta 角度（ラジアン）
		[[nodiscard]]
		constexpr OffsetCircularBase(const position_type& _center, value_type _r = 0, value_type _theta = 0) noexcept;

		/// @brief オフセット付き円座標を作成します。
		/// @param _center 中心座標
		/// @param _r 中心からの距離
		/// @param _theta 角度（ラジアン）
		[[nodiscard]]
		constexpr OffsetCircularBase(const position_type& _center, Arg::r_<value_type> _r, Arg::theta_<value_type> _theta) noexcept;

		/// @brief オフセット付き円座標を作成します。
		/// @param _center 中心座標
		/// @param _theta 角度（ラジアン）
		/// @param _r 中心からの距離
		[[nodiscard]]
		constexpr OffsetCircularBase(const position_type& _center, Arg::theta_<value_type> _theta, Arg::r_<value_type> _r) noexcept;

		/// @brief オフセット付き円座標を作成します。
		/// @param _center 中心座標
		/// @param target 目標とする直交座標
		[[nodiscard]]
		OffsetCircularBase(const position_type& _center, const position_type& target) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator +
		//
		////////////////////////////////////////////////////////////////

		/// @brief そのままの円座標を返します。
		/// @return 自身のコピー
		[[nodiscard]]
		constexpr OffsetCircularBase operator +() const noexcept;

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

		/// @brief 中心に対して 180° 反対の位置にある円座標を返します。
		/// @return 中心に対して 180° 反対の位置にある円座標
		[[nodiscard]]
		constexpr OffsetCircularBase operator -() const noexcept;

		[[nodiscard]]
		position_type operator -(position_type v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withCenter
		//
		////////////////////////////////////////////////////////////////

		/// @brief r と theta はそのままで、中心座標を変更した自身のコピーを返します。
		/// @param _center 中心座標
		/// @return 中心座標を変更したコピー
		[[nodiscard]]
		constexpr OffsetCircularBase withCenter(position_type _center) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withBase
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指す直交座標はそのままで、中心座標を変更した自身のコピーを返します。
		/// @param _center 中心座標
		/// @return 中心座標を変更したコピー
		[[nodiscard]]
		OffsetCircularBase withBase(position_type _center) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withR
		//
		////////////////////////////////////////////////////////////////

		/// @brief r 成分のみを変更した自身のコピーを返します。
		/// @param _r r 成分
		/// @return r 成分を変更したコピー
		[[nodiscard]]
		constexpr OffsetCircularBase withR(value_type _r) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withTheta
		//
		////////////////////////////////////////////////////////////////

		/// @brief theta 成分のみを変更した自身のコピーを返します。
		/// @param _theta theta 成分
		/// @return theta 成分を変更したコピー
		[[nodiscard]]
		constexpr OffsetCircularBase withTheta(value_type _theta) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withTarget
		//
		////////////////////////////////////////////////////////////////

		/// @brief 中心座標はそのままで、目標とする直交座標を変更した自身のコピーを返します。
		/// @param target 目標とする直交座標
		/// @return 成分（r と theta）を変更したコピー
		[[nodiscard]]
		constexpr OffsetCircularBase withTarget(position_type target) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setCenter
		//
		////////////////////////////////////////////////////////////////
		
		constexpr OffsetCircularBase& setCenter(value_type x, value_type y) noexcept;

		constexpr OffsetCircularBase& setCenter(position_type _center) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setTarget
		//
		////////////////////////////////////////////////////////////////

		OffsetCircularBase& setTarget(value_type x, value_type y) noexcept;

		OffsetCircularBase& setTarget(position_type target) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	movedBy
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr OffsetCircularBase movedBy(value_type x, value_type y) const noexcept;

		[[nodiscard]]
		constexpr OffsetCircularBase movedBy(position_type v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	moveBy
		//
		////////////////////////////////////////////////////////////////

		constexpr OffsetCircularBase& moveBy(value_type x, value_type y) noexcept;

		constexpr OffsetCircularBase& moveBy(position_type v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotated
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr OffsetCircularBase rotated(value_type angle) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotate
		//
		////////////////////////////////////////////////////////////////

		constexpr OffsetCircularBase& rotate(value_type angle) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	lerp
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つのオフセット付き円座標間を線形補間したオフセット付き円座標を返します。
		/// @param other もう一方のオフセット付き円座標
		/// @param f 補間係数
		/// @return 補間結果
		[[nodiscard]]
		OffsetCircularBase lerp(const OffsetCircularBase& other, double f) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toFloat2, toVec2
		//
		////////////////////////////////////////////////////////////////

		/// @brief オフセット付き円座標を通常の直交座標に変換します。
		/// @return 直交座標
		[[nodiscard]]
		Float2 toFloat2() const noexcept;

		/// @brief オフセット付き円座標を通常の直交座標に変換します。
		/// @return 直交座標
		[[nodiscard]]
		Vec2 toVec2() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	fastToFloat2, fastToVec2
		//
		////////////////////////////////////////////////////////////////

		/// @brief オフセット付き円座標を通常の直交座標に近似で変換します。
		/// @return 直交座標
		/// @remark 小さな誤差が生じる場合があります。
		[[nodiscard]]
		Float2 fastToFloat2() const noexcept;

		/// @brief オフセット付き円座標を通常の直交座標に近似で変換します。
		/// @return 直交座標
		/// @remark 小さな誤差が生じる場合があります。
		[[nodiscard]]
		Vec2 fastToVec2() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toPosition
		//
		////////////////////////////////////////////////////////////////

		/// @brief オフセット付き円座標を通常の直交座標に変換します。
		/// @return 直交座標
		[[nodiscard]]
		position_type toPosition() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator position_type
		//
		////////////////////////////////////////////////////////////////

		/// @brief オフセット付き円座標を通常の直交座標に変換します。
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
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const OffsetCircularBase& value)
		{
			return output << CharType('(')
				<< value.center.x << CharType(',') << CharType(' ')
				<< value.center.y << CharType(',') << CharType(' ')
				<< value.r << CharType(',') << CharType(' ')
				<< value.theta << CharType(')');
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator >>
		//
		////////////////////////////////////////////////////////////////

		template <class CharType>
		friend std::basic_istream<CharType>& operator >>(std::basic_istream<CharType>& input, OffsetCircularBase& value)
		{
			CharType unused;
			return input >> unused
				>> value.center.x >> unused
				>> value.center.y >> unused
				>> value.r >> unused
				>> value.theta >> unused;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, const OffsetCircularBase& value)
		{
			Formatter(formatData, vector_type{ value.center, value.r, value.theta });
		}

	private:

		[[nodiscard]]
		static constexpr value_type ClampAngle(value_type theta) noexcept;

		[[nodiscard]]
		static constexpr value_type Clock() noexcept;

		[[nodiscard]]
		static constexpr value_type Offset(value_type theta) noexcept;
	};

	using OffsetCircular	= OffsetCircularBase<double, 0>;
	using OffsetCircular0	= OffsetCircularBase<double, 0>;
	using OffsetCircular3	= OffsetCircularBase<double, 3>;
	using OffsetCircular6	= OffsetCircularBase<double, 6>;
	using OffsetCircular9	= OffsetCircularBase<double, 9>;

	using OffsetCircularF	= OffsetCircularBase<float, 0>;
	using OffsetCircular0F	= OffsetCircularBase<float, 0>;
	using OffsetCircular3F	= OffsetCircularBase<float, 3>;
	using OffsetCircular6F	= OffsetCircularBase<float, 6>;
	using OffsetCircular9F	= OffsetCircularBase<float, 9>;
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

template <s3d::Concept::FloatingPoint Float, s3d::int32 Oclock>
struct fmt::formatter<s3d::OffsetCircularBase<Float, Oclock>, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx)
	{
		return s3d::FmtHelper::GetFormatTag(tag, ctx);
	}

	s3d::BufferContext::iterator format(const s3d::OffsetCircularBase<Float, Oclock>& value, s3d::BufferContext& ctx)
	{
		if (tag.empty())
		{
			return format_to(ctx.out(), U"({}, {}, {}, {})", value.center.x, value.center.y, value.r, value.theta);
		}
		else
		{
			const std::u32string format
				= (U"({:" + tag + U"}, {:" + tag + U"}, {:" + tag + U"}, {:" + tag + U"})");
			return format_to(ctx.out(), format, value.center.x, value.center.y, value.r, value.theta);
		}
	}
};

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <s3d::Concept::FloatingPoint Float, s3d::int32 Oclock>
struct std::hash<s3d::OffsetCircularBase<Float, Oclock>>
{
	[[nodiscard]]
	size_t operator ()(const s3d::OffsetCircularBase<Float, Oclock>& value) const noexcept
	{
		return value.hash();
	}
};

# include "detail/OffsetCircular.ipp"
