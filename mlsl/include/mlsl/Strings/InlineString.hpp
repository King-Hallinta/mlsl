// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mlsl/Strings/BasicInlineString.hpp>

namespace mlsl
{
	template <std::size_t N>
	using InlineString = BasicInlineString<char, N>;

	template <std::size_t N>
	using InlineWString = BasicInlineString<wchar_t, N>;
} // namespace mlsl
