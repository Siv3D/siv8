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
# include "SIMDMath.hpp"

namespace s3d
{
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	SimdFloat4
	//
	////////////////////////////////////////////////////////////////

	/// @brief SIMD 対応の 4 要素ベクトル | SIMD-enabled 4-element vector
	struct SimdFloat4
	{
		/// @brief ベクトルの内部表現 | Internal representation of the vector
		aligned_float4 vec;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ | Default constructor
		[[nodiscard]]
		SimdFloat4() = default;

		/// @brief コピーコンストラクタ | Copy constructor
		/// @param other コピーするベクトル | Vector to copy
		[[nodiscard]]
		SimdFloat4(const SimdFloat4& other) = default;

		/// @brief 全ての成分が同じ値のベクトルを作成します。 | Creates a vector with all components set to the same value.
		/// @param xyzw 各成分の値 | Value of each component
		[[nodiscard]]
		explicit SimdFloat4(float xyzw) noexcept;

		/// @brief ベクトルを作成します。 | Creates a vector.
		/// @param x x 成分 | x component
		/// @param y y 成分 | y component
		/// @param z z 成分 | z component
		/// @param w w 成分 | w component
		[[nodiscard]]
		SimdFloat4(float x, float y, float z, float w) noexcept;

		/// @brief ベクトルを作成します。 | Creates a vector.
		/// @param _x x 成分 | x component
		/// @param _y y 成分 | y component
		/// @param _z z 成分 | z component
		/// @param _w w 成分 | w component
		[[nodiscard]]
		SimdFloat4(Concept::Arithmetic auto _x, Concept::Arithmetic auto _y, Concept::Arithmetic auto _z, Concept::Arithmetic auto _w) noexcept;

		/// @brief ベクトルを作成します。 | Creates a vector.
		/// @tparam U ベクトルの要素の型 | Element type of the vector
		/// @param xy x, y 成分 | x and y components
		/// @param _z z 成分 | z component
		/// @param _w w 成分 | w component
		template <Concept::FloatingPoint U>
		[[nodiscard]]
		SimdFloat4(const Vector2D<U>& xy, Concept::Arithmetic auto _z, Concept::Arithmetic auto _w) noexcept;

		/// @brief ベクトルを作成します。 | Creates a vector.
		/// @tparam U ベクトルの要素の型 | Element type of the vector
		/// @param _x x 成分 | x component
		/// @param yz y, z 成分 | y and z components
		/// @param _w w 成分 | w component
		template <Concept::FloatingPoint U>
		[[nodiscard]]
		SimdFloat4(Concept::Arithmetic auto _x, const Vector2D<U>& yz, Concept::Arithmetic auto _w) noexcept;

		/// @brief ベクトルを作成します。 | Creates a vector.
		/// @tparam U ベクトルの要素の型 | Element type of the vector
		/// @param _x x 成分 | x component
		/// @param _y y 成分 | y component
		/// @param zw z, w 成分 | z and w components
		template <Concept::FloatingPoint U>
		[[nodiscard]]
		SimdFloat4(Concept::Arithmetic auto _x, Concept::Arithmetic auto _y, const Vector2D<U>& zw) noexcept;

		/// @brief ベクトルを作成します。 | Creates a vector.
		/// @tparam U 一方のベクトルの要素の型 | Element type of one vector
		/// @tparam V もう一方のベクトルの要素の型 | Element type of the other vector
		/// @param xy x, y 成分 | x and y components
		/// @param zw z, w 成分 | z and w components
		template <Concept::FloatingPoint U, Concept::FloatingPoint V>
		[[nodiscard]]
		SimdFloat4(const Vector2D<U>& xy, const Vector2D<V>& zw) noexcept;

		/// @brief ベクトルを作成します。 | Creates a vector.
		/// @tparam U ベクトルの要素の型 | Element type of the vector
		/// @param xyz x, y, z 成分 | x, y, and z components
		/// @param _w w 成分 | w component
		template <Concept::FloatingPoint U>
		[[nodiscard]]
		SimdFloat4(const Vector3D<U>& xyz, Concept::Arithmetic auto _w) noexcept;

		/// @brief ベクトルを作成します。 | Creates a vector.
		/// @tparam U ベクトルの要素の型 | Element type of the vector
		/// @param _x x 成分 | x component
		/// @param yzw y, z, w 成分 | y, z, and w components
		template <Concept::FloatingPoint U>
		[[nodiscard]]
		SimdFloat4(Concept::Arithmetic auto _x, const Vector3D<U>& yzw) noexcept;

		/// @brief ベクトルを作成します。 | Creates a vector.
		/// @param xyzw 各成分の値 | Value of each component
		[[nodiscard]]
		SimdFloat4(Float4 xyzw) noexcept;

		/// @brief ベクトルを作成します。 | Creates a vector.
		/// @param xyzw 各成分の値 | Value of each component
		[[nodiscard]]
		SimdFloat4(Vec4 xyzw) noexcept;

		/// @brief ベクトルを作成します。 | Creates a vector.
		/// @param xyzw 各成分の値 | Value of each component
		[[nodiscard]]
		SimdFloat4(aligned_float4 xyzw) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator +
		//
		////////////////////////////////////////////////////////////////

		/// @brief ベクトルをそのまま返します。 | Returns the vector as is.
		/// @param v ベクトル | Vector
		/// @return 引数と同じベクトル | The same vector as the argument
		[[nodiscard]]
		friend SimdFloat4 SIV3D_VECTOR_CALL operator +(SimdFloat4 v) noexcept
		{
			return v;
		}

		/// @brief 2 つのベクトルの各成分同士を加算した結果を返します。 | Returns the result of adding each component of the two vectors.
		/// @param lhs 一方のベクトル | One vector
		/// @param rhs もう一方のベクトル | The other vector
		/// @return 加算した結果 | The result of the addition
		[[nodiscard]]
		friend SimdFloat4 SIV3D_VECTOR_CALL operator +(SimdFloat4 lhs, SimdFloat4 rhs) noexcept
		{
			return DirectX::XMVectorAdd(lhs.vec, rhs.vec);
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator -
		//
		////////////////////////////////////////////////////////////////

		/// @brief 各成分の符号を反転したベクトルを返します。 | Returns a vector with the sign of each component inverted.
		/// @param v ベクトル | Vector
		/// @return 各成分の符号を反転したベクトル | A vector with the sign of each component inverted
		[[nodiscard]]
		friend SimdFloat4 SIV3D_VECTOR_CALL operator -(SimdFloat4 v) noexcept
		{
			return DirectX::XMVectorNegate(v.vec);
		}

		/// @brief 2 つのベクトルの各成分同士を減算した結果を返します。 | Returns the result of subtracting each component of the two vectors.
		/// @param lhs 一方のベクトル | One vector
		/// @param rhs もう一方のベクトル | The other vector
		/// @return 減算した結果 | The result of the subtraction
		[[nodiscard]]
		friend SimdFloat4 SIV3D_VECTOR_CALL operator -(SimdFloat4 lhs, SimdFloat4 rhs) noexcept
		{
			return DirectX::XMVectorSubtract(lhs.vec, rhs.vec);
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator *
		//
		////////////////////////////////////////////////////////////////

		/// @brief ベクトルの各成分にスカラーを乗算した結果を返します。 | Returns the result of multiplying each component of the vector by a scalar.
		/// @param v ベクトル | Vector
		/// @param s スカラー | Scalar
		/// @return 乗算した結果 | The result of the multiplication
		[[nodiscard]]
		friend SimdFloat4 SIV3D_VECTOR_CALL operator *(SimdFloat4 v, float s) noexcept
		{
			return DirectX::XMVectorScale(v.vec, s);
		}

		/// @brief ベクトルの各成分にスカラーを乗算した結果を返します。 | Returns the result of multiplying each component of the vector by a scalar.
		/// @param s スカラー | Scalar
		/// @param v ベクトル | Vector
		/// @return 乗算した結果 | The result of the multiplication
		[[nodiscard]]
		friend SimdFloat4 SIV3D_VECTOR_CALL operator *(float s, SimdFloat4 v) noexcept
		{
			return DirectX::XMVectorScale(v.vec, s);
		}

		/// @brief 2 つのベクトルの各成分同士を乗算した結果を返します。 | Returns the result of multiplying each component of the two vectors.
		/// @param lhs 一方のベクトル | One vector
		/// @param rhs もう一方のベクトル | The other vector
		/// @return 乗算した結果 | The result of the multiplication
		[[nodiscard]]
		friend SimdFloat4 SIV3D_VECTOR_CALL operator *(SimdFloat4 lhs, SimdFloat4 rhs) noexcept
		{
			return DirectX::XMVectorMultiply(lhs.vec, rhs.vec);
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator /
		//
		////////////////////////////////////////////////////////////////

		/// @brief ベクトルの各成分をスカラーで除算した結果を返します。 | Returns the result of dividing each component of the vector by a scalar.
		/// @param v ベクトル | Vector
		/// @param s スカラー | Scalar
		/// @return 除算した結果 | The result of the division
		[[nodiscard]]
		friend SimdFloat4 SIV3D_VECTOR_CALL operator /(SimdFloat4 v, float s) noexcept
		{
			return DirectX::XMVectorDivide(v.vec, DirectX::XMVectorReplicate(s));
		}

		/// @brief 2 つのベクトルの各成分同士を除算した結果を返します。 | Returns the result of dividing each component of the two vectors.
		/// @param lhs 一方のベクトル | One vector
		/// @param rhs もう一方のベクトル | The other vector
		/// @return 除算した結果 | The result of the division
		[[nodiscard]]
		friend SimdFloat4 SIV3D_VECTOR_CALL operator /(SimdFloat4 lhs, SimdFloat4 rhs) noexcept
		{
			return DirectX::XMVectorDivide(lhs.vec, rhs.vec);
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator +=
		//
		////////////////////////////////////////////////////////////////

		/// @brief ベクトルの各成分を加算します。 | Adds each component of the vector.
		/// @param v 加算するベクトル | Vector to add
		/// @return *this
		SimdFloat4& SIV3D_VECTOR_CALL operator +=(SimdFloat4 v) & noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator -=
		//
		////////////////////////////////////////////////////////////////

		/// @brief ベクトルの各成分を減算します。 | Subtracts each component of the vector.
		/// @param v 減算するベクトル | Vector to subtract
		/// @return *this
		SimdFloat4& SIV3D_VECTOR_CALL operator -=(SimdFloat4 v) & noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator *=
		//
		////////////////////////////////////////////////////////////////

		/// @brief ベクトルの各成分にスカラーを乗算します。 | Multiplies each component of the vector by a scalar.
		/// @param s スカラー | Scalar
		/// @return *this
		SimdFloat4& SIV3D_VECTOR_CALL operator *=(float s) & noexcept;

		/// @brief ベクトルの各成分を乗算します。 | Multiplies each component of the vector.
		/// @param v 乗算するベクトル | Vector to multiply
		/// @return *this
		SimdFloat4& SIV3D_VECTOR_CALL operator *=(SimdFloat4 v) & noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator /=
		//
		////////////////////////////////////////////////////////////////

		/// @brief ベクトルの各成分をスカラーで除算します。 | Divides each component of the vector by a scalar.
		/// @param s スカラー | Scalar
		/// @return *this
		SimdFloat4& SIV3D_VECTOR_CALL operator /=(float s) & noexcept;

		/// @brief ベクトルの各成分を除算します。 | Divides each component of the vector.
		/// @param v 除算するベクトル | Vector to divide
		/// @return *this
		SimdFloat4& SIV3D_VECTOR_CALL operator /=(SimdFloat4 v) & noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator ==
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つのベクトルが等しいかを返します。 | Returns whether the two vectors are equal.
		/// @param lhs 一方のベクトル | One vector
		/// @param rhs もう一方のベクトル | The other vector
		/// @return 2 つのベクトルが等しい場合 true, それ以外の場合は false | true if the two vectors are equal, false otherwise
		[[nodiscard]]
		friend bool SIV3D_VECTOR_CALL operator ==(SimdFloat4 lhs, SimdFloat4 rhs) noexcept
		{
			return DirectX::XMVector4Equal(lhs.vec, rhs.vec);
		}

		////////////////////////////////////////////////////////////////
		//
		//	epsilonEquals
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した別のベクトルとの各成分の差の絶対値が epsilon 以下であるかを返します。 | Returns whether the absolute difference of each component from the specified vector is less than or equal to epsilon.
		/// @param other 別のベクトル | Another vector
		/// @param epsilon 差の絶対値の許容量 | Tolerance for the absolute difference
		/// @return 別のベクトルとの各成分の差の絶対値が epsilon 以下である場合 true, それ以外の場合は false | true if the absolute difference of each component from the other vector is less than or equal to epsilon, false otherwise
		[[nodiscard]]
		bool SIV3D_VECTOR_CALL epsilonEquals(SimdFloat4 other, float epsilon) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator aligned_float4
		//
		////////////////////////////////////////////////////////////////

		/// @brief aligned_float4 に変換します。 | Converts to aligned_float4.
		/// @return 変換した値 | The converted value
		/// @remark DirectXMath の関数にそのまま渡せるようにするための暗黙の変換です。 | This is an implicit conversion so that the value can be passed directly to DirectXMath functions.
		[[nodiscard]]
		operator aligned_float4() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toFloat3
		//
		////////////////////////////////////////////////////////////////

		/// @brief x, y, z 成分から Float3 を作成して返します。 | Creates and returns a Float3 from the x, y, and z components.
		/// @return Float3{ x, y, z }
		[[nodiscard]]
		Float3 toFloat3() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	toFloat4
		//
		////////////////////////////////////////////////////////////////

		/// @brief Float4 を作成して返します。 | Creates and returns a Float4.
		/// @return Float4{ x, y, z, w }
		[[nodiscard]]
		Float4 toFloat4() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	elem
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したインデックスの成分を返します。 | Returns the component at the specified index.
		/// @param index インデックス | Index
		/// @return 指定したインデックスの成分。index が範囲外の場合は 0 | The component at the specified index. 0 if index is out of range
		/// @remark index が 0 の場合は x, 1 の場合は y, 2 の場合は z, 3 の場合は w を返します。 | Returns x if index is 0, y if 1, z if 2, and w if 3.
		[[nodiscard]]
		float elem(size_t index) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getX
		//
		////////////////////////////////////////////////////////////////

		/// @brief x 成分を返します。 | Returns the x component.
		/// @return x 成分 | The x component
		[[nodiscard]]
		float getX() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getY
		//
		////////////////////////////////////////////////////////////////

		/// @brief y 成分を返します。 | Returns the y component.
		/// @return y 成分 | The y component
		[[nodiscard]]
		float getY() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getZ
		//
		////////////////////////////////////////////////////////////////

		/// @brief z 成分を返します。 | Returns the z component.
		/// @return z 成分 | The z component
		[[nodiscard]]
		float getZ() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getW
		//
		////////////////////////////////////////////////////////////////

		/// @brief w 成分を返します。 | Returns the w component.
		/// @return w 成分 | The w component
		[[nodiscard]]
		float getW() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isZero
		//
		////////////////////////////////////////////////////////////////

		/// @brief 全ての成分が 0 であるかを返します。 | Returns whether all components are zero.
		/// @return 全ての成分が 0 である場合 true, それ以外の場合は false | true if all components are zero, false otherwise
		[[nodiscard]]
		bool isZero() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hasNaN
		//
		////////////////////////////////////////////////////////////////

		/// @brief NaN である成分を持つかを返します。 | Returns whether any component is NaN.
		/// @return NaN である成分を持つ場合 true, それ以外の場合は false | true if any component is NaN, false otherwise
		[[nodiscard]]
		bool hasNaN() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hasInf
		//
		////////////////////////////////////////////////////////////////

		/// @brief 無限大である成分を持つかを返します。 | Returns whether any component is infinity.
		/// @return 無限大である成分を持つ場合 true, それ以外の場合は false | true if any component is infinity, false otherwise
		[[nodiscard]]
		bool hasInf() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isFinite
		//
		////////////////////////////////////////////////////////////////

		/// @brief 全ての成分が有限値であるかを返します。 | Returns whether all components are finite.
		/// @return 全ての成分が有限値である場合 true, NaN または無限大である成分を持つ場合は false | true if all components are finite, false if any component is NaN or infinity
		[[nodiscard]]
		bool isFinite() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setZero
		//
		////////////////////////////////////////////////////////////////

		/// @brief 各成分を 0 にセットします。 | Sets all components to zero.
		void setZero() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withX, withY, withZ, withW
		//
		////////////////////////////////////////////////////////////////

		/// @brief x 成分のみを変更した自身のコピーを返します。 | Returns a copy of itself with only the x component changed.
		/// @param _x 新しい x 成分 | New x component
		/// @return x 成分を変更したコピー | A copy with the x component changed
		[[nodiscard]]
		SimdFloat4 SIV3D_VECTOR_CALL withX(float _x) const noexcept;

		/// @brief y 成分のみを変更した自身のコピーを返します。 | Returns a copy of itself with only the y component changed.
		/// @param _y 新しい y 成分 | New y component
		/// @return y 成分を変更したコピー | A copy with the y component changed
		[[nodiscard]]
		SimdFloat4 SIV3D_VECTOR_CALL withY(float _y) const noexcept;

		/// @brief z 成分のみを変更した自身のコピーを返します。 | Returns a copy of itself with only the z component changed.
		/// @param _z 新しい z 成分 | New z component
		/// @return z 成分を変更したコピー | A copy with the z component changed
		[[nodiscard]]
		SimdFloat4 SIV3D_VECTOR_CALL withZ(float _z) const noexcept;

		/// @brief w 成分のみを変更した自身のコピーを返します。 | Returns a copy of itself with only the w component changed.
		/// @param _w 新しい w 成分 | New w component
		/// @return w 成分を変更したコピー | A copy with the w component changed
		[[nodiscard]]
		SimdFloat4 SIV3D_VECTOR_CALL withW(float _w) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setX
		//
		////////////////////////////////////////////////////////////////

		/// @brief x 成分を変更します。 | Changes the x component.
		/// @param _x 新しい x 成分 | New x component
		void setX(float _x) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setY
		//
		////////////////////////////////////////////////////////////////

		/// @brief y 成分を変更します。 | Changes the y component.
		/// @param _y 新しい y 成分 | New y component
		void setY(float _y) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setZ
		//
		////////////////////////////////////////////////////////////////

		/// @brief z 成分を変更します。 | Changes the z component.
		/// @param _z 新しい z 成分 | New z component
		void setZ(float _z) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setW
		//
		////////////////////////////////////////////////////////////////

		/// @brief w 成分を変更します。 | Changes the w component.
		/// @param _w 新しい w 成分 | New w component
		void setW(float _w) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	set
		//
		////////////////////////////////////////////////////////////////

		/// @brief 各成分を変更します。 | Changes all components.
		/// @param _x 新しい x 成分 | New x component
		/// @param _y 新しい y 成分 | New y component
		/// @param _z 新しい z 成分 | New z component
		/// @param _w 新しい w 成分 | New w component
		/// @return *this
		SimdFloat4& set(float _x, float _y, float _z, float _w) noexcept;

		/// @brief 各成分を変更します。 | Changes all components.
		/// @param xyzw 新しい値 | New value
		/// @return *this
		SimdFloat4& set(Float4 xyzw) noexcept;

		/// @brief 各成分を変更します。 | Changes all components.
		/// @param xyzw 新しい値 | New value
		/// @return *this
		SimdFloat4& SIV3D_VECTOR_CALL set(SimdFloat4 xyzw) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	movedBy
		//
		////////////////////////////////////////////////////////////////

		/// @brief 各成分に値を加算した自身のコピーを返します。 | Returns a copy of itself with the values added to each component.
		/// @param _x x 成分の加算量 | Amount to add to the x component
		/// @param _y y 成分の加算量 | Amount to add to the y component
		/// @param _z z 成分の加算量 | Amount to add to the z component
		/// @param _w w 成分の加算量 | Amount to add to the w component
		/// @return 移動後のコピー | The moved copy
		[[nodiscard]]
		SimdFloat4 SIV3D_VECTOR_CALL movedBy(float _x, float _y, float _z, float _w) const noexcept;

		/// @brief 各成分に値を加算した自身のコピーを返します。 | Returns a copy of itself with the values added to each component.
		/// @param v 加算量 | Amount to add
		/// @return 移動後のコピー | The moved copy
		[[nodiscard]]
		SimdFloat4 SIV3D_VECTOR_CALL movedBy(SimdFloat4 v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	moveBy
		//
		////////////////////////////////////////////////////////////////

		/// @brief 各成分に値を加算します。 | Adds the values to each component.
		/// @param _x x 成分の加算量 | Amount to add to the x component
		/// @param _y y 成分の加算量 | Amount to add to the y component
		/// @param _z z 成分の加算量 | Amount to add to the z component
		/// @param _w w 成分の加算量 | Amount to add to the w component
		/// @return *this
		SimdFloat4& SIV3D_VECTOR_CALL moveBy(float _x, float _y, float _z, float _w) noexcept;

		/// @brief 各成分に値を加算します。 | Adds the values to each component.
		/// @param v 加算量 | Amount to add
		/// @return *this
		SimdFloat4& SIV3D_VECTOR_CALL moveBy(SimdFloat4 v) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withOffset
		//
		////////////////////////////////////////////////////////////////

		/// @brief 各成分に値を加算した自身のコピーを返します。 | Returns a copy of itself with the values added to each component.
		/// @param _x x 成分の加算量 | Amount to add to the x component
		/// @param _y y 成分の加算量 | Amount to add to the y component
		/// @param _z z 成分の加算量 | Amount to add to the z component
		/// @param _w w 成分の加算量 | Amount to add to the w component
		/// @return 加算後のコピー | The offset copy
		[[nodiscard]]
		SimdFloat4 SIV3D_VECTOR_CALL withOffset(float _x, float _y, float _z, float _w) const noexcept;

		/// @brief 各成分に値を加算した自身のコピーを返します。 | Returns a copy of itself with the values added to each component.
		/// @param v 加算量 | Amount to add
		/// @return 加算後のコピー | The offset copy
		[[nodiscard]]
		SimdFloat4 SIV3D_VECTOR_CALL withOffset(SimdFloat4 v) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	withOffsetX, withOffsetY, withOffsetZ, withOffsetW
		//
		////////////////////////////////////////////////////////////////

		/// @brief x 成分に値を加算した自身のコピーを返します。 | Returns a copy of itself with the value added to the x component.
		/// @param _x x 成分の加算量 | Amount to add to the x component
		/// @return 加算後のコピー | The offset copy
		[[nodiscard]]
		SimdFloat4 SIV3D_VECTOR_CALL withOffsetX(float _x) const noexcept;

		/// @brief y 成分に値を加算した自身のコピーを返します。 | Returns a copy of itself with the value added to the y component.
		/// @param _y y 成分の加算量 | Amount to add to the y component
		/// @return 加算後のコピー | The offset copy
		[[nodiscard]]
		SimdFloat4 SIV3D_VECTOR_CALL withOffsetY(float _y) const noexcept;

		/// @brief z 成分に値を加算した自身のコピーを返します。 | Returns a copy of itself with the value added to the z component.
		/// @param _z z 成分の加算量 | Amount to add to the z component
		/// @return 加算後のコピー | The offset copy
		[[nodiscard]]
		SimdFloat4 SIV3D_VECTOR_CALL withOffsetZ(float _z) const noexcept;

		/// @brief w 成分に値を加算した自身のコピーを返します。 | Returns a copy of itself with the value added to the w component.
		/// @param _w w 成分の加算量 | Amount to add to the w component
		/// @return 加算後のコピー | The offset copy
		[[nodiscard]]
		SimdFloat4 SIV3D_VECTOR_CALL withOffsetW(float _w) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	dot
		//
		////////////////////////////////////////////////////////////////

		/// @brief ベクトル同士の内積を返します。 | Returns the dot product of two vectors.
		/// @param other もう一方のベクトル | The other vector
		/// @return 内積 | The dot product
		[[nodiscard]]
		float SIV3D_VECTOR_CALL dot(SimdFloat4 other) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	length
		//
		////////////////////////////////////////////////////////////////

		/// @brief ベクトルの大きさ（長さ）を返します。 | Returns the length of the vector.
		/// @return ベクトルの大きさ（長さ） | The length of the vector
		[[nodiscard]]
		float length() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	lengthSq
		//
		////////////////////////////////////////////////////////////////

		/// @brief ベクトルの大きさ（長さ）の二乗を返します。 | Returns the squared length of the vector.
		/// @return ベクトルの大きさ（長さ）の二乗 | The squared length of the vector
		/// @remark 平方根を計算しないため `length()` より高速です。 | Faster than `length()` because it does not compute a square root.
		[[nodiscard]]
		float lengthSq() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	invLength
		//
		////////////////////////////////////////////////////////////////

		/// @brief ベクトルの長さの逆数を返します。 | Returns the reciprocal of the vector length.
		/// @return ベクトルの長さの逆数 | The reciprocal of the vector length
		[[nodiscard]]
		float invLength() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	reciprocal
		//
		////////////////////////////////////////////////////////////////

		/// @brief 各成分の逆数からなるベクトルを返します。 | Returns a vector consisting of the reciprocal of each component.
		/// @return 各成分の逆数からなるベクトル | A vector consisting of the reciprocal of each component
		[[nodiscard]]
		SimdFloat4 SIV3D_VECTOR_CALL reciprocal() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	fastReciprocal
		//
		////////////////////////////////////////////////////////////////

		/// @brief 各成分の逆数の近似値からなるベクトルを返します。 | Returns a vector consisting of an approximation of the reciprocal of each component.
		/// @return 各成分の逆数の近似値からなるベクトル | A vector consisting of an approximation of the reciprocal of each component
		/// @remark `reciprocal()` より高速ですが、精度が低下します。 | Faster than `reciprocal()` but less accurate.
		[[nodiscard]]
		SimdFloat4 SIV3D_VECTOR_CALL fastReciprocal() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	sqrt
		//
		////////////////////////////////////////////////////////////////

		/// @brief 各成分の平方根からなるベクトルを返します。 | Returns a vector consisting of the square root of each component.
		/// @return 各成分の平方根からなるベクトル | A vector consisting of the square root of each component
		[[nodiscard]]
		SimdFloat4 SIV3D_VECTOR_CALL sqrt() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	fastSqrt
		//
		////////////////////////////////////////////////////////////////

		/// @brief 各成分の平方根の近似値からなるベクトルを返します。 | Returns a vector consisting of an approximation of the square root of each component.
		/// @return 各成分の平方根の近似値からなるベクトル | A vector consisting of an approximation of the square root of each component
		/// @remark `sqrt()` より高速ですが、精度が低下します。 | Faster than `sqrt()` but less accurate.
		[[nodiscard]]
		SimdFloat4 SIV3D_VECTOR_CALL fastSqrt() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rsqrt
		//
		////////////////////////////////////////////////////////////////

		/// @brief 各成分の平方根の逆数からなるベクトルを返します。 | Returns a vector consisting of the reciprocal square root of each component.
		/// @return 各成分の平方根の逆数からなるベクトル | A vector consisting of the reciprocal square root of each component
		[[nodiscard]]
		SimdFloat4 SIV3D_VECTOR_CALL rsqrt() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	fastRsqrt
		//
		////////////////////////////////////////////////////////////////

		/// @brief 各成分の平方根の逆数の近似値からなるベクトルを返します。 | Returns a vector consisting of an approximation of the reciprocal square root of each component.
		/// @return 各成分の平方根の逆数の近似値からなるベクトル | A vector consisting of an approximation of the reciprocal square root of each component
		/// @remark `rsqrt()` より高速ですが、精度が低下します。 | Faster than `rsqrt()` but less accurate.
		[[nodiscard]]
		SimdFloat4 SIV3D_VECTOR_CALL fastRsqrt() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	normalize
		//
		////////////////////////////////////////////////////////////////

		/// @brief ベクトルを正規化します（大きさを 1 にします）。 | Normalizes the vector (makes its length 1).
		/// @return *this
		SimdFloat4& SIV3D_VECTOR_CALL normalize() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	normalized
		//
		////////////////////////////////////////////////////////////////

		/// @brief 正規化した（大きさを 1 にした）ベクトルを返します。 | Returns the normalized vector (with a length of 1).
		/// @return 正規化したベクトル | The normalized vector
		[[nodiscard]]
		SimdFloat4 SIV3D_VECTOR_CALL normalized() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	lerp
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別のベクトルとの線形補間を返します。 | Returns the linear interpolation with another vector.
		/// @param other 別のベクトル | Another vector
		/// @param f 補間係数 | Interpolation coefficient
		/// @return 線形補間した結果 | The linearly interpolated result
		[[nodiscard]]
		SimdFloat4 SIV3D_VECTOR_CALL lerp(SimdFloat4 other, float f) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	xy, yz, zw
		//
		////////////////////////////////////////////////////////////////

		/// @brief Float2{ x, y } を返します。 | Returns Float2{ x, y }.
		/// @return Float2{ x, y }
		[[nodiscard]]
		Float2 xy() const noexcept;

		/// @brief Float2{ y, z } を返します。 | Returns Float2{ y, z }.
		/// @return Float2{ y, z }
		[[nodiscard]]
		Float2 yz() const noexcept;

		/// @brief Float2{ z, w } を返します。 | Returns Float2{ z, w }.
		/// @return Float2{ z, w }
		[[nodiscard]]
		Float2 zw() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	xyz, yzw
		//
		////////////////////////////////////////////////////////////////

		/// @brief Float3{ x, y, z } を返します。 | Returns Float3{ x, y, z }.
		/// @return Float3{ x, y, z }
		[[nodiscard]]
		Float3 xyz() const noexcept;

		/// @brief Float3{ y, z, w } を返します。 | Returns Float3{ y, z, w }.
		/// @return Float3{ y, z, w }
		[[nodiscard]]
		Float3 yzw() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	xyz0, xyz1
		//
		////////////////////////////////////////////////////////////////

		/// @brief w 成分を 0 にしたベクトル SimdFloat4{ x, y, z, 0 } を返します。 | Returns SimdFloat4{ x, y, z, 0 } with the w component set to 0.
		/// @return SimdFloat4{ x, y, z, 0 }
		[[nodiscard]]
		SimdFloat4 SIV3D_VECTOR_CALL xyz0() const noexcept;

		/// @brief w 成分を 1 にしたベクトル SimdFloat4{ x, y, z, 1 } を返します。 | Returns SimdFloat4{ x, y, z, 1 } with the w component set to 1.
		/// @return SimdFloat4{ x, y, z, 1 }
		[[nodiscard]]
		SimdFloat4 SIV3D_VECTOR_CALL xyz1() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	xyzw, xxxx, yyyy, zzzz, wwww, wzyx
		//
		////////////////////////////////////////////////////////////////

		/// @brief SimdFloat4{ x, y, z, w } を返します。 | Returns SimdFloat4{ x, y, z, w }.
		/// @return SimdFloat4{ x, y, z, w }
		[[nodiscard]]
		SimdFloat4 SIV3D_VECTOR_CALL xyzw() const noexcept;

		/// @brief SimdFloat4{ x, x, x, x } を返します。 | Returns SimdFloat4{ x, x, x, x }.
		/// @return SimdFloat4{ x, x, x, x }
		[[nodiscard]]
		SimdFloat4 SIV3D_VECTOR_CALL xxxx() const noexcept;

		/// @brief SimdFloat4{ y, y, y, y } を返します。 | Returns SimdFloat4{ y, y, y, y }.
		/// @return SimdFloat4{ y, y, y, y }
		[[nodiscard]]
		SimdFloat4 SIV3D_VECTOR_CALL yyyy() const noexcept;

		/// @brief SimdFloat4{ z, z, z, z } を返します。 | Returns SimdFloat4{ z, z, z, z }.
		/// @return SimdFloat4{ z, z, z, z }
		[[nodiscard]]
		SimdFloat4 SIV3D_VECTOR_CALL zzzz() const noexcept;

		/// @brief SimdFloat4{ w, w, w, w } を返します。 | Returns SimdFloat4{ w, w, w, w }.
		/// @return SimdFloat4{ w, w, w, w }
		[[nodiscard]]
		SimdFloat4 SIV3D_VECTOR_CALL wwww() const noexcept;

		/// @brief SimdFloat4{ w, z, y, x } を返します。 | Returns SimdFloat4{ w, z, y, x }.
		/// @return SimdFloat4{ w, z, y, x }
		[[nodiscard]]
		SimdFloat4 SIV3D_VECTOR_CALL wzyx() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Zero
		//
		////////////////////////////////////////////////////////////////

		/// @brief ゼロベクトル SimdFloat4{ 0, 0, 0, 0 } を返します。 | Returns the zero vector SimdFloat4{ 0, 0, 0, 0 }.
		/// @return SimdFloat4{ 0, 0, 0, 0 }
		[[nodiscard]]
		static SimdFloat4 SIV3D_VECTOR_CALL Zero() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	One
		//
		////////////////////////////////////////////////////////////////

		/// @brief 全ての成分が 1 のベクトル SimdFloat4{ 1, 1, 1, 1 } を返します。 | Returns the vector SimdFloat4{ 1, 1, 1, 1 } with all components set to 1.
		/// @return SimdFloat4{ 1, 1, 1, 1 }
		[[nodiscard]]
		static SimdFloat4 SIV3D_VECTOR_CALL One() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	NaN
		//
		////////////////////////////////////////////////////////////////

		/// @brief 全ての成分が NaN のベクトル SimdFloat4{ NaN, NaN, NaN, NaN } を返します。 | Returns the vector SimdFloat4{ NaN, NaN, NaN, NaN } with all components set to NaN.
		/// @return SimdFloat4{ NaN, NaN, NaN, NaN }
		[[nodiscard]]
		static SimdFloat4 SIV3D_VECTOR_CALL NaN() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	All
		//
		////////////////////////////////////////////////////////////////

		/// @brief 全ての成分が value のベクトル SimdFloat4{ value, value, value, value } を返します。 | Returns the vector SimdFloat4{ value, value, value, value } with all components set to value.
		/// @param value 成分の値 | The component value
		/// @return SimdFloat4{ value, value, value, value }
		[[nodiscard]]
		static SimdFloat4 SIV3D_VECTOR_CALL All(float value = 1.0f) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	UnitX, UnitY, UnitZ, UnitW
		//
		////////////////////////////////////////////////////////////////

		/// @brief x 成分が 1 でそれ以外が 0 のベクトル SimdFloat4{ 1, 0, 0, 0 } を返します。 | Returns the vector SimdFloat4{ 1, 0, 0, 0 } whose x component is 1 and the rest are 0.
		/// @return SimdFloat4{ 1, 0, 0, 0 }
		[[nodiscard]]
		static SimdFloat4 SIV3D_VECTOR_CALL UnitX() noexcept;

		/// @brief y 成分が 1 でそれ以外が 0 のベクトル SimdFloat4{ 0, 1, 0, 0 } を返します。 | Returns the vector SimdFloat4{ 0, 1, 0, 0 } whose y component is 1 and the rest are 0.
		/// @return SimdFloat4{ 0, 1, 0, 0 }
		[[nodiscard]]
		static SimdFloat4 SIV3D_VECTOR_CALL UnitY() noexcept;

		/// @brief z 成分が 1 でそれ以外が 0 のベクトル SimdFloat4{ 0, 0, 1, 0 } を返します。 | Returns the vector SimdFloat4{ 0, 0, 1, 0 } whose z component is 1 and the rest are 0.
		/// @return SimdFloat4{ 0, 0, 1, 0 }
		[[nodiscard]]
		static SimdFloat4 SIV3D_VECTOR_CALL UnitZ() noexcept;

		/// @brief w 成分が 1 でそれ以外が 0 のベクトル SimdFloat4{ 0, 0, 0, 1 } を返します。 | Returns the vector SimdFloat4{ 0, 0, 0, 1 } whose w component is 1 and the rest are 0.
		/// @return SimdFloat4{ 0, 0, 0, 1 }
		[[nodiscard]]
		static SimdFloat4 SIV3D_VECTOR_CALL UnitW() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		/// @brief ベクトルを出力します。 | Outputs the vector.
		/// @tparam CharType 出力ストリームの文字型 | Character type of the output stream
		/// @param output 出力ストリーム | Output stream
		/// @param value ベクトル | Vector
		/// @return 出力ストリーム | The output stream
		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const SimdFloat4& value)
		{
			return (output << value.toFloat4());
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator >>
		//
		////////////////////////////////////////////////////////////////

		/// @brief ベクトルを入力します。 | Inputs the vector.
		/// @tparam CharType 入力ストリームの文字型 | Character type of the input stream
		/// @param input 入力ストリーム | Input stream
		/// @param value 入力した値を格納するベクトル | Vector to store the input value
		/// @return 入力ストリーム | The input stream
		template <class CharType>
		friend std::basic_istream<CharType>& operator >>(std::basic_istream<CharType>& input, SimdFloat4& value)
		{
			Float4 t;

			input >> t;

			value.set(t);

			return input;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		/// @brief ベクトルを文字列に変換します。 | Converts the vector to a string.
		/// @param formatData 文字列バッファ | String buffer
		/// @param value ベクトル | Vector
		/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。 | This function is for Format. Users usually do not need to call it directly.
		friend void Formatter(FormatData& formatData, const SimdFloat4& value);
	};
}


////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

template <>
struct fmt::formatter<s3d::SimdFloat4>
{
	std::string tag;

	constexpr auto parse(format_parse_context& ctx)
	{
		return s3d::FmtHelper::GetFormatTag(tag, ctx);
	}

	format_context::iterator format(const s3d::SimdFloat4& value, format_context& ctx) const;
};

template <>
struct fmt::formatter<s3d::SimdFloat4, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx);

	s3d::BufferContext::iterator format(const s3d::SimdFloat4& value, s3d::BufferContext& ctx) const;
};

# include "detail/SimdFloat4.ipp"
