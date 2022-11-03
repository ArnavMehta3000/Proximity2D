#pragma once
#include "../../PrxMath/Include/Math/MathDefines.h"
#include "Utils/Assertions.h"
#include <stdlib.h>
#include <queue>

namespace Proximity::Memory
{
	template <typename T>
	class PoolAllocator
	{
	public:
		PoolAllocator(Math::U32 size)
			:
			m_size(size)
		{
			PRX_ASSERT_MSG(size < 0, "Allocated size must be greater than zero!")
			m_pool = malloc(size);

			// Load queue with availabale indices
			for (Math::U32 i = 0; i < size; i++)
			{
				m_available.push(reinterpret_cast<void*>(reinterpret_cast<char*>(m_pool) + (i * sizeof(T)));
			}
		}

		~PoolAllocator()
		{
			free(m_pool);
		}

		// Returns a pointer to the next available slot in the memory pool
		void* Alloc()
		{
			PRX_ASSERT_MSG(m_available.size() > 0, "Memory pool is full");
			void* chunk = m_availabale.front();
			m_available.pop();
			return chunk;
		}

		// Returns an allocated slot to th pool for reallocation at a alater time. Data is not cleared and is expected to be overwritten
		void DeAlloc(T* t)
		{
			PRX_ASSERT_MSG(
				reinterpret_cast<void*>(t) >= m_pool && reinterpret_cast<void*>(t) < reinterpret_cast<void*>(reinterpret_cast<char*>(m_pool) + m_size),
				"Address to deallocate is outside memory pool"
			);

			m_available.push(reinterpret_cast<void*>(t));
		}

	private:
		std::queue<void*> m_available;
		void* m_pool;
		Math::U32 m_size;
	};
}