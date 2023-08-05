//
// Created by User on 23.07.2023.
//

#pragma once

class zFormTZX : public zViewForm {
public:
    zFormTZX(zStyle* _sts, i32 _id, zStyle* _sts_capt, zStyle* _sts_foot, u32 _capt, bool _m) :
            zViewForm(_sts, _id, _sts_capt, _sts_foot, _capt, _m) { }
    virtual i32 updateVisible(bool set) override;
    virtual void onInit(bool _theme) override;
    zViewForm* setMode(int _mode, czs& info);
protected:
    struct Z_SELECT { zViewButton* but{nullptr}; int offs; };
    // блок ленты
    zDevTape::BLK_TAPE* block{nullptr};
    // кнопки(меню выбора)
    Z_SELECT buttons[10]{};
    // режим
    int mode{-1}, pause{0};
    zString8 message{};
    zLinearLayout* tzxSel{nullptr};
    zViewText* tzxMsg{nullptr};
};
