//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include <Siv3D/Array.hpp>
# include <ThirdParty/doctest/doctest.h>
# include <map>
# include <set>

using namespace s3d;

namespace
{
	struct InjectedArrayFailure {};

	struct FailureGate
	{
		int failAt = -1;
		int calls = 0;

		void arm(int position) { failAt = position; calls = 0; }
		void hit()
		{
			if (calls++ == failAt) { throw InjectedArrayFailure{}; }
		}
	};

	struct ArrayFaultState
	{
		FailureGate copies, moves, assignments, allocations;
		std::set<const void*> live;
		std::map<void*, size_t> blocks;
	};

	template <bool Copyable>
	struct TrackedArrayValue
	{
		ArrayFaultState* state;
		std::unique_ptr<int> resource;

		TrackedArrayValue(ArrayFaultState& s, int value)
			: state(&s), resource(std::make_unique<int>(value))
		{
			CHECK(state->live.insert(this).second);
		}

		TrackedArrayValue(const TrackedArrayValue& other) requires Copyable
			: state(other.state)
		{
			state->copies.hit();
			resource = std::make_unique<int>(other.value());
			CHECK(state->live.insert(this).second);
		}

		TrackedArrayValue(TrackedArrayValue&& other) noexcept(false)
			: state(other.state)
		{
			state->moves.hit();
			resource = std::move(other.resource);
			CHECK(state->live.insert(this).second);
		}

		TrackedArrayValue& operator =(const TrackedArrayValue& other) requires Copyable
		{
			state->assignments.hit();
			resource = std::make_unique<int>(other.value());
			return *this;
		}

		TrackedArrayValue& operator =(TrackedArrayValue&& other) noexcept(false)
		{
			state->assignments.hit();
			resource = std::move(other.resource);
			return *this;
		}

		~TrackedArrayValue() { CHECK_EQ(state->live.erase(this), size_t{ 1 }); }
		int value() const { return resource ? *resource : -1; }
	};

	template <class T, bool Propagate = false, class Element = T>
	struct FaultArrayAllocator
	{
		using value_type = T;
		using propagate_on_container_copy_assignment = std::bool_constant<Propagate>;
		using propagate_on_container_move_assignment = std::bool_constant<Propagate>;
		using propagate_on_container_swap = std::bool_constant<Propagate>;
		template <class U> struct rebind { using other = FaultArrayAllocator<U, Propagate, Element>; };

		ArrayFaultState* state = nullptr;
		FaultArrayAllocator() = default;
		explicit FaultArrayAllocator(ArrayFaultState& s) : state(&s) {}
		template <class U>
		FaultArrayAllocator(const FaultArrayAllocator<U, Propagate, Element>& other) : state(other.state) {}

		T* allocate(size_t n)
		{
			// Inject failures only into element storage. MSVC also rebinds this allocator
			// for debug iterator bookkeeping, including allocations in noexcept constructors.
			if constexpr (std::same_as<T, Element>)
			{
				if (state)
				{
					try { state->allocations.hit(); }
					catch (const InjectedArrayFailure&) { throw std::bad_alloc{}; }
				}
			}
			T* p = std::allocator<T>{}.allocate(n);
			if (state) { CHECK(state->blocks.emplace(p, n).second); }
			return p;
		}

		void deallocate(T* p, size_t n)
		{
			if (state)
			{
				const auto it = state->blocks.find(p);
				CHECK(it != state->blocks.end());
				if (it != state->blocks.end())
				{
					CHECK_EQ(it->second, n);
					state->blocks.erase(it);
				}
			}
			std::allocator<T>{}.deallocate(p, n);
		}

		template <class U>
		bool operator ==(const FaultArrayAllocator<U, Propagate, Element>& other) const { return state == other.state; }
	};

	using CopyValue = TrackedArrayValue<true>;
	using FaultArray = Array<CopyValue, FaultArrayAllocator<CopyValue>>;

	FaultArray MakeFaultArray(ArrayFaultState& state)
	{
		FaultArray result{ FaultArrayAllocator<CopyValue>{ state } };
		result.reserve(8);
		for (int i = 0; i < 8; ++i) { result.emplace_back(state, i); }
		return result;
	}

	void CheckOriginal(const FaultArray& source)
	{
		REQUIRE_EQ(source.size(), size_t{ 8 });
		for (size_t i = 0; i < source.size(); ++i) { CHECK_EQ(source[i].value(), static_cast<int>(i)); }
	}
}

TEST_CASE("Array.exception.copy_construction_and_derived_results")
{
	// Sweep every copy construction, including successful completion after the last fault point.
	for (int operation = 0; operation < 4; ++operation)
	{
		for (int failAt = 0; failAt <= 8; ++failAt)
		{
			CAPTURE(operation); CAPTURE(failAt);
			ArrayFaultState state;
			{
				const auto source = MakeFaultArray(state);
				const auto blocks = state.blocks.size();
				state.copies.arm(failAt);
				bool failed = false;
				try
				{
					const auto result = [&]() -> FaultArray
					{
						switch (operation)
						{
						case 0: return FaultArray{ source };
						case 1: return source.filter([](const CopyValue&) { return true; });
						case 2: return source.slice(0, source.size());
						default: return source.reversed();
						}
					}();
					CHECK_EQ(result.size(), source.size());
				}
				catch (const InjectedArrayFailure&) { failed = true; }
				CHECK_EQ(failed, failAt < 8);
				CheckOriginal(source);
				CHECK_EQ(state.live.size(), source.size());
				CHECK_EQ(state.blocks.size(), blocks);
			}
			CHECK(state.live.empty());
			CHECK(state.blocks.empty());
		}
	}
}

TEST_CASE("Array.exception.reserve_relocation")
{
	for (int failAt = 0; failAt < 8; ++failAt)
	{
		CAPTURE(failAt);
		ArrayFaultState state;
		{
			auto source = MakeFaultArray(state);
			const auto storage = source.data();
			const auto capacity = source.capacity();
			const auto blocks = state.blocks.size();
			state.copies.arm(failAt);
			// A copyable type with a throwing move constructor allows reserve's strong guarantee.
			CHECK_THROWS_AS(source.reserve(capacity + 16), InjectedArrayFailure);
			CheckOriginal(source);
			CHECK_EQ(source.data(), storage);
			CHECK_EQ(source.capacity(), capacity);
			CHECK_EQ(state.live.size(), source.size());
			CHECK_EQ(state.blocks.size(), blocks);
			state.copies.arm(-1);
			source.reserve(capacity + 16);
			CheckOriginal(source);
		}
		CHECK(state.live.empty());
		CHECK(state.blocks.empty());
	}
}

TEST_CASE("Array.exception.consuming_filter_assignment")
{
	for (int failAt = 0; failAt <= 4; ++failAt)
	{
		CAPTURE(failAt);
		ArrayFaultState state;
		{
			auto source = MakeFaultArray(state);
			state.assignments.arm(failAt);
			bool failed = false;
			try
			{
				const auto result = std::move(source).filter([](const CopyValue& v) { return (v.value() % 2) != 0; });
				REQUIRE_EQ(result.size(), size_t{ 4 });
				for (size_t i = 0; i < result.size(); ++i) { CHECK_EQ(result[i].value(), static_cast<int>(i * 2 + 1)); }
			}
			catch (const InjectedArrayFailure&) { failed = true; }
			CHECK_EQ(failed, failAt < 4);
			// Compaction may already have moved elements. Assert lifetime and reuse, not rollback.
			CHECK_EQ(state.live.size(), source.size());
			for (const auto& v : source) { CHECK(state.live.contains(&v)); }
			state.assignments.arm(-1);
			source.clear();
			source.emplace_back(state, 42);
			CHECK_EQ(source.front().value(), 42);
		}
		CHECK(state.live.empty());
		CHECK(state.blocks.empty());
	}
}

TEST_CASE("Array.exception.flatten_move_construction")
{
	using Value = TrackedArrayValue<false>;
	static_assert(not std::copy_constructible<Value>);
	for (int failAt = 0; failAt <= 6; ++failAt)
	{
		CAPTURE(failAt);
		ArrayFaultState state;
		{
			Array<Array<Value>> source;
			for (int row = 0; row < 2; ++row)
			{
				Array<Value> inner;
				inner.reserve(3);
				for (int col = 0; col < 3; ++col) { inner.emplace_back(state, row * 3 + col); }
				source.push_back(std::move(inner));
			}
			state.moves.arm(failAt);
			bool failed = false;
			try
			{
				const auto result = std::move(source).flatten();
				REQUIRE_EQ(result.size(), size_t{ 6 });
				for (size_t i = 0; i < result.size(); ++i) { CHECK_EQ(result[i].value(), static_cast<int>(i)); }
			}
			catch (const InjectedArrayFailure&) { failed = true; }
			CHECK_EQ(failed, failAt < 6);
			CHECK_EQ(state.live.size(), size_t{ 6 });
			for (const auto& row : source)
			{
				for (const auto& v : row) { CHECK(state.live.contains(&v)); }
			}
			source.clear();
			CHECK(state.live.empty());
		}
		CHECK(state.live.empty());
	}
}

namespace
{
	template <class T>
	void CheckArrayAllocationFailure()
	{
		using A = Array<T, FaultArrayAllocator<T>>;
		for (int operation = 0; operation < 5; ++operation)
		{
			CAPTURE(operation);
			ArrayFaultState state;
			{
				A source(128, T{ 1 }, FaultArrayAllocator<T>{ state });
				const auto storage = source.data();
				const auto capacity = source.capacity();
				const auto blocks = state.blocks.size();
				state.allocations.arm(0);
				const auto action = [&]
				{
					switch (operation)
					{
					case 0: source.reserve(capacity + 1); break;
					case 1: source.resize(capacity + 1, T{}); break;
					case 2: (void)source.filter([](T) { return true; }); break;
					case 3: (void)source.slice(0); break;
					default: source.append(capacity + 1, T{}); break;
					}
				};
				CHECK_THROWS_AS(action(), std::bad_alloc);
				CHECK_EQ(source.data(), storage);
				CHECK_EQ(source.capacity(), capacity);
				CHECK_EQ(source.size(), size_t{ 128 });
				CHECK(source.all([](T v) { return v == T{ 1 }; }));
				CHECK_EQ(state.blocks.size(), blocks);
				state.allocations.arm(-1);
				source.push_back(T{});
				CHECK_EQ(source.back(), T{});
			}
			CHECK(state.blocks.empty());
		}
	}

	template <class T, bool Propagate>
	void CheckArrayAllocatorPropagation()
	{
		using A = Array<T, FaultArrayAllocator<T, Propagate>>;
		ArrayFaultState first, second;
		{
			A source(128, T{ 1 }, FaultArrayAllocator<T, Propagate>{ first });
			A destination(64, T{}, FaultArrayAllocator<T, Propagate>{ second });
			destination = source;
			CHECK_EQ(destination.get_allocator().state, Propagate ? &first : &second);
			CHECK_EQ(destination.size(), source.size());
			CHECK(destination.all([](T v) { return v == T{ 1 }; }));
		}
		CHECK(first.blocks.empty()); CHECK(second.blocks.empty());
		{
			A source(128, T{ 1 }, FaultArrayAllocator<T, Propagate>{ first });
			A destination(64, T{}, FaultArrayAllocator<T, Propagate>{ second });
			destination = std::move(source);
			CHECK_EQ(destination.get_allocator().state, Propagate ? &first : &second);
			CHECK_EQ(destination.size(), size_t{ 128 });
			CHECK(destination.all([](T v) { return v == T{ 1 }; }));
			source.clear(); // The source's exact moved-from contents are not assumed.
			source.push_back(T{});
			CHECK_EQ(source.back(), T{});
		}
		CHECK(first.blocks.empty()); CHECK(second.blocks.empty());
		{
			// Swapping unequal non-propagating allocators would violate the precondition.
			A a(128, T{ 1 }, FaultArrayAllocator<T, Propagate>{ first });
			A b(64, T{}, FaultArrayAllocator<T, Propagate>{ Propagate ? second : first });
			a.swap(b);
			CHECK_EQ(a.size(), size_t{ 64 }); CHECK_EQ(b.size(), size_t{ 128 });
			CHECK_EQ(a.get_allocator().state, Propagate ? &second : &first);
			CHECK_EQ(b.get_allocator().state, &first);
			CHECK(a.all([](T v) { return v == T{}; }));
			CHECK(b.all([](T v) { return v == T{ 1 }; }));
		}
		CHECK(first.blocks.empty()); CHECK(second.blocks.empty());
	}
}

TEST_CASE("Array.exception.allocation_failure")
{
	CheckArrayAllocationFailure<int>();
	CheckArrayAllocationFailure<bool>();
}

TEST_CASE("Array.exception.allocator_propagation")
{
	CheckArrayAllocatorPropagation<int, false>();
	CheckArrayAllocatorPropagation<int, true>();
	CheckArrayAllocatorPropagation<bool, false>();
	CheckArrayAllocatorPropagation<bool, true>();
}

TEST_CASE("Array.exception.bool_storage_transition")
{
	using B = Array<bool, FaultArrayAllocator<bool>>;
	ArrayFaultState state;
	{
		B flags{ FaultArrayAllocator<bool>{ state } };
		// An empty container may own debug iterator bookkeeping until destruction.
		const auto emptyBlocks = state.blocks.size();
		const auto smallCapacity = flags.capacity();
		flags.resize(smallCapacity, true);
		const auto storage = flags.data();
		const auto blocks = state.blocks.size();
		state.allocations.arm(0);
		CHECK_THROWS_AS(flags.push_back(false), std::bad_alloc);
		CHECK_EQ(flags.data(), storage);
		CHECK_EQ(flags.size(), smallCapacity);
		CHECK(flags.all());
		CHECK_EQ(state.blocks.size(), blocks);
		state.allocations.arm(-1);
		flags.push_back(false);
		CHECK_EQ(flags.size(), smallCapacity + 1);
		CHECK_FALSE(flags.back());
		CHECK_EQ(flags.get_allocator().state, &state);
		flags.resize(1, false);
		flags.shrink_to_fit(); // Shrinking is non-binding; only contents and ownership are promised.
		CHECK_EQ(flags.size(), size_t{ 1 });
		flags.release();
		CHECK(flags.empty());
		CHECK_EQ(state.blocks.size(), emptyBlocks);
		flags.push_back(true);
		CHECK(flags.front());
	}
	CHECK(state.blocks.empty());
}
