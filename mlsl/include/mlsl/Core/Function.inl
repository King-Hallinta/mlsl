// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cassert>
#include <new>

namespace mlsl
{
	template <typename R, typename... Args>
	constexpr Function<R(Args...)>::Function(std::nullptr_t)
	{
	}

	template <typename R, typename... Args>
	constexpr Function<R(Args...)>::Function(const Function &other)
	{
		if (other.m_Copy)
		{
			other.m_Copy(Storage(), other.Storage());
			m_Invoke = other.m_Invoke;
			m_Destroy = other.m_Destroy;
			m_Copy = other.m_Copy;
			m_Move = other.m_Move;
		}
	}

	template <typename R, typename... Args>
	constexpr Function<R(Args...)>::Function(Function &&other)
	{
		if (other.m_Move)
		{
			other.m_Move(Storage(), other.Storage());
			m_Invoke = other.m_Invoke;
			m_Destroy = other.m_Destroy;
			m_Copy = other.m_Copy;
			m_Move = other.m_Move;
			other.m_Invoke = nullptr;
			other.m_Destroy = nullptr;
			other.m_Copy = nullptr;
			other.m_Move = nullptr;
		}
	}

	template <typename R, typename... Args>
	constexpr Function<R(Args...)>::~Function()
	{
		Reset();
	}

	template <typename R, typename... Args>
	template <typename Callable>
		requires Detail::CompatibleCallable<Callable, R, Args...>
	constexpr Function<R(Args...)>::Function(Callable &&callable)
	{
		Bind(std::forward<Callable>(callable));
	}

	template <typename R, typename... Args>
	constexpr Function<R(Args...)> &Function<R(Args...)>::operator=(std::nullptr_t)
	{
		Reset();

		return *this;
	}

	template <typename R, typename... Args>
	constexpr Function<R(Args...)> &Function<R(Args...)>::operator=(const Function &other)
	{
		if (this != &other)
		{
			Reset();

			if (other.m_Copy)
			{
				other.m_Copy(Storage(), other.Storage());
				m_Invoke = other.m_Invoke;
				m_Destroy = other.m_Destroy;
				m_Copy = other.m_Copy;
				m_Move = other.m_Move;
			}
		}

		return *this;
	}

	template <typename R, typename... Args>
	constexpr Function<R(Args...)> &Function<R(Args...)>::operator=(Function &&other)
	{
		if (this != &other)
		{
			Reset();

			if (other.m_Move)
			{
				other.m_Move(Storage(), other.Storage());
				m_Invoke = other.m_Invoke;
				m_Destroy = other.m_Destroy;
				m_Copy = other.m_Copy;
				m_Move = other.m_Move;
				other.m_Invoke = nullptr;
				other.m_Destroy = nullptr;
				other.m_Copy = nullptr;
				other.m_Move = nullptr;
			}
		}

		return *this;
	}

	template <typename R, typename... Args>
	template <typename Callable>
		requires Detail::CompatibleCallable<Callable, R, Args...>
	constexpr Function<R(Args...)> &Function<R(Args...)>::operator=(Callable &&callable)
	{
		Bind(std::forward<Callable>(callable));

		return *this;
	}

	template <typename R, typename... Args>
	template <typename Callable>
		requires Detail::CompatibleCallable<Callable, R, Args...>
	constexpr void Function<R(Args...)>::Bind(Callable &&callable)
	{
		using CallableType = std::decay_t<Callable>;

		Reset();

		new (Storage()) CallableType(std::forward<Callable>(callable));
		m_Invoke = &Invoke<CallableType>;
		m_Destroy = &Destroy<CallableType>;
		m_Copy = &Copy<CallableType>;
		m_Move = &Move<CallableType>;
	}

	template <typename R, typename... Args>
	constexpr void Function<R(Args...)>::Reset()
	{
		if (m_Destroy)
		{
			m_Destroy(Storage());
		}

		m_Invoke = nullptr;
		m_Destroy = nullptr;
		m_Copy = nullptr;
		m_Move = nullptr;
	}

	template <typename R, typename... Args>
	constexpr bool Function<R(Args...)>::Empty() const
	{
		return m_Invoke == nullptr;
	}

	template <typename R, typename... Args>
	constexpr Function<R(Args...)>::operator bool() const
	{
		return not Empty();
	}

	template <typename R, typename... Args>
	constexpr R Function<R(Args...)>::operator()(Args... args) const
	{
		assert(m_Invoke != nullptr);

		return m_Invoke(Storage(), std::forward<Args>(args)...);
	}

	template <typename R, typename... Args>
	constexpr void Function<R(Args...)>::Swap(Function &other)
	{
		Function temporary(std::move(other));
		other = std::move(*this);
		*this = std::move(temporary);
	}

	template <typename R, typename... Args>
	constexpr void *Function<R(Args...)>::Storage()
	{
		return m_Storage;
	}

	template <typename R, typename... Args>
	constexpr void *Function<R(Args...)>::Storage() const
	{
		return m_Storage;
	}

	template <typename R, typename... Args>
	template <typename Callable>
	constexpr R Function<R(Args...)>::Invoke(void *target, Args &&...args)
	{
		if constexpr (std::is_void_v<R>)
		{
			(*static_cast<Callable *>(target))(std::forward<Args>(args)...);
		}
		else
		{
			return (*static_cast<Callable *>(target))(std::forward<Args>(args)...);
		}
	}

	template <typename R, typename... Args>
	template <typename Callable>
	constexpr void Function<R(Args...)>::Destroy(void *target)
	{
		static_cast<Callable *>(target)->~Callable();
	}

	template <typename R, typename... Args>
	template <typename Callable>
	constexpr void Function<R(Args...)>::Copy(void *destination, const void *source)
	{
		new (destination) Callable(*static_cast<const Callable *>(source));
	}

	template <typename R, typename... Args>
	template <typename Callable>
	constexpr void Function<R(Args...)>::Move(void *destination, void *source)
	{
		new (destination) Callable(std::move(*static_cast<Callable *>(source)));
		static_cast<Callable *>(source)->~Callable();
	}
} // namespace mlsl
