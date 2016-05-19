//
//  DebugUtils.h
//  Chippy
//
//  Created by Tyron Bonsu on 17/05/2016.
//
//

#ifndef DebugUtils_h
#define DebugUtils_h


#define debug 0 // toggle to enable debug print outs.

#define __NOT_IMPLEMENTED__ assert(true);
#define __NOT_IMPLEMENTED_CONTINUE__ return;

#if debug
#define DBG_OUT cinder::app::console() << std::setw(0) <<  std::setfill(' ')

#define DBG_PRINT(s) do { DBG_OUT << s << std::endl; } while (0)
#define DBG_PRINT_NO_NEWLINE(s) do { DBG_OUT << s; } while (0)
#define DBG_PRINT_FUNC do { DBG_OUT << "->" << __func__ << "()" << std::endl; } while (0)
#define DBG_PRINT_VAR(var) do { DBG_OUT << "\t" << std::setw(15) << #var << " \t= 0x" << std::setw(2) << std::setfill('0') << std::right << std::hex << (int)var << std::setfill(' ') << std::endl; } while (0)
#define DBG_PRINT_VAR_DEC(var) do { DBG_OUT << "\t" << std::setw(15) << #var << " \t= " << std::dec << (int)var << std::setfill(' ') << std::endl; } while (0)
#define DBG_PRINT_VAR_DOUBLE(var) do { DBG_OUT << "\t" << std::setw(15) << #var << " \t= " << std::dec << var << std::setfill(' ') << std::endl; } while (0)
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

#define DBG_PRINT_BINARY(b) do {\
                                        DBG_OUT << "\t" << #b <<": 0x" << std::setw(2) << std::setfill('0') << std::right << std::hex << (int)b \
                                        << "\t" << std::bitset<8>(b) << std::endl;\
                                    } while (0)

#else
#define DBG_OUT do { } while (0)
#define DBG_PRINT_FUNC
#define DBG_PRINT(s)
#define DBG_PRINT_NO_NEWLINE(s)
#define DBG_PRINT_VAR(var)
#define DBG_PRINT_VAR_DEC(var)
#define DBG_PRINT_VAR_DOUBLE(var)
#define DBG_PRINT_REG
#define DBG_PRINT_PIXEL_DATA(pixel)
#define DBG_PRINT_BINARY(b)
#endif

#endif /* DebugUtils_h */
