//
// Created by User on 20.07.2023.
//

#pragma once

#include "zDevs.h"
#include "zCpus.h"

#define ZSI_TYPE_JOY                19
#define ZSI_KEY_JOY_L               20
#define ZSI_KEY_JOY_U               21
#define ZSI_KEY_JOY_R               22
#define ZSI_KEY_JOY_D               23
#define ZSI_KEY_JOY_Y               24
#define ZSI_KEY_JOY_X               25
#define ZSI_KEY_JOY_A               26
#define ZSI_KEY_JOY_B               27
#define ZSI_GS_RESET                64
#define ZSI_SPEED_TAPE              65
#define ZSI_SND_LAUNCH              66
#define ZSI_FILTER                  67
#define ZSI_EXEC_LAUNCH             68
#define ZSI_DEBUG_LAUNCH            69
#define ZSI_TURBO_MODE              70
#define ZSI_BP_LAUNCH               71
#define ZSI_AY_LAUNCH               72
#define ZSI_SHOW_HEX                73
#define ZSI_SHOW_DEBUGGER           74
#define ZSI_SHOW_DEBUG_ADDR         75
#define ZSI_SHOW_DEBUG_CODE         76
#define ZSI_SHOW_DEBUG_CONTENT      77
#define ZSI_PLAY_TAPE               78
#define ZSI_REC_TAPE                79
#define ZSI_SHOW_REG16              80
#define ZSI_AUTO_TAPE               81
#define ZSI_RESET_TAPE              82
#define ZSI_DARK_THEME              83
#define ZSI_SWAP_MOUSE              84
#define ZSI_GIGA                    85
#define ZSI_DEBUG_CPU               86
#define ZSI_COVOX_LAUNCH            87
#define ZSI_GS_LAUNCH               88
#define ZSI_TRAP_DOS                89
#define ZSI_FPS                     90
#define ZSI_SHOW_TAPE               91
#define ZSI_SIZE_BORDER             94
#define ZSI_SIZE_JOY                95
#define ZSI_SIZE_KEYB               96
#define ZSI_AY_CHIP                 97
#define ZSI_AY_CHANNEL              98
#define ZSI_SND_FREQ                99
#define ZSI_BEEP_VOL                100
#define ZSI_AY_VOL                  101
#define ZSI_COV_VOL                 102
#define ZSI_GS_VOL                  103
#define ZSI_CPU_SPEED               104
#define ZSI_SET_TABBED              105
#define ZSI_DEBUG_MODE              106
#define ZSI_PANEL_MODE              107
#define ZSI_PALETTES                108

enum DEV_ACCESS { ACCESS_NO = 0, ACCESS_WR, ACCESS_RD, ACCESS_RW };

#pragma pack(push, 1)
struct SPECCY {
    u8 flags;												// 0
    u8 mouse[3];											// 1
    u8 ram;													// 4
    u8 rom;													// 5
    u8 vid;													// 6
    u8 _fe;													// 7
    u8 _1ffd;												// 8
    u8 _7ffd;												// 9
    u8 _eff7;												// 10
    u8 _ext1;												// 11
    u8 _ext2;												// 12
    u8 _ext3;												// 13
    u8 model;												// 14
    // 0. Байтовые значения, вычисляемые во время работы программы
    // Значение FPS
    float nfps;												// 15
    // тип джойстика
    u8 joyType;                                             // 19
    // Привазанные к джойстику коды кнопок клавиатуры(4 POV, 4 Buttons)
    u8 joyKeys[8];  										// 20
    // Значения в полурядах клавиатуры
    u8 semiRow[10];											// 28
    // Индекс файла загрузки ленты
    i32 tapeIndex;											// 39
    // всего индексов ленты
    i32 tapeAllIndex;										// 42
    // индекс блока ленты для передачи в UI
    u16 indexBlkUI;											// 46
    // текущий блок ленты
    u16 tapeCurrent;										// 48
    // количество блоков ленты
    u32 tapeCount;                                          // 50
    // для передачи промежуточных данных
    u32 jni;												// 54
    u32 kmode;                                              // 58
    bool cshift, sshift;                                    // 62
    // 1. Булевы значения
    union {
        struct {
            // Признак сброса gs
            bool gsReset;									// 64
            // Признак скоростной загрузки ленты
            bool speedTape;
            // Признак запуска звука
            bool sndLaunch;
            // Признак вывода с фильтром
            bool filter;
            // Признак выполнения программы
            bool execLaunch;
            // Признак активной отладки(выполнение трассировки)
            bool debugLaunch;
            // Признак турбо-режима процессора
            bool turboMode;
            // Признак запуска бипера
            bool bpLaunch;
            // Признак запуска AY
            bool ayLaunch;
            // Признак HEX вывода
            bool showHex;
            // Признак видимости отладчика
            bool showDebugger;
            // Признак отображения адреса инструции
            bool showDebugAddress;
            // Признак режима кода инструкции
            bool showDebugCode;
            // Признак режима содержимого кода
            bool showDebugContent;
            // Признак проигрывания магнитофона
            bool playTape;
            // Признак записи на магнитофон
            bool recTape;
            // Признак 16-тиричного вывода
            bool showReg16;
            // авто старт/стоп ленты
            bool tapeStartStop;
            // Признак сброса ленты при ресете/изменении модели
            bool resetTape;
            // Признак темной темы оформления
            bool darkTheme;
            // Обмен кнопок мыши
            bool swapMouse;
            // Гига скрин
            bool gigaScreen;
            // признак отладки процессора 1
            bool debugCpu;
            // Признак запуска covox
            bool cvLaunch;
            // Признак запуска gs
            bool gsLaunch;
            // Перехват TRDOS процедур
            bool trapDos;
            // Признак отображения FPS
            bool fps;
            // Признак отображения TAPE
            bool showTape;
            //
        };
        bool bools[30];
    };
    // 2. Байтовые значения
    union {
        struct {
            // Размер границы
            u8 sizeBorder;                                     // 94
            // Размер джойстика
            u8 sizeJoy;
            // Размер клавиатуры
            u8 sizeKeyb;
            // Тип звукового сопроцессора
            u8 ayChip;
            // Раскладка каналов в звуковом процессоре
            u8 ayChannel;
            // Частота звука
            u8 sndFreq;
            // Громкость бипера
            u8 bpVolume;
            // Громкость AY
            u8 ayVolume;
            // Громкость Covox
            u8 covVolume;
            // Громкость GS
            u8 gsVolume;
            // Скорость процессора
            u8 cpuSpeed;
            // текущая вкладка в установках
            u8 settingsTab;
            // режим списка отладчика
            u8 debugMode;
            // режим панели
            u8 panelMode;
            // фильтр браузера
            u8 browseFlt;
        };
        u8 uints8[15];
    };
    // 3. Целые значения
    // значения оригинальных цветов (32 * 4)
    u32 colors[32];											// 109
    u8 sectorBuf[256];
    // имя программы
    zString8 progName;									    // 237
    inline bool isQTape() const { return playTape && speedTape; }
    inline bool is48k() const { return (_7ffd & 32) || model == 0; }
    inline int value(int offs) const { return *((u8*)this + offs); }
};
#pragma pack(pop)

class zSpeccy: public SPECCY {
    friend class zFormSelFile;
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
    zSpeccy();
    ~zSpeccy();
    // инициализация
    bool init();
    // загрузка
    bool load(czs& path, int option);
    // запись
    bool save(czs& path, int option);
    // запись в порт
    void writePort(u16 port, u8 val, u32 tick);
    // обновление устройств
    void update(int param, int arg);
    // выполнение
    void execute();
    // установка имени текущей программы
    void programName(cstr name, bool trim);
    // обновление отладчика в режиме паузы
    void updateDebugger();
    // операции с диском
    int diskOperation(int ops, int num, zString8& path);
    // чтение из порта
    u8 readPort(uint16_t port, u32 tick);
    // вернуть процессор
    zCpu* getCpu(int num) const { return (zCpu*)devs[DEV_GS + num]; }
    // применить турбо режим
    int turboDiv(int tik) const { return (tik + turboMode) >> (int)turboMode; }
    // тактов на кадр
    u32 turboTS() const { return (u32)(machine->tsTotal / divClock); }
    // такты с учетом турбирования
    int turboTick(int tick, bool multiply) const { return (int)(multiply ? tick * divClock : tick / divClock); }
    // частота звукового процессора
    u32 turboAyClock() const { return (u32)(machine->ayClock / divClock); }
    // создание и установка пресетов
    void joyMakePresets(int id) const;
    // обновление/добавление
    void updateJoyPokes();
    // поиск джойстика по имени программы
    Z_JOY_POKES* findJoyPokes(czs& name) const { return findJoyPokes(jpokes.indexOf(name)); }
    Z_JOY_POKES* findJoyPokes(int idx) const { return (idx >= 0 && idx < jpokes.size() ? jpokes[idx] : nullptr); }
    // загрузка ПЗУ
    static bool loadROM(u8* ptr, const u8* pages, int count) ;
    // вернуть устройство в соответствии с его типом
    template<typename T> T* dev() { return (T*)devs[T::index()]; }
    // текущая машина
    static Z_MACHINE* machine;
    // делитель частоты
    float divClock{1.0f};
    // загрузчик дисков
    u8* diskBoot{nullptr};
    // признак выхода
    u32 exit{0};
protected:
    // вернуть все имена программ, описывающие джойстики/покес
    zArray<zString8> getAllNamesJoyPokes() const;
    // парсер джойстиков/покес
    void parserJoyPokes(const zArray<zString8>& sarr);
    // внутренний загрузчик
    bool _load(zFile* fl, int index, int option);
    // восстановление состояния
    bool restoreState(u8* ptr);
    // сохранение состояния
    u8* saveState();
    // добавление устройства
    void addDev(int type, int access, zDev* dev);
    // список доступных устройств
    zDev* devs[DEV_COUNT]{};
    // список устройств на чтение/запись
    zDev* rdevs[DEV_COUNT]{}, *wdevs[DEV_COUNT]{};
    // атрибуты треда
    pthread_attr_t lAttrs{};
    // массив джойстиков/покес
    zArray<Z_JOY_POKES*> jpokes{};
};
