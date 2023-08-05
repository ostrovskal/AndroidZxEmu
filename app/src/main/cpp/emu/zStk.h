
#pragma once

#pragma pack(push, 1)

struct HEAD_SNA {
	enum { S_48 = 49179, S_128_5 = 131103, S_128_6 = 147487 };
	u8 I;
	u16 HL_, DE_, BC_, AF_;
	u16 HL, DE, BC, IY, IX;
	u8 IFF1;                       // 2 бит
	u8 R;
	u16 AF, SP;
	u8 IM, _FE;
	u8 PAGE5[16384];	            // 4000-7FFF
	u8 PAGE2[16384];	            // 8000-BFFF
	u8 PAGEX[16384];	            // C000-FFFF
	// 128k extension
	u16 PC;
	u8 _7FFD;
	u8 TRDOS;
	// остальные страницы для 128K
};

struct HEAD_Z80 {
	u8 A, F;
	u16 BC, HL, PC, SP;
	u8 I, R;
	u8 STATE1;				// 0 -> 7 бит R, 1-3 -> бордер
	u16 DE;
	u16 BC_, DE_, HL_;
	u8 A_, F_;
	u16 IY, IX;
	u8 IFF1, IFF2, IM;
	/* 2.01 extension */
	u16 length;	    	// длина HEAD2 + HEAD3 + если экстра байт для скорпиона
	u16 PC_;				// новый ПС
	u8 model;  			// тип оборудования
	u8 _7FFD;	    		// значение порта 7ffd для 128К
	u8 ROM1; 			    // 255 - ROM1?
	u8 emulateFlags;		// 7 - признак мрдифицированного оборудования
	u8 sndChipRegNumber;	// номер последнего AY регистра
	u8 sndRegs[16];		// содержимое всех AY регистров
	/* 3.0 extension */
	u16 lowTSTATE;			// не знаю зачем он нужен
	u8 highTSTATE;			// аналогично
	u8 reserved;			// резерв
	u8 mgtRomPaged;		// принтер - мне не надо
	u8 multifaceRomPaged;	// мне не надо
	u8 mem0000_1FFF;		// мне не надо
	u8 mem2000_3FFF;		// мне не надо
	u8 keyMaps[10];		// я сделал по своему
	u8 asciiVals[10];		// аналогично
	u8 mgtType;			// тип принтера
	u8 inhibitorButton;	// магическая кнопка - нахрена?
	u8 inhibitorFlag;		// аналогично
	u8 _1FFD;			    // значение порта 1ffd для скорпиона (типа версия 4)
	// блоки данных:
	// 2 - длина блока(65535 - не сжатый, иначе длина сжатой страницы)
	// 1 - номер страницы(48К : 4 - 2, 5 - 0, 8 - 5, 128К и пентагон : 3-10 - 0-7, скорпион: 3-18 - 0-15)
	// типы оборудования:
	// 48К - 2(0,1) 3(0,1) 128К - все кроме(3(3)),12,13, пентагон - 9, скорпион - 10
};

struct HEAD_CSW {
	char     signature[22];
	u8  version_major;
	u8  version_minor;
	u8  unk;
	u16 freq;
	u8  rle;
	u32 what;
};

struct HEAD_WAV {
	u32 fileID;		// RIFF                 0
	u32 fileSize;		// len + 8 + 12 + 16    4
	u32 waveID;		// WAVE                 8
	u32 chunkID;		// fmt_                 12
	u32 wFormatTag;	// 16                   16
	u16 flags;		    // 1                    20
	u16 nChannels; 	// 1/2                  22
	u32 nSamplesPerSec;// 11025/22050/44100    24
	u32 nAvgBytesPerSec;// 11025/22050/44100   28
	u16 nBlockAlign;	// 1                    32
	u16 wBitsPerSample;// 8/16                 34
	u32 nData;			// data                 36
	u32 nDataSize;		// len                  40
};

#define FDD_TRK     86
#define FDD_HEAD    2
#define FDD_SEC     16
#define FDD_LEN		6250
#define FDD_RPS     5 // скорость вращения
#define FDD_FQ		3500000
#define FDD_DL		3500
#define FDD_MS		6
#define FDD_WTRK	(FDD_MS * 24)
#define FDD_WSEC	(FDD_MS * 48)
#define FDD_RFQ		(FDD_FQ / FDD_RPS)	// время оборота
#define FDD_IDX		(FDD_DL * 4)		// время чередования импульса
#define FDD_PRW		(FDD_DL * 8)		// задержка перед операцией RW

struct Z_DISK {
	enum { ID_TRK = 0, ID_HEAD, ID_SEC, ID_LEN, ID_CRC };
public:
	Z_DISK(size_t size) {
		raw = new u8[size];
		memset(raw, 0, (size_t)size);
		memset(tracks, 0, sizeof(tracks));
	}
	Z_DISK(int nTracks, int nHeads) {
		int size(nTracks * nHeads * FDD_LEN);
		raw = new u8[size];
		memset(raw, 0, (size_t)size);
		memset(tracks, 0, sizeof(tracks));
		for(int i = 0; i < nTracks; ++i) {
			for(int j = 0; j < nHeads; ++j) {
				auto t(&tracks[i][j]);
				t->len = FDD_LEN; t->content = raw + FDD_LEN * ((size_t)i * nHeads + j);
			}
		}
	}
	~Z_DISK() { SAFE_A_DELETE(raw); }
	struct Z_TRACK {
		struct Z_SECTOR {
			Z_SECTOR() : caption(nullptr), content(nullptr) {}
			u8  trk() const { return caption[ID_TRK]; }
			u8  head()const { return caption[ID_HEAD]; }
			u8  sec() const { return caption[ID_SEC]; }
			size_t len() const { return (size_t)(128ULL << (caption[ID_LEN] & 3)); }
			u16 crcCaption() const { auto c(caption + ID_CRC); return wordBE(&c); }
			u16 crcContent() const { auto c(content + len());  return wordBE(&c); }
			void crcUpdate() const;
			// заголовок - идет сразу после A1,A1,A1,FE
			u8* caption;
			// содержимое - идет сразу после A1,A1,A1,(F8/FB)
			u8* content;
		};
		Z_TRACK() : len(6400), content(nullptr), total(0) { memset(sectors, 0, sizeof(sectors)); }
		// обновление разметки
		void updateGAP(bool rewrite, u8 head);
		// длина дорожки - 6250 байт
		u16 len;
		// количество секторов
		u8 total;
		// содержимое дорожки
		u8* content;
		// массив секторов
		Z_SECTOR sectors[FDD_SEC];
	};
	Z_TRACK* track(int t, int h) { return &tracks[t][h]; }
	Z_TRACK  tracks[FDD_TRK][FDD_HEAD];
	u8* raw;
};
#pragma pack(pop)

struct ZX_KEY {
	u8 semi, bit;
	u8 vk;
};

struct Z_MACHINE {
	// задержки стейтов для пикселей экрана
	u8 tsDelay[8];
	// индексы страниц ПЗУ
	u8 pageROM[8];
	// страница ПЗУ при старте
	u8 startROM;
	// страницы с задержкой
	u64 delayPages;
	// признак наличия evenM1
	u32 evenM1;
	// длина вызова int
	u32 tsInt;
	// всего стейтов на кадр
	u32 tsTotal;
	// первый цикл кадра
	u32 tsFirst;
	// циклов на линию
	u32 tsLine;
	// количество линий в кадре
	u32 lineFrame;
	// размер границы(высота, ширина)
	u8 borderHeight, borderWidth;
	// частота звукового процессора
	u32 ayClock;
	// частота процессора
	u32 cpuClock;
	// всего страниц ОЗУ
	u8 totalRAM;
	// имя
	cstr name;
};

struct Z_MNEMONIC {
	// циклы
	u8 cycles;
	// приемник/смещение при префиксе
	union { u8 rd; u8 offs; };
	// источник
	union { u8 rs; u8 pref; };
	// операция
	u8 ops;
	// длина
	u8 len;
	// имя для дизасма
	u8 name;
	// флаги
	u8 flags;
};

enum LABEL_TYPE {
	L_UNDEF	= 0, L_LABEL, L_CONST, L_VARIABLE
};

struct Z_LABEL {
	zString name;
	int type, value;
};

enum MNEMONIC_REGS {
	_RC, _RE, _RL, _RF, _RS, _RB, _RD, _RH, _RA, _RI, _RR, _RPC,
	/* 12 */ _N_,
	/* 13 */ _C8, _C16,
	/* 15 */ _ZERO,
	/* 16 */ _PBC, _PDE, _PHL, _P16, _PSP,
	_R16 = 32
};

enum CPU_FLAGS {
	FC = 1, FN = 2, FPV = 4, F3 = 8, FH = 16, F5 = 32, FZ = 64, FS = 128
};

enum CPU_REGS {
	// 0
	STATE = 0, MOUSE0, MOUSE1, MOUSE2,
	RAM, ROM, VID, 
	PORT_FE, PORT_1FFD, PORT_7FFD, PORT_EFF7, PORT_EXT1, PORT_EXT2, PORT_EXT3,
	MODEL,
	COUNT_COMP,
	// 0
	RC = 0, RB, RE, RD, RL, RH, RF, RA,
	// 8
	RC_, RB_, RE_, RD_, RL_, RH_, RF_, RA_,
	// 16
	RXL, RXH, RYL, RYH,
	// 20
	RSPL, RSPH, RPCL, RPCH,
	// 24
	RPCB1, RPCB2,
	// 26
	CALL0, CALL1,
	// 28
	ROP1, ROP2,
	// 30
	FRAME0, FRAME1, FRAME2, FRAME3,
	// 34
	HALT, EI,
	// 36
	RI, RR1, 
	// 38
	IFF1, IFF2, 
	// 40
	IM, RR2, RTMP, 
	// 43
	TACTS,
	// 44
	TICK0, TICK1, TICK2, TICK3, TICK4, TICK5, TICK6, TICK7,
	// bp, banks
	COUNT_CPU = 148
};

enum MNEMONIC_OPS {
	O_OR, O_XOR, O_AND,
	O_ADC8, O_ADD8, O_SBC8, O_SUB8,
	O_ADC, O_ADD, O_SBC,
	O_CP, O_IM, O_AR, O_AI, O_RA, O_XI, O_NEG, O_RRD, O_RLD,
	O_DEC8, O_INC8, O_DEC, O_INC, O_DECP, O_INCP,
	O_IN,
	O_SET, O_RES, O_BIT,
	O_RETN, O_DJNZ, O_RST,
	O_JMP, O_JR, O_CALL, O_RET,
	O_JMPX, O_JRX, O_CALLX, O_RETX,
	O_LDI, O_CPI, O_INI, O_OTI,
	O_EXA, O_EXHD, O_EXSP, O_EXX,
	O_HLT, O_DAA, O_CPL, O_CCF, O_SCF, O_LDSP,
	O_RLC, O_RRC, O_RL, O_RR, O_SLA, O_SRA, O_SLL, O_SRL,
	O_RLCA, O_RRCA, O_RLA, O_RRA,
	O_UNDEF,
	O_SKIP, O_SAVE, O_LOAD, O_ASSGN, O_PUSH, O_POP, O_PREF,
	O_OUT
};

static cstr namesCode[] = { "C", "B", "E", "D", "L", "H", "R", "A",
							"XL", "XH", "YL", "YH", "F", "I",
						   "", "BC", "DE", "HL", "AF", "SP", "IX", "IY", "PC",
						   "NZ", "Z", "NC", "C", "PO", "PE", "P", "M",
						   "(IX", "(IY", "(BC)", "(DE)", "(HL)", "(NN)", "(SP)",
							"JP ", "CALL ", "JR ", "RET ", "DJNZ ", "LD ",
						   "BIT ", "RES ", "SET ",
						   "INC ", "DEC ",
						   "EX ", "IM ",
						   "ADD ", "ADC ", "SUB ", "SBC ", "AND ", "XOR ", "OR ", "CP ",
						   "RLC ", "RRC ", "RL ", "RR ", "SLA ", "SRA ", "SLL ", "SRL ",
						   "OUT ", "IN ",
						   "RST ", "PUSH ", "POP ",
							// без операнда
						   "NOP", "EX AF, AF'",
						   "RLCA", "RRCA", "RLA", "RRA", "DAA", "CPL", "SCF", "CCF",
						   "DI", "EI",
						   "RRD", "RLD",
						   "LDI", "CPI", "INI", "OTI",
						   "LDD", "CPD", "IND",  "OTD",
						   "LDIR", "CPIR", "INIR", "OTIR",
						   "LDDR", "CPDR", "INDR",  "OTDR",
						   "EXX", "EX DE, HL", "EX (SP), ",
						   "RETI", "RETN",
						   "HALT", "NEG",
						   "ORG ", "END ", "DB ", "DW ", "DEFB ", "DEFW ", "REP ", "EQU ",
						   "*IX*", "*IY*", "*ED*",
						   ", ", "0", "LD SP, ", "(C)", nullptr
};

enum DA_MNEMONIC_NAMES {
	C_C, C_B, C_E, C_D, C_L, C_H, C_R, C_A,
	C_XL, C_XH, C_YL, C_YH, C_F, C_I,
	C_NULL, C_BC, C_DE, C_HL, C_AF, C_SP, C_IX, C_IY, C_pc,
	// 23
	C_FNZ, C_FZ, C_FNC, C_FC, C_FPO, C_FPE, C_FP, C_FM,
	// 31
	C_PIX, C_PIY, C_PBC, C_PDE, C_PHL, C_PNN, C_PSP,
	// 38
	C_JP, C_CALL, C_JR, C_RET, C_DJNZ, C_LD,
	C_BIT, C_RES, C_SET,
	C_INC, C_DEC,
	C_EX, C_IM,
	// 50
	C_ADD, C_ADC, C_SUB, C_SBC, C_AND, C_XOR, C_OR, C_CP,
	C_RLC, C_RRC, C_RL, C_RR, C_SLA, C_SRA, C_SLI, C_SRL,
	// 66
	C_OUT, C_IN,
	C_RST, C_PUSH, C_POP,
	C_NOP, C_EX_AF,
	C_RLCA, C_RRCA, C_RLA, C_RRA, C_DAA, C_CPL, C_SCF, C_CCF,
	// 81
	C_DI, C_EI,
	C_RRD, C_RLD,
	C_LDI, C_CPI, C_INI, C_OTI,
	C_LDD, C_CPD, C_IND, C_OTD,
	C_LDIR, C_CPIR, C_INIR, C_OTIR,
	C_LDDR, C_CPDR, C_INDR, C_OTDR,
	// 101
	C_EXX, C_EX_DE, C_EX_SP,
	C_RETI, C_RETN,
	C_HALT, C_NEG, 
	// 108
	C_ORG, C_END, C_DB, C_DW, C_DEFB, C_DEFW, C_REP, C_EQU,
	// 116
	C_IX_NONI, C_IY_NONI, C_ED_NONI,
	C_COMMA, C_ZERO, C_LDSP, C_PC,
	// 123
	C_SRC, C_DST, C_CB_PHL,
	C_N, C_NN, C_NUM
};

enum MNEMONIC_FLAGS {
	_NZ = 1, _Z, _NC, _C, _PO, _PE, _P, _M
};

enum DA_INSTRUCTION_FLAGS {
	DA_PC	= 1, // адрес
	DA_CODE = 2, // машинные коды
	DA_PN	= 4, // содержимое адреса
	DA_PNN	= 8  // вычисляемое значение
};

struct ZIP_FILTER {
	// путь
	cstr path;
	// имя файла
	cstr name;
	// фильтр
	u16* filter;
};

struct SAMPLER {
	unsigned long left;
	unsigned long right;
};
