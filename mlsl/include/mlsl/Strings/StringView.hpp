// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mlsl/Strings/BasicStringView.hpp>

namespace mlsl
{
	using StringView = BasicStringView<char>;
	using WStringView = BasicStringView<wchar_t>;
} // namespace mlsl
