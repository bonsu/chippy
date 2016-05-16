//
//  Emulator.hpp
//  Chip8Emu
//
//  Created by Tyron Bonsu on 27/04/2016.
//  Copyright © 2016 bonsu. All rights reserved.
//

#ifndef Emulator_hpp
#define Emulator_hpp

#include <cstdint>
#include <string>


const int displayWidth  = 64;
const int displayHeight = 32;

class Emulator
{
public:
    Emulator();
    ~Emulator();
    
    uint8_t display[displayHeight][displayWidth];
    bool drawDisplay = false;
    bool waitForKey = false;
    
    int statInstructionCount = 0;
    
    void reset();
    bool loadBinary(const std::string&);
    void cpuCycle();
    void setKeyPressed(uint8_t);
    void setKeyReleased(uint8_t);
    
    
private:
    uint8_t memory[0x1000];
    uint8_t *font = memory;
    uint8_t vReg[16], keys[16];
    uint8_t delayTimer, soundTimer;
    uint16_t pc, I;
    int8_t sp;
    uint16_t stack[16];
    
    std::string currentProgram {""};
    
    
    enum { V0, VF = 0xF};
    
    union {
        struct {
            uint16_t op_instr;
            uint16_t op_nnn;
            uint16_t op_n;
            uint16_t op_x;
            uint16_t op_y;
            uint16_t op_kk;
        };
    };
    
    void initialize (bool reset=false);
    
    typedef void (Emulator::*opcodeFunc)();
    
    const opcodeFunc opcodeFuncTable[16] = {
        &Emulator::opcodeZeroDispatch,   // 00E0, 00EE, 0nnn
        &Emulator::jpOpcodeFunc,         // 1nnn
        &Emulator::callOpcodeFunc,       // 2nnn
        &Emulator::seByteOpcodeFunc,     // 3xkk
        &Emulator::sneByteOpcodeFunc,    // 4xkk
        &Emulator::seRegOpcodeFunc,      // 5xy0
        &Emulator::ldRegByteOpcodeFunc,  // 6xkk
        &Emulator::addRegByteOpcodeFunc, // 7xkk
        &Emulator::opcodeEightDispatch,  // 8xy{0..7,E}
        &Emulator::sneRegRegOpcodeFunc,  // 9xy0
        &Emulator::ldIOpcodeFunc,        // Annn
        &Emulator::jpV0OpcodeFunc,       // Bnnn
        &Emulator::rndOpcodeFunc,        // Cxkk
        &Emulator::drwOpcodeFunc,        // Dxyn
        &Emulator::opcodeEDispatch,      // Ex
        &Emulator::opcodeFDispatch
    };
    
    const opcodeFunc opcodeEFuncTable[16] = {
        &Emulator::ldRegRegOpcodeFunc,   // 8xy0
        &Emulator::orOpcodeFunc,         // 8xy1
        &Emulator::andOpcodeFunc,        // 8xy2
        &Emulator::xorOpcodeFunc,        // 8xy3
        &Emulator::addRegRegOpcodeFunc,  // 8xy4
        &Emulator::subOpcodeFunc,        // 8xy5
        &Emulator::shrOpcodeFunc,        // 8xy6
        &Emulator::subnOpcodeFunc,       // 8xy7
        nullptr,                         // ---8
        nullptr,                         // ---9
        nullptr,                         // ---A
        nullptr,                         // ---B
        nullptr,                         // ---C
        nullptr,                         // ---D
        &Emulator::shlOpcodeFunc,        // 8xyE,
        nullptr                          // ---F
    };


    void decodeInstr(const uint16_t opcode);

    void opcodeZeroDispatch();
    void opcodeEightDispatch();
    void opcodeEDispatch();
    void opcodeFDispatch();
    
    void clsOpcodeFunc();
    void retOpcodeFunc();
    void sysOpcodeFunc();
    void jpOpcodeFunc();
    void callOpcodeFunc();
    void seByteOpcodeFunc();
    void sneByteOpcodeFunc();
    void seRegOpcodeFunc();
    void ldRegByteOpcodeFunc();
    void addRegByteOpcodeFunc();
    void ldRegRegOpcodeFunc();
    void orOpcodeFunc();
    void andOpcodeFunc();
    void xorOpcodeFunc();
    void addRegRegOpcodeFunc();
    void subOpcodeFunc();
    void shrOpcodeFunc();
    void subnOpcodeFunc();
    void shlOpcodeFunc();
    void sneRegRegOpcodeFunc();
    void ldIOpcodeFunc();
    void jpV0OpcodeFunc();
    void rndOpcodeFunc();
    void drwOpcodeFunc();
    void skpOpcodeFunc();
    void sknpOpcodeFunc();
    void ldRegDelayOpcodeFunc();
    void ldRegKeyOpcodeFunc();
    void ldDelayRegOpcodeFunc();
    void ldSoundRegOpcodeFunc();
    void addIRegOpcodeFunc();
    void ldFRegOpcodeFunc();
    void ldBRegOpcodeFunc();
    void ldMemRegOpcodeFunc();
    void ldRegMemOpcodeFunc();
    
    
    // functions for debugging purposes ... exposes internal structures.
    uint8_t dbgGetIReg() { return I; }
    
};

#endif /* Emulator_hpp */
