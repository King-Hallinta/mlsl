// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <new>

namespace mlsl
{
	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	FixedMap<KeyT, ValueT, N, LessT>::FixedMap(LessT less)
		: m_Less(std::move(less))
	{
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	FixedMap<KeyT, ValueT, N, LessT>::FixedMap(const FixedMap &other)
		: m_Less(other.m_Less)
	{
		CopyFrom(other);
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	FixedMap<KeyT, ValueT, N, LessT>::FixedMap(FixedMap &&other) noexcept
		: m_Less(std::move(other.m_Less))
	{
		MoveFrom(other);
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	FixedMap<KeyT, ValueT, N, LessT>::~FixedMap()
	{
		Clear();
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	FixedMap<KeyT, ValueT, N, LessT> &FixedMap<KeyT, ValueT, N, LessT>::operator=(const FixedMap &other)
	{
		if (this != &other)
		{
			Clear();
			m_Less = other.m_Less;
			CopyFrom(other);
		}

		return *this;
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	FixedMap<KeyT, ValueT, N, LessT> &FixedMap<KeyT, ValueT, N, LessT>::operator=(FixedMap &&other) noexcept
	{
		if (this != &other)
		{
			Clear();
			m_Less = std::move(other.m_Less);
			MoveFrom(other);
		}

		return *this;
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	typename FixedMap<KeyT, ValueT, N, LessT>::SizeType FixedMap<KeyT, ValueT, N, LessT>::Size() const
	{
		return m_Size;
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	typename FixedMap<KeyT, ValueT, N, LessT>::SizeType FixedMap<KeyT, ValueT, N, LessT>::Capacity() const
	{
		return N;
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	bool FixedMap<KeyT, ValueT, N, LessT>::Empty() const
	{
		return m_Size == 0;
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	bool FixedMap<KeyT, ValueT, N, LessT>::Full() const
	{
		return m_Size >= N;
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	void FixedMap<KeyT, ValueT, N, LessT>::Clear()
	{
		DestroyEntries();
		m_Size = 0;
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	bool FixedMap<KeyT, ValueT, N, LessT>::Contains(const KeyT &key) const
	{
		SizeType index = FindIndex(key);

		return index < m_Size and KeysEqual(Entries()[index].key, key);
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	std::expected<typename FixedMap<KeyT, ValueT, N, LessT>::Pointer, Error> FixedMap<KeyT, ValueT, N, LessT>::Get(const KeyT &key)
	{
		SizeType index = FindIndex(key);

		if (index < m_Size and KeysEqual(Entries()[index].key, key))
		{
			return &Entries()[index].value;
		}

		return std::unexpected(Error {ErrorType::NotFound});
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	std::expected<typename FixedMap<KeyT, ValueT, N, LessT>::ConstPointer, Error> FixedMap<KeyT, ValueT, N, LessT>::Get(const KeyT &key) const
	{
		SizeType index = FindIndex(key);

		if (index < m_Size and KeysEqual(Entries()[index].key, key))
		{
			return &Entries()[index].value;
		}

		return std::unexpected(Error {ErrorType::NotFound});
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	std::expected<void, Error> FixedMap<KeyT, ValueT, N, LessT>::Add(const KeyT &key, const ValueT &value)
	{
		return AddInternal(key, value);
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	std::expected<void, Error> FixedMap<KeyT, ValueT, N, LessT>::Add(const KeyT &key, ValueT &&value)
	{
		return AddInternal(key, std::move(value));
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	std::expected<void, Error> FixedMap<KeyT, ValueT, N, LessT>::Add(KeyT &&key, const ValueT &value)
	{
		return AddInternal(std::move(key), value);
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	std::expected<void, Error> FixedMap<KeyT, ValueT, N, LessT>::Add(KeyT &&key, ValueT &&value)
	{
		return AddInternal(std::move(key), std::move(value));
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	bool FixedMap<KeyT, ValueT, N, LessT>::Remove(const KeyT &key)
	{
		SizeType index = FindIndex(key);
		Entry *entries = Entries();

		if (index >= m_Size or not KeysEqual(entries[index].key, key))
		{
			return false;
		}

		entries[index].~Entry();

		for (SizeType i = index; i + 1 < m_Size; ++i)
		{
			new (&entries[i]) Entry {std::move(entries[i + 1].key), std::move(entries[i + 1].value)};
			entries[i + 1].~Entry();
		}

		--m_Size;

		return true;
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	typename FixedMap<KeyT, ValueT, N, LessT>::Entry *FixedMap<KeyT, ValueT, N, LessT>::Entries()
	{
		return reinterpret_cast<Entry *>(m_Storage);
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	const typename FixedMap<KeyT, ValueT, N, LessT>::Entry *FixedMap<KeyT, ValueT, N, LessT>::Entries() const
	{
		return reinterpret_cast<const Entry *>(m_Storage);
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	bool FixedMap<KeyT, ValueT, N, LessT>::KeysEqual(const KeyT &left, const KeyT &right) const
	{
		return not m_Less(left, right) and not m_Less(right, left);
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	typename FixedMap<KeyT, ValueT, N, LessT>::SizeType FixedMap<KeyT, ValueT, N, LessT>::FindIndex(const KeyT &key) const
	{
		const Entry *entries = Entries();
		SizeType first = 0;
		SizeType count = m_Size;

		while (count > 0)
		{
			SizeType step = count / 2;
			SizeType middle = first + step;

			if (m_Less(entries[middle].key, key))
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

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	template <typename KeyArg, typename ValueArg>
	std::expected<void, Error> FixedMap<KeyT, ValueT, N, LessT>::AddInternal(KeyArg &&key, ValueArg &&value)
	{
		SizeType index = FindIndex(key);
		Entry *entries = Entries();

		if (index < m_Size and KeysEqual(entries[index].key, key))
		{
			return std::unexpected(Error {ErrorType::AlreadyExists});
		}

		if (m_Size >= N)
		{
			return std::unexpected(Error {ErrorType::OutOfMemory});
		}

		if (index < m_Size)
		{
			new (&entries[m_Size]) Entry {std::move(entries[m_Size - 1].key), std::move(entries[m_Size - 1].value)};

			for (SizeType i = m_Size - 1; i > index; --i)
			{
				entries[i].~Entry();
				new (&entries[i]) Entry {std::move(entries[i - 1].key), std::move(entries[i - 1].value)};
			}

			entries[index].~Entry();
		}

		new (&entries[index]) Entry {std::forward<KeyArg>(key), std::forward<ValueArg>(value)};
		++m_Size;

		return {};
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	void FixedMap<KeyT, ValueT, N, LessT>::DestroyEntries()
	{
		Entry *entries = Entries();

		for (SizeType i = 0; i < m_Size; ++i)
		{
			entries[i].~Entry();
		}
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	void FixedMap<KeyT, ValueT, N, LessT>::CopyFrom(const FixedMap &other)
	{
		const Entry *source = other.Entries();
		Entry *entries = Entries();

		for (SizeType i = 0; i < other.m_Size; ++i)
		{
			new (&entries[i]) Entry {source[i].key, source[i].value};
		}

		m_Size = other.m_Size;
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	void FixedMap<KeyT, ValueT, N, LessT>::MoveFrom(FixedMap &other)
	{
		Entry *source = other.Entries();
		Entry *entries = Entries();

		for (SizeType i = 0; i < other.m_Size; ++i)
		{
			new (&entries[i]) Entry {std::move(source[i].key), std::move(source[i].value)};
		}

		m_Size = other.m_Size;
		other.Clear();
	}
} // namespace mlsl
