//
// Created by User on 22.07.2023.
//

#include "sshCommon.h"
#include "zFormBpList.h"

static zArray<zString8> makeBpValues(BREAK_POINT* bp) {
    zArray<zString8> arr;
    arr += zString8(bp->address1, ZFV_OPS16, speccy->showHex);
    arr += zString8(bp->address2, ZFV_OPS16, speccy->showHex);
    arr += zString8(bp->val, ZFV_OPS8, speccy->showHex);
    arr += zString8(bp->msk, ZFV_OPS8, speccy->showHex);
    return arr;
}

class zFabricBpListItem : public zBaseFabric {
public:
    explicit zFabricBpListItem() : zBaseFabric(styles_z_list_item) { }
    zView* make(zViewGroup* parent) override {
        auto v(new zLinearLayout(styles_default, 0, false));
        for(int i = 0 ; i < 6; i++) v->attach(new zViewText(styles_bptext, 0, 0), VIEW_MATCH, VIEW_MATCH);
        v->lps.set(0, 0, VIEW_MATCH, VIEW_WRAP);
        v->onInit(false);
        return v;
    }
};

class zAdapterBpList : public zAdapterList {
public:
    explicit zAdapterBpList(zFormBpList* _bp) : zAdapterList({}, new zFabricBpListItem()), bps(_bp) { }
    [[nodiscard]] int getCount() const override { return 12; }
    zView* getView(int position, zView* convert, zViewGroup* parent) override {
        auto nv((zLinearLayout*)createView(position, convert, parent, fabricBase, false));
        auto bp(&speccy->getCpu(speccy->debugCpu)->bps[position]);
        auto abp(makeBpValues(bp)); zString8 s;
        auto disable1(bp->type < ZX_BP_RMEM), disable2(bp->type == ZX_BP_NONE);
        for(int i = 0 ; i < 5; i++) {
            if(disable2) s = "-------";
            else if(i > 1) {
                s = (disable1 ? "-------" : (i == 4 ? bps->cond[bp->cond] : abp[i]));
            } else {
                s = abp[i];
            }
            nv->atView<zViewText>(i)->setText(s);
        }
        nv->atView<zViewText>(5)->setText(bps->ops[bp->type]);
        return nv;
    }
    zFormBpList* bps{nullptr};
};

void zFormBpList::close(int code) {
    speccy->getCpu(speccy->debugCpu)->filledBPS();
    zViewForm::close(code);
}

void zFormBpList::onInit(bool _theme) {
    auto change = [this](zView*, int) { updateBp(); };
    zViewForm::onInit(_theme);
    if(isOpen) {
        cond = theme->findArray(R.string.bp_cond); ops = theme->findArray(R.string.bp_ops2);
        _lst = idView<zViewRibbon>(R.id.bpList); _lst->setAdapter(new zAdapterBpList(this));
        _lst->setItemSelected(0); _lst->setOnChangeSelected([this](zView*, int s) { stateTools(true); });
        _cond = idView<zViewSelect>(R.id.bpSpinCond); _cond->setOnChangeSelected(change);
        _ops = idView<zViewSelect>(R.id.bpSpinOps); _ops->setOnChangeSelected(change);
        _val = idView<zViewEdit>(R.id.bpEditValue); _val->setOnChangeText(change);
        _msk = idView<zViewEdit>(R.id.bpEditMask); _msk->setOnChangeText(change);
        _addr1 = idView<zViewEdit>(R.id.bpEditRange1); _addr1->setOnChangeText(change);
        _addr2 = idView<zViewEdit>(R.id.bpEditRange2); _addr2->setOnChangeText(change);
        stateTools(true);
    }
}

BREAK_POINT* zFormBpList::getAccess(bool o) {
    auto item(_lst->getItemSelected());
    auto bp(&speccy->getCpu(speccy->debugCpu)->bps[item]);
    auto access(o ? _ops->getItemSelected() : bp->type);
    disable1 = access < ZX_BP_RMEM; disable2 = access == ZX_BP_NONE;
    return bp;
}

void zFormBpList::stateTools(bool list) {
    auto bp(getAccess(!list));
    if(list) {
        auto abp(makeBpValues(bp));
        _cond->setItemSelected(bp->cond); _ops->setItemSelected(bp->type);
        _addr1->setText(disable2 ? "" : abp[0]); _addr2->setText(disable2 ? "" : abp[1]);
        _val->setText(disable1 ? "" : abp[2]); _msk->setText(disable1 ? "" : abp[3]);
    }
    _cond->disable(disable1); _val->disable(disable1); _msk->disable(disable1);
    _addr1->disable(disable2); _addr2->disable(disable2);
}

void zFormBpList::updateBp() {
    auto bp(getAccess(true));
    int o(0), f(0), a1(0), a2(0), v(0), m(0);
    if(!disable2) {
        auto _1(_addr1->getText()), _2(_addr2->getText());
        if(_1.isEmpty() || _2.isEmpty()) { disable2 = true; }
        else {
            a1 = _1.toNum(); a2 = _2.toNum();
            if(a1 <= a2) {
                if(!disable1) {
                    _1 = _val->getText(); _2 = _msk->getText();
                    if(_1.isEmpty() || _2.isEmpty()) { disable2 = true; }
                    else {
                        v = _1.toNum(); m = _2.toNum();
                        f = _cond->getItemSelected();
                    }
                }
                o = _ops->getItemSelected();
            }
        }
    }
    if(disable2) { a1 = a2 = v = m = f = o = 0; }
    bp->address1 = a1; bp->address2 = a2; bp->type = o;
    bp->val = v; bp->msk = m; bp->cond = f;
    _lst->requestPosition();
    stateTools(false);
}
