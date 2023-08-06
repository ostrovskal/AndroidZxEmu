//
// Created by User on 22.07.2023.
//

#include "sshCommon.h"
#include "zFormBpList.h"

static zArray<zString8> makeBpValues(BREAK_POINT* bp) {
    return z_fmt8("%s|%s|%s|%s",    z_fmtValue(bp->address1, ZFV_OPS16, speccy->showHex),
                                    z_fmtValue(bp->address2, ZFV_OPS16, speccy->showHex),
                                    z_fmtValue(bp->val, ZFV_OPS8, speccy->showHex),
                                    z_fmtValue(bp->msk, ZFV_OPS8, speccy->showHex)).split("|");
}

class zFabricBpListItem : public zFabricListItem {
public:
    explicit zFabricBpListItem() : zFabricListItem(styles_z_list_item) { }
    zView* make(zViewGroup* parent) override {
        auto v(new zLinearLayout(styles_default, 0, false));
        for(int i = 0 ; i < 6; i++) v->attach(new zViewText(styles_diskheadtext, 0, 0),VIEW_MATCH, VIEW_MATCH);
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
        auto bp(speccy->getCpu(speccy->debugCpu)->bps[position]);
        auto abp(makeBpValues(&bp));
        auto nil(bp.ops == ZX_BP_NONE);
        for(int i = 0 ; i < 5; i++) nv->atView<zViewText>(i)->setText(nil ? "-------" : i == 4 ? bps->cond[bp.flg] : abp[i]);
        nv->atView<zViewText>(5)->setText(bps->ops[bp.ops]);
        return nv;
    }
    zFormBpList* bps{nullptr};
};

void zFormBpList::onInit(bool _theme) {
    zViewForm::onInit(_theme);
    if(isOpen) {
        static int ids[] = { R.id.bpSpinCond, R.id.bpSpinOps, R.id.bpEditRange1, R.id.bpEditRange2,
                             R.id.bpEditValue1, R.id.bpEditValue2, R.id.bpList };
        for(int i = 0 ; i < 8; i++) views[i] = idView(ids[i]);
        cond = theme->findArray(R.string.bp_cond);
        ops = theme->findArray(R.string.bp_ops);
        getView<zViewRibbon>(VW_LST)->setAdapter(new zAdapterBpList(this))->setOnChangeSelected([this](zView*, int s) { stateTools(true); });
        setOnClose([](zViewForm*, int) { return 1; });
        getView<zViewSelect>(VW_COND)->setOnChangeSelected([this](zView*, int) { updateBp(); });
        getView<zViewSelect>(VW_OPS)->setOnChangeSelected([this](zView*, int) { updateBp(); });
        getView<zViewEdit>(VW_VAL)->setOnChangeText([this](zView*, int) { updateBp(); });
        getView<zViewEdit>(VW_MSK)->setOnChangeText([this](zView*, int) { updateBp(); });
        getView<zViewEdit>(VW_ADDR1)->setOnChangeText([this](zView*, int) { updateBp(); });
        getView<zViewEdit>(VW_ADDR2)->setOnChangeText([this](zView*, int) { updateBp(); });
        stateTools(false);
    }
}

void zFormBpList::stateTools(bool list) {
    if(list) {
        auto item(idView<zViewRibbon>(R.id.bpList)->getItemSelected());
        auto bp(speccy->getCpu(speccy->debugCpu)->bps[item]);
        auto abp(makeBpValues(&bp));
        getView<zViewSelect>(VW_COND)->setItemSelected(bp.flg);
        getView<zViewSelect>(VW_OPS)->setItemSelected(bp.ops);
        getView<zViewEdit>(VW_VAL)->setText(abp[2]);
        getView<zViewEdit>(VW_MSK)->setText(abp[3]);
        getView<zViewEdit>(VW_ADDR1)->setText(abp[0]);
        getView<zViewEdit>(VW_ADDR2)->setText(abp[1]);
    }
    auto access(idView<zViewSelect>(R.id.bpSpinOps)->getItemSelected());
    auto disable1(access < ZX_BP_RMEM), disable2(access == ZX_BP_NONE);
    views[VW_COND]->disable(disable1);
    views[VW_VAL]->disable(disable1);
    views[VW_MSK]->disable(disable1);
    views[VW_ADDR1]->disable(disable2);
    views[VW_ADDR2]->disable(disable2);
}

void zFormBpList::updateBp() {
    stateTools(false);
}
