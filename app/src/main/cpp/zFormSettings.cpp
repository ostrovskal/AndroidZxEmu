//
// Created by User on 20.07.2023.
//

#include "sshCommon.h"
#include "zFormSettings.h"

class zFabricTapeItem : public zFabricListItem {
public:
    zFabricTapeItem() : zFabricListItem(styles_z_list_item) { }
    zView* make(zViewGroup* parent) override {
        auto v(new zLinearLayout(styles_default, 0, false));
        v->lps.set(0, 0, VIEW_MATCH, VIEW_WRAP);
        v->onInit(false);
        return v;
    }
};

class zAdapterTape : public zAdapterList {
public:
    zAdapterTape() : zAdapterList({}, new zFabricTapeItem()) { }
    zView* getView(int position, zView* convert, zViewGroup* parent) override {
        auto nv((zLinearLayout*)createView(position, convert, parent, fabricBase, false));
        return nv;
    }
};

class zFabricDiskItem : public zFabricListItem {
public:
    zFabricDiskItem() : zFabricListItem(styles_diskheadtext) { }
    zView* make(zViewGroup* parent) override {
        auto v(new zLinearLayout(styles_default, 0, false));
        for(int i = 0 ; i < 7; i++) v->attach(new zViewText(styles_diskheadtext, 0, 0),VIEW_MATCH, VIEW_MATCH);
        v->lps.set(0, 0, VIEW_MATCH, VIEW_WRAP);
        v->onInit(false);
        return v;
    }
};

class zAdapterDiskList : public zAdapterList {
public:
    zAdapterDiskList() : zAdapterList({}, new zFabricDiskItem()) { }
    zView* getView(int position, zView* convert, zViewGroup* parent) override {
        auto nv((zLinearLayout*)createView(position, convert, parent, fabricBase, false));
        auto txt(getItem(position).split("\n"));
        for(int i = 0 ; i < 7; i++) nv->atView<zViewText>(i)->setText(txt[i]);
        return nv;
    }
};

static u32 palettes_speccy[] = {
        // [sshEmu]
        0xFF000000, 0xFF2030C0, 0xFFC04010, 0xFFC040C0, 0xFF40B010, 0xFF50C0B0, 0xFFE0C010, 0xFFC0C0C0,
        0xFF000000, 0xFF3040FF, 0xFFFF4030, 0xFFFF70F0, 0xFF50E010, 0xFF50E0FF, 0xFFFFE850, 0xFFFFFFFF,
        // [Z80Stealth]
        0xFF000000, 0xFF0000C8, 0xFFC80000, 0xFFC800C8, 0xFF00C800, 0xFF00C8C8, 0xFFC8C800, 0xFFC8C8C8,
        0xFF000000, 0xFF0000FF, 0xFFFF0000, 0xFFFF00FF, 0xFF00FF00, 0xFF00FFFF, 0xFFFFFF00, 0xFFFFFFFF,
        // [Hacker KAY]
        0xFF000000, 0xFF202020, 0xFF303030, 0xFF404040, 0xFF505050, 0xFF606060, 0xFF707070, 0xFF808080,
        0xFF000000, 0xFF999999, 0xFFAAAAAA, 0xFFBBBBBB, 0xFFCCCCCC, 0xFFDDDDDD, 0xFFEEEEEE, 0xFFFFFFFF,
        // [Strange]
        0xFF000000, 0xFF2828C8, 0xFFC82828, 0xFFD048D0, 0xFF28C828, 0xFF48D0D0, 0xFFD0D048, 0xFFD4D4D4,
        0xFF000000, 0xFF3434F0, 0xFFF03434, 0xFFF850F8, 0xFF34F034, 0xFF50F8F8, 0xFFF8F850, 0xFFFFFFFF,
        // [ElectroScale]
        0xFF40404C, 0xFF4C505C, 0xFF606474, 0xFF707488, 0xFF80849C, 0xFF9498B4, 0xFFA4A8C8, 0xFFB4BCDC,
        0xFF40404C, 0xFF545464, 0xFF64687C, 0xFF787C94, 0xFF8C94AC, 0xFFA4A8C8, 0xFFB4BCFC, 0xFFC8D0F4,
        // [JPP]
        0xFF000000, 0xFF0000B4, 0xFFB40000, 0xFFB400B4, 0xFF00B400, 0xFF00B4B4, 0xFFB4B400, 0xFFB4B4B4,
        0xFF000000, 0xFF0000FF, 0xFFFF0000, 0xFFFF00FF, 0xFF00FF00, 0xFF00FFFF, 0xFFFFFF00, 0xFFFFFFFF,
        // [RealSpec]
        0xFF000000, 0xFF0000C8, 0xFFC80000, 0xFFC800C8, 0xFF00C800, 0xFF00C8C8, 0xFFC8C800, 0xFFC8C8C8,
        0xFFFF0000, 0xFF0000FF, 0xFFFF0000, 0xFFFF00FF, 0xFF00FF00, 0xFF00FFFF, 0xFFFFFF00, 0xFFFFFFFF };

static u32 palettes_asm[] = {
        // [DARK]
        0x00000000, 0x00505050, 0x00A0A0A0, 0x0000FFFF, 0x00FF00FF, 0x00FFFF00, 0x0000FF00, 0x00FFFFFF,
        0x00FFA070, 0x000000FF, 0x00FF8010, 0x00FFFFFF, 0x00303030, 0x00F0F0F0, 0x00404040, 0x00000000,
        // [WHITE]
        0x00EFEFEF, 0x00C0C0C0, 0x00000000, 0x00007F7F, 0x007F007F, 0x007F7F00, 0x00007F00, 0x007F7F7F,
        0x007F5060, 0x0000007F, 0x007F6010, 0x007F1010, 0x00303030, 0x00707070, 0x00404040, 0x00000000
};

static int ids[] = { // main
                     R.id.mainBorder, ZSI_SIZE_BORDER, R.id.mainJoystick, ZSI_SIZE_JOY, R.id.mainKeyboard, ZSI_SIZE_KEYB, R.id.mainSystem, ZSI_CPU_SPEED,
                     R.id.mainDarkTheme, ZSI_DARK_THEME, R.id.mainGsReset, ZSI_GS_RESET, R.id.mainTapeReset, ZSI_RESET_TAPE, R.id.mainAutoTape, ZSI_AUTO_TAPE,
                     R.id.mainSwapMouse, ZSI_SWAP_MOUSE, R.id.mainGigaScreen, ZSI_GIGA, R.id.mainTrapTRDOS, ZSI_TRAP_DOS, R.id.mainShowFPS, ZSI_FPS,
                     // sound
                     R.id.soundSpinChip, ZSI_AY_CHIP, R.id.soundSpinChannels, ZSI_AY_CHANNEL, R.id.soundSpinFreq, ZSI_SND_FREQ,
                     R.id.soundChkBeeper, ZSI_BP_LAUNCH, R.id.soundSlrBeeper, ZSI_BEEP_VOL,
                     R.id.soundChkAY, ZSI_AY_LAUNCH, R.id.soundSlrAY, ZSI_AY_VOL,
                     R.id.soundChkCovox, ZSI_COVOX_LAUNCH, R.id.soundSlrCovox, ZSI_COV_VOL,
                     R.id.soundChkGS, ZSI_GS_LAUNCH, R.id.soundSlrGS, ZSI_GS_VOL,
                     // joy
                     R.id.joySpinLyt, ZSI_TYPE_JOY, R.id.joySpinPresets, 0,
                     R.id.joySpinUp, ZSI_KEY_JOY_U, R.id.joySpinRight, ZSI_KEY_JOY_R, R.id.joySpinDown, ZSI_KEY_JOY_D, R.id.joySpinLeft, ZSI_KEY_JOY_L,
                     R.id.joySpinX, ZSI_KEY_JOY_X, R.id.joySpinY, ZSI_KEY_JOY_Y, R.id.joySpinA, ZSI_KEY_JOY_A, R.id.joySpinB, ZSI_KEY_JOY_B,
                     // display
                     R.id.dispSpinPalettes, ZSI_PALETTES, R.id.dispChkAsm, 0,
                     R.id.dispTextB, 113, R.id.dispTextR, 117, R.id.dispTextM, 121,
                     R.id.dispTextG, 125, R.id.dispTextC, 129, R.id.dispTextY, 133, R.id.dispTextW, 137,
                     R.id.dispTextBrBl, 141, R.id.dispTextBrB, 145, R.id.dispTextBrR, 149, R.id.dispTextBrM, 153,
                     R.id.dispTextBrG, 157, R.id.dispTextBrC, 161, R.id.dispTextBrY, 165, R.id.dispTextBrW, 169,
                     R.id.dispTextBkg, 173, R.id.dispTextSels, 177, R.id.dispTextText, 181, R.id.dispTextNumber, 185,
                     R.id.dispTextStrs, 189, R.id.dispTextSplits, 193, R.id.dispTextComment, 197, R.id.dispTextOps, 201,
                     R.id.dispTextCmds, 205, R.id.dispTextReg, 209, R.id.dispTextFlags, 213, R.id.dispTextLabels, 217,
                     R.id.dispTextBkgLines, 221, R.id.dispTextNumLines, 225, R.id.dispTextCurLine, 229, R.id.dispTextNulls, 233,
                     R.id.dispTextBl, 109,
                     R.id.dispSlrR, 0, R.id.dispSlrG, 0, R.id.dispSlrB, 0,
                     // casette
                     R.id.casetList, 0,
                     // disk
                     R.id.diskSpinDisk, 0, R.id.diskChkReadOnly, 0, R.id.diskList, 0,
                     // help
                     R.id.helpText, 0, 0, 0 };

i32 zFormSettings::updateVisible(bool set) {
    if(set) {
        memcpy(savedValues, (u8*)speccy, sizeof(savedValues));
        for(int i = 0 ; i < sizeof(ids) / 4; i += 2)
            onInit(idView(ids[i]), ids[i + 1]);
    }
    return zViewForm::updateVisible(set);
}

void zFormSettings::onInit(bool _theme) {
    zViewForm::onInit(_theme);
    if(isOpen) {
        for(int i = 0 ; i < sizeof(ids) / 4; i += 2) {
            auto vw(idView(ids[i]));
            if(dynamic_cast<zViewRibbon*>(vw)) {
                vw->setOnClick([this](zView* v, int sel) { onCommand(v, sel); });
            } else if(dynamic_cast<zViewSelect*>(vw)) {
                ((zViewSelect*)vw)->setOnChangeSelected([this](zView* v, int sel) { onCommand(v, sel); });
            } else if(dynamic_cast<zViewSlider*>(vw)) {
                ((zViewSlider*)vw)->setOnChangeSelected([this](zViewSlider* v, int pos) { onCommand(v, pos); return 0; });
            } else {
                vw->setOnClick([this](zView* v, int l) { onCommand(v, v->isChecked()); });
            }
        }
        setOnClose([this](zViewForm*, int code) {
            if(code == z.R.id.no) memcpy((u8*)speccy, savedValues, sizeof(savedValues));
            for(int i = 0 ; i < sizeof(ids) / 4; i += 2)
                onSave(idView(ids[i]), ids[i + 1], code == z.R.id.def);
            return code != z.R.id.def;
        });
    }
}

void zFormSettings::onCommand(zView* v, int a1) {
    zString8 tmp;
    auto id(v->id); int action(0), cmd(-1);
    auto offs(z_remap(id, ids));
    if(offs > 0 && offs < 109) *(u8*)((u8*)speccy + offs) = (u8)a1;
    switch(id) {
        case R.id.mainShowFPS: onInit(v, offs); break;
        case R.id.mainBorder: case R.id.mainSystem: cmd = ZX_MESSAGE_PROPS; break;
        case R.id.mainJoystick: action = ZFT_UPD_CONTROLLER; break;
        case R.id.mainKeyboard: action = ZFT_UPD_MENU_DBG; break;
        case R.id.mainDarkTheme: onInit(v, a1); break;
            // sound
        case R.id.soundChkBeeper: case R.id.soundChkAY:
        case R.id.soundChkCovox: case R.id.soundChkGS:
            onInit(v, offs);
        case R.id.soundSpinChip: case R.id.soundSpinChannels: case R.id.soundSpinFreq:
        case R.id.soundSlrBeeper: case R.id.soundSlrAY:
        case R.id.soundSlrCovox: case R.id.soundSlrGS:
            cmd = ZX_MESSAGE_PROPS;
            break;
        // joy
        case R.id.joySpinLyt: applyJoyStd(a1); break;
        case R.id.joySpinPresets: applyJoyPresets(a1); break;
        // display
        case R.id.dispChkAsm:
            onInit(v, a1);
        case R.id.dispSpinPalettes:
            applyPalette();
            break;
        case R.id.dispTextB: case R.id.dispTextR: case R.id.dispTextM: case R.id.dispTextG:
        case R.id.dispTextC: case R.id.dispTextY: case R.id.dispTextW: case R.id.dispTextBrBl:
        case R.id.dispTextBrB: case R.id.dispTextBrR: case R.id.dispTextBrM: case R.id.dispTextBrG:
        case R.id.dispTextBrC: case R.id.dispTextBrY: case R.id.dispTextBrW: case R.id.dispTextBkg:
        case R.id.dispTextSels: case R.id.dispTextText: case R.id.dispTextNumber: case R.id.dispTextStrs:
        case R.id.dispTextSplits: case R.id.dispTextComment: case R.id.dispTextOps: case R.id.dispTextCmds:
        case R.id.dispTextReg: case R.id.dispTextFlags: case R.id.dispTextLabels: case R.id.dispTextBkgLines:
        case R.id.dispTextNumLines: case R.id.dispTextCurLine: case R.id.dispTextNulls: case R.id.dispTextBl:
            onInit(v, offs);
            break;
        case R.id.dispSlrR: case R.id.dispSlrG: case R.id.dispSlrB:
            if(selColor) {
                auto c(&selColor->drw[DRW_BK]->color);
                id = id - R.id.dispSlrR; c->vec[id] = ((float)a1 / 255.0f);
                speccy->colors[(argColor - 109) / 4] = c->toABGR();
                selColor->invalidate();
            }
            break;
        // casette
        case R.id.casetList:
            break;
        // disk
        case R.id.diskSpinDisk: onInit(v, a1); break;
        case R.id.diskChkReadOnly:
            break;
    }
    if(cmd != -1) frame->send(cmd);
    if(action) frame->stateTools(action);
}

void zFormSettings::onInit(zView* v, int a1) {
    static int idsChk[] = { R.id.soundChkBeeper, R.id.soundSlrBeeper, R.id.soundChkAY, R.id.soundSlrAY,
                            R.id.soundChkCovox, R.id.soundSlrCovox, R.id.soundChkGS, R.id.soundSlrGS, 0, 0 };
    auto id(v->id);
    switch(id) {
        case R.id.mainBorder: case R.id.mainJoystick:
        case R.id.mainKeyboard: case R.id.mainSystem:
            ((zViewSlider*)v)->setProgress(speccy->value(a1));
            break;
        case R.id.mainShowFPS:
            frame->idView(R.id.speccyFps)->updateStatus(ZS_VISIBLED, speccy->value(a1));
        case R.id.mainDarkTheme:
            if(id == R.id.mainDarkTheme)
                theApp->setTheme(a1 ? styles_mythemedark : styles_mythemelight, resources_ptr_arrays, ::styles);
        case R.id.mainGsReset: case R.id.mainTapeReset: case R.id.mainAutoTape:
        case R.id.mainSwapMouse: case R.id.mainGigaScreen: case R.id.mainTrapTRDOS:
            ((zViewCheck*)v)->checked(speccy->value(a1));
            break;
            // sound
        case R.id.soundSpinChip: case R.id.soundSpinChannels: case R.id.soundSpinFreq:
            ((zViewSlider*)v)->setProgress(speccy->value(a1));
            break;
        case R.id.soundChkBeeper: case R.id.soundChkAY:
        case R.id.soundChkCovox: case R.id.soundChkGS:
            ((zViewCheck*)v)->checked(speccy->value(a1));
            idView(z_remap(id, idsChk))->disable(!speccy->value(a1));
            break;
        case R.id.soundSlrBeeper: case R.id.soundSlrAY:
        case R.id.soundSlrGS: case R.id.soundSlrCovox:
            ((zViewSlider*)v)->setProgress(speccy->value(a1));
            break;
            // joy
        case R.id.joySpinLyt:
            applyJoyStd(speccy->joyType);
            break;
        case R.id.joySpinPresets:
            speccy->joyMakePresets(id);
            applyJoyPresets(0);
            break;
        case R.id.joySpinY: case R.id.joySpinA: case R.id.joySpinB:
            ((zViewSelect*)v)->setItemSelected(speccy->joyKeys[id - R.id.joySpinY + 5]);
            break;
        // display
        case R.id.dispTextB: case R.id.dispTextR: case R.id.dispTextM: case R.id.dispTextG:
        case R.id.dispTextC: case R.id.dispTextY: case R.id.dispTextW: case R.id.dispTextBrBl:
        case R.id.dispTextBrB: case R.id.dispTextBrR: case R.id.dispTextBrM: case R.id.dispTextBrG:
        case R.id.dispTextBrC: case R.id.dispTextBrY: case R.id.dispTextBrW: case R.id.dispTextBkg:
        case R.id.dispTextSels: case R.id.dispTextText: case R.id.dispTextNumber: case R.id.dispTextStrs:
        case R.id.dispTextSplits: case R.id.dispTextComment: case R.id.dispTextOps: case R.id.dispTextCmds:
        case R.id.dispTextReg: case R.id.dispTextFlags: case R.id.dispTextLabels: case R.id.dispTextBkgLines:
        case R.id.dispTextNumLines: case R.id.dispTextCurLine: case R.id.dispTextNulls:
        case R.id.dispTextBl:
            if(selColor) {
                selColor->drw[DRW_BK]->tile = z.R.integer.rect;
                selColor->drw[DRW_BK]->measure(0, 0, PIVOT_ALL, true);
            }
            selColor = v; argColor = a1;
            selColor->drw[DRW_BK]->tile = z.R.integer.ellips;
            selColor->drw[DRW_BK]->measure(0, 0, PIVOT_ALL, true);
            applyColorSlider(a1);
            break;
        case R.id.dispChkAsm: {
            idView<zLinearLayout>(a1 ? R.id.llSpeccyCols1 : R.id.llAsmCols1)->updateVisible(false);
            idView<zLinearLayout>(a1 ? R.id.llSpeccyCols2 : R.id.llAsmCols2)->updateVisible(false);
            idView<zLinearLayout>(a1 ? R.id.llAsmCols1 : R.id.llSpeccyCols1)->updateVisible(true);
            idView<zLinearLayout>(a1 ? R.id.llAsmCols2 : R.id.llSpeccyCols2)->updateVisible(true);
            auto adapt(idView<zViewSelect>(R.id.dispSpinPalettes)->getAdapter());
            adapt->clear(false);
            adapt->addAll(theme->findArray(a1 ? R.string.palette_asm : R.string.palette_speccy));
        }
        case R.id.dispSpinPalettes:
            applyPalette();
            break;
            // casette
        case R.id.casetList:
            ((zViewRibbon*)v)->setAdapter(new zAdapterTape());
            break;
        // disk
        case R.id.diskSpinDisk: {
            zString8 tmp;
            ((zViewSelect*)v)->setItemSelected(a1);
            speccy->diskOperation(ZX_DISK_OPS_GET_READONLY, a1, tmp);
            idView<zViewText>(R.id.diskTextFile)->setText(z_trimName(tmp, true));
            // установить секторы
            makeDiskCatalog(a1);
        }
            break;
        case R.id.diskList:
            ((zViewRibbon*)v)->setAdapter(new zAdapterDiskList());
            break;
        // help
        case R.id.helpText:
            ((zViewText*)v)->setOnClickUrl([](zView*, czs& link) {
                DLOG("link %s", link.str());
            });
            break;
    }
}

void zFormSettings::onSave(zView* v, int a1, bool def) {
    auto id(v->id);
    switch(id) {
        case R.id.mainBorder: break;
        case R.id.mainJoystick: break;
        case R.id.mainKeyboard: break;
        case R.id.mainSystem: break;
        case R.id.mainDarkTheme: break;
        case R.id.mainGsReset: break;
        case R.id.mainTapeReset: break;
        case R.id.mainAutoTape: break;
        case R.id.mainSwapMouse: break;
        case R.id.mainGigaScreen: break;
        case R.id.mainTrapTRDOS: break;
        case R.id.mainShowFPS: break;
            // sound
        case R.id.soundSpinChip: break;
        case R.id.soundSpinChannels: break;
        case R.id.soundSpinFreq: break;
        case R.id.soundChkBeeper: break;
        case R.id.soundSlrBeeper: break;
        case R.id.soundChkAY: break;
        case R.id.soundSlrAY: break;
        case R.id.soundChkCovox: break;
        case R.id.soundSlrCovox: break;
        case R.id.soundChkGS: break;
        case R.id.soundSlrGS: break;
            // joy
        case R.id.joySpinLyt: break;
        case R.id.joySpinPresets: break;
        case R.id.joySpinUp: break;
        case R.id.joySpinRight: break;
        case R.id.joySpinDown: break;
        case R.id.joySpinLeft: break;
        case R.id.joySpinX: break;
        case R.id.joySpinY: break;
        case R.id.joySpinA: break;
        case R.id.joySpinB: break;
            // display
        case R.id.dispSpinPalettes: break;
        case R.id.dispChkAsm: break;
            // casette
        case R.id.casetList: break;
            // disk
        case R.id.diskSpinDisk:
            break;
        case R.id.diskChkReadOnly: break;
        case R.id.diskList: break;
            // help
        case R.id.helpText: break;
    }
}

void zFormSettings::applyPalette() {
    static int idsTxt[] = { R.id.dispTextBl, R.id.dispTextB, R.id.dispTextR, R.id.dispTextM, R.id.dispTextG, R.id.dispTextC, R.id.dispTextY,
                            R.id.dispTextW, R.id.dispTextBrBl, R.id.dispTextBrB, R.id.dispTextBrR, R.id.dispTextBrM, R.id.dispTextBrG,
                            R.id.dispTextBrC, R.id.dispTextBrY, R.id.dispTextBrW, R.id.dispTextBkg, R.id.dispTextSels,
                            R.id.dispTextText, R.id.dispTextNumber, R.id.dispTextStrs, R.id.dispTextSplits, R.id.dispTextComment,
                            R.id.dispTextOps, R.id.dispTextCmds, R.id.dispTextReg, R.id.dispTextFlags, R.id.dispTextLabels,
                            R.id.dispTextBkgLines, R.id.dispTextNumLines, R.id.dispTextCurLine, R.id.dispTextNulls };
    bool chkAsm(idView(R.id.dispChkAsm)->isChecked());
    int num(idView<zViewSelect>(R.id.dispSpinPalettes)->getSelectedItem());
    if(num) {
        num--; num *= 16;
        auto pal(chkAsm ? &palettes_asm[num] : &palettes_speccy[num]);
        for(int i = 0 ; i < 16; i++) {
            idView(idsTxt[i + chkAsm * 16])->drw[DRW_BK]->color = pal[i];
        }
    }
}

void zFormSettings::makeDiskCatalog(int num) {
    // catalog sectors
    static int offsets[] = { 0, 8, 9, 11, 13, 14, 15 };
    int idx(0);
    auto adapt(idView<zViewRibbon>(R.id.diskList)->getAdapter());
    adapt->clear(false);
    for(int i = 0 ; i < 8; i++) {
        if(!zxCmd(ZX_CMD_DISK_OPS, ZX_DISK_OPS_RSECTOR, num | (i << 3))) continue;
        // records
        for(int r = 0 ; r < 16; r++) {
            // columns
            zString8 rec;
            auto buf(speccy->tmpBuf + (r << 4)); auto marker(*buf);
            if(marker < 32) { if(marker == 0) return; continue; }
            for(int c = 0; c < 7; c++) {
                zString8 val; int n(*(u16*)(buf + offsets[c]));
                switch(c) {
                    case 0: val = zString8(buf, 8); break;
                    case 1:	n &= 0xff; val = (cstr)&n; break;
                    case 2:
                    case 3: val = z_fmtValue(n, ZFV_SIMPLE, false); break;
                    default: val = z_fmtValue((u8)n, ZFV_SIMPLE, false); break;
                }
                rec.appendNotEmpty(val, "\n");
            }
            adapt->add(rec); idx++;
        }
    }
}

static int idsJSpin[] = { R.id.joySpinUp, R.id.joySpinRight, R.id.joySpinDown, R.id.joySpinLeft,
                          R.id.joySpinX, R.id.joySpinY, R.id.joySpinA, R.id.joySpinB };

void zFormSettings::applyJoyStd(int num) {
    auto keys(theme->findArray(R.string.key_names)); auto is(num != 4);
    auto arr(zString8(stdJoyKeys[num]).split(","));
    for(int i = 0 ; i < 5; i++) {
        auto sel(idView<zViewSelect>(idsJSpin[i]));
        if(is) {
            auto idx(keys.indexOf(arr[i]));
            if(idx != -1) sel->setItemSelected(idx);
        }
        sel->disable(is);
    }
}

void zFormSettings::applyJoyPresets(int num) {
    auto j(speccy->findJoyPokes(num)); num = j->joy.type;
    idView<zViewSelect>(R.id.joySpinLyt)->setItemSelected(num);
    for(int i = 0 ; i < 8; i++) {
        auto sel(idView<zViewSelect>(idsJSpin[i]));
        sel->setItemSelected(j->joy.keys[i]);
        sel->disable(num != 4 && i < 5);
    }
}

void zFormSettings::applyColorSlider(int num) {
    zColor color(speccy->colors[(num - 109) / 4]);
    idView<zViewSlider>(R.id.dispSlrR)->setProgress(z_round(color.b * 255.0f));
    idView<zViewSlider>(R.id.dispSlrG)->setProgress(z_round(color.g * 255.0f));
    idView<zViewSlider>(R.id.dispSlrB)->setProgress(z_round(color.r * 255.0f));
}
