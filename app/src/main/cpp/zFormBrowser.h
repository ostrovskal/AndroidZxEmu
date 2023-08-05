//
// Created by User on 20.07.2023.
//

#pragma once

#include <zstandard/zCloud.h>

class zFormBrowser : public zViewForm {
public:
    enum { BUT_OPEN, BUT_FOLDER, BUT_SAVE, BUT_DEL, BUT_EJECT, BUT_TRDOS };
    enum { FLT_SNAP, FLT_TAP, FLT_NET, FLT_DSK };
    zFormBrowser(zStyle* _sts, i32 _id, zStyle* _sts_capt, zStyle* _sts_foot, u32 _capt, bool _m) :
            zViewForm(_sts, _id, _sts_capt, _sts_foot, _capt, _m) { }
    virtual void onInit(bool _theme) override;
    virtual i32 updateVisible(bool set) override;
protected:
    virtual i32 touchEventDispatch(AInputEvent *event) override;
    void onCommand(int id);
    void setFilter(int filter, bool force = false);
    void updateControls();
    void updateList();
    bool checkAuth();
    void makeList(const zArray<zFile::zFileInfo>& arr);
    zViewEdit* edt{nullptr};
    zViewRibbon* lst{nullptr};
    zViewSelect* flt{nullptr};
    zViewButton* but[6]{};
    int filter{-1};
    zString8 root{}, current{}, diskPath{};
    zString8Array files{};
    zString8Array fltExt{};
    zCloudMail* dbx{nullptr};
    u64 timeAuth{0};
    std::thread* th{nullptr};
};
