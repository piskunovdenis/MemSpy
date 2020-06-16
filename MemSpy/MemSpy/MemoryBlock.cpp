#pragma once

#include <stdexcept>
#include "MemoryBlock.hpp"

namespace ms
{
	MemoryBlock::MemoryBlock(DWORD_PTR length) :
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

	MemoryBlock::~MemoryBlock()
		{
			ZeroMemory(m_Memory, m_Length);
			free(m_Memory);

			m_Length = 0;
			m_Memory = nullptr;
		}

		DWORD_PTR MemoryBlock::Length() const
		{
			return m_Length;
		}

		void* MemoryBlock::Memory() const
		{
			return m_Memory;
		}
}