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
# include "SimdFloat4.hpp"

namespace s3d
{
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	Quaternion
	//
	////////////////////////////////////////////////////////////////

	/// @brief クォータニオン | Quaternion
	struct alignas(16) Quaternion
	{
		/// @brief 要素の型
		using value_type = float;

		/// @brief クォータニオンの成分 `(x, y, z, w)`
		SimdFloat4 value{ 0.0f, 0.0f, 0.0f, 1.0f };

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief 恒等回転を表すクォータニオンを作成します。
		[[nodiscard]]
		Quaternion() noexcept = default;

		/// @brief クォータニオンをコピーして作成します。
		/// @param other コピーするクォータニオン
		[[nodiscard]]
		Quaternion(const Quaternion& other) = default;

		/// @brief 指定した成分を持つクォータニオンを作成します。
		/// @param x x 成分
		/// @param y y 成分
		/// @param z z 成分
		/// @param w w 成分
		[[nodiscard]]
		Quaternion(float x, float y, float z, float w) noexcept;

		/// @brief 指定した成分を持つクォータニオンを作成します。
		/// @param x x 成分
		/// @param y y 成分
		/// @param z z 成分
		/// @param w w 成分
		[[nodiscard]]
		Quaternion(Concept::Arithmetic auto x, Concept::Arithmetic auto y,
			Concept::Arithmetic auto z, Concept::Arithmetic auto w) noexcept;

		/// @brief 4 要素ベクトルからクォータニオンを作成します。
		/// @param xyzw クォータニオンの成分 `(x, y, z, w)`
		[[nodiscard]]
		explicit Quaternion(Float4 xyzw) noexcept;

		/// @brief SIMD 対応の 4 要素ベクトルからクォータニオンを作成します。
		/// @param xyzw クォータニオンの成分 `(x, y, z, w)`
		[[nodiscard]]
		explicit Quaternion(SimdFloat4 xyzw) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator =
		//
		////////////////////////////////////////////////////////////////

		/// @brief クォータニオンをコピー代入します。
		/// @param other コピーするクォータニオン
		/// @return *this
		Quaternion& operator =(const Quaternion& other) = default;

		////////////////////////////////////////////////////////////////
		//
		//	operator ==
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つのクォータニオンの各成分が等しいかを返します。
		/// @param lhs 一方のクォータニオン
		/// @param rhs もう一方のクォータニオン
		/// @return 各成分が等しい場合 true, それ以外の場合は false
		/// @remark 同じ回転を表す符号の異なるクォータニオンは等しいとは判定されません。
		[[nodiscard]]
		friend bool SIV3D_VECTOR_CALL operator ==(Quaternion lhs, Quaternion rhs) noexcept
		{
			return DirectX::XMQuaternionEqual(lhs.value.vec, rhs.value.vec);
		}

		////////////////////////////////////////////////////////////////
		//
		//	epsilonEquals
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したクォータニオンとの各成分の差の絶対値が epsilon 以下であるかを返します。
		/// @param other 別のクォータニオン
		/// @param epsilon 差の絶対値の許容量
		/// @return 各成分の差の絶対値が epsilon 以下である場合 true, それ以外の場合は false
		/// @remark 同じ回転を表す符号の異なるクォータニオンは、各成分をそのまま比較します。
		[[nodiscard]]
		bool SIV3D_VECTOR_CALL epsilonEquals(Quaternion other, float epsilon) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toFloat4
		//
		////////////////////////////////////////////////////////////////

		/// @brief クォータニオンの各成分から Float4 を作成して返します。
		/// @return Float4{ x, y, z, w }
		[[nodiscard]]
		Float4 toFloat4() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	xyz
		//
		////////////////////////////////////////////////////////////////

		/// @brief クォータニオンの x, y, z 成分を返します。
		/// @return Float3{ x, y, z }
		[[nodiscard]]
		Float3 xyz() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getX, getY, getZ, getW
		//
		////////////////////////////////////////////////////////////////

		/// @brief x 成分を返します。
		/// @return x 成分
		[[nodiscard]]
		float getX() const noexcept;

		/// @brief y 成分を返します。
		/// @return y 成分
		[[nodiscard]]
		float getY() const noexcept;

		/// @brief z 成分を返します。
		/// @return z 成分
		[[nodiscard]]
		float getZ() const noexcept;

		/// @brief w 成分を返します。
		/// @return w 成分
		[[nodiscard]]
		float getW() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	dot
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したクォータニオンとの内積を返します。
		/// @param other もう一方のクォータニオン
		/// @return 内積
		[[nodiscard]]
		float SIV3D_VECTOR_CALL dot(Quaternion other) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	length
		//
		////////////////////////////////////////////////////////////////

		/// @brief クォータニオンの長さを返します。
		/// @return クォータニオンの長さ
		[[nodiscard]]
		float length() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	lengthSq
		//
		////////////////////////////////////////////////////////////////

		/// @brief クォータニオンの長さの二乗を返します。
		/// @return クォータニオンの長さの二乗
		/// @remark 平方根を計算しないため `length()` より高速です。
		[[nodiscard]]
		float lengthSq() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hasNaN
		//
		////////////////////////////////////////////////////////////////

		/// @brief 成分に NaN が含まれているかを返します。
		/// @return 成分に NaN が含まれている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool hasNaN() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hasInf
		//
		////////////////////////////////////////////////////////////////

		/// @brief 成分に正または負の無限大が含まれているかを返します。
		/// @return 成分に正または負の無限大が含まれている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool hasInf() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isFinite
		//
		////////////////////////////////////////////////////////////////

		/// @brief すべての成分が有限値であるかを返します。
		/// @return すべての成分が有限値である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isFinite() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isIdentity
		//
		////////////////////////////////////////////////////////////////

		/// @brief 恒等回転を表すクォータニオンであるかを返します。
		/// @return `(0, 0, 0, 1)` である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isIdentity() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isNormalized
		//
		////////////////////////////////////////////////////////////////

		/// @brief 正規化されているかを返します。
		/// @param epsilon 長さの二乗と 1.0 との差の許容量
		/// @return 長さの二乗と 1.0 との差の絶対値が epsilon 以下である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isNormalized(float epsilon = 1e-5f) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	normalize
		//
		////////////////////////////////////////////////////////////////

		/// @brief クォータニオンを正規化します。
		/// @return *this
		/// @remark 長さが 0 の場合、すべての成分が 0 になります。
		Quaternion& SIV3D_VECTOR_CALL normalize() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	normalized
		//
		////////////////////////////////////////////////////////////////

		/// @brief 正規化したクォータニオンを返します。
		/// @return 正規化したクォータニオン
		/// @remark 長さが 0 の場合、すべての成分が 0 のクォータニオンを返します。
		[[nodiscard]]
		Quaternion SIV3D_VECTOR_CALL normalized() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	conjugated
		//
		////////////////////////////////////////////////////////////////

		/// @brief 共役クォータニオンを返します。
		/// @return 共役クォータニオン
		[[nodiscard]]
		Quaternion SIV3D_VECTOR_CALL conjugated() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	inverse
		//
		////////////////////////////////////////////////////////////////

		/// @brief 逆クォータニオンを返します。
		/// @return 逆クォータニオン
		/// @remark 長さの二乗が DirectXMath の `g_XMEpsilon` 以下の場合、すべての成分が 0 のクォータニオンを返します。
		[[nodiscard]]
		Quaternion SIV3D_VECTOR_CALL inverse() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Identity
		//
		////////////////////////////////////////////////////////////////

		/// @brief 恒等回転を表すクォータニオンを返します。
		/// @return `(0, 0, 0, 1)`
		[[nodiscard]]
		static Quaternion SIV3D_VECTOR_CALL Identity() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		/// @brief クォータニオンを出力します。
		/// @tparam CharType 出力ストリームの文字型
		/// @param output 出力ストリーム
		/// @param quaternion クォータニオン
		/// @return 出力ストリーム
		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const Quaternion& quaternion)
		{
			return (output << quaternion.toFloat4());
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator >>
		//
		////////////////////////////////////////////////////////////////

		/// @brief クォータニオンを入力します。
		/// @tparam CharType 入力ストリームの文字型
		/// @param input 入力ストリーム
		/// @param quaternion 入力した値を格納するクォータニオン
		/// @return 入力ストリーム
		template <class CharType>
		friend std::basic_istream<CharType>& operator >>(std::basic_istream<CharType>& input, Quaternion& quaternion)
		{
			Float4 xyzw;
			input >> xyzw;
			quaternion.value.set(xyzw);
			return input;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		/// @brief クォータニオンを文字列に変換します。
		/// @param formatData 文字列バッファ
		/// @param value クォータニオン
		/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
		friend void Formatter(FormatData& formatData, const Quaternion& value);
	};
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

template <>
struct fmt::formatter<s3d::Quaternion>
{
	std::string tag;

	constexpr auto parse(format_parse_context& ctx)
	{
		return s3d::FmtHelper::GetFormatTag(tag, ctx);
	}

	format_context::iterator format(const s3d::Quaternion& value, format_context& ctx) const;
};

template <>
struct fmt::formatter<s3d::Quaternion, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx);

	s3d::BufferContext::iterator format(const s3d::Quaternion& value, s3d::BufferContext& ctx) const;
};

# include "detail/Quaternion.ipp"
