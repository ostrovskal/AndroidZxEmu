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
            if(!touch->isDblClicked()) return;
            zFileAsset fl(pth, true); zFile::zFileInfo zfi{};
            if(fl.info(zfi, sel)) {
                // сформировать полный путь к кэшу
                auto path(settings->makePath(zfi.path.substrAfterLast("/", zfi.path), FOLDER_CACHE));
                // распаковать файл в папку кэша
                auto ret(fl.copy(path, zfi.index));
                if(ret) {
                    frame->send(ZX_MESSAGE_LOAD, 0, 0, path);
                    ret = path.substrAfterLast(".") != "$";
                }
                if(ret) close(z.R.id.ok);
            }
        });
    }
}

void zFormSelFile::setParams(zFileAsset* _fl, int _option) {
    zFile::zFileInfo zfi; zString8Array arrs;
    for(int i = 0 ; i < _fl->countFiles(); i++) {
        if(_fl->info(zfi, i)) arrs += zfi.path;
    }
    option = _option; pth = _fl->pth();
    lst->setAdapter(new zAdapterList(arrs, new zFabricListItem(styles_browslisttext)));
}
