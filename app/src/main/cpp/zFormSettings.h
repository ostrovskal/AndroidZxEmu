//
// Created by User on 20.07.2023.
//

#pragma once

class zFormSettings : public zViewForm {
public:
    zFormSettings(zStyle* _sts, i32 _id, zStyle* _sts_capt, zStyle* _sts_foot, u32 _capt, bool _m) :
            zViewForm(_sts, _id, _sts_capt, _sts_foot, _capt, _m) { }
    virtual void onInit(bool _theme) override;
    virtual i32 updateVisible(bool set) override;
protected:
    void onInit(zView* v, int a1);
    void onCommand(zView* v, int a1);
    void makeDiskCatalog(int num);
    void applyPalette();
    void applyJoyStd(int num);
    void applyJoyPresets(int num);
    void applyColorSlider();
    // текущий цвет
    zView* selColor{nullptr};
    int argColor{0}, joyType{0};
    u8 joyKeys[8]{};
    // сохраненные значения
    u8 savedValues[173];
};
