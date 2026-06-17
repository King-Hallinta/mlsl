// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace mlsl
{
	template <typename T, std::size_t N>
	FixedDeque<T, N>::FixedDeque(const FixedDeque &other)
	{
		for (SizeType i = 0; i < other.m_Size; ++i)
		{
			static_cast<void>(this->Append(other[i]));
		}
	}

	template <typename T, std::size_t N>
	FixedDeque<T, N>::FixedDeque(FixedDeque &&other) noexcept
	{
		for (SizeType i = 0; i < other.m_Size; ++i)
		{
			static_cast<void>(this->Append(std::move(other[i])));
		}

		other.Clear();
	}

	template <typename T, std::size_t N>
	FixedDeque<T, N>::~FixedDeque()
	{
		this->Clear();
	}

	template <typename T, std::size_t N>
	FixedDeque<T, N> &FixedDeque<T, N>::operator=(const FixedDeque &other)
	{
		if (this != &other)
		{
			this->Clear();

			for (SizeType i = 0; i < other.m_Size; ++i)
			{
				static_cast<void>(this->Append(other[i]));
			}
		}

		return *this;
	}

	template <typename T, std::size_t N>
	FixedDeque<T, N> &FixedDeque<T, N>::operator=(FixedDeque &&other) noexcept
	{
		if (this != &other)
		{
			this->Clear();

			for (SizeType i = 0; i < other.m_Size; ++i)
			{
				static_cast<void>(this->Append(std::move(other[i])));
			}

			other.Clear();
		}

		return *this;
	}

	template <typename T, std::size_t N>
	T *FixedDeque<T, N>::GetStorage()
	{
		return reinterpret_cast<T *>(m_Storage);
	}

	template <typename T, std::size_t N>
	const T *FixedDeque<T, N>::GetStorage() const
	{
		return reinterpret_cast<const T *>(m_Storage);
	}

	template <typename T, std::size_t N>
	typename FixedDeque<T, N>::SizeType FixedDeque<T, N>::GetCapacity() const
	{
		return N;
	}

	template <typename T, std::size_t N>
	std::expected<void, Error> FixedDeque<T, N>::EnsureCapacity(SizeType required)
	{
		if (required > N)
		{
			return std::unexpected(Error {ErrorType::OutOfMemory});
		}

		return {};
	}

	template <typename T, std::size_t N>
	typename FixedDeque<T, N>::SizeType FixedDeque<T, N>::Capacity() const
	{
		return N;
	}
} // namespace mlsl
