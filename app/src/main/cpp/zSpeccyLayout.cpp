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
    // меню ленты
    tapeLyt   = (zLinearLayout*)attach(new zLinearLayout(styles_default, R.id.llTape, manager->isLandscape()),
                                            ZS_GRAVITY_START | ZS_GRAVITY_TOP, 0, VIEW_WRAP, VIEW_WRAP, 0);
    tapePlay  = new zViewButton(styles_zxtools, R.id.tapePlay, 0, R.integer.iconZxPlay);
    tapeTurbo = new zViewButton(styles_zxtools, R.id.tapeTurbo, 0, R.integer.iconZxAccelOff);
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
    ac = (zViewController*)frame->attach(new zViewController(styles_z_controller, z.R.id.acontroller, z.R.integer.acontrol, z.R.string.acontrollerMap),
                                        ZS_GRAVITY_END | ZS_GRAVITY_BOTTOM, 0, 128_dp, 128_dp);
    cc = (zViewController*)frame->attach(new zViewController(styles_z_controller, z.R.id.ccontroller, z.R.integer.ccontrol, z.R.string.ccontrollerMap),
                                        ZS_GRAVITY_START | ZS_GRAVITY_BOTTOM, 0, 128_dp, 128_dp);
    cc->setOnChangeButton([this](zView*, int b) {
        theApp->keyb->keyEvent(b | (int)(ac->getButtons() << 4), false); });
    ac->setOnChangeButton([this](zView*, int b) { theApp->keyb->keyEvent((b << 4) | (int)cc->getButtons(), false); });
    _status = idView<zViewImage>(R.id.speccyStatus);
    setOnTouch([this](zView*, zTouch* t) {
//        auto sz((float)(speccy->sizeBorder << 4));
  //      frame->setScale(352.0f / (256 + sz), 288.0f / (192 + sz));
        auto b(0);//speccy->sizeBorder << 4);
        auto x(z_round(t->cpt.x * (256.0f / rview.w))), y(z_round(((float)rview.h - t->cpt.y) * (192.0f / (float)rview.h)));
        auto xx(x - b), yy(y - b);
        // относительно старых
        speccy->mouse[1] = xx;
        speccy->mouse[2] = yy;
        return false;
    });
    setOnClick([this](zView*, int b) {
        static i64 _tm(0); static pti _pt;
        // двойной клик - qload/qsave
        auto dbl(zView::touch->isDblClicked());
        speccy->mouse[0] = !dbl * (speccy->swapMouse + 1);
        pti pt(z_round(zView::touch->cpt.x), z_round(zView::touch->cpt.y));
        auto tm(z_timeMillis()), t((tm - _tm));
        if(dbl) {
            if(t < 600) {
                auto vert(!theApp->isLandscape());
                auto s(rview); if(!vert) { s.x = s.w / 4; s.w /= 2; }
                if(s.contains(pt)) onCommand(pt[vert] > (s[vert] + s[vert + 2] / 2) ? R.integer.MENU_QSAVE : R.integer.MENU_QLOAD, nullptr);
                tm = 0;
            }
        } else if(b) {
            theApp->getActionBar()->show(true);
        } else {
            if(t < 500) {
                static int pressed(0);
                // переключение джойстиков/клавы/экрана  - нарисовать крест
                auto div1(pt[pressed] - _pt[pressed] > 150);
                auto div2(abs(pt[!pressed] - _pt[!pressed]) < (pt[pressed] - _pt[pressed]) / 2);
                //ILOG("div1:%i div2:%i pres:%i", div1, div2, pressed);
                if(div1 && div2) {
                    if(++pressed > 1) {
                        speccy->panelMode = (++speccy->panelMode % 3);
                        stateTools(ZFT_UPD_MENU_DISP);
                        pt.set(-1000, -1000); tm = 0; pressed = 0;
                    }
                } else pressed = 0;
            }
            _pt = pt;
        }
        _tm = tm;
    });
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
    auto sz((float)(speccy->sizeBorder << 4));
    frame->setScale(352.0f / (256 + sz), 288.0f / (192 + sz));
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.w, size.h, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, speccy->dev<zDevUla>()->frameBuffer + 101376);
}

void zSpeccyLayout::notifyEvent(HANDLER_MESSAGE* msg) {
    int bind;
    switch(msg->what) {
        case MSG_UPDATE_STATUS:
            _status->setImage(msg->arg1);
            break;
        case MSG_UPDATE_CAPTION:
            setCaption();
            break;
        case MSG_UPDATE_KEYBOARD:
            theApp->keyb->invalidate();
            break;
        case MSG_UPDATE_CONTROLLER:
            setParamControllers();
            break;
        case MSG_UPDATE_TAPE:
            stateTools(ZFT_UPD_TAPE);
            break;
            // активировать отладчик
        case MSG_ACTIVATE_DEBUGGER:
            activateDebugger();
            break;
        // отобразить форму информации TZX
        case MSG_SHOW_TZX_INFO:
            theApp->getForm<zFormTZX>(FORM_TZX)->updateVisible(true);
            break;
        case MSG_HIDE_TZX_INFO:
            theApp->getForm(FORM_TZX)->updateVisible(false);
            speccy->tapeCurrent++;
            if(!speccy->execLaunch) {
                speccy->execLaunch = true;
                send(ZX_MESSAGE_PROPS);
            }
            break;
        // отобразить форму сообщений
        case MSG_FORM_MSG:
            zViewManager::showToast(msg->sarg);
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
    auto dbg(theApp->getForm<zFormDebugger>(FORM_DEBUG));
    dbg->stateTools(SD_TOOLS_ALL | SD_TOOLS_LST);
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
        auto land(manager->isLandscape()), chk(speccy->showDebugger);
        menu->idFind(R.integer.MENU_DEBUGGER)->setVisibled(chk);
        menu->idFind(R.integer.MENU_DISPLAY)->setVisibled(!chk);
        dbg->updateVisible(false); keyb->updateVisible(false);
        if(chk) {
            main->atView(0)->lps.weight = 60; dbg->lps.weight = 40;
            main->changeOrientation(!land);
            dbg->updateVisible(true);
        }
        if(speccy->panelMode == 1) {
            lps.weight = 90 - land * 10 - speccy->sizeKeyb * 5;
            keyb->lps.weight = 10 + land * 10 + speccy->sizeKeyb * 5;
            keyb->updateVisible(true);
        }
        chk = (speccy->panelMode == 0);
        ac->updateVisible(chk); cc->updateVisible(chk);
    }
    if(action & ZFT_UPD_MENU_ITEM) {
        // execute
        auto dbg(main->atView<zFormDebugger>(1));
        if(dbg) dbg->stateTools(SD_TOOLS_BUT | SD_TOOLS_LST);
    }
    if(action & ZFT_UPD_TAPE) {
        auto vis(tapeLyt->isVisibled()), sts(false), shw(false);
        if(speccy->showTape) {
            shw = speccy->tapeCount > 0;
            if(shw) shw = speccy->tapeCurrent < speccy->tapeCount;
            if(shw) shw = speccy->tapeIndex < (speccy->tapeAllIndex - 1000);
            if(shw) {
                // normal/speed Tape
                tapeTurbo->setIcon(speccy->speedTape ? R.integer.iconZxAccelOn : R.integer.iconZxAccelOff);
                // ops Tape
                tapePlay->setIcon(speccy->recTape ? R.integer.iconZxBp : (speccy->playTape ? R.integer.iconZxPlay : R.integer.iconZxPause));
                // progress Tape
                tapeProgress->setRange(szi(0, speccy->tapeAllIndex));
                tapeProgress->setProgress(speccy->tapeIndex);
                sts = true;
            }
        }
        if(sts != vis) tapeLyt->updateVisible(shw);
    }
    if(action & ZFT_UPD_CONTROLLER) {
        szi sz(z_dp(128 + 20 * speccy->sizeJoy), z_dp(128 + 20 * speccy->sizeJoy));
        ac->setSize(sz); cc->setSize(sz);
    }
}

void zSpeccyLayout::onCommand(int id, zMenuItem* mi) {
    int action(0);
    auto _id(z_remap(id, valIds));
    if(_id != -1 && mi) mi->setChecked(speccy->bools[_id - 64] ^= 1);
    switch(id) {
        case R.integer.MENU_QLOAD:
            send(ZX_MESSAGE_LOAD, 0, ZX_ARG_IO_QUICK,
                 settings->makePath(z_fmt8("savers/qsave_%s.ezx", speccy->progName.str()), FOLDER_FILES));
            break;
        case R.integer.MENU_QSAVE:
            send(ZX_MESSAGE_SAVE, 0, ZX_ARG_IO_QUICK,
                 settings->makePath(z_fmt8("savers/qsave_%s.ezx", speccy->progName.str()), FOLDER_FILES));
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
            send(ZX_MESSAGE_LOAD, 1, ZX_ARG_IO_QUICK, settings->makePath("autosave.ezx", FOLDER_CACHE));
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
        auto arg1(msg->arg1), arg2(msg->arg2); auto isQuick(arg2 & ZX_ARG_IO_QUICK);
        switch(msg->what) {
            case ZX_MESSAGE_MAGIC:  zxCmd(ZX_CMD_MAGIC); break;
            case ZX_MESSAGE_PROPS:  zxCmd(ZX_CMD_PROPS); break;
            case ZX_MESSAGE_RESET:  zxCmd(ZX_CMD_RESET); break;
            case ZX_MESSAGE_DEBUG:  speccy->getCpu(arg1)->debug(); break;
            case ZX_MESSAGE_MODEL:  zxCmd(ZX_CMD_MODEL, arg1, arg2); break;
            case ZX_MESSAGE_DISK:   zxCmd(ZX_CMD_DISK_OPS, arg1, arg2, msg->sarg); break;
            case ZX_MESSAGE_SAVE:
                tmp = (isQuick ? msg->sarg.substrAfterLast("/") : msg->sarg);
                if(speccy->save(msg->sarg, arg1)) {
                    // запись - вывести форму с сообщением
                    tmp = (isQuick ? theme->findString(R.string.qSave) : "Save\n") + tmp;
                } else {
                    tmp = z_fmt8(theme->findString(R.string.failedSave), tmp.str());
                }
                zViewManager::showToast(tmp);
                break;
            case ZX_MESSAGE_LOAD:
                tmp = (isQuick ? msg->sarg.substrAfterLast("/") : msg->sarg);
                if((error = speccy->load(msg->sarg, speccy->tapeStartStop))) {
                    if(isQuick) {
                        // быстрая загрузка - вывести форму с сообщением
                        msg->sarg = theme->findString(R.string.qLoad) + tmp;
                    } else msg->sarg.empty();
                    zxCmd(ZX_CMD_PROPS);
                } else {
                    msg->sarg = z_fmt8(theme->findString(R.string.failedLoad), tmp.str());
                }
                if(tmp.indexOf(".ezx") == -1) settings->mruOpen(0, tmp, !error);
                if(msg->sarg.isNotEmpty() && !arg1) zViewManager::showToast(msg->sarg);
                break;
        }
    }
    if(checkSTATE(ZX_CAPT)) {
        post(MSG_UPDATE_CAPTION, 0);
        modifySTATE(0, ZX_CAPT)
    }
}

void zSpeccyLayout::send(int what, int a1, int a2, cstr s) {
    handler.send(this, what, 50, a1, a2, s);
}

void zSpeccyLayout::setParamControllers() {
    static int tiles[] = { z.R.integer.ccontrolL, z.R.integer.ccontrolU, z.R.integer.ccontrolR, z.R.integer.ccontrolD,
                           z.R.integer.acontrolY, z.R.integer.acontrolX, z.R.integer.acontrolA, z.R.integer.acontrolB };
    auto keys(theme->findArray(R.string.key_names));
    auto tex(manager->cache->get("zx_icons.ttl", nullptr));
    // установить надписи/значки на кнопках джойстика
    for(int i = 0 ; i < 8; i++) {
        auto c(i < 4 ? cc : ac);
        auto k(keys[speccy->joyKeys[i]]);
        auto tx(k.substrBefore("\b", k));
        auto ic(tex->getTile(k.substrAfter("\b")));
        c->setDecorateKey(i & 3, ic == -1 ? tx : "", ic, tiles[i]);
    }
    manager->cache->recycle(tex);
}

