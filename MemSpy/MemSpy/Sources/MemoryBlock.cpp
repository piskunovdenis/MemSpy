#include "MemoryBlock.hpp"

namespace memspy
{
	MemoryBlock::MemoryBlock(DWORD_PTR length) :
		m_Length(length),
		m_Memory(nullptr)
	{
		m_Memory = std::make_unique<BYTE[]>(length);
		ZeroMemory(m_Memory.get(), m_Length);
	}

	MemoryBlock::~MemoryBlock()
	{
		ZeroMemory(m_Memory.get(), m_Length);
		m_Length = 0;
		m_Memory = nullptr;
	}

	DWORD_PTR MemoryBlock::Length() const
	{
		return m_Length;
	}

	void* MemoryBlock::Memory() const
	{
		return m_Memory.get();
	}
}