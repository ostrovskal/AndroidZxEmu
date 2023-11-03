//
// Created by User on 23.07.2023.
//

#include "sshCommon.h"
#include "zFormTZX.h"

zViewForm* zFormTZX::setMode(int _mode, czs& info) {
    mode = -1; block = nullptr;
    if(_mode != TZX_QINFO) {
        block = speccy->dev<zDevTape>()->blockInfo(speccy->indexBlkUI);
        if(!block) { ILOG("block info undefined!"); return this; }
        _mode = block->type;
    }
    mode = _mode; pause = 0;
    switch(mode) {
        case TZX_SELECT:
            break;
        case TZX_STOP_48K:
            message = "MODE 48K: STOP TAPE!";
            pause = 5;
            break;
        case TZX_MESSAGE:
            message = zString(block->data, (int)block->size);
            pause = block->pause;
            break;
        case TZX_QINFO:
            message = info;
            pause = 3;
            break;
    }
    pause *= 1000;
    return this;
}

void zFormTZX::stateView(STATE& state, bool save, int& index) {
    if(save) {
        state.data += mode;
        state.string = message;
    } else {
        setMode(state.data[index++], state.string);
    }
    zViewForm::stateView(state, save, index);
}

void zFormTZX::onInit(bool _theme) {
    zViewForm::onInit(_theme);
    if(isOpen) {
        tzxSel = idView<zLinearLayout>(R.id.tzxSel);
        tzxMsg = idView<zViewText>(R.id.tzxMsg);
        setOnClose([](zViewForm*, int) { return 0; });
    }
}

int zFormTZX::updateVisible(bool set) {
    if(set) {
        tzxMsg->updateStatus(ZS_VISIBLED, false);
        tzxSel->updateStatus(ZS_VISIBLED, false);
        switch(mode) {
            case TZX_SELECT: {
                //speccy->execLaunch = false;
                frame->send(ZX_MESSAGE_PROPS);
                tzxSel->removeAllViews(false);
                // кол-во
                auto data(block->data); auto count((int)*data++);
                // надписи
                for(int i = 0 ; i < count; i++) {
                    buttons[i].offs = *(u16*)data; data += 2;
                    auto len(*data++); auto txt(zString8(data, len)); data += len;
                    auto but(new zViewButton(styles_buttzxui, i + 1000, 0));
                    but->setText(txt); but->setOnClick([this](zView* v, int) {
                        zxCmd(ZX_CMD_TAPE_UI, speccy->indexBlkUI + buttons[v->id - 1000].offs);
                        //speccy->execLaunch = true;
                        tzxSel->updateStatus(ZS_VISIBLED, false);
                    });
                    buttons[i].but = but; tzxSel->attach(but, VIEW_WRAP, VIEW_WRAP);
                }
                tzxSel->updateStatus(ZS_VISIBLED, true);
                break;
            }
            case TZX_STOP_48K:
            case TZX_MESSAGE:
            case TZX_QINFO:
                tzxMsg->setText(message);
                tzxMsg->updateStatus(ZS_VISIBLED, true);
                tzxSel->updateStatus(ZS_VISIBLED, false);
                break;
        }
        if(pause) frame->post(MSG_HIDE_TZX_INFO, pause);
    }
    return zViewForm::updateVisible(set);
}
