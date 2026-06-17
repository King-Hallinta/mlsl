// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <type_traits>
#include <utility>

namespace mlsl
{
	template <typename Signature>
	class Function;

	namespace Detail
	{
		inline constexpr std::size_t InlineCapacity = 64;
		inline constexpr std::size_t InlineAlignment = alignof(std::max_align_t);

		template <typename Callable, typename R, typename... Args>
		concept CompatibleCallable =
			not std::is_same_v<std::decay_t<Callable>, Function<R(Args...)>> and sizeof(std::decay_t<Callable>) <= InlineCapacity and alignof(std::decay_t<Callable>) <= InlineAlignment and std::is_copy_constructible_v<std::decay_t<Callable>> and std::is_move_constructible_v<std::decay_t<Callable>> and std::is_invocable_r_v<R, std::decay_t<Callable> &, Args...>; // Enjoy now, worry later
	} // namespace Detail

	template <typename R, typename... Args>
	class Function<R(Args...)>
	{
	public:
		using ResultType = R;
		using SizeType = std::size_t;

	public:
		static constexpr SizeType InlineCapacity = Detail::InlineCapacity;
		static constexpr SizeType InlineAlignment = Detail::InlineAlignment;

	public:
		constexpr Function() = default;
		constexpr Function(std::nullptr_t);
		constexpr Function(const Function &other);
		constexpr Function(Function &&other);
		constexpr ~Function();

		template <typename Callable>
			requires Detail::CompatibleCallable<Callable, R, Args...>
		constexpr Function(Callable &&callable);

	public:
		constexpr Function &operator=(std::nullptr_t);
		constexpr Function &operator=(const Function &other);
		constexpr Function &operator=(Function &&other);

		template <typename Callable>
			requires Detail::CompatibleCallable<Callable, R, Args...>
		constexpr Function &operator=(Callable &&callable);

	public:
		template <typename Callable>
			requires Detail::CompatibleCallable<Callable, R, Args...>
		constexpr void Bind(Callable &&callable);

		constexpr void Reset();
		[[nodiscard]] constexpr bool Empty() const;
		[[nodiscard]] constexpr explicit operator bool() const;

		constexpr R operator()(Args... args) const;
		constexpr void Swap(Function &other);

	private:
		using InvokeStub = R (*)(void *, Args &&...);
		using DestroyStub = void (*)(void *);
		using CopyStub = void (*)(void *, const void *);
		using MoveStub = void (*)(void *, void *);

	private:
		[[nodiscard]] constexpr void *Storage();
		[[nodiscard]] constexpr void *Storage() const;

		template <typename Callable>
		static constexpr R Invoke(void *target, Args &&...args);

		template <typename Callable>
		static constexpr void Destroy(void *target);

		template <typename Callable>
		static constexpr void Copy(void *destination, const void *source);

		template <typename Callable>
		static constexpr void Move(void *destination, void *source);

	private:
		alignas(InlineAlignment) mutable std::byte m_Storage[InlineCapacity] {};
		InvokeStub m_Invoke = nullptr;
		DestroyStub m_Destroy = nullptr;
		CopyStub m_Copy = nullptr;
		MoveStub m_Move = nullptr;
	};
} // namespace mlsl

#include <mlsl/Core/Function.inl>
