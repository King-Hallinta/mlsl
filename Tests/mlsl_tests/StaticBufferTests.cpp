// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Memory/StaticBuffer.hpp>

#include <catch2/catch_test_macros.hpp>

#include <cstddef>
#include <cstdint>

TEST_CASE("StaticBuffer exposes capacity alignment and writable storage")
{
	mlsl::StaticBuffer<32, 16> buffer;

	REQUIRE(buffer.Size() == 32);
	REQUIRE(buffer.Alignment() == 16);
	REQUIRE(buffer.Data() != nullptr);
	REQUIRE(reinterpret_cast<std::uintptr_t>(buffer.Data()) % buffer.Alignment() == 0);

	buffer.Data()[0] = std::byte {0x12};
	buffer.Data()[31] = std::byte {0x34};

	REQUIRE(buffer.Data()[0] == std::byte {0x12});
	REQUIRE(buffer.Data()[31] == std::byte {0x34});
}

TEST_CASE("Const StaticBuffer exposes read only storage")
{
	const mlsl::StaticBuffer<8, 8> buffer;

	REQUIRE(buffer.Size() == 8);
	REQUIRE(buffer.Alignment() == 8);
	REQUIRE(buffer.Data() != nullptr);
	REQUIRE(reinterpret_cast<std::uintptr_t>(buffer.Data()) % buffer.Alignment() == 0);

	for (std::size_t i = 0; i < buffer.Size(); ++i)
	{
		REQUIRE(buffer.Data()[i] == std::byte {});
	}
}
