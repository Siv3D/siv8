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
# include <span>
# include "Common.hpp"
# include "PointVector.hpp"
# include "SimdFloat4.hpp"
# include "Quaternion.hpp"

namespace s3d
{
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	Mat4x4
	//
	////////////////////////////////////////////////////////////////

	/// @brief 4x4 行列クラス | 4x4 matrix class
	struct alignas(16) Mat4x4
	{
		/// @brief 要素の型
		using value_type = float;

		/// @brief 行列の内部表現
		DirectX::XMMATRIX value;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief 未初期化の 4x4 行列を作成します。
		/// @remark 行列の各要素は初期化されません。
		[[nodiscard]]
		Mat4x4() noexcept = default;

		/// @brief 4x4 行列をコピーして作成します。
		/// @param other コピーする行列
		[[nodiscard]]
		Mat4x4(const Mat4x4& other) = default;

		/// @brief DirectXMath の行列から 4x4 行列を作成します。
		/// @param matrix DirectXMath の行列
		[[nodiscard]]
		explicit Mat4x4(DirectX::XMMATRIX matrix) noexcept;

		/// @brief 4 本の行ベクトルから 4x4 行列を作成します。
		/// @param row0 第 0 行
		/// @param row1 第 1 行
		/// @param row2 第 2 行
		/// @param row3 第 3 行
		[[nodiscard]]
		Mat4x4(SimdFloat4 row0, SimdFloat4 row1, SimdFloat4 row2, SimdFloat4 row3) noexcept;

		/// @brief 4 本の行ベクトルから 4x4 行列を作成します。
		/// @param row0 第 0 行
		/// @param row1 第 1 行
		/// @param row2 第 2 行
		/// @param row3 第 3 行
		[[nodiscard]]
		Mat4x4(Float4 row0, Float4 row1, Float4 row2, Float4 row3) noexcept;

		/// @brief 16 個の要素から 4x4 行列を作成します。
		/// @param m00 第 0 行第 0 列の要素
		/// @param m01 第 0 行第 1 列の要素
		/// @param m02 第 0 行第 2 列の要素
		/// @param m03 第 0 行第 3 列の要素
		/// @param m10 第 1 行第 0 列の要素
		/// @param m11 第 1 行第 1 列の要素
		/// @param m12 第 1 行第 2 列の要素
		/// @param m13 第 1 行第 3 列の要素
		/// @param m20 第 2 行第 0 列の要素
		/// @param m21 第 2 行第 1 列の要素
		/// @param m22 第 2 行第 2 列の要素
		/// @param m23 第 2 行第 3 列の要素
		/// @param m30 第 3 行第 0 列の要素
		/// @param m31 第 3 行第 1 列の要素
		/// @param m32 第 3 行第 2 列の要素
		/// @param m33 第 3 行第 3 列の要素
		[[nodiscard]]
		Mat4x4(
			float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33) noexcept;

		/// @brief 行優先で並んだ 16 個の要素から 4x4 行列を作成します。
		/// @param elements 行列の要素
		[[nodiscard]]
		explicit Mat4x4(std::span<const float, 16> elements) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator =
		//
		////////////////////////////////////////////////////////////////

		/// @brief 4x4 行列をコピー代入します。
		/// @param other コピーする行列
		/// @return *this
		Mat4x4& operator =(const Mat4x4& other) = default;

		////////////////////////////////////////////////////////////////
		//
		//	operator +
		//
		////////////////////////////////////////////////////////////////

		/// @brief 自身と同じ行列を返します。
		/// @return *this のコピー
		[[nodiscard]]
		Mat4x4 SIV3D_VECTOR_CALL operator +() const noexcept;

		/// @brief 2 つの行列の各要素を加算します。
		/// @param other 加算する行列
		/// @return 加算した行列
		[[nodiscard]]
		Mat4x4 SIV3D_VECTOR_CALL operator +(const Mat4x4& other) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator -
		//
		////////////////////////////////////////////////////////////////

		/// @brief 各要素の符号を反転した行列を返します。
		/// @return 各要素の符号を反転した行列
		[[nodiscard]]
		Mat4x4 SIV3D_VECTOR_CALL operator -() const noexcept;

		/// @brief 2 つの行列の各要素を減算します。
		/// @param other 減算する行列
		/// @return 減算した行列
		[[nodiscard]]
		Mat4x4 SIV3D_VECTOR_CALL operator -(const Mat4x4& other) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator *
		//
		////////////////////////////////////////////////////////////////

		/// @brief 行列の各要素にスカラーを乗算します。
		/// @param s スカラー
		/// @return 乗算した行列
		[[nodiscard]]
		Mat4x4 SIV3D_VECTOR_CALL operator *(float s) const noexcept;

		/// @brief 2 つの行列を乗算します。
		/// @param other 後に適用する変換行列
		/// @return 乗算した行列
		/// @remark 戻り値の変換は、`*this` の変換、other の変換の順に適用されます。
		[[nodiscard]]
		Mat4x4 SIV3D_VECTOR_CALL operator *(const Mat4x4& other) const noexcept;

		/// @brief 行列の各要素にスカラーを乗算します。
		/// @param s スカラー
		/// @param matrix 行列
		/// @return 乗算した行列
		friend Mat4x4 SIV3D_VECTOR_CALL operator *(float s, const Mat4x4& matrix) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator /
		//
		////////////////////////////////////////////////////////////////

		/// @brief 行列の各要素をスカラーで除算します。
		/// @param s スカラー
		/// @return 除算した行列
		[[nodiscard]]
		Mat4x4 SIV3D_VECTOR_CALL operator /(float s) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator +=
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の行列の各要素を加算します。
		/// @param other 加算する行列
		/// @return *this
		Mat4x4& SIV3D_VECTOR_CALL operator +=(const Mat4x4& other) & noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator -=
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の行列の各要素を減算します。
		/// @param other 減算する行列
		/// @return *this
		Mat4x4& SIV3D_VECTOR_CALL operator -=(const Mat4x4& other) & noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator *=
		//
		////////////////////////////////////////////////////////////////

		/// @brief 行列の各要素にスカラーを乗算します。
		/// @param s スカラー
		/// @return *this
		Mat4x4& SIV3D_VECTOR_CALL operator *=(float s) & noexcept;

		/// @brief 現在の変換の後に別の変換を合成します。
		/// @param other 後に適用する変換行列
		/// @return *this
		Mat4x4& SIV3D_VECTOR_CALL operator *=(const Mat4x4& other) & noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator /=
		//
		////////////////////////////////////////////////////////////////

		/// @brief 行列の各要素をスカラーで除算します。
		/// @param s スカラー
		/// @return *this
		Mat4x4& SIV3D_VECTOR_CALL operator /=(float s) & noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator ==
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの行列の各要素が等しいかを返します。
		/// @param lhs 一方の行列
		/// @param rhs もう一方の行列
		/// @return 各要素が等しい場合 true, それ以外の場合は false
		friend bool SIV3D_VECTOR_CALL operator ==(const Mat4x4& lhs, const Mat4x4& rhs) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	epsilonEquals
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した行列との各要素の差の絶対値が epsilon 以下であるかを返します。
		/// @param other 別の行列
		/// @param epsilon 差の絶対値の許容量
		/// @return 各要素の差の絶対値が epsilon 以下である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool SIV3D_VECTOR_CALL epsilonEquals(const Mat4x4& other, float epsilon) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hasNaN
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素に NaN が含まれているかを返します。
		/// @return 要素に NaN が含まれている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool SIV3D_VECTOR_CALL hasNaN() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hasInf
		//
		////////////////////////////////////////////////////////////////

		/// @brief 要素に正または負の無限大が含まれているかを返します。
		/// @return 要素に正または負の無限大が含まれている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool SIV3D_VECTOR_CALL hasInf() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isFinite
		//
		////////////////////////////////////////////////////////////////

		/// @brief すべての要素が有限値であるかを返します。
		/// @return すべての要素が有限値である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool SIV3D_VECTOR_CALL isFinite() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isIdentity
		//
		////////////////////////////////////////////////////////////////

		/// @brief 恒等行列であるかを返します。
		/// @return 恒等行列である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool SIV3D_VECTOR_CALL isIdentity() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isAffine
		//
		////////////////////////////////////////////////////////////////

		/// @brief アフィン変換行列であるかを返します。
		/// @param epsilon 第 0～2 行の w 成分と 0、および第 3 行の w 成分と 1 の差の許容量
		/// @return アフィン変換行列である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool SIV3D_VECTOR_CALL isAffine(float epsilon = 1e-5f) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	decompose
		//
		////////////////////////////////////////////////////////////////

		/// @brief 行列を拡大率、回転、平行移動に分解します。
		/// @param scale 拡大率の格納先
		/// @param rotation 回転を表すクォータニオンの格納先
		/// @param translation 平行移動量の格納先
		/// @return 分解に成功した場合 true, それ以外の場合は false
		bool SIV3D_VECTOR_CALL decompose(Float3& scale, Quaternion& rotation, Float3& translation) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	transposed
		//
		////////////////////////////////////////////////////////////////

		/// @brief 転置行列を返します。
		/// @return 転置行列
		[[nodiscard]]
		Mat4x4 SIV3D_VECTOR_CALL transposed() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	inverse
		//
		////////////////////////////////////////////////////////////////

		/// @brief 逆行列を返します。
		/// @return 逆行列
		/// @remark 行列式が 0 の場合、戻り値の各要素は有限値になるとは限りません。
		[[nodiscard]]
		Mat4x4 SIV3D_VECTOR_CALL inverse() const noexcept;

		/// @brief 逆行列を返し、行列式を取得します。
		/// @param determinant 行列式の格納先
		/// @return 逆行列
		/// @remark 行列式が 0 の場合、戻り値の各要素は有限値になるとは限りません。
		[[nodiscard]]
		Mat4x4 SIV3D_VECTOR_CALL inverse(float& determinant) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	tryInverse
		//
		////////////////////////////////////////////////////////////////

		/// @brief 逆行列を計算します。
		/// @param result 逆行列の格納先
		/// @return 逆行列が存在する場合 true, 行列式が 0 の場合は false
		/// @remark この行列のすべての要素は有限値である必要があります。
		/// @remark false を返した場合、result は変更されません。
		[[nodiscard]]
		bool SIV3D_VECTOR_CALL tryInverse(Mat4x4& result) const noexcept;

		/// @brief 逆行列を計算し、行列式を取得します。
		/// @param result 逆行列の格納先
		/// @param determinant 行列式の格納先
		/// @return 逆行列が存在する場合 true, 行列式が 0 の場合は false
		/// @remark この行列のすべての要素は有限値である必要があります。
		/// @remark false を返した場合、result は変更されません。determinant には 0 が格納されます。
		[[nodiscard]]
		bool SIV3D_VECTOR_CALL tryInverse(Mat4x4& result, float& determinant) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	determinant
		//
		////////////////////////////////////////////////////////////////

		/// @brief 行列式を返します。
		/// @return 行列式
		[[nodiscard]]
		float SIV3D_VECTOR_CALL determinant() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	translated
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在の変換の後に平行移動を適用した行列を返します。
		/// @param x X 軸方向の平行移動量
		/// @param y Y 軸方向の平行移動量
		/// @param z Z 軸方向の平行移動量
		/// @return 平行移動を合成した行列
		[[nodiscard]]
		Mat4x4 SIV3D_VECTOR_CALL translated(float x, float y, float z) const noexcept;

		/// @brief 現在の変換の後に平行移動を適用した行列を返します。
		/// @param offset 平行移動量
		/// @return 平行移動を合成した行列
		[[nodiscard]]
		Mat4x4 SIV3D_VECTOR_CALL translated(Float3 offset) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	scaled
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在の変換の後に原点を中心とする一様な拡大・縮小を適用した行列を返します。
		/// @param scale 拡大率
		/// @return 拡大・縮小を合成した行列
		[[nodiscard]]
		Mat4x4 SIV3D_VECTOR_CALL scaled(float scale) const noexcept;

		/// @brief 現在の変換の後に原点を中心とする各軸方向の拡大・縮小を適用した行列を返します。
		/// @param scale 各軸方向の拡大率
		/// @return 拡大・縮小を合成した行列
		[[nodiscard]]
		Mat4x4 SIV3D_VECTOR_CALL scaled(Float3 scale) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotatedX, rotatedY, rotatedZ
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在の変換の後に X 軸周りの回転を適用した行列を返します。
		/// @param angle 回転角（ラジアン）
		/// @return 回転を合成した行列
		[[nodiscard]]
		Mat4x4 SIV3D_VECTOR_CALL rotatedX(float angle) const noexcept;

		/// @brief 現在の変換の後に Y 軸周りの回転を適用した行列を返します。
		/// @param angle 回転角（ラジアン）
		/// @return 回転を合成した行列
		[[nodiscard]]
		Mat4x4 SIV3D_VECTOR_CALL rotatedY(float angle) const noexcept;

		/// @brief 現在の変換の後に Z 軸周りの回転を適用した行列を返します。
		/// @param angle 回転角（ラジアン）
		/// @return 回転を合成した行列
		[[nodiscard]]
		Mat4x4 SIV3D_VECTOR_CALL rotatedZ(float angle) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotatedRollPitchYaw
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在の変換の後にピッチ、ヨー、ロールによる回転を適用した行列を返します。
		/// @param pitch X 軸周りの回転角（ラジアン）
		/// @param yaw Y 軸周りの回転角（ラジアン）
		/// @param roll Z 軸周りの回転角（ラジアン）
		/// @return 回転を合成した行列
		/// @remark 回転は roll, pitch, yaw の順に適用されます。
		[[nodiscard]]
		Mat4x4 SIV3D_VECTOR_CALL rotatedRollPitchYaw(float pitch, float yaw, float roll) const noexcept;

		/// @brief 現在の変換の後にピッチ、ヨー、ロールによる回転を適用した行列を返します。
		/// @param pitchYawRoll X 成分が pitch, Y 成分が yaw, Z 成分が roll の回転角（ラジアン）
		/// @return 回転を合成した行列
		/// @remark 回転は roll, pitch, yaw の順に適用されます。
		[[nodiscard]]
		Mat4x4 SIV3D_VECTOR_CALL rotatedRollPitchYaw(Float3 pitchYawRoll) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotatedNormal
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在の変換の後に単位軸周りの回転を適用した行列を返します。
		/// @param normalAxis 回転軸を表す単位ベクトル
		/// @param angle 回転角（ラジアン）
		/// @return 回転を合成した行列
		/// @remark normalAxis は単位ベクトルである必要があります。
		[[nodiscard]]
		Mat4x4 SIV3D_VECTOR_CALL rotatedNormal(Float3 normalAxis, float angle) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotatedAxis
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在の変換の後に指定した軸周りの回転を適用した行列を返します。
		/// @param axis 回転軸を表す非ゼロベクトル
		/// @param angle 回転角（ラジアン）
		/// @return 回転を合成した行列
		/// @remark axis はゼロベクトルであってはいけません。
		[[nodiscard]]
		Mat4x4 SIV3D_VECTOR_CALL rotatedAxis(Float3 axis, float angle) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rotated
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在の変換の後にクォータニオンが表す回転を適用した行列を返します。
		/// @param rotation 適用する回転を表す単位クォータニオン
		/// @return 回転を合成した行列
		/// @remark rotation は正規化されている必要があります。
		[[nodiscard]]
		Mat4x4 SIV3D_VECTOR_CALL rotated(Quaternion rotation) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	transform
		//
		////////////////////////////////////////////////////////////////

		/// @brief 4 要素ベクトルを変換します。
		/// @param vector 変換するベクトル
		/// @return 変換後のベクトル
		[[nodiscard]]
		Float4 SIV3D_VECTOR_CALL transform(Float4 vector) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	transformPoint
		//
		////////////////////////////////////////////////////////////////

		/// @brief 3 次元の点を変換します。
		/// @param point 変換する点
		/// @return 変換後の点
		/// @remark 同次座標で変換した後、w 成分による除算を行います。
		[[nodiscard]]
		Float3 SIV3D_VECTOR_CALL transformPoint(Float3 point) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	transformVector
		//
		////////////////////////////////////////////////////////////////

		/// @brief 3 次元ベクトルを変換します。
		/// @param vector 変換するベクトル
		/// @return 変換後のベクトル
		/// @remark 平行移動および同次座標の w 成分による除算は適用されません。
		[[nodiscard]]
		Float3 SIV3D_VECTOR_CALL transformVector(Float3 vector) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	transformPoints
		//
		////////////////////////////////////////////////////////////////

		/// @brief 3 次元の点の配列を変換します。
		/// @param output 変換結果の書き込み先
		/// @param input 変換する点の配列
		/// @param vectorCount 変換する点の個数
		void SIV3D_VECTOR_CALL transformPoints(Float3* output, const Float3* input, size_t vectorCount) const noexcept;

		/// @brief ストライドを指定して 3 次元の点の配列を変換します。
		/// @param output 変換結果の書き込み先
		/// @param outputStrideBytes 出力要素間のバイト数
		/// @param input 変換する点の配列
		/// @param inputStrideBytes 入力要素間のバイト数
		/// @param vectorCount 変換する点の個数
		void SIV3D_VECTOR_CALL transformPoints(
			Float3* output, size_t outputStrideBytes,
			const Float3* input, size_t inputStrideBytes, size_t vectorCount) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	transformVectors
		//
		////////////////////////////////////////////////////////////////

		/// @brief 3 次元ベクトルの配列を変換します。
		/// @param output 変換結果の書き込み先
		/// @param input 変換するベクトルの配列
		/// @param vectorCount 変換するベクトルの個数
		void SIV3D_VECTOR_CALL transformVectors(Float3* output, const Float3* input, size_t vectorCount) const noexcept;

		/// @brief ストライドを指定して 3 次元ベクトルの配列を変換します。
		/// @param output 変換結果の書き込み先
		/// @param outputStrideBytes 出力要素間のバイト数
		/// @param input 変換するベクトルの配列
		/// @param inputStrideBytes 入力要素間のバイト数
		/// @param vectorCount 変換するベクトルの個数
		void SIV3D_VECTOR_CALL transformVectors(
			Float3* output, size_t outputStrideBytes,
			const Float3* input, size_t inputStrideBytes, size_t vectorCount) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Identity
		//
		////////////////////////////////////////////////////////////////

		/// @brief 恒等行列を返します。
		/// @return 恒等行列
		[[nodiscard]]
		static Mat4x4 SIV3D_VECTOR_CALL Identity() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Translate
		//
		////////////////////////////////////////////////////////////////

		/// @brief 平行移動行列を返します。
		/// @param x X 軸方向の平行移動量
		/// @param y Y 軸方向の平行移動量
		/// @param z Z 軸方向の平行移動量
		/// @return 平行移動行列
		[[nodiscard]]
		static Mat4x4 SIV3D_VECTOR_CALL Translate(float x, float y, float z) noexcept;

		/// @brief 平行移動行列を返します。
		/// @param offset 平行移動量
		/// @return 平行移動行列
		[[nodiscard]]
		static Mat4x4 SIV3D_VECTOR_CALL Translate(Float3 offset) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Scale
		//
		////////////////////////////////////////////////////////////////

		/// @brief 原点を中心とする一様な拡大・縮小行列を返します。
		/// @param scale 拡大率
		/// @return 拡大・縮小行列
		[[nodiscard]]
		static Mat4x4 SIV3D_VECTOR_CALL Scale(float scale) noexcept;

		/// @brief 原点を中心とする各軸方向の拡大・縮小行列を返します。
		/// @param scale 各軸方向の拡大率
		/// @return 拡大・縮小行列
		[[nodiscard]]
		static Mat4x4 SIV3D_VECTOR_CALL Scale(Float3 scale) noexcept;

		/// @brief 指定した点を中心とする一様な拡大・縮小行列を返します。
		/// @param scale 拡大率
		/// @param center 拡大・縮小の中心
		/// @return 拡大・縮小行列
		[[nodiscard]]
		static Mat4x4 SIV3D_VECTOR_CALL Scale(float scale, Float3 center) noexcept;

		/// @brief 指定した点を中心とする各軸方向の拡大・縮小行列を返します。
		/// @param scale 各軸方向の拡大率
		/// @param center 拡大・縮小の中心
		/// @return 拡大・縮小行列
		[[nodiscard]]
		static Mat4x4 SIV3D_VECTOR_CALL Scale(Float3 scale, Float3 center) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	RotateX, RotateY, RotateZ
		//
		////////////////////////////////////////////////////////////////

		/// @brief X 軸周りの回転行列を返します。
		/// @param angle 回転角（ラジアン）
		/// @return X 軸周りの回転行列
		[[nodiscard]]
		static Mat4x4 SIV3D_VECTOR_CALL RotateX(float angle) noexcept;

		/// @brief Y 軸周りの回転行列を返します。
		/// @param angle 回転角（ラジアン）
		/// @return Y 軸周りの回転行列
		[[nodiscard]]
		static Mat4x4 SIV3D_VECTOR_CALL RotateY(float angle) noexcept;

		/// @brief Z 軸周りの回転行列を返します。
		/// @param angle 回転角（ラジアン）
		/// @return Z 軸周りの回転行列
		[[nodiscard]]
		static Mat4x4 SIV3D_VECTOR_CALL RotateZ(float angle) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	RollPitchYaw
		//
		////////////////////////////////////////////////////////////////

		/// @brief ピッチ、ヨー、ロールによる回転行列を返します。
		/// @param pitch X 軸周りの回転角（ラジアン）
		/// @param yaw Y 軸周りの回転角（ラジアン）
		/// @param roll Z 軸周りの回転角（ラジアン）
		/// @return 回転行列
		/// @remark 回転は roll, pitch, yaw の順に適用されます。
		[[nodiscard]]
		static Mat4x4 SIV3D_VECTOR_CALL RollPitchYaw(float pitch, float yaw, float roll) noexcept;

		/// @brief ピッチ、ヨー、ロールによる回転行列を返します。
		/// @param pitchYawRoll X 成分が pitch, Y 成分が yaw, Z 成分が roll の回転角（ラジアン）
		/// @return 回転行列
		/// @remark 回転は roll, pitch, yaw の順に適用されます。
		[[nodiscard]]
		static Mat4x4 SIV3D_VECTOR_CALL RollPitchYaw(Float3 pitchYawRoll) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	RotateNormal
		//
		////////////////////////////////////////////////////////////////

		/// @brief 単位軸周りの回転行列を返します。
		/// @param normalAxis 回転軸を表す単位ベクトル
		/// @param angle 回転角（ラジアン）
		/// @return 回転行列
		/// @remark normalAxis は単位ベクトルである必要があります。
		[[nodiscard]]
		static Mat4x4 SIV3D_VECTOR_CALL RotateNormal(Float3 normalAxis, float angle) noexcept;

		/// @brief 指定した点を中心とする単位軸周りの回転行列を返します。
		/// @param normalAxis 回転軸を表す単位ベクトル
		/// @param angle 回転角（ラジアン）
		/// @param center 回転の中心
		/// @return 回転行列
		/// @remark normalAxis は単位ベクトルである必要があります。
		[[nodiscard]]
		static Mat4x4 SIV3D_VECTOR_CALL RotateNormal(Float3 normalAxis, float angle, Float3 center) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	RotateAxis
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した軸周りの回転行列を返します。
		/// @param axis 回転軸を表す非ゼロベクトル
		/// @param angle 回転角（ラジアン）
		/// @return 回転行列
		/// @remark axis はゼロベクトルであってはいけません。
		[[nodiscard]]
		static Mat4x4 SIV3D_VECTOR_CALL RotateAxis(Float3 axis, float angle) noexcept;

		/// @brief 指定した点を中心とする指定軸周りの回転行列を返します。
		/// @param axis 回転軸を表す非ゼロベクトル
		/// @param angle 回転角（ラジアン）
		/// @param center 回転の中心
		/// @return 回転行列
		/// @remark axis はゼロベクトルであってはいけません。
		[[nodiscard]]
		static Mat4x4 SIV3D_VECTOR_CALL RotateAxis(Float3 axis, float angle, Float3 center) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Rotate
		//
		////////////////////////////////////////////////////////////////

		/// @brief クォータニオンが表す回転行列を返します。
		/// @param rotation 回転を表す単位クォータニオン
		/// @return 回転行列
		/// @remark rotation は正規化されている必要があります。
		[[nodiscard]]
		static Mat4x4 SIV3D_VECTOR_CALL Rotate(Quaternion rotation) noexcept;

		/// @brief 指定した点を中心とする、クォータニオンが表す回転行列を返します。
		/// @param rotation 回転を表す単位クォータニオン
		/// @param center 回転の中心
		/// @return 回転行列
		/// @remark rotation は正規化されている必要があります。
		[[nodiscard]]
		static Mat4x4 SIV3D_VECTOR_CALL Rotate(Quaternion rotation, Float3 center) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	AffineTransform
		//
		////////////////////////////////////////////////////////////////

		/// @brief 拡大・縮小、回転、平行移動を合成したアフィン変換行列を返します。
		/// @param scale 各軸方向の拡大率
		/// @param rotation 回転を表す単位クォータニオン
		/// @param translation 平行移動量
		/// @return scale, rotation, translation の順に適用するアフィン変換行列
		/// @remark rotation は正規化されている必要があります。
		[[nodiscard]]
		static Mat4x4 SIV3D_VECTOR_CALL AffineTransform(Float3 scale, Quaternion rotation, Float3 translation) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		/// @brief 4x4 行列を出力します。
		/// @tparam CharType 出力ストリームの文字型
		/// @param output 出力ストリーム
		/// @param matrix 4x4 行列
		/// @return 出力ストリーム
		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const Mat4x4& matrix);

		////////////////////////////////////////////////////////////////
		//
		//	operator >>
		//
		////////////////////////////////////////////////////////////////

		/// @brief 4x4 行列を入力します。
		/// @tparam CharType 入力ストリームの文字型
		/// @param input 入力ストリーム
		/// @param matrix 入力した値を格納する 4x4 行列
		/// @return 入力ストリーム
		template <class CharType>
		friend std::basic_istream<CharType>& operator >>(std::basic_istream<CharType>& input, Mat4x4& matrix);

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, const Mat4x4& value);
	};
}

# include "detail/Mat4x4.ipp"
