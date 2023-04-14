//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <future>
# include <type_traits>

namespace s3d
{
	/// @brief 非同期処理クラス | Asynchronous Task
	/// @tparam Type 非同期処理のタスクで実行する関数の戻り値の型 | Return type of the function to be executed by the asynchronous task
	template <class Type>
	class AsyncTask
	{
	public:

		using base_type = std::future<Type>;

		/// @brief デフォルトコンストラクタ | Default constructor
		/// @remark 何もしません | Does nothing
		[[nodiscard]]
		AsyncTask() = default;

		[[nodiscard]]
		AsyncTask(base_type&& other) noexcept;

		[[nodiscard]]
		AsyncTask(AsyncTask&& other) noexcept;

		/// @brief 非同期処理のタスクを作成します | Creates an asynchronous task
		/// @tparam Fty 非同期処理のタスクで実行する関数の型 | Type of function to be executed by the asynchronous task
		/// @tparam ...Args 非同期処理のタスクで実行する関数の引数の型 | Type of arguments of the function to be executed by the asynchronous task
		/// @param f 非同期処理のタスクで実行する関数 | Function to be executed by the asynchronous task
		/// @param ...args 非同期処理のタスクで実行する関数の引数 | Arguments of the function to be executed by the asynchronous task
		/// @remark 作成と同時にタスクが非同期で実行されます | The task is executed asynchronously at the same time as creation
		template <class Fty, class... Args>
			requires std::invocable<Fty, Args...>
		[[nodiscard]]
		explicit AsyncTask(Fty&& f, Args&&... args);

		AsyncTask(const base_type&) = delete;

		AsyncTask(const AsyncTask&) = delete;

		~AsyncTask() = default;

		AsyncTask& operator =(const base_type&) = delete;
		
		AsyncTask& operator =(const AsyncTask&) = delete;

		AsyncTask& operator =(base_type&& other) noexcept;
		
		AsyncTask& operator =(AsyncTask&& other) noexcept;

		/// @brief タスクを持っているかを返します。 | Returns whether the task is held.
		/// @remark `get()` を呼ぶと、タスクを持っていない状態になります。 | Calling `get()` will make the task not held.
		/// @return タスクを持っている場合 true, それ以外の場合は false | If the task is held, true, otherwise false
		[[nodiscard]]
		bool isValid() const noexcept;

		/// @brief タスクが完了して結果を返せる状態であるかを返します。 | Returns whether the task is in a state where it can return the result.
		/// @remark `get()` を呼ぶと、タスクを持っていない状態になります。 | Calling `get()` will make the task not held.
		/// @return タスクが完了して結果を返せる状態である場合 true, それ以外の場合は false | If the task is in a state where it can return the result, true, otherwise false
		[[nodiscard]]
		bool isReady() const;

		/// @brief タスクが完了した非同期処理の結果を返します。 | Returns the result of the asynchronous task that has completed.
		/// @remark タスクが完了していない場合、タスクの完了まで待機します。 | If the task has not been completed, it waits for the task to complete.
		/// @return タスクが完了した非同期処理の結果 | Result of the asynchronous task that has completed
		Type get();

		/// @brief 非同期処理のタスク完了を待ちます。 | Waits for the asynchronous task to complete.
		void wait() const;

		template <class Rep, class Period>
		std::future_status wait_for(const std::chrono::duration<Rep, Period>& relTime) const;

		template <class Clock, class Duration>
		std::future_status wait_until(const std::chrono::time_point<Clock, Duration>& absTime) const;

		[[nodiscard]]
		std::shared_future<Type> share() noexcept;

	private:

		base_type m_data;
	};

	template <class Fty, class... Args>
		requires std::invocable<Fty, Args...>
	AsyncTask(Fty, Args...) -> AsyncTask<std::invoke_result_t<std::decay_t<Fty>, std::decay_t<Args>...>>;

	/// @brief 非同期処理のタスクを作成します。 | Creates an asynchronous task.
	/// @tparam Fty 非同期処理のタスクで実行する関数の型 | Type of function to be executed by the asynchronous task
	/// @tparam ...Args 非同期処理のタスクで実行する関数の引数の型 | Type of arguments of the function to be executed by the asynchronous task
	/// @param f 非同期処理のタスクで実行する関数 | Function to be executed by the asynchronous task
	/// @param ...args 非同期処理のタスクで実行する関数の引数 | Arguments of the function to be executed by the asynchronous task
	/// @remark 作成と同時にタスクが非同期で実行されます | The task is executed asynchronously at the same time as creation
	/// @return 作成された非同期処理のタスク | The asynchronous task created
	template <class Fty, class... Args>
		requires std::invocable<Fty, Args...>
	[[nodiscard]]
	inline auto Async(Fty&& f, Args&&... args);
}

# include "detail/AsyncTask.ipp"
