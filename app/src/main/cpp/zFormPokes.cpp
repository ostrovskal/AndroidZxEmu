//
// Created by User on 23.07.2023.
//

#include "sshCommon.h"
#include "zFormPokes.h"

class zFabricPokesItem : public zFabricListItem {
public:
    zFabricPokesItem() : zFabricListItem(styles_z_list_item) { }
    zView* make(zViewGroup* parent) override {
        auto v(new zLinearLayout(styles_default, 0, false));
        v->attach(new zViewText(styles_diskheadtext, 0, 0),VIEW_MATCH, VIEW_WRAP);
        v->attach(new zViewButton(styles_debbut, 0, 0, R.integer.iconZxOK), VIEW_WRAP, VIEW_WRAP);
        v->lps.set(0, 0, VIEW_MATCH, VIEW_WRAP);
        v->onInit(false);
        v->setGravity(ZS_GRAVITY_VCENTER);
        return v;
    }
};

class zAdapterPokesList : public zAdapterList {
public:
    explicit zAdapterPokesList() : zAdapterList({""}, new zFabricPokesItem()) { }
    zView* getView(int position, zView* convert, zViewGroup* parent) override {
        auto nv((zLinearLayout*)createView(position, convert, parent, fabricBase, false));
        nv->atView<zViewText>(0)->setText(getItem(position).substrBefore("|"));
        nv->atView<zViewButton>(1)->setOnClick([](zView*, int) {
            DLOG("poke click");
        });
        return nv;
    }
};

void zFormPokes::onInit(bool _theme) {
    zViewForm::onInit(_theme);
    if(isOpen) {
        lst = idView<zViewRibbon>(R.id.pokesList);
        spn = idView<zViewSelect>(R.id.pokesSpin);
        lst->setAdapter(new zAdapterPokesList());
        spn->setOnChangeSelected([this](zView*, int s) { setListPokes(s); });
    }
}

i32 zFormPokes::updateVisible(bool set) {
    speccy->joyMakePresets(R.id.pokesSpin);
    return zViewForm::updateVisible(set);
}

void zFormPokes::setListPokes(int idx) {
    auto pks(speccy->findJoyPokes(idx));
    if(pks) {
        auto adapt(lst->getAdapter());
        adapt->clear(false); adapt->addAll(pks->pokes);
    }
}