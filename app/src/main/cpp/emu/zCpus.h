
#pragma once

enum CPU_CYCLES {
    SKP = 0,         // (skip)                   - пропуск операции
    IOP = 8,         // (internal operation)     - внутренние операции
    FIN = 16,        // (finish)                 - завершение команды
    XRD = 24,        // (register read)          - чтение из регистра-приемника
    SDL = 32,        // (rsrc to rdest, high)    - перенос из регистра-источника в регистр-приемник старшего байта 16-разрядного слова из косвенно адресованной ячейки памяти
    SDH = 40,        // (rsrc to rdest, low)     - перенос из регистр-источника в регистр-приемник младшего байта 16-разрядного слова из косвенно адресованной ячейки памяти
    RRD = 48,        // (resiter read)           - чтение из регистра с записью в память
    RWR = 56,        // (register write)         - запись в регистр значения из памяти
    PRT = 64,        // (port read/write)        - чтение/запись в порт
    OPH = 72,        // (operand read, low)      - чтение младшего байта 16-разрядного слова, непосредственно представленного в команде
    OPL = 80,        // (operand read, high)     - чтение старшего байта 16-разрядного слова, непосредственно представленного в команде
    DRD = 88,        // (dist read)              - чтение смещения disp
    OCR = 96,        // (operation code read)    - чтение кода операции
    SRH = 104,       // (stack read, low)        - чтение из стека в младший приемник
    SRL = 112,       // (stack read, high)       - чтение из стека в старший приемник
    SWH = 120,       // (stack write, low)       - запись в стек из младшего источника
    SWL = 128,       // (stack write, high)      - запись в стек из старшего источника
    MRD = 136,       // (memory read)            - чтение из памяти
    PRD = 144,       // (ptr read)
    MWR = 152,       // (memory write)           - запись в память
    PWR = 160,       // (ptr write)
    XWR = 168,       // (register/memory write)  - запись в память-источник и/не в регистр-приемник
};

#pragma pack(push, 1)
struct BREAK_POINT {
    // начальный адрес
    u16 address1;
    // конечный адрес
    u16 address2;
    // маска
    u8 msk;
    // значение
    u8 val;
    // операция со значением
    u8 ops;
    // флаги
    u8 flg;
};

struct CPU {
    union { struct { u8 c; u8 b; }; u16 bc; };      // 0
    union { struct { u8 e; u8 d; }; u16 de; };      // 2
    union { struct { u8 l; u8 h; }; u16 hl; };      // 4
    union { struct { u8 f; u8 a; }; u16 af; };      // 6
    union { struct { u8 c_; u8 b_; }; u16 bc_; };   // 8
    union { struct { u8 e_; u8 d_; }; u16 de_; };   // 10
    union { struct { u8 l_; u8 h_; }; u16 hl_; };   // 12
    union { struct { u8 f_; u8 a_; }; u16 af_; };   // 14
    union { struct { u8 xl; u8 xh; }; u16 ix; };    // 16
    union { struct { u8 yl; u8 yh; }; u16 iy; };    // 18
    union { struct { u8 spl; u8 sph; }; u16 sp; };  // 20
    union { struct { u8 pcl; u8 pch; }; u16 pc; };  // 22
    u16     pcb;									// 24
    u16     _call;									// 26
    u8      op[2];									// 28
    u32     frame;						    		// 30
    u8      halt, ei;								// 34
    u8      i, rl;									// 36
    u8      iff[2];         						// 38
    u8      im, rh;								    // 40
    u8      tmp;									// 42
    u8      tacts;									// 43
    u64     ts;										// 44
    // значения точек останова (8 * 12)
    BREAK_POINT bps[12];						    // 52
    // банки памяти
    u8      nb[4];                                  // 148
    u8*     rb[4];                                  // 152
    u8*     wb[4];                                  // 168
    u8*     _bp;                                    // 184
    inline void updateBanks(u8* r0, u8* w0, u8* rw3, int n) {
        rb[0] = r0; wb[0] = w0; 
        rb[3] = wb[3] = rw3; nb[3] = n; 
    }
    inline void clear() { memset(&c, 0, 52); }
    inline void incFrame(int tik) { frame += tik; tacts += tik; }
    inline u16 im2() const { return (u16)((i << 8) | 255); }
};
#pragma pack(pop)

class zCpu: public CPU, public zDevSound {
public:
    zCpu();
    virtual ~zCpu() { delete[] _bp; _bp = nullptr; }
    // обновление
    virtual void    update(int param) override;
    // вернуть реальный адрес памяти
    u8*             ptr(u16 address) const { return &rb[address >> 14][address & 16383]; }
    // пишем в память 8 бит
    void            _wm8(u16 address, u8 val) { wb[address >> 14][address & 16383] = val; }
    // чтение из памяти 8 бит
    u8              _rm8(u16 address) { return *ptr(address); }
    // чтение из памяти 16 бит
    u16             rm16(u16 address) { return (_rm8(address) | (_rm8((u16)(address + 1)) << 8)); }
    // копирование инструкции
    void            copy(u16 address, u8* buf, int count) { while(count--) wm8(address++, *buf++); }
    // выполнение при трассировке
    void            debug();
    // срабатывание ловушки
    void            setBreak();
    // заполнить ловушки
    void            filledBPS();
    // быстрая установка ловушки
    void            quickBP(int address);
    // быстрая запись
    u8*             speedSave();
    // адреса регистров
    u8*             regs[36]{};
    // требование немаскируемого прерывания
    bool            reqNMI{false};
protected:
    // пишем в память 8 битное значение, с проверкой на ловушку
    void            wm8(u16 address, u8 val);
    // чтение из памяти 8 бит, с проверкой на ловушку
    u8              rm8(u16 address);
    // выполнение операции
    virtual int     exec() = 0;
    // операции ввода/вывода
    virtual bool    operationIO() = 0;
    // внутренние операции
    void            iop();
    // чтение кода операции
    void            ocr(bool main);
    // завершение инструкции
    void            finishCmd();
    // проверка на ловушку
    int             checkBPs(int address, u8 flg, u8 val = 0);
    // вызов подпрограммы
    void            call(u16 address);
    // установка флагов I/R
    void            flagIR();
    // операция чтения/записи
    void            rw();
    // перемотка останова
    int             _halt();
    // запрос на немаскируемое прерывание
    int             irqNmi();
    // запрос на маскируемое прерывание
    int             irqInt();
    // флаги ini/oti
    int             inOu(u8 t);
    // инициализация операнда команды
    u8*             getOperand(u8 o, u8 oo, u16* adr);
    // страницы задержки
    u64             pages{0};
    // доступ к ULA
    zDevUla*        ula{nullptr};
    // доступ к памяти
    zDevMem*        mem{nullptr};
    // адреса источника/приемника
    u8*             src{nullptr}, *dst{nullptr};
    // кол-во тактов на фрейм
    u32             frameTS{0};
    // индекс выполнения IO
    int             io{0};
    // текущий цикл
    int             step{0};
    // текущий набор инструкций
    int             offs{0};
    // префикс #DD/#FD
    int             pref{0};
    // число тактов на цикл
    int             ticks{0};
    // код инструкции
    int             ops{0};
    // текущая инструкция
    Z_MNEMONIC*     zm{nullptr};
    // адреса виртуального приемника памяти
    u16             _dest{0}, _source{0};
    // прочитано из памяти/регистра
    u8              m8{0};
    // операнд расстояние
    i8              dist{0};
    // признак задержки
    bool            dl{false};
};

class zCpuMain: public zCpu {
public:
    zCpuMain();
    // индекс устройства
    static constexpr int index() { return DEV_CPU; }
    // выполнение на кадр
    virtual void     update(int param) override;
    // открыть
    virtual bool     open(u8* ptr, size_t size, int type) override;
    // сохранение/восстановление состояния
    virtual u8*     state(u8* ptr, bool restore) override;
    // сохранить
    virtual u8*     save(int type) override;
protected:
    // выполнение операции
    virtual int     exec() override;
    // операции ввода/вывода
    virtual bool    operationIO() override;
    // перехватчик
    void            trap();
    // вернуть признак задержки
    int             ulaDelay();
    // задержка IO
    bool            delayIO(int low, int high);
    // задержка операции
    bool            delayOps(int value);
    // таблица указателей на источник задержки
    u16*            chkCycl[13]{};
};

class zCpuGs: public zCpu {
//    friend class zWndDebugger;
public:
    enum { gsCmd, gsSts, gsDat, gsOut, gsPage, gsVol0, gsVol1, gsVol2, gsVol3, gsCount };
    zCpuGs();
    // индекс устройства
    static constexpr int index() { return DEV_GS; }
    // проверка на порт для чтения
    virtual bool    checkRead(u16 port) const override { return ((port & 0xF7) == 0xB3 || (port & 0xF7) == 0xBB); }
    // проверка на порт на запись
    virtual bool    checkWrite(u16 port) const override { return checkRead(port); }
    // запись в порт
    virtual bool    write(u16 port, u8 val, u32 ticks) override;
    // чтение из порта
    virtual bool    read(u16 port, u8* ret, u32 ticks) override;
    // сохранение/восстановление состояния
    virtual u8*     state(u8* ptr, bool restore) override;
    // обновление
    virtual void    update(int param) override;
    // начало кадра
    virtual void    begin(u32 tacts) override;
    // завершение кадра
    virtual void    finish(u32 tacts) override;
    // чтение из памяти 8 бит
    u8              rm8(u16 address);
    // внутреннее состояние
    u8              gs[gsCount]{};
protected:
    // выполнение операции
    virtual int     exec() override;
    // операции ввода/вывода
    virtual bool    operationIO() override;
    // выполнение кадра
    bool            updateFrame(u32 clk);
    // обновление банков памяти
    void            updateBanks();
    // сброс данных каналов
    void            flush();
    // множитель
    float           mult{1.0f};
    // тактов при старте кадра
    u64             fstart{0};
    // громкость на каждом из каналов
    u32             vv[4]{};
};

