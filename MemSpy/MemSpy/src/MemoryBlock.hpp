#pragma once

#include <windows.h>
#include <memory>

namespace memspy
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
		std::unique_ptr<BYTE[]> m_Memory;
	};
}