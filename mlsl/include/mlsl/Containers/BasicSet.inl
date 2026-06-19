// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace mlsl
{
	template <typename KeyT, typename Derived, typename LessT>
	BasicSet<KeyT, Derived, LessT>::BasicSet(LessT less)
		: m_Less(std::move(less))
	{
	}

	template <typename KeyT, typename Derived, typename LessT>
	typename BasicSet<KeyT, Derived, LessT>::SizeType BasicSet<KeyT, Derived, LessT>::Size() const
	{
		return m_Size;
	}

	template <typename KeyT, typename Derived, typename LessT>
	typename BasicSet<KeyT, Derived, LessT>::SizeType BasicSet<KeyT, Derived, LessT>::Capacity() const
	{
		return AsDerived().GetCapacity();
	}

	template <typename KeyT, typename Derived, typename LessT>
	bool BasicSet<KeyT, Derived, LessT>::Empty() const
	{
		return m_Size == 0;
	}

	template <typename KeyT, typename Derived, typename LessT>
	void BasicSet<KeyT, Derived, LessT>::Clear()
	{
		DestroyKeys();
		m_Size = 0;
	}

	template <typename KeyT, typename Derived, typename LessT>
	std::expected<void, Error> BasicSet<KeyT, Derived, LessT>::Reserve(SizeType capacity)
	{
		return AsDerived().EnsureCapacity(capacity);
	}

	template <typename KeyT, typename Derived, typename LessT>
	bool BasicSet<KeyT, Derived, LessT>::Contains(const KeyT &key) const
	{
		SizeType index = FindIndex(key);

		return index < m_Size and KeysEqual(AsDerived().GetKeys()[index], key);
	}

	template <typename KeyT, typename Derived, typename LessT>
	std::expected<typename BasicSet<KeyT, Derived, LessT>::Pointer, Error> BasicSet<KeyT, Derived, LessT>::Get(const KeyT &key)
	{
		SizeType index = FindIndex(key);
		KeyT *keys = AsDerived().GetKeys();

		if (index < m_Size and KeysEqual(keys[index], key))
		{
			return &keys[index];
		}

		return std::unexpected(Error {ErrorType::NotFound});
	}

	template <typename KeyT, typename Derived, typename LessT>
	std::expected<typename BasicSet<KeyT, Derived, LessT>::ConstPointer, Error> BasicSet<KeyT, Derived, LessT>::Get(const KeyT &key) const
	{
		SizeType index = FindIndex(key);
		const KeyT *keys = AsDerived().GetKeys();

		if (index < m_Size and KeysEqual(keys[index], key))
		{
			return &keys[index];
		}

		return std::unexpected(Error {ErrorType::NotFound});
	}

	template <typename KeyT, typename Derived, typename LessT>
	std::expected<void, Error> BasicSet<KeyT, Derived, LessT>::Add(const KeyT &key)
	{
		return AddInternal(key);
	}

	template <typename KeyT, typename Derived, typename LessT>
	std::expected<void, Error> BasicSet<KeyT, Derived, LessT>::Add(KeyT &&key)
	{
		return AddInternal(std::move(key));
	}

	template <typename KeyT, typename Derived, typename LessT>
	bool BasicSet<KeyT, Derived, LessT>::Remove(const KeyT &key)
	{
		SizeType index = FindIndex(key);
		KeyT *keys = AsDerived().GetKeys();

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

	template <typename KeyT, typename Derived, typename LessT>
	void BasicSet<KeyT, Derived, LessT>::DestroyKeys()
	{
		KeyT *keys = AsDerived().GetKeys();

		for (SizeType i = 0; i < m_Size; ++i)
		{
			keys[i].~KeyT();
		}
	}

	template <typename KeyT, typename Derived, typename LessT>
	Derived &BasicSet<KeyT, Derived, LessT>::AsDerived()
	{
		return static_cast<Derived &>(*this);
	}

	template <typename KeyT, typename Derived, typename LessT>
	const Derived &BasicSet<KeyT, Derived, LessT>::AsDerived() const
	{
		return static_cast<const Derived &>(*this);
	}

	template <typename KeyT, typename Derived, typename LessT>
	bool BasicSet<KeyT, Derived, LessT>::KeysEqual(const KeyT &left, const KeyT &right) const
	{
		return not m_Less(left, right) and not m_Less(right, left);
	}

	template <typename KeyT, typename Derived, typename LessT>
	typename BasicSet<KeyT, Derived, LessT>::SizeType BasicSet<KeyT, Derived, LessT>::FindIndex(const KeyT &key) const
	{
		const KeyT *keys = AsDerived().GetKeys();
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

	template <typename KeyT, typename Derived, typename LessT>
	template <typename KeyArg>
	std::expected<void, Error> BasicSet<KeyT, Derived, LessT>::AddInternal(KeyArg &&key)
	{
		SizeType index = FindIndex(key);

		if (index < m_Size and KeysEqual(AsDerived().GetKeys()[index], key))
		{
			return std::unexpected(Error {ErrorType::AlreadyExists});
		}

		auto result = AsDerived().EnsureCapacity(m_Size + 1);

		if (not result)
		{
			return std::unexpected(result.error());
		}

		KeyT *keys = AsDerived().GetKeys();

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
} // namespace mlsl
