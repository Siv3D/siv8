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
# include <utility>
# include "Common.hpp"
# include "PointVector.hpp"
# include "SimdFloat4.hpp"

namespace s3d
{
	struct FormatData;
	struct Mat4x4;

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
		Quaternion(Concept::Arithmetic auto x, Concept::Arithmetic auto y, Concept::Arithmetic auto z, Concept::Arithmetic auto w) noexcept;

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
		//	operator *
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの回転を合成します。
		/// @param other 後に適用する回転
		/// @return 合成した回転
		/// @remark 戻り値の回転は、`*this` の回転、`other` の回転の順に適用されます。
		[[nodiscard]]
		Quaternion SIV3D_VECTOR_CALL operator *(Quaternion other) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator *=
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在の回転の後に別の回転を合成します。
		/// @param other 後に適用する回転
		/// @return *this
		Quaternion& SIV3D_VECTOR_CALL operator *=(Quaternion other) & noexcept;

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
		//	rotationEquals
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したクォータニオンと同じ回転を表すかを返します。
		/// @param other 別のクォータニオン
		/// @param epsilon 2 つのクォータニオンの内積の絶対値と 1.0 との差の許容量
		/// @return 同じ回転を表す場合 true, それ以外の場合は false
		/// @remark `*this` と other は正規化されている必要があります。
		/// @remark 符号が異なるクォータニオンも同じ回転を表すものとして判定します。
		[[nodiscard]]
		bool SIV3D_VECTOR_CALL rotationEquals(Quaternion other, float epsilon = 1e-5f) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	angleTo
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したクォータニオンが表す回転までの最小回転角を返します。
		/// @param other 別のクォータニオン
		/// @return 2 つの回転間の角度（ラジアン）。範囲は `[0, Math::Pi]`
		/// @remark `*this` と other は正規化されている必要があります。
		/// @remark 符号が異なるクォータニオンも同じ回転を表すものとして扱います。
		[[nodiscard]]
		float SIV3D_VECTOR_CALL angleTo(Quaternion other) const noexcept;

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
		//	fastNormalized
		//
		////////////////////////////////////////////////////////////////

		/// @brief 逆数平方根の近似値を用いて高速に正規化したクォータニオンを返します。
		/// @return 近似的に正規化したクォータニオン
		/// @remark `normalized()` より精度が低い代わりに高速です。
		/// @remark 長さが 0 または無限大の場合、NaN を含むクォータニオンを返します。
		[[nodiscard]]
		Quaternion SIV3D_VECTOR_CALL fastNormalized() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	canonicalize, canonicalized
		//
		////////////////////////////////////////////////////////////////

		/// @brief 同じ回転を表す符号の異なるクォータニオンを一意な符号に揃えます。
		/// @return *this
		/// @remark 成分を `(w, x, y, z)` の順に見たとき、最初の非ゼロ成分が正になるよう、必要に応じて全成分の符号を反転します。
		/// @remark 正規化は行いません。
		/// @remark すべての成分は有限値である必要があります。
		Quaternion& SIV3D_VECTOR_CALL canonicalize() & noexcept;

		/// @brief 同じ回転を表す符号の異なるクォータニオンを一意な符号に揃えたクォータニオンを返します。
		/// @return 一意な符号に揃えたクォータニオン
		/// @remark 成分を `(w, x, y, z)` の順に見たとき、最初の非ゼロ成分が正になるよう、必要に応じて全成分の符号を反転します。
		/// @remark 正規化は行いません。
		/// @remark すべての成分は有限値である必要があります。
		[[nodiscard]]
		Quaternion SIV3D_VECTOR_CALL canonicalized() const noexcept;

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
		//	conjugate
		//
		////////////////////////////////////////////////////////////////

		/// @brief 自身を共役クォータニオンに変更します。
		/// @return *this
		Quaternion& SIV3D_VECTOR_CALL conjugate() & noexcept;

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
		//	rotate
		//
		////////////////////////////////////////////////////////////////

		/// @brief このクォータニオンが表す回転をベクトルに適用します。
		/// @param v 回転させるベクトル
		/// @return 回転後のベクトル
		/// @remark `*this` は正規化されている必要があります。
		[[nodiscard]]
		Float3 SIV3D_VECTOR_CALL rotate(Float3 v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	inverseRotate
		//
		////////////////////////////////////////////////////////////////

		/// @brief このクォータニオンが表す回転の逆回転をベクトルに適用します。
		/// @param v 回転させるベクトル
		/// @return 逆回転後のベクトル
		/// @remark `*this` は正規化されている必要があります。
		[[nodiscard]]
		Float3 SIV3D_VECTOR_CALL inverseRotate(Float3 v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	slerp
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの回転を球面線形補間します。
		/// @param other もう一方の回転
		/// @param t 補間係数。0.0 の場合は `*this`, 1.0 の場合は other
		/// @return 補間した回転
		/// @remark `*this` と other は正規化されている必要があります。
		/// @remark t は `[0.0, 1.0]` の範囲にクランプされません。
		[[nodiscard]]
		Quaternion SIV3D_VECTOR_CALL slerp(Quaternion other, float t) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	nlerp
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの回転を正規化線形補間します。
		/// @param other もう一方の回転
		/// @param t 補間係数。0.0 の場合は `*this`, 1.0 の場合は other と同じ回転
		/// @return 補間した回転
		/// @remark `*this` と other は正規化されている必要があります。
		/// @remark 最短経路となるよう other の符号を選択して線形補間し、結果を正規化します。
		/// @remark t は `[0.0, 1.0]` の範囲にクランプされません。
		[[nodiscard]]
		Quaternion SIV3D_VECTOR_CALL nlerp(Quaternion other, float t) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	log
		//
		////////////////////////////////////////////////////////////////

		/// @brief 単位クォータニオンの自然対数を返します。
		/// @return クォータニオンの自然対数。w 成分は 0
		/// @remark `*this` は正規化されている必要があります。
		[[nodiscard]]
		Quaternion SIV3D_VECTOR_CALL log() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	exp
		//
		////////////////////////////////////////////////////////////////

		/// @brief 純虚クォータニオンの指数関数を返します。
		/// @return クォータニオンの指数関数
		/// @remark `*this` の w 成分は 0 である必要があります。
		[[nodiscard]]
		Quaternion SIV3D_VECTOR_CALL exp() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toAxisAngle
		//
		////////////////////////////////////////////////////////////////

		/// @brief このクォータニオンが表す回転を回転軸と回転角に変換します。
		/// @return 回転軸を表す単位ベクトルと回転角（ラジアン）のペア
		/// @remark `*this` は正規化されている必要があります。
		/// @remark 恒等回転の場合、回転軸としてゼロベクトルを返します。
		[[nodiscard]]
		std::pair<Float3, float> SIV3D_VECTOR_CALL toAxisAngle() const noexcept;

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
		//	FromUnitVectors
		//
		////////////////////////////////////////////////////////////////

		/// @brief 1 つの単位ベクトルを別の単位ベクトルへ回転させるクォータニオンを返します。
		/// @param from 回転前の単位ベクトル
		/// @param to 回転後の単位ベクトル
		/// @return from を to へ回転させるクォータニオン
		/// @remark from と to は単位ベクトルである必要があります。
		/// @remark 回転軸が一意に定まらない場合は、そのうち 1 つを選択します。
		[[nodiscard]]
		static Quaternion SIV3D_VECTOR_CALL FromUnitVectors(const Vec3& from, const Vec3& to) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	FromVectors
		//
		////////////////////////////////////////////////////////////////

		/// @brief 1 つのベクトルの方向を別のベクトルの方向へ回転させるクォータニオンを返します。
		/// @param from 回転前の方向を表すベクトル
		/// @param to 回転後の方向を表すベクトル
		/// @return from の方向を to の方向へ回転させる正規化済みのクォータニオン
		/// @remark from と to は有限な非ゼロベクトルである必要があります。
		/// @remark from と to の長さは回転結果に影響しません。
		/// @remark 回転軸が一意に定まらない場合は、そのうち 1 つを選択します。
		[[nodiscard]]
		static Quaternion SIV3D_VECTOR_CALL FromVectors(const Vec3& from, const Vec3& to) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	FromUnitVectorPairs
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 本の単位ベクトルの組を、別の 2 本の単位ベクトルの組へ回転させるクォータニオンを返します。
		/// @param from 回転前の 2 本の単位ベクトル
		/// @param to 回転後の 2 本の単位ベクトル
		/// @return from の各ベクトルを to の対応するベクトルへ回転させるクォータニオン
		/// @remark 4 本のベクトルは単位ベクトルである必要があります。
		/// @remark from および to の各ペアは、互いに平行でない必要があります。
		/// @remark 厳密な回転が存在するためには、from の 2 本がなす角と to の 2 本がなす角が等しい必要があります。
		[[nodiscard]]
		static Quaternion SIV3D_VECTOR_CALL FromUnitVectorPairs(const std::pair<Vec3, Vec3>& from, const std::pair<Vec3, Vec3>& to) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	RollPitchYaw
		//
		////////////////////////////////////////////////////////////////

		/// @brief ピッチ、ヨー、ロールからクォータニオンを作成します。
		/// @param pitch X 軸周りの回転角（ラジアン）
		/// @param yaw Y 軸周りの回転角（ラジアン）
		/// @param roll Z 軸周りの回転角（ラジアン）
		/// @return 指定した回転を表すクォータニオン
		/// @remark 回転は roll, pitch, yaw の順に適用されます。
		[[nodiscard]]
		static Quaternion SIV3D_VECTOR_CALL RollPitchYaw(float pitch, float yaw, float roll) noexcept;

		/// @brief ピッチ、ヨー、ロールからクォータニオンを作成します。
		/// @param pitchYawRoll X 成分が pitch, Y 成分が yaw, Z 成分が roll の回転角（ラジアン）
		/// @return 指定した回転を表すクォータニオン
		/// @remark 回転は roll, pitch, yaw の順に適用されます。
		[[nodiscard]]
		static Quaternion SIV3D_VECTOR_CALL RollPitchYaw(Float3 pitchYawRoll) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	RotationNormal
		//
		////////////////////////////////////////////////////////////////

		/// @brief 単位軸と回転角からクォータニオンを作成します。
		/// @param normalAxis 回転軸を表す単位ベクトル
		/// @param angle 回転角（ラジアン）
		/// @return 指定した回転を表すクォータニオン
		/// @remark normalAxis は単位ベクトルである必要があります。
		[[nodiscard]]
		static Quaternion SIV3D_VECTOR_CALL RotationNormal(Float3 normalAxis, float angle) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	RotationAxis
		//
		////////////////////////////////////////////////////////////////

		/// @brief 軸と回転角からクォータニオンを作成します。
		/// @param axis 回転軸を表す非ゼロベクトル
		/// @param angle 回転角（ラジアン）
		/// @return 指定した回転を表すクォータニオン
		/// @remark axis はゼロベクトルであってはいけません。
		[[nodiscard]]
		static Quaternion SIV3D_VECTOR_CALL RotationAxis(Float3 axis, float angle) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	RotateX, RotateY, RotateZ
		//
		////////////////////////////////////////////////////////////////

		/// @brief X 軸周りの回転を表すクォータニオンを返します。
		/// @param angle 回転角（ラジアン）
		/// @return X 軸周りの回転を表すクォータニオン
		[[nodiscard]]
		static Quaternion SIV3D_VECTOR_CALL RotateX(float angle) noexcept;

		/// @brief Y 軸周りの回転を表すクォータニオンを返します。
		/// @param angle 回転角（ラジアン）
		/// @return Y 軸周りの回転を表すクォータニオン
		[[nodiscard]]
		static Quaternion SIV3D_VECTOR_CALL RotateY(float angle) noexcept;

		/// @brief Z 軸周りの回転を表すクォータニオンを返します。
		/// @param angle 回転角（ラジアン）
		/// @return Z 軸周りの回転を表すクォータニオン
		[[nodiscard]]
		static Quaternion SIV3D_VECTOR_CALL RotateZ(float angle) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	FromRotationMatrix
		//
		////////////////////////////////////////////////////////////////

		/// @brief 回転行列からクォータニオンを作成します。
		/// @param rotationMatrix 回転行列
		/// @return rotationMatrix が表す回転と同じ回転を表すクォータニオン
		/// @remark rotationMatrix は回転行列である必要があります。
		[[nodiscard]]
		static Quaternion SIV3D_VECTOR_CALL FromRotationMatrix(const Mat4x4& rotationMatrix) noexcept;

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
