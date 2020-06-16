#pragma once

#include <windows.h>

namespace ms
{
	class MemoryBlock
	{
	public:
		MemoryBlock(DWORD_PTR length);
		MemoryBlock(MemoryBlock&& other);
		MemoryBlock(const MemoryBlock&) = delete;
		~MemoryBlock();

		MemoryBlock& operator=(MemoryBlock&&) = delete;
		MemoryBlock& operator=(const MemoryBlock&) = delete;

		DWORD_PTR Length() const;
		void* Memory() const;
	private:
		DWORD_PTR m_Length;
		void* m_Memory;
	};
}