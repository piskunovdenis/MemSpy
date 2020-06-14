#pragma once

#include <windows.h>

namespace ms
{
	class MemoryBlock
	{
	public:
		MemoryBlock(DWORD_PTR length) : 
			m_Length(length), 
			m_Memory(nullptr)
		{
			m_Memory = malloc(m_Length);
			if (m_Memory == nullptr)
			{
				throw std::runtime_error("malloc failed");
			}
			ZeroMemory(m_Memory, m_Length);
		}

		~MemoryBlock()
		{
			ZeroMemory(m_Memory, m_Length);
			free(m_Memory);

			m_Length = 0;
			m_Memory = nullptr;
		}

		DWORD_PTR Length() const
		{
			return m_Length;
		}

		void* Memory() const
		{
			return m_Memory;
		}
	private:
		DWORD_PTR m_Length;
		void* m_Memory;
	};
}