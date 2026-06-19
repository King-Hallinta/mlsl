// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace mlsl
{
	template <typename KeyT, typename ValueT, typename Derived, typename LessT>
	BasicMap<KeyT, ValueT, Derived, LessT>::BasicMap(LessT less)
		: m_Less(std::move(less))
	{
	}

	template <typename KeyT, typename ValueT, typename Derived, typename LessT>
	typename BasicMap<KeyT, ValueT, Derived, LessT>::SizeType BasicMap<KeyT, ValueT, Derived, LessT>::Size() const
	{
		return m_Size;
	}

	template <typename KeyT, typename ValueT, typename Derived, typename LessT>
	typename BasicMap<KeyT, ValueT, Derived, LessT>::SizeType BasicMap<KeyT, ValueT, Derived, LessT>::Capacity() const
	{
		return AsDerived().GetCapacity();
	}

	template <typename KeyT, typename ValueT, typename Derived, typename LessT>
	bool BasicMap<KeyT, ValueT, Derived, LessT>::Empty() const
	{
		return m_Size == 0;
	}

	template <typename KeyT, typename ValueT, typename Derived, typename LessT>
	void BasicMap<KeyT, ValueT, Derived, LessT>::Clear()
	{
		DestroyEntries();
		m_Size = 0;
	}

	template <typename KeyT, typename ValueT, typename Derived, typename LessT>
	std::expected<void, Error> BasicMap<KeyT, ValueT, Derived, LessT>::Reserve(SizeType capacity)
	{
		return AsDerived().EnsureCapacity(capacity);
	}

	template <typename KeyT, typename ValueT, typename Derived, typename LessT>
	bool BasicMap<KeyT, ValueT, Derived, LessT>::Contains(const KeyT &key) const
	{
		SizeType index = FindIndex(key);

		return index < m_Size and KeysEqual(AsDerived().GetEntries()[index].key, key);
	}

	template <typename KeyT, typename ValueT, typename Derived, typename LessT>
	std::expected<typename BasicMap<KeyT, ValueT, Derived, LessT>::Pointer, Error> BasicMap<KeyT, ValueT, Derived, LessT>::Get(const KeyT &key)
	{
		SizeType index = FindIndex(key);
		Entry *entries = AsDerived().GetEntries();

		if (index < m_Size and KeysEqual(entries[index].key, key))
		{
			return &entries[index].value;
		}

		return std::unexpected(Error {ErrorType::NotFound});
	}

	template <typename KeyT, typename ValueT, typename Derived, typename LessT>
	std::expected<typename BasicMap<KeyT, ValueT, Derived, LessT>::ConstPointer, Error> BasicMap<KeyT, ValueT, Derived, LessT>::Get(const KeyT &key) const
	{
		SizeType index = FindIndex(key);
		const Entry *entries = AsDerived().GetEntries();

		if (index < m_Size and KeysEqual(entries[index].key, key))
		{
			return &entries[index].value;
		}

		return std::unexpected(Error {ErrorType::NotFound});
	}

	template <typename KeyT, typename ValueT, typename Derived, typename LessT>
	std::expected<void, Error> BasicMap<KeyT, ValueT, Derived, LessT>::Add(const KeyT &key, const ValueT &value)
	{
		return AddInternal(key, value);
	}

	template <typename KeyT, typename ValueT, typename Derived, typename LessT>
	std::expected<void, Error> BasicMap<KeyT, ValueT, Derived, LessT>::Add(const KeyT &key, ValueT &&value)
	{
		return AddInternal(key, std::move(value));
	}

	template <typename KeyT, typename ValueT, typename Derived, typename LessT>
	std::expected<void, Error> BasicMap<KeyT, ValueT, Derived, LessT>::Add(KeyT &&key, const ValueT &value)
	{
		return AddInternal(std::move(key), value);
	}

	template <typename KeyT, typename ValueT, typename Derived, typename LessT>
	std::expected<void, Error> BasicMap<KeyT, ValueT, Derived, LessT>::Add(KeyT &&key, ValueT &&value)
	{
		return AddInternal(std::move(key), std::move(value));
	}

	template <typename KeyT, typename ValueT, typename Derived, typename LessT>
	bool BasicMap<KeyT, ValueT, Derived, LessT>::Remove(const KeyT &key)
	{
		SizeType index = FindIndex(key);
		Entry *entries = AsDerived().GetEntries();

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

	template <typename KeyT, typename ValueT, typename Derived, typename LessT>
	void BasicMap<KeyT, ValueT, Derived, LessT>::DestroyEntries()
	{
		Entry *entries = AsDerived().GetEntries();

		for (SizeType i = 0; i < m_Size; ++i)
		{
			entries[i].~Entry();
		}
	}

	template <typename KeyT, typename ValueT, typename Derived, typename LessT>
	Derived &BasicMap<KeyT, ValueT, Derived, LessT>::AsDerived()
	{
		return static_cast<Derived &>(*this);
	}

	template <typename KeyT, typename ValueT, typename Derived, typename LessT>
	const Derived &BasicMap<KeyT, ValueT, Derived, LessT>::AsDerived() const
	{
		return static_cast<const Derived &>(*this);
	}

	template <typename KeyT, typename ValueT, typename Derived, typename LessT>
	bool BasicMap<KeyT, ValueT, Derived, LessT>::KeysEqual(const KeyT &left, const KeyT &right) const
	{
		return not m_Less(left, right) and not m_Less(right, left);
	}

	template <typename KeyT, typename ValueT, typename Derived, typename LessT>
	typename BasicMap<KeyT, ValueT, Derived, LessT>::SizeType BasicMap<KeyT, ValueT, Derived, LessT>::FindIndex(const KeyT &key) const
	{
		const Entry *entries = AsDerived().GetEntries();
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

	template <typename KeyT, typename ValueT, typename Derived, typename LessT>
	template <typename KeyArg, typename ValueArg>
	std::expected<void, Error> BasicMap<KeyT, ValueT, Derived, LessT>::AddInternal(KeyArg &&key, ValueArg &&value)
	{
		SizeType index = FindIndex(key);

		if (index < m_Size and KeysEqual(AsDerived().GetEntries()[index].key, key))
		{
			return std::unexpected(Error {ErrorType::AlreadyExists});
		}

		auto result = AsDerived().EnsureCapacity(m_Size + 1);

		if (not result)
		{
			return std::unexpected(result.error());
		}

		Entry *entries = AsDerived().GetEntries();

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
} // namespace mlsl
