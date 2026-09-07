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
	//	CylindricalBase
	//
	////////////////////////////////////////////////////////////////

	/// @brief 円柱座標
	/// @tparam Float 要素の型
	/// @remark `phi = 0` は X 軸の正方向を表し、正の `phi` は Y 軸の正の回転方向に進みます。
	/// @remark Y 軸の正方向から原点を見たとき、正の `phi` は時計回りです。
	template <Concept::FloatingPoint Float>
	struct CylindricalBase
	{
		/// @brief 要素の型
		using value_type = Float;

		/// @brief 表現する三次元座標の型
		using position_type = Vector3D<value_type>;

		/// @brief 要素をまとめたベクトル型
		using vector_type = Vector3D<value_type>;

		/// @brief Y 軸からの距離
		value_type r;

		/// @brief Y 軸周りの方位角（ラジアン）
		value_type phi;

		/// @brief Y 座標
		value_type y;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		CylindricalBase() = default;

		/// @brief 円柱座標を作成します。
		/// @param _r Y 軸からの距離
		/// @param _phi Y 軸周りの方位角（ラジアン）
		/// @param _y Y 座標
		[[nodiscard]]
		constexpr CylindricalBase(value_type _r, value_type _phi, value_type _y) noexcept;

		/// @brief 円柱座標を作成します。
		/// @param _r Y 軸からの距離
		/// @param _phi Y 軸周りの方位角（ラジアン）
		/// @param _y Y 座標
		[[nodiscard]]
		constexpr CylindricalBase(Concept::Arithmetic auto _r, Concept::Arithmetic auto _phi, Concept::Arithmetic auto _y) noexcept;

		/// @brief 円柱座標を作成します。
		/// @param _r Y 軸からの距離
		/// @param _phi Y 軸周りの方位角（ラジアン）
		/// @param _y Y 座標
		[[nodiscard]]
		constexpr CylindricalBase(Arg::r_<value_type> _r, Arg::phi_<value_type> _phi, Arg::y_<value_type> _y) noexcept;

		/// @brief 円柱座標を作成します。
		/// @param _r Y 軸からの距離
		/// @param _y Y 座標
		/// @param _phi Y 軸周りの方位角（ラジアン）
		[[nodiscard]]
		constexpr CylindricalBase(Arg::r_<value_type> _r, Arg::y_<value_type> _y, Arg::phi_<value_type> _phi) noexcept;

		/// @brief 円柱座標を作成します。
		/// @param _phi Y 軸周りの方位角（ラジアン）
		/// @param _r Y 軸からの距離
		/// @param _y Y 座標
		[[nodiscard]]
		constexpr CylindricalBase(Arg::phi_<value_type> _phi, Arg::r_<value_type> _r, Arg::y_<value_type> _y) noexcept;

		/// @brief 円柱座標を作成します。
		/// @param _phi Y 軸周りの方位角（ラジアン）
		/// @param _y Y 座標
		/// @param _r Y 軸からの距離
		[[nodiscard]]
		constexpr CylindricalBase(Arg::phi_<value_type> _phi, Arg::y_<value_type> _y, Arg::r_<value_type> _r) noexcept;

		/// @brief 円柱座標を作成します。
		/// @param _y Y 座標
		/// @param _r Y 軸からの距離
		/// @param _phi Y 軸周りの方位角（ラジアン）
		[[nodiscard]]
		constexpr CylindricalBase(Arg::y_<value_type> _y, Arg::r_<value_type> _r, Arg::phi_<value_type> _phi) noexcept;

		/// @brief 円柱座標を作成します。
		/// @param _y Y 座標
		/// @param _phi Y 軸周りの方位角（ラジアン）
		/// @param _r Y 軸からの距離
		[[nodiscard]]
		constexpr CylindricalBase(Arg::y_<value_type> _y, Arg::phi_<value_type> _phi, Arg::r_<value_type> _r) noexcept;

		/// @brief 直交座標から変換して円柱座標を作成します。
		/// @param target 円柱座標に変換する直交座標
		/// @remark `target.x == 0` かつ `target.z == 0` の場合、`phi` は 0 になります。
		[[nodiscard]]
		CylindricalBase(position_type target) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator +, operator -
		//
		////////////////////////////////////////////////////////////////

		/// @brief そのままの円柱座標を返します。
		/// @return 自身のコピー
		[[nodiscard]]
		constexpr CylindricalBase operator +() const noexcept;

		/// @brief 原点を挟んで反対側にある円柱座標を返します。
		/// @return 反対側の円柱座標
		[[nodiscard]]
		constexpr CylindricalBase operator -() const noexcept;

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
		//	withR, withPhi, withY
		//
		////////////////////////////////////////////////////////////////

		/// @brief r 成分のみを変更した新しい円柱座標を返します。
		/// @param _r 新しい r 成分
		/// @return 新しい円柱座標
		[[nodiscard]]
		constexpr CylindricalBase withR(value_type _r) const noexcept;

		/// @brief phi 成分のみを変更した新しい円柱座標を返します。
		/// @param _phi 新しい phi 成分
		/// @return 新しい円柱座標
		[[nodiscard]]
		constexpr CylindricalBase withPhi(value_type _phi) const noexcept;

		/// @brief y 成分のみを変更した新しい円柱座標を返します。
		/// @param _y 新しい y 成分
		/// @return 新しい円柱座標
		[[nodiscard]]
		constexpr CylindricalBase withY(value_type _y) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withOffsetR, withOffsetPhi, withOffsetY
		//
		////////////////////////////////////////////////////////////////

		/// @brief r 成分をオフセットした新しい円柱座標を返します。
		/// @param _r r 成分のオフセット量
		/// @return 新しい円柱座標
		[[nodiscard]]
		constexpr CylindricalBase withOffsetR(value_type _r) const noexcept;

		/// @brief phi 成分をオフセットした新しい円柱座標を返します。
		/// @param _phi phi 成分のオフセット量
		/// @return 新しい円柱座標
		[[nodiscard]]
		constexpr CylindricalBase withOffsetPhi(value_type _phi) const noexcept;

		/// @brief y 成分をオフセットした新しい円柱座標を返します。
		/// @param _y y 成分のオフセット量
		/// @return 新しい円柱座標
		[[nodiscard]]
		constexpr CylindricalBase withOffsetY(value_type _y) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotated, rotate
		//
		////////////////////////////////////////////////////////////////

		/// @brief Y 軸周りに回転させた円柱座標を返します。
		/// @param angle 回転する角度（ラジアン）
		/// @return 回転後の円柱座標
		[[nodiscard]]
		constexpr CylindricalBase rotated(value_type angle) const noexcept;

		/// @brief 自身を Y 軸周りに回転させます。
		/// @param angle 回転する角度（ラジアン）
		/// @return *this
		constexpr CylindricalBase& rotate(value_type angle) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	lerp
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの円柱座標間を補間した円柱座標を返します。
		/// @param other もう一方の円柱座標
		/// @param f 補間係数
		/// @return 補間結果
		[[nodiscard]]
		CylindricalBase lerp(const CylindricalBase& other, double f) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toFloat3, toVec3
		//
		////////////////////////////////////////////////////////////////

		/// @brief 円柱座標を Float3 に変換します。
		/// @return 変換された直交座標
		[[nodiscard]]
		Float3 toFloat3() const noexcept;

		/// @brief 円柱座標を Vec3 に変換します。
		/// @return 変換された直交座標
		[[nodiscard]]
		Vec3 toVec3() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	fastToFloat3, fastToVec3
		//
		////////////////////////////////////////////////////////////////

		/// @brief 円柱座標を Float3 に近似で変換します。
		/// @return 変換された直交座標
		/// @remark 小さな誤差が生じる場合があります。
		[[nodiscard]]
		Float3 fastToFloat3() const noexcept;

		/// @brief 円柱座標を Vec3 に近似で変換します。
		/// @return 変換された直交座標
		/// @remark 小さな誤差が生じる場合があります。
		[[nodiscard]]
		Vec3 fastToVec3() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toPosition, operator position_type
		//
		////////////////////////////////////////////////////////////////

		/// @brief 円柱座標を同じ要素型の直交座標に変換します。
		/// @return 変換された直交座標
		[[nodiscard]]
		position_type toPosition() const noexcept;

		/// @brief 円柱座標を同じ要素型の直交座標に変換します。
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
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const CylindricalBase& value)
		{
			return output << CharType('(')
				<< value.r << CharType(',') << CharType(' ')
				<< value.phi << CharType(',') << CharType(' ')
				<< value.y << CharType(')');
		}

		template <class CharType>
		friend std::basic_istream<CharType>& operator >>(std::basic_istream<CharType>& input, CylindricalBase& value)
		{
			CharType unused;
			return input >> unused
				>> value.r >> unused
				>> value.phi >> unused
				>> value.y >> unused;
		}

		/// @brief 円柱座標を文字列に変換します。
		/// @param formatData 文字列バッファ
		/// @param value 円柱座標
		/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
		friend void Formatter(FormatData& formatData, const CylindricalBase& value)
		{
			Formatter(formatData, vector_type{ value.r, value.phi, value.y });
		}
	};

	using Cylindrical	= CylindricalBase<double>;
	using CylindricalF	= CylindricalBase<float>;
}

# include "detail/Cylindrical.ipp"

template <s3d::Concept::FloatingPoint Float>
struct fmt::formatter<s3d::CylindricalBase<Float>, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx)
	{
		return s3d::FmtHelper::GetFormatTag(tag, ctx);
	}

	s3d::BufferContext::iterator format(const s3d::CylindricalBase<Float>& value, s3d::BufferContext& ctx) const
	{
		if (tag.empty())
		{
			return format_to(ctx.out(), U"({}, {}, {})", value.r, value.phi, value.y);
		}
		else
		{
			const std::u32string format = (U"({:" + tag + U"}, {:" + tag + U"}, {:" + tag + U"})");
			return format_to(ctx.out(), format, value.r, value.phi, value.y);
		}
	}
};

template <s3d::Concept::FloatingPoint Float>
struct std::hash<s3d::CylindricalBase<Float>>
{
	[[nodiscard]]
	size_t operator ()(const s3d::CylindricalBase<Float>& value) const noexcept
	{
		return value.hash();
	}
};
