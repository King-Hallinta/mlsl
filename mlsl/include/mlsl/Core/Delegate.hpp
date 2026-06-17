// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <utility>

namespace mlsl
{
	template <typename Signature>
	class Delegate;

	template <typename R, typename... Args>
	class Delegate<R(Args...)>
	{
	public:
		using ResultType = R;
		using FunctionPointer = R (*)(Args...);

	public:
		constexpr Delegate() = default;
		constexpr Delegate(std::nullptr_t);
		constexpr Delegate(FunctionPointer function);

	public:
		constexpr Delegate &operator=(std::nullptr_t);
		constexpr Delegate &operator=(FunctionPointer function);

	public:
		template <FunctionPointer Function>
		[[nodiscard]] static constexpr Delegate From();

		template <typename T, R (T::*Method)(Args...)>
		[[nodiscard]] static constexpr Delegate From(T *object);

		template <typename T, R (T::*Method)(Args...) const>
		[[nodiscard]] static constexpr Delegate From(const T *object);

	public:
		constexpr void Bind(FunctionPointer function);

		template <typename Callable>
		constexpr void Bind(Callable &callable);

		template <typename Callable>
		constexpr void Bind(const Callable &callable);

		template <typename T, R (T::*Method)(Args...)>
		constexpr void Bind(T *object);

		template <typename T, R (T::*Method)(Args...) const>
		constexpr void Bind(const T *object);

		constexpr void Reset();
		[[nodiscard]] constexpr bool Empty() const;
		[[nodiscard]] constexpr explicit operator bool() const;

		constexpr R operator()(Args... args) const;

	private:
		static constexpr R InvokeFunction(void *, FunctionPointer function, Args &&...args);

		template <FunctionPointer Function>
		static constexpr R InvokeStaticFunction(void *, FunctionPointer, Args &&...args);

		template <typename Callable>
		static constexpr R InvokeCallable(void *target, FunctionPointer, Args &&...args);

		template <typename Callable>
		static constexpr R InvokeConstCallable(void *target, FunctionPointer, Args &&...args);

		template <typename T, R (T::*Method)(Args...)>
		static constexpr R InvokeMethod(void *target, FunctionPointer, Args &&...args);

		template <typename T, R (T::*Method)(Args...) const>
		static constexpr R InvokeConstMethod(void *target, FunctionPointer, Args &&...args);

	private:
		using Stub = R (*)(void *, FunctionPointer, Args &&...);

	private:
		void *m_Target = nullptr;
		FunctionPointer m_Function = nullptr;
		Stub m_Invoke = nullptr;
	};
} // namespace mlsl

#include <mlsl/Core/Delegate.inl>
