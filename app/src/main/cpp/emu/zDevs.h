
#pragma once

#include "zStk.h"
enum DEVS {
    DEV_EXT, DEV_MEM, DEV_GS, DEV_CPU, DEV_ULA, DEV_KEYB, DEV_TAPE, DEV_DISK, DEV_JOY, DEV_MOUSE,
    DEV_COVOX, DEV_BEEPER, DEV_AY, DEV_MIX, DEV_COUNT };

class zDev {
public:
    // проверка на порт для чтения
    virtual bool    checkRead(u16 port) const { return false; }
    // проверка на порт на запись
    virtual bool    checkWrite(u16 port) const { return false; }
    // открыть
    virtual bool    open(u8* ptr, size_t size, int type) { return false; }
    // запись в порт
    virtual bool    write(u16 port, u8 val, u32 ticks) { return false; }
    // чтение из порта
    virtual bool    read(u16 port, u8* ret, u32 ticks) { return false; }
    // восстановление/сохранение состояния
    virtual u8*     state(u8* ptr, bool restore) { return ptr; }
    // сохранить
    virtual u8*     save(int type) { return nullptr; }
    // обновление
    virtual void    update(int param) { }
};

class zDevUla: public zDev {
public:
    struct TIMING {
        enum ZONE { Z_SHADOW, Z_BORDER, Z_PAPER };
        void    set(int _t, ZONE _zone = ZONE::Z_SHADOW, u32* _dst = nullptr, int _so = 0, int _ao = 0);
        u32*    dst;
        ZONE	zone;
        int		t, so, ao;
    };
    zDevUla();
    virtual ~zDevUla() { SAFE_A_DELETE(frameBuffer); }
    // индекс устройства
    static constexpr int index() { return DEV_ULA; }
    // проверка на порт для чтения
    virtual bool    checkRead(u16 port) const override { return (port & 0xFF) == 0xFF; }
    // проверка на порт на запись
    virtual bool    checkWrite(u16 port) const override { return !(port & 1) || !(port & 0x8002); }
    // запись в порт
    virtual bool    write(u16 port, u8 val, u32 ticks) override;
    // чтение из порта
    virtual bool    read(u16 port, u8* ret, u32 ticks) override;
    // обновление
    virtual void    update(int param) override;
    // перерисовка части кадра
    void            updateRay(int tact);
    // проверка на позицию луча
    bool            isPaper() const { return timing->zone == TIMING::Z_PAPER;}
    // признак активности гига скрина
    bool            isGigaApply{false};
    // фреймбуфер
    u32*            frameBuffer{nullptr};
protected:
    // отрисовка начального экрана
    void            firstScreen();
    // перерисовка части экрана
    void updatePaper(int t, int count);
    // перерисовка части границы
    void updateBorder(int t, int count);
    // цвет границы
    int colorBorder{7};
    // параметры мерцания
    int blink{0};
    // такт последнего отрисованного пикселя
    int	tm{0};
    // текущий кадр гигаскрина
    int giga{0};
    // таблицы строк атрибутов/экрана
    int atrTab[256]{}, scrTab[256]{};
    // текущая таблица атрибутов
    u8* colorTab{nullptr};
    // видео
    u8* VIDEO{nullptr};
    // тайминги
    TIMING timings[970]{}, *timing{nullptr};
    // таблица атрибутов
    u8 colTab[512]{};
    // цвета
    u32 colors[16]{};
    // признак начального экрана
    bool fscr{true};
    // размер буфера экрана
    static const int szScr{352 * 288};
};

class zDevExtPort: public zDev {
public:
    // индекс устройства
    static constexpr int index() { return DEV_EXT; }
    // проверка порта на запись
    virtual bool    checkWrite(u16 port) const override;
    // проверка порта на чтение
    virtual bool    checkRead(u16 port) const override { return checkWrite(port); }
    // запись в порт
    virtual bool    write(u16 port, u8 val, u32 ticks) override;
    // чтение из порта
    virtual bool    read(u16 port, u8* val, u32 ticks) override;
    // обновление
    virtual void    update(int param) override;
};

class zDevMem: public zDev {
public:
    zDevMem();
    // индекс устройства
    static constexpr int index() { return DEV_MEM; }
    // проверка порта на запись
    virtual bool    checkWrite(u16 port) const override { return !(port & 0x8002); }
    // проверка порта на чтение
    virtual bool    checkRead(u16 port) const override;
    // запись в порт
    virtual bool    write(u16 port, u8 val, u32 ticks) override;
    // чтение из порта
    virtual bool    read(u16 port, u8* val, u32 ticks) override;
    // обновление
    virtual void    update(int param) override;
    // восстановление/сохранение состояния
    virtual u8*     state(u8* ptr, bool restore) override;
    // вернуть страницу ОЗУ
    u8* page(int page) { return &RAM[page << 14]; }
protected:
    // буфер ОЗУ
    u8* RAM{nullptr};
};

class zDevKempston: public zDev {
public:
    // индекс устройства
    static constexpr int index() { return DEV_JOY; }
    // проверка на порт для чтения
    virtual bool    checkRead(u16 port) const override;
    // чтение из порта
    virtual bool    read(u16 port, u8* ret, u32 ticks) override;
    // обновление
    virtual void    update(int param) override;
};

class zDevMouse: public zDev {
public:
    // индекс устройства
    static constexpr int index() { return DEV_MOUSE; }
    // проверка на порт для чтения
    virtual bool    checkRead(u16 port) const override;
    // чтение из порта
    virtual bool    read(u16 port, u8* ret, u32 ticks) override;
    // обновление
    virtual void    update(int param) override;
};

class zDevKeyboard: public zDev {
public:
    // индекс устройства
    static constexpr int index() { return DEV_KEYB; }
    // проверка на порт для чтения
    virtual bool    checkRead(u16 port) const override { return !(port & 1); }
    // чтение из порта
    virtual bool    read(u16 port, u8* ret, u32 ticks) override;
    // обновление
    virtual void    update(int param) override;
};

class zDevSound: public zDev {
public:
    // запуск кадра
    virtual void    begin(u32 tacts);
    // завершение кадра
    virtual void    finish(u32 tacts);
    // обновление
    virtual void    update(int param) override;
    // обновление данных
    void            updateData(u32 tact, u32 l, u32 r);
    // буфер звуковых данных
    SAMPLER*        buffer{nullptr};
    u32             nSamples{0};
    u32             activeCnt{0};
protected:
    // левый/правый канал
    u32        left{0}, right{0};
    // частота процессора/звука
    u32        clock{0}, sample{0};
    // позиция в буфере
    SAMPLER*   cpos{nullptr};
    SAMPLER*   spos{nullptr};
private:
    // завершение кодирования
    void       flush(u32 endtick);
    u32        tick{0}, base_tick{0};
    u32        l1{0}, r1{0};
    u32        l2{0}, r2{0};
    u64        passedClkTicks{0}, passedSndTicks{0};
};

class zDevBeeper: public zDevSound {
public:
    zDevBeeper();
    // индекс устройства
    static constexpr int index() { return DEV_BEEPER; }
    // проверка на порт на запись
    virtual bool    checkWrite(u16 port) const override { return !(port & 1); }
    // запись в порт
    virtual bool    write(u16 port, u8 val, u32 ticks) override { _write(val, ticks, true); return false; }
    // обновление
    virtual void    update(int param) override;
protected:
    void            _write(u8 val, u32 ticks, bool speaker);
    // уровень громкости спикера/магнитофона
    int             volSpk, volMic;
};

class zDevCovox: public zDevSound {
public:
    // индекс устройства
    static constexpr int index() { return DEV_COVOX; }
    // проверка на порт на запись
    virtual bool    checkWrite(u16 port) const override { return ((port & 0x07) == 0x03); }
    // запись в порт
    virtual bool    write(u16 port, u8 val, u32 ticks) override {
        return false;
    }
};

class zDevAY: public zDevSound {
public:
    enum {
        AFINE, ACOARSE, BFINE, BCOARSE, CFINE, CCOARSE, NOISEPER, ENABLE, AVOL,
        BVOL, CVOL, EFINE, ECOARSE, ESHAPE, CUR_REG = 16
    };
    zDevAY();
    // индекс устройства
    static constexpr int index() { return DEV_AY; }
    // проверка на порт для чтения
    virtual bool    checkRead(u16 port) const override { return (port & 0xC0FF) == 0xC0FD; }
    // проверка на порт на запись
    virtual bool    checkWrite(u16 port) const override;
    // запись в порт
    virtual bool    write(u16 port, u8 val, u32 ticks) override;
    // чтение из порта
    virtual bool    read(u16 port, u8* ret, u32 ticks) override { *ret = regs[regs[CUR_REG]]; return false; }
    // обновление
    virtual void    update(int param) override;
    // восстановление/сохранение состояния
    virtual u8*     state(u8* ptr, bool restore) override;
    // запуск кадра
    virtual void    begin(u32 tacts) override;
    // завершение кадра
    virtual void    finish(u32 tacts) override;
    // завершение кодирования
    void            flush(int chiptick);
protected:
    u8      regs[17]{};
    int     denv{0}, t{0};
    u32     bits[10]{};
    u32     ev[6]{}, ns{0}, env{0};
    u32     fv[10]{};
    u32     mult_const{0};
    u32     chip_clock{0}, cpu_clock{0};
    u64     chip_ticks{0}, clk_ticks{0};
    u32     vols[6][32]{};
};

class zCpuMain;
class zDevTape: public zDevBeeper {
public:
#pragma pack(push, 1)
    struct BLK_TAPE {
        BLK_TAPE() : data(nullptr), twb(nullptr), pilots(nullptr), bits(nullptr),
                     size(0), nwp(0), nwb(0), use(0), type(0), pause(0) { }
        // буфер блока/указатель на таблицу волн пилота
        union { u8* data{nullptr}; u8* twp; };
        // указатель на таблицу волн битов
        u8* twb{nullptr};
        // указатель на волны пилота
        u8* pilots{nullptr};
        // указатель на волны битов
        u8* bits{nullptr};
        // общий размер данных блока
        u32 size{0};
        // кол-во волн пилота/кол-во элементов в CALL/счетчик в LOOP/индекс для JUMP
        union { u32 nwp{0}; u16 count; };
        // кол-во волн битов/rate для CSW/уровень сигнала
        union { u32 nwb{0}; u16 index; u8 level; u32 rate; };
        // кол-во импульсов пилота на волну
        u8 nip{0};
        // кол-во импульсов битов на волну
        u8 nib{0};
        // признак обработанного блока
        u8 use{0};
        // тип блока
        u8 type{0};
        // пауза после блока
        u16 pause{0};
    };
#pragma pack(pop)
    // конструктор
    zDevTape();
    // индекс устройства
    static constexpr int index() { return DEV_TAPE; }
    // деструктор
    virtual ~zDevTape() { clear(true); }
    // проверка на порт для чтения
    virtual bool    checkRead(u16 port) const override { return !(port & 1); }
    // проверка на порт для записи
    virtual bool    checkWrite(u16 port) const override { return !(port & 1); }
    // чтение из порта
    virtual bool    read(u16 port, u8* ret, u32 ticks) override;
    // запись в порт
    virtual bool    write(u16 port, u8 val, u32 ticks) override;
    // обновление
    virtual void    update(int param) override;
    // восстановление/сохранение состояния
    virtual u8*     state(u8* ptr, bool restore) override;
    // открыть
    virtual bool    open(u8* ptr, size_t size, int type) override;
    // сохранить
    virtual u8*     save(int type) override;
    // запуск загрузки
    void            startPlay();
    // остановка загрузки
    void            stopPlay();
    // запуск записи
    void            startRecord();
    // остановка записи
    void            stopRecord();
    // стирание ленты
    void            clear(bool all);
    // формирование блока ленты
    BLK_TAPE*       addBlock(u8 type, u8* data, u32 size, u32 add_size);
    // следующий блок данных
    bool            nextBlock();
    // выполнение операции выбор блока
    void            execUI(int index);
    // получение информации о блоке
    BLK_TAPE*       blockInfo(int index);
    // установка текущего блока
    void            setCurrentBlock(int index);
    // перехват загрузки/сохранения
    void            trap(bool load);
    // удалить блок
    void            removeBlock(int index);
    // установка/получение пути
    zString8        path(cstr _path, cstr _name) { if(_path) pth = _path; if(_name) nam = _name; return nam; }
protected:
    // установить волну с повтором
    static int setWaveRLE(u8** ptr, u8 wave, u16 rep);
    // внутреннее восстановление/сохранение состояния
    u8* statePrivate(u8* ptr, bool restore);
    // подсчитать количество импульсов на количество блоков
    i32 resolveCountImpulses(int count);
    // вернуть индекс импульса
    u8 indexOfPulse(u16 pulse);
    // вставка стандартного блока
    BLK_TAPE* addNormalBlock(u8* data, u32 size);
    // вернуть импульс
    u32 getImpulse();
    // массив блоков
    zArray<BLK_TAPE*> blks{};
    // путь к файлу/имя
    zString8 pth{}, nam{};
    // индекс импульсов на блок/пилот
    u32 iblock{0}, ipilot{0};
    // значение текущего бита(64/0)
    u8 bit{0};
    // такты для следующих импульсов
    u64 edge{0};
    // индекс блока для цикла/вызова
    u16 loop{0}, call{0};
    // количество/индекс волны
    u32 waves{0}, iwaves{0};
    // указатель на таблицу волн
    u8* tw{nullptr};
    // текущая волна
    u8 wave{0}, ni{0};
    // количество импульсов
    u16 npulses{0};
    // массив импульсов
    u16 pulses[256]{};
    // текущий PC при загрузке/записи и кол-во пропусков ленты
    int npc{0}, ctape{0};
    // позиция при записи
    u32 rpos{0};
    // процессор
    zCpuMain* cpu{nullptr};
private:
    void writePulse(u32 count, int what, int plen, int volume = 8191);
    bool openCSW(const u8* ptr, size_t size);
    bool openWAV(const u8* ptr, size_t size);
    bool openTAP(u8* ptr, size_t size);
    bool openTZX(u8* ptr, size_t size);
    u8* saveCSW();
    u8* saveWAV();
    u8* saveTAP();
    u8* saveTZX();
    u16 indexIfSaveBlock();
    zFile wfile{};
    u32 wcur{0};
};

class zDevVG93: public zDev {
public:
    class zFLOPPY {
    public:
        enum { GAP1 = 0, GAP2, GAP4A };
        ~zFLOPPY() { eject(); }
        // изъять диск
        void eject() { SAFE_DELETE(disk); path.empty(); name = "Empty"; }
        // обновить информацию о каталоге диска
        void resetCatalog();
        // вернуть признак установленного образа
        bool valid() const { return disk != nullptr; }
        // вернуть признак наличия бута
        bool existBoot();
        // установить образ
        bool insert(u8* data, size_t data_size, int type);
        // сохранить образ
        u8* save(int type);
        // получение количества свободных секторов
        int getFreeSectors();
        // установка текущего трека/головки с текущим периодом
        Z_DISK::Z_TRACK* seek(int t, int h);
        // вернуть текущий трэк
        Z_DISK::Z_TRACK* track() { return valid() ? disk->track(trk, head) : nullptr; }
        // вернуть сектор
        //Z_DISK::Z_TRACK::Z_SECTOR* sector(int sec) { return &track()->sectors[sec]; }
        // вернуть сектор
        Z_DISK::Z_TRACK::Z_SECTOR* sector(int t, int h, int s);
        // признак защиты от записи
        bool readOnly{false};
        // текущая дорожка/головка
        u8 trk{0}, head{0};
        // время раскрутки мотора/задержка привода в миллисекундах
        uint64_t motor{0};
        // путь к файлу
        zString8 path, name{"Empty"};
    protected:
        // образ
        Z_DISK* disk{nullptr};
        // записать данные сектора
        bool writeSEC(int t, int h, int s, u8* data);
        // записать информацию о разметке
        static void writeGAP(u8** buf, int gap);
    private:
        bool makeEmptyTRD();
        bool trdAddFile(u8* header, u8* ptr);
        bool openTD0(u8* ptr, size_t size);
        bool openTRD(u8* ptr, size_t size);
        bool openSCL(u8* ptr, size_t size);
        bool openFDI(u8* ptr, size_t size);
        bool openUDI(u8* ptr, size_t size);
        bool openHOB(u8* ptr, size_t size, bool basic);
        u8* saveTD0();
        u8* saveTRD();
        u8* saveSCL();
        u8* saveFDI();
        u8* saveUDI();
    };
    union {
        struct {
            u8 dosTrack{0}, dosSec{1}, dosHead{0};
            u8 dosDat{0}, dosSts{0};
            u8 dosCmd{0}, dosSys{0};
        };
        u8 dos[7];
    };
    enum { C_WTRK = 0xF0, C_RTRK = 0xE0, C_RSEC = 0x80, C_WSEC = 0xA0, C_RADR = 0xC0, C_INTERRUPT = 0xD0 };
    enum FDD_STATUS {
        ST_BUSY = 0x01, ST_INDEX = 0x02, ST_DRQ = 0x02, ST_TRK00 = 0x04,
        ST_LOST = 0x04, ST_CRCERR = 0x08, ST_NOT_SEC = 0x10, ST_SEEKERR = 0x10,
        ST_RECORDT = 0x20, ST_HEADL = 0x20, ST_WRITEP = 0x40, ST_NOTRDY = 0x80
    };
    enum FDD_STATE {
        S_IDLE, S_WAIT, S_DELAY_BEFORE_CMD, S_CMD_RW, S_FIND_SEC,
        S_READ, S_WRSEC, S_WRITE, S_WRTRACK, S_WR_TRACK_DATA, S_TYPE1_CMD,
        S_STEP, S_SEEKSTART, S_SEEK, S_VERIFY
    };
    enum FDD_CMD {
        /*CB_SEEK_RATE = 0x03,*/ CB_SEEK_VERIFY = 0x04, CB_SEEK_TRKUPD = 0x10, CB_SEEK_DIR = 0x20, CB_SYS_HLT = 0x08, CB_WRITE_DEL = 0x01,
        CB_DELAY = 0x04, CB_MULTIPLE = 0x10, CB_RESET = 0x04/*, CB_SIDE_CMP = 0x02, CB_SIDE = 0x08, CB_SIDE_SHIFT = 3*/
    };
    zDevVG93();
    // индекс устройства
    static constexpr int index() { return DEV_DISK; }
    // проверка на порт для чтения
    virtual bool    checkRead(u16 port) const override;
    // проверка на порт на запись
    virtual bool    checkWrite(u16 port) const override { return checkRead(port); }
    // запись в порт
    virtual bool    write(u16 port, u8 val, u32 ticks) override;
    // чтение из порта
    virtual bool    read(u16 port, u8* ret, u32 ticks) override;
    // обновление
    virtual void    update(int param) override;
    // восстановление/сохранение состояния
    virtual u8*     state(u8* ptr, bool restore) override;
    // монтировать образ
    virtual bool    open(u8* ptr, size_t size, int type) override;
    // сохранение диска
    virtual u8*     save(int type) override;
    // установка/получение пути и имени к образу диска
    static zString8 path(int num, cstr _path = nullptr, cstr _name = nullptr);
    // извлечь
    static void     eject(int num);
    // установка/чтение режима защиты записи
    static int      is_readonly(int num, int write = -1);
    // прочитать содержимое сектора
    static int      read_sector(int num, int sec);
    // перехватчик
    void            trap(u16 pc);
protected:
    // выполнение
    void            exec(u32 ticks);
    // чтение первого байта
    void            read_byte();
    // ищем сектор на дорожке
    void            find_sec();
    // признак готовности
    bool            ready();
    // получение импульса
    void            impulse(int next);
private:
    // процедуры обработки команд
    void            cmdReadWrite();
    void            cmdWriteTrackData();
    void            cmdType1();
    void            cmdStep();
    void            cmdWrite();
    void            cmdPrepareSec();
    void            cmdRead();
    void            cmdWriteSector();
    void            cmdWriteTrack();
    // текущий сектор
    Z_DISK::Z_TRACK::Z_SECTOR* csec{nullptr};
    // номер текущего дисковода/статус
    u8              nfdd{0}, rq{0};
    // следующее время/время ожидания сектора
    u64             time{0}, next{0}, wdrq{0};
    // направление
    i8              direction{0};
    // адрес чтения/записи
    u8*             rwptr{nullptr};
    // длина буфера чтения/записи
    u16             rwlen{0};
    // текущее состояние
    u8	            states{0};
    // текущий КК
    u16             crc{0};
    // текущий дисковод
    zFLOPPY*        fdd{nullptr};
};

class zDevMixer : public zDev {
public:
    zDevMixer();
    virtual ~zDevMixer() { release(); sources.free(); }
    // индекс устройства
    static constexpr int index() { return DEV_MIX; }
    // обновление
    virtual void update(int param) override;
    // добавить источник звука
    void        addSource(zDevSound* _src) { sources += _src; _src->buffer = mixBuffer; }
    // запуск кадра
    void        begin(u32 tacts) { for(auto& src : sources) src->begin(tacts); }
    // завершение кадра
    void        finish(u32 tacts) { for(auto& src : sources) src->finish(tacts); }
    // выполнение смешивания
    void        mix();
protected:
    void        release();
    // признак включенного звука
    bool		isEnable{false};
    // источники звука
    zArray<zDevSound*> sources{};
    // движок
    zSoundPlayer* player{nullptr};
    // локальный буфер
    u16         audioBuffer[4096 * 2]{};
    // буфер всех устройств
    SAMPLER     mixBuffer[4096 * 2]{};
};

