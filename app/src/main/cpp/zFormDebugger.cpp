//
// Created by User on 20.07.2023.
//

#include "sshCommon.h"
#include "zFormDebugger.h"
#include "zRibbonDebugger.h"
#include "emu/zDisAsm.h"
#include "zFormAssembler.h"

constexpr int TB_SPECCY         = 0;
constexpr int TB_CPU	        = 1;
constexpr int TB_GS		        = 2;

static zFormDebugger::Z_DEBUGGER registers[] = {
        { "F",	ZFV_OPS8, TB_CPU, R.id.textF,  R.id.editF,          RF,     0xFF, 0, 0},
        { "A",	ZFV_OPS8, TB_CPU, R.id.textA,  R.id.editA,          RA,     0xFF, 0, 0},
        { "C",	ZFV_OPS8, TB_CPU, R.id.textC,  R.id.editC,          RC,     0xFF, 0, 0},
        { "B",	ZFV_OPS8, TB_CPU, R.id.textB,  R.id.editB,          RB,     0xFF, 0, 0},
        { "E",	ZFV_OPS8, TB_CPU, R.id.textE,  R.id.editE,          RE,     0xFF, 0, 0},
        { "D",	ZFV_OPS8, TB_CPU, R.id.textD,  R.id.editD,          RD,     0xFF, 0, 0},
        { "L",	ZFV_OPS8, TB_CPU, R.id.textL,  R.id.editL,          RL,     0xFF, 0, 0},
        { "H",	ZFV_OPS8, TB_CPU, R.id.textH,  R.id.editH,          RH,     0xFF, 0, 0},
        { "F'",	ZFV_OPS8, TB_CPU, R.id.textF_, R.id.editF_,         RF_,    0xFF, 0, 0},
        { "A'",	ZFV_OPS8, TB_CPU, R.id.textA_, R.id.editA_,         RA_,    0xFF, 0, 0},
        { "C'",	ZFV_OPS8, TB_CPU, R.id.textC_, R.id.editC_,         RC_,    0xFF, 0, 0},
        { "B'",	ZFV_OPS8, TB_CPU, R.id.textB_, R.id.editB_,         RB_,    0xFF, 0, 0},
        { "E'",	ZFV_OPS8, TB_CPU, R.id.textE_, R.id.editE_,         RE_,    0xFF, 0, 0},
        { "D'",	ZFV_OPS8, TB_CPU, R.id.textD_, R.id.editD_,         RD_,    0xFF, 0, 0},
        { "L'",	ZFV_OPS8, TB_CPU, R.id.textL_, R.id.editL_,         RL_,    0xFF, 0, 0},
        { "H'",	ZFV_OPS8, TB_CPU, R.id.textH_, R.id.editH_,         RH_,    0xFF, 0, 0},
        { "XL",	ZFV_OPS8, TB_CPU, R.id.textXL, R.id.editXL,         RXL,    0xFF, 0, 0},
        { "XH",	ZFV_OPS8, TB_CPU, R.id.textXH, R.id.editXH,         RXH,    0xFF, 0, 0},
        { "YL",	ZFV_OPS8, TB_CPU, R.id.textYL, R.id.editYL,         RYL,    0xFF, 0, 0},
        { "YH",	ZFV_OPS8, TB_CPU, R.id.textYH, R.id.editYH,         RYH,    0xFF, 0, 0},

        { "AF",  ZFV_OPS16, TB_CPU, R.id.textAF,  R.id.editAF,      RF,     0xFFFF },
        { "BC",  ZFV_OPS16, TB_CPU, R.id.textBC,  R.id.editBC,      RC,     0xFFFF },
        { "DE",  ZFV_OPS16, TB_CPU, R.id.textDE,  R.id.editDE,      RE,     0xFFFF },
        { "HL",  ZFV_OPS16, TB_CPU, R.id.textHL,  R.id.editHL,      RL,     0xFFFF },
        { "AF'", ZFV_OPS16, TB_CPU, R.id.textAF_, R.id.editAF_,     RF_,    0xFFFF },
        { "BC'", ZFV_OPS16, TB_CPU, R.id.textBC_, R.id.editBC_,     RC_,    0xFFFF },
        { "DE'", ZFV_OPS16, TB_CPU, R.id.textDE_, R.id.editDE_,     RE_,    0xFFFF },
        { "HL'", ZFV_OPS16, TB_CPU, R.id.textHL_, R.id.editHL_,     RL_,    0xFFFF },
        { "IX",  ZFV_OPS16, TB_CPU, R.id.textIX,  R.id.editIX,      RXL,    0xFFFF },
        { "IY",  ZFV_OPS16, TB_CPU, R.id.textIY,  R.id.editIY,      RYL,    0xFFFF },

        { "I",	ZFV_OPS8,  TB_CPU, R.id.textI,  R.id.editI,         RI,     0xFF },
        { "R",	ZFV_OPS8,  TB_CPU, R.id.textR,	R.id.editR,         RR1,    0xFF },
        { "IM",	ZFV_SIMPLE,TB_CPU, R.id.textIM,	R.id.editIM,         IM,    0xFF },

        { "SP",	ZFV_OPS16, TB_CPU, R.id.textSP, R.id.editSP,        RSPL,   0xFFFF },
        { "PC",	ZFV_OPS16, TB_CPU, R.id.textPC, R.id.editPC,        RPCL,   0xFFFF },

        { "IFF", ZFV_SIMPLE, TB_CPU, R.id.textIFF, 0,               IFF1,   0xFF },
        { "DOS", ZFV_SIMPLE, TB_SPECCY, R.id.textDOS,  0,           STATE,	ZX_TRDOS, 1 },

        { "S",	ZFV_SIMPLE, TB_CPU, R.id.textFS,  0,                RF,     FS,  7 },
        { "Z",	ZFV_SIMPLE, TB_CPU, R.id.textFZ,  0,                RF,     FZ,  6 },
        { "Y",	ZFV_SIMPLE, TB_CPU, R.id.textFY,  0,                RF,     F5,  5 },
        { "H",	ZFV_SIMPLE, TB_CPU, R.id.textFH,  0,                RF,     FH,  4 },
        { "X",	ZFV_SIMPLE, TB_CPU, R.id.textFX,  0,                RF,     F3,  3 },
        { "P",	ZFV_SIMPLE, TB_CPU, R.id.textFP,  0,                RF,     FPV, 2 },
        { "N",	ZFV_SIMPLE, TB_CPU, R.id.textFN,  0,                RF,     FN,  1 },
        { "C",	ZFV_SIMPLE, TB_CPU, R.id.textFC,  0,                RF,     FC,  0 },

        { "",	ZFV_DECIMAL,TB_CPU, R.id.textTS,  R.id.editTS1,	    TACTS,	0xFF	   },
        { "TS",	ZFV_DECIMAL,TB_CPU, R.id.textTS,  R.id.editTS,	    FRAME0,	0xFFFFFFFF },
        // 47
        // 0
        { "cmd",ZFV_SIMPLE, TB_GS, R.id.textGsCmd, R.id.editGsCmd,	zCpuGs::gsCmd,	0xff },
        { "sts",ZFV_SIMPLE, TB_GS, R.id.textGsSts, R.id.editGsSts,	zCpuGs::gsSts,	0xff },
        { "dat",ZFV_SIMPLE, TB_GS, R.id.textGsDat, R.id.editGsDat,	zCpuGs::gsDat,	0xff },
        { "out",ZFV_SIMPLE, TB_GS, R.id.textGsOut, R.id.editGsOut,	zCpuGs::gsOut,	0xff },
        { "page",ZFV_SIMPLE,TB_GS, R.id.textGsPage,R.id.editGsPage,	zCpuGs::gsPage,	0xff },
        // 53
        { "7FFD",ZFV_BINARY, TB_SPECCY, R.id.text7FFD, R.id.edit7FFD, PORT_7FFD, 0xFF },
        { "1FFD",ZFV_BINARY, TB_SPECCY, R.id.text1FFD, R.id.edit1FFD, PORT_1FFD, 0xFF },
        { "7FFD",ZFV_BINARY, TB_SPECCY, R.id.textEFF7, R.id.editEFF7, PORT_EFF7, 0xFF },
        { "1FFD",ZFV_BINARY, TB_SPECCY, R.id.textDFDF, R.id.editDFDF, PORT_EXT1, 0xFF },
        // 55
};

int zFormDebugger::updateVisible(bool set) {
    if(set) stateTools(SD_TOOLS_ALL | SD_TOOLS_CH_16 | SD_TOOLS_CH_CPU);
    return zViewForm::updateVisible(set);
}

void zFormDebugger::fillRegisters(bool full) {
    for(auto& r : registers) {
        auto p(r.base);
        if(full) {
            r.text = idView<zViewText>(r.idText);
            r.edit = idView<zViewEdit>(r.idEdit);
            if(r.edit) {
                r.edit->setOnChangeText([this](zView *v, int act) {
                    if(act == MSG_EDIT_FINISH) stateTools(SD_TOOLS_CH_REG, v->id);
                });
            }
        }
        switch(r.baseType) {
            case TB_SPECCY: p = &speccy->flags; break;
            case TB_GS:		p = speccy->dev<zCpuGs>()->gs; break;
            case TB_CPU:    p = (u8*)&cpu->c; break;
        }
        r.base = p;
    }
}

void zFormDebugger::stateTools(int action, int id) {
    if(!speccy->showDebugger) return;
    if(action & SD_TOOLS_CH_CPU) {
        static int ids[] = { R.id.llGS, R.id.llPorts };
        idView(ids[!speccy->debugCpu])->updateStatus(ZS_VISIBLED, false);
        idView(ids[speccy->debugCpu])->updateStatus(ZS_VISIBLED, true);
        fillRegisters(false);
    }
    if(action & SD_TOOLS_CH_16) {
        static int ids[] = { R.id.ll8, R.id.ll16 };
        idView(ids[!speccy->showReg16])->updateVisible(false);
        idView(ids[speccy->showReg16])->updateVisible(true);
    }
    if(action & SD_TOOLS_BUT) {
        // надписи
        static cstr txtBut[] = { "DEC", "HEX", "8", "16", "GS", "CPU", "DA", "SP", "CM" };
        static int  idBut[]  = { R.id.butRadix, R.id.butSize, R.id.butCpu, R.id.butList,
                                 R.id.butStep, R.id.butStepIn, R.id.butStepOut };
        static int  vlBut[]  = { ZSI_SHOW_HEX, ZSI_SHOW_REG16, ZSI_DEBUG_CPU, ZSI_DEBUG_MODE };
        for(int i = 0 ; i < 4; i++) {
            auto v((u8)speccy->value(vlBut[i]));
            idView<zViewButton>(idBut[i])->setText(txtBut[i * 2 + v], true);
        }
        idView<zViewButton>(R.id.butPlay)->setIcon(speccy->execLaunch ? R.integer.iconZxPlay : R.integer.iconZxPause);
        // блокировка
        auto play(!speccy->execLaunch && (speccy->debugMode == MODE_PC));
        for(int i = 4; i < 7; i++) idView<zViewButton>(idBut[i])->updateStatus(ZS_DISABLED, !play);
    }
    if(action & SD_TOOLS_REG) {
        for(auto& reg : registers) {
            auto nval((int)((*(i32*)(reg.base + reg.offset) & reg.mask) >> reg.shift));
            reg.text->setTextColorForeground(nval != reg.value ? z.R.color.red : reg.text->getTextColorDefault());
            if(reg.edit) {
                reg.edit->setText(z_fmtValue(nval, reg.fmtValue, speccy->showHex), true);
            } else {
                reg.text->setTextStyle((nval != 0) * ZS_TEXT_BOLD_ITALIC);
            }
            reg.value = nval;
        }
    }
    if(action & SD_TOOLS_CH_REG) {
        // найти поле ввода для установки значения
        for(auto& reg : registers) {
            if(reg.idEdit != id) continue;
            auto nval((int)(z_ston(reg.edit->getText(), RADIX_DEC) & reg.mask));
            auto ptr(reg.base + reg.offset);
            nval = ((int)((*(i32*)ptr) & ~reg.mask) | nval);
            if(id == R.id.editPC || id == R.id.editSP) {
                if(id == R.id.editSP) {
                    _list->update(nval, SFLAG_SP);
                } else {
                    cpu->halt = 0;
                    _list->update(nval, SFLAG_PC);
                }
                if(!(action & SD_TOOLS_CH_SP_PC)) break;
            }
            *(u32*)ptr = nval;
            break;
        }
    }
    if(action & SD_TOOLS_LST) {
        if(action & SD_TOOLS_UPD_SREG) {
            switch(speccy->debugMode) {
                case MODE_SP: _list->update(cpu->sp, SFLAG_SP); break;
                case MODE_PC:
                    _list->update(cpu->pc, checkSTATE(ZX_TRACE) ? SFLAG_SEL : SFLAG_PC);
                    modifySTATE(0, ZX_TRACE)
                    break;
            }
        } else {
            _list->update(0, 0);
        }
    }
}

void zFormDebugger::onCommand(int id, bool dbl) {
    int action(0);
    switch(id) {
        case R.id.textFS: case R.id.textFZ:
        case R.id.textFY: case R.id.textFH:
        case R.id.textFX: case R.id.textFP:
        case R.id.textFN: case R.id.textFC:
            cpu->f ^= 1 << (R.id.textFC - id);
            action = SD_TOOLS_REG;
            break;
        case R.id.textIFF:
            cpu->iff[0] ^= 1;
            action = SD_TOOLS_REG;
            break;
        case R.id.listDebugger:
            if(dbl) action = SD_TOOLS_BUT; else _asm->setText(_list->cmdAsm, true);
            break;
        case R.id.butRadix:
            speccy->showHex ^= 1;
            action = SD_TOOLS_ALL;
            break;
        case R.id.butList:
            _list->setMode(speccy->debugMode + 1, cpu);
            action = SD_TOOLS_LST | SD_TOOLS_BUT;
            break;
        case R.id.butSize:
            speccy->showReg16 ^= 1;
            action = SD_TOOLS_REG | SD_TOOLS_BUT | SD_TOOLS_CH_16;
            break;
        case R.id.butCpu:
            speccy->debugCpu = !speccy->debugCpu;
            cpu = speccy->getCpu(speccy->debugCpu);
            action = SD_TOOLS_REG | SD_TOOLS_BUT | SD_TOOLS_CH_CPU | SD_TOOLS_LST | SD_TOOLS_UPD_SREG;
            break;
        case R.id.butStep:
        case R.id.butStepIn:
        case R.id.butStepOut:
            zDisAsm::trace(cpu, id - R.id.butStep);
        case R.id.butPlay:
            if(speccy->execLaunch) {
                action = SD_TOOLS_UPD_SREG | SD_TOOLS_LST | SD_TOOLS_REG;
            } else frame->send(ZX_MESSAGE_DEBUG, 0, speccy->debugCpu);
            action |= SD_TOOLS_BUT;
            if(id == R.id.butPlay) {
                // остановить/запустить звук
                //frame->send(ZX_MESSAGE_PROPS);
            }
            speccy->execLaunch ^= 1;
            break;
        case R.id.butBp:
            cpu->quickBP(_list->selItems[speccy->debugMode]);
            action = SD_TOOLS_LST;
            break;
        case R.id.butBpList:
            theApp->getForm(FORM_BPS)->updateVisible(true);
            action = SD_TOOLS_LST;
            break;
        case R.id.editAsm:
            auto data(_asm->getText());
            auto assem(theApp->getForm<zFormAssembler>(FORM_ASM));
            if(speccy->debugMode == MODE_PC) {
                auto pc(_list->selItems[MODE_PC]);
                auto ret(assem->parser(1, pc, data, nullptr));
                if(ret == 0) {
                    assem->copy(cpu, pc);
                    action = SD_TOOLS_LST;
                }
                else {
                    // если ошибка
                    zViewManager::showToast("Error Assembler");
                    _asm->setSelected(speccy->jni & 0xffff, speccy->jni >> 16);
                }
            } else if(speccy->debugMode == MODE_DT) {
                auto n(assem->quickParser(data));
                if(n >= 0) _list->update(n, SFLAG_DT | SFLAG_SEL);
            }
            break;
    }
    if(action) stateTools(action, id);
}

void zFormDebugger::onInit(bool _theme) {
    zViewForm::onInit(_theme);
    if(!isOpen) return;
    auto onClickButton = [this](zView* v, int dclk) { onCommand(v->id, dclk); };
    setOnClose([](zViewForm*, int code) { return 0; });
    static int ids[] = { R.id.butBp, R.id.butBpList, R.id.butStep, R.id.butStepIn, R.id.butStepOut,
                         R.id.butPlay, R.id.butRadix, R.id.butList, R.id.butSize, R.id.butCpu,
                         R.id.textFS, R.id.textFZ, R.id.textFY, R.id.textFH, R.id.textFX, R.id.textFP,
                         R.id.textFN, R.id.textFC };
    for(auto& id : ids) idView(id)->setOnClick(onClickButton);
    _list = idView<zRibbonDebugger>(R.id.listDebugger);
    _asm  = idView<zViewEdit>(R.id.editAsm);
    _asm->setOnChangeText([this](zView* v, int act) {
        if(act == MSG_EDIT_FINISH) onCommand(v->id, false);
    });
    cpu   = speccy->getCpu(speccy->debugCpu);
    fillRegisters(true);
}
