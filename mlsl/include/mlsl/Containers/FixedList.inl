// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace mlsl
{
	template <typename T, std::size_t N>
	FixedList<T, N>::FixedList()
	{
		for (std::size_t i = 0; i < N - 1; ++i)
		{
			m_Storage[i].next = &m_Storage[i + 1];
		}

		m_Storage[N - 1].next = nullptr;
		m_FreeList = &m_Storage[0];
	}

	template <typename T, std::size_t N>
	FixedList<T, N>::FixedList(const FixedList &other)
		: FixedList()
	{
		for (auto it = other.Begin(); it != other.End(); ++it)
		{
			static_cast<void>(this->Append(*it));
		}
	}

	template <typename T, std::size_t N>
	FixedList<T, N>::FixedList(FixedList &&other) noexcept
		: FixedList()
	{
		for (auto it = other.Begin(); it != other.End(); ++it)
		{
			static_cast<void>(this->Append(std::move(*it)));
		}

		other.Clear();
	}

	template <typename T, std::size_t N>
	FixedList<T, N>::~FixedList()
	{
		this->Clear();
	}

	template <typename T, std::size_t N>
	FixedList<T, N> &FixedList<T, N>::operator=(const FixedList &other)
	{
		if (this != &other)
		{
			this->Clear();

			for (auto it = other.Begin(); it != other.End(); ++it)
			{
				static_cast<void>(this->Append(*it));
			}
		}

		return *this;
	}

	template <typename T, std::size_t N>
	FixedList<T, N> &FixedList<T, N>::operator=(FixedList &&other) noexcept
	{
		if (this != &other)
		{
			this->Clear();

			for (auto it = other.Begin(); it != other.End(); ++it)
			{
				static_cast<void>(this->Append(std::move(*it)));
			}

			other.Clear();
		}

		return *this;
	}

	template <typename T, std::size_t N>
	std::expected<typename FixedList<T, N>::NodeType *, Error> FixedList<T, N>::AllocateNode()
	{
		if (m_FreeList == nullptr)
		{
			return std::unexpected(Error {ErrorType::OutOfMemory});
		}

		Slot *slot = m_FreeList;
		m_FreeList = slot->next;

		return reinterpret_cast<NodeType *>(slot->storage);
	}

	template <typename T, std::size_t N>
	void FixedList<T, N>::DeallocateNode(NodeType *node)
	{
		Slot *slot = reinterpret_cast<Slot *>(node);
		slot->next = m_FreeList;
		m_FreeList = slot;
	}

	template <typename T, std::size_t N>
	typename FixedList<T, N>::SizeType FixedList<T, N>::Capacity() const
	{
		return N;
	}
} // namespace mlsl
