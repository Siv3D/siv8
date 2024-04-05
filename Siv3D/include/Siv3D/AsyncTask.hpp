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
# include <future>
# include <type_traits>
# include "Platform.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	AsyncTask
	//
	////////////////////////////////////////////////////////////////

	/// @brief 非同期処理クラス
	/// @tparam Type 非同期処理のタスクで実行する関数の戻り値の型
	template <class Type>
	class AsyncTask
	{
	public:

		using base_type = std::future<Type>;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		/// @remark 何もしません
		[[nodiscard]]
		AsyncTask() = default;

		[[nodiscard]]
		AsyncTask(base_type&& other) noexcept;

		[[nodiscard]] 
		AsyncTask(AsyncTask&& other) noexcept;

		/// @brief 非同期処理のタスクを作成します。
		/// @tparam Fty 非同期処理のタスクで実行する関数の型
		/// @tparam ...Args 非同期処理のタスクで実行する関数の引数の型
		/// @param f 非同期処理のタスクで実行する関数
		/// @param ...args 非同期処理のタスクで実行する関数の引数
		/// @remark 作成と同時にタスクが非同期で実行されます。
		/// @remark 参照を渡す場合は `std::ref()` を使ってください。
		template <class Fty, class... Args>
		[[nodiscard]]
		explicit AsyncTask(Fty&& f, Args&&... args) requires std::invocable<std::decay_t<Fty>, std::decay_t<Args>...>;

		AsyncTask(const base_type&) = delete;

		AsyncTask(const AsyncTask&) = delete;

		////////////////////////////////////////////////////////////////
		//
		//	operator =
		//
		////////////////////////////////////////////////////////////////

		AsyncTask& operator =(const base_type&) = delete;

		AsyncTask& operator =(const AsyncTask&) = delete;

		AsyncTask& operator =(base_type&& other) noexcept;

		AsyncTask& operator =(AsyncTask&& other) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isValid
		//
		////////////////////////////////////////////////////////////////

		/// @brief 非同期処理を持っているかを返します。
		/// @remark `get()` を呼ぶと、非同期処理を持たない状態に戻ります。
		/// @return 非同期処理を持っている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isValid() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isReady
		//
		////////////////////////////////////////////////////////////////

		/// @brief タスクが完了した非同期処理を持っていて、結果をすぐに返せる状態であるかを返します。
		/// @remark `get()` を呼ぶと、非同期処理を持たない状態に戻ります。
		/// @return タスクが完了した非同期処理を持っていて、結果をすぐに返せる状態である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isReady() const;

		////////////////////////////////////////////////////////////////
		//
		//	get
		//
		////////////////////////////////////////////////////////////////

		/// @brief タスクが完了した非同期処理の結果を返します。
		/// @remark タスクが完了していない場合は、完了まで待機します。
		/// @return タスクが完了した非同期処理の結果
		Type get();

		////////////////////////////////////////////////////////////////
		//
		//	wait
		//
		////////////////////////////////////////////////////////////////

		/// @brief 非同期処理のタスク完了を待ちます。
		void wait() const;

		////////////////////////////////////////////////////////////////
		//
		//	wait_for
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した時間だけ、非同期処理のタスク完了を待ちます。
		/// @tparam Rep 時間の表現の型
		/// @tparam Period 時間の単位
		/// @param relTime 時間
		/// @return 非同期処理のタスクが完了した場合 std::future_status::ready, 指定した時間が経過した場合 std::future_status::timeout
		template <class Rep, class Period>
		std::future_status wait_for(const std::chrono::duration<Rep, Period>& relTime) const;

		////////////////////////////////////////////////////////////////
		//
		//	wait_until
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した時刻まで、非同期処理のタスク完了を待ちます。
		/// @tparam Clock 時間の種類
		/// @tparam Duration 時間の型
		/// @param absTime 時刻
		/// @return 非同期処理のタスクが完了した場合 std::future_status::ready, 指定した時刻になった場合 std::future_status::timeout
		template <class Clock, class Duration>
		std::future_status wait_until(const std::chrono::time_point<Clock, Duration>& absTime) const;

		////////////////////////////////////////////////////////////////
		//
		//	share
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		std::shared_future<Type> share() noexcept;

	private:

		base_type m_data;
	};

	template <class Fty, class... Args> requires std::invocable<std::decay_t<Fty>, std::decay_t<Args>...>
	AsyncTask(Fty, Args...) -> AsyncTask<std::invoke_result_t<std::decay_t<Fty>, std::decay_t<Args>...>>;

	////////////////////////////////////////////////////////////////
	//
	//	Async
	//
	////////////////////////////////////////////////////////////////

	/// @brief 非同期処理のタスクを作成します。
	/// @tparam Fty 非同期処理のタスクで実行する関数の型
	/// @tparam ...Args 非同期処理のタスクで実行する関数の引数の型
	/// @param f 非同期処理のタスクで実行する関数
	/// @param ...args 非同期処理のタスクで実行する関数の引数
	/// @remark 作成と同時にタスクが非同期で実行されます。
	/// @remark 参照を渡す場合は `std::ref()` を使ってください。
	/// @return 作成された非同期処理のタスク
	template <class Fty, class... Args> requires std::invocable<std::decay_t<Fty>, std::decay_t<Args>...>
	[[nodiscard]]
	auto Async(Fty&& f, Args&&... args);
}

# include "detail/AsyncTask.ipp"
