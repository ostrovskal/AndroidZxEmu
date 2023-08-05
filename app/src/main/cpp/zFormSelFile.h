//
// Created by User on 23.07.2023.
//

#pragma once

class zFormSelFile : public zViewForm {
public:
    zFormSelFile(zStyle* _sts, int _id, zStyle* _scapt, zStyle* _sfoot, int _capt, bool _m) :
            zViewForm(_sts, _id, _scapt, _sfoot, _capt, _m) { }
    virtual void onInit(bool _theme) override;
    virtual i32 updateVisible(bool set) override;
    void setParams(zFile* _fl, int option);
protected:
    zViewRibbon* lst{nullptr};
    int option{0}; zFile* fl{nullptr};
};
