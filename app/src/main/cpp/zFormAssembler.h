//
// Created by User on 22.07.2023.
//

#pragma once

#define ERROR_OK                            0
#define CMD_ORG                             1
#define CMD_END                             2
#define ERROR_INVALID_COMMAND               -1
#define ERROR_COMMA_NOT_FOUND               -2
#define ERROR_PARAMETER_OUT_OF_RANGE        -3
#define ERROR_DISP_OUT_OF_RANGE             -4
#define ERROR_RELATIVE_JUMP_OUT_OF_RANGE    -5
#define ERROR_NUMBER_OUT_OF_RANGE           -6
#define ERROR_DISP_NOT_FOUND                -7
#define ERROR_COMMAND_NOT_FOUND             -8
#define ERROR_CLOSE_BRAKKET_NOT_FOUND       -9
#define ERROR_INVALID_INDEXED_OPERATION     -10
#define ERROR_INVALID_RST_NUMBER            -11
#define ERROR_INVALID_INTERRUPT_MODE        -12
#define ERROR_KEYWORD_NOT_FOUND             -13
#define ERROR_EXPECT_END_OF_COMMAD          -14
#define ERROR_BIT_OUT_OF_RANGE              -15
#define ERROR_INVALID_ADDRESS_OPERAND       -16
#define ERROR_DUPLICATE_LABEL               -17
#define ERROR_LABEL_NOT_FOUND               -18
#define ERROR_INVALID_OPERAND               -19
#define ERROR_NOT_FOUND_DISP_OFFSET         -20
#define ERROR_VALUE_OUT_OF_RANGE            -21
#define ERROR_INVALID_ESCAPE_SEQUENCE       -22
#define ERROR_INVALID_STRING                -23
#define ERROR_INVALID_BRACKET               -24
#define ERROR_UNDEFINED_OPERATION           -25
#define ERROR_WRONG_OPERATION               -26
#define ERROR_UNEXPECTED_LEXEM              -27
#define ERROR_INVALID_EXPRESSION            -28

class zFormAssembler : public zViewForm {
public:
    enum { PAR_COUNT_EMPTY, PAR_COUNT_ONE, PAR_COUNT_TWO, PAR_COUNT_ONE_OR_TWO, PAR_COUNT_TWO_OR_THREE };

    zFormAssembler(zStyle* _sts, i32 _id, zStyle* _sts_capt, zStyle* _sts_foot, u32 _capt, bool _m);
    virtual void onInit(bool _theme) override;
/*
    zAssembler() : lexPos(nullptr), text(nullptr), lb(nullptr), labels(nullptr), buf(nullptr),
                   number(0), icmd(0), disp(0), scmd(0), rep(1), pc(0), pass(0), is_lb(0) {
        memset(cmdBuf, 0, sizeof(cmdBuf));
    }
*/
    // парсер
    int parser(int pass, int address, const char *text, zArray<Z_LABEL>* labels);
    // скопировать инструкцию в буфер
    int copy(uint8_t* address) {
        memcpy(address, cmdBuf, scmd);
        return scmd;
    }
    int copy(zCpu* cpu, uint16_t address) {
        cpu->copy(address, cmdBuf, scmd);
        return scmd;
    }
    // признак инструкции
    bool isCmd() const { return rep || scmd; }
    // текущий начальный адрес
    int getOrg() const { return *(u16*)cmdBuf; }
    // число повторений команды
    int getRepeat() const { return rep; }
    // парсер выражения
    int quickParser(const char* text);
    // размер инструкции
    int scmd;
protected:
    // преобразование выражения во внутреннюю структуру
    int exprConvert(char** place);
    // парасер операции выражения
    int parseOps();
    // парсер строки
    bool parseString(char** place, int& ret);
    // парсер простого выражения(в скобках)
    int parseSubExpress(char* place);
    // парсер выражения
    int parseExpress(char** place, bool disp);
    // парсер операнда инструкции
    int parseOperand(char** text);
    // парсер инструкций
    int parserMnemonic();
    // парсер оператора
    int parserOperator(int len);
    // проверка на флаги
    bool checkFlags(int flags, bool is_jr);
    // проверка на 16 бит регистры
    bool checkReg16(int reg, bool is_sp);
    // проверк на 8 бит регистры
    bool checkReg8(int reg);
    // добавление метки
    Z_LABEL* add_label(char* place, int len, bool ret_always);
    // получение следующей лексемы
    int nextLexem(char** text);
    // получение кода ключевого слова
    int getCmd(const char* cmd, int len);
    // пропуск запятой, если есть
    int skipComma(char** text);
    // начальная позиция анализатора
    char* lexPos;
    // текст строки
    char* text;
    int icmd, pc, pass, is_lb, rep;
    // буфер инструкции
    uint8_t cmdBuf[16];
    // смещение
    uint8_t disp;
    // число
    int number;
    // текущая позиция буфера инструкции
    uint8_t* buf;
    // текущая метка
    Z_LABEL* lb;
    // массив меток
    zArray<Z_LABEL>* labels;
};
