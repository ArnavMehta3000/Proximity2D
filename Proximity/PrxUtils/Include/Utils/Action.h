#pragma once
#include <functional>
#include <vector>

namespace Proximity::Utils
{

	// Saves a list of function pointers as delegates
	template <typename Params>
	class Action
	{
	private:
		// Custom wrappers for function pointers
		struct Delegate
		{
			template <typename Func>
			Delegate(Func&& f)
			{
				m_Function = f;
				auto& funcTypeInfo = typeid(m_Function);
				m_Name = funcTypeInfo.raw_name();
			}

			~Delegate()
			{
				m_Name.clear();
			}

			std::function<void(Params)> m_Function;
			std::string m_Name;
		};


	public:
		Action() {}
		~Action()
		{
			for (auto& delegate : m_delegates)
			{
				delete delegate;
			}

			m_delegates.clear();
		}

		unsigned int Size() { return m_delegates.size(); }

		template <typename Func>
		void operator+=(Func&& f)
		{
			Delegate* d = new Delegate(f);

			for (auto& f : m_delegates)
			{
				// Delegate exist in vector, return
				if (f->m_Name == d->m_Name)
				{
					delete d;
					return;
				}
			}

			// Delegate does not exist in vector, 
			m_delegates.emplace_back(d);
		}

		// TODO: Does not work, needs working
		template <typename Func>
		void operator-=(Func&& f)
		{
			Delegate* d = new Delegate(f);

			for (auto& f : m_delegates)
			{
				// Delegate exist in vector, delete it
				if (f->m_Name == d->m_Name)
				{
					std::remove(m_delegates.begin(), m_delegates.end(), f);

					delete f;
					delete d;
					return;
				}
			}
		}

		template <typename ...Args>
		void operator()(Args&&... args)
		{
			for (auto& f : m_delegates)
			{
				f->m_Function(args...);
			}
		}

	private:
		// Hold delegates instead...?
		std::vector<Delegate*> m_delegates;
	};
}

#define PRX_ACTION_BIND(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }