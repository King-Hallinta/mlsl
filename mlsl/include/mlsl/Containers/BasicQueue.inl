// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <utility>

namespace mlsl
{
	template <typename T, typename Derived>
	typename BasicQueue<T, Derived>::Reference BasicQueue<T, Derived>::Front()
	{
		return Base::Front();
	}

	template <typename T, typename Derived>
	typename BasicQueue<T, Derived>::ConstReference BasicQueue<T, Derived>::Front() const
	{
		return Base::Front();
	}

	template <typename T, typename Derived>
	typename BasicQueue<T, Derived>::Reference BasicQueue<T, Derived>::Back()
	{
		return Base::Back();
	}

	template <typename T, typename Derived>
	typename BasicQueue<T, Derived>::ConstReference BasicQueue<T, Derived>::Back() const
	{
		return Base::Back();
	}

	template <typename T, typename Derived>
	typename BasicQueue<T, Derived>::SizeType BasicQueue<T, Derived>::Size() const
	{
		return Base::Size();
	}

	template <typename T, typename Derived>
	bool BasicQueue<T, Derived>::Empty() const
	{
		return Base::Empty();
	}

	template <typename T, typename Derived>
	std::expected<void, Error> BasicQueue<T, Derived>::Add(const T &value)
	{
		return Base::AddBack(value);
	}

	template <typename T, typename Derived>
	std::expected<void, Error> BasicQueue<T, Derived>::Add(T &&value)
	{
		return Base::AddBack(std::move(value));
	}

	template <typename T, typename Derived>
	void BasicQueue<T, Derived>::Remove()
	{
		Base::RemoveFront();
	}

	template <typename T, typename Derived>
	void BasicQueue<T, Derived>::Clear()
	{
		Base::Clear();
	}

	template <typename T, typename Derived>
	typename BasicQueue<T, Derived>::SizeType BasicQueue<T, Derived>::MapIndex(SizeType index) const
	{
		return Base::MapIndex(index);
	}
} // namespace mlsl
