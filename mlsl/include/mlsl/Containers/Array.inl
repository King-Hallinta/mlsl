// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace mlsl
{
	template <typename T, std::size_t N>
	constexpr Array<T, N>::Array(std::initializer_list<T> init)
	{
		SizeType i = 0;
		for (const auto &value : init)
		{
			if (i >= N)
			{
				break;
			}

			m_Data[i++] = value;
		}
	}

	template <typename T, std::size_t N>
	constexpr std::expected<typename Array<T, N>::Pointer, Error> Array<T, N>::Get(SizeType index)
	{
		if (index >= N)
		{
			return std::unexpected(Error {ErrorType::OutOfBounds});
		}

		return m_Data + index;
	}

	template <typename T, std::size_t N>
	constexpr std::expected<typename Array<T, N>::ConstPointer, Error> Array<T, N>::Get(SizeType index) const
	{
		if (index >= N)
		{
			return std::unexpected(Error {ErrorType::OutOfBounds});
		}

		return m_Data + index;
	}

	template <typename T, std::size_t N>
	constexpr typename Array<T, N>::Reference Array<T, N>::operator[](SizeType index)
	{
		return m_Data[index];
	}

	template <typename T, std::size_t N>
	constexpr typename Array<T, N>::ConstReference Array<T, N>::operator[](SizeType index) const
	{
		return m_Data[index];
	}

	template <typename T, std::size_t N>
	constexpr typename Array<T, N>::Reference Array<T, N>::Front()
	{
		return m_Data[0];
	}

	template <typename T, std::size_t N>
	constexpr typename Array<T, N>::ConstReference Array<T, N>::Front() const
	{
		return m_Data[0];
	}

	template <typename T, std::size_t N>
	constexpr typename Array<T, N>::Reference Array<T, N>::Back()
	{
		return m_Data[N - 1];
	}

	template <typename T, std::size_t N>
	constexpr typename Array<T, N>::ConstReference Array<T, N>::Back() const
	{
		return m_Data[N - 1];
	}

	template <typename T, std::size_t N>
	constexpr typename Array<T, N>::Pointer Array<T, N>::Data()
	{
		return m_Data;
	}

	template <typename T, std::size_t N>
	constexpr typename Array<T, N>::ConstPointer Array<T, N>::Data() const
	{
		return m_Data;
	}

	template <typename T, std::size_t N>
	constexpr typename Array<T, N>::Iterator Array<T, N>::Begin()
	{
		return m_Data;
	}

	template <typename T, std::size_t N>
	constexpr typename Array<T, N>::ConstIterator Array<T, N>::Begin() const
	{
		return m_Data;
	}

	template <typename T, std::size_t N>
	constexpr typename Array<T, N>::Iterator Array<T, N>::End()
	{
		return m_Data + N;
	}

	template <typename T, std::size_t N>
	constexpr typename Array<T, N>::ConstIterator Array<T, N>::End() const
	{
		return m_Data + N;
	}

	template <typename T, std::size_t N>
	constexpr typename Array<T, N>::SizeType Array<T, N>::Size() const
	{
		return N;
	}

	template <typename T, std::size_t N>
	constexpr bool Array<T, N>::Empty() const
	{
		return N == 0;
	}

	template <typename T, std::size_t N>
	constexpr void Array<T, N>::Fill(const T &value)
	{
		for (SizeType i = 0; i < N; ++i)
		{
			m_Data[i] = value;
		}
	}
} // namespace mlsl
