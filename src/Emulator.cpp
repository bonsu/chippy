 //
//  Emulator.cpp
//  Chip8Emu
//
//  Created by Tyron Bonsu on 27/04/2016.
//  Copyright © 2016 bonsu. All rights reserved.
//

#include <cassert>
#include <cstdint>
#include <initializer_list>
#include <limits>
#include <random>
#include <fstream>

#include "Emulator.hpp"

#define __NOT_IMPLEMENTED__ assert(false);
#define __NOT_IMPLEMENTED_CONTINUE__ return;

std::random_device rd;
std::mt19937 rndGenerator(rd());

Emulator::Emulator()
{
    // load font data into memory;
    for (uint32_t f : { 0xF999F, 0x26227, 0xF1F8F, 0xF1F1F, 0x99F11, 0xF8F1F, 0xF8F9F, 0xF1244,
                        0xF9F9F, 0xF9F1F, 0xF9F99, 0xE9E9E, 0xF888F, 0xE999E, 0xF8F8F, 0xF8F88 }) {
        auto *p = memory;
        for (int i = 5; i > 0; --i) {
            uint8_t b = ((f >> ((i - 1) * 4)) & 0xF) << 4;
            *(p++) = b;
        }
    }
    
    statInstructionCount = 0;
    
    clsOpcodeFunc();
    
    pc = 0x200;
}

Emulator::~Emulator() {}

bool Emulator::loadBinary(const std::string& progName)
{
    std::ifstream file (progName, std::ios::in | std::ios::binary | std::ios::ate);
    if (file.is_open()) {
        std::streampos size = file.tellg();
        if (size > (0x1000-0x200)) {
            // program size to big
            return false;
        }
        file.seekg(0, std::ios::beg);
        file.read((char*)(memory+0x200), size); // Eurgh, replace with constants.
    }
    else {
        // failed to open file.
        return false;
    }
    
    return true;
}

void Emulator::setKeyPressed(const uint8_t key)
{
    keys[key] = 1;
    
    if (waitForKey) {
        vReg[op_x] = key;
        waitForKey = false;
    }
}

void Emulator::setKeyReleased(const uint8_t key)
{
    keys[key] = 0;
}

void Emulator::cpuCycle()
{
    // encapsulate everything in wait for key check
    if (!waitForKey) {
        // fetch, decode, execute;
        uint16_t opcode = (memory[pc] << 8) | memory[pc + 1]; pc += 2;
        decodeInstr(opcode);
        // call the right opcode function for opcode.
        (this->*opcodeFuncTable[op_instr])();
        if (delayTimer)
            --delayTimer;
        
        // TODO: implement
        if (soundTimer)
            --soundTimer;
        
        ++statInstructionCount;
    }
}

void Emulator::decodeInstr(const uint16_t opcode)
{
    op_instr = opcode >> 12;
    op_nnn = opcode & 0xFFF;
    op_n =  opcode & 0xF;
    op_x = (opcode >> 8) & 0xF;
    op_y = (opcode >> 4) & 0xF;
    op_kk = opcode & 0xFF;
}

void Emulator::opcodeZeroDispatch()
{
    if (op_kk == 0xE0)
        this->clsOpcodeFunc();
    else if (op_kk == 0xEE)
        this->retOpcodeFunc();
    else
        this->sysOpcodeFunc();
}

void Emulator::opcodeEightDispatch()
{
    (this->*opcodeEFuncTable[op_n])();
}

void Emulator::opcodeEDispatch()
{
    if (op_n == 0xE)
        this->skpOpcodeFunc();
    else
        this->sknpOpcodeFunc();
}

void Emulator::opcodeFDispatch()
{
    if (op_kk == 0x07)
        this->ldDelayRegOpcodeFunc();
    if (op_kk == 0x0A)
        this->ldRegKeyOpcodeFunc();
    if (op_kk == 0x15)
        this->ldDelayRegOpcodeFunc();
    if (op_kk == 0x18)
        this->ldSoundRegOpcodeFunc();
    if (op_kk == 0x1E)
        this->addIRegOpcodeFunc();
    if (op_kk == 0x29)
        this->ldFRegOpcodeFunc();
    if (op_kk == 0x33)
        this->ldBRegOpcodeFunc();
    if (op_kk == 0x55)
        this->ldMemRegOpcodeFunc();
    if (op_kk == 0x65)
        this->ldRegMemOpcodeFunc();
}

void Emulator::clsOpcodeFunc()
{
    for (int i = 0; i < 32; ++i)
        for (int j = 0; j < 64; ++j)
            display[i][j] = 0;
}

void Emulator::retOpcodeFunc()
{
    pc = stack[sp--];
}

void Emulator::sysOpcodeFunc()
{
    __NOT_IMPLEMENTED__
}

void Emulator::jpOpcodeFunc()
{
    pc = op_nnn;
}

void Emulator::callOpcodeFunc()
{
    stack[++sp] = pc;
    pc = op_nnn;
    
}

void Emulator::seByteOpcodeFunc()
{
    if (vReg[op_x] == op_kk)
        pc += 2;
}

void Emulator::sneByteOpcodeFunc()
{
    if (vReg[op_x] != op_kk)
        pc += 2;
}

void Emulator::seRegOpcodeFunc()
{
    if (vReg[op_x] == vReg[op_y])
        pc += 2;
}

void Emulator::ldRegByteOpcodeFunc()
{
    vReg[op_x] = op_kk;
}

void Emulator::addRegByteOpcodeFunc()
{
    vReg[op_x] = vReg[op_x] + op_kk;
}

void Emulator::ldRegRegOpcodeFunc()
{
    vReg[op_x] = vReg[op_y];
}

void Emulator::orOpcodeFunc()
{
    vReg[op_x] = vReg[op_x] | vReg[op_y];
}

void Emulator::andOpcodeFunc()
{
    vReg[op_x] = vReg[op_x] & vReg[op_y];
}

void Emulator::xorOpcodeFunc()
{
    vReg[op_x] = vReg[op_x] ^ vReg[op_y];
}

void Emulator::addRegRegOpcodeFunc()
{
    uint16_t r = vReg[op_x] + vReg[op_y];
    if (r > std::numeric_limits<uint8_t>::max())
        vReg[VF] = 1;
    
    vReg[op_x] = r & 0xFF;;
    vReg[VF] = 0;
}

void Emulator::subOpcodeFunc()
{
    if (vReg[op_x] > vReg[op_y])
        vReg[VF] = 1;
    else
        vReg[VF] = 0;
    
    vReg[op_x] = vReg[op_x] - vReg[op_y];
}

void Emulator::shrOpcodeFunc()
{
    if (vReg[op_x] & 1)
        vReg[VF] = 1;
    else
        vReg[VF] = 0;
    vReg[op_x] >>= 1;
}

void Emulator::subnOpcodeFunc()
{
    if (vReg[op_y] > vReg[op_x])
        vReg[VF] = 1;
    else
        vReg[VF] = 0;
    
    vReg[op_x] = vReg[op_y] - vReg[op_x];
    
}

void Emulator::shlOpcodeFunc()
{
    if ((vReg[op_x] >> 7) & 1)
        vReg[VF] = 1;
    else
        vReg[VF] = 0;
    vReg[op_x] <<= 1;
}

void Emulator::sneRegRegOpcodeFunc()
{
    if (vReg[op_x] != vReg[op_y])
        pc += 2;
}

void Emulator::ldIOpcodeFunc()
{
    I = op_nnn;
}

void Emulator::jpV0OpcodeFunc()
{
    pc = V0 + op_nnn;
}

void Emulator::rndOpcodeFunc()
{

    std::uniform_int_distribution<> dis(0, 255);
    
    vReg[op_x] = dis(rndGenerator) & op_kk;
    
}

void Emulator::drwOpcodeFunc()
{
    vReg[VF] = 0;
    for (int y = 0; y < op_n; ++y) {
        auto pixel = memory[I + y];
        for (int x = 0; x < 8; ++x) {
            if ((pixel & (0x80 >> x)) != 0) {
                if (display[y + vReg[op_y]][x + vReg[op_x]] == 1)
                    vReg[VF] = 1;
                display[y + vReg[op_y]][x + vReg[op_x]] ^= 1;
            }
        }
    }
    drawDisplay = true;
}

void Emulator::skpOpcodeFunc()
{
    if (keys[vReg[op_x]] == 1)
        pc += 2;
}

void Emulator::sknpOpcodeFunc()
{
    if (keys[vReg[op_x]] == 0)
        pc += 2;
}

void Emulator::ldRegDelayOpcodeFunc()
{
    vReg[op_x] = delayTimer;
}

void Emulator::ldRegKeyOpcodeFunc()
{
    waitForKey = true;
}

void Emulator::ldDelayRegOpcodeFunc()
{
    delayTimer = vReg[op_x];
}

void Emulator::ldSoundRegOpcodeFunc()
{
    soundTimer = vReg[op_x];
}

void Emulator::addIRegOpcodeFunc()
{
    I += vReg[op_x];
}

void Emulator::ldFRegOpcodeFunc()
{
    I = *(font + ((vReg[op_x] * 5) + 1));
}

void Emulator::ldBRegOpcodeFunc()
{
    memory[I] = vReg[op_x] / 100;
    memory[I + 1] = (vReg[op_x] / 10) % 10;
    memory[I + 2] = vReg[op_x] % 10;
}

void Emulator::ldMemRegOpcodeFunc()
{
    for (int i = 0; i < 16; ++i)
        memory[I + i] = vReg[i];
}

void Emulator::ldRegMemOpcodeFunc()
{
    for (int i = 0; i < 16; ++i)
        vReg[i] = memory[I + i];
}


