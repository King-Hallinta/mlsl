// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <new>
#include <utility>

namespace mlsl
{
	template <typename T, typename Derived>
	std::expected<typename BasicDeque<T, Derived>::Pointer, Error> BasicDeque<T, Derived>::Get(SizeType index)
	{
		if (index >= m_Size)
		{
			return std::unexpected(Error {ErrorType::OutOfBounds});
		}

		return &AsDerived().GetStorage()[MapIndex(index)];
	}

	template <typename T, typename Derived>
	std::expected<typename BasicDeque<T, Derived>::ConstPointer, Error> BasicDeque<T, Derived>::Get(SizeType index) const
	{
		if (index >= m_Size)
		{
			return std::unexpected(Error {ErrorType::OutOfBounds});
		}

		return &AsDerived().GetStorage()[MapIndex(index)];
	}

	template <typename T, typename Derived>
	typename BasicDeque<T, Derived>::Reference BasicDeque<T, Derived>::operator[](SizeType index)
	{
		return AsDerived().GetStorage()[MapIndex(index)];
	}

	template <typename T, typename Derived>
	typename BasicDeque<T, Derived>::ConstReference BasicDeque<T, Derived>::operator[](SizeType index) const
	{
		return AsDerived().GetStorage()[MapIndex(index)];
	}

	template <typename T, typename Derived>
	typename BasicDeque<T, Derived>::Reference BasicDeque<T, Derived>::Front()
	{
		return AsDerived().GetStorage()[m_Head];
	}

	template <typename T, typename Derived>
	typename BasicDeque<T, Derived>::ConstReference BasicDeque<T, Derived>::Front() const
	{
		return AsDerived().GetStorage()[m_Head];
	}

	template <typename T, typename Derived>
	typename BasicDeque<T, Derived>::Reference BasicDeque<T, Derived>::Back()
	{
		return AsDerived().GetStorage()[MapIndex(m_Size - 1)];
	}

	template <typename T, typename Derived>
	typename BasicDeque<T, Derived>::ConstReference BasicDeque<T, Derived>::Back() const
	{
		return AsDerived().GetStorage()[MapIndex(m_Size - 1)];
	}

	template <typename T, typename Derived>
	typename BasicDeque<T, Derived>::SizeType BasicDeque<T, Derived>::Size() const
	{
		return m_Size;
	}

	template <typename T, typename Derived>
	bool BasicDeque<T, Derived>::Empty() const
	{
		return m_Size == 0;
	}

	template <typename T, typename Derived>
	std::expected<void, Error> BasicDeque<T, Derived>::Insert(const T &value)
	{
		auto result = AsDerived().EnsureCapacity(m_Size + 1);

		if (not result)
		{
			return std::unexpected(result.error());
		}

		auto capacity = AsDerived().GetCapacity();
		m_Head = (m_Head + capacity - 1) % capacity;
		new (&AsDerived().GetStorage()[m_Head]) T(value);
		++m_Size;

		return {};
	}

	template <typename T, typename Derived>
	std::expected<void, Error> BasicDeque<T, Derived>::Insert(T &&value)
	{
		auto result = AsDerived().EnsureCapacity(m_Size + 1);

		if (not result)
		{
			return std::unexpected(result.error());
		}

		auto capacity = AsDerived().GetCapacity();
		m_Head = (m_Head + capacity - 1) % capacity;
		new (&AsDerived().GetStorage()[m_Head]) T(std::move(value));
		++m_Size;

		return {};
	}

	template <typename T, typename Derived>
	std::expected<void, Error> BasicDeque<T, Derived>::Append(const T &value)
	{
		auto result = AsDerived().EnsureCapacity(m_Size + 1);

		if (not result)
		{
			return std::unexpected(result.error());
		}

		auto idx = MapIndex(m_Size);
		new (&AsDerived().GetStorage()[idx]) T(value);
		++m_Size;

		return {};
	}

	template <typename T, typename Derived>
	std::expected<void, Error> BasicDeque<T, Derived>::Append(T &&value)
	{
		auto result = AsDerived().EnsureCapacity(m_Size + 1);

		if (not result)
		{
			return std::unexpected(result.error());
		}

		auto idx = MapIndex(m_Size);
		new (&AsDerived().GetStorage()[idx]) T(std::move(value));
		++m_Size;

		return {};
	}

	template <typename T, typename Derived>
	void BasicDeque<T, Derived>::RemoveFront()
	{
		if (m_Size == 0)
		{
			return;
		}

		AsDerived().GetStorage()[m_Head].~T();
		m_Head = (m_Head + 1) % AsDerived().GetCapacity();
		--m_Size;
	}

	template <typename T, typename Derived>
	void BasicDeque<T, Derived>::RemoveBack()
	{
		if (m_Size == 0)
		{
			return;
		}

		auto idx = MapIndex(m_Size - 1);
		AsDerived().GetStorage()[idx].~T();
		--m_Size;
	}

	template <typename T, typename Derived>
	void BasicDeque<T, Derived>::Clear()
	{
		while (m_Size > 0)
		{
			RemoveBack();
		}

		m_Head = 0;
	}

	template <typename T, typename Derived>
	Derived &BasicDeque<T, Derived>::AsDerived()
	{
		return static_cast<Derived &>(*this);
	}

	template <typename T, typename Derived>
	const Derived &BasicDeque<T, Derived>::AsDerived() const
	{
		return static_cast<const Derived &>(*this);
	}

	template <typename T, typename Derived>
	typename BasicDeque<T, Derived>::SizeType BasicDeque<T, Derived>::MapIndex(SizeType index) const
	{
		return (m_Head + index) % AsDerived().GetCapacity();
	}
} // namespace mlsl
