// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace mlsl
{
	template <typename T>
	WeakPtr<T>::WeakPtr(std::nullptr_t)
		: m_Ptr(nullptr),
		  m_Control(nullptr)
	{
	}

	template <typename T>
	WeakPtr<T>::WeakPtr(const SharedPtr<T> &shared)
		: m_Ptr(shared.m_Ptr),
		  m_Control(shared.m_Control)
	{
		if (m_Control)
		{
			++m_Control->weakCount;
		}
	}

	template <typename T>
	WeakPtr<T>::WeakPtr(const WeakPtr &other)
		: m_Ptr(other.m_Ptr),
		  m_Control(other.m_Control)
	{
		if (m_Control)
		{
			++m_Control->weakCount;
		}
	}

	template <typename T>
	WeakPtr<T>::WeakPtr(WeakPtr &&other) noexcept
		: m_Ptr(other.m_Ptr),
		  m_Control(other.m_Control)
	{
		other.m_Ptr = nullptr;
		other.m_Control = nullptr;
	}

	template <typename T>
	template <typename U>
		requires std::is_convertible_v<U *, T *>
	WeakPtr<T>::WeakPtr(const SharedPtr<U> &shared)
		: m_Ptr(shared.m_Ptr),
		  m_Control(shared.m_Control)
	{
		if (m_Control)
		{
			++m_Control->weakCount;
		}
	}

	template <typename T>
	template <typename U>
		requires std::is_convertible_v<U *, T *>
	WeakPtr<T>::WeakPtr(const WeakPtr<U> &other)
		: m_Ptr(other.m_Ptr),
		  m_Control(other.m_Control)
	{
		if (m_Control)
		{
			++m_Control->weakCount;
		}
	}

	template <typename T>
	template <typename U>
		requires std::is_convertible_v<U *, T *>
	WeakPtr<T>::WeakPtr(WeakPtr<U> &&other) noexcept
		: m_Ptr(other.m_Ptr),
		  m_Control(other.m_Control)
	{
		other.m_Ptr = nullptr;
		other.m_Control = nullptr;
	}

	template <typename T>
	WeakPtr<T>::~WeakPtr()
	{
		Release();
	}

	template <typename T>
	WeakPtr<T> &WeakPtr<T>::operator=(const SharedPtr<T> &shared)
	{
		Release();

		m_Ptr = shared.m_Ptr;
		m_Control = shared.m_Control;

		if (m_Control)
		{
			++m_Control->weakCount;
		}

		return *this;
	}

	template <typename T>
	WeakPtr<T> &WeakPtr<T>::operator=(const WeakPtr &other)
	{
		if (this != &other)
		{
			Release();

			m_Ptr = other.m_Ptr;
			m_Control = other.m_Control;

			if (m_Control)
			{
				++m_Control->weakCount;
			}
		}

		return *this;
	}

	template <typename T>
	WeakPtr<T> &WeakPtr<T>::operator=(WeakPtr &&other) noexcept
	{
		if (this != &other)
		{
			Release();

			m_Ptr = other.m_Ptr;
			m_Control = other.m_Control;
			other.m_Ptr = nullptr;
			other.m_Control = nullptr;
		}

		return *this;
	}

	template <typename T>
	WeakPtr<T> &WeakPtr<T>::operator=(std::nullptr_t) noexcept
	{
		Release();

		return *this;
	}

	template <typename T>
	template <typename U>
		requires std::is_convertible_v<U *, T *>
	WeakPtr<T> &WeakPtr<T>::operator=(const SharedPtr<U> &shared)
	{
		Release();

		m_Ptr = shared.m_Ptr;
		m_Control = shared.m_Control;

		if (m_Control)
		{
			++m_Control->weakCount;
		}

		return *this;
	}

	template <typename T>
	template <typename U>
		requires std::is_convertible_v<U *, T *>
	WeakPtr<T> &WeakPtr<T>::operator=(const WeakPtr<U> &other)
	{
		Release();

		m_Ptr = other.m_Ptr;
		m_Control = other.m_Control;

		if (m_Control)
		{
			++m_Control->weakCount;
		}

		return *this;
	}

	template <typename T>
	template <typename U>
		requires std::is_convertible_v<U *, T *>
	WeakPtr<T> &WeakPtr<T>::operator=(WeakPtr<U> &&other) noexcept
	{
		Release();

		m_Ptr = other.m_Ptr;
		m_Control = other.m_Control;
		other.m_Ptr = nullptr;
		other.m_Control = nullptr;

		return *this;
	}

	template <typename T>
	std::optional<SharedPtr<T>> WeakPtr<T>::Lock() const
	{
		if (m_Control and m_Control->strongCount > 0)
		{
			return SharedPtr<T>(*this);
		}

		return std::nullopt;
	}

	template <typename T>
	bool WeakPtr<T>::Expired() const
	{
		return m_Control == nullptr or m_Control->strongCount == 0;
	}

	template <typename T>
	std::uint32_t WeakPtr<T>::RefCount() const
	{
		if (m_Control)
		{
			return m_Control->strongCount;
		}

		return 0;
	}

	template <typename T>
	void WeakPtr<T>::Reset()
	{
		Release();
	}

	template <typename T>
	void WeakPtr<T>::Swap(WeakPtr &other) noexcept
	{
		Pointer oldPtr = m_Ptr;
		SharedControlBlock *oldControl = m_Control;

		m_Ptr = other.m_Ptr;
		m_Control = other.m_Control;
		other.m_Ptr = oldPtr;
		other.m_Control = oldControl;
	}

	template <typename T>
	void WeakPtr<T>::Release()
	{
		if (m_Control)
		{
			if (--m_Control->weakCount == 0 and m_Control->strongCount == 0)
			{
				delete m_Control;
			}
		}

		m_Ptr = nullptr;
		m_Control = nullptr;
	}
} // namespace mlsl
