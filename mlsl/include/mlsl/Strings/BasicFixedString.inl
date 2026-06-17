// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace mlsl
{
	template <typename CharT, std::size_t N>
	constexpr BasicFixedString<CharT, N>::BasicFixedString()
	{
		WriteTerminator();
	}

	template <typename CharT, std::size_t N>
	constexpr BasicFixedString<CharT, N>::BasicFixedString(std::nullptr_t)
	{
		WriteTerminator();
	}

	template <typename CharT, std::size_t N>
	constexpr BasicFixedString<CharT, N>::BasicFixedString(ConstPointer data)
	{
		static_cast<void>(Assign(data));
	}

	template <typename CharT, std::size_t N>
	constexpr BasicFixedString<CharT, N>::BasicFixedString(ViewType view)
	{
		static_cast<void>(Assign(view));
	}

	template <typename CharT, std::size_t N>
	constexpr typename BasicFixedString<CharT, N>::Reference BasicFixedString<CharT, N>::operator[](SizeType index)
	{
		return m_Data[index];
	}

	template <typename CharT, std::size_t N>
	constexpr typename BasicFixedString<CharT, N>::ConstReference BasicFixedString<CharT, N>::operator[](SizeType index) const
	{
		return m_Data[index];
	}

	template <typename CharT, std::size_t N>
	constexpr std::expected<typename BasicFixedString<CharT, N>::Pointer, Error> BasicFixedString<CharT, N>::Get(SizeType index)
	{
		if (index >= m_Size)
		{
			return std::unexpected(Error {ErrorType::OutOfBounds});
		}

		return m_Data + index;
	}

	template <typename CharT, std::size_t N>
	constexpr std::expected<typename BasicFixedString<CharT, N>::ConstPointer, Error> BasicFixedString<CharT, N>::Get(SizeType index) const
	{
		if (index >= m_Size)
		{
			return std::unexpected(Error {ErrorType::OutOfBounds});
		}

		return m_Data + index;
	}

	template <typename CharT, std::size_t N>
	constexpr typename BasicFixedString<CharT, N>::Reference BasicFixedString<CharT, N>::Front()
	{
		return m_Data[0];
	}

	template <typename CharT, std::size_t N>
	constexpr typename BasicFixedString<CharT, N>::ConstReference BasicFixedString<CharT, N>::Front() const
	{
		return m_Data[0];
	}

	template <typename CharT, std::size_t N>
	constexpr typename BasicFixedString<CharT, N>::Reference BasicFixedString<CharT, N>::Back()
	{
		return m_Data[m_Size - 1];
	}

	template <typename CharT, std::size_t N>
	constexpr typename BasicFixedString<CharT, N>::ConstReference BasicFixedString<CharT, N>::Back() const
	{
		return m_Data[m_Size - 1];
	}

	template <typename CharT, std::size_t N>
	constexpr typename BasicFixedString<CharT, N>::Pointer BasicFixedString<CharT, N>::Data()
	{
		return m_Data;
	}

	template <typename CharT, std::size_t N>
	constexpr typename BasicFixedString<CharT, N>::ConstPointer BasicFixedString<CharT, N>::Data() const
	{
		return m_Data;
	}

	template <typename CharT, std::size_t N>
	constexpr typename BasicFixedString<CharT, N>::ConstPointer BasicFixedString<CharT, N>::CString() const
	{
		return m_Data;
	}

	template <typename CharT, std::size_t N>
	constexpr typename BasicFixedString<CharT, N>::Iterator BasicFixedString<CharT, N>::Begin()
	{
		return m_Data;
	}

	template <typename CharT, std::size_t N>
	constexpr typename BasicFixedString<CharT, N>::ConstIterator BasicFixedString<CharT, N>::Begin() const
	{
		return m_Data;
	}

	template <typename CharT, std::size_t N>
	constexpr typename BasicFixedString<CharT, N>::Iterator BasicFixedString<CharT, N>::End()
	{
		return m_Data + m_Size;
	}

	template <typename CharT, std::size_t N>
	constexpr typename BasicFixedString<CharT, N>::ConstIterator BasicFixedString<CharT, N>::End() const
	{
		return m_Data + m_Size;
	}

	template <typename CharT, std::size_t N>
	constexpr typename BasicFixedString<CharT, N>::SizeType BasicFixedString<CharT, N>::Size() const
	{
		return m_Size;
	}

	template <typename CharT, std::size_t N>
	constexpr typename BasicFixedString<CharT, N>::SizeType BasicFixedString<CharT, N>::Capacity() const
	{
		return N;
	}

	template <typename CharT, std::size_t N>
	constexpr bool BasicFixedString<CharT, N>::Empty() const
	{
		return m_Size == 0;
	}

	template <typename CharT, std::size_t N>
	constexpr std::expected<void, Error> BasicFixedString<CharT, N>::Reserve(SizeType capacity)
	{
		if (capacity > N)
		{
			return std::unexpected(Error {ErrorType::BufferOverflow});
		}

		return {};
	}

	template <typename CharT, std::size_t N>
	constexpr void BasicFixedString<CharT, N>::Clear()
	{
		m_Size = 0;
		WriteTerminator();
	}

	template <typename CharT, std::size_t N>
	constexpr std::expected<void, Error> BasicFixedString<CharT, N>::Assign(ConstPointer data)
	{
		return Assign(ViewType(data));
	}

	template <typename CharT, std::size_t N>
	constexpr std::expected<void, Error> BasicFixedString<CharT, N>::Assign(ViewType view)
	{
		if (view.Size() > N)
		{
			return std::unexpected(Error {ErrorType::BufferOverflow});
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
	constexpr std::expected<void, Error> BasicFixedString<CharT, N>::Append(ConstPointer data)
	{
		return Append(ViewType(data));
	}

	template <typename CharT, std::size_t N>
	constexpr std::expected<void, Error> BasicFixedString<CharT, N>::Append(ViewType view)
	{
		if (m_Size + view.Size() > N)
		{
			return std::unexpected(Error {ErrorType::BufferOverflow});
		}

		for (SizeType i = 0; i < view.Size(); ++i)
		{
			m_Data[m_Size + i] = view[i];
		}

		m_Size += view.Size();
		WriteTerminator();

		return {};
	}

	template <typename CharT, std::size_t N>
	constexpr std::expected<void, Error> BasicFixedString<CharT, N>::Append(CharT value)
	{
		if (m_Size + 1 > N)
		{
			return std::unexpected(Error {ErrorType::BufferOverflow});
		}

		m_Data[m_Size] = value;
		++m_Size;
		WriteTerminator();

		return {};
	}

	template <typename CharT, std::size_t N>
	constexpr void BasicFixedString<CharT, N>::Remove()
	{
		if (m_Size == 0)
		{
			return;
		}

		--m_Size;
		WriteTerminator();
	}

	template <typename CharT, std::size_t N>
	constexpr std::expected<void, Error> BasicFixedString<CharT, N>::Resize(SizeType size, CharT value)
	{
		if (size > N)
		{
			return std::unexpected(Error {ErrorType::BufferOverflow});
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
	constexpr typename BasicFixedString<CharT, N>::ViewType BasicFixedString<CharT, N>::View() const
	{
		return ViewType(m_Data, m_Size);
	}

	template <typename CharT, std::size_t N>
	constexpr std::expected<typename BasicFixedString<CharT, N>::ViewType, Error> BasicFixedString<CharT, N>::Substr(SizeType offset, SizeType count) const
	{
		return View().Substr(offset, count);
	}

	template <typename CharT, std::size_t N>
	constexpr bool BasicFixedString<CharT, N>::Equals(ViewType other) const
	{
		return View().Equals(other);
	}

	template <typename CharT, std::size_t N>
	constexpr int BasicFixedString<CharT, N>::Compare(ViewType other) const
	{
		return View().Compare(other);
	}

	template <typename CharT, std::size_t N>
	constexpr bool BasicFixedString<CharT, N>::operator==(ViewType other) const
	{
		return Equals(other);
	}

	template <typename CharT, std::size_t N>
	constexpr bool BasicFixedString<CharT, N>::operator!=(ViewType other) const
	{
		return not Equals(other);
	}

	template <typename CharT, std::size_t N>
	constexpr void BasicFixedString<CharT, N>::WriteTerminator()
	{
		m_Data[m_Size] = CharT {};
	}
} // namespace mlsl
