// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace mlsl
{
	template <typename KeyT, std::size_t N, typename LessT>
	FixedSet<KeyT, N, LessT>::FixedSet(LessT less)
		: Base(std::move(less))
	{
	}

	template <typename KeyT, std::size_t N, typename LessT>
	FixedSet<KeyT, N, LessT>::FixedSet(const FixedSet &other)
		: Base(other.m_Less)
	{
		CopyFrom(other);
	}

	template <typename KeyT, std::size_t N, typename LessT>
	FixedSet<KeyT, N, LessT>::FixedSet(FixedSet &&other) noexcept
		: Base(std::move(other.m_Less))
	{
		MoveFrom(other);
	}

	template <typename KeyT, std::size_t N, typename LessT>
	FixedSet<KeyT, N, LessT>::~FixedSet()
	{
		this->Clear();
	}

	template <typename KeyT, std::size_t N, typename LessT>
	FixedSet<KeyT, N, LessT> &FixedSet<KeyT, N, LessT>::operator=(const FixedSet &other)
	{
		if (this != &other)
		{
			this->Clear();
			this->m_Less = other.m_Less;
			CopyFrom(other);
		}

		return *this;
	}

	template <typename KeyT, std::size_t N, typename LessT>
	FixedSet<KeyT, N, LessT> &FixedSet<KeyT, N, LessT>::operator=(FixedSet &&other) noexcept
	{
		if (this != &other)
		{
			this->Clear();
			this->m_Less = std::move(other.m_Less);
			MoveFrom(other);
		}

		return *this;
	}

	template <typename KeyT, std::size_t N, typename LessT>
	bool FixedSet<KeyT, N, LessT>::Full() const
	{
		return this->m_Size >= N;
	}

	template <typename KeyT, std::size_t N, typename LessT>
	KeyT *FixedSet<KeyT, N, LessT>::GetKeys()
	{
		return reinterpret_cast<KeyT *>(m_Storage);
	}

	template <typename KeyT, std::size_t N, typename LessT>
	const KeyT *FixedSet<KeyT, N, LessT>::GetKeys() const
	{
		return reinterpret_cast<const KeyT *>(m_Storage);
	}

	template <typename KeyT, std::size_t N, typename LessT>
	typename FixedSet<KeyT, N, LessT>::SizeType FixedSet<KeyT, N, LessT>::GetCapacity() const
	{
		return N;
	}

	template <typename KeyT, std::size_t N, typename LessT>
	std::expected<void, Error> FixedSet<KeyT, N, LessT>::EnsureCapacity(SizeType capacity)
	{
		if (capacity > N)
		{
			return std::unexpected(Error {ErrorType::OutOfMemory});
		}

		return {};
	}

	template <typename KeyT, std::size_t N, typename LessT>
	void FixedSet<KeyT, N, LessT>::CopyFrom(const FixedSet &other)
	{
		const KeyT *source = other.GetKeys();
		KeyT *keys = GetKeys();

		for (SizeType i = 0; i < other.m_Size; ++i)
		{
			new (&keys[i]) KeyT {source[i]};
		}

		this->m_Size = other.m_Size;
	}

	template <typename KeyT, std::size_t N, typename LessT>
	void FixedSet<KeyT, N, LessT>::MoveFrom(FixedSet &other)
	{
		KeyT *source = other.GetKeys();
		KeyT *keys = GetKeys();

		for (SizeType i = 0; i < other.m_Size; ++i)
		{
			new (&keys[i]) KeyT {std::move(source[i])};
		}

		this->m_Size = other.m_Size;
		other.Clear();
	}
} // namespace mlsl
