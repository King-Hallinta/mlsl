// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <utility>

namespace mlsl
{
	template <typename T, typename Derived>
	std::expected<typename BasicRingBuffer<T, Derived>::Pointer, Error> BasicRingBuffer<T, Derived>::Get(SizeType index)
	{
		return Base::Get(index);
	}

	template <typename T, typename Derived>
	std::expected<typename BasicRingBuffer<T, Derived>::ConstPointer, Error> BasicRingBuffer<T, Derived>::Get(SizeType index) const
	{
		return Base::Get(index);
	}

	template <typename T, typename Derived>
	typename BasicRingBuffer<T, Derived>::Reference BasicRingBuffer<T, Derived>::operator[](SizeType index)
	{
		return Base::operator[](index);
	}

	template <typename T, typename Derived>
	typename BasicRingBuffer<T, Derived>::ConstReference BasicRingBuffer<T, Derived>::operator[](SizeType index) const
	{
		return Base::operator[](index);
	}

	template <typename T, typename Derived>
	typename BasicRingBuffer<T, Derived>::Reference BasicRingBuffer<T, Derived>::Front()
	{
		return Base::Front();
	}

	template <typename T, typename Derived>
	typename BasicRingBuffer<T, Derived>::ConstReference BasicRingBuffer<T, Derived>::Front() const
	{
		return Base::Front();
	}

	template <typename T, typename Derived>
	typename BasicRingBuffer<T, Derived>::Reference BasicRingBuffer<T, Derived>::Back()
	{
		return Base::Back();
	}

	template <typename T, typename Derived>
	typename BasicRingBuffer<T, Derived>::ConstReference BasicRingBuffer<T, Derived>::Back() const
	{
		return Base::Back();
	}

	template <typename T, typename Derived>
	typename BasicRingBuffer<T, Derived>::Iterator BasicRingBuffer<T, Derived>::Begin()
	{
		return Base::Begin();
	}

	template <typename T, typename Derived>
	typename BasicRingBuffer<T, Derived>::ConstIterator BasicRingBuffer<T, Derived>::Begin() const
	{
		return Base::Begin();
	}

	template <typename T, typename Derived>
	typename BasicRingBuffer<T, Derived>::Iterator BasicRingBuffer<T, Derived>::End()
	{
		return Base::End();
	}

	template <typename T, typename Derived>
	typename BasicRingBuffer<T, Derived>::ConstIterator BasicRingBuffer<T, Derived>::End() const
	{
		return Base::End();
	}

	template <typename T, typename Derived>
	typename BasicRingBuffer<T, Derived>::SizeType BasicRingBuffer<T, Derived>::Size() const
	{
		return Base::Size();
	}

	template <typename T, typename Derived>
	bool BasicRingBuffer<T, Derived>::Empty() const
	{
		return Base::Empty();
	}

	template <typename T, typename Derived>
	std::expected<void, Error> BasicRingBuffer<T, Derived>::Add(const T &value)
	{
		return Base::Append(value);
	}

	template <typename T, typename Derived>
	std::expected<void, Error> BasicRingBuffer<T, Derived>::Add(T &&value)
	{
		return Base::Append(std::move(value));
	}

	template <typename T, typename Derived>
	void BasicRingBuffer<T, Derived>::Remove()
	{
		Base::RemoveFront();
	}

	template <typename T, typename Derived>
	void BasicRingBuffer<T, Derived>::Clear()
	{
		Base::Clear();
	}

	template <typename T, typename Derived>
	typename BasicRingBuffer<T, Derived>::SizeType BasicRingBuffer<T, Derived>::MapIndex(SizeType index) const
	{
		return Base::MapIndex(index);
	}
} // namespace mlsl
