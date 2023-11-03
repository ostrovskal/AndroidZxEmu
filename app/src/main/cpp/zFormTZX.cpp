//
// Created by User on 23.07.2023.
//

#include "sshCommon.h"
#include "zFormTZX.h"

[[maybe_unused]] void zFormTZX::stateView(STATE& state, bool save, int& index) {
    //if(!save) updateVisible(true);
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
//        ILOG("idxBlkUI %i count %i", speccy->indexBlkUI, speccy->tapeCount);
        auto block(speccy->dev<zDevTape>()->blockInfo(speccy->indexBlkUI));
        if(block) {
            int pause(0);
            switch(block->type) {
                case TZX_STOP_48K:
                    tzxMsg->setText("MODE 48K: STOP TAPE!");
                    pause = 5;
                    tzxMsg->updateStatus(ZS_VISIBLED, true);
                    break;
                case TZX_MESSAGE:
                    tzxMsg->setText(zString8(block->data, (int) block->size));
                    pause = block->pause;
                    tzxMsg->updateStatus(ZS_VISIBLED, true);
                    break;
                case TZX_SELECT: {
                    tzxSel->removeAllViews(true);
                    // кол-во
                    auto data(block->data); auto count((int) *data++);
                    // надписи
                    for(int i = 0; i < count; i++) {
                        auto offs(*(u16*)data); data += 2;
                        auto len(*data++); auto txt(zString8(data, len)); data += len;
                        auto but(new zViewButton(styles_buttzxui, i + 1000, 0));
                        but->setText(txt); but->setOnClick([this, offs](zView *v, int) {
                            zxCmd(ZX_CMD_TAPE_UI, speccy->indexBlkUI + offs);
                            tzxSel->updateStatus(ZS_VISIBLED, false);
                        });
                        tzxSel->attach(but, VIEW_WRAP, VIEW_WRAP);
                    }
                    tzxSel->updateStatus(ZS_VISIBLED, true);
                    break;
                }
            }
            if(pause) frame->post(MSG_HIDE_TZX_INFO, pause * 1000);
        } else {
            ILOG("block info undefined! %i", speccy->indexBlkUI);
        }
    }
    return zViewForm::updateVisible(set);
}
