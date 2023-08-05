//
// Created by User on 23.07.2023.
//

#include "sshCommon.h"
#include "zFormSelFile.h"

void zFormSelFile::onInit(bool _theme) {
    zViewForm::onInit(_theme);
    if(isOpen) {
        setOnClose([](zViewForm*, int) { return 0; });
    }
}

void zFormSelFile::setParams(zFile* fl, int option) {

}

i32 zFormSelFile::updateVisible(bool set) {
    if(set) {

    }
    return zViewForm::updateVisible(set);
}
