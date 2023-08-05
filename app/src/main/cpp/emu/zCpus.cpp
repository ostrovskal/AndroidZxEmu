
#include "../sshCommon.h"
#include "stkCycles.h"
#include "zCpus.h"
//#include "zDAsm.h"

static u8 flags_vals[9] = { 0, FZ, FZ, FC, FC, FPV, FPV, FS, FS };
static u8 flags_cond[9] = { 0, 0, 64, 0, 1, 0, 4, 0, 128 };
static int snaPages[]   = { 0, 1, 3, 4, 6, 7 };

static void packPage(u8** buffer, u8* src, u8 page) {
    u32 size; auto buf(*buffer);
    *buffer     = z_packBlock(src, src + 16384, &buf[3], false, size);
    *(u16*)buf  = (u16)size;
    buf[2]      = page;
}

static void makeAdc() {
    for(int c = 0; c < 2; c++) {
        for(int x = 0; x < 256; x++) {
            for(int y = 0; y < 256; y++) {
                u32 res(x + y + c); u8 fl(0);
                if(!(res & 255)) fl |= FZ;
                fl |= (res & (F3 | F5 | FS));
                if(res >= 256) fl |= FC;
                if(((x & 15) + (y & 15) + c) & 16) fl |= FH;
                int ri((int8_t)x + (int8_t)y + c);
                if(ri >= 128 || ri < -128) fl |= FPV;
                adcf[c * 0x10000 + x * 256 + y] = fl;
            }
        }
    }
}

static void makeSbc() {
    for(int c = 0; c < 2; c++) {
        for(int x = 0; x < 256; x++) {
            for(int y = 0; y < 256; y++) {
                int res(x - y - c); u8 fl(res & (F3 | F5 | FS));
                if(!(res & 255)) fl |= FZ;
                if(res & 0x10000) fl |= FC;
                int r((int8_t)x - (int8_t)y - c);
                if(r >= 128 || r < -128) fl |= FPV;
                if(((x & 15) - (res & 15) - c) & 16) fl |= FH;
                sbcf[c * 0x10000 + x * 256 + y] = fl | FN;
            }
        }
    }
    for(int i = 0; i < 0x10000; i++) {
        cpf[i] = (sbcf[i] & ~(F3 | F5)) | (i & (F3 | F5));
        u8 tempbyte((i >> 8) - (i & 255) - ((sbcf[i] & FH) >> 4));
        cpf8b[i] = (sbcf[i] & ~(F3 | F5 | FPV | FC)) + (tempbyte & F3) + ((tempbyte << 4) & F5);
    }
}

zCpu::zCpu() : CPU() {
    static u8 regsTbl[] = { RC, RE, RL, RF, RSPL, RB, RD, RH, RA, RI, RR1 };
    makeAdc(); makeSbc(); memset(regs, 0, sizeof(regs));
    for(int i = 0 ; i < 11; i++) { auto reg((u8*)&c + regsTbl[i]); regs[i] = reg; regs[i + 12] = reg; regs[i + 24] = reg; }
    regs[14] = &xl; regs[19] = &xh; regs[26] = &yl; regs[31] = &yh;
    memset(&c, 0, sizeof(CPU));
    _bp = new u8[65536]; memset(_bp, 0, 65536);
    mem = speccy->dev<zDevMem>();
}

u8 zCpu::rm8(u16 address) {
    auto val(*ptr(address));
    if(speccy->debugLaunch && (_bp[address] & ZX_BP_RMEM)) {
        if(checkBPs(address, ZX_BP_RMEM, val)) return 0;
    }
    return val;
}

void zCpu::wm8(u16 address, u8 val) {
    if(speccy->debugLaunch && (_bp[address] & ZX_BP_WMEM)) { 
        if(checkBPs(address, ZX_BP_WMEM, val)) return;
    }
    wb[address >> 14][address & 16383] = val;
}

int zCpu::_halt() {
    // перемотать до конца фрейма
    auto st((frameTS - frame) / 4 + 1);
    rl += st; frame += st * 4;
    return 0;
}

int zCpu::irqInt() {
    halt = iff[0] = iff[1] = 0;
    switch(im) {
        case 0:
        case 1: call(0x38); return 13;
        case 2: call(rm16(im2())); return 19;
        default: ILOG("IM %i - invalid!", im); break;
    }
    return 0;
}

int zCpu::irqNmi() {
    halt = iff[0] = 0; reqNMI = false;
    speccy->flags |= ZX_TRDOS;
    speccy->dev<zDevMem>()->update(ZX_UPDATE_STATE);
    call(0x66);
    return 11;
}

void zCpu::call(u16 address) {
    wm8(--sp, pch); wm8(--sp, pcl);
    rl++; _call = pc; pc = address;
}

int zCpu::inOu(u8 t) {
    auto r(--b);
    f = (aluf[r] & ~FPV) | (aluf[(t & 7) ^ r] & FPV);
    f |= ((t < m8) << 4) | ((m8 >> 6) & FN) | (t < m8);
    return r;
}

void zCpu::flagIR() {
    f = (f & FC) | (aluf[a] & ~FPV);
    if(frameTS < 1000 || ((frame + 7) < frameTS)) f |= (iff[1] << 2);
}

void zCpu::iop() {
    u8 t8;
    u32 n32;
    auto s8(*src), fc((u8)(f & FC));
    u16 d16, s16;
    int dir, fpv, t;
    switch(zm->ops) {
        case O_IM:  im = zm->flags; break;
        case O_XI:  ei = iff[0] = iff[1] = (u8)((ops & 8) >> 3); break;
        case O_IN:  *dst = m8; if(zm->flags) f = fc | aluf[m8]; break;
        case O_RA:  rl = a; rh = a & 128; break;
        // SZ503*0-
        case O_AR:  a = (rl & 127) | rh; flagIR(); break;
        case O_AI:  a = i; flagIR(); break;
        // SZ*H*V1C
        case O_CP:   f = cpf[a * 256 + s8]; break;
        // SZ503P00
        case O_OR:   f = aluf[a |= s8]; break;
        // SZ503P00
        case O_XOR:  f = aluf[a ^= s8]; break;
        // SZ513P00
        case O_AND:  f = aluf[a &= s8] | FH; break;
        case O_ADD8: f = adcf[a + s8 * 256]; a += s8; break;
        case O_ADC8: f = adcf[a + s8 * 256 + fc * 65536]; a += s8 + fc; break;
        case O_SUB8: f = sbcf[a * 256 + s8]; a -= s8; break;
        case O_SBC8: f = sbcf[a * 256 + s8 + fc * 65536]; a -= s8 + fc; break;
        case O_NEG:  f = sbcf[a]; a = -a; break;
        case O_DEC8: f = fc | decf[s8--]; *src = s8; break;
        case O_INC8: f = fc | incf[s8++]; *src = s8; break;
        case O_DEC:  *(u16*)src -= 1; break;
        case O_INC:  *(u16*)src += 1; break;
        case O_DECP: f = fc | decf[m8--]; break;
        case O_INCP: f = fc | incf[m8++]; break;
        case O_DAA: af = daatab[a + 256 * ((f & 3) + ((f >> 2) & 4))]; break;
        case O_SET: m8 |= (1 << ((ops >> 3) & 7)); break;
        case O_RES: m8 &= ~(1 << ((ops >> 3) & 7)); break;
        case O_RETN: iff[0] = iff[1]; break;
        case O_DJNZ: if(--b) { pc += dist; ticks += 5; } break;
        case O_RST: _call = pc; pc = ops & 56; break;
        case O_JMPX: if((f & flags_vals[zm->flags]) != flags_cond[zm->flags]) break;
        case O_JMP: pc = *(u16*)src; break;
        case O_JRX: if((f & flags_vals[zm->flags]) != flags_cond[zm->flags]) { ticks -= 5; break; }
        case O_JR: pc += dist; tmp = (u8)(pc >> 8); break;
        case O_CALLX: if((f & flags_vals[zm->flags]) != flags_cond[zm->flags]) { step = 0; break; }
        case O_CALL: if(step != 5) _call = pc, pc = *(u16*)op; break;
        case O_RETX: if((f & flags_vals[zm->flags]) != flags_cond[zm->flags]) step = 0; break;
        case O_LDSP: sp = *(u16*)src; break;
        case O_EXA: SWAP_REG(af, af_); break;
        case O_EXHD: SWAP_REG(de, hl); break;
        case O_EXSP: *(u16*)src = *(u16*)dst; break;
        case O_EXX: SWAP_REG(bc, bc_); SWAP_REG(de, de_); SWAP_REG(hl, hl_); break;
        case O_HLT: halt = iff[0] = iff[1] = 1; break;
        // SZ503P0-
        case O_RLD: t8 = m8; m8 = (a & 15) | (m8 << 4); a = (a & 240) | (t8 >> 4); f = fc | aluf[a]; break;
        case O_RRD: t8 = m8; m8 = (a << 4) | (m8 >> 4); a = (a & 240) | (t8 & 15); f = fc | aluf[a]; break;
        // --*1*-1-
        case O_CPL:  a ^= 0xFF; f = (f & (FS | FZ | FPV | FC)) | FH | FN | (a & (F3 | F5)); break;
        // --***-0C
        case O_CCF:  f = (f & (FS | FZ | FPV)) | (fc << 4) | (a & (F3 | F5)) | (fc == 0); break;
        // --*0*-01
        case O_SCF:  f = (f & (FS | FZ | FPV)) | (a & (F3 | F5)) | FC; break;
        case O_RLC:  f = rlcf[m8]; m8 = rol[m8]; break;
        case O_RRC:  f = rrcf[m8]; m8 = ror[m8]; break;
        case O_RL:   f = _rl[m8 + fc * 256]; m8 = (m8 << 1) | fc; break;
        case O_RR:   f = _rr[m8 + fc * 256]; m8 = (m8 >> 1) | (fc << 7); break;
        case O_SLA:  f = _rl[m8];  m8 = (m8 << 1); break;
        case O_SRA:  f = sraf[m8]; m8 = (m8 >> 1) + (m8 & 0x80); break;
        case O_SLL:  f = _rl[m8 + 256]; m8 = (m8 << 1) | 1; break;
        case O_SRL:  f = _rr[m8], m8 = (m8 >> 1); break;
        case O_RLCA: f = rlcaf[a] | (f & (FS | FZ | FPV)); a = rol[a]; break;
        case O_RRCA: f = rrcaf[a] | (f & (FS | FZ | FPV)); a = ror[a]; break;
        case O_RLA:  f = rlcaf[a] | (f & (FS | FZ | FPV)); a = (a << 1) | fc; break;
        case O_RRA:  f = rrcaf[a] | (f & (FS | FZ | FPV)); a = (a >> 1) | (fc << 7); break;
        case O_UNDEF: DLOG("found NONI(%i) from PC: %i", zm->name, pcb); break;
        case O_BIT:
            // из старшего байта IX/IY + D/ из внутреннего регистра/из операции
            if(pref) t8 = _dest >> 8; else if(zm->rd == _PHL) t8 = tmp; else t8 = m8;
            f = fc | (aluf[m8 & (1 << ((ops >> 3) & 7))] & ~(F3 | F5)) | FH | (t8 & (F3 | F5));
            step = 0;
            break;
        case O_ADC:
            d16         = *(u16*)dst; s16 = *(u16*)src;
            *(u16*)dst  = (n32 = (d16 + s16 + fc));
            t8          = ((((d16 & 0x0FFF) + (s16 & 0x0FFF) + fc) >> 8) & FH) | (((u16)n32 == 0) << 6) | (n32 > 65535);
            fpv         = ((int)(short)d16 + (int)(short)s16 + (int)fc);
//            if(fpv < -0x8000 || fpv >= 0x8000) t8 |= FPV;
            t8          |= FPV * (fpv < -0x8000 || fpv >= 0x8000);
            f           = t8 | ((n32 >> 8) & (F3 | F5 | FS));
            break;
        case O_SBC:
            d16         = *(u16*)dst; s16 = *(u16*)src;
            *(u16*)dst  = (n32 = (d16 - s16 - fc));
            t8          = ((((d16 & 0x0FFF) - (s16 & 0x0FFF) - fc) >> 8) & FH) | FN | (((u16)n32 == 0) << 6) | (n32 > 65535);
            fpv         = ((int)(short)d16 - (int)(short)s16 - (int)fc);
            t8          |= FPV * (fpv < -0x8000 || fpv >= 0x8000);
            f           = t8 | ((n32 >> 8) & (F3 | F5 | FS));
            break;
        case O_ADD:
            tmp         = h;
            d16         = *(u16*)dst, s16 = *(u16*)src;
            *(u16*)dst  = (u16)(n32 = (d16 + s16));
            f           &= (FS | FZ | FPV);
            f           |= ((((d16 & 0x0FFF) + (s16 & 0x0FFF)) >> 8)& FH) | (n32 > 65535) | ((n32 >> 8) & (F3 | F5));
            break;
        case O_LDI: case O_CPI:
        case O_INI: case O_OTI:
            dir = (((ops & 8) >> 2) - 1); hl -= dir;
            switch(zm->ops) {
                // --*0**0-
                case O_LDI:
                    t8 = m8 + a; fpv = (--bc != 0); de -= dir;
                    f = (f & (FS | FZ | FC)) | (t8 & F3) | ((t8 << 4) & F5) | (fpv << 2);
                    break;
                // SZ*H**1-
                case O_CPI: 
                    f = fc | cpf8b[a * 256 + m8] | ((--bc != 0) << 2);
                    fpv = !(f & FZ) && bc;
                    break;
                // SZ5*3***
                case O_INI: fpv = inOu(m8 + c + dir); break;
                // SZ5*3***
                case O_OTI: fpv = inOu(m8 + l); break;
            }
            t = (fpv && ((ops & 16) >> 4)); pc -= t << 1; ticks += t + (t << 2);
            break;
    }
}

void zCpu::ocr(bool main) {
    auto pr(pref && offs == 256);
    ops = rm8(pc++); zm = &z_cmd[ops + offs];
    if(!pr) {
        if(main) frame += (frame & zSpeccy::machine->evenM1);
        rl++;
    }
    if(zm->ops == O_PREF) {
        offs = zm->offs << 8;
        pr = pref && offs == 256;
        if(pr) zm = &z_cmd[771]; else pref = zm->pref;
        ticks = 4; step--; return;
    }
    rw();
    // индекс текущего цикла, в зависимости от наличия CBR
    step += (pref != 0) * (zm->len >> 2) + pr;
}

void zCpu::finishCmd() {
    while(step) {
        incFrame(speccy->turboDiv(exec()));
        if(ula) ula->updateRay((int)frame);
    }
}

// выполнение при трассировке
void zCpu::debug() {
    // убрать отладчик - чтобы не сработала точка останова
    speccy->debugLaunch = false;
    incFrame(speccy->turboDiv(exec()));
    finishCmd();
    speccy->debugLaunch = true;
}

u8* zCpu::getOperand(u8 o, u8 oo, u16* address) {
    switch(o) {
        // 0 в операнде
        case _ZERO: op[0] = 0;
        // адрес из константы
        case _P16: *address = *(u16*)op;
        // без операнда
        case _N_:// return nullptr;
        // константа 8/16 бит
        case _C8: case _C16: return op;
        // PC
        case _RPC: return (u8*)&pc;
        // адрес [HL/BC/DE/IX/IY + D]
        case _PHL: case _PBC: case _PDE: 
            *address = *(u16*)regs[(o & 15) + pref] + dist; 
            return ptr(*address); 
    }
    // адрес регистра
    return regs[(o & 15) + ((oo != _PHL) * pref)];
}

void zCpu::update(int param) {
    switch(param) {
        case ZX_UPDATE_RESET:
            clear();
        case ZX_UPDATE_PRIVATE:
            offs = pref = io = 0;
            dist = 0; step = 0; ei = 0;
            dl = true; zm = &z_cmd[770];
            break;
        case ZX_UPDATE_MODEL:
        case ZX_UPDATE_STATE:
            halt = 0;
            pages = zSpeccy::machine->delayPages;
            frameTS = zSpeccy::machine->tsTotal;
            modifySTATE(ZX_CAPT, 0)
            break;
    }
}

void zCpu::rw() {
    dst = getOperand(zm->rd, zm->rs, &_dest);
    src = getOperand(zm->rs, zm->rd, &_source);
}

void zCpu::setBreak() {
    // сбросить текущую инструкцию
    update(ZX_UPDATE_PRIVATE);
    // вернуть адрес на начало инструкции
    pc = pcb;
    // установить глобальную остановку системы
    speccy->execLaunch  = false;
    // сообщение для выхода - активировать отладчик
    speccy->flags |= ZX_BP;
    // остановить звук
    speccy->dev<zDevMixer>()->update(ZX_UPDATE_STATE);
}

u8* zCpu::speedSave() {
    auto buf(&tmpBuf[0]);
    *buf++ = a;
    for(int i = 0; i < de; i++) { auto b(rm8(ix++)); *buf++ = b; a ^= b; }
    *buf++ = a;
    // выход
    pc = rm16(sp); sp += 2;
    return buf;
}

void zCpu::quickBP(int address) {
    BREAK_POINT* bpe(nullptr);
    for(auto& bp : bps) {
        if(bp.flg & ZX_BP_DISABLE || bp.flg == ZX_BP_NONE) bpe = &bp;
        else if(address >= bp.address1 && address <= bp.address2 && bp.flg == ZX_BP_EXEC) {
            bp.flg ^= ZX_BP_DISABLE; bpe = nullptr;
            break;
        }
    }
    if(bpe) {
        bpe->address1 = address; bpe->address2 = address;
        bpe->val = 0; bpe->msk = 0; bpe->ops = 0; bpe->flg = ZX_BP_EXEC;
    }
    filledBPS();
}

void zCpu::filledBPS() {
    memset(_bp, 0, 65536);
    for(auto& bp : bps) {
        auto flg(bp.flg);
        if(flg & ZX_BP_DISABLE) continue;
        auto addr1(bp.address1), addr2(bp.address2);
        while(addr1 <= addr2) _bp[addr1++] |= flg;
    }
}

int zCpu::checkBPs(int address, u8 flg, u8 val) {
    auto res(false);
    if(_bp[address] & ZX_BP_TRACE) {
        _bp[address] &= ~ZX_BP_TRACE;
//        _bp[zDAsm::trAddr[0]] &= ~ZX_BP_TRACE;
//        _bp[zDAsm::trAddr[1]] &= ~ZX_BP_TRACE;
        res = true;
    } else if((_bp[address] & ZX_BP_EXEC) == flg) {
        res = true;
    } else {
        for(auto& bp : bps) {
            if(bp.flg != flg) continue;
            if(address >= bp.address1 && address <= bp.address2) {
                if(bp.flg >= ZX_BP_RMEM) {
                    auto v1(val & bp.msk); auto v2(bp.val);
                    switch(bp.ops) {
                        case ZX_BP_OPS_EQ:  res = v1 == v2; break;
                        case ZX_BP_OPS_NQ:  res = v1 != v2; break;
                        case ZX_BP_OPS_GT:  res = v1 >  v2; break;
                        case ZX_BP_OPS_LS:  res = v1 <  v2; break;
                        case ZX_BP_OPS_GTE: res = v1 >= v2; break;
                        case ZX_BP_OPS_LSE: res = v1 <= v2; break;
                        default:            res = false;    break;
                    }
                }
                break;
            }
        }
    }
    if(res) setBreak();
    return res;
}

zCpuMain::zCpuMain() {
    nb[0] = 8; nb[1] = 5; nb[2] = 2;
    rb[1] = wb[1] = mem->page(5);
    rb[2] = wb[2] = mem->page(2);
    // адреса источников для проверки цикла на задержку
    chkCycl[0] = &pc; chkCycl[1] = &pc; chkCycl[2] = &pc; chkCycl[3] = &pc;
    chkCycl[4] = &sp; chkCycl[5] = &sp; chkCycl[6] = &sp; chkCycl[7] = &sp;
    chkCycl[8]  = &_source; chkCycl[9] = &_source; chkCycl[12] = &_source;
    chkCycl[10] = &_dest; chkCycl[11] = &_dest;
    ula = speccy->dev<zDevUla>();
}

bool zCpuMain::operationIO() {
    auto high(*(u8*)((u8*)&c + ((size_t)(offs == 0) * RF + RB)));
    auto low(*(u8*)((u8*)&c + ((size_t)(offs == 0) * ROP1)));
    if(delayIO(low, high)) {
        auto port((u16)(low | (high << 8)));
        auto rp(zm->name & 1);
        if(rp) speccy->writePort(port, m8 = *dst, frame); else m8 = speccy->readPort(port, frame);
        rp = (rp + 1) << 3;
        if(speccy->debugLaunch && (_bp[port] & rp)) {
            if(checkBPs(port, rp, m8)) return false;
        }
        return true;
    }
    return false;
}

u8* zCpuMain::state(u8* ptr, bool restore) {
    auto buf(ptr);
    if(restore) {
        if(strncmp((char*)buf, "SERG CPU_Z80A", 13) != 0) return nullptr;
        buf += 13; memcpy((u8*)&c, buf, COUNT_CPU);
        buf += COUNT_CPU;
        if(z_crc(ptr, buf - ptr) != wordLE(&buf)) return nullptr;
        filledBPS();
    } else {
        z_memcpy(&buf, "SERG CPU_Z80A", 13);
        z_memcpy(&buf, &c, COUNT_CPU);
        wordLE(&buf, z_crc(ptr, buf - ptr));
    }
    return buf;
}

int zCpuMain::exec() {
    if(step == 0) {
        if(halt) return _halt();
        pcb = pc;
        // проверить на точку останова для кода
        if(speccy->debugLaunch && (_bp[pc] & (ZX_BP_EXEC | ZX_BP_TRACE))) 
            return !checkBPs(pc, ZX_BP_EXEC);
        zCpu::update(ZX_UPDATE_PRIVATE); tacts = 0; step++;
    }
    do {
        auto tmp((int)z_cycles[zm->cycles + step]), cycl = (tmp & 0b11111000);
        if((cycl <= PRT) || delayOps(*chkCycl[(cycl - OPH) >> 3])) {
//          if(cycl > PRT ? delayOps(*chkCycl[(cycl - OPH) >> 3]) : true) {
            ticks = tmp & 7; step++;
            auto high((cycl & 8) >> 3);
            switch(cycl) {
                case CPU_CYCLES::SRL: case CPU_CYCLES::SRH: dst[high] = rm8(sp++); break;
                case CPU_CYCLES::SWL: wm8(sp--, m8); wm8(sp, *src); break;
                case CPU_CYCLES::SWH: sp--; m8 = src[high]; break;
//                case CPU_CYCLES::SWL: case CPU_CYCLES::SWH: wm8(--sp, src[high]); break;
                case CPU_CYCLES::SDL: case CPU_CYCLES::SDH: dst[high] = src[high]; break;
                case CPU_CYCLES::OPL: case CPU_CYCLES::OPH: op[high] = rm8(pc++); if(high) rw(); break;
                case CPU_CYCLES::XWR: *dst = m8; wm8(_source, m8); break;
                case CPU_CYCLES::MWR: wm8(_dest, m8); break;
                case CPU_CYCLES::PWR: wm8(_dest++, *src++); break;
                case CPU_CYCLES::PRD: *dst++ = rm8(_source++); break;
                case CPU_CYCLES::MRD: m8 = rm8(_source); break;
                case CPU_CYCLES::RWR: *dst = m8; break;
                case CPU_CYCLES::XRD: m8 = *dst; break;
                case CPU_CYCLES::RRD: m8 = *src; break;
                case CPU_CYCLES::DRD: dist = (i8)rm8(pc++); rw(); break;
                case CPU_CYCLES::PRT: if(!operationIO()) return ticks; break;
                case CPU_CYCLES::IOP: iop(); break;
                case CPU_CYCLES::OCR: ocr(true); break;
            }
            if(!step || z_cycles[zm->cycles + step] == CPU_CYCLES::FIN) { step = 0; break; }
        }
    } while(!ticks);
    return ticks;
}

void zCpuMain::update(int param) {
    zCpu::update(param);
    if(param == ZX_UPDATE_FRAME) {
        // если второй процессор в режиме трассировки - выход
        if(checkSTATE(ZX_TRACE) && speccy->debugCpu == 0) return;
        // выполнение инструкций
        while(frame < frameTS) {
            if(step == 0) {
                if(speccy->rom == PAGE_SOS48 && reqNMI) {
                    // вызов NMI
                    incFrame(speccy->turboDiv(irqNmi()));
                }
                if(checkSTATE(ZX_BP)) return;
                if(frame < zSpeccy::machine->tsInt) {
                    // вызов прерывания
                    if(iff[0] && !ei) incFrame(speccy->turboDiv(irqInt()));
                }
                trap();
            }
            incFrame(speccy->turboDiv(exec()));
            ula->updateRay(frame);
        }
        finishCmd();
        ts += frame;
        frame -= frameTS;
    }
}

void zCpuMain::trap() {
    auto trdos((bool)(checkSTATE(ZX_TRDOS))), _trdos(false);
    if(pch < 64) {
        switch(speccy->rom) {
            case PAGE_SOS48:
                // активность TRDOS
                if(!(_trdos = trdos | (pch == 0x3D))) {
                    // проверить на сброс
                    if(pc >= 4597 && pc < 4600) speccy->programName("BASIC", false);
                    // LOAD/SAVE
                    else if((pc == 0x4C2 || pc == 0x556)) {
//                        speccy->save(speccy->is48k() ? "c:\\tap48.zzz" : "c:\\tap128.zzz", 128);
                        speccy->dev<zDevTape>()->trap(pc == 0x556);
                    }
                } else {
//                    speccy->save(speccy->is48k() ? "c:\\tr48.zzz" : "c:\\tr128.zzz", 128);
                    speccy->dev<zDevVG93>()->trap(pc);
                }
                break;
            case PAGE_SOS128:
                // проверить на сброс
                if(pc == 311) speccy->programName("BASIC", false);
                break;
            case PAGE_SYS:
                break;
        }
    }
    if(trdos != _trdos) {
        modifySTATE(_trdos * ZX_TRDOS, ZX_TRDOS)
        speccy->dev<zDevMem>()->update(ZX_UPDATE_STATE);
    }
}

int zCpuMain::ulaDelay() {
    auto machine(zSpeccy::machine);
    auto ts((frame - machine->tsFirst) % machine->tsLine);
    return machine->tsDelay[ts & 7];
}

bool zCpuMain::delayOps(int value) {
    if(dl) {
        if(ula->isPaper()) {
            if(pages & (1ULL << nb[value >> 14])) ticks = ulaDelay(), dl = ticks == 0;
        }
    } else dl = true;
    return dl;
}

bool zCpuMain::delayIO(int low, int high) {
    static int _io[] = { 1, 11, 0, 0, 4, 0, 0, 0, 9, 11, 0, 0, 9, 9, 9, 9 };
    auto row(((high >= 0x40 && high <= 0x7f) << 3) + ((low & 1) << 2));
    auto val(_io[row + (io & 7)]); dl = true;
    return true;
    if(io < 128 && val > 8 && ula->isPaper()) ticks = ulaDelay(), io |= 128, dl = ticks == 0;
    if(dl) {
        io &= 127;
        ticks = val & 7; io += ticks;
        dl = io >= 4;
    } 
    return dl;
}

bool zCpuMain::open(u8* ptr, size_t size, int type) {
    u8 _7ffd(48), _fe(0); u16 PC(0);
    auto mem(speccy->dev<zDevMem>());
    if(type == ZX_FMT_SNA) {
        bool is128(size >= HEAD_SNA::S_128_5), is6(size == HEAD_SNA::S_128_6);
        auto head((HEAD_SNA*)ptr); ptr += sizeof(HEAD_SNA); size -= sizeof(HEAD_SNA);
        af  = head->AF;  bc = head->BC;  de = head->DE;  hl = head->HL;
        af_ = head->AF_; bc_= head->BC_; de_= head->DE_; hl_= head->HL_;
        ix  = head->IX;  iy = head->IY;  rl = head->R & 0x7F; rh = head->R & 0x80;
        i   = head->I;   im = head->IM;  sp = head->SP;
        _fe = head->_FE; iff[0] = iff[1] = (head->IFF1 >> 2) & 1;
        memcpy(mem->page(5), head->PAGE5, 16384);
        memcpy(mem->page(2), head->PAGE2, 16384);
        if(is128) _7ffd = head->_7FFD;
        int pageX(is128 ? (_7ffd & 7) : 0);
        memcpy(mem->page(pageX), head->PAGEX, 16384);
        if(is128) {
            int pages(5 + is6), page(0);
            while(size > 0) {
                if(snaPages[page] != pageX) {
                    memcpy(mem->page(snaPages[page]), ptr, 16384);
                    ptr += 16384; size -= 16384;
                }
                page++;
            }
            PC = head->PC;
        } else PC = rm16(sp), sp += 2;
        modifySTATE(head->TRDOS * ZX_TRDOS, ZX_TRDOS)
    } else {
        auto model(MODEL_48), mode(0), emuFlags(0), length(30), pages(3), version(1);
        auto head((HEAD_Z80*)ptr);
        auto isCompressed((head->STATE1 & 0x20) == 0x20);
        PC = head->PC;
        if(!PC) {
            // версия 2 или 3
            mode = head->model; emuFlags = head->emulateFlags; length += head->length + 2;
            switch(length) {
                case 55:
                    version = 2;
                    model = (u8)(mode < 3 ? MODEL_48 : MODEL_128);
                    break;
                case 86: case 87:
                    version = 3;
                    if(mode >= 4) mode = 9;
                    model = (u8)(mode < 4 ? MODEL_48 : MODEL_128);
                    break;
                default: DLOG("Недопустимый формат файла!", 0); return false;
            }
            PC = head->PC_;
        }
        if(model == MODEL_48) {
            pages = (emuFlags & 0x80) ? 1 : 3;
        } else if(model == MODEL_128) {
            _7ffd = head->_7FFD; pages = 8;
            if(mode == 9) model = MODEL_PENTAGON128;
            else if(mode == 10) model = MODEL_SCORPION256, pages = 16;
        } else {
            DLOG("Неизвестное оборудование %i", mode);
            return false;
        }
        DLOG("version:%i length:%i pages:%i mode:%i model:%i", version, length, pages, mode, model);
        ptr += length; size -= length;
        // формируем страницы во временном буфере
        if(length > 30) {
            for(int i = 0 ; i < pages; i++) {
                auto sizeData(*(u16*)ptr); ptr += 2;
                isCompressed = sizeData != 0xFFFF;
                auto numPage(*ptr++ - 3), sizePage(isCompressed ? sizeData : 0x4000);
                auto isValidPage(false);
                if(model == MODEL_48) {
                    isValidPage = true;
                    switch(numPage) {
                        // 4->2 5->0 8->5
                        case 1: numPage = 2; break;
                        case 2: numPage = 0; break;
                        case 5: numPage = 5; break;
                        default: isValidPage = false; break;
                    }
                } else if(model == MODEL_PENTAGON128) {
                    isValidPage = numPage == i;
                    if(!isValidPage) {
                        // если страница неверна - может в файле меньше страниц... для 128К/256К ?
                        if(i == 8 || i == 16) break;
                    }
                } else isValidPage = numPage == i;
                if(!isValidPage) {
                    DLOG("Неизвестная страница %i-%i!", numPage, i);
                    return false;
                }
                auto page(mem->page(numPage));
                if(!z_unpackBlock(ptr, page, page + 16384, sizePage, sizePage != 16384)) {
                    DLOG("Ошибка при распаковке страницы %i!", numPage);
                    return false;
                }
                ptr += sizePage;
            }
        } else {
            if(!z_unpackBlock(ptr, mem->page(1), mem->page(4), size, isCompressed)) {
                DLOG("Ошибка при распаковке!", 0);
                return false;
            }
            memcpy(mem->page(5), mem->page(1), 16384);
            memcpy(mem->page(0), mem->page(3), 16384);
        }
        // инициализируем регистры
        iff[0] = head->IFF1; iff[1] = head->IFF2;
        rh = ((head->STATE1 & 1) << 7);rl = (head->R & 0x7F);
        bc = head->BC; de = head->DE;  hl = head->HL;
        bc_= head->BC_;de_= head->DE_; hl_= head->HL_;
        a  = head->A;  f  = head->F;   a_ = head->A_; f_ = head->F_;
        sp = head->SP; ix = head->IX;  iy = head->IY;
        i  = head->I;  im = (u8)(head->IM & 3);
        _fe = (u8)(224 | ((head->STATE1 & 14) >> 1));
        if(length > 30) {
            speccy->dev<zDevAY>()->state(&head->sndChipRegNumber, true);
            if(speccy->model >= MODEL_SCORPION256) speccy->writePort(0x1ffd, head->_1FFD, 0);
        }
    }
    pc = PC;
    speccy->writePort(0xfe, _fe, 0);
    speccy->writePort(0x7ffd, _7ffd, 0);
    speccy->update(ZX_UPDATE_STATE, 0);
    ILOG("Start PC: %i", PC);
    return true;
}

u8* zCpuMain::save(int type) {
    auto _7ffd(speccy->_7ffd); auto ptr(tmpBuf);
    auto mem(speccy->dev<zDevMem>());
    if(type == ZX_FMT_SNA) {
        auto head(new HEAD_SNA); int pageX(_7ffd & 3);
        head->_7FFD = _7ffd;  head->_FE = speccy->_fe; 
        head->PC = pc;  head->TRDOS = (u8)checkSTATE(ZX_TRDOS);
        head->AF = af;  head->BC = bc;  head->DE   = de;  head->HL = hl;
        head->AF_= af_; head->BC_= bc_; head->DE_  = de_; head->HL_= hl_;
        head->IX = ix;  head->IY = iy;  head->I    = i;   head->R  = (rl & 127) | (rh & 128);
        head->IM = im;  head->SP = sp;  head->IFF1 = ((iff[0] << 2) | 0b11111011);
        memcpy(head->PAGE5, mem->page(5), 16384);
        memcpy(head->PAGE2, mem->page(2), 16384);
        memcpy(head->PAGEX, mem->page(pageX), 16384);
        z_memcpy(&ptr, head, sizeof(HEAD_SNA)); delete head;
        for(int page : snaPages) {
            if(page != pageX) z_memcpy(&ptr, mem->page(page), 16384);
        }
    } else {
        static u8 models[] = { 0, 4, 9, 9, 10 };
        HEAD_Z80 h_z80 = { 0 }; auto head(&h_z80);
        // основные
        head->BC = bc;  head->DE = de;  head->HL = hl;  head->model = models[speccy->model];
        head->BC_= bc_; head->DE_= de_; head->HL_= hl_; head->IM    = im;
        head->A  = a;   head->F  = f;   head->A_ = a_;  head->F_    = f_;
        head->SP = sp;  head->IX = ix;  head->IY = iy;  head->IFF1  = iff[0];
        head->I  = i;   head->PC = 0;   head->PC_= pc;  head->IFF2  = iff[1];
        head->length = sizeof(HEAD_Z80) - 32; head->_1FFD = speccy->_1ffd; head->_7FFD = _7ffd;
        head->R  = (u8)(rl & 127); head->STATE1 = (u8)((rh >> 7) | ((speccy->_fe & 7) << 1) | 0x20);
        // для режима 128К
        speccy->dev<zDevAY>()->state(&head->sndChipRegNumber, false);
        // формируем буфер из содержимого страниц
        z_memcpy(&ptr, head, sizeof(HEAD_Z80));
        // страницы, в зависимости от режима
        if(head->model < MODEL_128) {
            // 4->2 5->7 8->5
            packPage(&ptr, mem->page(5), 8);
            packPage(&ptr, mem->page(2), 4);
            packPage(&ptr, mem->page(0), 5);
        } else {
            auto count(zSpeccy::machine->totalRAM);
            for(int i = 0; i < count; i++) packPage(&ptr, mem->page(i), (u8)(i + 3));
        }
    }
    return ptr;
}

