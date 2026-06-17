// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace mlsl
{
	template <typename T, std::size_t N>
	FixedStack<T, N>::FixedStack(const FixedStack &other)
	{
		for (SizeType i = 0; i < other.m_Size; ++i)
		{
			static_cast<void>(this->Add(other.GetStorage()[i]));
		}
	}

	template <typename T, std::size_t N>
	FixedStack<T, N>::FixedStack(FixedStack &&other) noexcept
	{
		for (SizeType i = 0; i < other.m_Size; ++i)
		{
			static_cast<void>(this->Add(std::move(other.GetStorage()[i])));
		}

		other.Clear();
	}

	template <typename T, std::size_t N>
	FixedStack<T, N>::~FixedStack()
	{
		this->Clear();
	}

	template <typename T, std::size_t N>
	FixedStack<T, N> &FixedStack<T, N>::operator=(const FixedStack &other)
	{
		if (this != &other)
		{
			this->Clear();

			for (SizeType i = 0; i < other.m_Size; ++i)
			{
				static_cast<void>(this->Add(other.GetStorage()[i]));
			}
		}

		return *this;
	}

	template <typename T, std::size_t N>
	FixedStack<T, N> &FixedStack<T, N>::operator=(FixedStack &&other) noexcept
	{
		if (this != &other)
		{
			this->Clear();

			for (SizeType i = 0; i < other.m_Size; ++i)
			{
				static_cast<void>(this->Add(std::move(other.GetStorage()[i])));
			}

			other.Clear();
		}

		return *this;
	}

	template <typename T, std::size_t N>
	T *FixedStack<T, N>::GetStorage()
	{
		return reinterpret_cast<T *>(m_Storage);
	}

	template <typename T, std::size_t N>
	const T *FixedStack<T, N>::GetStorage() const
	{
		return reinterpret_cast<const T *>(m_Storage);
	}

	template <typename T, std::size_t N>
	typename FixedStack<T, N>::SizeType FixedStack<T, N>::GetCapacity() const
	{
		return N;
	}

	template <typename T, std::size_t N>
	std::expected<void, Error> FixedStack<T, N>::EnsureCapacity(SizeType required)
	{
		if (required > N)
		{
			return std::unexpected(Error {ErrorType::OutOfMemory});
		}

		return {};
	}

	template <typename T, std::size_t N>
	typename FixedStack<T, N>::SizeType FixedStack<T, N>::Capacity() const
	{
		return N;
	}
} // namespace mlsl
