//
// Created by User on 23.07.2023.
//

#pragma once

class zFormPokes : public zViewForm {
public:
    zFormPokes(zStyle* _sts, int _id, zStyle* _sts_capt, zStyle* _sts_foot, int _capt, bool _m) :
        zViewForm(_sts, _id, _sts_capt, _sts_foot, _capt, _m) { }
    virtual i32 updateVisible(bool set) override;
    virtual void onInit(bool _theme) override;
    virtual zString8 typeName() const override { return "zFormPokes"; }
protected:
    void setListPokes(int idx);
    zViewRibbon* lst{nullptr};
    zViewSelect* spn{nullptr};
};
