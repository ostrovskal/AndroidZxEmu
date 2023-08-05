//
// Created by User on 23.07.2023.
//

#include "sshCommon.h"
#include "zFormSelFile.h"

void zFormSelFile::onInit(bool _theme) {
    zViewForm::onInit(_theme);
    if(isOpen) {
        lst = idView<zViewRibbon>(R.id.selList);
        lst->setOnClick([this](zView*, int sel) {
            if(touch->isDblClicked()) {
                auto path(lst->getAdapter()->getItem(sel));
                if(!speccy->_load(fl, sel, option)) onClose(this, z.R.id.no);
                else {
                    auto ext(path.substrAfterLast("."));
                    if(ext[0] != '$') onClose(this, z.R.id.ok);
                }
            }
        });
//        setOnClose([](zViewForm*, int) { return 0; });
    }
}

void zFormSelFile::setParams(zFile* _fl, int _option) {
    zFile::zFileInfo zfi; zString8Array arrs;
    for(int i = 0 ; i < fl->countFiles(); i++) {
        if(fl->info(zfi, i)) arrs += zfi.path;
    }
    lst->setAdapter(new zAdapterList(arrs, new zFabricListItem(styles_browslisttext)));
    option = _option; fl = _fl;
}

i32 zFormSelFile::updateVisible(bool set) {
    if(set) {

    }
    return zViewForm::updateVisible(set);
}
