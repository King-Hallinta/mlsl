// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cassert>
#include <type_traits>

namespace mlsl
{
	template <typename R, typename... Args>
	constexpr Delegate<R(Args...)>::Delegate(std::nullptr_t)
	{
	}

	template <typename R, typename... Args>
	constexpr Delegate<R(Args...)>::Delegate(FunctionPointer function)
	{
		Bind(function);
	}

	template <typename R, typename... Args>
	constexpr Delegate<R(Args...)> &Delegate<R(Args...)>::operator=(std::nullptr_t)
	{
		Reset();

		return *this;
	}

	template <typename R, typename... Args>
	constexpr Delegate<R(Args...)> &Delegate<R(Args...)>::operator=(FunctionPointer function)
	{
		Bind(function);

		return *this;
	}

	template <typename R, typename... Args>
	template <typename Delegate<R(Args...)>::FunctionPointer Function>
	constexpr Delegate<R(Args...)> Delegate<R(Args...)>::From()
	{
		Delegate delegate;
		delegate.m_Invoke = &InvokeStaticFunction<Function>;

		return delegate;
	}

	template <typename R, typename... Args>
	template <typename T, R (T::*Method)(Args...)>
	constexpr Delegate<R(Args...)> Delegate<R(Args...)>::From(T *object)
	{
		Delegate delegate;
		delegate.template Bind<T, Method>(object);

		return delegate;
	}

	template <typename R, typename... Args>
	template <typename T, R (T::*Method)(Args...) const>
	constexpr Delegate<R(Args...)> Delegate<R(Args...)>::From(const T *object)
	{
		Delegate delegate;
		delegate.template Bind<T, Method>(object);

		return delegate;
	}

	template <typename R, typename... Args>
	constexpr void Delegate<R(Args...)>::Bind(FunctionPointer function)
	{
		if (function == nullptr)
		{
			Reset();

			return;
		}

		m_Target = nullptr;
		m_Function = function;
		m_Invoke = &InvokeFunction;
	}

	template <typename R, typename... Args>
	template <typename Callable>
	constexpr void Delegate<R(Args...)>::Bind(Callable &callable)
	{
		m_Target = &callable;
		m_Function = nullptr;
		m_Invoke = &InvokeCallable<Callable>;
	}

	template <typename R, typename... Args>
	template <typename Callable>
	constexpr void Delegate<R(Args...)>::Bind(const Callable &callable)
	{
		m_Target = const_cast<Callable *>(&callable);
		m_Function = nullptr;
		m_Invoke = &InvokeConstCallable<Callable>;
	}

	template <typename R, typename... Args>
	template <typename T, R (T::*Method)(Args...)>
	constexpr void Delegate<R(Args...)>::Bind(T *object)
	{
		if (object == nullptr)
		{
			Reset();

			return;
		}

		m_Target = object;
		m_Function = nullptr;
		m_Invoke = &InvokeMethod<T, Method>;
	}

	template <typename R, typename... Args>
	template <typename T, R (T::*Method)(Args...) const>
	constexpr void Delegate<R(Args...)>::Bind(const T *object)
	{
		if (object == nullptr)
		{
			Reset();

			return;
		}

		m_Target = const_cast<T *>(object);
		m_Function = nullptr;
		m_Invoke = &InvokeConstMethod<T, Method>;
	}

	template <typename R, typename... Args>
	constexpr void Delegate<R(Args...)>::Reset()
	{
		m_Target = nullptr;
		m_Function = nullptr;
		m_Invoke = nullptr;
	}

	template <typename R, typename... Args>
	constexpr bool Delegate<R(Args...)>::Empty() const
	{
		return m_Invoke == nullptr;
	}

	template <typename R, typename... Args>
	constexpr Delegate<R(Args...)>::operator bool() const
	{
		return not Empty();
	}

	template <typename R, typename... Args>
	constexpr R Delegate<R(Args...)>::operator()(Args... args) const
	{
		assert(m_Invoke != nullptr);

		return m_Invoke(m_Target, m_Function, std::forward<Args>(args)...);
	}

	template <typename R, typename... Args>
	constexpr R Delegate<R(Args...)>::InvokeFunction(void *, FunctionPointer function, Args &&...args)
	{
		if constexpr (std::is_void_v<R>)
		{
			function(std::forward<Args>(args)...);
		}
		else
		{
			return function(std::forward<Args>(args)...);
		}
	}

	template <typename R, typename... Args>
	template <typename Delegate<R(Args...)>::FunctionPointer Function>
	constexpr R Delegate<R(Args...)>::InvokeStaticFunction(void *, FunctionPointer, Args &&...args)
	{
		if constexpr (std::is_void_v<R>)
		{
			Function(std::forward<Args>(args)...);
		}
		else
		{
			return Function(std::forward<Args>(args)...);
		}
	}

	template <typename R, typename... Args>
	template <typename Callable>
	constexpr R Delegate<R(Args...)>::InvokeCallable(void *target, FunctionPointer, Args &&...args)
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
	constexpr R Delegate<R(Args...)>::InvokeConstCallable(void *target, FunctionPointer, Args &&...args)
	{
		if constexpr (std::is_void_v<R>)
		{
			(*static_cast<const Callable *>(target))(std::forward<Args>(args)...);
		}
		else
		{
			return (*static_cast<const Callable *>(target))(std::forward<Args>(args)...);
		}
	}

	template <typename R, typename... Args>
	template <typename T, R (T::*Method)(Args...)>
	constexpr R Delegate<R(Args...)>::InvokeMethod(void *target, FunctionPointer, Args &&...args)
	{
		if constexpr (std::is_void_v<R>)
		{
			(static_cast<T *>(target)->*Method)(std::forward<Args>(args)...);
		}
		else
		{
			return (static_cast<T *>(target)->*Method)(std::forward<Args>(args)...);
		}
	}

	template <typename R, typename... Args>
	template <typename T, R (T::*Method)(Args...) const>
	constexpr R Delegate<R(Args...)>::InvokeConstMethod(void *target, FunctionPointer, Args &&...args)
	{
		if constexpr (std::is_void_v<R>)
		{
			(static_cast<const T *>(target)->*Method)(std::forward<Args>(args)...);
		}
		else
		{
			return (static_cast<const T *>(target)->*Method)(std::forward<Args>(args)...);
		}
	}
} // namespace mlsl
