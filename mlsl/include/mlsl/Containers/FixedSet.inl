// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <new>

namespace mlsl
{
	template <typename KeyT, std::size_t N, typename LessT>
	FixedSet<KeyT, N, LessT>::FixedSet(LessT less)
		: m_Less(std::move(less))
	{
	}

	template <typename KeyT, std::size_t N, typename LessT>
	FixedSet<KeyT, N, LessT>::FixedSet(const FixedSet &other)
		: m_Less(other.m_Less)
	{
		CopyFrom(other);
	}

	template <typename KeyT, std::size_t N, typename LessT>
	FixedSet<KeyT, N, LessT>::FixedSet(FixedSet &&other) noexcept
		: m_Less(std::move(other.m_Less))
	{
		MoveFrom(other);
	}

	template <typename KeyT, std::size_t N, typename LessT>
	FixedSet<KeyT, N, LessT>::~FixedSet()
	{
		Clear();
	}

	template <typename KeyT, std::size_t N, typename LessT>
	FixedSet<KeyT, N, LessT> &FixedSet<KeyT, N, LessT>::operator=(const FixedSet &other)
	{
		if (this != &other)
		{
			Clear();
			m_Less = other.m_Less;
			CopyFrom(other);
		}

		return *this;
	}

	template <typename KeyT, std::size_t N, typename LessT>
	FixedSet<KeyT, N, LessT> &FixedSet<KeyT, N, LessT>::operator=(FixedSet &&other) noexcept
	{
		if (this != &other)
		{
			Clear();
			m_Less = std::move(other.m_Less);
			MoveFrom(other);
		}

		return *this;
	}

	template <typename KeyT, std::size_t N, typename LessT>
	typename FixedSet<KeyT, N, LessT>::SizeType FixedSet<KeyT, N, LessT>::Size() const
	{
		return m_Size;
	}

	template <typename KeyT, std::size_t N, typename LessT>
	typename FixedSet<KeyT, N, LessT>::SizeType FixedSet<KeyT, N, LessT>::Capacity() const
	{
		return N;
	}

	template <typename KeyT, std::size_t N, typename LessT>
	bool FixedSet<KeyT, N, LessT>::Empty() const
	{
		return m_Size == 0;
	}

	template <typename KeyT, std::size_t N, typename LessT>
	bool FixedSet<KeyT, N, LessT>::Full() const
	{
		return m_Size >= N;
	}

	template <typename KeyT, std::size_t N, typename LessT>
	void FixedSet<KeyT, N, LessT>::Clear()
	{
		DestroyKeys();
		m_Size = 0;
	}

	template <typename KeyT, std::size_t N, typename LessT>
	bool FixedSet<KeyT, N, LessT>::Contains(const KeyT &key) const
	{
		SizeType index = FindIndex(key);

		return index < m_Size and KeysEqual(Keys()[index], key);
	}

	template <typename KeyT, std::size_t N, typename LessT>
	std::expected<typename FixedSet<KeyT, N, LessT>::Pointer, Error> FixedSet<KeyT, N, LessT>::Get(const KeyT &key)
	{
		SizeType index = FindIndex(key);

		if (index < m_Size and KeysEqual(Keys()[index], key))
		{
			return &Keys()[index];
		}

		return std::unexpected(Error {ErrorType::NotFound});
	}

	template <typename KeyT, std::size_t N, typename LessT>
	std::expected<typename FixedSet<KeyT, N, LessT>::ConstPointer, Error> FixedSet<KeyT, N, LessT>::Get(const KeyT &key) const
	{
		SizeType index = FindIndex(key);

		if (index < m_Size and KeysEqual(Keys()[index], key))
		{
			return &Keys()[index];
		}

		return std::unexpected(Error {ErrorType::NotFound});
	}

	template <typename KeyT, std::size_t N, typename LessT>
	std::expected<void, Error> FixedSet<KeyT, N, LessT>::Add(const KeyT &key)
	{
		return AddInternal(key);
	}

	template <typename KeyT, std::size_t N, typename LessT>
	std::expected<void, Error> FixedSet<KeyT, N, LessT>::Add(KeyT &&key)
	{
		return AddInternal(std::move(key));
	}

	template <typename KeyT, std::size_t N, typename LessT>
	bool FixedSet<KeyT, N, LessT>::Remove(const KeyT &key)
	{
		SizeType index = FindIndex(key);
		KeyT *keys = Keys();

		if (index >= m_Size or not KeysEqual(keys[index], key))
		{
			return false;
		}

		keys[index].~KeyT();

		for (SizeType i = index; i + 1 < m_Size; ++i)
		{
			new (&keys[i]) KeyT {std::move(keys[i + 1])};
			keys[i + 1].~KeyT();
		}

		--m_Size;

		return true;
	}

	template <typename KeyT, std::size_t N, typename LessT>
	KeyT *FixedSet<KeyT, N, LessT>::Keys()
	{
		return reinterpret_cast<KeyT *>(m_Storage);
	}

	template <typename KeyT, std::size_t N, typename LessT>
	const KeyT *FixedSet<KeyT, N, LessT>::Keys() const
	{
		return reinterpret_cast<const KeyT *>(m_Storage);
	}

	template <typename KeyT, std::size_t N, typename LessT>
	bool FixedSet<KeyT, N, LessT>::KeysEqual(const KeyT &left, const KeyT &right) const
	{
		return not m_Less(left, right) and not m_Less(right, left);
	}

	template <typename KeyT, std::size_t N, typename LessT>
	typename FixedSet<KeyT, N, LessT>::SizeType FixedSet<KeyT, N, LessT>::FindIndex(const KeyT &key) const
	{
		const KeyT *keys = Keys();
		SizeType first = 0;
		SizeType count = m_Size;

		while (count > 0)
		{
			SizeType step = count / 2;
			SizeType middle = first + step;

			if (m_Less(keys[middle], key))
			{
				first = middle + 1;
				count -= step + 1;
			}
			else
			{
				count = step;
			}
		}

		return first;
	}

	template <typename KeyT, std::size_t N, typename LessT>
	template <typename KeyArg>
	std::expected<void, Error> FixedSet<KeyT, N, LessT>::AddInternal(KeyArg &&key)
	{
		SizeType index = FindIndex(key);
		KeyT *keys = Keys();

		if (index < m_Size and KeysEqual(keys[index], key))
		{
			return std::unexpected(Error {ErrorType::AlreadyExists});
		}

		if (m_Size >= N)
		{
			return std::unexpected(Error {ErrorType::OutOfMemory});
		}

		if (index < m_Size)
		{
			new (&keys[m_Size]) KeyT {std::move(keys[m_Size - 1])};

			for (SizeType i = m_Size - 1; i > index; --i)
			{
				keys[i].~KeyT();
				new (&keys[i]) KeyT {std::move(keys[i - 1])};
			}

			keys[index].~KeyT();
		}

		new (&keys[index]) KeyT {std::forward<KeyArg>(key)};
		++m_Size;

		return {};
	}

	template <typename KeyT, std::size_t N, typename LessT>
	void FixedSet<KeyT, N, LessT>::DestroyKeys()
	{
		KeyT *keys = Keys();

		for (SizeType i = 0; i < m_Size; ++i)
		{
			keys[i].~KeyT();
		}
	}

	template <typename KeyT, std::size_t N, typename LessT>
	void FixedSet<KeyT, N, LessT>::CopyFrom(const FixedSet &other)
	{
		const KeyT *source = other.Keys();
		KeyT *keys = Keys();

		for (SizeType i = 0; i < other.m_Size; ++i)
		{
			new (&keys[i]) KeyT {source[i]};
		}

		m_Size = other.m_Size;
	}

	template <typename KeyT, std::size_t N, typename LessT>
	void FixedSet<KeyT, N, LessT>::MoveFrom(FixedSet &other)
	{
		KeyT *source = other.Keys();
		KeyT *keys = Keys();

		for (SizeType i = 0; i < other.m_Size; ++i)
		{
			new (&keys[i]) KeyT {std::move(source[i])};
		}

		m_Size = other.m_Size;
		other.Clear();
	}
} // namespace mlsl
