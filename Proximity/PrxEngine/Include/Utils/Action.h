#pragma once
#include <functional>
#include <vector>

#define PRX_ACTION_MAX_DELEGATES 20;

namespace Proximity::Utils
{
	// Saves a list of function pointers as delegates, returns void
	template <typename... Params>
	class Action
	{
	private:
		// Custom wrappers for function pointers
		struct Delegate
		{
			template <typename Func>
			Delegate(Func&& f, size_t hash)
				:
				m_Function(f),
				m_HashCode(hash)
			{}

			std::function<void(Params...)> m_Function;
			size_t                      m_HashCode;
		};


	public:
		Action()
		{
			m_maxDelegates = PRX_ACTION_MAX_DELEGATES;
		}

		Action(Math::U32 limit)
		{
			m_maxDelegates = limit;
		}

		~Action()
		{
			m_delegates.clear();
		}

		// Returns true if subscription successful, false otherwise
		template <typename Func>
		bool operator+=(Func&& f)
		{
			if (m_delegates.size() > m_maxDelegates)
				return false;

			auto& info = typeid(f);
			size_t hash = info.hash_code();

			for (auto& del : m_delegates)
			{
				// Functor already exists, return
				if (del.m_HashCode == hash)
					return false;
			}
			
			// Delegate does not exist, add to vector
			Delegate d = Delegate(f, hash);
			m_delegates.emplace_back(d);
			m_maxDelegates++;
			return true;
		}

		// Returns true if unsunscription was successful, false otherwise
		template <typename Func>
		void operator-=(Func&& f)
		{
			auto& info = typeid(f);
			size_t hash = info.hash_code();

			for (unsigned int i = 0; i < m_delegates.size(); i++)
			{
				// Same hash code found, delete it
				if (m_delegates[i].m_HashCode == hash)
				{
					m_delegates.erase(m_delegates.begin() + i);

					m_maxDelegates--;
					if (m_maxDelegates < 0)
						m_maxDelegates = 0;
					
					return true;
				}
			}
			return false;
		}

		template <typename ...Args>
		void operator()(Args&&... args)
		{
			for (auto& del : m_delegates)
			{
				del.m_Function(args...);
			}
		}

	public:
		std::vector<Delegate> m_delegates;
		Math::U32 m_maxDelegates;
	};
}

#define PRX_ACTION_FUNC(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }