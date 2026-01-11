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

# pragma once
# include "Common.hpp"
# include "IEffect.hpp"
# include "Duration.hpp"
# include "AssetHandle.hpp"
# include "PredefinedYesNo.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Effect
	//
	////////////////////////////////////////////////////////////////

	/// @brief エフェクトグループ
	class Effect : public AssetHandle<Effect>
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	DefaultMaxLifeTimeSec
		//
		////////////////////////////////////////////////////////////////

		/// @brief エフェクトの最大継続時間のデフォルト値（秒）
		static constexpr double DefaultMaxLifeTimeSec = 10.0;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief エフェクトグループを作成します。
		/// @param sortingEnabled 描画順序のソートを有効にするか
		[[nodiscard]]
		explicit Effect(SortingEnabled sortingEnabled);

		/// @brief エフェクトグループを作成します。
		/// @param maxLifeTimeSec このエフェクトグループでのエフェクトの最大継続時間（秒）
		/// @param sortingEnabled 描画順序のソートを有効にするか
		[[nodiscard]]
		Effect(double maxLifeTimeSec = DefaultMaxLifeTimeSec, SortingEnabled sortingEnabled = SortingEnabled::Yes);

		/// @brief エフェクトグループを作成します。
		/// @param maxLifeTimeSec このエフェクトグループでのエフェクトの最大継続時間（秒）
		/// @param sortingEnabled 描画順序のソートを有効にするか
		[[nodiscard]]
		Effect(const Duration& maxLifeTimeSec, SortingEnabled sortingEnabled = SortingEnabled::Yes);

		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デストラクタ
		virtual ~Effect();

		////////////////////////////////////////////////////////////////
		//
		//	add
		//
		////////////////////////////////////////////////////////////////

		/// @brief エフェクトグループに新しいエフェクトを追加します
		/// @param effect 追加するエフェクト
		/// @param initialElapsedTimeSec エフェクト追加時点での経過時間オフセット（秒）
		const Effect& add(std::unique_ptr<IEffect>&& effect, double initialElapsedTimeSec = 0.0) const;

		/// @brief エフェクトグループに新しいエフェクトを追加します
		/// @tparam IEffectType 追加するエフェクトの型
		/// @tparam ...Args コンストラクタ引数の型
		/// @param ...args コンストラクタ引数
		template <class IEffectType, class... Args>
			requires std::derived_from<IEffectType, IEffect>
		const Effect& add(Args&&... args) const;

		/// @brief エフェクトグループに新しいエフェクトを追加します
		/// @remark 関数オブジェクトは double 型を受け取り bool 型を返す必要があります。
		/// @tparam Fty エフェクト（関数オブジェクト）の型
		/// @param f エフェクトの関数オブジェクト
		template <class Fty>
			requires std::invocable<Fty, double> &&
					 std::same_as<std::invoke_result_t<Fty, double>, bool>
		const Effect& add(Fty&& f) const;

		////////////////////////////////////////////////////////////////
		//
		//	operator bool
		//
		////////////////////////////////////////////////////////////////

		/// @brief エフェクトグループがアクティブなエフェクトを持っているかを返します。
		/// @remark `Effect::hasEffects()` と同じ結果を返します。
		/// @return アクティブなエフェクトがある場合 true, それ以外の場合は false
		[[nodiscard]]
		explicit operator bool() const;

		////////////////////////////////////////////////////////////////
		//
		//	isEmpty
		//
		////////////////////////////////////////////////////////////////

		/// @brief エフェクトグループがアクティブなエフェクトを持っていないかを返します。
		/// @return アクティブなエフェクトがない場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isEmpty() const;

		////////////////////////////////////////////////////////////////
		//
		//	size
		//
		////////////////////////////////////////////////////////////////

		/// @brief エフェクトグループでアクティブなエフェクトの個数を返します。
		/// @return アクティブなエフェクトの個数
		[[nodiscard]]
		size_t size() const;

		////////////////////////////////////////////////////////////////
		//
		//	pause
		//
		////////////////////////////////////////////////////////////////

		/// @brief このエフェクトグループの時間経過を一時停止します。
		void pause() const;

		////////////////////////////////////////////////////////////////
		//
		//	isPaused
		//
		////////////////////////////////////////////////////////////////

		/// @brief このエフェクトグループの時間経過が一時停止されているかを返します。
		/// @return 一時停止されている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isPaused() const;

		////////////////////////////////////////////////////////////////
		//
		//	resume
		//
		////////////////////////////////////////////////////////////////

		/// @brief このエフェクトグループの時間経過が一時停止されている場合、再開します。
		void resume() const;

		////////////////////////////////////////////////////////////////
		//
		//	setSpeed, getSpeed
		//
		////////////////////////////////////////////////////////////////

		/// @brief このエフェクトグループの時間経過の速さを、実時間に対する倍率 (2.0 で 2 倍早く経過）で設定します。
		/// @param speed 時間経過の速さ
		const Effect& setSpeed(double speed) const;

		/// @brief このエフェクトグループの時間経過の速さを返します。
		/// @return 時間経過の速さ
		[[nodiscard]]
		double getSpeed() const;

		////////////////////////////////////////////////////////////////
		//
		//	setMaxLifeTime, getMaxLifeTime
		//
		////////////////////////////////////////////////////////////////

		/// @brief このエフェクトグループでのエフェクトの最大継続時間（秒）を設定します。
		/// @param maxLifeTimeSec このエフェクトグループでのエフェクトの最大継続時間（秒）
		/// @return *this
		const Effect& setMaxLifeTime(double maxLifeTimeSec) const;

		/// @brief このエフェクトグループでのエフェクトの最大継続時間（秒）を設定します。
		/// @param maxLifeTimeSec このエフェクトグループでのエフェクトの最大継続時間（秒）
		/// @return *this
		const Effect& setMaxLifeTime(const Duration& maxLifeTimeSec) const;

		/// @brief このエフェクトグループでのエフェクトの最大継続時間（秒）を返します。
		/// @return このエフェクトグループでのエフェクトの最大継続時間（秒）
		[[nodiscard]]
		double getMaxLifeTime() const;

		////////////////////////////////////////////////////////////////
		//
		//	setSortingEnabled, isSortingEnabled
		//
		////////////////////////////////////////////////////////////////

		/// @brief このエフェクトグループでの描画順序のソートを有効化または無効化します。
		/// @param enabled 描画順序のソートを有効化する場合 true, 無効化する場合は false
		/// @return *this
		const Effect& setSortingEnabled(bool enabled) const;

		/// @brief このエフェクトグループでの描画順序のソートが有効化されているかを返します。
		/// @return 描画順序のソートが有効化されている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isSortingEnabled() const;

		////////////////////////////////////////////////////////////////
		//
		//	update
		//
		////////////////////////////////////////////////////////////////

		/// @brief このエフェクトグループ内のエフェクトの `update()` を実行します。
		void update() const;

		////////////////////////////////////////////////////////////////
		//
		//	clear
		//
		////////////////////////////////////////////////////////////////

		/// @brief このエフェクトグループ内の全てのエフェクトを、経過時間に関わらず消去します。
		void clear() const;

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の Effect と内容を交換します。
		/// @param other 別の Effect
		void swap(Effect& other) noexcept;

		/// @brief 2 つの Effect を入れ替えます。
		/// @param lhs 一方の Effect
		/// @param rhs もう一方の Effect
		friend void swap(Effect& lhs, Effect& rhs) noexcept
		{
			lhs.swap(rhs);
		}
	};
}

# include "detail/Effect.ipp"
