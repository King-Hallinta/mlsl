// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Allocators/DefaultAllocator.hpp>
#include <mlsl/Allocators/FreeListAllocator.hpp>
#include <mlsl/Allocators/LinearArena.hpp>
#include <mlsl/Allocators/PoolAllocator.hpp>
#include <mlsl/Allocators/StackArena.hpp>
#include <mlsl/Memory/StaticBuffer.hpp>

#include <catch2/catch_test_macros.hpp>

#include <cstddef>
#include <cstring>
#include <utility>

TEST_CASE("DefaultAllocator allocates and frees heap memory")
{
	mlsl::DefaultAllocator allocator;
	auto block = allocator.Allocate(64);

	REQUIRE(block);
	REQUIRE(allocator.Owns(*block));

	allocator.Deallocate(*block, 64);
}

TEST_CASE("FreeListAllocator reuses released memory")
{
	mlsl::StaticBuffer<256> storage;
	mlsl::FreeListAllocator allocator(storage.Data(), storage.Size());

	auto block = allocator.Allocate(48, alignof(double));

	REQUIRE(block);
	REQUIRE(allocator.Owns(*block));
	REQUIRE(allocator.Used() > 0);

	allocator.Deallocate(*block, 48);

	REQUIRE(allocator.Used() == 0);
}

TEST_CASE("FreeListAllocator supports best fit allocation")
{
	mlsl::StaticBuffer<256> storage;
	mlsl::FreeListAllocator allocator(storage.Data(), storage.Size(), mlsl::FreeListAllocator::SearchPolicy::BestFit);

	auto block = allocator.Allocate(32, alignof(std::max_align_t));

	REQUIRE(block);
	REQUIRE(allocator.Owns(*block));

	allocator.Reset();

	REQUIRE(allocator.Used() == 0);
}

TEST_CASE("PoolAllocator tracks fixed block availability")
{
	mlsl::StaticBuffer<256> storage;
	mlsl::PoolAllocator allocator(storage.Data(), storage.Size(), 32);
	auto expectedAvailable = allocator.BlockCount();

	auto blockA = allocator.Allocate(16);
	auto blockB = allocator.Allocate(32);

	REQUIRE(blockA);
	REQUIRE(blockB);
	REQUIRE(allocator.Owns(*blockA));
	REQUIRE(allocator.Owns(*blockB));
	REQUIRE(allocator.Available() == expectedAvailable - 2);

	allocator.Deallocate(*blockB, 32);
	allocator.Deallocate(*blockA, 16);

	REQUIRE(allocator.Available() == expectedAvailable);
}

TEST_CASE("LinearArena resets all allocations at once")
{
	mlsl::StaticBuffer<256> storage;
	mlsl::LinearArena arena(storage.Data(), storage.Size());

	auto blockA = arena.Allocate(24);
	auto blockB = arena.Allocate(40, alignof(double));

	REQUIRE(blockA);
	REQUIRE(blockB);
	REQUIRE(arena.Owns(*blockA));
	REQUIRE(arena.Owns(*blockB));
	REQUIRE(arena.Used() > 0);

	arena.Reset();

	REQUIRE(arena.Used() == 0);
	REQUIRE(arena.Remaining() == arena.Capacity());
}

TEST_CASE("StackArena frees back to a marker")
{
	mlsl::StaticBuffer<256> storage;
	mlsl::StackArena arena(storage.Data(), storage.Size());

	auto blockA = arena.Allocate(24);
	auto marker = arena.GetMarker();
	auto blockB = arena.Allocate(40, alignof(double));

	REQUIRE(blockA);
	REQUIRE(blockB);
	REQUIRE(arena.Owns(*blockA));
	REQUIRE(arena.Owns(*blockB));

	arena.FreeToMarker(marker);

	REQUIRE(not arena.Owns(*blockB));
	REQUIRE(arena.Owns(*blockA));

	arena.Reset();

	REQUIRE(arena.Used() == 0);
}

TEST_CASE("DefaultAllocator handles zero size allocation and null deallocation")
{
	mlsl::DefaultAllocator allocator;

	auto block = allocator.Allocate(0);

	REQUIRE(block);
	REQUIRE(*block == nullptr);

	allocator.Deallocate(nullptr, 0);
}

TEST_CASE("Allocator reallocate handles null grow shrink and zero size")
{
	mlsl::DefaultAllocator allocator;

	auto initial = allocator.Reallocate(nullptr, 0, 8, alignof(unsigned char));

	REQUIRE(initial);
	REQUIRE(*initial != nullptr);

	auto bytes = static_cast<unsigned char *>(*initial);

	for (unsigned char i = 0; i < 8; ++i)
	{
		bytes[i] = i;
	}

	auto grown = allocator.Reallocate(*initial, 8, 16, alignof(unsigned char));

	REQUIRE(grown);

	auto grownBytes = static_cast<unsigned char *>(*grown);

	for (unsigned char i = 0; i < 8; ++i)
	{
		REQUIRE(grownBytes[i] == i);
	}

	auto shrunk = allocator.Reallocate(*grown, 16, 4, alignof(unsigned char));

	REQUIRE(shrunk);

	auto shrunkBytes = static_cast<unsigned char *>(*shrunk);

	for (unsigned char i = 0; i < 4; ++i)
	{
		REQUIRE(shrunkBytes[i] == i);
	}

	auto released = allocator.Reallocate(*shrunk, 4, 0, alignof(unsigned char));

	REQUIRE(released);
	REQUIRE(*released == nullptr);
}

TEST_CASE("FreeListAllocator reports exhaustion and coalesces released blocks")
{
	mlsl::StaticBuffer<128> storage;
	mlsl::FreeListAllocator allocator(storage.Data(), storage.Size());

	auto first = allocator.Allocate(24);
	auto second = allocator.Allocate(24);
	auto third = allocator.Allocate(24);

	REQUIRE(first);
	REQUIRE(second);
	REQUIRE(third);

	allocator.Deallocate(*second, 24);
	allocator.Deallocate(*first, 24);

	auto combined = allocator.Allocate(40);

	REQUIRE(combined);
	REQUIRE(allocator.Owns(*combined));

	auto oversized = allocator.Allocate(storage.Size());

	REQUIRE(not oversized);
	REQUIRE(oversized.error().type == mlsl::ErrorType::OutOfMemory);
}

TEST_CASE("FreeListAllocator move transfers ownership state")
{
	mlsl::StaticBuffer<128> storage;
	mlsl::FreeListAllocator allocator(storage.Data(), storage.Size());

	auto block = allocator.Allocate(16);

	REQUIRE(block);

	mlsl::FreeListAllocator moved(std::move(allocator));

	REQUIRE(moved.Capacity() == storage.Size());
	REQUIRE(moved.Owns(*block));
	REQUIRE(allocator.Capacity() == 0);

	moved.Deallocate(*block, 16);

	REQUIRE(moved.Used() == 0);
}

TEST_CASE("LinearArena reports out of memory and owns only active allocations")
{
	mlsl::StaticBuffer<64> storage;
	mlsl::LinearArena arena(storage.Data(), storage.Size());

	auto block = arena.Allocate(48, alignof(std::max_align_t));
	auto overflow = arena.Allocate(64, alignof(std::max_align_t));

	REQUIRE(block);
	REQUIRE(not overflow);
	REQUIRE(overflow.error().type == mlsl::ErrorType::OutOfMemory);
	REQUIRE(arena.Owns(*block));

	arena.Reset();

	REQUIRE(not arena.Owns(*block));
	REQUIRE(arena.Used() == 0);
}

TEST_CASE("LinearArena move assignment transfers allocation state")
{
	mlsl::StaticBuffer<64> storage;
	mlsl::LinearArena source(storage.Data(), storage.Size());
	mlsl::LinearArena target;
	auto block = source.Allocate(16);

	REQUIRE(block);

	target = std::move(source);

	REQUIRE(target.Capacity() == storage.Size());
	REQUIRE(target.Owns(*block));
	REQUIRE(source.Capacity() == 0);
}

TEST_CASE("StackArena reports out of memory and restores markers")
{
	mlsl::StaticBuffer<64> storage;
	mlsl::StackArena arena(storage.Data(), storage.Size());

	auto first = arena.Allocate(16);
	auto marker = arena.GetMarker();
	auto second = arena.Allocate(16);
	auto overflow = arena.Allocate(64);

	REQUIRE(first);
	REQUIRE(second);
	REQUIRE(not overflow);
	REQUIRE(overflow.error().type == mlsl::ErrorType::OutOfMemory);

	arena.FreeToMarker(marker);

	REQUIRE(arena.Owns(*first));
	REQUIRE(not arena.Owns(*second));
}

TEST_CASE("StackArena move construction transfers marker state")
{
	mlsl::StaticBuffer<64> storage;
	mlsl::StackArena source(storage.Data(), storage.Size());
	auto block = source.Allocate(24);

	REQUIRE(block);

	mlsl::StackArena moved(std::move(source));

	REQUIRE(moved.Capacity() == storage.Size());
	REQUIRE(moved.Owns(*block));
	REQUIRE(source.Capacity() == 0);
}

TEST_CASE("PoolAllocator rejects oversize requests and exhausts fixed blocks")
{
	mlsl::StaticBuffer<64> storage;
	mlsl::PoolAllocator allocator(storage.Data(), storage.Size(), 16);

	auto tooLarge = allocator.Allocate(32);

	REQUIRE(not tooLarge);
	REQUIRE(tooLarge.error().type == mlsl::ErrorType::OutOfMemory);

	for (std::size_t i = 0; i < allocator.BlockCount(); ++i)
	{
		auto block = allocator.Allocate(16);

		REQUIRE(block);
	}

	auto exhausted = allocator.Allocate(16);

	REQUIRE(not exhausted);
	REQUIRE(exhausted.error().type == mlsl::ErrorType::OutOfMemory);
	REQUIRE(allocator.Available() == 0);
}

TEST_CASE("PoolAllocator reset rebuilds the free list")
{
	mlsl::StaticBuffer<64> storage;
	mlsl::PoolAllocator allocator(storage.Data(), storage.Size(), 16);

	auto first = allocator.Allocate(16);
	auto second = allocator.Allocate(16);

	REQUIRE(first);
	REQUIRE(second);
	REQUIRE(allocator.Available() == allocator.BlockCount() - 2);

	allocator.Reset();

	REQUIRE(allocator.Available() == allocator.BlockCount());

	auto afterReset = allocator.Allocate(16);

	REQUIRE(afterReset);
	REQUIRE(allocator.Owns(*afterReset));
}

TEST_CASE("PoolAllocator move assignment transfers free list state")
{
	mlsl::StaticBuffer<64> storage;
	mlsl::PoolAllocator source(storage.Data(), storage.Size(), 16);
	mlsl::PoolAllocator target;

	auto block = source.Allocate(16);

	REQUIRE(block);

	target = std::move(source);

	REQUIRE(target.BlockCount() == storage.Size() / target.BlockSize());
	REQUIRE(target.Owns(*block));
	REQUIRE(source.BlockCount() == 0);

	target.Deallocate(*block, 16);

	REQUIRE(target.Available() == target.BlockCount());
}
