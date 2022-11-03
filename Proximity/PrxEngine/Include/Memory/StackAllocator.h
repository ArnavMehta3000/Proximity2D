#pragma once

namespace Proximity::Memory
{
	// Allocates a contiguous section of memory
	typedef Math::U32 StackMarker;
	class StackAllocator
	{
	public:
		// Number of byetes rto to allocate for this stack
		StackAllocator(Math::U32 size);
		~StackAllocator();

		// Returns a chunk of memory from the top of this stack
		void* Alloc(Math::U32 size);
		// Clears the stack down to a previous marker. All data below is preserved
		void FreeToMarker(StackMarker m);
		// Clears the stack and resets the top marker
		void Clear();

		// Returns the position of the top of the stack
		StackMarker GetMarker();


	private:
		StackMarker m_topMarker;
		void* m_stack;
		Math::U32 m_size;
	};
}