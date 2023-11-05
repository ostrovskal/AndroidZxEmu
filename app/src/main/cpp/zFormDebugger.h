//
// Created by User on 20.07.2023.
//

#pragma once

#include "zRibbonDebugger.h"
#include "emu/zCpus.h"

// режим регистров
constexpr int SD_TOOLS_REG		= 1;
// режим списка
constexpr int SD_TOOLS_LST		= 2;
// режим кнопок
constexpr int SD_TOOLS_BUT		= 4;
// при изменении регистра
constexpr int SD_TOOLS_CH_REG	= 8;
// при изменении размера регистров
constexpr int SD_TOOLS_CH_16	= 16;
// при изменении PC/SP
constexpr int SD_TOOLS_CH_SP_PC = 32;
// при изменении процессора
constexpr int SD_TOOLS_CH_CPU	= 128;
constexpr int SD_TOOLS_ALL		= 7;

class zFormDebugger : public zViewForm {
public:
    struct Z_DEBUGGER {
        // заголовок
        cstr title;
        // формат отображения числа
        int fmtValue;
        // тип базового адреса
        int baseType;
        // идентификатор текста
        int idText;
        // идентификатор поля ввода
        int idEdit;
        // смещение к данным
        int offset;
        // маска
        u32 mask;
        // сдвиг
        int shift;
        // полученное значение
        int value{0};
        // хэндлы контролов
        zViewEdit* edit{nullptr};
        zViewText* text{nullptr};
        // указатель на базовый адрес значения
        u8* base{nullptr};
    };
    zFormDebugger(zStyle* _sts, i32 _id, zStyle* _sts_capt, zStyle* _sts_foot, u32 _capt, bool _m) :
            zViewForm(_sts, _id, _sts_capt, _sts_foot, _capt, _m) { }
    virtual int updateVisible(bool set) override;
    virtual void onInit(bool _theme) override;
    virtual bool stateChildren() const override { return false; }
    void onCommand(int id, bool dbl);
    void stateTools(int action, int id = 0);
protected:
    // заполнить базовый адрес в структурах
    void fillRegisters(bool full);
    // процессор для отладки
    zCpu* cpu{nullptr};
    // список
    zRibbonDebugger* _list{nullptr};
    // поле ввода aсм текста
    zViewEdit*  _asm{nullptr};
};
