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
    zArray<zString8> cond{}, ops{};
protected:
    template<typename T> T* getView(int idx) const { return dynamic_cast<T*>(views[idx]); }
    void stateTools(bool list);
    void updateBp();
    zView* views[7]{};
};
