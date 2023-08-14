//
// Created by User on 20.07.2023.
//

#include "sshCommon.h"
#include "emu/zSpeccy.h"
#include "zFormTZX.h"
#include "zFormDebugger.h"

static int valIds[] = { R.integer.MENU_DEBUGGER_ADDR, ZSI_SHOW_DEBUG_ADDR, R.integer.MENU_DEBUGGER_CODE, ZSI_SHOW_DEBUG_CODE,
                        R.integer.MENU_DEBUGGER_CONTENT, ZSI_SHOW_DEBUG_CONTENT, R.integer.MENU_PROPS_SOUND, ZSI_SND_LAUNCH,
                        R.integer.MENU_PROPS_MAGNITOPHON, ZSI_SHOW_TAPE, R.integer.MENU_PROPS_FILTER, ZSI_FILTER,
                        R.integer.MENU_PROPS_TURBO, ZSI_TURBO_MODE, R.integer.MENU_PROPS_EXECUTE, ZSI_EXEC_LAUNCH,
                        R.integer.MENU_PROPS_DEBUGGER, ZSI_SHOW_DEBUGGER, -1 };

zSpeccyLayout::~zSpeccyLayout() {
    SAFE_DELETE(speccy);
}

bool zSpeccyLayout::init() {
    menu = theApp->getActionBar()->getMenu();
    auto root(theApp->getSystemView(true));
    // меню ленты
    auto tapeLyt((zLinearLayout*)attach(new zLinearLayout(styles_default, R.id.llTape, manager->isLandscape()),
                                            ZS_GRAVITY_START | ZS_GRAVITY_TOP, 0, VIEW_WRAP, VIEW_WRAP, 0));
    tapePlay  = new zViewButton(styles_debbut, R.id.tapePlay, 0, R.integer.iconZxPlay);
    tapeTurbo = new zViewButton(styles_debbut, R.id.tapeTurbo, 0, R.integer.iconZxAccelOff);
    tapeProgress = new zViewProgress(styles_z_linearprogress, R.id.tapeProgress, 0, szi(0, 100), 70, manager->isLandscape());
    tapeLyt->attach(tapeTurbo, VIEW_WRAP, VIEW_WRAP)->setOnClick([this](zView*, int d) {
        speccy->speedTape ^= 1; stateTools(ZFT_UPD_TAPE, 0); });
    tapeLyt->attach(tapePlay, VIEW_WRAP, VIEW_WRAP)->setOnClick([this](zView*, int d) {
        auto tp(speccy->dev<zDevTape>());
        if(!speccy->playTape) tp->startPlay(); else tp->stopPlay();
        stateTools(ZFT_UPD_TAPE);
    });
    tapeLyt->attach(tapeProgress, VIEW_MATCH, VIEW_MATCH);
    // заголовок/fps
    capt = new zViewText(styles_textcapt, 0, R.string.msg_error);
    theApp->getActionBar()->setContent(capt);
    fps = idView<zViewText>(R.id.speccyFps);
    // джойстики
    ac = (zViewController*)root->attach(new zViewController(styles_z_acontroller, z.R.id.acontroller, z.R.integer.acontrol, z.R.string.acontrollerMap),
                                        ZS_GRAVITY_END | ZS_GRAVITY_BOTTOM, 0, 100_dp, 128_dp);
    cc = (zViewController*)root->attach(new zViewController(styles_z_ccontroller, z.R.id.ccontroller, z.R.integer.ccontrol, z.R.string.ccontrollerMap),
                                        ZS_GRAVITY_START | ZS_GRAVITY_BOTTOM, 0, 100_dp, 128_dp);
    cc->setOnChangeButton([this](zView*, int b) {
        theApp->keyb->keyEvent(b | (int)(ac->getButtons() << 4), false); });
    ac->setOnChangeButton([this](zView*, int b) { theApp->keyb->keyEvent((b << 4) | (int)cc->getButtons(), false); });
    _status = idView<zViewImage>(R.id.speccyStatus);
    // эмулятор
    speccy = new zSpeccy();
    if(speccy->init()) {
        // filter
        drw[DRW_FK]->texture->enableFilter(speccy->filter);
        return true;
    }
    return false;
}

void zSpeccyLayout::setCaption() {
    zString8 txt;
    if(zSpeccy::machine) {
        auto clock((int)zSpeccy::machine->cpuClock);
        auto nmode((speccy->execLaunch ? (speccy->turboMode ? "TURBO" : "NORMAL") : "PAUSE"));
        auto mode(z_fmt8("%s - %.02fMhz", nmode, (float)speccy->turboTick(clock, false) / 1000000.0f));
        txt = z_fmt8("%s [%s %s]", speccy->progName.str(), zSpeccy::machine->name, mode.str());
    }
    capt->setText(txt);
}

void zSpeccyLayout::updateTexture() {
    if(speccy->fps) fps->setText(z_fmt8("%0.2f", speccy->nfps));
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.w, size.h, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, speccy->dev<zDevUla>()->frameBuffer + 101376);
}

void zSpeccyLayout::notifyEvent(HANDLER_MESSAGE* msg) {
    switch(msg->what) {
        // отобразить форму информации TZX
        case MSG_SHOW_TZX_INFO:
            theApp->getForm<zFormTZX>(FORM_TZX)->setMode(msg->arg1, msg->sarg)->updateVisible(true);
            break;
        case MSG_HIDE_TZX_INFO:
            theApp->getForm(FORM_TZX)->updateVisible(false);
            if(!speccy->execLaunch) {
                speccy->execLaunch = true;
                send(ZX_MESSAGE_PROPS);
            }
            break;
        // отобразить форму сообщений
        case MSG_FORM_MSG:
            fm = new zViewFormMessage(styles_z_msgform, 0, styles_z_formcaption, styles_z_formfooter,
                                      msg->arg1, msg->sarg);
            manager->attachForm(fm, 340_dp, 220_dp)->updateVisible(true);
            break;
    }
    zViewGroup::notifyEvent(msg);
}

void zSpeccyLayout::activateDebugger() {
    auto sd(speccy->showDebugger);
    if(!sd) {
        speccy->showDebugger = true;
        stateTools(ZFT_UPD_MENU_DBG);
    }
    speccy->debugMode = MODE_PC;
    theApp->getForm<zFormDebugger>(FORM_DEBUG)->stateTools(SD_TOOLS_ALL | SD_TOOLS_UPD_SREG | SD_TOOLS_LST);
}

void zSpeccyLayout::stateTools(int action, int id) {
    auto main(theApp->main);
    // инициализация группы
    if((action & ZFT_UPD_MENU_GROUP) && id) {
        auto mi(menu->idFind(id)); zMenuGroup* mg(nullptr);
        if(mi) mg = mi->getGroup();
        if(mg) {
            if(id == R.integer.MENU_MODEL) {
                mg->atFind(speccy->model)->setChecked(true);
            } else if(id == R.integer.MENU_MRU) {
                for(int i = 0 ; i < 10; i++) {
                    auto m(mg->atFind(i));
                    auto s(settings->mruDecorate(i));
                    m->setEnabled(s.isNotEmpty());
                    m->setText(s.isEmpty() ? theme->findString(R.string.menuMruEmpty) : s);
                }
            } else {
                for(auto &m: *mg) m->setChecked(speccy->bools[z_remap(m->getId(), valIds) - 64]);
            }
        }
    }
    if(action & ZFT_UPD_MENU_DISP) {
        static int imgs[] = { R.integer.iconZxGamepad, R.integer.iconZxKeyb, R.integer.iconZxDisplay };
        menu->idFind(R.integer.MENU_DISPLAY)->setImage(imgs[speccy->panelMode]);
        action |= ZFT_UPD_MENU_DBG;
    }
    if(action & ZFT_UPD_MENU_DBG) {
        auto dbg(theApp->getForm(FORM_DEBUG)); auto keyb(theApp->keyb);
        auto land(manager->isLandscape());
        auto chk(!land && speccy->showDebugger);
        menu->idFind(R.integer.MENU_PROPS_DEBUGGER)->setVisibled(!land);
        menu->idFind(R.integer.MENU_DEBUGGER)->setVisibled(chk);
        menu->idFind(R.integer.MENU_DISPLAY)->setVisibled(!chk);
        dbg->updateStatus(ZS_VISIBLED, false);
        keyb->updateStatus(ZS_VISIBLED, false);
        if(chk) {
            frame->lps.y = 60; dbg->lps.y = 40;
            chk = false; dbg->updateVisible(true);
        } else {
            if(speccy->panelMode == 1) {
                frame->lps.y = 90 - land * 10 - speccy->sizeKeyb * 5;
                keyb->lps.y = 10 + land * 10 + speccy->sizeKeyb * 5;
                keyb->updateVisible(true);
            }
            chk = (speccy->panelMode == 0);
        }
        ac->updateStatus(ZS_VISIBLED, chk);
        cc->updateStatus(ZS_VISIBLED, chk);
        main->requestLayout();
    }
    if(action & ZFT_UPD_MENU_ITEM) {
        // tape
        frame->idView(R.id.llTape)->updateVisible(speccy->showTape);
        // execute
        auto dbg(main->atView<zFormDebugger>(1));
        if(dbg) dbg->stateTools(SD_TOOLS_BUT | SD_TOOLS_LST);
    }
    if(action & ZFT_UPD_TAPE) {
        // normal/speed Tape
        tapeTurbo->setIcon(speccy->speedTape ? R.integer.iconZxAccelOn : R.integer.iconZxAccelOff);
        // ops Tape
        tapePlay->setIcon(speccy->playTape ? R.integer.iconZxPause : R.integer.iconZxPlay);
        // progress Tape
        tapeProgress->setRange(szi(0, speccy->tapeAllIndex));
        tapeProgress->setProgress(speccy->tapeIndex);
    }
    if(action & ZFT_UPD_CONTROLLER) {
        szi sz(z_dp(128 + 20 * speccy->sizeJoy), z_dp(128 + 20 * speccy->sizeJoy));
        ac->setSize(sz);
        cc->setSize(sz);
    }
}

void zSpeccyLayout::onCommand(int id, zMenuItem* mi) {
    int action(0);
    auto _id(z_remap(id, valIds));
    if(_id != -1) mi->setChecked(speccy->bools[_id - 64] ^= 1);
    switch(id) {
        case R.integer.MENU_QLOAD:
            send(ZX_MESSAGE_LOAD, 0, ZX_ARG_IO_QUICK,
                 settings->makePath(z_fmt8("qsave_%s.ezx", speccy->progName.str()), FOLDER_CACHE));
            break;
        case R.integer.MENU_QSAVE:
            send(ZX_MESSAGE_SAVE, 0, ZX_ARG_IO_QUICK,
                 settings->makePath(z_fmt8("qsave_%s.ezx", speccy->progName.str()), FOLDER_CACHE));
            break;
        case R.integer.MENU_EXIT:
            theApp->exitProgramm();
            break;
        case R.integer.MENU_DISPLAY:
            speccy->panelMode = (++speccy->panelMode % 3);
            action = ZFT_UPD_MENU_DISP;
            break;
        case R.integer.MENU_OPS_ASSEMBLER:
            theApp->getForm(FORM_ASM)->updateVisible(true);
            break;
        case R.integer.MENU_OPS_EDITOR_TZX:
            theApp->getForm(FORM_ETZX)->updateVisible(true);
            break;
        case R.integer.MENU_OPS_POKES:
            theApp->getForm(FORM_POKES)->updateVisible(true);
            break;
        case R.integer.MENU_OPS_RESET:
            send(ZX_MESSAGE_RESET);
            break;
        case R.integer.MENU_OPS_RESTORE:
            send(ZX_MESSAGE_LOAD, 0, ZX_ARG_IO_QUICK, settings->makePath("autosave.ezx", FOLDER_CACHE));
            break;
        case R.integer.MENU_OPS_MAGIC:
            send(ZX_MESSAGE_MAGIC);
            break;
        case R.integer.MENU_SETTINGS:
            theApp->getForm(FORM_SETTING)->updateVisible(true);
            break;
        case R.integer.MENU_BROWSER:
            theApp->getForm(FORM_OPEN)->updateVisible(true);
            break;
        case R.integer.MENU_DEBUGGER_ADDR:
        case R.integer.MENU_DEBUGGER_CODE:
        case R.integer.MENU_DEBUGGER_CONTENT:
            theApp->getForm<zFormDebugger>(FORM_DEBUG)->stateTools(SD_TOOLS_LST);
            break;
        case R.integer.MENU_MODEL_48K:
        case R.integer.MENU_MODEL_128K:
        case R.integer.MENU_MODEL_PENTAGON128:
        case R.integer.MENU_MODEL_PENTAGON512:
        case R.integer.MENU_MODEL_SCORPION:
            send(ZX_MESSAGE_MODEL, id - R.integer.MENU_MODEL_48K, 0);
            break;
        case R.integer.MENU_MRU_1: case R.integer.MENU_MRU_2:
        case R.integer.MENU_MRU_3: case R.integer.MENU_MRU_4:
        case R.integer.MENU_MRU_5: case R.integer.MENU_MRU_6:
        case R.integer.MENU_MRU_7: case R.integer.MENU_MRU_8:
        case R.integer.MENU_MRU_9: case R.integer.MENU_MRU_10:
            send(ZX_MESSAGE_LOAD, 0, 0, settings->mruOpen(id - R.integer.MENU_MRU_1, "", false));
            break;
        case R.integer.MENU_PROPS_DEBUGGER:
            action = ZFT_UPD_MENU_DBG;
            break;
        case R.integer.MENU_PROPS_FILTER:
            drw[DRW_FK]->texture->enableFilter(speccy->filter);
            break;
        case R.integer.MENU_PROPS_EXECUTE:
        case R.integer.MENU_PROPS_TURBO:
            modifySTATE(ZX_CAPT, 0)
        case R.integer.MENU_PROPS_SOUND:
            send(ZX_MESSAGE_PROPS);
        case R.integer.MENU_PROPS_MAGNITOPHON:
            action = ZFT_UPD_MENU_ITEM;
            break;
    }
    if(action) stateTools(action, id);
}

void zSpeccyLayout::processHandler() {
    zString8 tmp; bool error;
    while(auto msg = handler.obtain()) {
        auto arg2(msg->arg2);
        switch(msg->what) {
            case ZX_MESSAGE_MAGIC:  zxCmd(ZX_CMD_MAGIC); break;
            case ZX_MESSAGE_PROPS:  zxCmd(ZX_CMD_PROPS); break;
            case ZX_MESSAGE_RESET:  zxCmd(ZX_CMD_RESET); break;
            case ZX_MESSAGE_DEBUG:  speccy->getCpu(msg->arg1)->debug(); break;
            case ZX_MESSAGE_MODEL:  zxCmd(ZX_CMD_MODEL, msg->arg1, arg2); break;
            case ZX_MESSAGE_DISK:   zxCmd(ZX_CMD_DISK_OPS, msg->arg1, arg2, msg->sarg); break;
            case ZX_MESSAGE_SAVE:
                tmp = (arg2 & ZX_ARG_IO_QUICK ? msg->sarg.substrAfterLast("/") : msg->sarg);
                if(speccy->save(msg->sarg, msg->arg1)) {
                    if(arg2 & ZX_ARG_IO_QUICK) {
                        // быстрая запись - вывести форму с сообщением
                        post(MSG_SHOW_TZX_INFO, 0, TZX_QINFO, 0, "Quick Save\n" + tmp);
                    }
                } else {
                    post(MSG_FORM_MSG, 0, R.string.msg_error, 0, z_fmt8("Не удалось сохранить -\n%s!", tmp.str()));
                }
                break;
            case ZX_MESSAGE_LOAD:
                tmp = (arg2 & ZX_ARG_IO_QUICK ? msg->sarg.substrAfterLast("/") : msg->sarg);
                if((error = speccy->load(msg->sarg, 1))) {
                    if(arg2 & ZX_ARG_IO_QUICK) {
                        // быстрая загрузка - вывести форму с сообщением
                        post(MSG_SHOW_TZX_INFO, 0, TZX_QINFO, 0, "Quick Load\n" + tmp);
                    }
                    zxCmd(ZX_CMD_PROPS);
                } else {
                    post(MSG_FORM_MSG, 0, R.string.msg_error, 0, z_fmt8("Не удалось загрузить -\n%s!", tmp.str()));
                }
                settings->mruOpen(0, tmp, !error);
                break;
        }
    }
    if(checkSTATE(ZX_CAPT)) { setCaption(); modifySTATE(0, ZX_CAPT) }
}

void zSpeccyLayout::send(int what, int a1, int a2, cstr s) {
    handler.send(nullptr, what, 50, a1, a2, s);
}

void zSpeccyLayout::setParamControllers() {
    auto keys(theme->findArray(R.string.key_names));
    auto root(manager->getSystemView(true));
    auto cc(root->idView<zViewController>(z.R.id.ccontroller));
    auto ac(root->idView<zViewController>(z.R.id.acontroller));
    auto tex(manager->cache->get("zx_icons", nullptr));
    // установить надписи/значки на кнопках джойстика
    for(int i = 0 ; i < 8; i++) {
        auto c(i < 4 ? cc : ac);
        auto k(keys[speccy->joyKeys[i]]);
        auto tx(k.substrBefore("\b", k));
        auto ic(tex->getTile(k.substrAfter("\b")));
        c->setDecorateKey(i & 3, ic == -1 ? tx : "", ic);
    }
}
