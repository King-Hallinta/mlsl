// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace mlsl
{
	template <typename T, std::size_t N>
	FixedQueue<T, N>::FixedQueue(const FixedQueue &other)
	{
		for (SizeType i = 0; i < other.m_Size; ++i)
		{
			static_cast<void>(this->Add(other.GetStorage()[other.MapIndex(i)]));
		}
	}

	template <typename T, std::size_t N>
	FixedQueue<T, N>::FixedQueue(FixedQueue &&other) noexcept
	{
		for (SizeType i = 0; i < other.m_Size; ++i)
		{
			static_cast<void>(this->Add(std::move(other.GetStorage()[other.MapIndex(i)])));
		}

		other.Clear();
	}

	template <typename T, std::size_t N>
	FixedQueue<T, N>::~FixedQueue()
	{
		this->Clear();
	}

	template <typename T, std::size_t N>
	FixedQueue<T, N> &FixedQueue<T, N>::operator=(const FixedQueue &other)
	{
		if (this != &other)
		{
			this->Clear();

			for (SizeType i = 0; i < other.m_Size; ++i)
			{
				static_cast<void>(this->Add(other.GetStorage()[other.MapIndex(i)]));
			}
		}

		return *this;
	}

	template <typename T, std::size_t N>
	FixedQueue<T, N> &FixedQueue<T, N>::operator=(FixedQueue &&other) noexcept
	{
		if (this != &other)
		{
			this->Clear();

			for (SizeType i = 0; i < other.m_Size; ++i)
			{
				static_cast<void>(this->Add(std::move(other.GetStorage()[other.MapIndex(i)])));
			}

			other.Clear();
		}

		return *this;
	}

	template <typename T, std::size_t N>
	T *FixedQueue<T, N>::GetStorage()
	{
		return reinterpret_cast<T *>(m_Storage);
	}

	template <typename T, std::size_t N>
	const T *FixedQueue<T, N>::GetStorage() const
	{
		return reinterpret_cast<const T *>(m_Storage);
	}

	template <typename T, std::size_t N>
	typename FixedQueue<T, N>::SizeType FixedQueue<T, N>::GetCapacity() const
	{
		return N;
	}

	template <typename T, std::size_t N>
	std::expected<void, Error> FixedQueue<T, N>::EnsureCapacity(SizeType required)
	{
		if (required > N)
		{
			return std::unexpected(Error {ErrorType::OutOfMemory});
		}

		return {};
	}

	template <typename T, std::size_t N>
	typename FixedQueue<T, N>::SizeType FixedQueue<T, N>::Capacity() const
	{
		return N;
	}
} // namespace mlsl
