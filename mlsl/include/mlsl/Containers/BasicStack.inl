// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <new>
#include <utility>

namespace mlsl
{
	template <typename T, typename Derived>
	typename BasicStack<T, Derived>::Reference BasicStack<T, Derived>::Top()
	{
		return AsDerived().GetStorage()[m_Size - 1];
	}

	template <typename T, typename Derived>
	typename BasicStack<T, Derived>::ConstReference BasicStack<T, Derived>::Top() const
	{
		return AsDerived().GetStorage()[m_Size - 1];
	}

	template <typename T, typename Derived>
	typename BasicStack<T, Derived>::Iterator BasicStack<T, Derived>::Begin()
	{
		return AsDerived().GetStorage();
	}

	template <typename T, typename Derived>
	typename BasicStack<T, Derived>::ConstIterator BasicStack<T, Derived>::Begin() const
	{
		return AsDerived().GetStorage();
	}

	template <typename T, typename Derived>
	typename BasicStack<T, Derived>::Iterator BasicStack<T, Derived>::End()
	{
		return AsDerived().GetStorage() + m_Size;
	}

	template <typename T, typename Derived>
	typename BasicStack<T, Derived>::ConstIterator BasicStack<T, Derived>::End() const
	{
		return AsDerived().GetStorage() + m_Size;
	}

	template <typename T, typename Derived>
	typename BasicStack<T, Derived>::SizeType BasicStack<T, Derived>::Size() const
	{
		return m_Size;
	}

	template <typename T, typename Derived>
	bool BasicStack<T, Derived>::Empty() const
	{
		return m_Size == 0;
	}

	template <typename T, typename Derived>
	std::expected<void, Error> BasicStack<T, Derived>::Add(const T &value)
	{
		auto result = AsDerived().EnsureCapacity(m_Size + 1);

		if (!result)
		{
			return std::unexpected(result.error());
		}

		new (&AsDerived().GetStorage()[m_Size]) T(value);
		++m_Size;

		return {};
	}

	template <typename T, typename Derived>
	std::expected<void, Error> BasicStack<T, Derived>::Add(T &&value)
	{
		auto result = AsDerived().EnsureCapacity(m_Size + 1);

		if (!result)
		{
			return std::unexpected(result.error());
		}

		new (&AsDerived().GetStorage()[m_Size]) T(std::move(value));
		++m_Size;

		return {};
	}

	template <typename T, typename Derived>
	void BasicStack<T, Derived>::Remove()
	{
		if (m_Size == 0)
		{
			return;
		}

		--m_Size;
		AsDerived().GetStorage()[m_Size].~T();
	}

	template <typename T, typename Derived>
	void BasicStack<T, Derived>::Clear()
	{
		while (m_Size > 0)
		{
			Remove();
		}
	}

	template <typename T, typename Derived>
	Derived &BasicStack<T, Derived>::AsDerived()
	{
		return static_cast<Derived &>(*this);
	}

	template <typename T, typename Derived>
	const Derived &BasicStack<T, Derived>::AsDerived() const
	{
		return static_cast<const Derived &>(*this);
	}
} // namespace mlsl
