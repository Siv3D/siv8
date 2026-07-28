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

# include "Siv3DTest.hpp"

namespace
{
	struct ReturnInt
	{
		int32 operator ()() const
		{
			return 42;
		}
	};

	struct ReturnDouble
	{
		double operator ()() const
		{
			return 42.0;
		}
	};

	struct MoveOnlyCallable
	{
		MoveOnlyCallable() = default;

		MoveOnlyCallable(const MoveOnlyCallable&) = delete;

		MoveOnlyCallable(MoveOnlyCallable&&) = default;

		int32 operator ()() const
		{
			return 42;
		}
	};

	template <class Type, class Fty, class... Args>
	concept AsyncTaskConstructible = requires(Fty&& f, Args&&... args)
	{
		AsyncTask<Type>{ std::forward<Fty>(f), std::forward<Args>(args)... };
	};

	template <class Type>
	concept AsyncTaskShareable = requires(Type&& task)
	{
		std::forward<Type>(task).share();
	};

	static_assert(AsyncTaskConstructible<int32, ReturnInt>);
	static_assert(AsyncTaskConstructible<double, ReturnDouble>);
	static_assert(not AsyncTaskConstructible<int32, ReturnDouble>);
	static_assert(not AsyncTaskConstructible<double, ReturnInt>);
	static_assert(AsyncTaskConstructible<int32, MoveOnlyCallable>);
	static_assert(not AsyncTaskConstructible<int32, MoveOnlyCallable&>);
	static_assert(not std::constructible_from<AsyncTask<int32>, std::future<int32>>);
	static_assert(not std::is_assignable_v<AsyncTask<int32>&, std::future<int32>>);
	static_assert(not AsyncTaskShareable<AsyncTask<int32>&>);
	static_assert(AsyncTaskShareable<AsyncTask<int32>>);
}

TEST_CASE("AsyncTask")
{
	using namespace std::chrono_literals;

	SUBCASE("AsyncTaskStatus Formatter")
	{
		CHECK_EQ(Format(AsyncTaskStatus::Invalid), U"Invalid");
		CHECK_EQ(Format(AsyncTaskStatus::Running), U"Running");
		CHECK_EQ(Format(AsyncTaskStatus::Ready), U"Ready");
	}

	SUBCASE("Default construction")
	{
		const AsyncTask<int32> task;

		CHECK_FALSE(task.isValid());
		CHECK_EQ(task.status(), AsyncTaskStatus::Invalid);
		CHECK_FALSE(task.isReady());
	}

	SUBCASE("Value result and status")
	{
		std::promise<void> promise;
		const std::shared_future<void> gate = promise.get_future().share();
		AsyncTask task{ [gate]
		{
			gate.wait();
			return int32{ 42 };
		} };

		static_assert(std::same_as<decltype(task), AsyncTask<int32>>);

		CHECK(task.isValid());
		CHECK_EQ(task.status(), AsyncTaskStatus::Running);
		CHECK_EQ(task.wait_for(0ms), std::future_status::timeout);

		promise.set_value();
		task.wait();

		CHECK_EQ(task.status(), AsyncTaskStatus::Ready);
		CHECK(task.isReady());
		CHECK_EQ(task.get(), 42);
		CHECK_FALSE(task.isValid());
		CHECK_EQ(task.status(), AsyncTaskStatus::Invalid);
	}

	SUBCASE("Void result")
	{
		int32 value = 0;
		AsyncTask<void> task{ [&value]
		{
			value = 42;
		} };

		task.get();

		CHECK_EQ(value, 42);
		CHECK_FALSE(task.isValid());
	}

	SUBCASE("Reference result")
	{
		int32 value = 42;
		AsyncTask<int32&> task{ [&value]() -> int32&
		{
			return value;
		} };

		int32& result = task.get();

		CHECK_EQ(&result, &value);
		result = 123;
		CHECK_EQ(value, 123);
	}

	SUBCASE("Move-only callable")
	{
		AsyncTask task{ [value = std::make_unique<int32>(42)]
		{
			return *value;
		} };

		CHECK_EQ(task.get(), 42);
	}

	SUBCASE("Arguments")
	{
		int32 value = 1;
		AsyncTask task{ [](int32& target, int32 amount)
		{
			target += amount;
		}, std::ref(value), 41 };

		task.get();

		CHECK_EQ(value, 42);
	}

	SUBCASE("share")
	{
		AsyncTask task{ []
		{
			return int32{ 42 };
		} };

		const std::shared_future<int32> sharedTask = std::move(task).share();

		CHECK_FALSE(task.isValid());
		CHECK_EQ(task.status(), AsyncTaskStatus::Invalid);
		CHECK_EQ(sharedTask.get(), 42);
	}
}
