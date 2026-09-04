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
# include "Common.hpp"
# include "PointVector.hpp"
# include "FastMath.hpp"
# include "PredefinedNamedParameter.hpp"
# include "Interpolation.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	SphericalBase
	//
	////////////////////////////////////////////////////////////////

	/// @brief 球面座標
	/// @tparam Float 要素の型
	/// @remark `theta` は Y 軸の正方向から測る極角です。
	/// @remark `phi = 0` は X 軸の正方向を表し、正の `phi` は Y 軸の正の回転方向に進みます。
	/// @remark Y 軸の正方向から原点を見たとき、正の `phi` は時計回りです。
	template <Concept::FloatingPoint Float>
	struct SphericalBase
	{
		/// @brief 要素の型
		using value_type = Float;

		/// @brief 表現する三次元座標の型
		using position_type = Vector3D<value_type>;

		/// @brief 要素をまとめたベクトル型
		using vector_type = Vector3D<value_type>;

		/// @brief 原点からの距離
		value_type r;

		/// @brief Y 軸の正方向から測る極角（ラジアン）
		value_type theta;

		/// @brief Y 軸周りの方位角（ラジアン）
		value_type phi;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		SphericalBase() = default;

		/// @brief 球面座標を作成します。
		/// @param _r 原点からの距離
		/// @param _theta Y 軸の正方向から測る極角（ラジアン）
		/// @param _phi Y 軸周りの方位角（ラジアン）
		[[nodiscard]]
		constexpr SphericalBase(value_type _r, value_type _theta, value_type _phi) noexcept;

		/// @brief 球面座標を作成します。
		/// @param _r 原点からの距離
		/// @param _theta Y 軸の正方向から測る極角（ラジアン）
		/// @param _phi Y 軸周りの方位角（ラジアン）
		[[nodiscard]]
		constexpr SphericalBase(Concept::Arithmetic auto _r, Concept::Arithmetic auto _theta, Concept::Arithmetic auto _phi) noexcept;

		/// @brief 球面座標を作成します。
		/// @param _r 原点からの距離
		/// @param _theta Y 軸の正方向から測る極角（ラジアン）
		/// @param _phi Y 軸周りの方位角（ラジアン）
		[[nodiscard]]
		constexpr SphericalBase(Arg::r_<value_type> _r, Arg::theta_<value_type> _theta, Arg::phi_<value_type> _phi) noexcept;

		/// @brief 球面座標を作成します。
		/// @param _r 原点からの距離
		/// @param _phi Y 軸周りの方位角（ラジアン）
		/// @param _theta Y 軸の正方向から測る極角（ラジアン）
		[[nodiscard]]
		constexpr SphericalBase(Arg::r_<value_type> _r, Arg::phi_<value_type> _phi, Arg::theta_<value_type> _theta) noexcept;

		/// @brief 球面座標を作成します。
		/// @param _theta Y 軸の正方向から測る極角（ラジアン）
		/// @param _r 原点からの距離
		/// @param _phi Y 軸周りの方位角（ラジアン）
		[[nodiscard]]
		constexpr SphericalBase(Arg::theta_<value_type> _theta, Arg::r_<value_type> _r, Arg::phi_<value_type> _phi) noexcept;

		/// @brief 球面座標を作成します。
		/// @param _theta Y 軸の正方向から測る極角（ラジアン）
		/// @param _phi Y 軸周りの方位角（ラジアン）
		/// @param _r 原点からの距離
		[[nodiscard]]
		constexpr SphericalBase(Arg::theta_<value_type> _theta, Arg::phi_<value_type> _phi, Arg::r_<value_type> _r) noexcept;

		/// @brief 球面座標を作成します。
		/// @param _phi Y 軸周りの方位角（ラジアン）
		/// @param _r 原点からの距離
		/// @param _theta Y 軸の正方向から測る極角（ラジアン）
		[[nodiscard]]
		constexpr SphericalBase(Arg::phi_<value_type> _phi, Arg::r_<value_type> _r, Arg::theta_<value_type> _theta) noexcept;

		/// @brief 球面座標を作成します。
		/// @param _phi Y 軸周りの方位角（ラジアン）
		/// @param _theta Y 軸の正方向から測る極角（ラジアン）
		/// @param _r 原点からの距離
		[[nodiscard]]
		constexpr SphericalBase(Arg::phi_<value_type> _phi, Arg::theta_<value_type> _theta, Arg::r_<value_type> _r) noexcept;

		/// @brief 直交座標から変換して球面座標を作成します。
		/// @param target 球面座標に変換する直交座標
		/// @remark 原点では `theta` と `phi` が 0 になります。Y 軸上では `phi` が 0 になります。
		[[nodiscard]]
		SphericalBase(position_type target) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator +, operator -
		//
		////////////////////////////////////////////////////////////////

		/// @brief そのままの球面座標を返します。
		/// @return 自身のコピー
		[[nodiscard]]
		constexpr SphericalBase operator +() const noexcept;

		/// @brief 原点を挟んで反対側にある球面座標を返します。
		/// @return 反対側の球面座標
		[[nodiscard]]
		constexpr SphericalBase operator -() const noexcept;

		/// @brief 直交座標に変換してベクトルを加算します。
		/// @param v 加算するベクトル
		/// @return 加算結果
		[[nodiscard]]
		position_type operator +(position_type v) const noexcept;

		/// @brief 直交座標に変換してベクトルを減算します。
		/// @param v 減算するベクトル
		/// @return 減算結果
		[[nodiscard]]
		position_type operator -(position_type v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withR, withTheta, withPhi
		//
		////////////////////////////////////////////////////////////////

		/// @brief r 成分のみを変更した新しい球面座標を返します。
		/// @param _r 新しい r 成分
		/// @return 新しい球面座標
		[[nodiscard]]
		constexpr SphericalBase withR(value_type _r) const noexcept;

		/// @brief theta 成分のみを変更した新しい球面座標を返します。
		/// @param _theta 新しい theta 成分
		/// @return 新しい球面座標
		[[nodiscard]]
		constexpr SphericalBase withTheta(value_type _theta) const noexcept;

		/// @brief phi 成分のみを変更した新しい球面座標を返します。
		/// @param _phi 新しい phi 成分
		/// @return 新しい球面座標
		[[nodiscard]]
		constexpr SphericalBase withPhi(value_type _phi) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withOffsetR, withOffsetTheta, withOffsetPhi
		//
		////////////////////////////////////////////////////////////////

		/// @brief r 成分をオフセットした新しい球面座標を返します。
		/// @param _r r 成分のオフセット量
		/// @return 新しい球面座標
		[[nodiscard]]
		constexpr SphericalBase withOffsetR(value_type _r) const noexcept;

		/// @brief theta 成分をオフセットした新しい球面座標を返します。
		/// @param _theta theta 成分のオフセット量
		/// @return 新しい球面座標
		[[nodiscard]]
		constexpr SphericalBase withOffsetTheta(value_type _theta) const noexcept;

		/// @brief phi 成分をオフセットした新しい球面座標を返します。
		/// @param _phi phi 成分のオフセット量
		/// @return 新しい球面座標
		[[nodiscard]]
		constexpr SphericalBase withOffsetPhi(value_type _phi) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotated, rotate
		//
		////////////////////////////////////////////////////////////////

		/// @brief Y 軸周りに回転させた球面座標を返します。
		/// @param angle 回転する角度（ラジアン）
		/// @return 回転後の球面座標
		[[nodiscard]]
		constexpr SphericalBase rotated(value_type angle) const noexcept;

		/// @brief 自身を Y 軸周りに回転させます。
		/// @param angle 回転する角度（ラジアン）
		/// @return *this
		constexpr SphericalBase& rotate(value_type angle) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	lerp
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの球面座標間を補間した球面座標を返します。
		/// @param other もう一方の球面座標
		/// @param f 補間係数
		/// @return 補間結果
		[[nodiscard]]
		SphericalBase lerp(const SphericalBase& other, double f) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toFloat3, toVec3
		//
		////////////////////////////////////////////////////////////////

		/// @brief 球面座標を Float3 に変換します。
		/// @return 変換された直交座標
		[[nodiscard]]
		Float3 toFloat3() const noexcept;

		/// @brief 球面座標を Vec3 に変換します。
		/// @return 変換された直交座標
		[[nodiscard]]
		Vec3 toVec3() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	fastToFloat3, fastToVec3
		//
		////////////////////////////////////////////////////////////////

		/// @brief 球面座標を Float3 に近似で変換します。
		/// @return 変換された直交座標
		/// @remark 小さな誤差が生じる場合があります。
		[[nodiscard]]
		Float3 fastToFloat3() const noexcept;

		/// @brief 球面座標を Vec3 に近似で変換します。
		/// @return 変換された直交座標
		/// @remark 小さな誤差が生じる場合があります。
		[[nodiscard]]
		Vec3 fastToVec3() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toPosition, operator position_type
		//
		////////////////////////////////////////////////////////////////

		/// @brief 球面座標を同じ要素型の直交座標に変換します。
		/// @return 変換された直交座標
		[[nodiscard]]
		position_type toPosition() const noexcept;

		/// @brief 球面座標を同じ要素型の直交座標に変換します。
		/// @return 変換された直交座標
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

		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const SphericalBase& value)
		{
			return output << CharType('(')
				<< value.r << CharType(',') << CharType(' ')
				<< value.theta << CharType(',') << CharType(' ')
				<< value.phi << CharType(')');
		}

		template <class CharType>
		friend std::basic_istream<CharType>& operator >>(std::basic_istream<CharType>& input, SphericalBase& value)
		{
			CharType unused;
			return input >> unused
				>> value.r >> unused
				>> value.theta >> unused
				>> value.phi >> unused;
		}

		/// @brief 球面座標を文字列に変換します。
		/// @param formatData 文字列バッファ
		/// @param value 球面座標
		/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
		friend void Formatter(FormatData& formatData, const SphericalBase& value)
		{
			Formatter(formatData, vector_type{ value.r, value.theta, value.phi });
		}
	};

	using Spherical		= SphericalBase<double>;
	using SphericalF	= SphericalBase<float>;
}

# include "detail/Spherical.ipp"

template <s3d::Concept::FloatingPoint Float>
struct fmt::formatter<s3d::SphericalBase<Float>, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx)
	{
		return s3d::FmtHelper::GetFormatTag(tag, ctx);
	}

	s3d::BufferContext::iterator format(const s3d::SphericalBase<Float>& value, s3d::BufferContext& ctx) const
	{
		if (tag.empty())
		{
			return format_to(ctx.out(), U"({}, {}, {})", value.r, value.theta, value.phi);
		}
		else
		{
			const std::u32string format = (U"({:" + tag + U"}, {:" + tag + U"}, {:" + tag + U"})");
			return format_to(ctx.out(), format, value.r, value.theta, value.phi);
		}
	}
};

template <s3d::Concept::FloatingPoint Float>
struct std::hash<s3d::SphericalBase<Float>>
{
	[[nodiscard]]
	size_t operator ()(const s3d::SphericalBase<Float>& value) const noexcept
	{
		return value.hash();
	}
};
