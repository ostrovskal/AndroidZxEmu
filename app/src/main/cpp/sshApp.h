//
// Created by User on 24.03.2022.
//

#pragma once

#include "zSpeccyLayout.h"
#include "zSpeccyKeyboard.h"

constexpr int FORM_ASM          = 0;
constexpr int FORM_BPS          = 1;
constexpr int FORM_DEBUG        = 2;
constexpr int FORM_SETTING      = 3;
constexpr int FORM_OPEN         = 4;
constexpr int FORM_TZX          = 5;
constexpr int FORM_CHOICE       = 6;
constexpr int FORM_POKES        = 7;
constexpr int FORM_ETZX         = 8;
constexpr int FORM_WAIT         = 9;
//
constexpr int ZFT_UPD_TAPE      = 1;
constexpr int ZFT_UPD_MENU_GROUP= 2;
constexpr int ZFT_UPD_MENU_DISP = 4;
constexpr int ZFT_UPD_MENU_DBG  = 8;
constexpr int ZFT_UPD_MENU_ITEM = 16;
constexpr int ZFT_UPD_CONTROLLER= 32;
constexpr int ZFT_ALL           = 255;

class sshApp : public zViewManager {
public:
    struct Z_JOY_POKES {
        Z_JOY_POKES(czs& nm) : programm(nm) { }
        bool isValid() const { return programm.isNotEmpty(); }
        void clear() { programm.empty(); pokes.clear(); joy.type = -1; memset(joy.keys, 0, sizeof(joy.keys)); }
        struct Z_JOY { int type{0}; u8 keys[8]; };
        bool operator == (cstr nm) const { return programm == nm; }
        // имя программы
        zString8 programm{};
        // параметры джойстика
        Z_JOY joy{};
        // массив покес
        zArray<zString8> pokes{};
    };
    sshApp(android_app* android);
    // запуск
    void run();
    // инициализация
    virtual void setContent() override;
    // обработка событий ввода
    void processEvents(i32 event);
    // выход
    void exitProgramm() { ANativeActivity_finish(android->activity); }
    // вернуть форму
    template<typename T = zViewForm> T* getForm(int idx) const { return (T*)forms[idx]; }
    // обновление/добавление
    void updateJoyPokes();
    // создание и установка пресетов
    void joyMakePresets(int id) const;
    // поиск джойстика по имени программы
    Z_JOY_POKES* findJoyPokes(czs& name) const { return findJoyPokes(jpokes.indexOf(name)); }
    Z_JOY_POKES* findJoyPokes(int idx) const { return (idx >= 0 && idx < jpokes.size() ? jpokes[idx] : nullptr); }
    // главная панель
    zSpeccyKeyboard*    keyb{nullptr};
    zLinearLayout*      main{nullptr};
protected:
    android_app*        android{nullptr};
    // массив форм
    zView*              forms[10]{};
    // вернуть все имена программ, описывающие джойстики/покес
    zArray<zString8> getAllNamesJoyPokes() const;
    // копированние стандартных из вкладов
    static void copyFromAssets(zString8 src, czs& dst);
    // формирование файла для джойстиков/покес
    void parserJoyPokes(const zArray<zString8>& sarr);
    // массив джойстиков/покес
    zArray<Z_JOY_POKES*> jpokes{};
};

inline sshApp* theApp(nullptr);
