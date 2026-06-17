// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace mlsl
{
	template <typename KeyT, std::size_t N, typename HashT, typename EqualT>
	FixedHashSet<KeyT, N, HashT, EqualT>::FixedHashSet(HashT hash, EqualT equal)
		: Base(std::move(hash), std::move(equal))
	{
	}

	template <typename KeyT, std::size_t N, typename HashT, typename EqualT>
	FixedHashSet<KeyT, N, HashT, EqualT>::FixedHashSet(const FixedHashSet &other)
		: Base(other.m_Hash, other.m_Equal)
	{
		CopyFrom(other);
	}

	template <typename KeyT, std::size_t N, typename HashT, typename EqualT>
	FixedHashSet<KeyT, N, HashT, EqualT>::FixedHashSet(FixedHashSet &&other) noexcept
		: Base(std::move(other.m_Hash), std::move(other.m_Equal))
	{
		MoveFrom(other);
	}

	template <typename KeyT, std::size_t N, typename HashT, typename EqualT>
	FixedHashSet<KeyT, N, HashT, EqualT>::~FixedHashSet()
	{
		this->Clear();
	}

	template <typename KeyT, std::size_t N, typename HashT, typename EqualT>
	FixedHashSet<KeyT, N, HashT, EqualT> &FixedHashSet<KeyT, N, HashT, EqualT>::operator=(const FixedHashSet &other)
	{
		if (this != &other)
		{
			this->Clear();
			this->m_Hash = other.m_Hash;
			this->m_Equal = other.m_Equal;
			CopyFrom(other);
		}

		return *this;
	}

	template <typename KeyT, std::size_t N, typename HashT, typename EqualT>
	FixedHashSet<KeyT, N, HashT, EqualT> &FixedHashSet<KeyT, N, HashT, EqualT>::operator=(FixedHashSet &&other) noexcept
	{
		if (this != &other)
		{
			this->Clear();
			this->m_Hash = std::move(other.m_Hash);
			this->m_Equal = std::move(other.m_Equal);
			MoveFrom(other);
		}

		return *this;
	}

	template <typename KeyT, std::size_t N, typename HashT, typename EqualT>
	typename FixedHashSet<KeyT, N, HashT, EqualT>::Slot *FixedHashSet<KeyT, N, HashT, EqualT>::GetSlots()
	{
		return m_Slots;
	}

	template <typename KeyT, std::size_t N, typename HashT, typename EqualT>
	const typename FixedHashSet<KeyT, N, HashT, EqualT>::Slot *FixedHashSet<KeyT, N, HashT, EqualT>::GetSlots() const
	{
		return m_Slots;
	}

	template <typename KeyT, std::size_t N, typename HashT, typename EqualT>
	typename FixedHashSet<KeyT, N, HashT, EqualT>::SizeType FixedHashSet<KeyT, N, HashT, EqualT>::GetSlotCount() const
	{
		return SlotCount;
	}

	template <typename KeyT, std::size_t N, typename HashT, typename EqualT>
	typename FixedHashSet<KeyT, N, HashT, EqualT>::SizeType FixedHashSet<KeyT, N, HashT, EqualT>::GetCapacity() const
	{
		return N;
	}

	template <typename KeyT, std::size_t N, typename HashT, typename EqualT>
	std::expected<void, Error> FixedHashSet<KeyT, N, HashT, EqualT>::EnsureCapacity(SizeType capacity)
	{
		if (capacity > N)
		{
			return std::unexpected(Error {ErrorType::BufferOverflow});
		}

		return {};
	}

	template <typename KeyT, std::size_t N, typename HashT, typename EqualT>
	void FixedHashSet<KeyT, N, HashT, EqualT>::CopyFrom(const FixedHashSet &other)
	{
		for (SizeType i = 0; i < other.GetSlotCount(); ++i)
		{
			if (other.m_Slots[i].status == Base::SlotStatus::Occupied)
			{
				static_cast<void>(this->Add(*Base::KeyFrom(other.m_Slots[i])));
			}
		}
	}

	template <typename KeyT, std::size_t N, typename HashT, typename EqualT>
	void FixedHashSet<KeyT, N, HashT, EqualT>::MoveFrom(FixedHashSet &other)
	{
		for (SizeType i = 0; i < other.GetSlotCount(); ++i)
		{
			if (other.m_Slots[i].status == Base::SlotStatus::Occupied)
			{
				static_cast<void>(this->Add(std::move(*Base::KeyFrom(other.m_Slots[i]))));
			}
		}

		other.Clear();
	}
} // namespace mlsl
