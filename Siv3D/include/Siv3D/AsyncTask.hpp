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
# include <chrono>
# include <concepts>
# include <future>
# include <type_traits>
# include <utility>
# include "AsyncTaskStatus.hpp"

namespace s3d
{
	namespace detail
	{
		template <class Fty, class... Args>
		concept AsyncInvocable = std::invocable<std::decay_t<Fty>, std::decay_t<Args>...>
			&& std::constructible_from<std::decay_t<Fty>, Fty>
			&& (std::constructible_from<std::decay_t<Args>, Args> && ...);
	}

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
		AsyncTask(AsyncTask&& other) noexcept;

		/// @brief 非同期処理のタスクを作成します。
		/// @tparam Fty 非同期処理のタスクで実行する関数の型
		/// @tparam ...Args 非同期処理のタスクで実行する関数の引数の型
		/// @param f 非同期処理のタスクで実行する関数
		/// @param ...args 非同期処理のタスクで実行する関数の引数
		/// @remark 作成と同時にタスクが非同期で実行されます。
		/// @remark 参照を渡す場合は `std::ref()` を使ってください。
		/// @throw std::system_error 非同期処理の開始に失敗した場合
		/// @throw std::bad_alloc 非同期処理の開始に必要なメモリの確保に失敗した場合
		template <class Fty, class... Args>
		[[nodiscard]]
		explicit AsyncTask(Fty&& f, Args&&... args)
			requires detail::AsyncInvocable<Fty, Args...>
				&& std::same_as<Type, std::invoke_result_t<std::decay_t<Fty>, std::decay_t<Args>...>>;

		AsyncTask(const AsyncTask&) = delete;

		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デストラクタ
		/// @remark 非同期処理を持っている場合、その完了まで待機することがあります。
		~AsyncTask() = default;

		////////////////////////////////////////////////////////////////
		//
		//	operator =
		//
		////////////////////////////////////////////////////////////////

		AsyncTask& operator =(const AsyncTask&) = delete;

		/// @brief ムーブ代入演算子
		/// @param other ムーブする非同期タスク
		/// @return *this
		/// @remark 現在の非同期処理を持っている場合、その完了まで待機することがあります。
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
		//	status
		//
		////////////////////////////////////////////////////////////////

		/// @brief 非同期タスクの状態を返します。
		/// @return 非同期タスクの状態
		[[nodiscard]]
		AsyncTaskStatus status() const;

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
		/// @remark 非同期処理が例外を送出した場合、その例外を再送出します。
		/// @return タスクが完了した非同期処理の結果
		/// @pre `isValid() == true`
		Type get();

		////////////////////////////////////////////////////////////////
		//
		//	wait
		//
		////////////////////////////////////////////////////////////////

		/// @brief 非同期処理のタスク完了を待ちます。
		/// @pre `isValid() == true`
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
		/// @pre `isValid() == true`
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
		/// @pre `isValid() == true`
		template <class Clock, class Duration>
		std::future_status wait_until(const std::chrono::time_point<Clock, Duration>& absTime) const;

		////////////////////////////////////////////////////////////////
		//
		//	share
		//
		////////////////////////////////////////////////////////////////

		/// @brief 非同期処理を共有可能な非同期処理に変換します。
		/// @return 共有可能な非同期処理
		/// @remark この関数を呼ぶと、この非同期タスクは非同期処理を持たない状態になります。
		[[nodiscard]]
		std::shared_future<Type> share() && noexcept;

	private:

		std::future<Type> m_data;
	};

	template <class Fty, class... Args>
		requires detail::AsyncInvocable<Fty, Args...>
	AsyncTask(Fty&&, Args&&...) -> AsyncTask<std::invoke_result_t<std::decay_t<Fty>, std::decay_t<Args>...>>;

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
	/// @throw std::system_error 非同期処理の開始に失敗した場合
	/// @throw std::bad_alloc 非同期処理の開始に必要なメモリの確保に失敗した場合
	template <class Fty, class... Args>
		requires detail::AsyncInvocable<Fty, Args...>
	[[nodiscard]]
	auto Async(Fty&& f, Args&&... args);
}

# include "detail/AsyncTask.ipp"
