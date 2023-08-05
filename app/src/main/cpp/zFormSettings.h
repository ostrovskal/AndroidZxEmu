//
// Created by User on 20.07.2023.
//

#pragma once

class zFormSettings : public zViewForm {
public:
    zFormSettings(zStyle* _sts, i32 _id, zStyle* _sts_capt, zStyle* _sts_foot, u32 _capt, bool _m);
    virtual void onInit(bool _theme) override;
protected:
};
