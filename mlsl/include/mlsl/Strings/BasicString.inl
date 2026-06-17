// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace mlsl
{
	template <typename CharT>
	BasicString<CharT>::BasicString()
		: m_Allocator(new DefaultAllocator()), m_OwnsAllocator(true)
	{
		WriteTerminator();
	}

	template <typename CharT>
	BasicString<CharT>::BasicString(Allocator *allocator)
		: m_Allocator(allocator == nullptr ? new DefaultAllocator() : allocator), m_OwnsAllocator(allocator == nullptr)
	{
		WriteTerminator();
	}

	template <typename CharT>
	BasicString<CharT>::BasicString(std::nullptr_t)
		: BasicString()
	{
	}

	template <typename CharT>
	BasicString<CharT>::BasicString(ConstPointer data)
		: BasicString()
	{
		static_cast<void>(Assign(data));
	}

	template <typename CharT>
	BasicString<CharT>::BasicString(ViewType view)
		: BasicString()
	{
		static_cast<void>(Assign(view));
	}

	template <typename CharT>
	BasicString<CharT>::BasicString(const BasicString &other)
		: BasicString()
	{
		static_cast<void>(Assign(other.View()));
	}

	template <typename CharT>
	BasicString<CharT>::BasicString(BasicString &&other) noexcept
		: m_Size(other.m_Size), m_Capacity(other.m_Capacity), m_Allocator(other.m_Allocator), m_OwnsAllocator(other.m_OwnsAllocator)
	{
		if (other.UsesEmptyStorage())
		{
			m_Data = m_Empty;
			m_Capacity = 0;
			WriteTerminator();
		}
		else
		{
			m_Data = other.m_Data;
		}

		other.m_Data = other.m_Empty;
		other.m_Size = 0;
		other.m_Capacity = 0;
		other.m_Allocator = nullptr;
		other.m_OwnsAllocator = false;
		other.WriteTerminator();
	}

	template <typename CharT>
	BasicString<CharT>::~BasicString()
	{
		ReleaseStorage();

		if (m_OwnsAllocator)
		{
			delete m_Allocator;
		}
	}

	template <typename CharT>
	BasicString<CharT> &BasicString<CharT>::operator=(const BasicString &other)
	{
		if (this != &other)
		{
			static_cast<void>(Assign(other.View()));
		}

		return *this;
	}

	template <typename CharT>
	BasicString<CharT> &BasicString<CharT>::operator=(BasicString &&other) noexcept
	{
		if (this != &other)
		{
			ReleaseStorage();

			if (m_OwnsAllocator)
			{
				delete m_Allocator;
			}

			m_Size = other.m_Size;
			m_Capacity = other.m_Capacity;
			m_Allocator = other.m_Allocator;
			m_OwnsAllocator = other.m_OwnsAllocator;

			if (other.UsesEmptyStorage())
			{
				m_Data = m_Empty;
				m_Capacity = 0;
				WriteTerminator();
			}
			else
			{
				m_Data = other.m_Data;
			}

			other.m_Data = other.m_Empty;
			other.m_Size = 0;
			other.m_Capacity = 0;
			other.m_Allocator = nullptr;
			other.m_OwnsAllocator = false;
			other.WriteTerminator();
		}

		return *this;
	}

	template <typename CharT>
	typename BasicString<CharT>::Reference BasicString<CharT>::operator[](SizeType index)
	{
		return m_Data[index];
	}

	template <typename CharT>
	typename BasicString<CharT>::ConstReference BasicString<CharT>::operator[](SizeType index) const
	{
		return m_Data[index];
	}

	template <typename CharT>
	std::expected<typename BasicString<CharT>::Pointer, Error> BasicString<CharT>::Get(SizeType index)
	{
		if (index >= m_Size)
		{
			return std::unexpected(Error {ErrorType::OutOfBounds});
		}

		return m_Data + index;
	}

	template <typename CharT>
	std::expected<typename BasicString<CharT>::ConstPointer, Error> BasicString<CharT>::Get(SizeType index) const
	{
		if (index >= m_Size)
		{
			return std::unexpected(Error {ErrorType::OutOfBounds});
		}

		return m_Data + index;
	}

	template <typename CharT>
	typename BasicString<CharT>::Reference BasicString<CharT>::Front()
	{
		return m_Data[0];
	}

	template <typename CharT>
	typename BasicString<CharT>::ConstReference BasicString<CharT>::Front() const
	{
		return m_Data[0];
	}

	template <typename CharT>
	typename BasicString<CharT>::Reference BasicString<CharT>::Back()
	{
		return m_Data[m_Size - 1];
	}

	template <typename CharT>
	typename BasicString<CharT>::ConstReference BasicString<CharT>::Back() const
	{
		return m_Data[m_Size - 1];
	}

	template <typename CharT>
	typename BasicString<CharT>::Pointer BasicString<CharT>::Data()
	{
		return m_Data;
	}

	template <typename CharT>
	typename BasicString<CharT>::ConstPointer BasicString<CharT>::Data() const
	{
		return m_Data;
	}

	template <typename CharT>
	typename BasicString<CharT>::ConstPointer BasicString<CharT>::CString() const
	{
		return m_Data;
	}

	template <typename CharT>
	typename BasicString<CharT>::Iterator BasicString<CharT>::Begin()
	{
		return m_Data;
	}

	template <typename CharT>
	typename BasicString<CharT>::ConstIterator BasicString<CharT>::Begin() const
	{
		return m_Data;
	}

	template <typename CharT>
	typename BasicString<CharT>::Iterator BasicString<CharT>::End()
	{
		return m_Data + m_Size;
	}

	template <typename CharT>
	typename BasicString<CharT>::ConstIterator BasicString<CharT>::End() const
	{
		return m_Data + m_Size;
	}

	template <typename CharT>
	typename BasicString<CharT>::SizeType BasicString<CharT>::Size() const
	{
		return m_Size;
	}

	template <typename CharT>
	typename BasicString<CharT>::SizeType BasicString<CharT>::Capacity() const
	{
		return m_Capacity;
	}

	template <typename CharT>
	bool BasicString<CharT>::Empty() const
	{
		return m_Size == 0;
	}

	template <typename CharT>
	std::expected<void, Error> BasicString<CharT>::Reserve(SizeType capacity)
	{
		if (capacity <= m_Capacity)
		{
			return {};
		}

		return Grow(capacity);
	}

	template <typename CharT>
	void BasicString<CharT>::Clear()
	{
		m_Size = 0;
		WriteTerminator();
	}

	template <typename CharT>
	std::expected<void, Error> BasicString<CharT>::Assign(ConstPointer data)
	{
		return Assign(ViewType(data));
	}

	template <typename CharT>
	std::expected<void, Error> BasicString<CharT>::Assign(ViewType view)
	{
		auto result = Reserve(view.Size());

		if (not result)
		{
			return result;
		}

		for (SizeType i = 0; i < view.Size(); ++i)
		{
			m_Data[i] = view[i];
		}

		m_Size = view.Size();
		WriteTerminator();

		return {};
	}

	template <typename CharT>
	std::expected<void, Error> BasicString<CharT>::Append(ConstPointer data)
	{
		return Append(ViewType(data));
	}

	template <typename CharT>
	std::expected<void, Error> BasicString<CharT>::Append(ViewType view)
	{
		ConstPointer appendData = view.Data();
		SizeType appendOffset = 0;
		bool aliasesStorage = false;

		if (appendData != nullptr)
		{
			auto dataStart = reinterpret_cast<std::uintptr_t>(m_Data);
			auto dataEnd = dataStart + m_Size * sizeof(CharT);
			auto appendStart = reinterpret_cast<std::uintptr_t>(appendData);
			aliasesStorage = appendStart >= dataStart and appendStart < dataEnd;
			appendOffset = aliasesStorage ? static_cast<SizeType>(appendData - m_Data) : 0;
		}

		auto result = Reserve(m_Size + view.Size());

		if (not result)
		{
			return result;
		}

		appendData = aliasesStorage ? m_Data + appendOffset : appendData;

		for (SizeType i = 0; i < view.Size(); ++i)
		{
			m_Data[m_Size + i] = appendData[i];
		}

		m_Size += view.Size();
		WriteTerminator();

		return {};
	}

	template <typename CharT>
	std::expected<void, Error> BasicString<CharT>::Append(CharT value)
	{
		auto result = Reserve(m_Size + 1);

		if (not result)
		{
			return result;
		}

		m_Data[m_Size] = value;
		++m_Size;
		WriteTerminator();

		return {};
	}

	template <typename CharT>
	void BasicString<CharT>::Remove()
	{
		if (m_Size == 0)
		{
			return;
		}

		--m_Size;
		WriteTerminator();
	}

	template <typename CharT>
	std::expected<void, Error> BasicString<CharT>::Resize(SizeType size, CharT value)
	{
		auto result = Reserve(size);

		if (not result)
		{
			return result;
		}

		for (SizeType i = m_Size; i < size; ++i)
		{
			m_Data[i] = value;
		}

		m_Size = size;
		WriteTerminator();

		return {};
	}

	template <typename CharT>
	typename BasicString<CharT>::ViewType BasicString<CharT>::View() const
	{
		return ViewType(m_Data, m_Size);
	}

	template <typename CharT>
	std::expected<typename BasicString<CharT>::ViewType, Error> BasicString<CharT>::Substr(SizeType offset, SizeType count) const
	{
		return View().Substr(offset, count);
	}

	template <typename CharT>
	bool BasicString<CharT>::Equals(ViewType other) const
	{
		return View().Equals(other);
	}

	template <typename CharT>
	int BasicString<CharT>::Compare(ViewType other) const
	{
		return View().Compare(other);
	}

	template <typename CharT>
	bool BasicString<CharT>::operator==(ViewType other) const
	{
		return Equals(other);
	}

	template <typename CharT>
	bool BasicString<CharT>::operator!=(ViewType other) const
	{
		return not Equals(other);
	}

	template <typename CharT>
	bool BasicString<CharT>::UsesEmptyStorage() const
	{
		return m_Data == m_Empty;
	}

	template <typename CharT>
	std::expected<void, Error> BasicString<CharT>::Grow(SizeType capacity)
	{
		auto allocation = m_Allocator->Allocate((capacity + 1) * sizeof(CharT), alignof(CharT));

		if (not allocation)
		{
			return std::unexpected(allocation.error());
		}

		Pointer newData = static_cast<Pointer>(*allocation);

		for (SizeType i = 0; i < m_Size; ++i)
		{
			newData[i] = m_Data[i];
		}

		newData[m_Size] = CharT {};
		ReleaseStorage();

		m_Data = newData;
		m_Capacity = capacity;

		return {};
	}

	template <typename CharT>
	void BasicString<CharT>::ReleaseStorage()
	{
		if (not UsesEmptyStorage() and m_Allocator != nullptr)
		{
			m_Allocator->Deallocate(m_Data, (m_Capacity + 1) * sizeof(CharT));
		}
	}

	template <typename CharT>
	void BasicString<CharT>::WriteTerminator()
	{
		m_Data[m_Size] = CharT {};
	}
} // namespace mlsl
