//
// Created by User on 22.07.2023.
//

#pragma once

class zFormBpList : public zViewForm {
public:
    enum { VW_COND, VW_OPS, VW_ADDR1, VW_ADDR2, VW_VAL, VW_MSK, VW_LST };
    zFormBpList(zStyle* _sts, i32 _id, zStyle* _scapt, zStyle* _sfoot, u32 _capt, bool _m) :
            zViewForm(_sts, _id, _scapt, _sfoot, _capt, _m) { }
    virtual void onInit(bool _theme) override;
    virtual void close(int code) override;
    zArray<zString8> cond{}, ops{};
protected:
    BREAK_POINT* getAccess(bool ops);
    void stateTools(bool list);
    void updateBp();
    bool disable1{true}, disable2{true};
    zViewRibbon* _lst{nullptr};
    zViewSelect* _cond{nullptr}, *_ops{nullptr};
    zViewEdit* _val{nullptr}, *_msk{nullptr}, *_addr1{nullptr}, *_addr2{nullptr};
};
