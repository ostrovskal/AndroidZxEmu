//
// Created by User on 22.07.2023.
//

#include "sshCommon.h"
#include "zFormAssembler.h"

zFormAssembler::zFormAssembler(zStyle *_sts, i32 _id, zStyle *_sts_capt, zStyle *_sts_foot, u32 _capt, bool _m) :
        zViewForm(_sts, _id, _sts_capt, _sts_foot, _capt, _m) {
}

void zFormAssembler::onInit(bool _theme) {
    zViewForm::onInit(_theme);
}

static int tret;

struct ASM_CODE {
    u8 pref;
    u8 bas_cod;
    u8 alt_cod;
    u8 pcount;// 0 -> нет, 1 -> 1, 2 -> 2, 3 -> 1 или 2, 4 -> 2 или 3, 5 -> 0 или 1
};

static u8 stop_sym[] = {
        0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111,// 0-47
        0b00000000,// 48-55 0-7
        0b11111100,// 56-63 8,9,:;<=>?
        0b00000001,// 64-71 @,A-G
        0b00000000, 0b00000000,// 72-87
        0b01111000,// 88-95
        0b00000001,// 96-103
        0b00000000, 0b00000000,// 104-119
        0b11110000,// 120-127
        0b11111111, 0b11111111, 0b11111111, 0b11111111,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0// 160-255
};

static ASM_CODE asm_cod[] = {
        { 0x00, 0b11000011, 0b11000010, 3 },/* JP*/ { 0x00, 0b11001101, 0b11000100, 3 },/* CALL */ { 0x00, 0x18, 0b00100000, 3 },/* JR */ { 0, 0b11001001, 0b11000000, 5 },/* RET  */
        { 0x00, 0x10, 0x10, 1 },/* DJNZ */          { 0x00, 0b00000000, 0b00000000, 2 },/* LD   */
        { 0xCB, 0x40, 0x40, 4 },/* BIT  */          { 0xCB, 0x80, 0x80, 4 },/* RES  */             { 0xCB, 0xC0, 0xC0, 4 },/* SET  */
        { 0x00, 0b00000011, 0b00000100, 1 },/* INC*/{ 0x00, 0b00001011, 0b00000101, 1 },/* DEC  */
        { 0x00, 0b11101011, 0b11100011, 2 },/* EX DE,HL/EX AF,AF' */
        { 0xED, 0b01000110, 0b01000110, 1 },/* IM 0/1/2 */
        { 0x00, 0b10000000, 0xC6, 3 },/* ADD  */    { 0x00, 0b10001000, 0xCE, 3 },/* ADC  */ { 0, 0b10010000, 0xD6, 3 },/* SUB */  { 0x00, 0b10011000, 0xDE, 3 },/* SBC */
        { 0x00, 0b10100000, 0xE6, 3 },/* AND  */    { 0x00, 0b10101000, 0xEE, 3 },/* XOR  */ { 0, 0b10110000, 0xF6, 3 },/* OR  */  { 0x00, 0b10111000, 0xFE, 3 },/* CP  */
        { 0xCB, 0x00, 0x00, 3 },/* RLC  */          { 0xCB, 0x08, 0x08, 3 },/* RRC  */
        { 0xCB, 0x10, 0x10, 3 },/* RL   */          { 0xCB, 0x18, 0x18, 3 },/* RR   */
        { 0xCB, 0x20, 0x20, 3 },/* SLA  */          { 0xCB, 0x28, 0x28, 3 },/* SRA  */
        { 0xCB, 0x30, 0x30, 3 },/* SLL  */          { 0xCB, 0x38, 0x38, 3 },/* SRL  */
        { 0xED, 0x41, 0xD3, 2 },/* OUT  */          { 0xED, 0x40, 0xDB, 2 },/* IN   */
        { 0x00, 0b11000111, 0b11000111, 1 },/* RST*/
        { 0x00, 0b11000101, 0b11000101, 1 },/*PUSH*/{ 0x00, 0b11000001, 0b11000001, 1 },/* POP  */
        { 0x00, 0x00, 0x00 },/* NOP  */
        { 0x00, 0x08, 0x08, 2 },/* EX AF, AF  */
        { 0x00, 0x07, 0x07 },/* RLCA */          { 0x00, 0x0F, 0x0F },/* RRCA */
        { 0x00, 0x17, 0x17 },/* RLA  */          { 0x00, 0x1F, 0x1F },/* RRA  */
        { 0x00, 0x27, 0x27 },/* DAA  */          { 0x00, 0x2F, 0x2F },/* CPL  */ { 0x00, 0x37, 0x37 },/* SCF  */ { 0x00, 0x3F, 0x3F },/* CCF  */
        { 0x00, 0xF3, 0xF3 },/* DI   */          { 0x00, 0xFB, 0xFB },/* EI   */
        { 0xED, 0x67, 0x67 },/* RRD  */          { 0xED, 0x6F, 0x6F },/* RLD  */
        { 0xED, 0xA0, 0xA0 },/* LDI  */          { 0xED, 0xA1, 0xA1 },/* CPI  */ { 0xED, 0xA2, 0xA2 },/* INI  */ { 0xED, 0xA3, 0xA3 },/* OTI  */
        { 0xED, 0xA8, 0xA8 },/* LDD  */          { 0xED, 0xA9, 0xA9 },/* CPD  */ { 0xED, 0xAA, 0xAA },/* IND  */ { 0xED, 0xAB, 0xAB },/* OTD  */
        { 0xED, 0xB0, 0xB0 },/* LDIR */          { 0xED, 0xB1, 0xB1 },/* CPIR */ { 0xED, 0xB2, 0xB2 },/* INIR */ { 0xED, 0xB3, 0xB3 },/* OTIR */
        { 0xED, 0xB8, 0xB8 },/* LDDR */          { 0xED, 0xB9, 0xB9 },/* CPDR */ { 0xED, 0xBA, 0xBA },/* INIR */ { 0xED, 0xBB, 0xBB },/* OTIR */
        { 0x00, 0xD9, 0xD9 },/* EXX  */          { 0x00, 0xEB, 0xEB },/* EX DE,*/{ 0x00, 0xE3, 0xE3, 2 },/* EX (SP), */
        { 0xED, 0x4D, 0x4D },/* RETI */          { 0xED, 0x45, 0x45 },/* RETN */
        { 0x00, 0x76, 0x76 },/* HALT */
        { 0xED, 0x44, 0x44 },/* NEG  */
        { 0x00, 0xDD, 0xDD },/* *IX* */          { 0x00, 0xFD, 0xFD },/* *IY* */ { 0xED, 0x00, 0x00 } /* *ED* */
};

static char tbuf[512];
static int tpos(0);

#define ASM(lex)    tbuf[tpos++] = (lex);

static void skip_spc(char** text) {
    auto txt(*text);
    while(*txt == ' ' || *txt == '\t') txt++;
    *text = txt;
}

static int is_eof(char** text) {
    skip_spc(text); auto txt(*text);
    return (*txt == '\r' || *txt == '\n' || *txt == 0 || *txt == ';');
}

static bool is_word(char* place) {
    auto ch(*place);
    if(stop_sym[ch >> 3] & (1 << (ch & 7))) return false;
    return ch >= 'A';
}

static void calcOps(char* txt) {
    int op1(*(uint16_t*)(txt + 1)), op2(*(uint16_t*)(txt + 5));
    auto ops(*(txt + 3));
    switch(ops) {
        case '+': op1 += op2; break;
        case '-': op1 -= op2; break;
        case '*': op1 *= op2; break;
        case '/': op1 /= op2; break;
        case '%': op1 %= op2; break;
        case '&': op1 &= op2; break;
        case '^': op1 ^= op2; break;
        case '|': op1 |= op2; break;
        case 'e': op1 = op1 == op2; break;
        case 'n': op1 = op1 != op2; break;
        case '>': op1 = op1 > op2; break;
        case '<': op1 = op1 < op2; break;
        case 'a': op1 = op1 >= op2; break;
        case 'b': op1 = op1 <= op2; break;
        case 'r': op1 = op1 >> op2; break;
        case 'l': op1 = op1 << op2; break;
        default: ILOG("unknown operation"); break;
    }
    *(uint16_t*)(txt + 1) = op1; *(txt + 3) = 0;
    memcpy(txt + 3, txt + 7, (&tbuf[512] - (txt + 7)));
}

static char* findOps(char* txt, char ops) {
    char ch; char* op1(nullptr);
    while((ch = *txt)) {
        if(ch == ')' || ch == 0) break;
        else if(ch == 'N') { op1 = txt; txt += 2; } else if(ch == ops) return op1;
        txt++;
    }
    return nullptr;
}

int zFormAssembler::parseOps() {
    static uint8_t ops[] = { '+', '-', '*', '/', '%', '^', '&', '|', '(', ')' };
    auto ch(*lexPos), ch1(lexPos[1]);
    for(auto o : ops) { if(ch == o) return o; }
    if(ch == '=')      return (ch1 == '=' ? 'e' : ERROR_WRONG_OPERATION);
    else if(ch == '!') return (ch1 == '=' ? 'n' : ERROR_WRONG_OPERATION);
    else if(ch == '>') return (ch1 == '>' ? 'r' : (ch1 == '=' ? 'a' : '>'));
    else if(ch == '<') return (ch1 == '<' ? 'l' : (ch1 == '=' ? 'b' : '<'));
    return ERROR_UNDEFINED_OPERATION;
}

int zFormAssembler::exprConvert(char** place) {
    tpos = 0;
    auto txt(*place); bool ops(true); int oo(0), bracket(0);
    while(!is_eof(&txt)) {
        auto len(nextLexem(&txt)), cmd(getCmd(lexPos, len));
        if(cmd == C_NULL || *lexPos == ',') break;
        if(cmd > 22) return ERROR_UNEXPECTED_LEXEM;
        if(len == 0 && *lexPos != '$') {
            // operation
            auto o(parseOps());
            if(o < 0) return o;
            if(o == '(' || o == ')') {
                if(o == '(') { bracket++; if(!ops) return ERROR_INVALID_EXPRESSION; }
                else if(o == ')') { bracket--; if(ops) return ERROR_INVALID_EXPRESSION; }
                // проверить на парную скобку
                if(bracket < 0) break;
                txt++; ASM(o);
            } else {
                if(ops) {
                    // может унарная операция?
                    if(oo) return ERROR_UNEXPECTED_LEXEM;
                    if(o == '+') oo = 1;
                    else if(o == '-') oo = -1;
                    else return ERROR_UNEXPECTED_LEXEM;
                    txt++; ops = true; continue;
                }
                if(o >= 'a') txt++;
                txt++; ops = true; oo = 0; ASM(o);
            }
        } else {
            int n(0);
            if(!ops) return ERROR_UNEXPECTED_LEXEM;
            // number/label/register
            if(*lexPos == '$' || is_word(lexPos)) {
                if(*lexPos == '$') { n = pc + (int)(buf - cmdBuf); txt++; }
                else {
                    // register?
                    int i(-1);
                    if(pass == 3) {
                        static int nmRegs[23] = {   RC, RB, RE, RD, RL, RH, RR1, RA, RXL, RXH, RYL, RYH, RF, RI, 0, RC, RE, RL, RF, RSPL, RXL, RYL, RPCL };
                        for(i = 22 ; i >= 0; i--) {
                            auto nc(namesCode[i]); auto ln((nc[1] != 0) + 1);
                            if(strncmp(nc, lexPos, ln) == 0) {
                                auto base((uint8_t*)&speccy->dev<zCpuMain>()->c + nmRegs[i]);
                                n = (i > 14 ? *(uint16_t*)base : *(uint8_t*)base);
                                break;
                            }
                        }
                    }
                    if(i < 0 && labels) {
                        // label
                        auto lb(add_label(lexPos, len, false));
                        if(!lb && pass >= 2) return ERROR_LABEL_NOT_FOUND;
                        n = lb ? lb->value : 0;
                    }
                }
            } else {
                auto tend(lexPos);
                n = *(int*)z_ston(lexPos, RADIX_DEC, &tend);
                if((tend - lexPos) != len) return 0;// ERROR_WRONG_NUMBER;
            }
            if(n < 0 || n > 65535) return ERROR_NUMBER_OUT_OF_RANGE;
            if(oo) n *= oo; ASM('N'); ASM(n & 255); ASM(n >> 8); ops = false;
        }
    }
    if(ops) return ERROR_INVALID_EXPRESSION;
    ASM(0);
    *place = txt;
    return ERROR_OK;
}

int zFormAssembler::parseSubExpress(char* place) {
    static char priority[] = { '*', '/', '%', '+', '-', '&', '^', '|',  '>', '<', 'e', 'n', 'a', 'b', 'r', 'l' };
    for(auto o : priority) {
        auto txt(place);
        while((txt = findOps(txt, o)))
            calcOps(txt);
    }
    return ERROR_OK;
}

int zFormAssembler::quickParser(const char* text) {
    pass = 3; labels = nullptr;
    if(parseExpress((char**)&text, false) >= 0)
        return number;
    return ERROR_NUMBER_OUT_OF_RANGE;
}

int zFormAssembler::parseExpress(char** place, bool _disp) {
    char ch;
    auto bpos((char*)nullptr), pos(tbuf), end(&tbuf[tpos]);
    // 1. преобразовать выражение во внутреннюю структуру
    auto ret(exprConvert(place));
    if(ret < 0) return ret;
    while((ch = *pos)) {
        if(ch == 'N') pos += 2;
        else if(ch == '(') bpos = pos;
        else if(ch == ')') {
            // выход, если скобка без пары
            if(!bpos) break;
            // парсить внутреннее выражение в скобках
            auto ret(parseSubExpress(bpos + 1));
            if(ret < 0) return ret;
            // заменить его на результат
            *bpos = 'N'; *(uint16_t*)(bpos + 1) = *(uint16_t*)(bpos + 2);
            memcpy(bpos + 3, bpos + 5, (&tbuf[512] - (bpos + 5)));
            // начать заново
            pos = tbuf - 1;
        }
        pos++;
    }
    if(tbuf[3]) parseSubExpress(tbuf);
    number = *(u16*)(tbuf + 1);
    if(_disp) {
        if((i16)number < -128 || (i16)number > 127) return ERROR_DISP_OUT_OF_RANGE;
        disp = (u8)number;
    }
    return C_NN;
}

int zFormAssembler::skipComma(char** text) {
    lexPos = *text;
    if(lexPos[0] != ',') return 1;
    lexPos++;
    skip_spc(&lexPos);
    *text = lexPos;
    return 0;
}

int zFormAssembler::getCmd(const char* cmd, int len) {
    int i, idx = -1; const char* name; char ch;
    if(len) {
        while((name = namesCode[++idx])) {
            for(i = 0 ; i < len; i++) {
                ch = cmd[i];
                if(ch < 33) break;
                ch &= -33;
                if(ch != name[i]) break;
            }
            if(i == len && name[i] <= ' ')
                return idx;
        }
    } else {
        ch = *cmd;
        if(ch == ';' || ch == 0) return C_NULL;
    }
    return ERROR_KEYWORD_NOT_FOUND;
}

bool zFormAssembler::parseString(char** place, int& ret) {
    skip_spc(place); auto txt(*place); auto quote(*txt++);
    if(quote != '\'' && quote != '\"') return false;
    while(!is_eof(&txt)) {
        auto ch(*txt);
        if(ch == '\\') {
            txt++;
            // escape sequence
            switch(*txt++) {
                case 't': ch = 9; break;
                case 'r': ch = 10; break;
                case 'n': ch = 13; break;
                case '\\':ch = '\\';break;
                case '\'':ch = '\''; break;
                case '\"':ch = '\"'; break;
                default: ret = ERROR_INVALID_ESCAPE_SEQUENCE; lexPos = txt - 2; return false;
            }
        } else {
            if(ch == quote) break;
            txt++;
        }
        *buf++ = ch;
    }
    if(*txt != quote) { ret = ERROR_INVALID_STRING; return false; }
    *place = ++txt;
    return true;
}

int zFormAssembler::nextLexem(char** text) {
    skip_spc(text);
    char ch; auto txt(*text); lexPos = txt;
    auto current(lexPos - 1);
    while ((ch = *++current)) {
        if(ch == '#' || ch == '%') {
            auto cend(current);
            z_ston(current, RADIX_DEC, &cend);
            if((cend - current) > 1) { current = cend - 1; continue; }
        }
        if(stop_sym[ch >> 3] & (1 << (ch & 7))) break;
    }
    auto len((int)(current - lexPos));
    skip_spc(&current);
    *text = current;
    return len;
}

int zFormAssembler::parseOperand(char** text) {
    // number/(number)/(HL/DE/BC)/(IX+-number/IY+-number)/REG/FLAG/LABEL/CONSTANT/VARIABLE
    skip_spc(text);
    auto txt(*text);
    auto isBrakket(txt[0] == '(');
    if(isBrakket) { txt++; skip_spc(&txt); }
    lexPos = txt; is_lb = 0;
    // выражение/регистр
    auto len(nextLexem(&txt)), ret(getCmd(lexPos, len));
    if(ret < 0) {
        txt = lexPos;
        ret = parseExpress(&txt, false);
        if(ret < 0) return ret;
    }
    if(ret < C_JP) {
        // регистры
        lexPos = txt;
        if(isBrakket) {
            if(ret == C_IX || ret == C_IY) {
                // disp
                if(*txt == '+' || *txt == '-') {
                    auto res(parseExpress(&txt, true));
                    if(res < 0) return res;
                } else return ERROR_NOT_FOUND_DISP_OFFSET;
            }
        }
    }
    skip_spc(&txt);
    if(isBrakket) {
        if(txt[0] != ')') {
            ret = ERROR_CLOSE_BRAKKET_NOT_FOUND;
        } else {
            txt++; skip_spc(&txt);
            // индекс ключевого слова привести к верному
            // IX+-/IY+-/BC/DE/number
            switch(ret) {
                case C_NN: ret = C_PNN; break;
                case C_IX: ret = C_PIX; break;
                case C_IY: ret = C_PIY; break;
                case C_HL: ret = C_PHL; break;
                case C_DE: ret = C_PDE; break;
                case C_BC: ret = C_PBC; break;
                case C_SP: ret = C_PSP; break;
                case C_C:  ret = C_PC;  break;
                default: ret = ERROR_INVALID_INDEXED_OPERATION;
            }
        }
    }
    *text = txt;
    return ret;
}

int zFormAssembler::parserOperator(int len) {
    int ret(ERROR_OK); zString label;
    auto addr(pc + (int)(buf - cmdBuf)); auto cmd(icmd); icmd = -1;
    bool isDef(false);
    switch(cmd) {
        case C_REP:
            if(parseOperand(&text) != C_NN) { ret = ERROR_INVALID_COMMAND; break; }
            skipComma(&text); ret = C_REP; if(!rep) rep = number + 1; isDef = true;
            break;
        case C_DB: case C_DEFB:
            if(lb) lb->type = L_VARIABLE, lb->value = addr;
            while(!is_eof(&text)) {
                if(!parseString(&text, ret)) {
                    if(ret < 0) break;
                    if(parseOperand(&text) != C_NN) { ret = ERROR_WRONG_OPERATION; break; }
                    if(number > 255) { ret = ERROR_VALUE_OUT_OF_RANGE; break; }
                    *buf++ = (uint8_t)number;
                }
                skipComma(&text);
            }
            break;
        case C_DW: case C_DEFW:
            if(lb) lb->type = L_VARIABLE, lb->value = addr;
            while(!is_eof(&text)) {
                if(parseOperand(&text) != C_NN) { ret = ERROR_WRONG_OPERATION; break; }
                skipComma(&text); wordLE(&buf, (uint16_t)number);
            }
            break;
        case C_ORG:
            ret = parseOperand(&text);
            if(ret != C_NN) ret = ERROR_INVALID_OPERAND;
            else if(is_lb && number == 0) ret = ERROR_INVALID_OPERAND;
            else ret = CMD_ORG, * (uint16_t*)buf = (uint16_t)number;
            break;
        case C_END:
            ret = CMD_END;
            break;
        case C_EQU: {
            // последняя метка
            if(parseOperand(&text) != C_NN) { ret = ERROR_INVALID_OPERAND; break; }
            lb->value = number; lb->type = L_CONST; lb = nullptr;
            break;
        }
        default:
            lb = add_label(lexPos, len, true);
            if(lb->type != L_UNDEF && pass == 1)
                ret = ERROR_DUPLICATE_LABEL;
            else {
                skip_spc(&text);
                switch(*text) {
                    case '=':
                        text++;
                        if(parseOperand(&text) != C_NN) { ret = ERROR_INVALID_COMMAND; break; }
                        lb->value = number; lb->type = L_CONST;
                        break;
                    default: {
                        auto ch(lexPos[len]);
                        if(ch == 0 || ch == ':' || ch == ' ' || ch == '\t') {
                            lb->type = L_LABEL; lb->value = addr;
                            text += ch == ':'; lexPos = text;
                        }
                        isDef = true;
                    }
                }
                break;
            }
    }
    if(isDef) {
        icmd = getCmd(lexPos, nextLexem(&text));
        if(icmd < ERROR_OK) ret = icmd;
        else if(icmd < C_JP) ret = ERROR_INVALID_COMMAND;
    } else if(ret >= ERROR_OK && !is_eof(&text)) ret = ERROR_EXPECT_END_OF_COMMAD;
    return ret;
}

int zFormAssembler::parser(int _pass, int address, const char *txt, zArray<Z_LABEL>* _labels) {
    // 1. ищем инструкцию
    int ret(ERROR_INVALID_COMMAND);
    labels = _labels; pass = _pass; lb = nullptr;
    scmd = 0; buf = cmdBuf; pc = address;
    text = (char*)txt;
    auto len(nextLexem(&text));
    icmd = getCmd(lexPos, len);
    // 2. в зависимости от типа - переходим на процедуру детального парсинга
    // если это слово
    if(labels && (icmd == C_REP || icmd < 0 && is_word(lexPos))) {
        // variable/label
        ret = parserOperator(len);
    }
    if(ret != C_REP) rep = 0; else rep--, ret = ERROR_OK;
    if(icmd >= C_JP && icmd < C_IX_NONI) {
        lexPos = text;
        ret = (icmd >= C_ORG ? parserOperator(0) : parserMnemonic());
    }
    // 3. регистрируем ошибку, при наличии
    if(ret != ERROR_OK && icmd != C_NULL) {
        // error
        if(ret == ERROR_CLOSE_BRAKKET_NOT_FOUND) {
            auto t(strchr(lexPos, ')'));
            text = (t ? t : (char*)(txt + strlen(txt)));
        }
        speccy->jni = ((u16)(lexPos - txt) | ((u16)(text - txt) << 16));
    } else scmd = (int)(buf - cmdBuf), ret = ERROR_OK;
    return ret;
}

int zFormAssembler::parserMnemonic() {
    static uint8_t asm_ops[]   = {  1, 0, 3, 2, 5, 4, 255, 7, 5, 4, 5, 4, 6, 255, 0, 0, 1, 2, 3, 3, 2, 2, 0,
                                    0, 1, 2, 3, 4, 5, 6, 7,
                                    6, 6, 0, 1, 6, 255 };
    static uint8_t asm_prefs[] = {  0, 0, 0, 0, 0, 0, 0, 0, 221, 221, 253, 253, 48, 0, 0, 0, 0, 0, 0, 0, 221, 253, 0,
                                    0, 0, 0, 0, 0, 0, 0, 0,
                                    221, 253, 0, 0, 0, 0, 0 };
    int src(0), tmp;
    uint8_t dstPref(0), srcPref(0), dstOps(0);
    int16_t dstNum(0);
    auto stk(&asm_cod[icmd - C_JP]);
    auto cmd_op1(text), cmd_op2(text);
    auto dst(parseOperand(&text));
    if(dst < 0) return dst;
    switch(stk->pcount) {
        case PAR_COUNT_EMPTY:
            if(dst != C_NULL) return ERROR_INVALID_COMMAND;
            break;
        case PAR_COUNT_ONE:
            dstNum = number;
            break;
        case PAR_COUNT_TWO:
        case PAR_COUNT_TWO_OR_THREE:
            if(skipComma(&text)) return ERROR_COMMA_NOT_FOUND;
            dstNum = number; cmd_op2 = text;
            src = parseOperand(&text);
            if(src < 0) return src;
            if(stk->pcount == PAR_COUNT_TWO_OR_THREE) {
                if(dst != C_NN || dstNum > 7) return ERROR_INVALID_OPERAND;
                dst = src; src = C_NULL;
                if(text[0] == ',') {
                    cmd_op2 = ++text;
                    src = parseOperand(&text);
                }
            }
            break;
        case PAR_COUNT_ONE_OR_TWO:
            if(text[0] == ',') {
                cmd_op2 = ++text;
                src = parseOperand(&text);
            } else { src = dst; dst = C_NULL; }
            break;
    }
    if(src < 0) return src;
    if(!is_eof(&text)) return ERROR_EXPECT_END_OF_COMMAD;
    switch(dst) {
        case C_IX: case C_IY: case C_PIX: case C_PIY:
        case C_XL: case C_XH: case C_YL: case C_YH:
            dstPref = asm_prefs[dst];
            break;
    }
    switch(src) {
        case C_IX: case C_IY: case C_PIX: case C_PIY:
        case C_XH: case C_YH: case C_XL: case C_YL:
            srcPref = asm_prefs[src];
            if((dst == C_HL || dst == C_L || dst == C_H || (dst >= C_XL && dst <= C_PIY)) && dstPref != srcPref) {
                lexPos = cmd_op2;
                return ERROR_INVALID_COMMAND;
            }
            break;
    }
    auto prf = stk->pref;
    auto cod = stk->bas_cod;
    if(prf == 0xED && (dstPref || srcPref)) {
        lexPos = dstPref ? cmd_op1 : cmd_op2;
        return ERROR_INVALID_COMMAND;
    }
    if(dstPref || srcPref) *buf++ = dstPref | srcPref;
    if(prf) *buf++ = prf;
    if(dst < C_JP) dstOps = asm_ops[dst];
    auto srcOps(asm_ops[src]);
    switch(icmd) {
        case C_IM:
            if(dst == C_NN && dstNum < 3) { *buf++ = (uint8_t)(cod | (((number & 3) + 1) << 3)); break; }
            lexPos = cmd_op1;
            return ERROR_INVALID_INTERRUPT_MODE;
        case C_RST:
            if(dst == C_NN && (number & ~56) == 0) { *buf++ = (uint8_t)(cod | number); break; }
            lexPos = cmd_op1;
            return ERROR_INVALID_RST_NUMBER;
        case C_EX:
            lexPos = cmd_op1;
            switch(dst) {
                // EX DE, HL
                case C_DE:
                    if(src == C_HL) { *buf++ = cod; return ERROR_OK; }
                    break;
                    // EX AF, AF
                case C_AF:
                    if(src == C_AF) { *buf++ = 0x08; return ERROR_OK; }
                    break;
                    // EX (SP), HL/IX/IY
                case C_PSP:
                    if(src == C_IX || src == C_IY || src == C_HL) { *buf++ = stk->alt_cod; return ERROR_OK; }
                    break;
            }
            lexPos = cmd_op2;
            return ERROR_INVALID_COMMAND;
            // INC/DEC REG8/REG16/(HL/IX/IY)
        case C_INC: case C_DEC:
            switch(dst) {
                case C_HL: case C_DE: case C_BC: case C_SP: case C_IX: case C_IY:
                    *buf++ = (uint8_t)(cod | (asm_ops[dst] << 4));
                    break;
                case C_PIX: case C_PIY:
                case C_XH: case C_XL: case C_YH: case C_YL:
                case C_B: case C_C: case C_D: case C_E: case C_H: case C_L: case C_A: case C_PHL:
                    *buf++ = (uint8_t)(stk->alt_cod | (dstOps << 3));
                    if(dst >= C_PIX) *buf++ = disp;
                    break;
                default:
                    lexPos = cmd_op1;
                    return ERROR_INVALID_COMMAND;
            }
            break;
        case C_RLC: case C_RRC: case C_RL: case C_RR: case C_SLA: case C_SRA: case C_SLI: case C_SRL:
            dstNum = (uint16_t)(icmd - C_RLC);
            if(dst == C_NULL) { dst = src; src = C_NULL; }
            // CMD BIT,(PHL/PIX/PIY/SRC8)[,SRC8]
        case C_BIT: case C_RES: case C_SET:
            if(dst == C_PIX || dst == C_PIY) *buf++ = disp;
            switch(src) {
                case C_NULL: if(dstPref) dstOps = 6; break;
                case C_B: case C_C: case C_D: case C_E: case C_H: case C_L: case C_A: dstOps = 0; break;
                default:
                    lexPos = cmd_op2;
                    return ERROR_INVALID_COMMAND;
            }
            *buf++ = (uint8_t)(cod | (dstNum << 3) | srcOps | dstOps);
            break;
        case C_DJNZ:
            src = dst; dst = C_NULL;
        case C_JR:
            // JR [cc], NN
            lexPos = cmd_op1;
            if(checkFlags(dst, true)) {
                if(dst == C_C) dstOps = 3;
                cod = stk->alt_cod;
            } else if(dst != C_NULL)
                return ERROR_INVALID_COMMAND;
            lexPos = cmd_op2;
            if(src != C_NN) return ERROR_INVALID_ADDRESS_OPERAND;
            number -= (pc + 2);
            if(labels && pass == 1) number = 0;
            if(number < -128 || number > 127) return ERROR_RELATIVE_JUMP_OUT_OF_RANGE;
            *buf++ = (uint8_t)(cod | (dstOps << 3));
            *buf++ = (uint8_t)number;
            break;
        case C_JP:
            // JP IX/IY/HL
            if(src == C_IX || src == C_IY || src == C_HL) { *buf++ = 0xE9; return ERROR_OK; }
        case C_CALL:
            // cmd [cc], NN
            lexPos = cmd_op1;
            if(checkFlags(dst, false)) {
                if(dst == C_C) dstOps = 3;
                cod = stk->alt_cod;
            } else if(dst != C_NULL) return ERROR_INVALID_COMMAND;
            lexPos = cmd_op2;
            if(src != C_NN) return ERROR_INVALID_ADDRESS_OPERAND;
            *buf++ = (uint8_t)(cod | (dstOps << 3));
            *buf++ = (uint8_t)(number & 0xFF);
            *buf++ = (uint8_t)(number >> 8);
            break;
        case C_RET:
            // RET [cc]
            lexPos = cmd_op1;
            if(checkFlags(dst, false)) {
                if(dst == C_C) dstOps = 3;
                cod = stk->alt_cod;
            } else if(dst != C_NULL) return ERROR_INVALID_COMMAND;
            *buf++ = (uint8_t)(cod | (dstOps << 3));
            break;
        case C_PUSH: case C_POP:
            // PUSH/POP REG16
            lexPos = cmd_op1;
            if(checkReg16(dst, false)) {
                *buf++ = (uint8_t)(cod | (dstOps << 4)); break;
            } else return ERROR_INVALID_OPERAND;
            break;
        case C_OUT:
            lexPos = cmd_op2; tret = ERROR_INVALID_OPERAND;
            if(!srcPref && (checkReg8(src) || src == C_NN)) {
                tmp = dst;
                if(dst == C_NN || dst == C_PNN) {
                    if(src != C_A) return tret;
                    if(dstNum > 255) return ERROR_PARAMETER_OUT_OF_RANGE;
                    // OUT (N), A
                    buf--; cod = stk->alt_cod;
                    dst = C_PC; srcOps = 0; tmp = C_NN;
                }
                if(dst == C_PC) {
                    if(src == C_NN) {
                        // OUT (C), 0
                        if(number) return ERROR_PARAMETER_OUT_OF_RANGE;
                        srcOps = 6;
                    }
                    // OUT (C/N), REG8/A
                    *buf++ = (uint8_t)(cod | (srcOps << 3));
                    if(tmp == C_NN) *buf++ = (uint8_t)dstNum;
                    break;
                }
            }
            lexPos = cmd_op1;
            return tret;
        case C_IN:
            tret = ERROR_INVALID_OPERAND;
            if(!dstPref && (checkReg8(dst) || dst == C_F)) {
                tmp = src;
                if(src == C_NN || src == C_PNN) {
                    if(dst != C_A) { lexPos = cmd_op1; return tret; }
                    if(number < 256) {
                        // IN A, (N)
                        buf--; cod = stk->alt_cod;
                        src = C_PC; dstOps = 0; tmp = C_NN;
                    } else tret = ERROR_PARAMETER_OUT_OF_RANGE;
                }
                if(src == C_PC) {
                    // IN A/REG8,(C/N)
                    *buf++ = (uint8_t)(cod | (dstOps << 3));
                    if(tmp == C_NN) *buf++ = (uint8_t)number;
                    break;
                }
            }
            lexPos = cmd_op2;
            return tret;
        case C_ADD:
            if(dst == C_IX || dst == C_IY || dst == C_HL) {
                if(checkReg16(src, true)) { *buf++ = (uint8_t)(0b00001001 | (asm_ops[src] << 4)); break; } else {
                    lexPos = cmd_op2;
                    return ERROR_INVALID_OPERAND;
                }
                return ERROR_OK;
            }
        case C_ADC: case C_SBC:
            if(dst == C_HL && icmd != C_ADD) {
                if(!srcPref && checkReg16(src, true)) {
                    *buf++ = 0xED;
                    *buf++ = (uint8_t) ((icmd == C_ADC ? 0b01001010 : 0b01000010) | (asm_ops[src] << 4));
                    return ERROR_OK;
                } else {
                    lexPos = cmd_op2;
                    return ERROR_INVALID_OPERAND;
                }
            }
        case C_SUB: case C_OR: case C_XOR: case C_AND: case C_CP:
            // ALU [A, ]N/SRC8
            if(dst == C_NULL) dst = C_A;
            if(dst != C_A) {
                lexPos = cmd_op1;
                return ERROR_INVALID_OPERAND;
            }
            lexPos = cmd_op2;
            switch(src) {
                case C_PIX: case C_PIY:
                case C_XH: case C_XL: case C_YH: case C_YL:
                case C_B: case C_C: case C_D: case C_E: case C_H: case C_L: case C_A: case C_PHL:
                    *buf++ = (uint8_t)(cod | srcOps);
                    if(srcPref) *buf++ = disp;
                    break;
                case C_NN:
                    if(number > 255) return ERROR_PARAMETER_OUT_OF_RANGE;
                    *buf++ = stk->alt_cod;
                    *buf++ = (uint8_t)number;
                    break;
                default: return ERROR_INVALID_OPERAND;
            }
            break;
        case C_LD:
            lexPos = cmd_op2;
            switch(dst) {
                case C_I: case C_R:
                    // LD I,A/LD R,A
                    if(src != C_A) return ERROR_INVALID_OPERAND;
                    *buf++ = 0xED; *buf++ = (uint8_t)(dst == C_I ? 0b01000111 : 0b01001111);
                    break;
                    // LD (NN), SRC16
                case C_PNN:
                    switch(src) {
                        // LD (NN), IX/IY/HL/A
                        case C_IX: case C_IY: case C_A: case C_HL:
                            *buf++ = (uint8_t)(src == C_A ? 50 : 34);
                            *buf++ = (uint8_t)(number & 0xFF);
                            *buf++ = (uint8_t)(number >> 8);
                            break;
                            // LD (NN), BC/DE/SP
                        case C_BC: case C_DE: case C_SP:
                            *buf++ = 0xED;
                            *buf++ = (uint8_t)(0b01000011 | (srcOps << 4));
                            *buf++ = (uint8_t)(number & 0xFF);
                            *buf++ = (uint8_t)(number >> 8);
                            break;
                        default: return ERROR_INVALID_OPERAND;
                    }
                    break;
                    // LD (BC/DE), A
                case C_PBC: case C_PDE:
                    if(src != C_A) return ERROR_INVALID_OPERAND;
                    *buf++ = (uint8_t)(0b00000010 | (dstOps << 4));
                    break;
                    // LD HL/IX/IY/BC/DE/SP, NN/(NN)
                case C_IX: case C_IY: case C_HL: case C_DE: case C_BC: case C_SP:
                    switch(src) {
                        case C_NN:
                            *buf++ = (uint8_t)(1 + (dstOps << 4));
                            break;
                        case C_PNN:
                            if(!dstPref && dst != C_HL) *buf++ = 0xED;
                            *buf++ = (uint8_t)(dstOps == 2 ? 42 : (0b01001011 | (dstOps << 4)));
                            break;
                            // LD SP,HL/IX/IY
                        case C_IX: case C_IY: case C_HL:
                            if(dst == C_SP) { *buf++ = 0xF9; return ERROR_OK; }
                        default: return ERROR_INVALID_OPERAND;
                    }
                    *buf++ = (uint8_t)(number & 0xFF);
                    *buf++ = (uint8_t)(number >> 8);
                    break;
                    // LD (PHL/PIX/PIY), N/SRC8
                case C_PIX: case C_PIY: case C_PHL:
                    switch(src) {
                        case C_NN:
                            if(number > 255) return ERROR_PARAMETER_OUT_OF_RANGE;
                            *buf++ = 54; if(dstPref) *buf++ = disp; *buf++ = (uint8_t)number;
                            break;
                        case C_B: case C_C: case C_D: case C_E: case C_H: case C_L: case C_A:
                            *buf++ = (uint8_t)(0b01110000 | srcOps); if(dstPref) *buf++ = disp;
                            break;
                        default: return ERROR_INVALID_OPERAND;
                    }
                    break;
                    // LD REG8, N/(NN)/(BC/DE)/(HL/IX/IY)
                case C_XH: case C_XL: case C_YH: case C_YL:
                case C_B: case C_C: case C_D: case C_E: case C_H: case C_L: case C_A:
                    switch(src) {
                        // LD REG8, REG8
                        case C_XH: case C_XL: case C_YH: case C_YL:
                            if(dst == C_L || dst == C_H) return ERROR_INVALID_COMMAND;
                        case C_H: case C_L:
                            if(dstPref && (src == C_L || src == C_H)) return ERROR_INVALID_COMMAND;
                        case C_B: case C_C: case C_D: case C_E: case C_A:
                            *buf++ = (uint8_t)(0b01000000 | (dstOps << 3) | srcOps);
                            break;
                            // LD REG8, N
                        case C_NN:
                            if(number > 255) return ERROR_PARAMETER_OUT_OF_RANGE;
                            *buf++ = (uint8_t)(0b00000110 | (dstOps << 3));
                            *buf++ = (uint8_t)number;
                            break;
                            // LD A, (BC/DE)
                        case C_PBC: case C_PDE:
                            if(dst != C_A) return ERROR_INVALID_OPERAND;
                            *buf++ = (uint8_t)(0b00001010 | (srcOps << 4));
                            break;
                            // LD A, (NN)
                        case C_PNN:
                            if(dst != C_A) return ERROR_INVALID_OPERAND;
                            *buf++ = 58;
                            *buf++ = (uint8_t)(number & 0xFF);
                            *buf++ = (uint8_t)(number >> 8);
                            break;
                            // LD A,I/LD A,R
                        case C_I: case C_R:
                            *buf++ = 0xED; *buf++ = (uint8_t)(src == C_I ? 0b01010111 : 0b01011111);
                            break;
                            // LD REG8, (IX+-d/IY+-d)
                        case C_PIX: case C_PIY: case C_PHL:
                            *buf++ = (uint8_t)(0b01000110 | (dstOps << 3));
                            if(srcPref) *buf++ = disp;
                            break;
                        default: return ERROR_INVALID_OPERAND;
                    }
                    break;
                default:
                    lexPos = cmd_op1;
                    return ERROR_INVALID_COMMAND;
            }
            break;
        default: *buf++ = cod; break;
    }
    return ERROR_OK;
}

bool zFormAssembler::checkFlags(int flags, bool jr) {
    //auto ef(is_jr ? C_FC : C_FM);
    auto ef(C_FM - jr * 4);
    return ((flags >= C_FNZ && flags <= ef) || flags == C_C);
}

bool zFormAssembler::checkReg16(int reg, bool isSP) {
//    auto sp_af(is_sp ? C_SP : C_AF);
    auto sp_af(C_AF + isSP);
    return (reg == C_IX || reg == C_IY || reg == C_HL || reg == C_DE || reg == C_BC || reg == sp_af);
}

bool zFormAssembler::checkReg8(int reg) {
    return (reg == C_B || reg == C_C || reg == C_D || reg == C_E || reg == C_H || reg == C_L ||
            reg == C_A || reg == C_XH || reg == C_XL || reg == C_YH || reg == C_YL);
}

Z_LABEL* zFormAssembler::add_label(char* place, int len, bool ret_always) {
    zString name(place, len);
    // ищем такую же
    for(int i = 0; i < labels->size(); i++) {
        auto lb(&labels->at(i));
        if(lb->name == name)
            return ((lb->type != L_UNDEF || ret_always) ? lb : nullptr);
    }
    Z_LABEL lb;
    lb.name  = name; lb.type  = L_UNDEF; lb.value = 0; *labels  += lb;
    return (ret_always ? &labels->at(labels->size() - 1) : nullptr);
}
