  
#include "../sshCommon.h"
#include "zDevs.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
//                                    КЛАВИАТУРА                                               //
/////////////////////////////////////////////////////////////////////////////////////////////////

void zDevKeyboard::update(int param) {
    if(param == ZX_UPDATE_RESET) {
        memset(speccy->semiRow, 255, 8);
    }
}

bool zDevKeyboard::read(u16 port, u8* ret, u32) {
    // опрос клавиатуры из ПЗУ
    theApp->keyb->updateMode();
    // 0,1,2,3,4 - клавиши полуряда, 6 - EAR, 5,7 - не используется
    port >>= 8; u8 res(0xBF);
    for(int i = 0; i < 8; i++) { if(!(port & (1 << i))) res &= speccy->semiRow[i]; }
    *ret = res;
    return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//                                     ДЖОЙСТИК                                                //
/////////////////////////////////////////////////////////////////////////////////////////////////

bool zDevKempston::checkRead(u16 port) const {
    if(port & 0x20) return false;
    port |= 0xfa00; return !(port == 0xfadf || port == 0xfbdf || port == 0xffdf);
}

bool zDevKempston::read(u16, u8* ret, u32) {
    *ret = speccy->semiRow[8];
    return false;
}

void zDevKempston::update(int param) {
    if(param == ZX_UPDATE_RESET) speccy->semiRow[8] = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//                                       МЫШЬ                                                  //
/////////////////////////////////////////////////////////////////////////////////////////////////

bool zDevMouse::checkRead(u16 port) const {
    if(port & 0x20) return false;
    port |= 0xFA00; return (port == 0xFADF || port == 0xFBDF || port == 0xFFDF);
}

bool zDevMouse::read(u16 port, u8* ret, u32) {
    switch(port | 0xFA00) {
        case 0xFBDF: *ret = speccy->mouse[1]; break;
        case 0xFFDF: *ret = speccy->mouse[2]; break;
        case 0xFADF: *ret = speccy->mouse[0]; break;
    }
    return false;
}

void zDevMouse::update(int param) {
    if(param == ZX_UPDATE_RESET) {
        speccy->mouse[0] = 0xFF;
        speccy->mouse[1] = 128;
        speccy->mouse[2] = 96;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//                                    КОНТРОЛЛЕР ULA                                           //
/////////////////////////////////////////////////////////////////////////////////////////////////

void zDevUla::TIMING::set(int _t, ZONE _zone, u32* _dst, int _so, int _ao) {
    dst = _dst; t = _t; zone = _zone; so = _so; ao = _ao;
}

zDevUla::zDevUla() {
    frameBuffer  = new u32[352 * 864];
    memset(frameBuffer, 0, 352 * 576 * 4);
    memset(timings, 0, sizeof(timings));
    memset(atrTab, 0, sizeof(atrTab));
    memset(scrTab, 0, sizeof(scrTab));
    memset(colTab, 0, sizeof(colTab));
    colors = speccy->colors;
    // вычисление таблицы адреса строк экрана
    int i = 0;
    for(int p = 0; p < 4; p++) {
        for(int y = 0; y < 8; y++) {
            for(int o = 0; o < 8; o++, i++) {
                scrTab[i] = p * 2048 + y * 32 + o * 256;
                atrTab[i] = (6144 + (p * 8 + y) * 32) - scrTab[i];
            }
        }
    }
    // таблица для мгновенного доступа к атрибутам
    for(int a = 0; a < 256; a++) {
        auto ink((u8)((a >> 0) & 7));
        auto paper((u8)((a >> 3) & 7));
        auto bright((u8)((a >> 6) & 1));
        auto flash((u8)((a >> 7) & 1));
        if(ink) ink |= bright << 3;
        if(paper) paper |= bright << 3;
        auto c1((u8)((paper << 4) | ink));
        if(flash) { auto t = ink; ink = paper; paper = t; }
        auto c2((u8)((paper << 4) | ink));
        colTab[a] = c1; colTab[a + 256] = c2;
    }
}

bool zDevUla::read(u16 port, u8* ret, u32 ticks) {
    *ret = (timing->zone == TIMING::Z_PAPER ? VIDEO[timing->so + timing->ao + ((tm - timing->t) >> 2)] : 0xFF);
    return false;
}

bool zDevUla::write(u16 port, u8 val, u32 ticks) {
    if(!(port & 1)) {
        speccy->_fe = val;
        colorBorder = val & 7;
    } else if(!(port & 0x8002)) {
        auto vid((val & 8) ? 7 : 5);
        if(speccy->vid != vid) {
            speccy->vid = vid;
            update(ZX_UPDATE_STATE);
        }
        return true;
    }
    return false;
}

static u32 _c[] = { 0, 0 };

static u32 gigaBlend(u32** src, u32 c1) {
    return c1;
    auto s(*src); auto c0(*s); s++;
    auto r0((int)c0 & 0xFF), g0((int)(c0 >> 8) & 0xff), b0((int)(c0 >> 16) & 0xff);
    auto r1((int)c1 & 0xFF), g1((int)(c1 >> 8) & 0xff), b1((int)(c1 >> 16) & 0xff);
    r0 = (int)(((float)r0 * 0.66f + (float)r1 * 0.66f) * 0.75f);
    g0 = (int)(((float)g0 * 0.66f + (float)g1 * 0.66f) * 0.75f);
    b0 = (int)(((float)b0 * 0.66f + (float)b1 * 0.66f) * 0.75f);
    *src = s;
    return (r0 | (g0 << 8) | (b0 << 16));
}

void zDevUla::updateBorder(int offs, int count) {
    auto dst(timing->dst + offs);
    auto c(colors[colorBorder]);
    if(isGigaApply) {
        int g1(giga * szScr), g2((1 - giga) * szScr);
        dst += g1; auto dst1(timing->dst + offs + g2);
        while(count-- > 0) {
            *dst++ = gigaBlend(&dst1, c);
            *dst++ = gigaBlend(&dst1, c);
        }
    } else {
        while(count-- > 0) *dst++ = c, * dst++ = c;
    }
}

void zDevUla::updatePaper(int offs, int count) {
    auto scr(VIDEO + timing->so + offs);
    auto dst(timing->dst + (offs << 3));
    if(isGigaApply) {
        int g1(giga * szScr), g2((1 - giga) * szScr);
        dst += g1; auto dst1(timing->dst + (offs << 3) + g2);
        while(count-- > 0) {
            auto idx(colorTab[*(scr + timing->ao)]);
            auto pix(*scr++); auto col1(colors[idx & 15]), col2(colors[idx >> 4]);
            for(int b = 7 ; b >= 0; b--) *dst++ = gigaBlend(&dst1, ((pix >> b) & 1) ? col1 : col2);
        }
    } else {
        while(count-- > 0) {
            auto idx(colorTab[*(scr + timing->ao)]);
            auto pix(*scr++); _c[1] = colors[idx & 15]; _c[0] = colors[idx >> 4];
            for(int b = 7 ; b >= 0; b--) *dst++ = _c[(pix >> b) & 1];
        }
    }
}

void zDevUla::updateRay(int tact) {
    int t(tm);
    while(t < tact) {
        auto next((timing + 1)->t);
        int end(z_min(tact, next)), offs(t - timing->t);
        switch(timing->zone) {
            case TIMING::Z_SHADOW: end = next; break;
            case TIMING::Z_BORDER: updateBorder(offs << 1, end - t); break;
            case TIMING::Z_PAPER:  updatePaper(offs >> 2, end - t);  break;
        }
        t = end;
        if(t == next) timing++;
    }
    tm = t;
}

void zDevUla::firstScreen() {
    auto c(colors[colorBorder]); auto dst(frameBuffer);
    int borderWidth(zSpeccy::machine->borderWidth), borderHeight(zSpeccy::machine->borderHeight);
    for(int i = 0; i < borderHeight * (256 + borderWidth * 2); i++) *dst++ = c;
    for(int y = 0; y < 192; ++y) {
        for(int x = 0; x < borderWidth; ++x) *dst++ = c;
        auto scr(VIDEO + scrTab[y]);
        for(int x = 0; x < 32; x++) {
            auto idx(colorTab[*(scr + atrTab[y])]);
            auto pix(*scr++); _c[1] = colors[idx & 15]; _c[0] = colors[idx >> 4];
            for(int b = 7 ; b >= 0; b--) *dst++ = _c[(pix >> b) & 1];
        }
        for(int x = 0; x < borderWidth; ++x) *dst++ = c;
    }
    for(int i = 0; i < borderHeight * (256 + borderWidth * 2); ++i) *dst++ = c;
    updateRay((int)speccy->dev<zCpuMain>()->frame);
}

void zDevUla::update(int param) {
    if(param == ZX_UPDATE_FRAME) {
        if(speccy->dev<zCpuMain>()->frame < 32) {
            giga = 1 - giga; tm = 0; timing = timings;
            //isGigaApply = (speccy->gigaScreen && giga);
            isGigaApply = false;
            colorTab = &colTab[(blink++ & 16) << 4];
        }
        if(fscr) {
            fscr = false;
            firstScreen();
        }
        memcpy(frameBuffer + 202752, frameBuffer, 405504);
    } else {
        if(param == ZX_UPDATE_RESET) {
            speccy->_fe = 0b11100111;
            speccy->vid = 5;
        } else if(param == ZX_UPDATE_MODEL) {
            // таблица таймингов экрана
            auto dst(frameBuffer);
            auto hborder((int)zSpeccy::machine->borderHeight), wborder((int)zSpeccy::machine->borderWidth);
            auto line_t((int)(zSpeccy::machine->tsFirst - (hborder * zSpeccy::machine->tsLine + wborder / 2)));
            int idx(0), scr_width(wborder * 2 + 256);
            // пропустить невидимую зону
            timings[idx++].set(0, TIMING::Z_SHADOW);
            // верхняя граница
            for(size_t i = 0; i < hborder; i++) {
                timings[idx++].set(line_t, TIMING::Z_BORDER, dst);
                timings[idx++].set(line_t + (wborder * 2 + 256) / 2, TIMING::Z_SHADOW);
                dst += scr_width; line_t += (int)zSpeccy::machine->tsLine;
            }
            // экран + граница
            for(size_t i = 0; i < 192; i++) {
                timings[idx++].set(line_t, TIMING::Z_BORDER, dst);
                timings[idx++].set(line_t + wborder / 2, TIMING::Z_PAPER, dst + wborder, scrTab[i], atrTab[i]);
                timings[idx++].set(line_t + (wborder + 256) / 2, TIMING::Z_BORDER, dst + wborder + 256);
                timings[idx++].set(line_t + (wborder * 2 + 256) / 2, TIMING::Z_SHADOW);
                dst += scr_width; line_t += (int)zSpeccy::machine->tsLine;
            }
            // нижняя граница
            for(size_t i = 0; i < hborder; ++i) {
                timings[idx++].set(line_t, TIMING::Z_BORDER, dst);
                timings[idx++].set(line_t + (wborder * 2 + 256) / 2, TIMING::Z_SHADOW);
                dst += scr_width; line_t += (int)zSpeccy::machine->tsLine;
            }
            // теневая зона
            timings[idx].set(0x7fffffff, TIMING::Z_SHADOW);
            colorTab = &colTab[0];
            tm = blink = giga = 0;
            timing = timings;
        }
        VIDEO = speccy->dev<zDevMem>()->page(speccy->vid);
        colorBorder = speccy->_fe & 7;
        auto sz((float)((6 - speccy->sizeBorder) * 16));
        frame->setScale(352.0f / (256 + sz), 288.0f / (192 + sz));
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//                                      ПОРТЫ РАСШИРЕНИЯ                                       //
/////////////////////////////////////////////////////////////////////////////////////////////////

bool zDevExtPort::checkWrite(u16 port) const {
    auto model(speccy->model);
    auto ret = ((port & 0x7F) == 0x7B) || 
        (model == MODEL_SCORPION256 && port == 0x1FFD) ||
        ((model == MODEL_PENTAGON128 || model == MODEL_PENTAGON512) && port == 0xEFF7);
    return ret;
}

bool zDevExtPort::read(u16 port, u8* ret, u32 ticks) {
    if(port == 0x1FFD) {
        *ret = speccy->_1ffd;
        speccy->turboMode = true;
        speccy->flags |= ZX_CAPT;
    } else if(port == 0xEFF7) {
        *ret = speccy->_eff7;
    } else if((port & 0x7F) == 0x7B) {
        // FB/7B
        auto _flags(speccy->flags);
        auto cache((port & 0x80) >> 7);
        modifySTATE(cache * ZX_CACHE, ZX_CACHE)
        if(_flags != speccy->flags) speccy->dev<zDevMem>()->update(ZX_UPDATE_STATE);
    }
    return true;
}

bool zDevExtPort::write(u16 port, u8 val, u32 ticks) {
    if(port == 0x1FFD) {
        // 0,2,5,12=1; 1,14,15=0
        // 0   -> 1 - ПЗУ - RAM0, 0 - ПЗУ из бита 1
        // 1   -> 1 - ROM 2, 0 - из 0x7FFD
        // 4   -> 1 - RAM SCORPION/KAY 256K, 0 - из 0x7FFD
        // 6.7 -> SCORPION/KAY 1024K
        speccy->_1ffd = val;
        speccy->rom = (val & 1 ? PAGE_RAM_CPU : (val & 2 ? PAGE_SYS : (PAGE_ROM_CPU + ((speccy->_7ffd & 16) >> 4))));
        speccy->ram = PAGE_RAM_CPU + (u8)((speccy->_7ffd & 7) + (u8)((val & 16) >> 1));
        speccy->dev<zDevMem>()->update(ZX_UPDATE_STATE);
    } else if(port == 0xEFF7) {
        speccy->_eff7 = val;
        speccy->turboMode = !(val & EFF7_TURBO);
        modifySTATE(ZX_CAPT, 0)
    }
    return true;
}

void zDevExtPort::update(int param) {
    if(param == ZX_UPDATE_RESET) {
        speccy->_1ffd = speccy->_eff7 = 0;
        speccy->turboMode = false;
        modifySTATE(ZX_CAPT, 0)
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////
//                                      ПАМЯТЬ                                                 //
/////////////////////////////////////////////////////////////////////////////////////////////////

zDevMem::zDevMem() {
    RAM = new u8[ZX_TOTAL_RAM];
    memset(RAM, 0, ZX_TOTAL_RAM);
}

bool zDevMem::checkRead(u16 port) const {
    return speccy->model == MODEL_SCORPION256 && checkWrite(port);
}

bool zDevMem::read(u16 port, u8* ret, u32 ticks) {
    *ret = speccy->_7ffd;
    speccy->turboMode = false;
    speccy->flags |= ZX_CAPT;
    return true;
}

bool zDevMem::write(u16 port, u8 val, u32) {
    if(speccy->is48k()) return true;
    auto ram(speccy->ram), rom(speccy->rom);
    // 0, 1, 2 - страница 0-7, 3 - экран 5/7, 4 - ПЗУ 0 - 128К 1 - 48К, 5 - блокировка, 6/7 - pentagon 256K/512K
    speccy->_7ffd = val;
    ram = PAGE_RAM_CPU + (val & 7);
    switch(speccy->model) {
        case MODEL_SCORPION256:
            if(!(speccy->_1ffd & 2)) rom = PAGE_ROM_CPU + ((val & 16) >> 4);
            ram += ((speccy->_1ffd & 16) >> 1);
            break;
        case MODEL_PENTAGON512:
            if(!(speccy->_eff7 & EFF7_128LOCK)) 
                ram += ((val & 192) >> 3);
        default:
            rom = PAGE_ROM_CPU + ((val & 16) >> 4);
            break;
    }
    speccy->ram = ram; speccy->rom = rom;
    update(ZX_UPDATE_STATE);
    return false;
}

void zDevMem::update(int param) {
    if(param >= ZX_UPDATE_STATE) {
        if(param == ZX_UPDATE_RESET) {
            speccy->ram = 0; speccy->rom = zSpeccy::machine->startROM;
            speccy->_7ffd = ((speccy->rom - PAGE_ROM_CPU) << 4) | speccy->ram;
            if(speccy->model == MODEL_48) speccy->_7ffd |= 32;
            speccy->flags &= ~(ZX_TRDOS | ZX_CACHE);
            // очищаем ОЗУ
            z_memzero(RAM, PAGE_RAM_GS << 14);
        }
        else if(param == ZX_UPDATE_MODEL) {
            // загрузить ПЗУ
            zSpeccy::loadROM(page(PAGE_ROM_CPU), zSpeccy::machine->pageROM, 4);
        }
        int r, w;
        if(checkSTATE(ZX_CACHE)) {
            r = PAGE_CACHE + ((speccy->_7ffd & 0x10) == 0);
            w = (speccy->_eff7 & EFF7_RAM_MAP_ROM ? PAGE_TRASH : r);
        } else if(checkSTATE(ZX_TRDOS)) {
            r = w = PAGE_SYS + ((speccy->_7ffd & 0x10) >> 4);
        } else {
            r = w = speccy->rom;
        }
        if(r >= PAGE_ROM_GS) w = PAGE_TRASH;
        speccy->dev<zCpuMain>()->updateBanks(page(r), page(w), page(speccy->ram), speccy->ram);
    }
}

u8* zDevMem::state(u8* buf, bool restore) {
    u32 size; auto ptr(buf);
    if(restore) {
        if(strncmp((char*)ptr, "SSH MEMORY", 10) != 0) return nullptr;
        ptr += 10;
        // грузим страницы ОЗУ
        for(int i = 0; i < PAGE_TRASH; i++) {
            size = wordLE(&ptr);
            if(!z_unpackBlock(ptr, &RAM[i << 14], &RAM[i << 14] + 16384, size, true)) {
                DLOG("Ошибка при распаковке страницы %i состояния!!!", i);
                return nullptr;
            }
            ptr += size;
        }
        if(z_crc(buf, ptr - buf) != wordLE(&ptr)) return nullptr;
    } else {
        z_memcpy(&ptr, "SSH MEMORY", 10);
        // сохраняем страницы ОЗУ
        for(int i = 0; i < PAGE_TRASH; i++) {
            z_packBlock(&RAM[i << 14], &RAM[i << 14] + 16384, &ptr[2], false, size);
            wordLE(&ptr, (u16)size); ptr += size;
        }
        wordLE(&ptr, z_crc(buf, ptr - buf));
    }
    return ptr;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//                                ЗВУК - БАЗОВЫЕ ФУНКЦИИ                                       //
/////////////////////////////////////////////////////////////////////////////////////////////////

static u32 filters[128] = {
        0,     52,    105,   161,   219,   280,   346,   418,
        495,   580,   672,   773,   884,   1006,  1140,  1286,
        1445,  1619,  1809,  2014,  2236,  2476,  2735,  3013,
        3311,  3629,  3969,  4331,  4715,  5122,  5552,  6006,
        6483,  6985,  7511,  8062,  8636,  9236,  9860,  10507,
        11179, 11875, 12593, 13335, 14099, 14884, 15691, 16518,
        17365, 18231, 19114, 20015, 20932, 21863, 22809, 23767,
        24738, 25718, 26708, 27705, 28710, 29720, 30733, 31750,
        32768, 33785, 34802, 35815, 36825, 37830, 38827, 39817,
        40797, 41768, 42726, 43672, 44603, 45520, 46421, 47304,
        48170, 49017, 49844, 50651, 51436, 52200, 52942, 53660,
        54356, 55028, 55675, 56299, 56899, 57473, 58024, 58550,
        59052, 59529, 59983, 60413, 60820, 61204, 61566, 61906,
        62224, 62522, 62800, 63059, 63299, 63521, 63726, 63916,
        64090, 64249, 64395, 64529, 64651, 64762, 64863, 64955,
        65040, 65117, 65189, 65255, 65316, 65374, 65430, 65483
};

void zDevSound::update(int _clock) {
    clock           = (u32)_clock;
    passedSndTicks  = passedClkTicks = 0;
    tick            = base_tick = 0;
    spos            = cpos = nullptr;
    sample          = frequencies[speccy->sndFreq];
}

void zDevSound::begin(u32 tacts) {
    if(nSamples > 2048) {
//        ILOG("Sound buffer overflow %i", nSamples);
        nSamples = 0;
    }
    spos        = &buffer[nSamples];
    cpos        = spos;
    base_tick   = tick;
}

void zDevSound::finish(u32 tacts) {
    auto prevMixL(left), prevMixR(right);
    auto endTick(((passedClkTicks + tacts) * (u64)sample * 64ULL) / clock);
    if(!activeCnt) left = right = 0;
    flush((u32)(endTick - passedSndTicks));
    if(!activeCnt) left = prevMixL, right = prevMixR;
    nSamples += cpos - spos;
    tick -= (nSamples << 6);
    passedSndTicks += ((uint64_t)nSamples << 6ULL);
    passedClkTicks += tacts;
}

void zDevSound::updateData(u32 tacts, u32 l, u32 r) {
    if(!((l ^ left) | (r ^ right))) return;
    activeCnt = 50;
    auto endTick((u32)((tacts * (u64)sample * 64ULL) / clock));
    flush(base_tick + endTick);
    left = l; right = r;
}

void zDevSound::flush(u32 endtick) {
    u32 scale;
    if(!((endtick ^ tick) & ~63)) {
        scale = filters[(endtick & 63) + 64] - filters[(tick & 63) + 64];
        l2 += left * scale; r2 += right * scale;
        scale = filters[endtick & 63] - filters[tick & 63];
        l1 += left * scale; r1 += right * scale;
        tick = endtick;
    } else {
        scale = 65535 - filters[(tick & 63) + 64];
        cpos->left  += (left  * scale + l2) >> 16;
        cpos->right += (right * scale + r2) >> 16;
        cpos++;
        scale = 32768 - filters[tick & 63];
        l2 = l1 + left  * scale;
        r2 = r1 + right * scale;
        tick = (tick | 63) + 1;
        if((endtick ^ tick) & ~63) {
            auto valL(left * 32768), valR(right * 32768);
            do {
                cpos->left  += (l2 + valL) >> 16;
                cpos->right += (r2 + valR) >> 16;
                cpos++;
                tick += 64; l2 = valL; r2 = valR;
            } while((endtick ^ tick) & ~63);
        }
        tick = endtick;
        scale = filters[(endtick & 63) + 64] - 32768;
        l2 += left  * scale; r2 += right * scale;
        scale = filters[endtick & 63];
        l1 = left * scale; r1 = right * scale;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//                                      ДИНАМИК                                                //
/////////////////////////////////////////////////////////////////////////////////////////////////

zDevBeeper::zDevBeeper() : volSpk(12800), volMic(3200) {
    speccy->dev<zDevMixer>()->addSource(this);
}

void zDevBeeper::_write(u8 val, u32 ticks, bool speaker) {
    if(speccy->bpLaunch) {
        auto mono((u32)(speaker ? (((val & 16) >> 4) * volSpk) : (((val & 8) >> 3) * volMic)));
        updateData(ticks, mono, mono);
    }
}

void zDevBeeper::update(int) {
    zDevSound::update((int)speccy->turboTS() * 50);
    volSpk = (int)((2.5f * (float)speccy->bpVolume) * 512);
    volMic = volSpk >> 2;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//                               ЗВУКОВОЙ СОПРОЦЕССОР AY                                       //
/////////////////////////////////////////////////////////////////////////////////////////////////

static u32 chipVol[] = {
        0x0000, 0x0000, 0x0340, 0x0340, 0x04C0, 0x04C0, 0x06F2, 0x06F2, 0x0A44, 0x0A44, 0x0F13, 0x0F13, 0x1510, 0x1510, 0x227E, 0x227E,
        0x289F, 0x289F, 0x414E, 0x414E, 0x5B21, 0x5B21, 0x7258, 0x7258, 0x905E, 0x905E, 0xB550, 0xB550, 0xD7A0, 0xD7A0, 0xFFFF, 0xFFFF, // AY
        0x0000, 0x0000, 0x00EF, 0x01D0, 0x0290, 0x032A, 0x03EE, 0x04D2, 0x0611, 0x0782, 0x0912, 0x0A36, 0x0C31, 0x0EB6, 0x1130, 0x13A0,
        0x1751, 0x1BF5, 0x20E2, 0x2594, 0x2CA1, 0x357F, 0x3E45, 0x475E, 0x5502, 0x6620, 0x7730, 0x8844, 0xA1D2, 0xC102, 0xE0A2, 0xFFFF  // YM
};

static u32 stereoMode[] = {
        58, 58,  58,58,   58,58 , // mono
        100,10,  66,66,   10,100, // ABC
        100,10,  10,100,  66,66 , // ACB
        66,66,   100,10,  10,100, // BAC
        10,100,  100,10,  66,66 , // BCA
        66,66,   10,100,  100,10, // CAB
        10,100,  66,66,   100,10  // CBA
};

zDevAY::zDevAY() {
    memset(vols, 0, sizeof(vols));
    speccy->dev<zDevMixer>()->addSource(this);
}

bool zDevAY::checkWrite(u16 port) const {
    if(port & 2) return false;
    if((port & 0xC0FF)      == 0xC0FD) return true;
    return (port & 0xC000)  == 0x8000;
}

bool zDevAY::write(u16 port, u8 val, u32 ticks) {
    if((port & 0xC0FF) == 0xC0FD) {
        regs[CUR_REG] = (u8)(val & 0x0F);
    } else if((port & 0xC000) == 0x8000) {
        static const u8 mask[16] = { 0xff, 0x0f, 0xff, 0x0f, 0xff, 0x0f, 0x1f, 0xff, 0x1f, 0x1f, 0x1f, 0xff, 0xff, 0x0f, 0xff, 0xff };
        if(speccy->ayLaunch) {
            auto curReg(regs[CUR_REG]);
            if(curReg > 15) return false;
            val &= mask[curReg];
            if(curReg != ESHAPE && regs[curReg] == val) return false;
            regs[curReg] = val;
            if(ticks) flush((int)(ticks * mult_const) >> 14);
            switch(curReg) {
                case AFINE: case ACOARSE: case BFINE: case BCOARSE: case CFINE: case CCOARSE: 
                    curReg &= 0x0E;
                case EFINE: case ECOARSE:
                    fv[(curReg - (curReg > 10) * 3) >> 1] = (regs[curReg] | (regs[curReg + 1] << 8));
                    break;
                case NOISEPER: fv[3] = val * 2U; break;
                case ENABLE:
                    for(int i = 0; i < 6; i++) bits[i + 4] = (0U - ((val & (1 << i)) != 0));
                    break;
                case AVOL: case BVOL: case CVOL:
                    ev[curReg - 8] = (0U - ((val & 0x10) != 0));
                    ev[curReg - 5] = ((val & 0x0F) * 2 + 1) & ~ev[curReg - 8];
                    break;
                case ESHAPE:
                    fv[9] = 0; if(regs[ESHAPE] & 4) env = 0, denv = 1; else env = 31, denv = -1;
                    break;
            }
        }
    }
    return false;
}

void zDevAY::flush(int chiptick) {
    while(t++ < chiptick) {
        for(int i = 0 ; i < 3; i++) { if(++fv[i + 5] >= fv[i]) fv[i + 5] = 0, bits[i] ^= 0xFFFFFFFF; }
        if(++fv[8] >= fv[3]) fv[8] = 0, ns = (ns * 2 + 1) ^ (((ns >> 16) ^ (ns >> 13)) & 1), bits[3] = 0 - ((ns >> 16) & 1);
        if(++fv[9] >= fv[4]) {
            fv[9] = 0, env += denv;
            if(env & ~31) {
                auto mask((u32)(1 << regs[ESHAPE]));
                if(mask      & 0b1000001011111111) env = 0, denv = 0;
                else if(mask & 0b0001000100000000) env &= 31;
                else if(mask & 0b0100010000000000) denv = -denv, env += denv;
                else env = 31, denv = 0; // 11,13
            }
        }
        u32 mixl(0), mixr(0);
        for(u32 i = 0 ; i < 3; i++) {
            auto en(((ev[i] & env) | ev[i + 3]) & ((bits[i] | bits[i + 4]) & (bits[3] | bits[i + 7])));
            mixl += vols[i * 2 + 0][en]; mixr += vols[i * 2 + 1][en];
        }
        updateData(t, mixl, mixr);
    }
}

void zDevAY::begin(u32 tacts) {
    t = (int)(tacts * chip_clock / cpu_clock);
    zDevSound::begin(t); 
}

void zDevAY::finish(u32 tacts) {
    auto tick(((clk_ticks + tacts) * chip_clock) / cpu_clock);
    flush((i32)(tick - chip_ticks));
    zDevSound::finish(t);
    clk_ticks += tacts;
    chip_ticks += t;
}

void zDevAY::update(int param) {
    if(param != ZX_UPDATE_FRAME) {
        cpu_clock  = speccy->turboTS() * 50;
        chip_clock = (speccy->turboAyClock() >> 3);
        mult_const = (u32)(((uint64_t)chip_clock << 14) / cpu_clock);
        chip_ticks = clk_ticks = 0;
        ns         = 0xFFFF;
        zDevSound::update((int)chip_clock);
        for(u8& reg : regs) reg = 0;
        env = denv = 0; ns = 0;
        memset(ev, 0, sizeof(ev));
        memset(fv, 0, sizeof(fv));
        memset(bits, 0, sizeof(bits));
        left = right = 0;

        auto vol = (int)speccy->ayVolume * 2184 + 7;
        auto voltab = &chipVol[speccy->ayChip * 32];
        auto stereo = &stereoMode[speccy->ayChannel * 6];

        for(int j = 0; j < 6; j++) {
            for(int i = 0; i < 32; i++) {
                vols[j][i] = (u32)(((u64)vol * voltab[i] * stereo[j]) / (65535 * 100 * 3));
            }
        }
    }
}

u8* zDevAY::state(u8* ptr, bool restore) {
    if(restore) {
        memcpy(regs, ptr, sizeof(regs)); ptr += sizeof(regs);
    } else {
        z_memcpy(&ptr, regs, sizeof(regs));
    }
    return ptr;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//                                   ЗВУКОВАЯ СИСТЕМА                                          //
/////////////////////////////////////////////////////////////////////////////////////////////////

zDevMixer::zDevMixer() {
    memset(audioBuffer, 0, sizeof(audioBuffer));
}

void zDevMixer::release() {
    if(player) {
        manager->sound.deletePlayer(1234);
        player = nullptr;
    }
}

void zDevMixer::update(int param) {
    if(param == ZX_UPDATE_FRAME) mix();
    else {
        auto is(isEnable);
        isEnable   = speccy->sndLaunch && speccy->execLaunch;
        for(auto s : sources) s->nSamples = 0;
        memset(mixBuffer, 0, sizeof(mixBuffer));
        if(is != isEnable) {
            if(!isEnable) release();
            else {
                zPlayerParams pr;
                pr.rate = frequencies[speccy->sndFreq] * 1000;
                pr.chan = 2; pr.bits = 16;
                pr.bufSize = sizeof(mixBuffer);
                player = manager->sound.createPlayer(1234, TYPE_MEM, pr);
            }
        }
    }
}

void zDevMixer::mix() {
    int sourcesCount(sources.size());
    if(sourcesCount == 0 || !isEnable) return;
    auto minSamples(sources[0]->nSamples), maxSamples(sources[0]->nSamples);
    for(int i = 1; i < sourcesCount; i++) {
        auto samples(sources[i]->nSamples);
        if(samples < minSamples) minSamples = samples;
        if(samples > maxSamples) maxSamples = samples;
    }
    if(!minSamples) minSamples = maxSamples;
    if(minSamples) {
        auto p(mixBuffer);
        auto divider(0UL);
        for(int i = 0; i < sourcesCount; i++) {
            if(sources[i]->activeCnt) divider++, sources[i]->activeCnt--;
        }
        if(divider) {
            for(int i = minSamples; i--; p++) p->left /= divider, p->right /= divider;
        }
        p = mixBuffer; auto o(audioBuffer);
        for(auto i = minSamples; i--; p++) {
            *(o++) = (u16)((long)p->left);// - 0x8000L);
            *(o++) = (u16)((long)p->right);// - 0x8000L);
        }
        player->setData((u8*)audioBuffer, minSamples << 2);
        player->play(true);
    }
    auto diffSamples(maxSamples - minSamples);
    if(maxSamples > minSamples) {
        memmove(mixBuffer, &mixBuffer[minSamples], diffSamples * sizeof(SAMPLER));
    }
    memset(&mixBuffer[diffSamples], 0x8000, (4096 - diffSamples) * sizeof(SAMPLER));
    for(int i = 0; i < sourcesCount; i++) sources[i]->nSamples -= minSamples;
}
