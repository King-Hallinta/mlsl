// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace mlsl
{
	template <typename T>
	constexpr Span<T>::Span(Pointer data, SizeType size)
		: m_Data(data), m_Size(size)
	{
	}

	template <typename T>
	template <std::size_t N>
	constexpr Span<T>::Span(T (&arr)[N])
		: m_Data(arr), m_Size(N)
	{
	}

	template <typename T>
	constexpr std::expected<typename Span<T>::Pointer, Error> Span<T>::Get(SizeType index)
	{
		if (index >= m_Size)
		{
			return std::unexpected(Error {ErrorType::OutOfBounds});
		}

		return m_Data + index;
	}

	template <typename T>
	constexpr std::expected<typename Span<T>::ConstPointer, Error> Span<T>::Get(SizeType index) const
	{
		if (index >= m_Size)
		{
			return std::unexpected(Error {ErrorType::OutOfBounds});
		}

		return m_Data + index;
	}

	template <typename T>
	constexpr typename Span<T>::Reference Span<T>::operator[](SizeType index)
	{
		return m_Data[index];
	}

	template <typename T>
	constexpr typename Span<T>::ConstReference Span<T>::operator[](SizeType index) const
	{
		return m_Data[index];
	}

	template <typename T>
	constexpr typename Span<T>::Reference Span<T>::Front()
	{
		return m_Data[0];
	}

	template <typename T>
	constexpr typename Span<T>::ConstReference Span<T>::Front() const
	{
		return m_Data[0];
	}

	template <typename T>
	constexpr typename Span<T>::Reference Span<T>::Back()
	{
		return m_Data[m_Size - 1];
	}

	template <typename T>
	constexpr typename Span<T>::ConstReference Span<T>::Back() const
	{
		return m_Data[m_Size - 1];
	}

	template <typename T>
	constexpr typename Span<T>::Pointer Span<T>::Data()
	{
		return m_Data;
	}

	template <typename T>
	constexpr typename Span<T>::ConstPointer Span<T>::Data() const
	{
		return m_Data;
	}

	template <typename T>
	constexpr typename Span<T>::Iterator Span<T>::Begin()
	{
		return m_Data;
	}

	template <typename T>
	constexpr typename Span<T>::ConstIterator Span<T>::Begin() const
	{
		return m_Data;
	}

	template <typename T>
	constexpr typename Span<T>::Iterator Span<T>::End()
	{
		return m_Data + m_Size;
	}

	template <typename T>
	constexpr typename Span<T>::ConstIterator Span<T>::End() const
	{
		return m_Data + m_Size;
	}

	template <typename T>
	constexpr typename Span<T>::SizeType Span<T>::Size() const
	{
		return m_Size;
	}

	template <typename T>
	constexpr typename Span<T>::SizeType Span<T>::SizeBytes() const
	{
		return m_Size * sizeof(T);
	}

	template <typename T>
	constexpr bool Span<T>::Empty() const
	{
		return m_Size == 0;
	}

	template <typename T>
	constexpr Span<T> Span<T>::Subspan(SizeType offset, SizeType count) const
	{
		return Span(m_Data + offset, count);
	}

	template <typename T>
	constexpr Span<T> Span<T>::First(SizeType count) const
	{
		return Span(m_Data, count);
	}

	template <typename T>
	constexpr Span<T> Span<T>::Last(SizeType count) const
	{
		return Span(m_Data + m_Size - count, count);
	}
} // namespace mlsl
