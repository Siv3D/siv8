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
# include <cassert>
# include <cmath>
# include "Common.hpp"
# include "Optional.hpp"
# include "CommonFloat.hpp"
# include "MathConstants.hpp"
# include "PointVector.hpp"
# include "ColorHSV.hpp"
# include "Scene.hpp"

namespace s3d
{
	/// @brief const な lerp() メンバ関数によって例外を送出せず補間できる型
	template <class Type>
	concept HasLerp = requires(const Type& t, const Type& u)
	{
		{ t.lerp(u, 0.0) } noexcept -> std::same_as<Type>;
	};

	namespace Math
	{
		////////////////////////////////////////////////////////////////
		//
		//	Lerp
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの値を線形補間します。
		/// @param v1 一方の値
		/// @param v2 もう一方の値
		/// @param f 補間係数
		/// @return 補間結果
		[[nodiscard]]
		constexpr auto Lerp(Concept::Arithmetic auto v1, Concept::Arithmetic auto v2, Concept::FloatingPoint auto f) noexcept;

		/// @brief 2 つの値を線形補間します。
		/// @tparam Type 補間する値の型
		/// @param v1 一方の値
		/// @param v2 もう一方の値
		/// @param f 補間係数
		/// @return 補間結果
		template <HasLerp Type>
		[[nodiscard]]
		constexpr auto Lerp(const Type& v1, const Type& v2, Concept::FloatingPoint auto f) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	InvLerp
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値が区間内のどの位置にあるかを返します。
		/// @param a 区間の始点
		/// @param b 区間の終点。a と異なる値でなければなりません
		/// @param value 位置を調べる値
		/// @return a を 0、b を 1 としたときの value の位置
		[[nodiscard]]
		constexpr auto InvLerp(Concept::FloatingPoint auto a, Concept::FloatingPoint auto b, Concept::FloatingPoint auto value) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	LerpAngle
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの角度を線形補間します。
		/// @param from 一方の角度
		/// @param to もう一方の角度
		/// @param t 補間係数
		/// @return 補間結果
		[[nodiscard]]
		auto LerpAngle(Concept::Arithmetic auto from, Concept::Arithmetic auto to, Concept::FloatingPoint auto t) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Eerp
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの値を指数補間します。
		/// @param a 一方の値。0 以外でなければなりません
		/// @param b もう一方の値。a と同符号または 0 でなければなりません
		/// @param t 補間係数
		/// @return 補間結果
		[[nodiscard]]
		auto Eerp(Concept::Arithmetic auto a, Concept::Arithmetic auto b, Concept::FloatingPoint auto t) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Map
		//
		////////////////////////////////////////////////////////////////

		/// @brief ある区間の値を別の区間の値に変換します。
		/// @param value 変換する値
		/// @param a 変換元区間の始点
		/// @param b 変換元区間の終点。a と異なる値でなければなりません
		/// @param v1 変換先区間の始点
		/// @param v2 変換先区間の終点
		/// @return 変換結果
		[[nodiscard]]
		constexpr auto Map(double value, double a, double b, Concept::Arithmetic auto v1, Concept::Arithmetic auto v2) noexcept;

		/// @brief ある区間の値を別の区間の値に変換します。
		/// @tparam Type 変換先の値の型
		/// @param value 変換する値
		/// @param a 変換元区間の始点
		/// @param b 変換元区間の終点。a と異なる値でなければなりません
		/// @param v1 変換先区間の始点
		/// @param v2 変換先区間の終点
		/// @return 変換結果
		template <HasLerp Type>
		[[nodiscard]]
		constexpr auto Map(double value, double a, double b, const Type& v1, const Type& v2) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	MoveTowards
		//
		////////////////////////////////////////////////////////////////

		/// @brief 目標地点に向かって移動させます。
		/// @param current 現在地
		/// @param target 目標地点
		/// @param maxSpeed 1 回の呼び出しで移動する最大距離
		/// @remark maxSpeed が 0 以下の場合、current を返します。
		/// @return 新しい現在地
		[[nodiscard]]
		constexpr float MoveTowards(float current, float target, float maxSpeed) noexcept;

		/// @brief 目標地点に向かって移動させます。
		/// @param current 現在地
		/// @param target 目標地点
		/// @param maxSpeed 1 回の呼び出しで移動する最大距離
		/// @remark maxSpeed が 0 以下の場合、current を返します。
		/// @return 新しい現在地
		[[nodiscard]]
		constexpr double MoveTowards(double current, double target, double maxSpeed) noexcept;

		/// @brief 目標地点に向かって移動させます。
		/// @param current 現在地
		/// @param target 目標地点
		/// @param maxSpeed 1 回の呼び出しで移動する最大距離
		/// @remark maxSpeed が 0 以下の場合、current を返します。
		/// @return 新しい現在地
		[[nodiscard]]
		Float2 MoveTowards(const Float2& current, const Float2& target, float maxSpeed) noexcept;

		/// @brief 目標地点に向かって移動させます。
		/// @param current 現在地
		/// @param target 目標地点
		/// @param maxSpeed 1 回の呼び出しで移動する最大距離
		/// @remark maxSpeed が 0 以下の場合、current を返します。
		/// @return 新しい現在地
		[[nodiscard]]
		Vec2 MoveTowards(const Vec2& current, const Vec2& target, double maxSpeed) noexcept;

		/// @brief 目標地点に向かって移動させます。
		/// @param current 現在地
		/// @param target 目標地点
		/// @param maxSpeed 1 回の呼び出しで移動する最大距離
		/// @remark maxSpeed が 0 以下の場合、current を返します。
		/// @return 新しい現在地
		[[nodiscard]]
		Float3 MoveTowards(const Float3& current, const Float3& target, float maxSpeed) noexcept;

		/// @brief 目標地点に向かって移動させます。
		/// @param current 現在地
		/// @param target 目標地点
		/// @param maxSpeed 1 回の呼び出しで移動する最大距離
		/// @remark maxSpeed が 0 以下の場合、current を返します。
		/// @return 新しい現在地
		[[nodiscard]]
		Vec3 MoveTowards(const Vec3& current, const Vec3& target, double maxSpeed) noexcept;

		/// @brief 目標地点に向かって移動させます。
		/// @param current 現在地
		/// @param target 目標地点
		/// @param maxSpeed 1 回の呼び出しで移動する最大距離
		/// @remark maxSpeed が 0 以下の場合、current を返します。
		/// @return 新しい現在地
		[[nodiscard]]
		Float4 MoveTowards(const Float4& current, const Float4& target, float maxSpeed) noexcept;

		/// @brief 目標地点に向かって移動させます。
		/// @param current 現在地
		/// @param target 目標地点
		/// @param maxSpeed 1 回の呼び出しで移動する最大距離
		/// @remark maxSpeed が 0 以下の場合、current を返します。
		/// @return 新しい現在地
		[[nodiscard]]
		Vec4 MoveTowards(const Vec4& current, const Vec4& target, double maxSpeed) noexcept;

		/// @brief 目標の色に向かって移動させます。
		/// @param current 現在の色
		/// @param target 目標の色
		/// @param maxSpeed 1 回の呼び出しで移動する最大距離
		/// @remark maxSpeed が 0 以下の場合、current を返します。
		/// @return 新しい現在の色
		[[nodiscard]]
		ColorF MoveTowards(const ColorF& current, const ColorF& target, double maxSpeed) noexcept;

		/// @brief 目標の色に向かって移動させます。
		/// @param current 現在の色
		/// @param target 目標の色
		/// @param maxSpeed 1 回の呼び出しで移動する最大距離
		/// @remark maxSpeed が 0 以下の場合、current を返します。
		/// @return 新しい現在の色
		[[nodiscard]]
		HSV MoveTowards(const HSV& current, const HSV& target, double maxSpeed) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Damp
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		float Damp(float current, float target, float r, float dt);

		[[nodiscard]]
		double Damp(double current, double target, double r, double dt);

		[[nodiscard]]
		Float2 Damp(const Float2& current, const Float2& target, float r, float dt);

		[[nodiscard]]
		Vec2 Damp(const Vec2& current, const Vec2& target, double r, double dt);

		[[nodiscard]]
		Float3 Damp(const Float3& current, const Float3& target, float r, float dt);

		[[nodiscard]]
		Vec3 Damp(const Vec3& current, const Vec3& target, double r, double dt);

		[[nodiscard]]
		Float4 Damp(const Float4& current, const Float4& target, float r, float dt);

		[[nodiscard]]
		Vec4 Damp(const Vec4& current, const Vec4& target, double r, double dt);

		[[nodiscard]]
		ColorF Damp(const ColorF& current, const ColorF& target, double r, double dt);

		[[nodiscard]]
		HSV Damp(const HSV& current, const HSV& target, double r, double dt);

		////////////////////////////////////////////////////////////////
		//
		//	SmoothDamp
		//
		////////////////////////////////////////////////////////////////

		/// @brief 目標地点に向かってスムーズに移動させます。
		/// @param current 現在地
		/// @param target 目標地点
		/// @param velocity 現在の速度
		/// @param smoothTime 平滑化時間（最大速度で目標に向かうときに期待される所要時間）。動く目標を追いかけるときの遅延時間で、小さいと目標に早く到達する
		/// @param maxSpeed 最大速度。無制限の場合は unspecified
		/// @param deltaTime 前回からの経過時間。デフォルトでは Scene::DeltaTime()
		/// @remark smoothTime が 0.0001 未満の場合、0.0001 として扱います。
		/// @remark maxSpeed が 0 未満の場合、0 として扱います。
		/// @remark deltaTime が 0 以下の場合、current を返し、velocity を変更しません。
		/// @remark deltaTime が正で current と target が等しい場合、velocity を 0 にして target を返します。
		/// @return 新しい現在地
		[[nodiscard]]
		float SmoothDamp(float current, float target, float& velocity, float smoothTime, const Optional<float>& maxSpeed = unspecified, float deltaTime = static_cast<float>(Scene::DeltaTime()));

		/// @brief 目標地点に向かってスムーズに移動させます。
		/// @param current 現在地
		/// @param target 目標地点
		/// @param velocity 現在の速度
		/// @param smoothTime 平滑化時間（最大速度で目標に向かうときに期待される所要時間）。動く目標を追いかけるときの遅延時間で、小さいと目標に早く到達する
		/// @param maxSpeed 最大速度。無制限の場合は unspecified
		/// @param deltaTime 前回からの経過時間。デフォルトでは Scene::DeltaTime()
		/// @remark smoothTime が 0.0001 未満の場合、0.0001 として扱います。
		/// @remark maxSpeed が 0 未満の場合、0 として扱います。
		/// @remark deltaTime が 0 以下の場合、current を返し、velocity を変更しません。
		/// @remark deltaTime が正で current と target が等しい場合、velocity を 0 にして target を返します。
		/// @return 新しい現在地	
		[[nodiscard]]
		double SmoothDamp(double current, double target, double& velocity, double smoothTime, const Optional<double>& maxSpeed = unspecified, double deltaTime = Scene::DeltaTime());

		/// @brief 目標地点に向かってスムーズに移動させます。
		/// @param current 現在地
		/// @param target 目標地点
		/// @param velocity 現在の速度
		/// @param smoothTime 平滑化時間（最大速度で目標に向かうときに期待される所要時間）。動く目標を追いかけるときの遅延時間で、小さいと目標に早く到達する
		/// @param maxSpeed 最大速度。無制限の場合は unspecified
		/// @param deltaTime 前回からの経過時間。デフォルトでは Scene::DeltaTime()
		/// @remark smoothTime が 0.0001 未満の場合、0.0001 として扱います。
		/// @remark maxSpeed が 0 未満の場合、0 として扱います。
		/// @remark deltaTime が 0 以下の場合、current を返し、velocity を変更しません。
		/// @remark deltaTime が正で current と target が等しい場合、velocity を 0 にして target を返します。
		/// @return 新しい現在地
		[[nodiscard]]
		Float2 SmoothDamp(const Float2& current, const Float2& target, Float2& velocity, float smoothTime, const Optional<float>& maxSpeed = unspecified, float deltaTime = static_cast<float>(Scene::DeltaTime()));

		/// @brief 目標地点に向かってスムーズに移動させます。
		/// @param current 現在地
		/// @param target 目標地点
		/// @param velocity 現在の速度
		/// @param smoothTime 平滑化時間（最大速度で目標に向かうときに期待される所要時間）。動く目標を追いかけるときの遅延時間で、小さいと目標に早く到達する
		/// @param maxSpeed 最大速度。無制限の場合は unspecified
		/// @param deltaTime 前回からの経過時間。デフォルトでは Scene::DeltaTime()
		/// @remark smoothTime が 0.0001 未満の場合、0.0001 として扱います。
		/// @remark maxSpeed が 0 未満の場合、0 として扱います。
		/// @remark deltaTime が 0 以下の場合、current を返し、velocity を変更しません。
		/// @remark deltaTime が正で current と target が等しい場合、velocity を 0 にして target を返します。
		/// @return 新しい現在地
		[[nodiscard]]
		Vec2 SmoothDamp(const Vec2& current, const Vec2& target, Vec2& velocity, double smoothTime, const Optional<double>& maxSpeed = unspecified, double deltaTime = Scene::DeltaTime());

		/// @brief 目標地点に向かってスムーズに移動させます。
		/// @param current 現在地
		/// @param target 目標地点
		/// @param velocity 現在の速度
		/// @param smoothTime 平滑化時間（最大速度で目標に向かうときに期待される所要時間）。動く目標を追いかけるときの遅延時間で、小さいと目標に早く到達する
		/// @param maxSpeed 最大速度。無制限の場合は unspecified
		/// @param deltaTime 前回からの経過時間。デフォルトでは Scene::DeltaTime()
		/// @remark smoothTime が 0.0001 未満の場合、0.0001 として扱います。
		/// @remark maxSpeed が 0 未満の場合、0 として扱います。
		/// @remark deltaTime が 0 以下の場合、current を返し、velocity を変更しません。
		/// @remark deltaTime が正で current と target が等しい場合、velocity を 0 にして target を返します。
		/// @return 新しい現在地
		[[nodiscard]]
		Float3 SmoothDamp(const Float3& current, const Float3& target, Float3& velocity, float smoothTime, const Optional<float>& maxSpeed = unspecified, float deltaTime = static_cast<float>(Scene::DeltaTime()));

		/// @brief 目標地点に向かってスムーズに移動させます。
		/// @param current 現在地
		/// @param target 目標地点
		/// @param velocity 現在の速度
		/// @param smoothTime 平滑化時間（最大速度で目標に向かうときに期待される所要時間）。動く目標を追いかけるときの遅延時間で、小さいと目標に早く到達する
		/// @param maxSpeed 最大速度。無制限の場合は unspecified
		/// @param deltaTime 前回からの経過時間。デフォルトでは Scene::DeltaTime()
		/// @remark smoothTime が 0.0001 未満の場合、0.0001 として扱います。
		/// @remark maxSpeed が 0 未満の場合、0 として扱います。
		/// @remark deltaTime が 0 以下の場合、current を返し、velocity を変更しません。
		/// @remark deltaTime が正で current と target が等しい場合、velocity を 0 にして target を返します。
		/// @return 新しい現在地
		[[nodiscard]]
		Vec3 SmoothDamp(const Vec3& current, const Vec3& target, Vec3& velocity, double smoothTime, const Optional<double>& maxSpeed = unspecified, double deltaTime = Scene::DeltaTime());

		/// @brief 目標地点に向かってスムーズに移動させます。
		/// @param current 現在地
		/// @param target 目標地点
		/// @param velocity 現在の速度
		/// @param smoothTime 平滑化時間（最大速度で目標に向かうときに期待される所要時間）。動く目標を追いかけるときの遅延時間で、小さいと目標に早く到達する
		/// @param maxSpeed 最大速度。無制限の場合は unspecified
		/// @param deltaTime 前回からの経過時間。デフォルトでは Scene::DeltaTime()
		/// @remark smoothTime が 0.0001 未満の場合、0.0001 として扱います。
		/// @remark maxSpeed が 0 未満の場合、0 として扱います。
		/// @remark deltaTime が 0 以下の場合、current を返し、velocity を変更しません。
		/// @remark deltaTime が正で current と target が等しい場合、velocity を 0 にして target を返します。
		/// @return 新しい現在地
		[[nodiscard]]
		Float4 SmoothDamp(const Float4& current, const Float4& target, Float4& velocity, float smoothTime, const Optional<float>& maxSpeed = unspecified, float deltaTime = static_cast<float>(Scene::DeltaTime()));

		/// @brief 目標地点に向かってスムーズに移動させます。
		/// @param current 現在地
		/// @param target 目標地点
		/// @param velocity 現在の速度
		/// @param smoothTime 平滑化時間（最大速度で目標に向かうときに期待される所要時間）。動く目標を追いかけるときの遅延時間で、小さいと目標に早く到達する
		/// @param maxSpeed 最大速度。無制限の場合は unspecified
		/// @param deltaTime 前回からの経過時間。デフォルトでは Scene::DeltaTime()
		/// @remark smoothTime が 0.0001 未満の場合、0.0001 として扱います。
		/// @remark maxSpeed が 0 未満の場合、0 として扱います。
		/// @remark deltaTime が 0 以下の場合、current を返し、velocity を変更しません。
		/// @remark deltaTime が正で current と target が等しい場合、velocity を 0 にして target を返します。
		/// @return 新しい現在地
		[[nodiscard]]
		Vec4 SmoothDamp(const Vec4& current, const Vec4& target, Vec4& velocity, double smoothTime, const Optional<double>& maxSpeed = unspecified, double deltaTime = Scene::DeltaTime());

		/// @brief 目標の色に向かってスムーズに移動させます。
		/// @param current 現在の色
		/// @param target 目標の色
		/// @param velocity 現在の変化速度
		/// @param smoothTime 平滑化時間（最大速度で目標に向かうときに期待される所要時間）。動く目標を追いかけるときの遅延時間で、小さいと目標に早く到達する
		/// @param maxSpeed 最大速度。無制限の場合は unspecified
		/// @param deltaTime 前回からの経過時間。デフォルトでは Scene::DeltaTime()
		/// @remark smoothTime が 0.0001 未満の場合、0.0001 として扱います。
		/// @remark maxSpeed が 0 未満の場合、0 として扱います。
		/// @remark deltaTime が 0 以下の場合、current を返し、velocity を変更しません。
		/// @remark deltaTime が正で current と target が等しい場合、velocity を 0 にして target を返します。
		/// @return 新しい色
		[[nodiscard]]
		ColorF SmoothDamp(const ColorF& current, const ColorF& target, ColorF& velocity, double smoothTime, const Optional<double>& maxSpeed = unspecified, double deltaTime = Scene::DeltaTime());

		/// @brief 目標の色に向かってスムーズに移動させます。
		/// @param current 現在の色
		/// @param target 目標の色
		/// @param velocity 現在の変化速度
		/// @param smoothTime 平滑化時間（最大速度で目標に向かうときに期待される所要時間）。動く目標を追いかけるときの遅延時間で、小さいと目標に早く到達する
		/// @param maxSpeed 最大速度。無制限の場合は unspecified
		/// @param deltaTime 前回からの経過時間。デフォルトでは Scene::DeltaTime()
		/// @remark smoothTime が 0.0001 未満の場合、0.0001 として扱います。
		/// @remark maxSpeed が 0 未満の場合、0 として扱います。
		/// @remark deltaTime が 0 以下の場合、current を返し、velocity を変更しません。
		/// @remark deltaTime が正で current と target が等しい場合、velocity を 0 にして target を返します。
		/// @return 新しい色
		[[nodiscard]]
		HSV SmoothDamp(const HSV& current, const HSV& target, HSV& velocity, double smoothTime, const Optional<double>& maxSpeed = unspecified, double deltaTime = Scene::DeltaTime());
	}
}

# include "detail/Interpolation.ipp"
