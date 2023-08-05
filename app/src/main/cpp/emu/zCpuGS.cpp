
#include "../sshCommon.h"
#include "zCpus.h"
//#include "zDAsm.h"
#include "stkCycles.h"

static u32 vfx[65];

constexpr int GSCPUFQ       = 12000000;             // hz
constexpr int GSINTFQ       = 37500;                // hz
//constexpr int GSCPUFQI      = GSCPUFQ / 50;
constexpr int GSCPUINT      = GSCPUFQ / GSINTFQ;
constexpr int BIT_COMMAND   = 0x01;
constexpr int BIT_DATA      = 0x80;

static void volume(uint32_t level) {
    for(int i = 0; i <= 64; i++) vfx[i] = 64 * i * level / 4096;
}

zCpuGs::zCpuGs() {
    static u8 gsROMs[] = { 10, 11 };
    clock = GSCPUFQ;
    memset(gs, 0, sizeof(gs));
    memset(vv, 0, sizeof(vv));
    volume(speccy->gsVolume);
    speccy->dev<zDevMixer>()->addSource(this);
    zSpeccy::loadROM(mem->page(PAGE_ROM_GS), gsROMs, 2);
    rb[0] = mem->page(PAGE_ROM_GS);
    wb[0] = mem->page(PAGE_TRASH);
    wb[1] = rb[1] = mem->page(PAGE_RAM_GS + 1);
    updateBanks();
}

void zCpuGs::updateBanks() {
    auto page(gs[gsPage]);
    if(page) {
        page = PAGE_RAM_GS + 2 * (page - 1);
        wb[2] = rb[2] = mem->page(page);
        wb[3] = rb[3] = mem->page(page + 1);
    } else {
        rb[2] = mem->page(PAGE_ROM_GS);
        rb[3] = mem->page(PAGE_ROM_GS + 1);
        wb[2] = wb[3] = mem->page(PAGE_TRASH);
    }
}

bool zCpuGs::write(u16 port, u8 val, u32 ticks) {
    if(updateFrame(ticks)) {
        if((u8)port == 0xBB) {
            gs[gsCmd] = val, gs[gsSts] |= BIT_COMMAND;
        } else {
            gs[gsOut] = val, gs[gsSts] |= BIT_DATA;
        }
    }
    return true;
}

bool zCpuGs::read(u16 port, u8* ret, u32 ticks) {
    if(updateFrame(ticks)) {
        if((u8)port == 0xBB) {
            *ret = gs[gsSts];
        } else {
            gs[gsSts] &= ~BIT_DATA, * ret = gs[gsDat];
        }
    }
    return true;
}

u8* zCpuGs::state(u8* ptr, bool restore) {
    auto buf(ptr);
    if(restore) {
        if(strncmp((char*)buf, "SERG CPU_GS", 11) != 0) return nullptr;
        buf += 11; 
        memcpy(gs, buf, sizeof(gs)); buf += sizeof(gs);
        memcpy(vv, buf, sizeof(vv)); buf += sizeof(vv);
        memcpy((u8*)&c, buf, COUNT_CPU); buf += COUNT_CPU;
        if(z_crc(ptr, buf - ptr) != wordLE(&buf)) return nullptr;
    } else {
        z_memcpy(&buf, "SERG CPU_GS", 11);
        z_memcpy(&buf, gs, sizeof(gs));
        z_memcpy(&buf, vv, sizeof(vv));
        z_memcpy(&buf, &c, COUNT_CPU);
        wordLE(&buf, z_crc(ptr, buf - ptr));
    }
    return buf;
}

bool zCpuGs::operationIO() {
    auto high(*(u8*)((u8*)&c + ((size_t)(offs == 0) * RF + RB)));
    auto low(*(u8*)((u8*)&c + ((size_t)(offs == 0) * ROP1)));
    auto port((u16)(low | (high << 8)) & 0x0F);
    if(zm->name & 1) {
        m8 = *dst;
        switch(port) {
            case 0x00: gs[gsPage] = m8 & 0x0F; updateBanks(); break;
            case 0x03: gs[gsSts] |= BIT_DATA; gs[gsDat] = m8; break;
            case 0x05: gs[gsSts] &= ~BIT_COMMAND; break;
            case 0x06: case 0x07:
            case 0x08: case 0x09: gs[gsVol0 + port - 6] = m8 & 0x3F; flush(); break;
            case 0x0A: gs[gsSts] = (gs[gsSts] & 0x7F) | ((gs[gsPage] ^ 1) << 7); break;
            case 0x0B: gs[gsSts] = (gs[gsSts] & 0xFE) | ((gs[gsVol0] >> 5) & 1); break;
        }
    } else {
        m8 = 0xFF;
        switch(port) {
            case 0x01: m8 = gs[gsCmd]; break;
            case 0x02: gs[gsSts] &= ~BIT_DATA; m8 = gs[gsOut]; break;
            case 0x04: m8 = gs[gsSts]; break;
            case 0x05: gs[gsSts] &= ~BIT_COMMAND; break;
            case 0x0A: gs[gsSts] = (gs[gsSts] & 0x7F) | ((gs[gsPage] ^ 1) << 7); break;
            case 0x0B: gs[gsSts] = (gs[gsSts] & 0xFE) | ((gs[gsVol0] >> 5) & 1); break;
        }
    }
    return true;
}

u8 zCpuGs::rm8(u16 address) {
    auto val(*ptr(address));
    if(speccy->debugLaunch && (_bp[address] & ZX_BP_RMEM)) {
        if(checkBPs(address, ZX_BP_RMEM, val)) return 0;
    }
    if((address & 0xE000) == 0x6000) {
        auto ch((address >> 8) & 3);
        vv[ch] = (val * vfx[gs[gsVol0 + ch]]);
        flush();
    }
    return val;
}

void zCpuGs::flush() {
    auto ticks(frame + (u32)(ts - fstart));
    auto l(vv[0] + vv[1]), r(vv[2] + vv[3]);
    zDevSound::updateData(ticks, l, r);
}

void zCpuGs::begin(u32 tacts) {
    fstart = ts + frame;
    zDevSound::begin(0);
}

void zCpuGs::finish(u32 tacts) {
    if(updateFrame(tacts)) {
        auto ticks(frame + (u32)(ts - fstart));
        zDevSound::finish(ticks);
    }
}

bool zCpuGs::updateFrame(u32 clk) {
    if(!speccy->gsLaunch) return false;
    auto _end((u32)((float)clk * mult)); auto end((u64)_end + fstart);
    auto count((end - ts) / GSCPUINT);
    frameTS = GSCPUINT;
    while(count--) {
        while((int)frame < GSCPUINT) {
            exec(); if(checkSTATE(ZX_BP)) return false;
        }
        if(frame < GSCPUINT) break;
        if(iff[0] && !ei) frame += irqInt();
        ts += GSCPUINT; frame -= GSCPUINT;
    }
    return true;
}

void zCpuGs::update(int param) {
    if(param == ZX_UPDATE_RESET) {
        if(speccy->gsReset) {
            memset(gs, 0, sizeof(gs));
            gs[gsSts] = 0x7E;
            updateBanks();
        } else param = ZX_UPDATE_PRIVATE;
    }
    zCpu::update(param);
    zDevSound::update((int)clock);
    if(param == ZX_UPDATE_STATE) {
        mult = ((float)clock / 50.0f) / (float)speccy->turboTS();
        volume(speccy->gsVolume);
    }
}

int zCpuGs::exec() {
    if(halt) return _halt();
    pcb = pc;
    // проверить на точку останова для кода
    if(speccy->debugLaunch && (_bp[pc] & (ZX_BP_TRACE | ZX_BP_EXEC))) 
        return !checkBPs(pc, ZX_BP_EXEC);
    zCpu::update(ZX_UPDATE_PRIVATE); step++; ticks = tacts = 0;
    while(step) {
        auto tmp((int)z_cycles[zm->cycles + step]), cycl(tmp & 0b11111000);
        if(cycl == CPU_CYCLES::FIN) { step = 0; break; }
        ticks = tmp & 7; step++;
        auto high((cycl & 8) >> 3);
        switch(cycl) {
            case CPU_CYCLES::SRL: case CPU_CYCLES::SRH: dst[high] = rm8(sp++); break;
            case CPU_CYCLES::SWL: case CPU_CYCLES::SWH: sp--; wm8(sp, src[high]); break;
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
            case CPU_CYCLES::PRT: operationIO(); break;
            case CPU_CYCLES::IOP: iop(); break;
            case CPU_CYCLES::OCR: ocr(false); break;
        }
        tacts += ticks;
    }
    frame += tacts;
    return 0;
}

