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
			m_delegates.push_back({ f, hash });
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