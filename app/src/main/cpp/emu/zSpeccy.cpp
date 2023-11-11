//
// Created by User on 20.07.2023.
//

#include "../sshCommon.h"
#include "zSpeccy.h"
#include "zDevs.h"
#include "../zFormDebugger.h"
#include "../zFormSelFile.h"
#include <filesystem>

// идентификатор треда
static volatile pthread_t threadID(0);
static int delay(0);

void zSpeccy::process() {
    if(manager->isActive()) {
        frame->processHandler();
        execute();
        // обновление информации на экране
        if(!(++delay & 31)) frame->post(MSG_UPDATE_TAPE, 0);
        if(checkSTATE(ZX_BP | ZX_T_UI)) {
            // активировать отладчик
            if(checkSTATE(ZX_BP)) frame->post(MSG_ACTIVATE_DEBUGGER, 0);
            // активировать форму TZX_INFO
            if(checkSTATE(ZX_T_UI)) frame->post(MSG_SHOW_TZX_INFO, 0);
            flags &= ~(ZX_BP | ZX_T_UI);
        }
    }
}

static void* threadFunc(void*) {
    while(speccy && speccy->exit == 0) speccy->process();
    threadID = 0;
    ILOG("exit speccy thread");
    return nullptr;
}

// параметры текущей машины
Z_MACHINE* zSpeccy::machine(nullptr);

static float turboClk[] = { 7.0f, 6.0f, 5.0f, 4.0f, 3.0f, 2.0f, 1.0f, 0.5f, 0.33333f, 0.25f, 0.2f, 0.166666f, 0.142857f };

Z_MACHINE machines[] = {
        {   { 6, 5, 4, 3, 2, 1, 0, 0 }, { 0, 0, 0, 9 }, PAGE_SOS48,
                0x20, 0, 36, 69888, 14335, 224, 312, 48, 48, 1773400, 3500000, 8,  "Spectrum 48K" },
        {   { 6, 5, 4, 3, 2, 1, 0, 0 }, { 1, 2, 1, 9 }, PAGE_SOS128,
                0xAA, 0, 36, 70908, 14361, 228, 311, 48, 48, 1773400, 3546900, 8,  "Spectrum 128K" },
        {   { 0, 0, 0, 0, 0, 0, 0, 0 }, { 3, 4, 3, 9 }, PAGE_SOS128,
                0x00, 0, 32, 71680, 17982, 224, 320, 48, 48, 1792000, 3584000, 8,  "Pentagon 128K" },
        {   { 0, 0, 0, 0, 0, 0, 0, 0 }, { 3, 4, 3, 9 }, PAGE_SOS128,
                0x00, 0, 32, 71680, 17989, 224, 320, 48, 48, 1792000, 3584000, 32, "Pentagon 512K" },
        {   { 0, 0, 0, 0, 0, 0, 0, 0 }, { 5, 6, 7, 8 }, PAGE_SOS128,
                0x00, 1, 36, 69888, 14332, 224, 312, 48, 48, 1750000, 3500000, 16, "Scorpion 256K" },
        {   { 0, 0, 0, 0, 0, 0, 0, 0 }, { 10, 11, 11, 11 }, 0,
                0x00, 0, 32, 320,   0,     0,   0,   0,  0,  0,       12000000,30, "General Sound" }
};

zSpeccy::zSpeccy() {
    memset(&flags,0, 362);
    memset(devs,  0, sizeof(devs));
    memset(rdevs, 0, sizeof(rdevs));
    memset(wdevs, 0, sizeof(wdevs));
    // машина по умолчанию(до загрузки состояния)
    machine = &machines[MODEL_48];
    // джойстик по умолчанию - KEMPSTON
    joyType = 0; memcpy(joyKeys, defJoyKeys, sizeof(defJoyKeys));
}

zSpeccy::~zSpeccy() {
    speccy->exit = 1; while(threadID) { }
    save(settings->makePath("autosave.ezx", FOLDER_CACHE), 0);
    settings->save((u8*)&speccy->gsReset, "settings.ini");
    for(auto& dev : devs) SAFE_DELETE(dev);
    // джойстики/покес
    saveJoyPokes();
    speccy = nullptr;
}

bool zSpeccy::loadROM(u8* ptr, const u8* pages, int count) {
    zFileAsset ass("data/rom.bin", true);
    for(int i = 0 ; i < count; i++) {
        if(!ass.read(nullptr, ptr, 16384, pages[i] << 14)) return false;
        ptr += 16384;
    }
    return true;
}

bool zSpeccy::init() {
    // 1. создаем устройства
    addDev(DEV_MIX,     ACCESS_NO,  new zDevMixer());
    addDev(DEV_EXT,     ACCESS_RW,  new zDevExtPort());
    addDev(DEV_MEM,     ACCESS_RW,  new zDevMem());
    addDev(DEV_ULA,     ACCESS_RW,  new zDevUla());
    addDev(DEV_KEYB,    ACCESS_RD,  new zDevKeyboard());
    addDev(DEV_JOY,     ACCESS_RD,  new zDevKempston());
    addDev(DEV_MOUSE,   ACCESS_RD,  new zDevMouse());
    addDev(DEV_BEEPER,  ACCESS_WR,  new zDevBeeper());
    addDev(DEV_COVOX,   ACCESS_WR,  new zDevCovox());
    addDev(DEV_AY,      ACCESS_RW,  new zDevAY());
    addDev(DEV_CPU,     ACCESS_NO,  new zCpuMain());
    addDev(DEV_TAPE,    ACCESS_RW,  new zDevTape());
    addDev(DEV_DISK,    ACCESS_RW,  new zDevVG93());
    addDev(DEV_GS,      ACCESS_RW,  new zCpuGs());
    // 1.1. загрузка параметров
    settings->init((u8*)&bools, "settings.ini");
    update(ZX_UPDATE_RESET, 0);
    // 1.2. бут для диска
    diskBoot = (u8*)zFileAsset("data/boot.zzz", true).readn();
    // 1.3. джойстики/покес
    auto dst(::settings->makePath("pokes.txt", FOLDER_CACHE)); int size;
    if(dst) {
        if(!zFileAsset::isFile(dst)) {
            // копируем в кэш
            zFileAsset("data/pokes.txt", true).copy(dst, 0);
        }
    }
    parserJoyPokes(zString8((cstr)zFileAsset(dst,true).readn(&size), size).split("\n"));
    // 2. состояние
    frame->send(ZX_MESSAGE_MODEL, MODEL_PENTAGON128);
    // 2.2. сбросить все устройства в исходное состояние
    frame->send(ZX_MESSAGE_RESET);
    frame->onCommand(R.integer.MENU_OPS_RESTORE, nullptr);
    // 2.3. запускаем тред
    if(!pthread_attr_init(&lAttrs)) {
        if(!pthread_attr_setschedpolicy(&lAttrs, SCHED_NORMAL)) {
            struct sched_param param{};
            param.sched_priority = 0;
            if(!pthread_attr_setschedparam(&lAttrs, &param)) {
                pthread_create((pthread_t*)&threadID, &lAttrs, threadFunc, this);
                pthread_attr_getschedparam(&lAttrs, &param);
            }
        }
    }
    return threadID != 0;
}

void zSpeccy::addDev(int type, int access, zDev* dev) {
    devs[type] = dev;
    if(access & ACCESS_RD) {
        for(auto& rdev : rdevs) { if(!rdev) { rdev = dev; break; } }
    }
    if(access & ACCESS_WR) {
        for(auto& wdev : wdevs) { if(!wdev) { wdev = dev; break; } }
    }
}

void zSpeccy::update(int param, int arg) {
    if(param == ZX_UPDATE_MODEL) {
        model = arg; machine = &machines[arg];
    } else if(param == ZX_UPDATE_RESET) {
        programName("BASIC", false);
    }
    divClock = turboClk[cpuSpeed];
    for(auto& dev : devs) dev->update(param);
}

void zSpeccy::execute() {
    static auto tm1(z_timeMillis());
    static int resetSnd(0);
    auto mix(dev<zDevMixer>());
    auto cpu(dev<zCpuMain>());
    if(execLaunch) {
        if(sndLaunch && !(++resetSnd & 4095)) {
            // обновление через 1 минуту 22 секунды
            update(ZX_UPDATE_STATE, 0);
        }
        // убрать иконку
        frame->post(MSG_UPDATE_STATUS, 0, -1);
        auto speed(cpuSpeed - 5); if(speed < 1) speed = 1;
        auto count(isQTape() ? 16 : speed);
        auto lm1((int)(20 * divClock));
        for(int i = 0 ; i < count; i++) {
            mix->begin(0);
            cpu->update(ZX_UPDATE_FRAME);
            mix->finish(machine->tsTotal + cpu->frame);
            mix->update(ZX_UPDATE_FRAME);
            auto tm2(z_timeMillis());
            auto tm((int)(tm2 - tm1));
            if(tm > 1000) tm = lm1, tm1 = tm2;
            if(!isQTape()) {
                auto lm2(lm1 - tm);
                if(lm2 > 0) {
                    usleep(lm2 * 1000);
                    tm2 = z_timeMillis();
                    tm = (int)(tm2 - tm1);
                }
            }
            nfps = 1000.0f / (float)tm;
            // компенсация задержки кадра
            tm -= lm1; if(tm > 0) tm2 -= tm;
            tm1 = tm2;
        }
    }
    devs[DEV_ULA]->update(ZX_UPDATE_FRAME);
}

void zSpeccy::writePort(u16 port, u8 val, u32 tick) {
    for(auto& dev : wdevs) {
        if(dev && dev->checkWrite(port)) {
            if(dev->write(port, val, tick)) return;
        }
    }
}

uint8_t zSpeccy::readPort(u16 port, u32 tick) {
    u8 ret(0xFF);
    for(auto& dev : rdevs) {
        if(dev && dev->checkRead(port)) {
            if(dev->read(port, &ret, tick)) break;
        }
    }
    return ret;
}

bool zSpeccy::_load(zFileAsset* fl, int index, int option) {
    u8* ptr; zString8 path(fl->pth()), name; int size(0); bool ret(false);
    if(!(ptr = z_openFile(fl, index, &size, name))) return false;
    auto type(z_extension(name)); zDevVG93* disk(nullptr); zDevTape* tape(nullptr);
    switch(type) {
        case ZX_FMT_EZX:
            ret = restoreState(ptr);
            break;
        case ZX_FMT_WAV: case ZX_FMT_CSW:
            fl->close();
        case ZX_FMT_TAP: case ZX_FMT_TZX:
            tape = dev<zDevTape>();
            ret = tape->open(ptr, size, type);
            // проверить на автозагрузку
            if(ret && option) {
                // запускаем автозагрузку
                ret = load(is48k() ? "data/tap48.zzz" : "data/tap128.zzz", 0);
            }
            break;
        case ZX_FMT_SNA: case ZX_FMT_Z80: case ZX_FMT_ZZZ: {
            ret = dev<zCpuMain>()->open(ptr, size, type);
            break;
        }
        case ZX_FMT_TRD: case ZX_FMT_SCL: case ZX_FMT_FDI:
        case ZX_FMT_UDI: case ZX_FMT_HOBB: case ZX_FMT_HOBC:
        case ZX_FMT_TD0:
            disk = dev<zDevVG93>();
            ret = disk->open(ptr, size, type | ((option & 3) << 4));
            break;
        case ZX_FMT_SCR:
        case ZX_FMT_TGA:
            break;
    }
    if(ret) {
        if(disk) zDevVG93::path(option & 3, path, name);
        else if(tape) tape->path(path, name);
        else if(type < ZX_FMT_ZZZ) programName(name, true);
    }
    delete[] ptr;
    return ret;
}

bool zSpeccy::load(czs& path, int option) {
    zFileAsset file;
    if(!file.open(path, true)) return false;
    if(file.countFiles() > 1) {
        auto selFile(theApp->getForm<zFormSelFile>(FORM_CHOICE));
        selFile->setParams(&file, option);
        selFile->updateVisible(true);
        return true;
    }
    return _load(&file, 0, option);
}

bool zSpeccy::save(czs& path, int option) {
    auto type(z_extension(path));
    zDevVG93* disk(nullptr); zDevTape* tape(nullptr); u8* ret(nullptr);
    switch(type) {
        case ZX_FMT_EZX:
            ret = saveState();
            break;
        case ZX_FMT_CSW: case ZX_FMT_WAV:
            tape = dev<zDevTape>();
            if(tape->save(type)) {
                if(zFile::move(settings->makePath("temp.tmp", FOLDER_CACHE), path)) {
                    tape->path(path, path);
                    return true;
                }
            }
            return false;
        case ZX_FMT_TAP: case ZX_FMT_TZX:
            tape = dev<zDevTape>();
            ret = tape->save(type);
            break;
        case ZX_FMT_TRD: case ZX_FMT_SCL: case ZX_FMT_FDI:
        case ZX_FMT_UDI: case ZX_FMT_HOBB: case ZX_FMT_HOBC:
        case ZX_FMT_TD0:
            disk = dev<zDevVG93>();
            ret = disk->save(type | ((option & 3) << 4));
            break;
        case ZX_FMT_Z80: case ZX_FMT_SNA: case ZX_FMT_ZZZ:
            ret = dev<zCpuMain>()->save(type);
            break;
        case ZX_FMT_SCR:
        case ZX_FMT_TGA:
            break;
    }
    if(ret) {
        if(z_saveFile(path, tmpBuf, (int)(ret - tmpBuf), (option & 128) != 0)) {
            if(disk) zDevVG93::path(option & 3, path, path);
            else if(tape) tape->path(path, path);
            else if(type < ZX_FMT_ZZZ) programName(path, true);
        } else ret = nullptr;
    }
    return ret != nullptr;
}

bool zSpeccy::restoreState(u8* buf) {
    static cstr devNames[] = { "EXT", "MEM", "Z80H", "Z80A", "ULA", "KEYBOARD", "TAPE", "DISK", "JOYSTICK", "MOUSE", "COVOX", "BEEPER", "AY", "MIXER", "VIDEO" };
    auto ptr(buf);
    // меняем модель
    update(ZX_UPDATE_MODEL, ptr[MODEL]);
    // грузим базовые параметры
    memcpy(&flags, ptr, 64); ptr += 64;
    zString8 ptrName((cstr)ptr); ptr += ptrName.size() + 1;
    // восстановление состояния устройств
    for(int i = 0 ; i < DEV_COUNT; i++) {
        ptr = devs[i]->state(ptr, true);
        if(!ptr) {
            ILOG("Восстановление состояния недоступно для - %s. Сброс!", devNames[i]);
            auto ii(i); for(; i < DEV_COUNT; i++) devs[i]->update(ZX_UPDATE_RESET);
            return ii > DEV_KEYB;
        }
    }
    // установить имя сохраненной проги
    programName(ptrName, false);
    return true;
}

uint8_t* zSpeccy::saveState() {
    auto ptr(tmpBuf);
    // сохраняем базовые параметры
    z_memcpy(&ptr, &flags, 64);
    // сохранить имя проги
    z_memcpy(&ptr, progName.str(), progName.size() + 1);
    // сохранение состояния устройств
    for(auto& dev : devs) ptr = dev->state(ptr, false);
    return ptr;
}

void zSpeccy::programName(cstr nm, bool trim) {
    progName = (trim ? z_trimName(nm) : zString8(nm));
    if(auto j = speccy->findJoyPokes(progName)) {
        speccy->joyType = j->joy.type;
        memcpy(speccy->joyKeys, j->joy.keys, sizeof(speccy->joyKeys));
    }
    frame->post(MSG_UPDATE_CONTROLLER, 0);
    modifySTATE(ZX_CAPT, 0)
}

int zSpeccy::diskOperation(int ops, int index, zString8& path) {
    int ret(0), num(index & 3);
    switch(ops & 7) {
        case ZX_DISK_OPS_GET_READONLY:  path = zDevVG93::path(num, nullptr, nullptr); ret = zDevVG93::is_readonly(num); break;
        case ZX_DISK_OPS_EJECT:         zDevVG93::eject(num); ret = 1; break;
        case ZX_DISK_OPS_INSERT:        ret = load(path, num); if(ret) settings->mruOpen(0, path, false); break;
        case ZX_DISK_OPS_SAVE:          ret = save(path, index); break;
        case ZX_DISK_OPS_SET_READONLY:  ret = zDevVG93::is_readonly(num, index >> 7); break;
        case ZX_DISK_OPS_RSECTOR:       ret = zDevVG93::read_sector(num, (index >> 3) + 1); break;
        case ZX_DISK_OPS_TRDOS:         load(is48k() ? "data/tr48.zzz" : "data/tr128.zzz", 0); break;
    }
    return ret;
}

/*
формат покес / джойстика:
[PROGRAMM]
JOY : NAME(KEYS1, KEYS2, ...)
POKE1 NAME = VALUE
POKE2 NAME = VALUE
*/
void zSpeccy::parserJoyPokes(const zArray<zString8>& sarr) {
    // стандартные кнопки джойстиков
    auto jNames(theme->findArray(R.string.joy_names));
    auto kNames(theme->findArray(R.string.key_names2));
    int jKeys(0); Z_JOY_POKES* jpks(nullptr);
    bool isd(false); zString8 poke;
    // поиск значений
    for(auto& str : sarr) {
        str.trim();
        if(str.isEmpty()) {
            if(jpks && jpks->programm.isNotEmpty() && jKeys >= 5) {
                if(poke.isNotEmpty()) jpks->pokes += poke;
                jpokes += jpks;
            } else delete jpks;
            poke.empty();
            jpks = nullptr;
            isd = false;
            continue;
        } else if(!jpks) {
            // имя программы
            if(str.startsWith("[") && str.endsWith("]")) {
                jpks = new Z_JOY_POKES(str.substr(1, str.count() - 2));
                memcpy(jpks->joy.keys, defJoyKeys, sizeof(defJoyKeys)); jKeys = 8;
            }
        } else if(jpks->isValid()) {
            if(str.startsWith("JOY: ")) {
                jKeys = 0;
                // тип джойстика
                auto jName(str.substrAfter("JOY: ").substrBefore("(").trim());
                jName.upper();
                auto jType(jName.indexOf(jNames.get_data(), jNames.size()));
                if(jType == -1) continue;
                jpks->joy.type = jType;
                auto keys(str.substrAfter("(").substrBefore(")"));
                auto lkeys(zString8(stdJoyKeys[jType] + keys).split(","));
                for(auto& k : lkeys) {
                    int idx(k.indexOf(kNames.get_data(), kNames.size()));
                    if(idx == -1) {
                        ILOG("error %s - <jkey: %s>", jpks->programm.str(), k.str());
                        idx = 0;
                    }
                    jpks->joy.keys[jKeys++] = idx;
                    if(jKeys > 7) break;
                }
            } else {
                // добавить покес(имя = значение)
                auto _isd(isdigit(str[0]));
                if(_isd != isd) {
                    if(isd) {
                        jpks->pokes += poke;
                        poke.empty();
                    }
                    isd = _isd;
                }
                poke.appendNotEmpty(str);
            }
        }
    }
}

void zSpeccy::saveJoyPokes() {
    zFile fl(settings->makePath("pokes.txt", FOLDER_CACHE), false);
    auto nmJoys(theme->findArray(R.string.joy_names));
    auto nmkeys(theme->findArray(R.string.key_names2));
    for(auto jp : jpokes) {
        auto type(jp->joy.type); zString8 joy;
        for(int i = (5 * (type != 4)); i < 8; i++) joy.appendNotEmpty(nmkeys[jp->joy.keys[i]], ",");
        fl.writeString(z_fmt8("[%s]", jp->programm.str()), true);
        fl.writeString(z_fmt8("JOY: %s(%s)", nmJoys[type].str(), joy.str()), true);
        for(auto& pk : jp->pokes) fl.writeString(pk, true);
        fl.writeString("", true);
    }
}

void zSpeccy::updateJoyPokes() {
    auto j(findJoyPokes(speccy->progName));
    if(!j) {
        // добавляем
        j = new Z_JOY_POKES(speccy->progName);
        jpokes += j;
    }
    // обновляем
    j->joy.type = speccy->joyType;
    memcpy(j->joy.keys, speccy->joyKeys, sizeof(j->joy.keys));
}

zArray<zString8> zSpeccy::getAllNamesJoyPokes() const {
    zArray<zString8> all;
    for(auto& jp : jpokes) all += jp->programm;
    return all;

}

static zString8 obfuskate(zString8 s, bool discard) {
    s.lower(); s = s.substrBefore("(", s);
    if(discard) {
        for(int i = 0 ; i < s.count(); i++) {
            auto ch(s[i]);
            if(ch >= 97 || ch == '-' || ch == '.') continue;
            if(ch <= 91 && ch >= 65) continue;
            if(!i) s.remove(i--, 1);
            else { s.crop(i); break; }
        }
    } else s.remove(" ");
    return s;
}

void zSpeccy::joyMakePresets(int id) const {
    // образцы
    auto presets(getAllNamesJoyPokes());
    auto spin(manager->idView<zViewSelect>(id));
    if(spin) {
        auto adapt(spin->getAdapter()); auto idx(0);
        adapt->clear(false); adapt->addAll(presets);
        for(int i = 0 ; i < 2; i++) {
            auto prg(obfuskate(speccy->progName, i == 1));
//            ILOG("prg %s", prg.str());
            for(int j = 0 ; j < presets.size(); j++) {
                auto p(obfuskate(presets[j], i == 1));
//                ILOG("%s", p.str());
                if(p == prg) { idx = j; break; }
            }
            if(idx) break;
        }
        spin->setItemSelected(idx);
    }
}
