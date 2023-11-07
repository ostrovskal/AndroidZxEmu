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
    virtual zString8 typeName() const override { return "zFormTZX"; }
protected:
    // режим
    zLinearLayout* tzxSel{nullptr};
    zViewText* tzxMsg{nullptr};
};
