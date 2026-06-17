// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mlsl/Strings/BasicFixedString.hpp>

namespace mlsl
{
	template <std::size_t N>
	using FixedString = BasicFixedString<char, N>;

	template <std::size_t N>
	using FixedWString = BasicFixedString<wchar_t, N>;
} // namespace mlsl
