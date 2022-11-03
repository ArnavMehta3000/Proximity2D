#include "Memory/StackAllocator.h"
#include "Utils/Assertions.h"
#include <stdlib.h>

namespace Proximity::Memory
{
	StackAllocator::StackAllocator(Math::U32 size)
		:
		m_size(size)
	{
		PRX_ASSERT_MSG(size < 0, "Allocated size must be greater than zero!");
		m_stack = malloc(size);
		m_topMarker = 0;
	}

	StackAllocator::~StackAllocator()
	{
		free(m_stack);
	}

	void* StackAllocator::Alloc(Math::U32 size)
	{
		PRX_ASSERT_MSG((m_topMarker + size) < m_size, "Insufficient space in allocator");
		void* chunk = static_cast<void*>(static_cast<char*>(m_stack) + m_topMarker);
		m_topMarker += size;
		return chunk;
	}

	void StackAllocator::FreeToMarker(StackMarker m)
	{
		PRX_ASSERT_MSG(m >= 0, "Cannot free memory to a negative address");
		PRX_ASSERT_MSG(m < m_topMarker, "Value of 'm' should be less than current top of the stack");

		m_topMarker = m;
	}

	void StackAllocator::Clear()
	{
		m_topMarker = 0;
	}

	StackMarker StackAllocator::GetMarker()
	{
		return m_topMarker;
	}
}
