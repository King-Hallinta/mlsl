// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace mlsl
{
	template <typename CharT>
	constexpr BasicStringView<CharT>::BasicStringView(std::nullptr_t)
	{
	}

	template <typename CharT>
	constexpr BasicStringView<CharT>::BasicStringView(ConstPointer data)
		: m_Data(data), m_Size(Length(data))
	{
	}

	template <typename CharT>
	constexpr BasicStringView<CharT>::BasicStringView(ConstPointer data, SizeType size)
		: m_Data(data), m_Size(data == nullptr ? 0 : size)
	{
	}

	template <typename CharT>
	constexpr BasicStringView<CharT>::BasicStringView(ConstPointer begin, ConstPointer end)
		: m_Data(begin), m_Size(begin == nullptr or end == nullptr ? 0 : static_cast<SizeType>(end - begin))
	{
	}

	template <typename CharT>
	constexpr typename BasicStringView<CharT>::ConstReference BasicStringView<CharT>::operator[](SizeType index) const
	{
		return m_Data[index];
	}

	template <typename CharT>
	constexpr std::expected<typename BasicStringView<CharT>::ConstPointer, Error> BasicStringView<CharT>::Get(SizeType index) const
	{
		if (index >= m_Size)
		{
			return std::unexpected(Error {ErrorType::OutOfBounds});
		}

		return m_Data + index;
	}

	template <typename CharT>
	constexpr typename BasicStringView<CharT>::ConstReference BasicStringView<CharT>::Front() const
	{
		return m_Data[0];
	}

	template <typename CharT>
	constexpr typename BasicStringView<CharT>::ConstReference BasicStringView<CharT>::Back() const
	{
		return m_Data[m_Size - 1];
	}

	template <typename CharT>
	constexpr typename BasicStringView<CharT>::ConstPointer BasicStringView<CharT>::Data() const
	{
		return m_Data;
	}

	template <typename CharT>
	constexpr typename BasicStringView<CharT>::ConstIterator BasicStringView<CharT>::Begin() const
	{
		return m_Data;
	}

	template <typename CharT>
	constexpr typename BasicStringView<CharT>::ConstIterator BasicStringView<CharT>::End() const
	{
		if (m_Data == nullptr)
		{
			return nullptr;
		}

		return m_Data + m_Size;
	}

	template <typename CharT>
	constexpr typename BasicStringView<CharT>::SizeType BasicStringView<CharT>::Size() const
	{
		return m_Size;
	}

	template <typename CharT>
	constexpr bool BasicStringView<CharT>::Empty() const
	{
		return m_Size == 0;
	}

	template <typename CharT>
	constexpr std::expected<BasicStringView<CharT>, Error> BasicStringView<CharT>::Substr(SizeType offset, SizeType count) const
	{
		if (offset > m_Size)
		{
			return std::unexpected(Error {ErrorType::OutOfBounds});
		}

		SizeType remaining = m_Size - offset;
		SizeType resultCount = count == Npos or count > remaining ? remaining : count;
		ConstPointer resultData = m_Data == nullptr ? nullptr : m_Data + offset;

		return BasicStringView(resultData, resultCount);
	}

	template <typename CharT>
	constexpr bool BasicStringView<CharT>::Equals(BasicStringView other) const
	{
		return Compare(other) == 0;
	}

	template <typename CharT>
	constexpr int BasicStringView<CharT>::Compare(BasicStringView other) const
	{
		SizeType count = m_Size < other.m_Size ? m_Size : other.m_Size;

		for (SizeType i = 0; i < count; ++i)
		{
			if (m_Data[i] < other.m_Data[i])
			{
				return -1;
			}

			if (other.m_Data[i] < m_Data[i])
			{
				return 1;
			}
		}

		if (m_Size < other.m_Size)
		{
			return -1;
		}

		if (other.m_Size < m_Size)
		{
			return 1;
		}

		return 0;
	}

	template <typename CharT>
	constexpr bool BasicStringView<CharT>::operator==(BasicStringView other) const
	{
		return Equals(other);
	}

	template <typename CharT>
	constexpr bool BasicStringView<CharT>::operator!=(BasicStringView other) const
	{
		return not Equals(other);
	}

	template <typename CharT>
	constexpr typename BasicStringView<CharT>::SizeType BasicStringView<CharT>::Length(ConstPointer data)
	{
		if (data == nullptr)
		{
			return 0;
		}

		SizeType size = 0;

		while (data[size] != CharT {})
		{
			++size;
		}

		return size;
	}
} // namespace mlsl
