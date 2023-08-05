
#include "../sshCommon.h"
#include "stkCycles.h"
#include "zDisAsm.h"

#define ZDA(token) codes[zpos++] = token

static int offs(0), pref(0), dist(0), dest(0), source(0), ops(0), zpos(0);
static Z_MNEMONIC* zm(nullptr);
static zCpu* cpu(nullptr);
u16 zDisAsm::trAddr[2];
u16 zDisAsm::pc;

static u8 nmReg[] = {C_C, C_E, C_L, C_F, C_SP, C_B, C_D, C_H, C_A, C_I, C_R, 0,
                     C_F, 0, 0, C_ZERO,C_PBC, C_PDE, C_PHL, C_PNN, C_PSP, 0,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, C_BC, C_DE,
                     C_HL, C_AF, C_SP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                     // IX
                     C_C, C_E, C_XL, C_F, C_SP, C_B, C_D, C_XH, C_A, C_I,
                     C_R, 0, C_F, 0, 0, C_ZERO, C_PBC, C_PDE, C_PIX, C_PNN,
                     C_PSP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                     C_BC, C_DE, C_IX, C_AF, C_SP, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0,
                     // IY
                     C_C, C_E, C_YL, C_F, C_SP, C_B, C_D, C_YH, C_A, C_I,
                     C_R, 0, C_F, 0, 0, C_ZERO, C_PBC, C_PDE, C_PIY,
                     C_PNN, C_PSP, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                     0, 0, C_BC, C_DE, C_IY, C_AF, C_SP, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0, 0 };

int zDisAsm::operand(int dst, int src, int& value) {
    switch(dst) {
        case _C8: value = cpu->_rm8(pc++); break;
        case _C16: case _P16: value = cpu->rm16(pc); pc += 2; break;
        case _PDE: case _PBC: case _PHL:
            if(dist == 256) dist = (pref ? cpu->_rm8(pc++) : 0);
            // виртуальный адрес [HL/DE/BC/IX/IY + D]
            value = *(u16*)cpu->regs[(dst & 15) + pref] + (i8)dist;
    }
    // индекс имени регистра
    if(dst == src && src == _PHL) src = 0;
    return nmReg[dst + (src == _PHL ? 0 : (pref << 2))];
}

void zDisAsm::ocr() {
    offs = pref = 0;
    while(true) {
        ops = cpu->_rm8(pc++);
        zm = &z_cmd[ops + offs];
        if(zm->ops != O_PREF) break;
        offs = zm->offs << 8;
        if(pref) {
            if(offs == 256) /* DDCB/FDCB */ { dist = cpu->_rm8(pc++); continue; }
            /* *IX*|*IY* */
            zm = &z_cmd[767 + pref / 12]; pc--;
            break;
        }
        pref = zm->pref;
    }
}

u16 zDisAsm::parser(zCpu* _cpu, u16 PC, char* buffer, int flags) {
    auto result(buffer);
    memset(buffer, 0, 256);
    // инициализация переменных
    u16 codes[64];
    cpu = _cpu; pc = PC; dist = 256;
    ops = dest = source = zpos = 0;
    // [префикс] + инструкция
    ocr();
    // операнды инструкции
    int rd(zm->rd), rs(zm->rs), len(zm->len & 3);
    auto nd(operand(rd, rs, dest)), ns(operand(rs, rd, source));
    // подготовка
    auto name(zm->name); auto cond(zm->flags);
    auto hex(speccy->showHex); int idx(0), n, v;
    char* lex(nullptr); u16 pval(0);
    if(rs & 16) pval = _cpu->rm16(source);
    else if(rd & 16) pval = _cpu->rm16(dest);
    // формирователь
    ZDA(name);
    switch(name) {
        case C_BIT: case C_RES: case C_SET:
            ZDA(C_NUM); ZDA(((ops >> 3) & 7)); ZDA(C_COMMA);
        case C_RR: case C_RL: case C_RLC: case C_RRC:
        case C_SLA: case C_SLI: case C_SRA: case C_SRL:
            if(pref) {
                ZDA(C_CB_PHL); if(rd == _PHL) break;
                ZDA(C_COMMA);
            }
            ZDA(C_DST);
            break;
        case C_LDSP: case C_INC: 
        case C_DEC: case C_PUSH: ZDA(C_SRC); break;
        case C_EX_SP: pc -= 2;
        case C_POP: ZDA(C_DST); break;
        case C_ADD: case C_ADC: case C_SUB: case C_SBC:
        case C_AND: case C_OR: case C_XOR: case C_CP:
            if(rd == _RHL) { ZDA(C_DST); ZDA(C_COMMA); }
            if(rs == _C8) { ZDA(C_N); ZDA(source); } else ZDA(C_SRC);
            break;
        case C_RST: ZDA(C_NUM); ZDA(ops & 56); break;
        case C_IM:  ZDA(C_NUM); ZDA(cond); break;
        case C_RET: if(cond) ZDA(C_pc + cond); break;
        case C_IN:
            ZDA(C_DST); ZDA(C_COMMA);
            if(rs == _C8) { ZDA(C_N); ZDA(source); } else ZDA(C_PC);
            break;
        case C_OUT:
            if(rs == _C8) { ZDA(C_N); ZDA(source); } else ZDA(C_PC);
            ZDA(C_COMMA); rd == _ZERO ? ZDA(C_ZERO) : ZDA(C_DST);
            break;
        case C_DJNZ: case C_JR:
            source = pc + (i8)source;
        case C_JP: case C_CALL:
            if(ops == 233) ZDA(C_SRC);
            else {
                if(cond) { ZDA(C_pc + cond); ZDA(C_COMMA); }
                ZDA(C_NN); if(name == C_CALL) ZDA(dest); else ZDA(source);
            }
            break;
        case C_LD:
            switch(zm->ops) {
                case O_RA: case O_AR: case O_AI: case O_ASSGN:
                    ZDA(C_DST); ZDA(C_COMMA);
                    if(rs == _C8 || rs == _C16) { rs == _C8 ? ZDA(C_N) : ZDA(C_NN); ZDA(source); } else ZDA(C_SRC);
                    break;
                case O_LOAD:
                    ZDA(C_DST); ZDA(C_COMMA); 
                    if(rs == _P16) { ZDA(C_PNN); ZDA(source); } else ZDA(C_SRC);
                    break;
                case O_SAVE:
                    if(rd == _P16) { ZDA(C_PNN); ZDA(dest); } else ZDA(C_DST);
                    ZDA(C_COMMA);
                    if(rs == _C8) { ZDA(C_N); ZDA(source); } else ZDA(C_SRC);
                    break;
            }
            break;
    }
    // формирование текста инструкции
    // адрес + [метка ловушки]
    if(flags & DA_PC) {
        auto bps(cpu->_bp[PC]);
        auto ex(bps & ZX_BP_EXEC ? "*" : " ");
        auto rm(bps & ZX_BP_RMEM ? "*" : " ");
        auto wm(bps & ZX_BP_WMEM ? "*" : " ");
        z_strcpy(&result, z_fmtValue(PC, ZFV_OPS16, speccy->showHex));
        z_strcpy(&result, rm);
        z_strcpy(&result, ex);
        z_strcpy(&result, wm);
    }
    // байты инструкции
    if(flags & DA_CODE) {
        static char bufCode[32];
        auto daCode(bufCode);
        memset(bufCode, 0, 32);
        // длина кодов и они сами
        int length((u16)(pc - PC));
        for(int i = 0; i < length; i++) 
            z_strcpy(&daCode, z_fmtValue(_cpu->_rm8(PC + i), (i != (length - 1)) * 2, speccy->showHex));
        length = z_strlen(bufCode);
        n = 18 - hex * 5; z_char(&daCode, ' ', n - length); *daCode = 0;
        z_strcpy(&result, bufCode);
    }
    // запомнить позицию текста инструкции
    speccy->jni = (int)(result - buffer);
    // инструкция + дополнительная информация
    while(idx < zpos) {
        auto token(codes[idx++]);
        bool op(false), o(false);
        if(token >= C_N || token == C_PNN) {
            auto pnn(token == C_PNN); if(pnn) token = C_N + 3;
            static int fmtTypes[] = { ZFV_OPS8, ZFV_OPS16, ZFV_NUMBER, ZFV_PADDR16 };
            n = codes[idx++]; lex = z_fmtValue(n, fmtTypes[token - C_N], speccy->showHex);
            if(pnn) {
                if(flags & DA_PN) {
                    z_strcpy(&result, lex);
                    bool is16((rd | rs) & _R16);
                    if(!is16) pval = (u8)pval;
                    lex = z_fmtValue(pval, is16 ? ZFV_PVAL16 : ZFV_PVAL8, speccy->showHex);
                }
            }
        } else switch(token) {
            case C_CB_PHL: n = nmReg[_PHL + (pref << 2)]; v = source; op = o = true; break;
            case C_SRC: n = ns; v = source; op = true; o = pref && rs == _PHL; break;
            case C_DST: n = nd; v = dest; op = true; o = pref && rd == _PHL; break;
            default: lex = (char*)namesCode[token]; break;
        }
        if(op) {
            z_strcpy(&result, namesCode[n]);
            if(o) {
                int d((i8)dist);
                lex = z_fmtValue(d, ZFV_OFFS, speccy->showHex);
                if(flags & DA_PNN) {
                    z_strcpy(&result, lex);
                    lex = z_fmtValue((u16)v, ZFV_CVAL, speccy->showHex);
                }
            } else lex = nullptr;
            if((token == C_DST && (rd & 16)) || (token == C_SRC && (rs & 16)) || token == C_CB_PHL) {
                if(flags & DA_PN) {
                    z_strcpy(&result, lex);
                    lex = z_fmtValue((u8)pval, ZFV_PVAL8, speccy->showHex);
                }
            }
        }
        z_strcpy(&result, lex);
    }
    return pc;
}

static int skipPrefix(int entry) {
    offs = pref = 0;
    while(true) {
        ops = cpu->_rm8(entry);
        zm = &z_cmd[ops + offs];
        if(zm->ops != O_PREF) break;
        entry++; offs = zm->offs << 8;
        if(offs == 256 && pref) entry++; else pref = zm->pref;
    }
    if(pref && offs == 256) entry--;
    return entry;
}

static int cmdLength(int address) {
    auto entry(skipPrefix(address));
    // displacement
    entry += (uint8_t)(pref && (zm->rs == _PHL || zm->rd == _PHL));
    // length
    entry += zm->len & 3;
    return entry;
}

static int wndLength(int address, int finish, int count, int* length) {
    *length = 0;
    auto buf((u16*)&tmpBuf[0]);
    while(count-- > 0 && address < finish) {
        auto len((u16)(cmdLength(address) - address));
        *buf++ = len; *length += len; address += len;
    }
    return count + 1;
}

int zDisAsm::movePC(int entry, int delta, int count) {
    int length(0);
    if(delta > 0) {
        auto buf((u16*)&tmpBuf[0]);
        int wnd(delta + count);
        auto size(wndLength(entry, 65536, wnd, &length));
        // отмотать назад на count
        for(int i = 1 ; i <= count; i++) {
            auto ll(buf[wnd - i - size]);
            length -= ll;
        }
        entry += length;
    } else if(delta < 0) {
        entry += delta;
        if(entry < 0) entry = 0;
    }
    return entry;
}

int zDisAsm::jump(zCpu* cpu, u16 entry, int mode, bool isCall) {
    u16 jmp(0), next(0);
    int addr(entry);
    switch(mode) {
        case ZX_DEBUGGER_MODE_PC:
            // взять инструкцию по адресу
            entry = skipPrefix(addr) + 1;
            switch(zm->ops) {
                case O_SAVE: case O_LOAD:
                    if(zm->rd == _C16 || zm->rs == _C16) {
                        jmp = cpu->rm16(entry); next = (u16)(entry + 2);
                        mode = ZX_DEBUGGER_MODE_DT;
                    } else mode = -1;
                    break;
                case O_RET: case O_RETX:
                    jmp = cpu->rm16(cpu->sp); next = entry;
                    break;
                case O_RST:
                    if(!isCall) { mode = -1; break; }
                    jmp = (ops & 56); next = entry;
                    break;
                case O_CALL: case O_CALLX:
                    if(!isCall) { mode = -1; break; }
                case O_JMP: case O_JMPX:
                case O_DJNZ:
                    if(ops == 233) {
                        if(pref == 12) jmp = cpu->ix;
                        else if(pref == 24) jmp = cpu->iy;
                        else jmp = cpu->hl;
                        next = entry;
                        break;
                    } else if(ops != 16) {
                        jmp = cpu->rm16(entry); next = (u16)(entry + 2);
                        break;
                    }
                case O_JR: case O_JRX:
                    ops = cpu->_rm8(entry++);
                    jmp = entry + (i8)ops; next = entry;
                    break;
                default: mode = -1; break;
            }
            break;
        case ZX_DEBUGGER_MODE_SP:
            jmp = cpu->rm16(entry);
            mode = ZX_DEBUGGER_MODE_PC;
            break;
        case ZX_DEBUGGER_MODE_DT:
            mode = -1;
            break;
    }
    speccy->jni = ((jmp << 16) | next);
    return mode;
}

void zDisAsm::trace(zCpu* cpu, int mode) {
    if(mode == ZX_CMD_TRACE_OVER) trAddr[0] = trAddr[1] = cpu->_call;
    else {
        if(jump(cpu, cpu->pc, ZX_DEBUGGER_MODE_PC, mode == ZX_CMD_TRACE_IN) == ZX_DEBUGGER_MODE_PC) {
            trAddr[0] = (speccy->jni & 0xffff); trAddr[1] = ((speccy->jni >> 16) & 0xffff);
        } else {
            trAddr[0] = trAddr[1] = (u16)cmdLength(cpu->pc);
        }
    }
    cpu->_bp[trAddr[0]] |= ZX_BP_TRACE;
    cpu->_bp[trAddr[1]] |= ZX_BP_TRACE;
//    speccy->debugLaunch = false;
    modifySTATE(ZX_TRACE, 0);
}
