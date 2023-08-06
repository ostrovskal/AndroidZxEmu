//
// Created by User on 20.07.2023.
//

#include "sshCommon.h"
#include "zSpeccyKeyboard.h"

// fe fd fb f7 ef df bf 7f
//	0xfefe// CS Z X C V
//	0xfdfe// A S D F G
//	0xfbfe// Q W E R T
//	0xf7fe// 1 2 3 4 5
//	0xeffe// 0 9 8 7 6
//	0xdffe// P O I U Y
//	0xbffe// ENTER L K J H
//	0x7ffe// SPACE SS M N B
// DEL fefe-254,effe-254,bffe-254

enum NameKeys { NK_SPC = 20, NK_NA, NK_SSP, NK_SS, NK_ENTER, NK_inv, NK_true, NK_cc, NK_gg, NK_edit,
                NK_BLUE, NK_RED, NK_MAGENTA, NK_GREEN, NK_CYAN, NK_YELLOW, NK_WHITE, NK_bright, NK_BLACK,
                NK_blue, NK_red, NK_magenta, NK_green, NK_cyan, NK_yellow, NK_white, NK_flash, NK_black,
                NK_DEF_FN, NK_FN, NK_LINE, NK_OPEN_, NK_CLOSE_, NK_MOVE, NK_ERASE, NK_POINT, NK_CAT, NK_FORMAT,
                NK_0, NK_1, NK_2, NK_3, NK_4, NK_5, NK_6, NK_7, NK_8, NK_9,
                NK_A, NK_B, NK_C, NK_D, NK_E, NK_F, NK_G, NK_H, NK_I, NK_J, NK_K, NK_L, NK_M,
                NK_N, NK_O, NK_P, NK_Q, NK_R, NK_S, NK_T, NK_U, NK_V, NK_W, NK_X, NK_Y, NK_Z,
                NK_a, NK_b, NK_c, NK_d, NK_e, NK_f, NK_g, NK_h, NK_i, NK_j, NK_k, NK_l, NK_m,
                NK_n, NK_o, NK_p, NK_q, NK_r, NK_s, NK_t, NK_u, NK_v, NK_w, NK_x, NK_y, NK_z,
                NK_PLOT, NK_DRAW, NK_REM, NK_RUN, NK_RAND, NK_RET, NK_IF, NK_INPUT, NK_POKE, NK_PRINT,
                NK_NEW, NK_SAVE, NK_DIM, NK_FOR, NK_GOTO, NK_GOSUB, NK_LOAD, NK_LIST, NK_LET,
                NK_COPY, NK_CLEAR, NK_CONT, NK_CLS, NK_BORDER, NK_NEXT, NK_PAUSE,
                NK_SIN, NK_COS, NK_TAN, NK_INT, NK_RND, NK_STR$, NK_CHR$, NK_CODE, NK_PEEK, NK_TAB,
                NK_READ, NK_RESTORE, NK_DATA, NK_SGN, NK_ABS, NK_SQR, NK_VAL, NK_LEN, NK_USR, NK_LN,
                NK_EXP, NK_LPRINT, NK_LLIST, NK_BIN, NK_INKEY$, NK_PI,
                NK_ASN, NK_ACS, NK_ATN, NK_VERIFY, NK_MERGE, NK_IN, NK_OUT, NK_BEEP, NK_INK, NK_PAPER,
                NK_FLASH, NK_BRIGHT, NK_OVER, NK_INVERSE, NK_CIRCLE, NK_VAL$, NK_SCREEN$, NK_ATTR,
                NK_STOP, NK_NOT, NK_STEP, NK_TO, NK_THEN, NK_FUNT, NK_DEL, NK_AND, NK_OR, NK_AT,
                // 200
                NK_QUESTION, NK_COMMA, NK_PT1, NK_PT_COMMA, NK_QUOTE2, NK_QUOTE1, NK_SLASH, NK_MUL, NK_DIV, NK_DOLLAR,
                NK_LSR, NK_RSR, NK_LBR, NK_RBR, NK_ANPENDANS, NK_PERCENT, NK_ADDR, NK_EQ, NK_PLUS, NK_MINUS, NK_PT2,
                NK_QNOT, NK_LS, NK_GT, NK_QLS, NK_QGT, NK_ZNAK, NK_NUM, NK_TILDA, NK_UNDERLINE, NK_or, NK_EXT, NK_EXTP };

cstr keyNames[]   = { "iconZxG00", "iconZxG01", "iconZxG02", "iconZxG03", "iconZxG04", "iconZxG05", "iconZxG06", "iconZxG07",
                      "iconZxG08", "iconZxG09", "iconZxG10", "iconZxG11", "iconZxG12", "iconZxG13", "iconZxG14", "iconZxG15",
                      "iconZxArrowLeft", "iconZxArrowDown", "iconZxArrowUp", "iconZxArrowRight",
                      // 20
                      "[ ]", "N/A", "[iconZxShiftPress]", "[iconZxShift]", "[iconZxOk]", "inv", "true", "[CAPS]", "[GRAPH]", "[EDIT]",
                      "#BLUE#iconZxG15", "#RED#iconZxG15", "#PURPLE#iconZxG15", "#GREEN#iconZxG15", "#CYAN#iconZxG15",
                      "#YELLOW#iconZxG15", "#WHITE#iconZxG15", "BRIGHT", "#BLACK#iconZxG15",
                      "#BLUE#iconZxG07", "#RED#iconZxG07", "#PURPLE#iconZxG07", "#GREEN#iconZxG07", "#CYAN#iconZxG07",
                      "#YELLOW#iconZxG07", "#WHITE#iconZxG07", "FLASH", "#BLACK#iconZxG07",
                      // 48
                      "DEF_FN", "FN", "LINE", "OPEN#", "CLOSE#", "MOVE", "ERASE", "POINT", "CAT", "FORMAT",
                      // 58
                      "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
                      // 68
                      "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
                      "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
                      // 94
                      "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
                      "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z",
                      // 120
                      "PLOT", "DRAW", "REM", "RUN", "RAND", "RETURN", "IF", "INPUT", "POKE", "PRINT",
                      "NEW", "SAVE", "DIM", "FOR", "GOTO", "GOSUB", "LOAD", "LIST", "LET",
                      "COPY", "CLEAR", "CONT", "CLS", "BORDER", "NEXT", "PAUSE",
                      // 146
                      "SIN", "COS", "TAN", "INT", "RND", "STR$", "CHR$", "CODE", "PEEK", "TAB",
                      "READ", "RESTOR", "DATA", "SGN", "ABS", "SQR", "VAL", "LEN", "USR", "LN",
                      // 166
                      "EXP", "LPRINT", "LLIST", "BIN", "INKEY$", "PI",
                      // 172
                      "ASN", "ACS", "ATN", "VERIFY", "MERGE", "IN", "OUT", "BEEP", "INK", "PAPER",
                      // 182
                      "FLASH", "BRIGHT", "OVER", "INVERSE", "CIRCLE", "VAL$", "SCREEN$", "ATTR",
                      // 190
                      "STOP", "NOT", "STEP", "TO", "THEN", "FUNT", "[iconZxDelete]", "AND", "OR", "AT",
                      // 200
                      "?", ",", ".", ";", "\"", "\'", "\\", "*", "/", "$",
                      "[", "]", "(", ")", "&", "%", "@", "=", "+", "-", ":",
                      "<>", "<", ">", "<=", ">=", "!", "#", "~", "_", "|", "[EXT]","#RED#[EXT]" };

static u8 namesC[]  = { NK_1, NK_2, NK_3, NK_4, NK_5, NK_6, NK_7, NK_8, NK_9, NK_0,
                        NK_Q, NK_W, NK_E, NK_R, NK_T, NK_Y, NK_U, NK_I, NK_O, NK_P,
                        NK_A, NK_S, NK_D, NK_F, NK_G, NK_H, NK_J, NK_K, NK_L, NK_ENTER,
                        NK_SS, NK_Z, NK_X, NK_C, NK_V, NK_B, NK_N, NK_M, NK_SS, NK_SPC, NK_EXT, NK_DEL };

static u8 namesK[]  = { NK_1, NK_2, NK_3, NK_4, NK_5, NK_6, NK_7, NK_8, NK_9, NK_0,
                        NK_PLOT, NK_DRAW, NK_REM, NK_RUN, NK_RAND, NK_RET, NK_IF, NK_INPUT, NK_POKE, NK_PRINT,
                        NK_NEW, NK_SAVE, NK_DIM, NK_FOR, NK_GOTO, NK_GOSUB, NK_LOAD, NK_LIST, NK_LET, NK_ENTER,
                        NK_SS, NK_COPY, NK_CLEAR, NK_CONT, NK_CLS, NK_BORDER, NK_NEXT, NK_PAUSE, NK_SS, NK_SPC, NK_EXT, NK_DEL };

static u8 namesL[]  = { NK_1, NK_2, NK_3, NK_4, NK_5, NK_6, NK_7, NK_8, NK_9, NK_0,
                        NK_q, NK_w, NK_e, NK_r, NK_t, NK_y, NK_u, NK_i, NK_o, NK_p,
                        NK_a, NK_s, NK_d, NK_f, NK_g, NK_h, NK_j, NK_k, NK_l, NK_ENTER,
                        NK_SS, NK_z, NK_x, NK_c, NK_v, NK_b, NK_n, NK_m, NK_SS, NK_SPC, NK_EXT, NK_DEL };

static u8 namesE[]  = { NK_BLUE, NK_RED, NK_MAGENTA, NK_GREEN, NK_CYAN, NK_YELLOW, NK_WHITE, NK_FLASH, NK_bright, NK_BLACK,
                        NK_SIN, NK_COS, NK_TAN, NK_INT, NK_RND, NK_STR$, NK_CHR$, NK_CODE, NK_PEEK, NK_TAB,
                        NK_READ, NK_RESTORE, NK_DATA, NK_SGN, NK_ABS, NK_SQR, NK_VAL, NK_LEN, NK_USR, NK_ENTER,
                        NK_SS, NK_LN, NK_EXP, NK_LPRINT, NK_LLIST, NK_BIN, NK_INKEY$, NK_PI, NK_SS, NK_SPC, NK_EXTP, NK_DEL };

static u8 namesCE[] = { NK_blue, NK_red, NK_magenta, NK_green, NK_cyan, NK_yellow, NK_white, NK_NA, NK_flash, NK_black,
                        NK_ASN, NK_ACS, NK_ATN, NK_VERIFY, NK_MERGE, NK_LSR, NK_RSR, NK_IN, NK_OUT, NK_ADDR,
                        NK_TILDA, NK_or, NK_SLASH, NK_LBR, NK_RBR, NK_CIRCLE, NK_VAL$, NK_SCREEN$, NK_ATTR, NK_ENTER,
                        NK_SSP, NK_BEEP, NK_INK, NK_PAPER, NK_FLASH, NK_BRIGHT, NK_OVER, NK_INVERSE, NK_SS, NK_SPC, NK_EXTP, NK_DEL };

static u8 namesSE[] = { NK_DEF_FN, NK_FN, NK_LINE, NK_OPEN_, NK_CLOSE_, NK_MOVE, NK_ERASE, NK_POINT, NK_CAT, NK_FORMAT,
                        NK_ASN, NK_ACS, NK_ATN, NK_VERIFY, NK_MERGE, NK_LSR, NK_RSR, NK_IN, NK_OUT, NK_ADDR,
                        NK_TILDA, NK_or, NK_SLASH, NK_LBR, NK_RBR, NK_CIRCLE, NK_VAL$, NK_SCREEN$, NK_ATTR, NK_ENTER,
                        NK_SS, NK_BEEP, NK_INK, NK_PAPER, NK_FLASH, NK_BRIGHT, NK_OVER, NK_INVERSE, NK_SSP, NK_SPC, NK_EXTP, NK_DEL };

static u8 namesSK[] = { NK_ZNAK, NK_ADDR, NK_NUM, NK_DOLLAR, NK_PERCENT, NK_ANPENDANS, NK_QUOTE1, NK_LBR, NK_RBR, NK_UNDERLINE,
                        NK_QLS, NK_QNOT, NK_QGT, NK_LS, NK_GT, NK_AND, NK_OR, NK_AT, NK_PT_COMMA, NK_QUOTE2,
                        NK_STOP, NK_NOT, NK_STEP, NK_TO, NK_THEN, NK_or, NK_MINUS, NK_PLUS, NK_EQ, NK_ENTER,
                        NK_SS, NK_PT2, NK_FUNT, NK_QUESTION, NK_DIV, NK_MUL, NK_COMMA, NK_PT1, NK_SSP, NK_SPC, NK_EXT, NK_DEL };

static u8 namesCL[] = { NK_edit, NK_cc, NK_true, NK_inv, 16, 17, 18, 19, NK_gg, NK_DEL,
                        NK_Q, NK_W, NK_E, NK_R, NK_T, NK_Y, NK_U, NK_I, NK_O, NK_P,
                        NK_A, NK_S, NK_D, NK_F, NK_G, NK_H, NK_J, NK_K, NK_L, NK_ENTER,
                        NK_SSP, NK_Z, NK_X, NK_C, NK_V, NK_B, NK_N, NK_M, NK_SS, NK_SPC, NK_EXT, NK_DEL };

static u8 namesCK[] = { NK_edit, NK_cc, NK_true, NK_inv, 16, 17, 18, 19, NK_gg, NK_DEL,
                        NK_PLOT, NK_DRAW, NK_REM, NK_RUN, NK_RAND, NK_RET, NK_IF, NK_INPUT, NK_POKE, NK_PRINT,
                        NK_NEW, NK_SAVE, NK_DIM, NK_FOR, NK_GOTO, NK_GOSUB, NK_LOAD, NK_LIST, NK_LET, NK_ENTER,
                        NK_SSP, NK_COPY, NK_CLEAR, NK_CONT, NK_CLS, NK_BORDER, NK_NEXT, NK_PAUSE, NK_SS, NK_SPC, NK_EXT, NK_DEL };

static u8 namesG[] = { 0, 1, 2, 3, 4, 5, 6, 7, NK_gg, NK_DEL,
                       NK_Q, NK_INKEY$, NK_E, NK_R, NK_T, NK_FN, NK_U, NK_I, NK_O, NK_P,
                       NK_A, NK_S, NK_D, NK_F, NK_G, NK_H, NK_J, NK_K, NK_L, NK_ENTER,
                       NK_SS, NK_POINT, NK_PI, NK_C, NK_RND, NK_B, NK_N, NK_M, NK_SS, NK_SPC, NK_EXT, NK_DEL };

static u8 namesCG[] = { 8, 9, 10, 11, 12, 13, 14, 15, NK_gg, NK_DEL,
                        NK_Q, NK_INKEY$, NK_E, NK_R, NK_T, NK_FN, NK_U, NK_I, NK_O, NK_P,
                        NK_A, NK_S, NK_D, NK_F, NK_G, NK_H, NK_J, NK_K, NK_L, NK_ENTER,
                        NK_SSP, NK_POINT, NK_PI, NK_C, NK_RND, NK_B, NK_N, NK_M, NK_SS, NK_SPC, NK_EXT, NK_DEL };

static u8* names[]  = { namesK, namesL, namesC, namesE, namesSE, namesSK, namesCL, namesCK, namesG, namesCG, namesCE };

static u8 semiRows[] = {
        /* 00 - 1 - 5 */ 3, 0x01, 0, 0, 3, 0x02, 0, 0, 3, 0x04, 0, 0, 3, 0x08, 0, 0, 3, 0x10, 0, 0,
        /* 05 - 6 - 0 */ 4, 0x10, 0, 0, 4, 0x08, 0, 0, 4, 0x04, 0, 0, 4, 0x02, 0, 0, 4, 0x01, 0, 0,
        /* 10 - Q - T */ 2, 0x01, 0, 0, 2, 0x02, 0, 0, 2, 0x04, 0, 0, 2, 0x08, 0, 0, 2, 0x10, 0, 0,
        /* 15 - Y - P */ 5, 0x10, 0, 0, 5, 0x08, 0, 0, 5, 0x04, 0, 0, 5, 0x02, 0, 0, 5, 0x01, 0, 0,
        /* 20 - A - G */ 1, 0x01, 0, 0, 1, 0x02, 0, 0, 1, 0x04, 0, 0, 1, 0x08, 0, 0, 1, 0x10, 0, 0,
        /* 25 - H - L */ 6, 0x10, 0, 0, 6, 0x08, 0, 0, 6, 0x04, 0, 0, 6, 0x02, 0, 0, 6, 0x01, 0, 0,
        /* 30 - cs*/     0, 0x01, 0, 0,
        /* 31 - Z - V */ 0, 0x02, 0, 0, 0, 0x04, 0, 0, 0, 0x08, 0, 0, 0, 0x10, 0, 0,
        /* 35 - B - M */ 7, 0x10, 0, 0, 7, 0x08, 0, 0, 7, 0x04, 0, 0,
        /* 38 - ss*/     7, 0x02, 0, 0,
        /* 39 - spc*/    7, 0x01, 0, 0,
        /* 40 - ext*/    0, 0x01, 7, 2,
        /* 41 - del*/    4, 0x01, 0, 1,
        /* 42 - cursor*/    4, 0x08, 0, 1, 4, 0x10, 0, 1, 3, 0x10, 0, 1, 4, 0x04, 0, 1,
        /* 46 - KEMPSTON */ 8, 0x02, 0, 0, 8, 0x08, 0, 0, 8, 0x01, 0, 0, 8, 0x04, 0, 0, 8, 0x10, 0, 0,
        /* 52 - N/A */ 0, 0, 0, 0 };

i32 zSpeccyKeyboard::onTouchEvent() {
    auto& buts(current->buttons);
    for(int i = 0 ; i < buts.size(); i++) {
        auto but(&buts[i]); auto r(but->rview);
        r.x = rview.x + z_round((float)r.x * deltaWidth);  r.w = z_round((float)r.w * deltaWidth);
        r.y = rview.y + z_round((float)r.y * deltaHeight); r.h = z_round((float)r.h * deltaHeight);
        if(r.contains((int)touch->cpt.x, (int)touch->cpt.y)) {
            if(touch->isCaptured()) {
                if(butIdx == -1) {
                    drw[DRW_FK]->measure(r.w, r.h, 0, false);
                    drw[DRW_FK]->bound = r;
                    butIdx = i; nPressSpec = 1;
                    keyCode = butIdx;
                    keyEvent(butIdx, false);
                    post(MSG_ANIM, duration, 0);
                }
                return TOUCH_STOP;
            }
            break;
        }
    }
    if(butIdx != -1) {
        // кнопка отпущена
        nPressSpec = 0; keyCode |= 0x80;
        manager->eraseAllEventsView(this);
        keyEvent(keyCode, false);
        butIdx = -1;
    }
    return TOUCH_STOP;
}

void zSpeccyKeyboard::updateMode() {
    // проверить режим клавиатуры
    u8 nmode(MODE_K); auto cpu(speccy->getCpu(1));
    auto val0(cpu->_rm8(23617)), val1(cpu->_rm8(23658)), val2(cpu->_rm8(23611));
    if(speccy->is48k()) {
        switch(val0) {
            case 0:
                if(val2 & 8) { nmode = (val1 & 8) ? MODE_C : MODE_L; } else if(val1 & 16) nmode = MODE_K;
                if((nmode == MODE_L || nmode == MODE_C) && speccy->cshift) nmode = MODE_CL;
                else { if(speccy->cshift) nmode = MODE_CK; else if(speccy->sshift) nmode = MODE_SK; }
                break;
            case 1: nmode = (speccy->cshift ? MODE_CE : (speccy->sshift ? MODE_SE : MODE_E)); break;
            case 2: nmode = speccy->cshift ? MODE_CG : MODE_G; break;
        }
    } else {
        nmode = ((val2 & 8) ? ((val1 & 8) ? MODE_C : MODE_L) : MODE_L);
        if((nmode == MODE_C || nmode == MODE_L) && speccy->cshift) nmode = MODE_CL;
    }
    if(speccy->kmode != nmode) {
        speccy->kmode = nmode;
        isDrawing = true;
        invalidate();
    }
}

static void execJoyKeys(int i, bool pressed) {
    auto k(speccy->joyKeys[i] * 4);
    auto semiRow(semiRows[k + 0]), bit(semiRows[k + 1]), semiRowEx(semiRows[k + 2]), bitEx(semiRows[k + 3]);
    if(bitEx) speccy->semiRow[semiRowEx] ^= (-pressed ^ speccy->semiRow[semiRowEx]) & bitEx;
    if(semiRow == 8) pressed = !pressed;
    speccy->semiRow[semiRow] ^= (-pressed ^ speccy->semiRow[semiRow]) & bit;
}

i32 zSpeccyKeyboard::keyEvent(int key, bool sysKey) {
    if(speccy->panelMode == 0) {
        // опрос джойстика
        // 1. отжать
        for (int i = 0; i < 8; i++) {
            if(!(key & (1 << i))) execJoyKeys(i, true);
        }
        // 2. нажать
        for (int i = 0; i < 8; i++) {
            if(key & (1 << i)) execJoyKeys(i, false);
        }
        return 0;
    }
    // клавиша была нажата/отпущена на экранной клавиатуре
    auto action(key & 0x80); key &= 0x7F; auto idx(key << 2);
    auto semiRow(semiRows[idx + 0]), semiRowEx(semiRows[idx + 2]), bit(semiRows[idx + 1]), bitEx(semiRows[idx + 3]);
    // убирать режим, если нажали другую кнопку
    if(!bitEx) {
        if(speccy->cshift) semiRowEx = 0, bitEx = 1;
        else if(speccy->sshift) semiRowEx = 7, bitEx = 2;
    }
    if(!action) {
        speccy->semiRow[semiRow]    &= ~bit;
        speccy->semiRow[semiRowEx]  &= ~bitEx;
    } else {
        if (key == 30) {
            speccy->sshift = false;
            speccy->semiRow[7] = 255; bitEx = 0;
            speccy->cshift ^= 1;
        } else if (key == 38) {
            speccy->cshift = false;
            speccy->semiRow[0] = 255; bitEx = 0;
            speccy->sshift ^= 1;
        }
        speccy->semiRow[semiRow]    |= bit;
        speccy->semiRow[semiRowEx]  |= bitEx;
    }
    return 0;
}

void zSpeccyKeyboard::onMeasure(cszm& spec) {
    auto heightSize(spec.h.size()), widthSize(spec.w.size());
    auto sz(current ? current->size : szi(1, 1));
    deltaHeight = ((float)heightSize / (float)sz.h);
    deltaWidth = ((float)widthSize / (float)sz.w);
    setMeasuredDimension(widthSize, heightSize);
    isDrawing = true;
}

void zSpeccyKeyboard::onDraw() {
    if(!isDrawing) return;
    // сформировать клавиатуру
    static int szs[] = { 0, 16_dp, 13_dp, 13_dp, 13_dp, 13_dp, 11_dp, 11_dp };
    auto baseTxt(atView<zViewButton>(0));
    baseTxt->updateStatus(ZS_VISIBLED, true);
    drw[DRW_FK]->measure(rview.w, rview.h, 0, false);
    drw[DRW_FK]->color.a = 0.9f; drw[DRW_FK]->draw(&rview); drw[DRW_FK]->color.a = 0.5f;
    auto lyt(layouts[0]->buttons); auto tex(manager->cache->get("zx_icons", nullptr));
    auto _mode(names[speccy->kmode]);
    for(int i = 0 ; i < lyt.size(); i++) {
        auto b(lyt[i]); auto r(b.rview); u32 col(0xffffffff);
        zString8 nm(keyNames[_mode[i]]);
        auto isCol(nm.count() > 1 && nm[0] == '#');
        if(isCol) { col = zColor(nm.substrBeforeLast("#")).toARGB(); nm = nm.substrAfterLast("#"); }
        auto isSpec(nm.count() > 1 && nm[0] == '[');
        if(isSpec) nm = nm.substr(1, nm.count() - 2);
        auto icon(nm.startsWith("icon") ? tex->getTile(nm) : -1);
        if(icon >= 0) nm.empty();
        auto size(szs[nm.count()]);
        r.x = rview.x + z_round((float)r.x * deltaWidth);  r.w = z_round((float)r.w * deltaWidth);
        r.y = rview.y + z_round((float)r.y * deltaHeight); r.h = z_round((float)r.h * deltaHeight);
        baseTxt->drw[DRW_FK]->color = isSpec ? 0xff808080 : theme->themeColor;
        baseTxt->setIcon(icon);
        baseTxt->setTextSize(size);
        baseTxt->setTextColorForeground(col);
        baseTxt->setTextColorIcon(col);
        szm spec(zMeasure(MEASURE_EXACT, r.w), zMeasure(MEASURE_EXACT, r.h));
        baseTxt->setTextSpecial(nm, spec);
        baseTxt->layout(r); baseTxt->draw();
    }
    manager->cache->recycle(tex);
    baseTxt->updateStatus(ZS_VISIBLED, false);
    isDrawing = false;
}
