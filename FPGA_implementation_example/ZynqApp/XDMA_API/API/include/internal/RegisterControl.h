/* 
 *  File: IPControl.h
 *  Copyright (c) 2021 Florian Porrmann
 *  
 *  MIT License
 *  
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *  
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *  
 */

#pragma once

#include "../xdmaAccess.h"
#include "RegisterInterface.h"
#include "Utils.h"

#include <vector>

// TODO: Move this to a more appropriate place
enum class DMAChannel
{
	MM2S,
	S2MM
};

class RegisterControlBase : public XDMAManaged
{
	DISABLE_COPY_ASSIGN_MOVE(RegisterControlBase)

public:
	RegisterControlBase(std::shared_ptr<class XDMABase> pXdma, const uint64_t& ctrlOffset) :
		XDMAManaged(pXdma),
		m_ctrlOffset(ctrlOffset),
		m_registers()
	{}

	// Method used by the static update callback function to update
	// the given register
	template<typename T>
	void UpdateRegister(Register<T>* pReg, const uint64_t& offset, const Direction& dir)
	{
		if (dir == Direction::READ)
			pReg->Update(readRegister<T>(offset));
		else
			writeRegister<T>(offset, pReg->GetValue());
	}

	// Updates all registered registers
	void UpdateAllRegisters()
	{
		for (RegisterIntf* pReg : m_registers)
			pReg->Update(Direction::READ);
	}

	// Callback function, called by the register when the Update() method is called
	template<typename T>
	static void UpdateCallBack(Register<T>* pReg, const uint64_t& offset, const Direction& dir, void* pObj)
	{
		// Make sure that the given object pointer is in fact an RegisterControlBase object
		RegisterControlBase* pIPCtrl = static_cast<RegisterControlBase*>(pObj);
		if (!pIPCtrl) return;

		pIPCtrl->UpdateRegister(pReg, offset, dir);
	}

protected:
	// Register a register to the list of known registers and
	// setup its update callback function
	template<typename T>
	void registerReg(Register<T>& reg, const uint64_t& offset = 0x0)
	{
		if constexpr (sizeof(T) > sizeof(uint64_t))
		{
			std::stringstream ss("");
			ss << CLASS_TAG("") << "Registers with a size > " << sizeof(uint64_t) << " byte are currently not supported";
			throw std::runtime_error(ss.str());
		}

		reg.SetupCallBackBasedUpdate(this, offset, UpdateCallBack<T>);
		m_registers.push_back(&reg);
	}

	template<typename T>
	T readRegister(const uint64_t& regOffset)
	{
		switch (sizeof(T))
		{
			case 8:
				return static_cast<T>(XDMA()->Read64(m_ctrlOffset + regOffset));
			case 4:
				return static_cast<T>(XDMA()->Read32(m_ctrlOffset + regOffset));
			case 2:
				return static_cast<T>(XDMA()->Read16(m_ctrlOffset + regOffset));
			case 1:
				return static_cast<T>(XDMA()->Read8(m_ctrlOffset + regOffset));
			default:
				std::stringstream ss("");
				ss << CLASS_TAG("") << "Registers with a size > " << sizeof(uint64_t) << " byte are currently not supported";
				throw std::runtime_error(ss.str());
		}
	}

	template<typename T>
	void writeRegister(const uint64_t& regOffset, const T& regData, const bool& validate = false)
	{
		switch (sizeof(T))
		{
			case 8:
				XDMA()->Write64(m_ctrlOffset + regOffset, static_cast<uint64_t>(regData));
				break;
			case 4:
				XDMA()->Write32(m_ctrlOffset + regOffset, static_cast<uint32_t>(regData));
				break;
			case 2:
				XDMA()->Write16(m_ctrlOffset + regOffset, static_cast<uint16_t>(regData));
				break;
			case 1:
				XDMA()->Write8(m_ctrlOffset + regOffset, static_cast<uint8_t>(regData));
				break;
			default:
				std::stringstream ss("");
				ss << CLASS_TAG("") << "Registers with a size > " << sizeof(uint64_t) << " byte are currently not supported";
				throw std::runtime_error(ss.str());
		}

		if(validate)
		{
			const T readData = readRegister<T>(regOffset);
			if (readData != regData)
			{
				std::stringstream ss("");
				ss << CLASS_TAG("") << "Register write validation failed. Expected: 0x" << std::hex << regData << ", Read: 0x" << readData << std::dec;
				throw std::runtime_error(ss.str());
			}
		}
	}

	uint32_t getDevNum()
	{
		return XDMA()->GetDevNum();
	}

protected:
	uint64_t m_ctrlOffset;
	std::vector<RegisterIntf*> m_registers;
};
