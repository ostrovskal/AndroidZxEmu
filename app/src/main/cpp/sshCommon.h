//
// Created by User on 18.04.2023.
//

#pragma once

#include "zostrov/zCommon.h"
#include "R.h"
#include "android_native.h"
#include "sshApp.h"
#include "emu/zStk.h"
#include "emu/zDevs.h"
#include "emu/zCpus.h"
#include "emu/zSpeccy.h"

// Глобальные
int zxCmd(int cmd, int arg1 = 0, int arg2 = 0, cstr arg3 = nullptr);
bool z_unpackBlock(u8* src, u8* dst, const u8* dstE, size_t sz, bool packed);
u8* z_packBlock(u8* src, const u8* srcE, u8* blk, bool sign, u32& newSize);
int z_extension(zString8 name);
u8* z_openFile(zFile* fl, int index, int* size, zString8& name);
bool z_saveFile(zString8 path, void* ptr, int size, bool zipped);
int getLexem(char** text, int x, int& lex);
i32 parseKeyword(int len, zArray<Z_LABEL>* labels);

#define SWAP_REG(a, b)      (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))
#define checkSTATE(st)      (speccy->flags & (st))
#define modifySTATE(a, r)   { speccy->flags &= ~(r); speccy->flags |= (a); }

extern cstr     keyNames[];
inline u32      frequencies[]   = { 48000, 44100, 22050 };
inline cstr     extValid[]      = { "sna", "z80", "tap", "tzx", "csw", "wav", "td0", "trd", "scl", "fdi", "udi",
                                    "$b", "$c", "zip", "scr", "tga", "zzz", "ezx" };
inline cstr     stdJoyKeys[]    = { "KLEFT,KUP,KRIGHT,KDOWN,KFIRE,", "1,4,2,3,5,", "6,9,7,8,0,", "LEFT,UP,RIGHT,DOWN,0,", "" };
inline u8       defJoyKeys[8]   = { 46,47, 48, 49, 50, 42, 29, 13 };
//inline u8       defJoyKeys[8]   = { 4, 6, 7, 5, 9, 9, 9, 9 };
inline u8*		tmpBuf(nullptr);
inline cstr     tzx_names[] = { "NORMAL", "TURBO", "TONE", "PULSES", "PURE DATA", "RECORD",
                                "", "", "CSW", "UNION", "", "", "", "", "", "",
                                "PAUSE", "GROUP START", "GROUP END", "JUMP",
                                "LOOP START", "LOOP END", "CALL", "RETURN",
                                "SELECT", "", "STOP48", "LEVEL", "", "", "", "",
                                "TEXT", "MESSAGE", "ARCHIVE", "HARDWARE", "", "CUSTOM", "SAVE",
                                "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
                                "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "Z" };

inline zString8 z_trimName(zString8 nm, bool _ext = false) {
    auto ext(_ext ? nm.substrAfterLast(".") : "");
    if(ext.isNotEmpty()) ext.insert(0, ".");
    nm = nm.substrAfterLast("/", nm);
    nm = nm.substrBeforeLast(".", nm);
    nm = nm.substrBefore("(", nm);
    return nm.trim() + ext;
}

enum EDITOR_COLORS {
    COLOR_BKG = 16, COLOR_SELECTION, COLOR_TEXT, COLOR_NUMBER, COLOR_STRING, COLOR_ZNAK, COLOR_COMMENT,
    COLOR_OPERATOR, COLOR_MNEMONIC, COLOR_REGISTER, COLOR_FLAG, COLOR_LABEL, COLOR_NUM_BKG, COLOR_NUM_TEXT, COLOR_CURRENT
};

inline u8       cnvLex[] = { COLOR_NUMBER, COLOR_TEXT, COLOR_TEXT, COLOR_TEXT, COLOR_ZNAK, COLOR_STRING, COLOR_COMMENT,
                             COLOR_OPERATOR, COLOR_TEXT, COLOR_MNEMONIC, COLOR_REGISTER, COLOR_FLAG, COLOR_LABEL, COLOR_NUM_TEXT };

inline u8       zsym[256] = { LN, LN, LN, LN, LN, LN, LN, LN, LN, LS, LN, LN, LN, LN, LN, LN, LN, LN, LN, LN, LN, LN, LN, LN,
                              LN, LN, LN, LN, LN, LN, LN, LN, LS, LO, LQ, LO, LO, LO, LO, LQ, LO, LO, LO, LO, LO, LO, LO, LO,
                              LD, LD, LD, LD, LD, LD, LD, LD, LD, LD, LO, LK, LO, LO, LO, LO, LO, LW, LW, LW, LW, LW, LW, LW,
                              LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LO, LO, LO, LO, LW,
                              LO, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW,
                              LW, LW, LW, LO, LO, LO, LO, LN, LN, LN, LN, LN, LN, LN, LN, LN, LN, LN, LN, LN, LN, LN, LN, LN,
                              LN, LN, LN, LN, LN, LN, LN, LN, LN, LN, LN, LN, LN, LN, LN, LN, LN, LW, LW, LW, LW, LW, LW, LW,
                              LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW,
                              LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW,
                              LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW,
                              LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW, LW };

// CPU + GS + CACHE + TRASH PAGES + GS_ROM + CPU_ROM
constexpr int PAGE_TOTAL                = (32 + 30 + 2 + 1 + 2 + 4);
constexpr int ZX_TOTAL_RAM              = (PAGE_TOTAL << 14);
constexpr int ZX_SIZE_TMP_BUF           = 2 * 1024 * 1024;
constexpr int INDEX_DA                  = 1019 * 1024;
constexpr int INDEX_FMT                 = 1020 * 1024;
constexpr int INDEX_CNV                 = 1021 * 1024;
constexpr int INDEX_TEMP                = 1022 * 1024;
constexpr int INDEX_LEXEM               = 1023 * 1024;
constexpr int INDEX_OPEN                = 1024 * 1024;
constexpr int PAGE_RAM_CPU              = 0;
constexpr int PAGE_RAM_GS               = 32;
constexpr int PAGE_CACHE                = 62;
constexpr int PAGE_TRASH                = 64;
constexpr int PAGE_ROM_GS               = 65;
constexpr int PAGE_ROM_CPU              = 67;
constexpr int PAGE_SOS128               = 67;
constexpr int PAGE_SOS48                = 68;
constexpr int PAGE_SYS                  = 69;
constexpr int PAGE_DOS                  = 70;

constexpr int EFF7_16COLORS             = 1;	// 16 colors (4bits per pixel)
constexpr int EFF7_512x192              = 2;	// 512x192 monochrome
constexpr int EFF7_128LOCK              = 4;	// disable memory above 128k
constexpr int EFF7_RAM_MAP_ROM          = 8;	// disable write cache
constexpr int EFF7_TURBO                = 16;	// turbo (0 - on, 1 - off) pentagon sl 2.2
constexpr int EFF7_MULTICOLOR           = 32;	// hardware multicolor
constexpr int EFF7_384x304              = 64;	// 384x304 mode
constexpr int EFF7_CMOS                 = 128;	// cmos

constexpr int ZX_UPDATE_FRAME           = 0;// обновление в кадре
constexpr int ZX_UPDATE_STATE           = 1;// обновление состояния
constexpr int ZX_UPDATE_MODEL           = 2;// при смене модели/таймингов
constexpr int ZX_UPDATE_RESET           = 3;// сброс
constexpr int ZX_UPDATE_PRIVATE         = 4;//

constexpr int ZX_MESSAGE_PROPS          = 0;
constexpr int ZX_MESSAGE_RESET          = 1;
constexpr int ZX_MESSAGE_DISK           = 2;
constexpr int ZX_MESSAGE_MODEL          = 3;
constexpr int ZX_MESSAGE_MAGIC          = 4;
constexpr int ZX_MESSAGE_CPU            = 5;
constexpr int ZX_MESSAGE_LOAD           = 7;
constexpr int ZX_MESSAGE_DEBUG          = 8;
constexpr int ZX_MESSAGE_SAVE           = 9;

constexpr int ZX_ARG_IO_QUICK           = 128;
constexpr int ZX_ARG_IO_MRU             = 64;

// Флаги точек остановки
constexpr int ZX_BP_NONE                = 0;   // не учитывается
constexpr int ZX_BP_EXEC                = 1;   // исполнение
constexpr int ZX_BP_RMEM                = 2;   // чтение из памяти
constexpr int ZX_BP_WMEM                = 4;   // запись в память
constexpr int ZX_BP_RPORT               = 8;   // чтение из порта
constexpr int ZX_BP_WPORT               = 16;  // запись в порт
constexpr int ZX_BP_TRACE               = 32;  // трассировщик

// Условия точек остановки
constexpr int ZX_BP_OPS_EQ              = 0; // ==
constexpr int ZX_BP_OPS_NQ              = 1; // !=
constexpr int ZX_BP_OPS_LS              = 2; // <
constexpr int ZX_BP_OPS_GT              = 3; // >
constexpr int ZX_BP_OPS_LSE             = 4; // <=
constexpr int ZX_BP_OPS_GTE             = 5; // >=

// Типы блоков TZX
constexpr int TZX_NORMAL                = 0x10;
constexpr int TZX_TURBO                 = 0x11;
constexpr int TZX_TONE                  = 0x12;
constexpr int TZX_PULSES                = 0x13;
constexpr int TZX_PURE_DATA             = 0x14;
constexpr int TZX_RECORD                = 0x15;
constexpr int TZX_CSW                   = 0x18;
constexpr int TZX_UNION                 = 0x19;
constexpr int TZX_PAUSE                 = 0x20;
constexpr int TZX_GROUP_START           = 0x21;
constexpr int TZX_GROUP_END             = 0x22;
constexpr int TZX_JUMP                  = 0x23;
constexpr int TZX_LOOP_START            = 0x24;
constexpr int TZX_LOOP_END              = 0x25;
constexpr int TZX_CALL                  = 0x26;
constexpr int TZX_RETURN                = 0x27;
constexpr int TZX_SELECT                = 0x28;
constexpr int TZX_STOP_48K              = 0x2A;
constexpr int TZX_LEVEL                 = 0x2B;
constexpr int TZX_TEXT                  = 0x30;
constexpr int TZX_MESSAGE               = 0x31;
constexpr int TZX_ARCHIVE               = 0x32;
constexpr int TZX_HARDWARE              = 0x33;
constexpr int TZX_CUSTOM                = 0x35;
constexpr int TZX_SAVE                  = 0x36;
constexpr int TZX_Z                     = 0x5A;

// Биты состояний
enum ZX_STATE {
    ZX_TRACE    = 0x01, // запущен под трассировкой
    ZX_TRDOS    = 0x02, // режим диска
    ZX_BP       = 0x04, // сработала точка останова
    ZX_T_UI     = 0x08, // SELECT_BLOCK/MESSAGE/STOP48
    ZX_CAPT     = 0x10, // обновление заголовка
    ZX_CACHE    = 0x20, // страницы кэша
    R_DRQ       = 0x40, // строб байта данных(=1 - есть данные)
    R_INTRQ     = 0x80, // признак готовности
    R_RQ        = (R_DRQ | R_INTRQ)
};

// Модели памяти
constexpr int MODEL_48                  = 0; // Синклер 48К
constexpr int MODEL_128                 = 1; // Синклер 128К
constexpr int MODEL_PENTAGON128         = 2; // Пентагон 128К
constexpr int MODEL_PENTAGON512         = 3; // Пентагон 512К
constexpr int MODEL_SCORPION256         = 4; // Скорпион 256К

// Команды
constexpr int ZX_CMD_MODEL              = 0; // Установка модели памяти
constexpr int ZX_CMD_PROPS              = 1; // Установка свойств
constexpr int ZX_CMD_RESET              = 2; // Сброс
constexpr int ZX_CMD_POKE               = 5; // Установка POKE
constexpr int ZX_CMD_ASSEMBLER          = 6; // Ассемблирование
constexpr int ZX_CMD_TAPE_UI            = 7; // Возврат в ленту из UI
constexpr int ZX_CMD_MAGIC              = 9; // Нажатие на кнопку MAGIC
constexpr int ZX_CMD_DISK_OPS           = 10;// Операции с диском

// Дисковые операции
constexpr int ZX_DISK_OPS_GET_READONLY  = 0; // вернуть признак только для чтения
constexpr int ZX_DISK_OPS_EJECT         = 1; // изъять образ из дисковода
constexpr int ZX_DISK_OPS_INSERT        = 2; // вставить образ в дисковод
constexpr int ZX_DISK_OPS_SAVE          = 3; // сохранить образ
constexpr int ZX_DISK_OPS_SET_READONLY  = 4; // установить признак только для чтения
constexpr int ZX_DISK_OPS_TRDOS         = 5; // запустить TRDOS
constexpr int ZX_DISK_OPS_RSECTOR       = 6; // прочитать сектор диска

// Операции трассировки в отладчике
constexpr int ZX_CMD_TRACE_IN           = 0; // Трассировка с заходом
constexpr int ZX_CMD_TRACE_OUT          = 1; // Трассировка с обходом
constexpr int ZX_CMD_TRACE_OVER         = 2; // Трассировка с выходом

// Режимы отображения в отладчике
constexpr int ZX_DEBUGGER_MODE_PC       = 0; // Список ДА
constexpr int ZX_DEBUGGER_MODE_SP       = 1; // Список СП
constexpr int ZX_DEBUGGER_MODE_DT       = 2; // Список ДТ

// Форматы файлов
constexpr int ZX_FMT_SNA                = 0; // .sna
constexpr int ZX_FMT_Z80                = 1; // .z80
constexpr int ZX_FMT_TAP                = 2; // .tap
constexpr int ZX_FMT_TZX                = 3; // .tzx
constexpr int ZX_FMT_CSW                = 4; // .csw
constexpr int ZX_FMT_WAV                = 5; // .wav
constexpr int ZX_FMT_TD0                = 6; // .td0
constexpr int ZX_FMT_TRD                = 7; // .trd
constexpr int ZX_FMT_SCL                = 8; // .scl
constexpr int ZX_FMT_FDI                = 9; // .fdi
constexpr int ZX_FMT_UDI                = 10; // .udi
constexpr int ZX_FMT_HOBB               = 11;// .$b
constexpr int ZX_FMT_HOBC               = 12;// .$c
constexpr int ZX_FMT_ZIP                = 13;// .zip
constexpr int ZX_FMT_SCR                = 14;// .scr
constexpr int ZX_FMT_TGA                = 15;// .tga
constexpr int ZX_FMT_ZZZ                = 16;// состояние
constexpr int ZX_FMT_EZX                = 17;// состояние

// сообщения
constexpr int MSG_SHOW_TZX_INFO         = 1000;
constexpr int MSG_HIDE_TZX_INFO         = 1001;
constexpr int MSG_FORM_MSG              = 1002;
constexpr int MSG_CLOUD_REAUTCH         = 1003;
//constexpr int MSG_              = 10;

//constexpr int ZARG_              = 0;

void z_chkGet(u8** p, u8* v, int s);
void z_chkSet(u8** p, u8* v, int s);
