#pragma once

namespace Proximity::Utils
{
	// Base struct to be used for service locator
	struct IService
	{
		friend class ServiceLocator;
		inline size_t ServiceID() { return m_Id; }

	protected:
		size_t m_Id = -999;
	};

	class ServiceLocator
	{
	public:
		ServiceLocator() noexcept;
		~ServiceLocator() noexcept;

	public:

		// Returns true if service was successfully registered, false otherwise
		template <typename T>
		bool RegisterService()
		{
			size_t hashVal = GetHash<T>();

			if (!HasService(hashVal))
			{
				// Create the service and assign it's hash value
				m_services[hashVal]       = std::make_unique<T>();
				m_services[hashVal]->m_Id = hashVal;

				PRX_LOG_DEBUG("Registering service hash code: %u", hashVal);

				return true;
			}
			return false;
		}

		// Returns true if service was successfully un-registered, false otherwise
		template <typename T>
		bool UnRegisterService()
		{
			size_t hashVal = GetHash<T>();

			if (HasService(hashVal))
			{
				PRX_LOG_DEBUG("Removing service hash code: %u", hashVal);
				m_services.erase(hashVal);
				return true;
			}
			return false;
		}

		// Returns the service requested, nullptr if not found
		template <typename T>
		T* ResolveService()
		{
			size_t hashVal = GetHash<T>();

			if (HasService(hashVal))
			{
				IService* service = m_services.at(hashVal).get();
				return static_cast<T*>(service);
			}
			else
				return nullptr;
		}

		// Returns service if found otherwise creates the service and returns nullptr
		template <typename T>
		T* ResolveOrRegisterService()
		{
			size_t hashVal = GetHash<T>();

			if (HasService(hashVal))
				return ResolveService<T>();
			else
			{
				RegisterService<T>();
				return ResolveService<T>();
			}
		}

		bool HasService(size_t service);

	private:
		std::unordered_map<size_t, std::unique_ptr<IService>> m_services;

		template<typename T>
		size_t GetHash()
		{
			IsValid<T>();  // Validity check done while getting hash value

			auto& serviceType = typeid(T);
			return serviceType.hash_code();
		}
		
		template <typename T>
		void IsValid()
		{
			static_assert(std::is_base_of<IService, T>::value, "Service class should inherit from IService interface");
		}
	};
}