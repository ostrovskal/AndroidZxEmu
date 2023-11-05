//
// Created by User on 01.08.2023.
//

#include "sshCommon.h"

int z_extension(zString8 name) {
    name = name.substrAfterLast(".", name).lower();
    return (name[1] == '$' ? ZX_FMT_HOBC : name.indexOf(extValid, 18));
}

u8* z_openFile(zFile* fl, int index, int* size, zString8& name) {
    u8* ptr(nullptr); zFile::zFileInfo zfi{};
    if(fl->info(zfi, index)) {
        if(size) *size = (int)zfi.usize; name = zfi.path;
        auto ext(z_extension(name)); bool success(true);
        if(ext == ZX_FMT_CSW || ext == ZX_FMT_WAV) {
            if(fl->isZip()) {
                // сформировать полный путь к кэшу
                name = settings->makePath(name.substrAfterLast("/", name), FOLDER_CACHE);
                // распаковать файл в папку кэша
                success = fl->copy(name, zfi.index);
            }
            if(success) ptr = (u8*)name.str();
        } else {
            ptr = (u8*)fl->readn(nullptr, *size, zfi.index);
        }
    }
    return ptr;
}

bool z_saveFile(zString8 path, void* ptr, int size, bool zipped) {
    zFile fl;
    if(fl.open(zipped ? path.substrBeforeLast(".") + ".zip" : path, false)) {
        fl.write(ptr, size, path.substrAfterLast("/", path));
        fl.close();
        return true;
    }
    return false;
}

static void z_packSegment(u8** dst, u32 count, u8 block) {
    auto dest(*dst);
    if(count == 1) {
        *dest++ = block;
    } else if(count < 5 && block != 0xed) {
        z_memset(dst, block, count); dest += count;
    } else {
        if(*(dest - 1) == 0xed) *dest++ = block, count--;
        *dest++ = 0xed; *dest++ = 0xed;
        *dest++ = (u8)count; *dest++ = block;
    }
    *dst = dest;
}

// распаковка блока памяти (с учетом или без завершающей сигнатуры)
bool z_unpackBlock(u8* src, u8* dst, const u8* dstE, size_t sz, bool packed) {
    u8 bp(0);
    while(sz > 0 && dst < dstE) {
        auto b(*src++); sz--;
        if(packed && b == 0xED && bp == 0xED) {
            auto count(*src++), byte(*src++); sz -= 2; b = 0;
            --dst; z_memset(&dst, byte, count);
        } else *dst++ = b;
        bp = b;
    }
    return dst == dstE;
}

// упаковка блока памяти (с учетом или без завершающей сигнатуры)
u8* z_packBlock(u8* src, const u8* srcE, u8* blk, bool sign, u32& newSize) {
    auto dst(blk); u8 block(0); u32 count(0);
    while(src < srcE) {
        block = *src++; count++;
        if(count == 255 || block != *src) { z_packSegment(&dst, count, block); count = 0; }
    }
    if(count) z_packSegment(&dst, count, block);
    if(sign) { *(u32*)dst = 0x00eded00; dst += 4; }
    newSize = (u32)(dst - blk);
    return dst;
}

int zxCmd(int cmd, int arg1, int arg2, cstr arg3) {
    int ret(0); zString8 str(arg3);
    switch(cmd) {
        case ZX_CMD_POKE:   speccy->dev<zCpuMain>()->_wm8((u16)arg1, (u8)arg2); break;
        case ZX_CMD_PROPS:  speccy->update(ZX_UPDATE_STATE, 0); break;
        case ZX_CMD_MODEL:  speccy->update(ZX_UPDATE_MODEL, arg1); break;
        case ZX_CMD_RESET:  speccy->update(ZX_UPDATE_RESET, 0); break;
        case ZX_CMD_DISK_OPS: ret = speccy->diskOperation(arg1, arg2, str); break;
        case ZX_CMD_TAPE_UI: speccy->dev<zDevTape>()->execUI(arg1); break;
        case ZX_CMD_MAGIC:  speccy->dev<zCpuMain>()->reqNMI = true; break;
    }
    return ret;
}

int getLexem(char** text, int x, int& lex) {
    auto buf((char*)&tmpBuf[INDEX_LEXEM]);
    auto txt(*text), tbuf(buf); u8 ch(*txt), quot(ch);
    auto lx(zsym[ch]); lex = lx;
    // табуляция+, комментарии+, буквы/цифры+, строки+
    while(*txt) {
        ch = *txt;
        if(lx != LK) {
            auto len(tbuf - buf);
            if(lx == LO && (ch == '%' || ch == '#')) {
                // проверить на HEX, BIN
                auto end(txt);
                z_ston(txt, RADIX_DEC, &end);
                auto l((int)(end - txt));
                if(l > 1 && len > 0) break;
                z_memcpy((u8**)&tbuf, txt, l);
                txt += l; x += l; lex = LD;
                break;
            }
            auto lxx(zsym[ch]);
            if(lx == LQ) {
                if(lxx == LQ && ch == quot && len > 0) {
                    *tbuf++ = quot; txt++;
                    break;
                }
            } else if(lxx != lx) {
                if(lx > LW || lxx > LW) break;
                if(lx == LD) lex = LN;
            }
        }
        if(ch == '\t') {
            auto dx(3 - (x & 3)); ch = ' ';
            tbuf = (char*)memset(tbuf, ch, dx);
            x += dx;
        }
        *tbuf++ = ch; x++; txt++;
    }
    *text = txt;
    return (int)(tbuf - buf);
}

i32 parseKeyword(int len, zArray<Z_LABEL>* labels) {
    static int oldIdx(LN);
    auto tbuf((char*)&tmpBuf[INDEX_LEXEM]);
    int i, idx = -1, lex(LN); const char* name;
    while((name = namesCode[++idx])) {
        for(i = 0 ; i < len; i++) {
            auto ch(tbuf[i] & -33);
            if(ch != name[i]) break;
        }
        if(i == len && name[i] <= ' ') {
            if(idx == C_C) {
                lex = (oldIdx >= C_JP && oldIdx <= C_RET) + L3;
                break;
            }
            if(idx >= C_ORG)     lex = L1;
            else if(idx >= C_JP) lex = L2;
            else if(idx < C_FNZ) lex = L3;
            else if(idx < C_JP)  lex = L4;
            break;
        }
    }
    if(lex == LN && labels) {
        // label
        for(int j = 0 ; j < labels->size(); j++) {
            name = labels->at(j).name;
            for(i = 0 ; i < len; i++) {
                if(tbuf[i] != name[i]) break;
            }
            if(i == len && name[i] < 33) {
                if(labels->at(j).type != L_UNDEF) {
                    lex = L5; idx = -1;
                    break;
                }
            }
        }
    }
    oldIdx = idx;
    return lex;
}

void z_chkGet(u8** p, u8* v, int s) {
    auto chk(*(*p++)), ch((u8)0);
    switch(s) {
        case 1: *v = *(*p); break;
        case 2: *v = *(u16*)(*p); break;
        case 4: *v = *(u32*)(*p); break;
        case 8: *v = *(u64*)(*p); break;
        default: memcpy(v, *p, s); break;
    }
    // checked
    for(int i = 0 ; i < s; i++) ch += *v++;
    if(chk == ch) (*p) += s;
}

void z_chkSet(u8** p, u8* v, int s) {
    // checked
    u8 ch(0); for(int i = 0 ; i < s; i++) ch += *(p[i]);
    *(*p++) = ch;
    switch(s) {
        case 1: *(*p++) = *v; break;
        case 2: wordLE(p, *v); break;
        case 4: dwordLE(p, *v); break;
        case 8: qwordLE(p, *v); break;
        default: z_memcpy(p, v, s); break;
    }
}
