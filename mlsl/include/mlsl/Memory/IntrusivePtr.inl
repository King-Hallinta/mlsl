// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace mlsl
{
	template <typename T>
	void IntrusivePointerTraits<T>::AddReference(T *object)
	{
		object->AddReference();
	}

	template <typename T>
	void IntrusivePointerTraits<T>::ReleaseReference(T *object)
	{
		object->ReleaseReference();
	}

	template <typename T, typename Traits>
	IntrusivePtr<T, Traits>::IntrusivePtr(Pointer ptr)
		: m_Ptr(ptr)
	{
		AddReference();
	}

	template <typename T, typename Traits>
	IntrusivePtr<T, Traits>::IntrusivePtr(std::nullptr_t)
		: m_Ptr(nullptr)
	{
	}

	template <typename T, typename Traits>
	IntrusivePtr<T, Traits>::IntrusivePtr(const IntrusivePtr &other)
		: m_Ptr(other.m_Ptr)
	{
		AddReference();
	}

	template <typename T, typename Traits>
	IntrusivePtr<T, Traits>::IntrusivePtr(IntrusivePtr &&other) noexcept
		: m_Ptr(other.m_Ptr)
	{
		other.m_Ptr = nullptr;
	}

	template <typename T, typename Traits>
	template <typename U, typename OtherTraits>
		requires std::is_convertible_v<U *, T *>
	IntrusivePtr<T, Traits>::IntrusivePtr(const IntrusivePtr<U, OtherTraits> &other)
		: m_Ptr(other.m_Ptr)
	{
		AddReference();
	}

	template <typename T, typename Traits>
	template <typename U, typename OtherTraits>
		requires std::is_convertible_v<U *, T *>
	IntrusivePtr<T, Traits>::IntrusivePtr(IntrusivePtr<U, OtherTraits> &&other) noexcept
		: m_Ptr(other.m_Ptr)
	{
		other.m_Ptr = nullptr;
	}

	template <typename T, typename Traits>
	IntrusivePtr<T, Traits>::~IntrusivePtr()
	{
		ReleaseReference();
	}

	template <typename T, typename Traits>
	IntrusivePtr<T, Traits> &IntrusivePtr<T, Traits>::operator=(const IntrusivePtr &other)
	{
		if (this != &other)
		{
			Pointer oldPtr = m_Ptr;

			m_Ptr = other.m_Ptr;
			AddReference();

			if (oldPtr)
			{
				Traits::ReleaseReference(oldPtr);
			}
		}

		return *this;
	}

	template <typename T, typename Traits>
	IntrusivePtr<T, Traits> &IntrusivePtr<T, Traits>::operator=(IntrusivePtr &&other) noexcept
	{
		if (this != &other)
		{
			ReleaseReference();

			m_Ptr = other.m_Ptr;
			other.m_Ptr = nullptr;
		}

		return *this;
	}

	template <typename T, typename Traits>
	IntrusivePtr<T, Traits> &IntrusivePtr<T, Traits>::operator=(std::nullptr_t) noexcept
	{
		ReleaseReference();

		return *this;
	}

	template <typename T, typename Traits>
	template <typename U, typename OtherTraits>
		requires std::is_convertible_v<U *, T *>
	IntrusivePtr<T, Traits> &IntrusivePtr<T, Traits>::operator=(const IntrusivePtr<U, OtherTraits> &other)
	{
		Pointer oldPtr = m_Ptr;

		m_Ptr = other.m_Ptr;
		AddReference();

		if (oldPtr)
		{
			Traits::ReleaseReference(oldPtr);
		}

		return *this;
	}

	template <typename T, typename Traits>
	template <typename U, typename OtherTraits>
		requires std::is_convertible_v<U *, T *>
	IntrusivePtr<T, Traits> &IntrusivePtr<T, Traits>::operator=(IntrusivePtr<U, OtherTraits> &&other) noexcept
	{
		ReleaseReference();

		m_Ptr = other.m_Ptr;
		other.m_Ptr = nullptr;

		return *this;
	}

	template <typename T, typename Traits>
	typename IntrusivePtr<T, Traits>::Pointer IntrusivePtr<T, Traits>::Get() const
	{
		return m_Ptr;
	}

	template <typename T, typename Traits>
	typename IntrusivePtr<T, Traits>::Pointer IntrusivePtr<T, Traits>::operator->() const
	{
		return m_Ptr;
	}

	template <typename T, typename Traits>
	typename IntrusivePtr<T, Traits>::Reference IntrusivePtr<T, Traits>::operator*() const
	{
		return *m_Ptr;
	}

	template <typename T, typename Traits>
	IntrusivePtr<T, Traits>::operator bool() const
	{
		return m_Ptr != nullptr;
	}

	template <typename T, typename Traits>
	void IntrusivePtr<T, Traits>::Reset(Pointer ptr)
	{
		Pointer oldPtr = m_Ptr;

		m_Ptr = ptr;
		AddReference();

		if (oldPtr)
		{
			Traits::ReleaseReference(oldPtr);
		}
	}

	template <typename T, typename Traits>
	typename IntrusivePtr<T, Traits>::Pointer IntrusivePtr<T, Traits>::Release()
	{
		Pointer ptr = m_Ptr;
		m_Ptr = nullptr;

		return ptr;
	}

	template <typename T, typename Traits>
	void IntrusivePtr<T, Traits>::Swap(IntrusivePtr &other) noexcept
	{
		Pointer oldPtr = m_Ptr;

		m_Ptr = other.m_Ptr;
		other.m_Ptr = oldPtr;
	}

	template <typename T, typename Traits>
	void IntrusivePtr<T, Traits>::AddReference()
	{
		if (m_Ptr)
		{
			Traits::AddReference(m_Ptr);
		}
	}

	template <typename T, typename Traits>
	void IntrusivePtr<T, Traits>::ReleaseReference()
	{
		if (m_Ptr)
		{
			Traits::ReleaseReference(m_Ptr);
			m_Ptr = nullptr;
		}
	}
} // namespace mlsl
