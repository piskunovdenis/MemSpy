#pragma once

#include <windows.h>

namespace ms
{
	class MemoryBlock
	{
	public:
		MemoryBlock(DWORD_PTR length);
		~MemoryBlock();

		DWORD_PTR Length() const;
		void* Memory() const;
	private:
		DWORD_PTR m_Length;
		void* m_Memory;
	};
}