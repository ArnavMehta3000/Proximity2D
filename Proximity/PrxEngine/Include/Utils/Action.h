#pragma once
#include <functional>
#include <vector>

namespace Proximity::Utils
{
	// Saves a list of function pointers as delegates, returns void
	template <typename Params>
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

			std::function<void(Params)> m_Function;
			size_t                      m_HashCode;
		};


	public:
		Action() {}
		~Action()
		{
			m_delegates.clear();
		}

		template <typename Func>
		void operator+=(Func&& f)
		{
			auto& info = typeid(f);
			size_t hash = info.hash_code();

			for (auto& del : m_delegates)
			{
				// Functor already exists, return
				if (del.m_HashCode == hash)
					return;
			}
			
			// Delegate does not exist, add to vector
			Delegate d = Delegate(f, hash);
			m_delegates.emplace_back(d);
		}

		// TODO: Does not work, needs working
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
					return;
				}
			}
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
	};
}

#define PRX_ACTION_FUNC(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }