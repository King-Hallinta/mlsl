// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstdint>

namespace mlsl
{
	struct SharedControlBlock
	{
		using ReleaseObjectFunction = void (*)(void *);

		void ReleaseObject();

		template <typename T>
		static void ReleaseTypedObject(void *object);

		void *object = nullptr;
		ReleaseObjectFunction releaseObject = nullptr;
		std::uint32_t strongCount = 1;
		std::uint32_t weakCount = 0;
	};
} // namespace mlsl

#include <mlsl/Memory/SharedControlBlock.inl>
