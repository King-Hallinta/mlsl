// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mlsl/Containers/BasicSet.hpp>

namespace mlsl
{
	template <typename KeyT, std::size_t N, typename LessT = std::less<KeyT>>
	class FixedSet : public BasicSet<KeyT, FixedSet<KeyT, N, LessT>, LessT>
	{
	public:
		using Base = BasicSet<KeyT, FixedSet<KeyT, N, LessT>, LessT>;
		using typename Base::SizeType;

	private:
		static constexpr SizeType StorageCount = N == 0 ? 1 : N;

	public:
		FixedSet() = default;
		FixedSet(LessT less);
		FixedSet(const FixedSet &other);
		FixedSet(FixedSet &&other) noexcept;
		~FixedSet();

	public:
		FixedSet &operator=(const FixedSet &other);
		FixedSet &operator=(FixedSet &&other) noexcept;

	public:
		[[nodiscard]] bool Full() const;

	private:
		[[nodiscard]] KeyT *GetKeys();
		[[nodiscard]] const KeyT *GetKeys() const;
		[[nodiscard]] SizeType GetCapacity() const;
		[[nodiscard]] std::expected<void, Error> EnsureCapacity(SizeType capacity);

		void CopyFrom(const FixedSet &other);
		void MoveFrom(FixedSet &other);

	private:
		alignas(KeyT) std::byte m_Storage[sizeof(KeyT) * StorageCount];

	private:
		friend Base;
	};
} // namespace mlsl

#include <mlsl/Containers/FixedSet.inl>
