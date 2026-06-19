// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mlsl/Containers/BasicMap.hpp>

namespace mlsl
{
	template <typename KeyT, typename ValueT, std::size_t N, typename LessT = std::less<KeyT>>
	class FixedMap : public BasicMap<KeyT, ValueT, FixedMap<KeyT, ValueT, N, LessT>, LessT>
	{
	public:
		using Base = BasicMap<KeyT, ValueT, FixedMap<KeyT, ValueT, N, LessT>, LessT>;
		using typename Base::SizeType;

	private:
		using Entry = typename Base::Entry;
		static constexpr SizeType StorageCount = N == 0 ? 1 : N;

	public:
		FixedMap() = default;
		FixedMap(LessT less);
		FixedMap(const FixedMap &other);
		FixedMap(FixedMap &&other) noexcept;
		~FixedMap();

	public:
		FixedMap &operator=(const FixedMap &other);
		FixedMap &operator=(FixedMap &&other) noexcept;

	public:
		[[nodiscard]] bool Full() const;

	private:
		[[nodiscard]] Entry *GetEntries();
		[[nodiscard]] const Entry *GetEntries() const;
		[[nodiscard]] SizeType GetCapacity() const;
		[[nodiscard]] std::expected<void, Error> EnsureCapacity(SizeType capacity);

		void CopyFrom(const FixedMap &other);
		void MoveFrom(FixedMap &other);

	private:
		alignas(Entry) std::byte m_Storage[sizeof(Entry) * StorageCount];

	private:
		friend Base;
	};
} // namespace mlsl

#include <mlsl/Containers/FixedMap.inl>
