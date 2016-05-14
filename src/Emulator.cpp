 //
//  Emulator.cpp
//  Chip8Emu
//
//  Created by Tyron Bonsu on 27/04/2016.
//  Copyright © 2016 bonsu. All rights reserved.
//

#include <bitset>
#include <cassert>
#include <cstdint>
#include <initializer_list>
#include <fstream>
#include <limits>
#include <random>

#include "Emulator.hpp"

#define debug

#define __NOT_IMPLEMENTED__ assert(true);
#define __NOT_IMPLEMENTED_CONTINUE__ return;

#ifdef debug
#define DBG_OUT cinder::app::console() << std::setw(0) <<  std::setfill(' ')

#define DBG_PRINT(s) do { DBG_OUT << s << std::endl; } while (0)

#define DBG_PRINT_FUNC do { DBG_OUT << "->" << __func__ << "()" << std::endl; } while (0)

#define DBG_PRINT_VAR(var) do { DBG_OUT << "\t" << std::setw(15) << #var << " \t= 0x" << std::setw(2) << std::setfill('0') << std::right << std::hex << (int)var << std::setfill(' ') << std::endl; } while (0)

#define DBG_PRINT_VAR_DEC(var) do { DBG_OUT << "\t" << std::setw(15) << #var << " \t= " << std::dec << (int)var << std::setfill(' ') << std::endl; } while (0)

#define DBG_PRINT_REG do { { int i = 0; \
                             DBG_OUT << "\t";\
                             for (uint8_t v : vReg) {\
                                 DBG_OUT << "V" << i << ": 0x" << std::setw(2) << std::setfill('0') << std::right << std::hex << (int)v;\
                                 DBG_OUT << "  "; \
                                 ++i;\
                             }\
                             DBG_OUT << std::setfill(' ');\
                             DBG_OUT << std::endl;\
                            }\
                          } while (0)

#define DBG_PRINT_PIXEL_DATA(pixel) do {\
                                        DBG_OUT << "\t" << "px: 0x" << std::setw(2) << std::setfill('0') << std::right << std::hex << (int)pixel \
                                        << "\t" << std::bitset<8>(pixel) << std::endl;\
                                    } while (0)

#else
#define DBG_OUT
#define DBG_PRINT_FUNC
#endif

std::random_device rd;
std::mt19937 rndGenerator(rd());

Emulator::Emulator()
{
    initialize();
}

Emulator::~Emulator() {}

void Emulator::initialize(bool reset)
{
    if (!reset) {
        auto *p = memory;
        for (uint32_t f : { 0xF999F, 0x26227, 0xF1F8F, 0xF1F1F, 0x99F11, 0xF8F1F, 0xF8F9F, 0xF1244,
                            0xF9F9F, 0xF9F1F, 0xF9F99, 0xE9E9E, 0xF888F, 0xE999E, 0xF8F8F, 0xF8F88 }) {
           for (int i = 5; i > 0; --i) {
               uint8_t b = ((f >> ((i - 1) * 4)) & 0xF) << 4;
               *(p++) = b;
           }
        }
    }
    
    // clear memory
    for (int i = 0 + 0x50; i < 0x1000; ++i)
        memory[i] = 0;
    for (int i = 0; i < 16; ++i)
        vReg[i] = keys[i] = stack[i] = 0;
    
    delayTimer = soundTimer = 0;
    sp = 0;
    I = 0;
    pc = 0x200;
    
    statInstructionCount = 0;
    
    // clear the display
    clsOpcodeFunc();
}

void Emulator::reset()
{
    DBG_PRINT("resetting...");
    initialize(true);
}

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
    
    currentProgram = progName;
    
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
        DBG_OUT << std::setw(4) << std::setfill('0') <<std::hex << opcode << std::setfill(' ');
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
    DBG_PRINT_FUNC;
    for (int i = 0; i < 32; ++i)
        for (int j = 0; j < 64; ++j)
            display[i][j] = 0;
}

void Emulator::retOpcodeFunc()
{
    DBG_PRINT_FUNC;
    pc = stack[sp--];
}

void Emulator::sysOpcodeFunc()
{
    DBG_PRINT_FUNC;
    __NOT_IMPLEMENTED_CONTINUE__
}

void Emulator::jpOpcodeFunc()
{
    DBG_PRINT_FUNC;
    pc = op_nnn;
}

void Emulator::callOpcodeFunc()
{
    DBG_PRINT_FUNC;
    stack[++sp] = pc;
    pc = op_nnn;
    
}

void Emulator::seByteOpcodeFunc()
{
    DBG_PRINT_FUNC;
    if (vReg[op_x] == op_kk)
        pc += 2;
}

void Emulator::sneByteOpcodeFunc()
{
    DBG_PRINT_FUNC;
    if (vReg[op_x] != op_kk)
        pc += 2;
}

void Emulator::seRegOpcodeFunc()
{
    DBG_PRINT_FUNC;
    if (vReg[op_x] == vReg[op_y])
        pc += 2;
}

void Emulator::ldRegByteOpcodeFunc()
{
    DBG_PRINT_FUNC;
    vReg[op_x] = op_kk;
}

void Emulator::addRegByteOpcodeFunc()
{
    DBG_PRINT_FUNC;
    vReg[op_x] = vReg[op_x] + op_kk;
}

void Emulator::ldRegRegOpcodeFunc()
{
    DBG_PRINT_FUNC;
    vReg[op_x] = vReg[op_y];
}

void Emulator::orOpcodeFunc()
{
    DBG_PRINT_FUNC;
    vReg[op_x] = vReg[op_x] | vReg[op_y];
}

void Emulator::andOpcodeFunc()
{
    DBG_PRINT_FUNC;
    vReg[op_x] = vReg[op_x] & vReg[op_y];
}

void Emulator::xorOpcodeFunc()
{
    DBG_PRINT_FUNC;
    vReg[op_x] = vReg[op_x] ^ vReg[op_y];
}

void Emulator::addRegRegOpcodeFunc()
{
    DBG_PRINT_FUNC;
    uint16_t r = vReg[op_x] + vReg[op_y];
    if (r > std::numeric_limits<uint8_t>::max())
        vReg[VF] = 1;
    else
        vReg[VF] = 0;
    
    vReg[op_x] = r & 0xFF;
    
}

void Emulator::subOpcodeFunc()
{
    DBG_PRINT_FUNC;
    if (vReg[op_x] > vReg[op_y])
        vReg[VF] = 1;
    else
        vReg[VF] = 0;
    
    vReg[op_x] = vReg[op_x] - vReg[op_y];
}

void Emulator::shrOpcodeFunc()
{
    DBG_PRINT_FUNC;
    if (vReg[op_x] & 1)
        vReg[VF] = 1;
    else
        vReg[VF] = 0;
    vReg[op_x] >>= 1;
}

void Emulator::subnOpcodeFunc()
{
    DBG_PRINT_FUNC;
    if (vReg[op_y] > vReg[op_x])
        vReg[VF] = 1;
    else
        vReg[VF] = 0;
    
    vReg[op_x] = vReg[op_y] - vReg[op_x];
    
}

void Emulator::shlOpcodeFunc()
{
    DBG_PRINT_FUNC;
    if ((vReg[op_x] >> 7) & 1)
        vReg[VF] = 1;
    else
        vReg[VF] = 0;
    vReg[op_x] <<= 1;
}

void Emulator::sneRegRegOpcodeFunc()
{
    DBG_PRINT_FUNC;
    if (vReg[op_x] != vReg[op_y])
        pc += 2;
}

void Emulator::ldIOpcodeFunc()
{
    I = op_nnn;
    DBG_PRINT_FUNC;
    DBG_PRINT_VAR(op_nnn);
    DBG_PRINT_VAR(I);
}

void Emulator::jpV0OpcodeFunc()
{
    pc = vReg[V0] + op_nnn;
    DBG_PRINT_FUNC;
    DBG_PRINT_VAR(vReg[V0]);
    DBG_PRINT_VAR(op_nnn);
    DBG_PRINT_VAR(pc);
}

void Emulator::rndOpcodeFunc()
{
    std::uniform_int_distribution<> dis(0, 255);
    auto rndNum = dis(rndGenerator);

    vReg[op_x] = rndNum & op_kk;
    
    DBG_PRINT_FUNC;
    DBG_PRINT_VAR(rndNum);
    DBG_PRINT_VAR(op_kk);
    DBG_PRINT_VAR(op_x);
    DBG_PRINT_VAR(vReg[op_x]);
}

void Emulator::drwOpcodeFunc()
{
    DBG_PRINT_FUNC;
    vReg[VF] = 0;
    for (int y = 0; y < op_n; ++y) {
        auto pixel = memory[I + y];
        //DBG_PRINT_VAR(I);
        //DBG_PRINT_VAR(y);
        //DBG_PRINT_VAR(memory[I + y]);
        DBG_PRINT_PIXEL_DATA(pixel);
        for (int x = 0; x < 8; ++x) {
            if ((pixel & (0x80 >> x)) != 0) {
                if (display[y + vReg[op_y]][x + vReg[op_x]] == 1)
                    vReg[VF] = 1;
                display[y + vReg[op_y]][x + vReg[op_x]] ^= 1;
            }
        }
    }
    drawDisplay = true;
    
    DBG_PRINT_VAR(vReg[VF]);
}

void Emulator::skpOpcodeFunc()
{
    if (keys[vReg[op_x]] == 1)
        pc += 2;
    
    DBG_PRINT_FUNC;
    DBG_PRINT_VAR(keys[vReg[op_x]]);
    DBG_PRINT_VAR(pc);
}

void Emulator::sknpOpcodeFunc()
{
    if (keys[vReg[op_x]] == 0)
        pc += 2;
    
    DBG_PRINT_FUNC;
    DBG_PRINT_VAR(keys[vReg[op_x]]);
    DBG_PRINT_VAR(pc);
}

void Emulator::ldRegDelayOpcodeFunc()
{
    vReg[op_x] = delayTimer;
    
    DBG_PRINT_FUNC;
    DBG_PRINT_VAR(delayTimer);
    DBG_PRINT_VAR(vReg[op_x]);
}

void Emulator::ldRegKeyOpcodeFunc()
{
    waitForKey = true;
    
    DBG_PRINT_FUNC;
}

void Emulator::ldDelayRegOpcodeFunc()
{
    delayTimer = vReg[op_x];
    
    DBG_PRINT_FUNC;
}

void Emulator::ldSoundRegOpcodeFunc()
{
    soundTimer = vReg[op_x];
   
    DBG_PRINT_FUNC;
}

void Emulator::addIRegOpcodeFunc()
{
    I += vReg[op_x];

    DBG_PRINT_FUNC;
}

void Emulator::ldFRegOpcodeFunc()
{
    auto fontLocation = vReg[op_x] * 5; // each font is 5 bytes
    I = fontLocation;
    
    DBG_PRINT_FUNC;
    DBG_PRINT_VAR(vReg[op_x]);
    DBG_PRINT_VAR(fontLocation);
    DBG_PRINT_VAR(I);
}

void Emulator::ldBRegOpcodeFunc()
{
    memory[I] = vReg[op_x] / 100;
    memory[I + 1] = (vReg[op_x] / 10) % 10;
    memory[I + 2] = vReg[op_x] % 10;

    DBG_PRINT_FUNC;
    DBG_PRINT_VAR_DEC(vReg[op_x]);
    DBG_PRINT_VAR_DEC(memory[I]);
    DBG_PRINT_VAR_DEC(memory[I + 1]);
    DBG_PRINT_VAR_DEC(memory[I + 2]);
}

void Emulator::ldMemRegOpcodeFunc()
{
    DBG_PRINT_FUNC;
    DBG_PRINT_REG;
    for (int i = 0; i < 16; ++i) {
        memory[I + i] = vReg[i];
        DBG_PRINT_VAR(memory[I + i]);
    }
}

void Emulator::ldRegMemOpcodeFunc()
{
    DBG_PRINT_FUNC;
    for (int i = 0; i < 16; ++i) {
        vReg[i] = memory[I + i];
        DBG_PRINT_VAR(memory[I + i]);
    }
    DBG_PRINT_REG;
}