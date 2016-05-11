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
    
    /*enum State { waitForKey, running };
    State state = running; */
    
    uint8_t display[displayHeight][displayWidth];
    bool drawDisplay = false;
    bool waitForKey = false;
    
    int statInstructionCount = 0;
    
    bool loadBinary(const std::string&);
    void cpuCycle();
    void setKeyPressed(uint8_t);
    void setKeyReleased(uint8_t);
    
    
private:
    uint8_t memory[0x1000];
    uint8_t *font = memory;
    uint8_t vReg[16], keys[16];
    uint8_t delayTimer, soundTimer, sp;
    uint16_t pc, I;
    uint16_t stack[16];
    
    
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
    
    
};
/* setup opcode func table
    opcodeFuncTable = { &Emulator::opcodeZeroDispatch,   // 00E0, 00EE, 0nnn
                        &Emulator::jpOpcodeFunc,         // 1nnn
                        &Emulator::callOpcodeFunc,       // 2nnn
                        &Emulator::seByteOpcodeFunc,     // 3xkk
                        &Emulator::sneByteOpcodeFunc,    // 4xkk
                        &Emulator::seRegOpcodeFunc,      // 5xy0
                        &Emulator::ldRegByteOpcodeFunc,  // 6xkk
                        &Emulator::addRegByteOpcodeFunc, // 7xkk
                        &Emulator::opcodeEightDispatch,  // 8xy{0..7},E
                        &Emulator::orOpcodeFunc,         // 8xy1
                        &Emulator::andOpcodeFunc,        // 8xy2
                        &Emulator::xorOpcodeFunc,        // 8xy3
                        &Emulator::addRegRegOpcodeFunc,  // 8xy4
                        &Emulator::subOpcodeFunc,        // 8xy5
                        &Emulator::shrOpcodeFunc,        // 8xy6
                        &Emulator::subnOpcodeFunc,       // 8xy7
                        &Emulator::shlOpcodeFunc,        // 8xyE
                        &Emulator::sneRegRegOpcodeFunc,  // 9xy0
                        &Emulator::ldIOpcodeFunc,        // Annn
                        &Emulator::jpV0OpcodeFunc,       // Bnnn
                        &Emulator::rndOpcodeFunc,        // Cxkk
                        &Emulator::drwOpcodeFunc,        // Dxyn
                        &Emulator::opcodeEDispatch,
                        &Emulator::opcodeFDispatch };
                        &Emulator::skpOpcodeFunc,        // Ex9E
                        &Emulator::sknpOpcodeFunc,       // ExA1
                        &Emulator::ldDelayRegOpcodeFunc, // Fx07
                        &Emulator::ldRegKeyOpcodeFunc,   // Fx0A
                        &Emulator::ldDelayRegOpcodeFunc, // Fx15
                        &Emulator::ldSoundRegOpcodeFunc, // Fx18
                        &Emulator::addIRegOpcodeFunc,    // Fx1E
                        &Emulator::ldFRegOpcodeFunc,     // Fx29
                        &Emulator::ldBRegOpcodeFunc,     // Fx33
                        &Emulator::ldMemRegOpcodeFunc,   // Fx55
                        &Emulator::ldRegMemOpcodeFunc }; // Fx65
     */
/*
 00E0 - CLS                     // clear the display
 00EE - RET                     // set pc to addr at top of stack; sp--;
 0nnn - SYS addr                // ignore
 1nnn - JP addr                 // pc = nnn
 2nnn - CALL addr               // call sub | sp++; top of sp = pc; pc = nnn
 3xkk - SE Vx, byte             // Skip next instruction if Vx = kk.
 4xkk - SNE Vx, byte            // Skip next instruction if Vx != kk.
 5xy0 - SE Vx, Vy               // Skip next instruction if Vx = Vy.
 6xkk - LD Vx, byte             // Set Vx = kk
 7xkk - ADD Vx, byte            // Set Vx = Vx + kk.
 8xy0 - LD Vx, Vy               // Set Vx = Vy
 8xy1 - OR Vx, Vy               // Set Vx = Vx OR Vy.
 8xy2 - AND Vx, Vy              // Set Vx = Vx AND Vy.
 8xy3 - XOR Vx, Vy              // Set Vx = Vx XOR Vy.
 8xy4 - ADD Vx, Vy              // Set Vx = Vx + Vy, set VF = carry.
 8xy5 - SUB Vx, Vy              // Set Vx = Vx - Vy, set VF = NOT borrow. ...
 8xy6 - SHR Vx {, Vy}           // Set Vx = Vx SHR 1
 8xy7 - SUBN Vx, Vy             // Set Vx = Vy - Vx, set VF = NOT borrow. ...
 8xyE - SHL Vx {, Vy}           // Set Vx = Vx SHL 1. ...
 9xy0 - SNE Vx, Vy              // Skip next instruction if Vx != Vy.
 Annn - LD I, addr              // Set I = nnn.
 Bnnn - JP V0, addr             // Jump to location nnn + V0.
 Cxkk - RND Vx, byte            // Set Vx = random byte AND kk. *...
 Dxyn - DRW Vx, Vy, nibble      // Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision. ...
 Ex9E - SKP Vx                  // Skip next instruction if key with the value of Vx is pressed.
 ExA1 - SKNP Vx                 // Skip next instruction if key with the value of Vx is not pressed.
 Fx07 - LD Vx, DT               // Set Vx = delay timer value. Wait for a key press, store the value of the key in Vx.
 Fx0A - LD Vx, K                // Wait for a key press, store the value of the key in Vx.
 Fx15 - LD DT, Vx               // Set delay timer = Vx.
 Fx18 - LD ST, Vx               // Set sound timer = Vx.
 Fx1E - ADD I, Vx               // Set I = I + Vx.
 Fx29 - LD F, Vx                // Set I = location of sprite for digit Vx. ...
 Fx33 - LD B, Vx                // Store BCD representation of Vx in memory locations I, I+1, and I+2.
 Fx55 - LD [I], Vx              // Store registers V0 through Vx in memory starting at location I.
 Fx65 - LD Vx, [I]              // Read registers V0 through Vx from memory starting at location I.
 */


#endif /* Emulator_hpp */
