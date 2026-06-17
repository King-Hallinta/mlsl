// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace mlsl
{
	template <typename T>
	SharedPtr<T>::SharedPtr(Pointer ptr)
		: m_Ptr(ptr),
		  m_Control(nullptr)
	{
		if (m_Ptr)
		{
			m_Control = new (std::nothrow) SharedControlBlock;

			if (not m_Control)
			{
				delete m_Ptr;
				m_Ptr = nullptr;

				return;
			}

			m_Control->object = m_Ptr;
			m_Control->releaseObject = &SharedControlBlock::ReleaseTypedObject<T>;
		}
	}

	template <typename T>
	SharedPtr<T>::SharedPtr(std::nullptr_t)
		: m_Ptr(nullptr),
		  m_Control(nullptr)
	{
	}

	template <typename T>
	SharedPtr<T>::SharedPtr(const SharedPtr &other)
		: m_Ptr(other.m_Ptr),
		  m_Control(other.m_Control)
	{
		if (m_Control)
		{
			++m_Control->strongCount;
		}
	}

	template <typename T>
	SharedPtr<T>::SharedPtr(SharedPtr &&other) noexcept
		: m_Ptr(other.m_Ptr),
		  m_Control(other.m_Control)
	{
		other.m_Ptr = nullptr;
		other.m_Control = nullptr;
	}

	template <typename T>
	template <typename U>
		requires std::is_convertible_v<U *, T *>
	SharedPtr<T>::SharedPtr(const SharedPtr<U> &other)
		: m_Ptr(other.m_Ptr),
		  m_Control(other.m_Control)
	{
		if (m_Control)
		{
			++m_Control->strongCount;
		}
	}

	template <typename T>
	template <typename U>
		requires std::is_convertible_v<U *, T *>
	SharedPtr<T>::SharedPtr(SharedPtr<U> &&other) noexcept
		: m_Ptr(other.m_Ptr),
		  m_Control(other.m_Control)
	{
		other.m_Ptr = nullptr;
		other.m_Control = nullptr;
	}

	template <typename T>
	SharedPtr<T>::SharedPtr(const WeakPtr<T> &weak)
		: m_Ptr(nullptr),
		  m_Control(nullptr)
	{
		if (weak.m_Control and weak.m_Control->strongCount > 0)
		{
			m_Ptr = weak.m_Ptr;
			m_Control = weak.m_Control;
			++m_Control->strongCount;
		}
	}

	template <typename T>
	SharedPtr<T>::~SharedPtr()
	{
		Release();
	}

	template <typename T>
	SharedPtr<T> &SharedPtr<T>::operator=(const SharedPtr &other)
	{
		if (this != &other)
		{
			SharedControlBlock *oldControl = m_Control;

			m_Ptr = other.m_Ptr;
			m_Control = other.m_Control;

			if (m_Control)
			{
				++m_Control->strongCount;
			}

			if (oldControl)
			{
				if (--oldControl->strongCount == 0)
				{
					oldControl->ReleaseObject();

					if (oldControl->weakCount == 0)
					{
						delete oldControl;
					}
				}
			}
		}

		return *this;
	}

	template <typename T>
	SharedPtr<T> &SharedPtr<T>::operator=(SharedPtr &&other) noexcept
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
	SharedPtr<T> &SharedPtr<T>::operator=(std::nullptr_t) noexcept
	{
		Release();

		return *this;
	}

	template <typename T>
	template <typename U>
		requires std::is_convertible_v<U *, T *>
	SharedPtr<T> &SharedPtr<T>::operator=(const SharedPtr<U> &other)
	{
		SharedControlBlock *oldControl = m_Control;

		m_Ptr = other.m_Ptr;
		m_Control = other.m_Control;

		if (m_Control)
		{
			++m_Control->strongCount;
		}

		if (oldControl)
		{
			if (--oldControl->strongCount == 0)
			{
				oldControl->ReleaseObject();

				if (oldControl->weakCount == 0)
				{
					delete oldControl;
				}
			}
		}

		return *this;
	}

	template <typename T>
	template <typename U>
		requires std::is_convertible_v<U *, T *>
	SharedPtr<T> &SharedPtr<T>::operator=(SharedPtr<U> &&other) noexcept
	{
		Release();

		m_Ptr = other.m_Ptr;
		m_Control = other.m_Control;
		other.m_Ptr = nullptr;
		other.m_Control = nullptr;

		return *this;
	}

	template <typename T>
	typename SharedPtr<T>::Pointer SharedPtr<T>::Get() const
	{
		return m_Ptr;
	}

	template <typename T>
	typename SharedPtr<T>::Pointer SharedPtr<T>::operator->() const
	{
		return m_Ptr;
	}

	template <typename T>
	typename SharedPtr<T>::Reference SharedPtr<T>::operator*() const
	{
		return *m_Ptr;
	}

	template <typename T>
	SharedPtr<T>::operator bool() const
	{
		return m_Ptr != nullptr;
	}

	template <typename T>
	std::uint32_t SharedPtr<T>::RefCount() const
	{
		if (m_Control)
		{
			return m_Control->strongCount;
		}

		return 0;
	}

	template <typename T>
	void SharedPtr<T>::Reset(Pointer ptr)
	{
		Release();

		if (ptr)
		{
			m_Ptr = ptr;
			m_Control = new (std::nothrow) SharedControlBlock;

			if (not m_Control)
			{
				delete m_Ptr;
				m_Ptr = nullptr;

				return;
			}

			m_Control->object = m_Ptr;
			m_Control->releaseObject = &SharedControlBlock::ReleaseTypedObject<T>;
		}
	}

	template <typename T>
	void SharedPtr<T>::Swap(SharedPtr &other) noexcept
	{
		Pointer oldPtr = m_Ptr;
		SharedControlBlock *oldControl = m_Control;

		m_Ptr = other.m_Ptr;
		m_Control = other.m_Control;
		other.m_Ptr = oldPtr;
		other.m_Control = oldControl;
	}

	template <typename T>
	void SharedPtr<T>::Release()
	{
		if (m_Control)
		{
			if (--m_Control->strongCount == 0)
			{
				m_Control->ReleaseObject();

				if (m_Control->weakCount == 0)
				{
					delete m_Control;
				}
			}
		}

		m_Ptr = nullptr;
		m_Control = nullptr;
	}

	template <typename T>
	template <typename... Args>
	SharedPtr<T> SharedPtr<T>::Create(Args &&...args)
	{
		return SharedPtr(new (std::nothrow) T(std::forward<Args>(args)...));
	}
} // namespace mlsl
