// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace mlsl
{
	template <typename KeyT, typename ValueT, std::size_t N, typename HashT, typename EqualT>
	FixedHashMap<KeyT, ValueT, N, HashT, EqualT>::FixedHashMap(HashT hash, EqualT equal)
		: Base(std::move(hash), std::move(equal))
	{
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename HashT, typename EqualT>
	FixedHashMap<KeyT, ValueT, N, HashT, EqualT>::FixedHashMap(const FixedHashMap &other)
		: Base(other.m_Hash, other.m_Equal)
	{
		CopyFrom(other);
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename HashT, typename EqualT>
	FixedHashMap<KeyT, ValueT, N, HashT, EqualT>::FixedHashMap(FixedHashMap &&other) noexcept
		: Base(std::move(other.m_Hash), std::move(other.m_Equal))
	{
		MoveFrom(other);
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename HashT, typename EqualT>
	FixedHashMap<KeyT, ValueT, N, HashT, EqualT>::~FixedHashMap()
	{
		this->Clear();
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename HashT, typename EqualT>
	FixedHashMap<KeyT, ValueT, N, HashT, EqualT> &FixedHashMap<KeyT, ValueT, N, HashT, EqualT>::operator=(const FixedHashMap &other)
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

	template <typename KeyT, typename ValueT, std::size_t N, typename HashT, typename EqualT>
	FixedHashMap<KeyT, ValueT, N, HashT, EqualT> &FixedHashMap<KeyT, ValueT, N, HashT, EqualT>::operator=(FixedHashMap &&other) noexcept
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

	template <typename KeyT, typename ValueT, std::size_t N, typename HashT, typename EqualT>
	typename FixedHashMap<KeyT, ValueT, N, HashT, EqualT>::Slot *FixedHashMap<KeyT, ValueT, N, HashT, EqualT>::GetSlots()
	{
		return m_Slots;
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename HashT, typename EqualT>
	const typename FixedHashMap<KeyT, ValueT, N, HashT, EqualT>::Slot *FixedHashMap<KeyT, ValueT, N, HashT, EqualT>::GetSlots() const
	{
		return m_Slots;
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename HashT, typename EqualT>
	typename FixedHashMap<KeyT, ValueT, N, HashT, EqualT>::SizeType FixedHashMap<KeyT, ValueT, N, HashT, EqualT>::GetSlotCount() const
	{
		return SlotCount;
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename HashT, typename EqualT>
	typename FixedHashMap<KeyT, ValueT, N, HashT, EqualT>::SizeType FixedHashMap<KeyT, ValueT, N, HashT, EqualT>::GetCapacity() const
	{
		return N;
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename HashT, typename EqualT>
	std::expected<void, Error> FixedHashMap<KeyT, ValueT, N, HashT, EqualT>::EnsureCapacity(SizeType capacity)
	{
		if (capacity > N)
		{
			return std::unexpected(Error {ErrorType::BufferOverflow});
		}

		return {};
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename HashT, typename EqualT>
	void FixedHashMap<KeyT, ValueT, N, HashT, EqualT>::CopyFrom(const FixedHashMap &other)
	{
		for (SizeType i = 0; i < other.GetSlotCount(); ++i)
		{
			if (other.m_Slots[i].status == Base::SlotStatus::Occupied)
			{
				static_cast<void>(this->Add(*Base::KeyFrom(other.m_Slots[i]), *Base::ValueFrom(other.m_Slots[i])));
			}
		}
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename HashT, typename EqualT>
	void FixedHashMap<KeyT, ValueT, N, HashT, EqualT>::MoveFrom(FixedHashMap &other)
	{
		for (SizeType i = 0; i < other.GetSlotCount(); ++i)
		{
			if (other.m_Slots[i].status == Base::SlotStatus::Occupied)
			{
				static_cast<void>(this->Add(std::move(*Base::KeyFrom(other.m_Slots[i])), std::move(*Base::ValueFrom(other.m_Slots[i]))));
			}
		}

		other.Clear();
	}
} // namespace mlsl
