#pragma once
namespace Proximity::Core
{
	class UUID
	{
	public:
		UUID();
		UUID(Math::U64 uuid);
		UUID(const UUID&) = default;

		std::string ToString();
		operator Math::U64() const noexcept { return  m_uuid; }

	private:
		Math::U64 m_uuid;
	};
}

namespace std
{
	template <typename T> struct hash;

	template<>
	struct hash<Proximity::Core::UUID>
	{
		std::size_t operator()(const Proximity::Core::UUID& uuid) const
		{
			return (uint64_t)uuid;
		}
	};
}