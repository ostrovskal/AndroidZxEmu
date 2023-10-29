
#include "sshCommon.h"
#include "zostrov/zViewManager.h"
#include "zostrov/zViewWidgets.h"
#include "sshApp.h"
#include "zFormDebugger.h"
#include "zFormPokes.h"
#include "zFormSelFile.h"
#include "zFormBrowser.h"
#include "zFormSettings.h"
#include "zFormAssembler.h"
#include "zFormBpList.h"
#include "zSpeccyKeyboard.h"
#include "zFormTZX.h"
#include "zRibbonTape.h"

void sshApp::processEvents(i32 event) {
    switch(event) {
        case APP_CMD_INIT_WINDOW:
        case APP_CMD_TERM_WINDOW:
            appActivate(event == APP_CMD_INIT_WINDOW, android->window);
            if(event == APP_CMD_TERM_WINDOW) exitProgramm();
            break;
        case APP_CMD_SAVE_STATE:
            stateAllViews(Z_SAVE, (u8**)&android->savedState, (u32*)&android->savedStateSize);
            break;
        case APP_CMD_GAINED_FOCUS:
        case APP_CMD_LOST_FOCUS:
            focusNativeWindow(event == APP_CMD_GAINED_FOCUS);
            break;
        case APP_CMD_PAUSE:
            DLOG("APP_CMD_PAUSE");
            break;
        case APP_CMD_RESUME:
            DLOG("APP_CMD_RESUME");
            stateAllViews(Z_RESUME, (u8**)&android->savedState, (u32*)&android->savedStateSize);
            break;
        case APP_CMD_WINDOW_REDRAW_NEEDED:
            redrawNativeWindow();
            break;
        case APP_CMD_WINDOW_RESIZED:
            updateNativeWindow(android->config, theme->styles ? theme->styles : styles_mythemelight, resources_ptr_arrays, styles);
            break;
    }
}

static void callback_appEvent(android_app*, i32 cmd) {
    theApp->processEvents(cmd);
}

static i32 callback_inputEvent(android_app*, AInputEvent *event) {
    return theApp->processInputEvens(event);
}

void android_main(android_app* android) {
    ANativeActivity_setWindowFlags(android->activity, AWINDOW_FLAG_FULLSCREEN | AWINDOW_FLAG_KEEP_SCREEN_ON | AWINDOW_FLAG_LAYOUT_IN_SCREEN | AWINDOW_FLAG_LAYOUT_INSET_DECOR, 0);
    sshApp app(android);
    theApp->run();
}

static cstr options_def[] = {
        "[BOOLEAN]",
        "gsReset=false", "speed_tape=false", "sound=true", "filter=true",
        "execute=true", "debugging=false", "turbo=false", "snd_bp=true", "snd_ay=true",
        "hex=false", "show_debugger=false", "debugger_address=true", "debugger_code=true",
        "debugger_content=true", "play_tape=false", "rec_tape=false",
        "debug_reg16=false", "tapeAutoStartStop=true", "reset_tape=false", "dark_mode=false",
        "swap_mouse=false", "giga_screen=false", "debugCpu=true", "snd_cv=false",
        "snd_gs=false", "trap_dos=true", "fps=false", "showTape=true",
        "boolTmp1=false", "boolTmp2=false",
        "[BYTES]",
        "border_size=3", "joy_size=3","keyb_size=2",
        "snd_chip_ay=0", "snd_channel_ay=1", "snd_freq=1", "snd_vol_bp=6",
        "snd_vol_ay=8", "snd_vol_cv=31", "snd_vol_gs=31", "cpu_speed=6", "settings_tab=0",
        "debug_mode=0", "panel_mode=0", "browse_flt=0",
        "[HEX]",
        "color_nb=ff000000", "color_ns=ff2030c0", "color_nr=ffC04010", "color_nm=ffc040c0",
        "color_ng=ff40b010", "color_nc=ff50c0b0", "color_ny=ffe0c010", "color_nw=ffc0c0c0",
        "color_bb=ff000000", "color_bs=ff3040ff", "color_br=ffff4030", "color_bm=ffff70f0",
        "color_bg=ff50e010", "color_bc=ff50e0ff", "color_by=ffffe850", "color_bw=ffffffff",
        "[HEX]",
        "acolor_bkg=ff000000", "acolor_sel=ff505050", "acolor_def=ffa0a0a0", "acolor_num=ffffff00",
        "acolor_str=ffff00ff", "acolor_ops=ff00ffff", "acolor_rem=ff00ff00", "acolor_opr=ffffffff",
        "acolor_cmd=ff70a0ff", "acolor_reg=ffff0000", "acolor_flg=ff1080ff", "acolor_lbl=ffffffff",
        "acolor_tln=ff303030", "acolor_bln=fff0f0f0", "acolor_cur=ff404040", "acolor=ff000000",
        "[MRU]",
        "#mru0=Empty", "#mru1=Empty", "#mru2=Empty", "#mru3=Empty", "#mru4=Empty",
        "#mru5=Empty", "#mru6=Empty", "#mru7=Empty", "#mru8=Empty", "#mru9=Empty",
        "[PATH]",
        "#path=", "#path_asm=", nullptr
};

sshApp::sshApp(android_app* _android) : zViewManager(_android->activity, options_def, 4) {
    theApp                = this;
    android               = _android;
    android->userData     = this;
    android->onAppCmd     = callback_appEvent;
    android->onInputEvent = callback_inputEvent;
    tmpBuf                = new u8[ZX_SIZE_TMP_BUF];
    memset(tmpBuf, 0, ZX_SIZE_TMP_BUF);
}

void sshApp::run() {
    while(true) {
        i32 events;
        android_poll_source* source;
        while((ALooper_pollAll((int)isActive() - 1, nullptr, &events, (void**)&source)) >= 0) {
            if(source) source->process(android, source);
            if(android->destroyRequested) return;
        }
        if(isActive()) drawViews();
    }
}

#include <filesystem>

void sshApp::setContent() {
//    debug = true;
#include "layout_main.h"
    // 1. фрейм
    frame = frame01; main = mainLL;
    // 3. меню
    getActionBar()->setOnClickMenuItem([](zView*, zMenuItem* mi) {
        frame->onCommand(mi->getId(), mi);
    })->setOnClickMenuGroup([](zView*, zMenuGroup* gp) {
        frame->stateTools(ZFT_UPD_MENU_GROUP, gp->getId());
    })->setMenu(R.integer.iconZxAppZx, main_menu);
#include "layout_form_browser.h"
#include "layout_form_settings.h"
#include "layout_form_debugger.h"
#include "layout_form_bp_list.h"
#include "layout_form_asm.h"
#include "layout_form_tzx.h"
#include "layout_form_pokes.h"
#include "layout_form_sel_file.h"
    // 1. формы
    keyb                = new zSpeccyKeyboard();
    forms[FORM_OPEN]    = browser;
    forms[FORM_CHOICE]  = selfile;
    forms[FORM_POKES]   = pokes;
    forms[FORM_SETTING] = settings;
    forms[FORM_ASM]     = assembler;
    forms[FORM_BPS]     = bplist;
    forms[FORM_DEBUG]   = debugger;
    forms[FORM_TZX]     = tzx;
    forms[FORM_WAIT]    = new zViewFormWait(0, 0, 0);
    // привязка и инициализация
    if(!frame->init()) { ILOG("Ошибка при запуске эмулятора!"); }
    frame->setOnClick([this](zView*, int b) {
        // двойной клик - qload/qsave
        if(zView::touch->isDblClicked()) {
            auto vert(!isLandscape());
            auto s(screen); if(!vert) s.set(screen.w / 4, screen.y, screen.w / 2, screen.h);
            pti p(z_round(zView::touch->cpt.x), z_round(zView::touch->cpt.y));
            if(s.contains(p)) {
                frame->onCommand(p[vert] > (s[vert] + s[vert + 2] / 2) ? R.integer.MENU_QSAVE : R.integer.MENU_QLOAD, nullptr);
            }
        } else if(b) getActionBar()->show(true);
    });
    frame->attach(assembler, VIEW_MATCH, VIEW_MATCH);
    frame->attach(tzx, VIEW_MATCH, VIEW_MATCH);
    main->attach(debugger, VIEW_MATCH, VIEW_MATCH);
    main->attach(keyb, VIEW_MATCH, VIEW_MATCH);
    attachForm(browser, 450_dp, 450_dp);
    attachForm(bplist, 400_dp, 350_dp);
    attachForm(settings, 450_dp, 450_dp);
    attachForm(pokes, 350_dp, 300_dp);
    attachForm(selfile, 350_dp, 300_dp);
    attachForm(getForm(FORM_WAIT), 300_dp, 300_dp);
    frame->stateTools(ZFT_ALL);
    // установить тему
    setTheme(speccy->darkTheme ? styles_mythemedark : styles_mythemelight, resources_ptr_arrays, ::styles);
    // создать папки savers/games/system/demo/gs
    static cstr folders[] = { "savers", "gs", "games", "demo", "system" };
    for(auto s : folders) {
        auto dst(::settings->makePath(s, FOLDER_FILES).slash());
        if(!std::filesystem::exists(dst.str())) {
            std::filesystem::create_directory(dst.str());
            copyFromAssets(zString8("programm/") + s, dst);
        }
    }
}

void sshApp::copyFromAssets(zString8 src, czs& dst) {
    auto dir(AAssetManager_openDir(manager->getAsset(), src));
    if(dir) {
        cstr name(nullptr); int len; zFile file; src.slash();
        while((name = AAssetDir_getNextFileName(dir))) {
            auto ptr(manager->assetFile(src + name, &len));
            if(ptr) {
                if(file.open(dst + name, false, false)) {
                    file.write(ptr, len);
                    file.close();
                }
            }
        }
    }
}