// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace mlsl
{
	inline void SharedControlBlock::ReleaseObject()
	{
		if (object and releaseObject)
		{
			releaseObject(object);
			object = nullptr;
		}
	}

	template <typename T>
	void SharedControlBlock::ReleaseTypedObject(void *object)
	{
		delete static_cast<T *>(object);
	}
} // namespace mlsl
