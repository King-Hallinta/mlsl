// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace mlsl
{
	template <typename CharT, std::size_t N>
	BasicInlineString<CharT, N>::BasicInlineString()
		: m_Allocator(new DefaultAllocator()), m_OwnsAllocator(true)
	{
		WriteTerminator();
	}

	template <typename CharT, std::size_t N>
	BasicInlineString<CharT, N>::BasicInlineString(Allocator *allocator)
		: m_Allocator(allocator == nullptr ? new DefaultAllocator() : allocator), m_OwnsAllocator(allocator == nullptr)
	{
		WriteTerminator();
	}

	template <typename CharT, std::size_t N>
	BasicInlineString<CharT, N>::BasicInlineString(std::nullptr_t)
		: BasicInlineString()
	{
	}

	template <typename CharT, std::size_t N>
	BasicInlineString<CharT, N>::BasicInlineString(ConstPointer data)
		: BasicInlineString()
	{
		static_cast<void>(Assign(data));
	}

	template <typename CharT, std::size_t N>
	BasicInlineString<CharT, N>::BasicInlineString(ViewType view)
		: BasicInlineString()
	{
		static_cast<void>(Assign(view));
	}

	template <typename CharT, std::size_t N>
	BasicInlineString<CharT, N>::BasicInlineString(const BasicInlineString &other)
		: BasicInlineString()
	{
		static_cast<void>(Assign(other.View()));
	}

	template <typename CharT, std::size_t N>
	BasicInlineString<CharT, N>::BasicInlineString(BasicInlineString &&other) noexcept
		: BasicInlineString()
	{
		auto result = Assign(other.View());

		if (result)
		{
			other.Clear();
		}
	}

	template <typename CharT, std::size_t N>
	BasicInlineString<CharT, N>::~BasicInlineString()
	{
		ReleaseHeapStorage();

		if (m_OwnsAllocator)
		{
			delete m_Allocator;
		}
	}

	template <typename CharT, std::size_t N>
	BasicInlineString<CharT, N> &BasicInlineString<CharT, N>::operator=(const BasicInlineString &other)
	{
		if (this != &other)
		{
			static_cast<void>(Assign(other.View()));
		}

		return *this;
	}

	template <typename CharT, std::size_t N>
	BasicInlineString<CharT, N> &BasicInlineString<CharT, N>::operator=(BasicInlineString &&other) noexcept
	{
		if (this != &other)
		{
			auto result = Assign(other.View());

			if (result)
			{
				other.Clear();
			}
		}

		return *this;
	}

	template <typename CharT, std::size_t N>
	typename BasicInlineString<CharT, N>::Reference BasicInlineString<CharT, N>::operator[](SizeType index)
	{
		return m_Data[index];
	}

	template <typename CharT, std::size_t N>
	typename BasicInlineString<CharT, N>::ConstReference BasicInlineString<CharT, N>::operator[](SizeType index) const
	{
		return m_Data[index];
	}

	template <typename CharT, std::size_t N>
	std::expected<typename BasicInlineString<CharT, N>::Pointer, Error> BasicInlineString<CharT, N>::Get(SizeType index)
	{
		if (index >= m_Size)
		{
			return std::unexpected(Error {ErrorType::OutOfBounds});
		}

		return m_Data + index;
	}

	template <typename CharT, std::size_t N>
	std::expected<typename BasicInlineString<CharT, N>::ConstPointer, Error> BasicInlineString<CharT, N>::Get(SizeType index) const
	{
		if (index >= m_Size)
		{
			return std::unexpected(Error {ErrorType::OutOfBounds});
		}

		return m_Data + index;
	}

	template <typename CharT, std::size_t N>
	typename BasicInlineString<CharT, N>::Reference BasicInlineString<CharT, N>::Front()
	{
		return m_Data[0];
	}

	template <typename CharT, std::size_t N>
	typename BasicInlineString<CharT, N>::ConstReference BasicInlineString<CharT, N>::Front() const
	{
		return m_Data[0];
	}

	template <typename CharT, std::size_t N>
	typename BasicInlineString<CharT, N>::Reference BasicInlineString<CharT, N>::Back()
	{
		return m_Data[m_Size - 1];
	}

	template <typename CharT, std::size_t N>
	typename BasicInlineString<CharT, N>::ConstReference BasicInlineString<CharT, N>::Back() const
	{
		return m_Data[m_Size - 1];
	}

	template <typename CharT, std::size_t N>
	typename BasicInlineString<CharT, N>::Pointer BasicInlineString<CharT, N>::Data()
	{
		return m_Data;
	}

	template <typename CharT, std::size_t N>
	typename BasicInlineString<CharT, N>::ConstPointer BasicInlineString<CharT, N>::Data() const
	{
		return m_Data;
	}

	template <typename CharT, std::size_t N>
	typename BasicInlineString<CharT, N>::ConstPointer BasicInlineString<CharT, N>::CString() const
	{
		return m_Data;
	}

	template <typename CharT, std::size_t N>
	typename BasicInlineString<CharT, N>::Iterator BasicInlineString<CharT, N>::Begin()
	{
		return m_Data;
	}

	template <typename CharT, std::size_t N>
	typename BasicInlineString<CharT, N>::ConstIterator BasicInlineString<CharT, N>::Begin() const
	{
		return m_Data;
	}

	template <typename CharT, std::size_t N>
	typename BasicInlineString<CharT, N>::Iterator BasicInlineString<CharT, N>::End()
	{
		return m_Data + m_Size;
	}

	template <typename CharT, std::size_t N>
	typename BasicInlineString<CharT, N>::ConstIterator BasicInlineString<CharT, N>::End() const
	{
		return m_Data + m_Size;
	}

	template <typename CharT, std::size_t N>
	typename BasicInlineString<CharT, N>::SizeType BasicInlineString<CharT, N>::Size() const
	{
		return m_Size;
	}

	template <typename CharT, std::size_t N>
	typename BasicInlineString<CharT, N>::SizeType BasicInlineString<CharT, N>::Capacity() const
	{
		return m_Capacity;
	}

	template <typename CharT, std::size_t N>
	typename BasicInlineString<CharT, N>::SizeType BasicInlineString<CharT, N>::InlineCapacity() const
	{
		return N;
	}

	template <typename CharT, std::size_t N>
	bool BasicInlineString<CharT, N>::Empty() const
	{
		return m_Size == 0;
	}

	template <typename CharT, std::size_t N>
	std::expected<void, Error> BasicInlineString<CharT, N>::Reserve(SizeType capacity)
	{
		if (capacity <= m_Capacity)
		{
			return {};
		}

		return Grow(capacity);
	}

	template <typename CharT, std::size_t N>
	void BasicInlineString<CharT, N>::Clear()
	{
		m_Size = 0;
		WriteTerminator();
	}

	template <typename CharT, std::size_t N>
	std::expected<void, Error> BasicInlineString<CharT, N>::Assign(ConstPointer data)
	{
		return Assign(ViewType(data));
	}

	template <typename CharT, std::size_t N>
	std::expected<void, Error> BasicInlineString<CharT, N>::Assign(ViewType view)
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

	template <typename CharT, std::size_t N>
	std::expected<void, Error> BasicInlineString<CharT, N>::Append(ConstPointer data)
	{
		return Append(ViewType(data));
	}

	template <typename CharT, std::size_t N>
	std::expected<void, Error> BasicInlineString<CharT, N>::Append(ViewType view)
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

	template <typename CharT, std::size_t N>
	std::expected<void, Error> BasicInlineString<CharT, N>::Append(CharT value)
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

	template <typename CharT, std::size_t N>
	std::expected<void, Error> BasicInlineString<CharT, N>::Insert(SizeType offset, ConstPointer data)
	{
		return Insert(offset, ViewType(data));
	}

	template <typename CharT, std::size_t N>
	std::expected<void, Error> BasicInlineString<CharT, N>::Insert(SizeType offset, ViewType view)
	{
		return Replace(offset, 0, view);
	}

	template <typename CharT, std::size_t N>
	std::expected<void, Error> BasicInlineString<CharT, N>::Insert(SizeType offset, CharT value)
	{
		return Replace(offset, 0, value);
	}

	template <typename CharT, std::size_t N>
	std::expected<void, Error> BasicInlineString<CharT, N>::Erase(SizeType offset, SizeType count)
	{
		if (offset > m_Size)
		{
			return std::unexpected(Error {ErrorType::OutOfBounds});
		}

		SizeType eraseCount = count == Npos or count > m_Size - offset ? m_Size - offset : count;

		for (SizeType i = offset + eraseCount; i < m_Size; ++i)
		{
			m_Data[i - eraseCount] = m_Data[i];
		}

		m_Size -= eraseCount;
		WriteTerminator();

		return {};
	}

	template <typename CharT, std::size_t N>
	std::expected<void, Error> BasicInlineString<CharT, N>::Replace(SizeType offset, SizeType count, ConstPointer data)
	{
		return Replace(offset, count, ViewType(data));
	}

	template <typename CharT, std::size_t N>
	std::expected<void, Error> BasicInlineString<CharT, N>::Replace(SizeType offset, SizeType count, ViewType view)
	{
		if (offset > m_Size)
		{
			return std::unexpected(Error {ErrorType::OutOfBounds});
		}

		SizeType replaceCount = count == Npos or count > m_Size - offset ? m_Size - offset : count;
		SizeType newSize = m_Size - replaceCount + view.Size();
		ConstPointer replaceData = view.Data();
		Pointer copiedData = nullptr;

		if (replaceData != nullptr and view.Size() > 0)
		{
			auto dataStart = reinterpret_cast<std::uintptr_t>(m_Data);
			auto dataEnd = dataStart + m_Size * sizeof(CharT);
			auto replaceStart = reinterpret_cast<std::uintptr_t>(replaceData);
			bool aliasesStorage = replaceStart >= dataStart and replaceStart < dataEnd;

			if (aliasesStorage)
			{
				auto allocation = m_Allocator->Allocate(view.Size() * sizeof(CharT), alignof(CharT));

				if (not allocation)
				{
					return std::unexpected(allocation.error());
				}

				copiedData = static_cast<Pointer>(*allocation);

				for (SizeType i = 0; i < view.Size(); ++i)
				{
					copiedData[i] = replaceData[i];
				}

				replaceData = copiedData;
			}
		}

		auto result = Reserve(newSize);

		if (not result)
		{
			if (copiedData != nullptr)
			{
				m_Allocator->Deallocate(copiedData, view.Size() * sizeof(CharT));
			}

			return result;
		}

		if (view.Size() > replaceCount)
		{
			SizeType growth = view.Size() - replaceCount;

			for (SizeType i = m_Size; i > offset + replaceCount; --i)
			{
				m_Data[i + growth - 1] = m_Data[i - 1];
			}
		}
		else if (replaceCount > view.Size())
		{
			SizeType shrink = replaceCount - view.Size();

			for (SizeType i = offset + replaceCount; i < m_Size; ++i)
			{
				m_Data[i - shrink] = m_Data[i];
			}
		}

		for (SizeType i = 0; i < view.Size(); ++i)
		{
			m_Data[offset + i] = replaceData[i];
		}

		m_Size = newSize;
		WriteTerminator();

		if (copiedData != nullptr)
		{
			m_Allocator->Deallocate(copiedData, view.Size() * sizeof(CharT));
		}

		return {};
	}

	template <typename CharT, std::size_t N>
	std::expected<void, Error> BasicInlineString<CharT, N>::Replace(SizeType offset, SizeType count, CharT value)
	{
		return Replace(offset, count, ViewType(&value, 1));
	}

	template <typename CharT, std::size_t N>
	void BasicInlineString<CharT, N>::Remove()
	{
		if (m_Size == 0)
		{
			return;
		}

		--m_Size;
		WriteTerminator();
	}

	template <typename CharT, std::size_t N>
	std::expected<void, Error> BasicInlineString<CharT, N>::Resize(SizeType size, CharT value)
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

	template <typename CharT, std::size_t N>
	typename BasicInlineString<CharT, N>::ViewType BasicInlineString<CharT, N>::View() const
	{
		return ViewType(m_Data, m_Size);
	}

	template <typename CharT, std::size_t N>
	std::expected<typename BasicInlineString<CharT, N>::ViewType, Error> BasicInlineString<CharT, N>::Slice(SizeType offset, SizeType count) const
	{
		return View().Slice(offset, count);
	}

	template <typename CharT, std::size_t N>
	std::expected<typename BasicInlineString<CharT, N>::ViewType, Error> BasicInlineString<CharT, N>::Substr(SizeType offset, SizeType count) const
	{
		return View().Substr(offset, count);
	}

	template <typename CharT, std::size_t N>
	bool BasicInlineString<CharT, N>::Equals(ViewType other) const
	{
		return View().Equals(other);
	}

	template <typename CharT, std::size_t N>
	int BasicInlineString<CharT, N>::Compare(ViewType other) const
	{
		return View().Compare(other);
	}

	template <typename CharT, std::size_t N>
	bool BasicInlineString<CharT, N>::operator==(ViewType other) const
	{
		return Equals(other);
	}

	template <typename CharT, std::size_t N>
	bool BasicInlineString<CharT, N>::operator!=(ViewType other) const
	{
		return not Equals(other);
	}

	template <typename CharT, std::size_t N>
	bool BasicInlineString<CharT, N>::IsInline() const
	{
		return m_Data == m_InlineStorage;
	}

	template <typename CharT, std::size_t N>
	std::expected<void, Error> BasicInlineString<CharT, N>::Grow(SizeType capacity)
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
		ReleaseHeapStorage();

		m_Data = newData;
		m_Capacity = capacity;

		return {};
	}

	template <typename CharT, std::size_t N>
	void BasicInlineString<CharT, N>::ReleaseHeapStorage()
	{
		if (not IsInline() and m_Allocator != nullptr)
		{
			m_Allocator->Deallocate(m_Data, (m_Capacity + 1) * sizeof(CharT));
		}
	}

	template <typename CharT, std::size_t N>
	void BasicInlineString<CharT, N>::WriteTerminator()
	{
		m_Data[m_Size] = CharT {};
	}
} // namespace mlsl
