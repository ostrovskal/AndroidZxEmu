//
// Created by User on 22.07.2023.
//

#include "sshCommon.h"
#include "zRibbonDebugger.h"
#include "emu/zDisAsm.h"
#include "zFormDebugger.h"

class zAdapterDbgList : public zAdapterList {
public:
    explicit zAdapterDbgList() : zAdapterList({"123"}, new zFabricListItem(styles_deblisttext)) { }
    [[nodiscard]] int getCount() const override { return 65535; }
    zView* getView(int position, zView* convert, zViewGroup* parent) override {
        auto nv(convert);
        if(!nv) nv = fabricBase->make(parent);
        nv->id = position;
        auto rb(((zRibbonDebugger*)parent));
        if(position < rb->items.size()) rb->initItem((zViewText*)nv, position);
        return nv;
    }
};

zRibbonDebugger::zRibbonDebugger(zStyle *_styles, int _id, bool _vert) : zViewRibbon(_styles, _id, _vert) {
    memset(entries, 0, sizeof(entries));
    memset(selItems, 0, sizeof(selItems));
}

void zRibbonDebugger::onInit(bool _theme) {
    zViewRibbon::onInit(_theme);
    setAdapter(new zAdapterDbgList());
}

bool zRibbonDebugger::correctSP() {
    auto entry(entries[MODE_SP]);
    auto delta(entry % 2);
    auto count(items.size() * 2);
    if(entry < 0) entry = -delta;
    else if((entry + count) > 65535) entry = (65536 + delta) - count;
    auto ret(entries[MODE_SP] != entry);
    entries[MODE_SP] = entry; return ret;
}

bool zRibbonDebugger::correctDT() {
    auto _count(countData + (countData == 0));
    auto entry(entries[MODE_DT]);
    auto delta(entry % _count);
    auto count(items.size() * _count);
    if(entry < 0) entry = delta;
    else if((entry + count) > 65535) entry = 65536 - count;
    auto ret(entries[MODE_DT] != entry);
    entries[MODE_DT] = entry; return ret;
}

bool zRibbonDebugger::scrolling(int delta) {
    auto pos(-delta / sizeTouch.h); bool ret(false);
    switch(speccy->debugMode) {
        case MODE_PC: entries[MODE_PC] = zDisAsm::movePC(entries[MODE_PC], pos, items.size()); break;
        case MODE_SP: entries[MODE_SP] += pos * 2; ret = correctSP(); break;
        case MODE_DT: entries[MODE_DT] += pos * countData; ret = correctDT(); break;
    }
    update(0, 0);
    return ret;
}

void zRibbonDebugger::onLayout(crti &position, bool changed) {
    if(!countCols) {
        auto tv((zViewText*)cacheViews[0]);
        if(tv) {
            auto wc(tv->getWidthText("1"));
            countCols = rclient.w / wc;
            items.resize(z_round((float)rclient.h / (float)tv->rview.h) + 1);
        }
    }
    zViewRibbon::onLayout(position, changed);
}

void zRibbonDebugger::setMode(int m, zCpu* cpu) {
    if(m > MODE_DT) m = MODE_PC;
    speccy->debugMode = m;
    auto flags(0), data(0);
    switch(m) {
        case MODE_PC: data = cpu->pc; flags = SFLAG_PC; break;
        case MODE_SP: data = cpu->sp; flags = SFLAG_SP; break;
        case MODE_DT: data = entries[MODE_DT]; flags = SFLAG_DT; break;
    }
    update(data, flags);
}

void zRibbonDebugger::update(int data, int flags) {
    if(flags & SFLAG_SP) {
        entries[MODE_SP] = data - (items.size() & (~1));
        correctSP(); selItems[MODE_SP] = data;
    }
    if(flags & SFLAG_DT) {
        entries[MODE_DT] = data;
        correctDT(); selItems[MODE_DT] = data;
    }
    if(flags & SFLAG_PC) {
        entries[MODE_PC] = data;
        selItems[MODE_PC] = data;
    }
    if(flags & SFLAG_SEL) {
        auto mode(speccy->debugMode);
        selItems[mode] = data;
        // найти в массиве адресов
        int i(0), ii(items.size() / 2);
        for(; i < items.size(); i++) {
            if(items[i] == data) {
                if(i > ii) entries[mode] = items[i - ii];
                break;
            }
        }
        if(i >= items.size() && mode != MODE_SP)
            entries[mode] = data;
    }
    selectItem = clickItem = -1;
    requestPosition();
}

u16 zRibbonDebugger::itemDT(zCpu* cpu, u16 entry, char* buf) {
    // address(5) + sep(4) + data + string
    countData = ((countCols - 10) / 5);
    auto _buf(buf); zString8 tmp;
    // адрес
    z_strcpy(&buf, z_fmtValue(entry, ZFV_OPS16, speccy->showHex));
    z_char(&buf, ' ', 3);
    for(int i = 0; i < countData ; i++) {
        auto v(cpu->_rm8((u16)(entry + i)));
        z_strcpy(&buf, z_fmtValue((int)v, ZFV_OPS8, speccy->showHex));
        *buf++ = ' '; int vv(v < 32 ? (int)'.' : (int)z_encode8(v)); tmp += (cstr)&vv;
    }
    z_char(&buf, ' ', 2);
    speccy->jni = (u32)(buf - _buf);
    z_strcpy(&buf, tmp.str());
    *buf = 0; return entry + countData;
}

u16 zRibbonDebugger::itemSP(zCpu* cpu, u16 entry, char* buf) const {
    // stack addr       >content<     chars
    auto _buf(buf); auto isSP(entry == cpu->sp); auto count(5 - isSP);
    auto length(countCols - (12 + count * 2 + isSP * 2));
    zString8 tmp;
    // адрес
    z_strcpy(&buf, z_fmtValue(entry, ZFV_OPS16, speccy->showHex));
    // содержимое
    z_char(&buf, ' ', count); if(isSP) *buf++ = '>';
    auto addr(cpu->rm16(entry)); entry += 2;
    z_strcpy(&buf, z_fmtValue(addr, ZFV_OPS16, speccy->showHex));
    if(isSP) *buf++ = '<'; z_char(&buf, ' ', count);
    // байты по содержимому
    for(int i = 0; i < length; i++) {
        auto v(cpu->_rm8((u16)(addr + i)));
        int vv(v < 32 ? (int)'.' : (int)z_encode8(v)); tmp += (cstr)&vv;
    }
    speccy->jni = (u32)(buf - _buf);
    z_strcpy(&buf, tmp.str());
    *buf = 0; return entry;
}

u16 zRibbonDebugger::itemPC(zCpu* cpu, u16 entry, char* buf) {
    // disassembler
    int flags((speccy->showDebugCode * DA_CODE) | (speccy->showDebugContent * (DA_PN | DA_PNN)) | (speccy->showDebugAddress * DA_PC));
    auto ret(zDisAsm::parser(cpu, entry, buf, flags));
    if(selItems[MODE_PC] == entry) {
        cmdAsm = (buf + speccy->jni);
        // убрать из команды [XXXX]{XXXX}
        cmdAsm = cmdAsm.substrBefore("[", cmdAsm) + cmdAsm.substrAfter("]");
        cmdAsm = cmdAsm.substrBefore("{", cmdAsm) + cmdAsm.substrAfter("}");
        theApp->getForm<zFormDebugger>(FORM_DEBUG)->onCommand(R.id.listDebugger, false);
    }
    return ret;
}

void zRibbonDebugger::notifyEvent(HANDLER_MESSAGE *msg) {
    if(msg->what == MSG_SELECTED && selectItem != -1) {
        auto idx(items[selectItem]);
        if(touch->isDblClicked()) {
            // jump
            auto cpu(speccy->getCpu(speccy->debugCpu));
            auto mode(zDisAsm::jump(cpu, (u16)idx, speccy->debugMode, true));
            if(mode != -1) {
                auto jmp((u16)((speccy->jni >> 16) & 0xffff));
                setMode(mode, cpu); entries[mode] = jmp; selItems[mode] = jmp;
                theApp->getForm<zFormDebugger>(FORM_DEBUG)->onCommand(R.id.listDebugger, true);
            }
        } else {
            selItems[speccy->debugMode] = idx;
        }
        update(0, 0);
    } else {
        zViewRibbon::notifyEvent(msg);
    }
}

void zRibbonDebugger::initItem(zViewText* nv, int idx) {
    auto mode(speccy->debugMode); u16 entry, ret;
    if(!idx) { entry = entries[mode]; items[0] = entry; } else entry = items[idx];
    auto cpu(speccy->getCpu(speccy->debugCpu)); auto buf((char*)&tmpBuf[INDEX_DA]);
    switch(mode) {
        case MODE_PC: ret = itemPC(cpu, entry, buf); break;
        case MODE_DT: ret = itemDT(cpu, entry, buf); break;
        case MODE_SP: ret = itemSP(cpu, entry, buf); break;
    }
    if(selItems[mode] == entry) selectItem = clickItem = idx;
    items[idx + 1] = ret; nv->setText(buf);
    int len, x(0), lex(0); auto ln(buf); auto isAddr(speccy->showDebugAddress);
    while((len = getLexem(&ln, x, lex))) {
        if(ln < (buf + speccy->jni)) {
            if(isAddr && ln < (buf + 6)) {
                lex = 13;
                nv->setSpan(new zTextSpanBackgroundColor(speccy->colors[COLOR_NUM_BKG]), x, x + len);
            } else lex = LK;
        } else if(speccy->debugMode != MODE_PC) lex = LQ;
        else if(lex == LW) lex = (int)parseKeyword(len, nullptr);
        nv->setSpan(new zTextSpanForegrounColor(speccy->colors[cnvLex[lex]]), x, x + len);
        x += len;
    }
    nv->updateText();
}
