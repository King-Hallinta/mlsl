// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace mlsl
{
	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	FixedMap<KeyT, ValueT, N, LessT>::FixedMap(LessT less)
		: Base(std::move(less))
	{
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	FixedMap<KeyT, ValueT, N, LessT>::FixedMap(const FixedMap &other)
		: Base(other.m_Less)
	{
		CopyFrom(other);
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	FixedMap<KeyT, ValueT, N, LessT>::FixedMap(FixedMap &&other) noexcept
		: Base(std::move(other.m_Less))
	{
		MoveFrom(other);
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	FixedMap<KeyT, ValueT, N, LessT>::~FixedMap()
	{
		this->Clear();
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	FixedMap<KeyT, ValueT, N, LessT> &FixedMap<KeyT, ValueT, N, LessT>::operator=(const FixedMap &other)
	{
		if (this != &other)
		{
			this->Clear();
			this->m_Less = other.m_Less;
			CopyFrom(other);
		}

		return *this;
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	FixedMap<KeyT, ValueT, N, LessT> &FixedMap<KeyT, ValueT, N, LessT>::operator=(FixedMap &&other) noexcept
	{
		if (this != &other)
		{
			this->Clear();
			this->m_Less = std::move(other.m_Less);
			MoveFrom(other);
		}

		return *this;
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	bool FixedMap<KeyT, ValueT, N, LessT>::Full() const
	{
		return this->m_Size >= N;
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	typename FixedMap<KeyT, ValueT, N, LessT>::Entry *FixedMap<KeyT, ValueT, N, LessT>::GetEntries()
	{
		return reinterpret_cast<Entry *>(m_Storage);
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	const typename FixedMap<KeyT, ValueT, N, LessT>::Entry *FixedMap<KeyT, ValueT, N, LessT>::GetEntries() const
	{
		return reinterpret_cast<const Entry *>(m_Storage);
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	typename FixedMap<KeyT, ValueT, N, LessT>::SizeType FixedMap<KeyT, ValueT, N, LessT>::GetCapacity() const
	{
		return N;
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	std::expected<void, Error> FixedMap<KeyT, ValueT, N, LessT>::EnsureCapacity(SizeType capacity)
	{
		if (capacity > N)
		{
			return std::unexpected(Error {ErrorType::OutOfMemory});
		}

		return {};
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	void FixedMap<KeyT, ValueT, N, LessT>::CopyFrom(const FixedMap &other)
	{
		const Entry *source = other.GetEntries();
		Entry *entries = GetEntries();

		for (SizeType i = 0; i < other.m_Size; ++i)
		{
			new (&entries[i]) Entry {source[i].key, source[i].value};
		}

		this->m_Size = other.m_Size;
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	void FixedMap<KeyT, ValueT, N, LessT>::MoveFrom(FixedMap &other)
	{
		Entry *source = other.GetEntries();
		Entry *entries = GetEntries();

		for (SizeType i = 0; i < other.m_Size; ++i)
		{
			new (&entries[i]) Entry {std::move(source[i].key), std::move(source[i].value)};
		}

		this->m_Size = other.m_Size;
		other.Clear();
	}
} // namespace mlsl
