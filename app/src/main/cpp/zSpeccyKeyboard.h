//
// Created by User on 20.07.2023.
//

#pragma once

#include "zostrov/zViewKeyboard.h"

class zSpeccyKeyboard : public zViewKeyboard {
public:
    // Режимы курсора
    enum CursorMode { MODE_K = 0, MODE_L, MODE_C, MODE_E, MODE_SE, MODE_SK, MODE_CL, MODE_CK, MODE_G, MODE_CG, MODE_CE };
    zSpeccyKeyboard() : zViewKeyboard("zxKeyboard.xml", styles_zxkeyb, styles_zxkeyb, 0) { owner = this; }
    void updateMode();
    // обработка кнопки
    virtual i32 keyEvent(int key, bool sysKey) override;
    // отобразить/скрыть
    virtual void show(u32 _id, bool set) override { }
    virtual i32 onTouchEvent() override;
    virtual zString8 typeName() const override { return "zSpeccyKeyboard"; }
protected:
    virtual void onMeasure(cszm& spec) override;
    virtual void onDraw() override;
};
