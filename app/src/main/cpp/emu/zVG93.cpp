
#include "../sshCommon.h"
#include "zDevs.h"

// адреса команд TRDOS - 5.03
// таблица - #2FF3(#2FA9) - сначала код команды, далее адреса
// выборка - #034E(#0344)
// CAT(#0433), *(#1018), FORMAT(#1EC2), MOVE(#16AB), NEW(#053A), ERASE(#0787), LOAD(#1815), SAVE(#1AD0),
// RETURN(#1CFB), PEEK(#19A5), POKE(#19A9), MERGE(#19B1), RUN(#1D4D), OPEN(#2182), CLOSE(#2656), 
// COPY(#0690), 40(#2997), GOTO(#2DA1), 80(#29AE), LIST(#11CE), VERIFY(#1810)
// run - 23825 - адрес строки-параметра
// ----------------------------------------------------------------------------------------------------------------------------------
// Бит      | Восст. и позиц.  |    Запись сектора  | Чтение сектора    |   Чтение адреса   |   Запись дорожки  |   Чтение дорожки  |
// ----------------------------------------------------------------------------------------------------------------------------------
//  7       |               Г   о   т   о   в   н   о   с   т   ь       д   и   с   к   о   в   о   д   а                           |
// ----------------------------------------------------------------------------------------------------------------------------------
//  6       |        З А Щ И Т А   З А П И С И      |                   0                   |   ЗАЩИТА ЗАПИСИ   |         0         |
// ----------------------------------------------------------------------------------------------------------------------------------
//  5       |     ЗАГРУЗКА     |       ОШИБКА       |    ПОВТОРЯЕТ      |                   |      ОШИБКА       |                   |
//          | МАГНИТНОЙ ГОЛОВКИ|       ЗАПИСИ       |  ЗНАЧЕНИЕ БИТА    |         0         |      ЗАПИСИ       |         0         |
// ----------------------------------------------------------------------------------------------------------------------------------
//  4       |  ОШИБКА ПОИСКА   |          C  Е  К  Т  О  Р   Н  Е   Н  А  Й  Д  Е  Н        |                   0                   |
// ----------------------------------------------------------------------------------------------------------------------------------
//  3       |                                     ОШИБКА CRC                                |                   0                   |
// ----------------------------------------------------------------------------------------------------------------------------------
//  2       |    ДОРОЖКА 0     |                                            ПОТЕРЯ  ДАННЫХ                                          |
// ----------------------------------------------------------------------------------------------------------------------------------
//  1       |ИНДЕКСНЫЙ ИМПУЛЬС |                                            ЗАПРОС  ДАННЫХ                                          |
// ----------------------------------------------------------------------------------------------------------------------------------
//  0       |                И   д   е   т           в   ы   п   о   л   н   е   н   и   е       к   о   м   а   н   д   ы          |
// ----------------------------------------------------------------------------------------------------------------------------------
// КОЛИЧЕСТВО   /  КОД     /  НАЗНАЧЕНИЕ
//   ~40/80     /  FF/4E   /   Последний пробел
//  6/12        /    0     /
//  3           /   F6     /  Поле С2     (MFM)
//  1           /   FC     /  Индексная метка
//  26/50       /  FF/4E   /  Первый пробел
//  6/12        /   0      /
//  3           /   F5     /   Попе А1      (MFM)
//  1           /   FE     /   Адресная метка заголовка сектора
//  1           /   xx     /   Номер дорожки
//  1           /   xx     /   Номер стороны дискеты
//  1           /   xx     /   Номер сектора
//  1           /   xx     /   Длина сектора
//  2           /   F7     /   Формирование двух байтов CRC заголовка
//  11/22       /   FF/4E  /   Второй пробел
//  6/12        /   0      /
//  3           /   F5     /   Попе А1      (MFM)
//  1           /   FB     /   Адресная метка данных
//  xx          /   xx     /   Данные 256/512/1024/2048/4096
//  2           /   F7     /   Формирование двух байтов CRC данных
//  27/54       /   FF/4E  /   Третий пробел
//  247/598     /   FF/4E  /   Четвертый пробел

/*
	По расчетам из спецификации к ВГ-93 на дорожку
	приблизительно помещается 10364 байт данных в
	режиме MFM и 5156 байт в режиме FM.

	Однако в TR-DOS'е с режимом MFM типичной длиной
	дорожки считается 6250 байт (6208...6464 байт).
	Эта цифра и взята за основу, т.к. при первом
	варианте случаются ошибки. Для режима FM взято
	соответственно число 3125.
*/

// Field		FM					MFM
// =====================================================
// GAP I		0xff * 16 (min)		0x4e * 32 (min)
// GAP II		0xff * 11 (exact)	0x4e * 22 (exact)
// GAP III		0xff * 12 (min)		0x4e * 24 (min)
// GAP IV		0xff * 16 (min)		0x4e * 32 (min)
// SYNC			0x00 * 6  (exact)	0x00 * 12 (exact)

// 0000 HVXX - восстановление
//     переход на нулевую дорожку
// позиционирование:
// I=1 дорожка меняет свое значение
// H - определяет положение магнитной головки дисковода во время выполнения команды
// V - в случае установки магнитной головки в рабочее положение (h=l) этот бит используется для задания режима проверки положения головки. Если
//     бит установлен, то в конце операции содержимое регистра дорожки сравнивается с действительным номером, считанным с дискеты.
// 0000 HVXX - восстановление
// 010I HVXX - на один шаг вперед
// 011I HVXX - на один шаг назад
// 001I HVXX - на один шаг в том же направлении
// 0001 HVXX - позиционирование магнитной головки на заданную дорожку. Номер в регистр данных.

// 100M SEC0 - Чтение секторов.
// M -  бит задает количество секторов, участвующих в операции. Если он сброшен, то обрабатывается один сектор,
//      если установлен — обрабатываются последовательно все сектора на текущей дорожке, начиная с того, который указан в регистре сектора
// S -  значение этого бита определяет номер стороны дискеты (0 — нижняя сторона, 1 — верхняя). Микросхема не имеет аппаратных сигналов для
//      выбора магнитных головок на дисководе (это делает системный регистр Beta Disk-интерфейса), но номер стороны содержится в заголовке сектора
// E -  этот бит используется для задания задержки при установке магнитной головки в рабочее положение. Если бит сброшен,
//      задержка не производится, в противном случае между выдачей сигнала на установку головки в рабочее состояние и началом операции делается задержка в 15 мс
// C -  значение этого бита определяет, делать или не делать проверку стороны дискеты при операции. Если бит сброшен, то проверки не производится
// 101M SECA - Запись секторов
// A -  указывает на один из двух возможных форматов сектора. В дальнейшем при считывании этот формат будет индицироваться в 5 бите системного регистра.
//      Обычно этот бит обнуляют, при этом в поле заголовка сектора формируется специальный байт #FB, в противном случае — байт #F8.
// 1111 0E00 - Запись (форматирование) дорожки FM/MFM
// 1110 0E00 - Чтение дорожки
//              Эта команда считывает с дорожки всю имеющуюся на ней информацию, включая поля пробелов, заголовков и служебные байты.
// 1100 0E00 - Чтение адреса
//              По этой команде с дискеты считывается первый встреченный заголовок сектора. Из поля заголовка передаются 6 байтов —
//              4 байта информационные (номер дорожки, номер стороны, номер сектора и длина сектора) и 2 байта контрольного кода.
//              При выполнении этой Команды содержимое регистра дорожки пересылается в регистр сектора

/////////////////////////////////////////////////////////////////////////////////////////////////
//                                       ДИСКЕТА                                               //
/////////////////////////////////////////////////////////////////////////////////////////////////

void Z_DISK::Z_TRACK::Z_SECTOR::crcUpdate() const {
	auto ptr(content + len());
	wordBE(&ptr, z_crc(content - 1, len() + 1));
}

void Z_DISK::Z_TRACK::updateGAP(bool rewrite, uint8_t head) {
	//auto src(content);
	int l(len - 8), i(0); total = 0;
	auto s(content), e(s + l);
	while(s < e) {
		auto sc(&sectors[total]);
		while(s < e) {
			// поиск маркера заголовка
			if(*s++ == 0xA1 && *s == 0xFE) {
				sc->caption = ++s; ++s;
				if(rewrite) *s = head;
				s += 3; sc->content = nullptr;
				wordBE(&s, z_crc(sc->caption - 1, 5));
				break;
			}
		}
		// длина маркера данных 30-SD, 43-DD
		auto m(s + 43);
		while(s < m && s < e) {
			// поиск маркера данных
			if(*s++ == 0xA1 && (*s == 0xFB || *s == 0xF8)) {
				sc->content = ++s; s += 258;
				break;
			}
		}
		if(total++ > FDD_SEC) {
			// слишком много секторов
			ILOG("слишком много секторов %i", total);
			break;
		}
	}
	total--;
}

bool zDevVG93::zFLOPPY::existBoot() {
	for(int i = 0; i < 8; ++i) {
		auto sc(sector(0, 0, (uint8_t)i));
		if(sc) {
			for(int p = 0; p < 256; p += 16) {
				if(!memcmp(sc->content + p, "boot    B", 9)) return true;
			}
		}
	}
	return false;
}

Z_DISK::Z_TRACK* zDevVG93::zFLOPPY::seek(int t, int h) {
	if(valid()) { trk = (uint8_t)t, head = (uint8_t)h; }
	return track();
}

int zDevVG93::zFLOPPY::getFreeSectors() {
	auto catalog(sector(0, 0, 9));
	return (catalog ? *(uint16_t*)(catalog->content + 229) : 0);
}

void zDevVG93::zFLOPPY::resetCatalog() {
	seek(0, 0);
	auto catalog(sector(0, 0, 9));
	if(catalog) {
		auto buf(catalog->content + 225);
		*buf++ = 0;				// первый свободный сектор
		*buf++ = 1;				// первая свободная дорожка
		*buf++ = 0x16;			// тип диска
		*buf++ = 0x00;			// количество файлов на диске(включая удаленные)
		wordLE(&buf, 2544);		// количество свободных секторов 2544
		*buf++ = 16;			// количество секторов на дорожке
		wordLE(&buf, 0);		// ???
		z_memset(&buf, 32, 9);  // password
		*buf++ = 0;
		*buf++ = 0;				// количество удаленных файлов
		buf[244] = 0;
		memcpy(buf, "sshEmu  ", 8); // метка диска(8 байт)
		catalog->crcUpdate();
	}
}

Z_DISK::Z_TRACK::Z_SECTOR* zDevVG93::zFLOPPY::sector(int t, int h, int s) {
	auto tr(disk->track(t, h)); auto i(0);
	if(tr) {
		while(i < tr->total) {
			auto sec(&tr->sectors[i++]);
			if(sec->sec() == s) return sec;
		}
	}
	return nullptr;
}

bool zDevVG93::zFLOPPY::writeSEC(int t, int h, int s, uint8_t* data) {
	auto sc(sector(t, h, s));
	if(!sc || !sc->content) return false;
	memcpy(sc->content, data, (size_t)sc->len());
	sc->crcUpdate();
	return true;
}

void zDevVG93::zFLOPPY::writeGAP(u8** ptr, int gap) {
	static u8 gaps[] = {  40, 12, 0xA1, 3, 0xFE, // GAP1
			        	  22, 12, 0xA1, 3, 0xFB, // GAP2
						  80, 12, 0xC2, 3, 0xFC  // GAP4A
	};
	auto buf(*ptr), gp(&gaps[gap * 5]);
	z_memset(&buf, 0x4E, gp[0]);
	z_memset(&buf, 0, gp[1]);
	z_memset(&buf, gp[2], gp[3]);
	*buf++ = gp[4]; *ptr = buf;
}

bool zDevVG93::zFLOPPY::insert(uint8_t* ptr, size_t size, int type) {
	motor = 0; bool ret(false);
	switch(type) {
		case ZX_FMT_TD0: ret = openTD0(ptr, size);
		case ZX_FMT_UDI: ret = openUDI(ptr, size);
		case ZX_FMT_FDI: ret = openFDI(ptr, size);
		case ZX_FMT_TRD: ret = openTRD(ptr, size);
		case ZX_FMT_SCL: ret = openSCL(ptr, size);
		case ZX_FMT_HOBC:
		case ZX_FMT_HOBB: ret = openHOB(ptr, size, type == ZX_FMT_HOBB);
	}
	if(ret) {
		// проверить на бут
		if(!existBoot()) {
			// проверить на свободные сектора
			auto trb(speccy->diskBoot);
			if(getFreeSectors() >= 3 && trb) {
				// дописать загрузочный сектор
				openHOB(trb, 273, false);
			}
		}
	}
	return ret;
}

uint8_t* zDevVG93::zFLOPPY::save(int type) {
	switch(type) {
		case ZX_FMT_TD0: return saveTD0();
		case ZX_FMT_UDI: return saveUDI();
		case ZX_FMT_FDI: return saveFDI();
		case ZX_FMT_TRD: return saveTRD();
		case ZX_FMT_SCL: return saveSCL();
	}
	return nullptr;
}

bool zDevVG93::zFLOPPY::trdAddFile(uint8_t* header, uint8_t* ptr) {
	auto catalog(sector(0, 0, 9));
	if(!catalog) return false;
	// кол-во секторов в файле
	int len(header[13]);
	// количество файлов на диске(включая удаленные)
	int pos(catalog->content[228] * 16);
	auto dir(sector(0, 0, 1 + pos / 256));
	if(!dir) return false;
	// свободных секторов
	auto cnt(catalog->content + 229);
	if(wordLE(&cnt) < len) return false;
	// заголовок файла
	memcpy(dir->content + (pos & 0xff), header, 14);
	cnt = catalog->content + 225;
	*(uint16_t*)(dir->content + ((pos & 0xff) + 14LL)) = wordLE(&cnt);
	dir->crcUpdate();
	pos = catalog->content[225] + (catalog->content[226] << 4);
	catalog->content[225] = (uint8_t)((pos + len) & 0x0f);
	catalog->content[226] = (uint8_t)((pos + len) >> 4);
	catalog->content[228]++;
	cnt = catalog->content + 229;
	*(uint16_t*)(catalog->content + 229) = (uint16_t)(wordLE(&cnt) - len);
	catalog->crcUpdate();
	// перейти на следующую дорожку
	for(int i = 0; i < len; ++i, ++pos) {
		int t(pos / 32), h((pos / 16) & 1);
		if(!writeSEC(t, h, (pos & 0x0f) + 1, ptr + i * 256LL))
			return false;
	}
	return true;
}

bool zDevVG93::zFLOPPY::makeEmptyTRD() {
	// варианты следования секторов на дорожке
	static const u8 lv[3][16] = { { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 },
                                  { 1, 9, 2, 10, 3, 11, 4, 12, 5, 13, 6, 14, 7, 15, 8, 16 },
                                  { 1, 12, 7, 2, 13, 8, 3, 14, 9, 4, 15, 10, 5, 16, 11, 6 }
	};

	auto dsk(disk);
	disk = new Z_DISK(FDD_TRK, FDD_HEAD);
	for(u8 i = 0; i < FDD_TRK; ++i) {
		for(u8 j = 0; j < FDD_HEAD; ++j) {
			auto tr(seek(i, j));
			auto buf(tr->content), dat = buf;
			memset(buf, 0, tr->len);
			writeGAP(&buf, zDevVG93::zFLOPPY::GAP4A);
			tr->total = 16;
			for(u8 s = 0; s < 16; ++s) {
				auto sc(&tr->sectors[s]);
				writeGAP(&buf, zDevVG93::zFLOPPY::GAP1);
				sc->caption = buf;
				// marker, track, head, sector, length, crc
				*buf++ = i; *buf++ = j; *buf++ = lv[1][s]; *buf++ = 1;
				wordBE(&buf, z_crc(sc->caption - 1, 5));
				writeGAP(&buf, zDevVG93::zFLOPPY::GAP2);
				auto l(sc->len());
				sc->content = buf; buf += l;
				wordBE(&buf, z_crc(sc->content - 1, l + 1));
			}
			// дорожка слишком длинная
			if(buf > (dat + tr->len)) {
				SAFE_DELETE(disk);
				disk = dsk;
				return false;
			}
			// GAP3 - 426 bytes
			z_memset(&buf, 0x4E, 6250 - (buf - dat));
		}
	}
	resetCatalog();
	return true;
}

bool zDevVG93::zFLOPPY::openTRD(u8* ptr, size_t size) {
	auto dsk(disk);
	if(!makeEmptyTRD()) return false;
	if(size > 655360) size = 655360;
	for(int i = 0; i < size; i += 256) {
		int t(i >> 13), h((i >> 12) & 1), s(((i >> 8) & 15) + 1);
		writeSEC(t, h, s, ptr + i);
	}
	SAFE_DELETE(dsk);
	return true;
}

bool zDevVG93::zFLOPPY::openSCL(uint8_t* ptr, size_t size) {
	if(size < 9) return false;
	auto nFiles((int)ptr[8]), i(0);
	if(memcmp(ptr, "SINCLAIR", 8) != 0 || size < (uint32_t)(279LL * nFiles)) return false;
	ptr += 9;
	// создаем пустой диск
	auto dsk(disk);
	if(!makeEmptyTRD()) return false;
	// вычисляем кол-во секторов
	auto nSectors((uint16_t)0);
	// 0-7 имя 8-тип 9,10-адрес 11,12-размер 13-количество секторов
	for(i = 0 ; i < nFiles; ++i) nSectors += ptr[14 * i + 13];
	// заполнить сектор-каталог
	if(nSectors > 2544) {
		auto catalog(sector(0, 0, 9)); auto content(catalog->content);
		wordLE(&content, nSectors);
		catalog->crcUpdate();
	}
	// добавить файлы
	auto buf(ptr + 14LL * nFiles);
	for(i = 0; i < nFiles; ++i) {
		if(!trdAddFile(ptr + 14LL * i, buf)) {
			// не удалось добавить файл - вернуть предыдущий диск
			SAFE_DELETE(disk); disk = dsk;
			return false;
		}
		buf += (size_t)ptr[14LL * i + 13LL] << 8;
	}
	SAFE_DELETE(dsk);
	return true;
}

bool zDevVG93::zFLOPPY::openFDI(uint8_t* ptr, size_t size) {
	if(memcmp(ptr, "FDI", 3) != 0) return false;
	auto dsk(disk); int nTracks(ptr[4]), nHeaders(ptr[6]), ln;
	auto trks(ptr + 14 + *(u16*)(ptr + 12)), data(ptr + *(u16*)(ptr + 10));
	disk = new Z_DISK(nTracks, nHeaders);
	for(int t = 0; t < nTracks; t++) {
		for(int h = 0; h < nHeaders; h++) {
			auto t0(data + *(u32*)trks); auto nsec(trks[6]); trks += 7;
			auto tr(seek(t, h)); auto buf(tr->content), dat(buf); tr->total = nsec;
			writeGAP(&buf, zDevVG93::zFLOPPY::GAP4A);
			for(int s = 0 ; s < nsec; s++) {
				auto psec(t0 + *(u16*)(trks + 5)); writeGAP(&buf, zDevVG93::zFLOPPY::GAP1);
				auto sc(&tr->sectors[s]); sc->caption = buf;
				// track, head, sector, length, crc
				z_memcpy(&buf, trks, 4); wordBE(&buf, z_crc(sc->caption - 1, 5));
				// дорожка слишком длинная
				if(buf > (dat + tr->len)) { SAFE_DELETE(disk); disk = dsk; return false; }
				writeGAP(&buf, zDevVG93::zFLOPPY::GAP2);
				sc->content = buf; ln = (int)sc->len(); z_memcpy(&buf, psec, ln);
				wordBE(&buf, z_crc(sc->content - 1, ln + 1LL));
				trks += 7;
			}
			// GAP3
			z_memset(&buf, 0x4E, 6250 - (buf - dat));
		}
	}
	SAFE_DELETE(dsk);
	return true;
}

bool zDevVG93::zFLOPPY::openUDI(uint8_t* ptr, size_t size) {
	if(strncmp((char*)ptr, "UDI!", 4) != 0) return false;
	auto dsk(disk); auto buf(ptr + 16 + *(u32*)(ptr + 12));
	int trks(ptr[9]), heads(ptr[10] + 1);//, fsize(*(u32*)(ptr + 4)), version(ptr[8]), unused(ptr[11]);
	disk = new Z_DISK(size); auto raw(disk->raw);
	for(int t = 0; t < trks; t++) {
		for(int h = 0; h < heads; h++) {
			size_t mfm(*buf++), tlen(*(u16*)buf), ctlen((tlen + 7) >> 3);
			if(mfm || ((size_t)(buf - ptr) > size)) { SAFE_DELETE(disk); disk = dsk; return false; }
			buf += 2; auto tr(seek(t, h));
			tr->len = (u16)tlen; tr->content = raw;
			z_memcpy(&raw, buf, tlen); tr->updateGAP(false, h);
			buf += tlen + ctlen;
		}
	}
	SAFE_DELETE(disk);
	return true;
}

bool zDevVG93::zFLOPPY::openHOB(uint8_t* ptr, size_t size, bool basic) {
	static uint8_t header[14];
	if(size < 17) return false;
	// создаем пустой диск
	auto dsk(disk);
	if(basic) {
		if(!makeEmptyTRD())
			return false;
	}
	memcpy(header, ptr, 13);
	header[13] = ptr[14];
	trdAddFile(header, ptr + 17);
	return true;
}

uint8_t* zDevVG93::zFLOPPY::saveTRD() {
	auto ptr(tmpBuf);
	for(int i = 0; i < 2560; i++) {
		auto sec(sector(i >> 5, (i >> 4) & 1, (i & 15) + 1));
		z_memcpy(&ptr, sec->content, sec->len());
	}
	return ptr;
}

uint8_t* zDevVG93::zFLOPPY::saveSCL() {
	auto catalog(tmpBuf), ptr(catalog + 1801), _ptr(ptr), cat(catalog); auto nfiles(0);
	z_memcpy(&cat, "SINCLAIR", 9);
	// считаем кол-во файлов/копируем содержимое файла
	for(int s = 0 ; s < 8 ; s++) {
		auto sc(sector(0, 0, s + 1));
		auto content(sc->content);
		for(int f = 0 ; f < 16; f++) {
			if(content[f * 16] >= 32) {
				nfiles++;
				// копируем заголовок файла
				auto file(&content[f * 16]);
				z_memcpy(&cat, file, 14);
				// копируем содержимое файла
				auto nsecs((int)file[13]), pos(file[14] + (file[15] << 4));
				for(int n = 0 ; n < nsecs; n++) {
					auto _sc(sector(pos / 32, (pos / 16) & 1, (pos & 15) + 1));
					z_memcpy(&ptr, _sc->content, _sc->len());
					pos++;
				}
			}
		}
	}
	catalog[8] = nfiles;
	memcpy(cat, _ptr, ptr - _ptr);
	ptr -= (128LL - nfiles) * 14LL;
	// подсчитать контрольную сумму(сумма всех байт)
	uint32_t _crc(0);
	while(catalog < ptr) _crc += *catalog++;
	dwordLE(&ptr, _crc);
	return ptr;
}

bool zDevVG93::zFLOPPY::openTD0(uint8_t* ptr, size_t size) {
	return false;
}

uint8_t* zDevVG93::zFLOPPY::saveFDI() {
	return nullptr;
}

uint8_t* zDevVG93::zFLOPPY::saveUDI() {
	return nullptr;
}

uint8_t* zDevVG93::zFLOPPY::saveTD0() {
	return nullptr;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//                                      ДИСКОВОД                                               //
/////////////////////////////////////////////////////////////////////////////////////////////////

#define VG93_STT_SET(current, next)      states = ((current) | ((next) << 4))
#define VG93_STT_NEXT                    (uint8_t)((states & 240) >> 4)

// все дисководы
static zDevVG93::zFLOPPY fdds[4];

uint8_t* zDevVG93::state(uint8_t* buf, bool restore) {
	time = speccy->dev<zCpuMain>()->ts;
	auto ptr(buf); uint64_t t64;
	if(restore) {
		if(strncmp((char*)ptr, "SERG BETA128", 12) != 0) return nullptr;
		ptr += 12; memcpy(dos, ptr, sizeof(dos)); ptr += sizeof(dos);
		rq = *ptr++; nfdd = *ptr++; direction = (i8)*ptr++; states = *ptr++;
		rwlen = wordLE(&ptr); crc = wordLE(&ptr);
		next = time + dwordLE(&ptr); wdrq = time + dwordLE(&ptr);
		fdd = &fdds[nfdd];
		for(int i = 0 ; i < 4; i++) {
			fdds[i].trk = *ptr++; fdds[i].head = *ptr++;
			fdds[i].motor = time + dwordLE(&ptr);
			zString8 pth((cstr)ptr); ptr += pth.size() + 1LL;
			auto ok(pth.isNotEmpty());
			if(ok) ok = speccy->load(pth, i);
			if(!ok) fdds[i].eject();
		}
		auto sc(dwordLE(&ptr)); auto rw(wordLE(&ptr));
		if(z_crc(buf, ptr - buf) != wordLE(&ptr)) return nullptr;
		if(sc == 0xFFFFFFFF || rw == 0xFFFF) rwptr = nullptr, csec = nullptr;
		else {
			csec = fdd->sector((int)(sc >> 16) & 0xFF, (int)(sc >> 8) & 0xFF, (int)sc & 0xFF);
			rwptr = rw + fdd->track()->content;
		}
	} else {
		z_memcpy(&ptr, "SERG BETA128", 12);
		z_memcpy(&ptr, dos, sizeof(dos));
		*ptr++ = rq; *ptr++ = nfdd; *ptr++ = (i8)direction; *ptr++ = states;
		wordLE(&ptr, rwlen); wordLE(&ptr, crc);
		t64 = next < time ? 0 : next - time; dwordLE(&ptr, (u32)(t64));
		t64 = wdrq < time ? 0 : wdrq - time; dwordLE(&ptr, (u32)(t64));
		for(auto& f : fdds) {
			*ptr++ = f.trk; *ptr++ = f.head;
			t64 = f.motor; t64 = t64 < time ? 0 : t64 - time;
			dwordLE(&ptr, (u32)(t64));
			auto pth(f.path); z_memcpy(&ptr, pth.buffer(), pth.size() + 1LL);
		}
		uint32_t _csec(0xFFFFFFFF); uint16_t _rw(0xFFFF);
		if(csec && rwptr) {
			_csec = ((csec->trk() << 16) | (csec->head() << 8) | csec->sec());
			_rw = (uint16_t)(rwptr - fdd->track()->content);
		}
		dwordLE(&ptr, _csec); wordLE(&ptr, _rw); wordLE(&ptr, z_crc(buf, ptr - buf));
	}
	return ptr;
}

zDevVG93::zDevVG93() {
	zDevVG93::update(ZX_UPDATE_RESET);
}

bool zDevVG93::open(uint8_t* ptr, size_t size, int type) {
	auto drive((type >> 4) & 3);
	if(drive == nfdd) { csec = nullptr; dosSts = ST_NOTRDY; rq = R_INTRQ; states = S_IDLE; }
	return fdds[drive].insert(ptr, size, type & 15);
}

uint8_t* zDevVG93::save(int type) {
	auto drive((type >> 4) & 3);
	return fdds[drive].save(type & 15);
}

void zDevVG93::exec(uint32_t ticks) {
	time = speccy->dev<zCpuMain>()->ts + ticks;
	// Неактивные диски игнорируют бит HLT
	if(time > fdd->motor && (dosSys & CB_SYS_HLT)) fdd->motor = 0;
	fdd->valid() ? dosSts &= ~ST_NOTRDY : dosSts |= ST_NOTRDY;
	// команды позиционирования
	if(!(dosCmd & 0x80)) {
		// seek/step
		dosSts &= ~(ST_TRK00 | ST_INDEX);
		if(!fdd->trk) dosSts |= ST_TRK00;
		if(fdd->motor) {
			if(dosSys & CB_SYS_HLT) dosSts |= ST_HEADL;
			// индексный импульс - чередуется каждые 4 мс
			if(fdd->valid() && (time % FDD_RFQ) < FDD_IDX) dosSts |= ST_INDEX;
		}
	}
	while(true) {
		switch(states & 15) {
			// шина свободна. команда завершена
			case S_IDLE: dosSts &= ~ST_BUSY; rq = R_INTRQ; return;
			// задержка при выполнении команды
			case S_WAIT: if(time < next) return; states = VG93_STT_NEXT; break;
			// подготовка к чтению/записи
			case S_DELAY_BEFORE_CMD:
				if(dosCmd & CB_DELAY) next += FDD_PRW;
				// сброс статуса
				dosSts = (dosSts | ST_BUSY) & ~(ST_DRQ | ST_LOST | ST_NOT_SEC | ST_RECORDT | ST_WRITEP);
				VG93_STT_SET(S_WAIT, S_CMD_RW);
				break;
			// подготовка операции чтения/записи
			case S_CMD_RW: cmdReadWrite(); break;
			// операция чтения(сектора, адреса, дорожки)
			case S_READ: cmdRead(); break;
			// нашли сектор - запускаем операцию
			case S_FIND_SEC: cmdPrepareSec(); break;
			// запись сектора
			case S_WRSEC: cmdWriteSector(); break;
			// запись дорожки
			case S_WRTRACK: cmdWriteTrack(); break;
			// операция записи
			case S_WRITE: cmdWrite(); break;
			// запись данных дорожки
			case S_WR_TRACK_DATA: cmdWriteTrackData(); break;
			// команды позиционирования
			case S_TYPE1_CMD: cmdType1(); break;
			case S_STEP: cmdStep(); break;
			case S_SEEKSTART:
				if(!(dosCmd & CB_SEEK_TRKUPD)) { dosTrack = dosDat = 0; }
			case S_SEEK:
				if(dosDat == dosTrack) states = S_VERIFY;
				else { direction = (i8)(dosDat < dosTrack ? -1 : 1); states = S_STEP; }
				break;
			case S_VERIFY:
				// для проверки существования сектора или правильности КК
//				if(dosCmd & CB_SEEK_VERIFY) find_sec();
//				states = S_IDLE;
				if(!(dosCmd & CB_SEEK_VERIFY)) { states = S_IDLE; break; }
				find_sec();
				break;
		}
	}
}

void zDevVG93::read_byte() {
	dosDat = *rwptr++; rwlen--;
	crc = z_crc(dosDat, crc);
	rq = R_DRQ; dosSts |= ST_DRQ;
	next += FDD_MS; VG93_STT_SET(S_WAIT, S_READ);
}

bool zDevVG93::ready() {
	if((rq & R_DRQ) == 0) return true;
	if(next > wdrq) return true;
	next = time + FDD_MS;
	VG93_STT_SET(S_WAIT, states);
	return false;
}

void zDevVG93::cmdRead() {
	if(!ready()) return;
	frame->setStatus(R.integer.iconZxDisk);
	if(rwlen) {
		if(rq & R_DRQ) dosSts |= ST_LOST;
		read_byte();
	} else {
		if((dosCmd & 0xe0) == C_RSEC) {
			// если чтение сектора - проверяем на CRC
			auto _crc = csec->crcContent();
			if(crc != csec->crcContent()) dosSts |= ST_CRCERR;
			// если множественная загрузка секторов
			if(dosCmd & CB_MULTIPLE) {
				dosSec++; states = S_CMD_RW;
				return;
			}
		} else if((dosCmd & 0xf0) == C_RADR) {
			// проверяем на CRC
			if(z_crc(csec->caption - 1, 5) != csec->crcCaption()) dosSts |= ST_CRCERR;
		}
		states = S_IDLE;
	}
}

void zDevVG93::cmdWrite() {
	if(!ready()) return;
	frame->setStatus(R.integer.iconZxDisk);
	if(rq & R_DRQ) {
		// потеря данных
		dosSts |= ST_LOST;
		dosDat = 0;
	}
	// запись байта
	crc = z_crc(dosDat, crc);
	*rwptr++ = dosDat; rwlen--;
	if(rwlen) {
		next += 4 * FDD_MS;
		rq = R_DRQ; dosSts |= ST_DRQ;
		VG93_STT_SET(S_WAIT, S_WRITE);
	} else {
		// запись CRC
		*rwptr++ = (uint8_t)(crc >> 8); *rwptr++ = (uint8_t)(crc);
		// завершение операции
		//ILOG("finish write");
		csec = nullptr;
		// проверка на множественные сектора
		if(dosCmd & CB_MULTIPLE) { dosSec++; states = S_CMD_RW; } else states = S_IDLE;
	}
}

void zDevVG93::cmdWriteTrackData() {
	static uint8_t* pcrc(nullptr);
	if(!ready()) return;
	frame->setStatus(R.integer.iconZxDisk);
	// потеря данных
	if(rq & R_DRQ) { dosSts |= ST_LOST; dosDat = 0; }
	auto d(dosDat), v(d);
	if(d == 0xF5) v = 0xA1;
	else if(d == 0xF6) v = 0xC2;
	else if(d == 0xFB || d == 0xFE) pcrc = rwptr;
	else if(d == 0xF7) {
		// считаем КК
		crc = z_crc(pcrc, (size_t)(rwptr - pcrc));
		*rwptr++ = (uint8_t)(crc >> 8); rwlen--; v = (uint8_t)crc;
		pcrc = nullptr;
	}
	*rwptr++ = v; rwlen--;
	if(rwlen) {
		next += 4 * FDD_MS;
		rq = R_DRQ; dosSts |= ST_DRQ;
		VG93_STT_SET(S_WAIT, S_WR_TRACK_DATA);
	} else {
		rwptr = nullptr; states = S_IDLE;
		//ILOG("updateGAP dos_h:%i t:%i h:%i", dosHead, fdd->trk, fdd->head);
		fdd->track()->updateGAP(true, dosHead);
	}
}

void zDevVG93::cmdWriteSector() {
	frame->setStatus(R.integer.iconZxDisk);
	if(rq & R_DRQ) {
		dosSts |= ST_LOST;
		states = S_IDLE;
	} else {
//		ILOG("write sector %i", dosSec);
		rwptr = csec->content;
		auto dat((uint8_t)((dosCmd & CB_WRITE_DEL) ? 0xF8 : 0xFB));
		rwptr[-1] = dat; rwlen = (uint16_t)csec->len();
		crc = z_crc(dat); states = S_WRITE;
	}
}

void zDevVG93::impulse(int s_next) {
	auto t(fdd->track());
	rwptr = t->content; rwlen = t->len;
	VG93_STT_SET(S_WAIT, s_next);
}

void zDevVG93::cmdWriteTrack() {
	frame->setStatus(R.integer.iconZxDisk);
	if(rq & R_DRQ) {
		dosSts |= ST_LOST;
		states = S_IDLE;
	} else {
//		ILOG("write track");
		impulse(S_WR_TRACK_DATA);
		wdrq = next + FDD_RFQ * 5;
	}
}

void zDevVG93::cmdReadWrite() {
	fdd->head = dosHead;
	if(((dosCmd & 0xe0) == C_WSEC || (dosCmd & 0xf0) == C_WTRK) && fdd->readOnly) {
		// проверка на защиту от записи в операциях записи
		dosSts |= ST_WRITEP;
		states = S_IDLE;
	} else if((dosCmd & 0xc0) == 0x80 || (dosCmd & 0xf8) == C_RADR) {
		// операции чтения/записи секторов или чтения адреса - поиск сектора
		wdrq = next + FDD_RFQ * 5;
		find_sec();
	} else if((dosCmd & 0xf8) == C_WTRK) {
		// запись дорожки(форматирование)
		next += FDD_WTRK;
		rq = R_DRQ; dosSts |= ST_DRQ;
		//ILOG("begin wtrack");
		VG93_STT_SET(S_WAIT, S_WRTRACK);
	} else if((dosCmd & 0xf8) == C_RTRK) {
		// чтение дорожки
		impulse(S_READ);
	} else {
		states = S_IDLE;
	}
}

void zDevVG93::cmdType1() {
	dosSts = (dosSts | ST_BUSY) & ~(ST_DRQ | ST_CRCERR | ST_SEEKERR | ST_WRITEP);
	rq = 0;
	if(fdd->readOnly) dosSts |= ST_WRITEP;
	fdd->motor = next + FDD_FQ * 2;
	// поиск/восстановление
	int cmd(S_SEEKSTART);
	if(dosCmd & 0xE0) {
		// один шаг
		if(dosCmd & 0x40) direction = (int8_t)((dosCmd & CB_SEEK_DIR) ? -1 : 1);
		cmd = S_STEP;
	}
	VG93_STT_SET(S_WAIT, cmd);
	next += FDD_DL;
}

void zDevVG93::cmdPrepareSec() {
	fdd->head = dosHead;
	if(dosCmd & 0x20) {
		// запись сектора
		next += FDD_WSEC;
//		ILOG("begin write sector");
		rq = R_DRQ; dosSts |= ST_DRQ;
		VG93_STT_SET(S_WAIT, S_WRSEC);
		return;
	}
	if((dosCmd & 0xf0) == C_RADR) {
		// чтение адресного маркера
		rwptr = csec->caption; rwlen = 6;
	} else {
		// чтение сектора
		rwptr = csec->content; rwlen = (uint16_t)csec->len();
		if(rwptr[-1] == 0xF8) dosSts |= ST_RECORDT; else dosSts &= ~ST_RECORDT;
	}
	crc = z_crc(rwptr[-1]); read_byte();
}

void zDevVG93::cmdStep() {
	if(!fdd->trk && !(dosCmd & 0xf0)) {
		// RESTORE
		dosTrack = 0;
		states = S_VERIFY;
	} else {
		// позиционирование дорожки
		if(!(dosCmd & 0xe0) || (dosCmd & CB_SEEK_TRKUPD)) 
			dosTrack += direction;
		VG93_STT_SET(S_WAIT, ((dosCmd & 0xe0) != 0) + S_SEEK);
		next += FDD_MS * FDD_DL;
	}
	// проверка на допустимые дорожки
	int cyl(dosTrack); if(cyl < 0) cyl = 0; if(cyl >= FDD_TRK) cyl = FDD_TRK - 1;
	fdd->trk = (uint8_t)cyl;
}

void zDevVG93::find_sec() {
	fdd->head = dosHead; fdd->trk = dosTrack; csec = nullptr;
	dosSts &= ~ST_CRCERR; dosSts |= ST_NOT_SEC;
	if(fdd->motor && fdd->valid()) {
		auto tr(fdd->track());
		for(int i = 0; i < tr->total; ++i) {
			auto sc(&tr->sectors[i]);
			if(sc->sec() == dosSec) {
				dosSts &= ~ST_NOT_SEC;
				if(z_crc(sc->caption - 1, 5) == sc->crcCaption()) {
//					ILOG("dosTrk:%i trk:%i", speccy->dosTrack, sc->trk());
					csec = sc; next += FDD_MS;
					VG93_STT_SET(S_WAIT, S_FIND_SEC);
					return;
				} else {
					dosSts |= ST_CRCERR;
					break;
				}
			}
		}
	}
	states = S_IDLE;
}

bool zDevVG93::checkRead(uint16_t port) const {
	if((port & 0x1F) != 0x1F) return false;
	auto p((uint8_t)port);
	return p == 0x1F || p == 0x3F || p == 0x5F || p == 0x7F || p & 0x80;
}

bool zDevVG93::read(uint16_t _port, uint8_t* ret, uint32_t ticks) {
	if(!(checkSTATE(ZX_TRDOS))) {
		auto res(speccy->model == MODEL_SCORPION256 && speccy->rom == PAGE_SYS);
		if(res) *ret = 0x80; 
		return res; 
	}
	auto port((uint8_t)_port);
	exec(ticks);
	if(port == 0x1F) { rq &= R_DRQ; *ret = dosSts; } else if(port == 0x3F) *ret = dosTrack;
	else if(port == 0x5F) *ret = dosSec;
	else if(port == 0x7F) { *ret = dosDat; dosSts &= ~ST_DRQ; rq &= R_INTRQ; }
	else if(port & 0x80) *ret = (uint8_t)(rq & R_RQ);
	return true;
}

bool zDevVG93::write(uint16_t _port, uint8_t v, uint32_t ticks) {
	if(!checkSTATE(ZX_TRDOS)) return false;
	auto port((uint8_t)_port);
	exec(ticks);
	if(port == 0x1F) {
		// прерывание команды
		if((v & 0xf0) == C_INTERRUPT) {
			states = S_IDLE; rq = R_INTRQ; dosSts &= ~ST_BUSY;
			return true;
		}
		if(dosSts & ST_BUSY) return true;
		dosCmd = v;
		next = speccy->dev<zCpuMain>()->ts + ticks;
		dosSts |= ST_BUSY; rq = 0;
		// команды чтения/записи
		if(dosCmd & 0x80) {
			// выйти, если нет диска
			if(dosSts & ST_NOTRDY) { states = S_IDLE; rq = R_INTRQ; return true; }
			// продолжить вращать диск
			if(fdd->motor) fdd->motor = next + FDD_FQ * 2;
			states = S_DELAY_BEFORE_CMD;
		} else {
			// для команд поиска/шага
			states = S_TYPE1_CMD;
		}
	} else if(port == 0x3F) dosTrack = v;
	else if(port == 0x5F) dosSec = v & 31;
	else if(port == 0x7F) { dosDat = v; rq &= R_INTRQ; dosSts &= ~ST_DRQ;}
	else if(port & 0x80) {
		// порт FF - запись:
		dosSys = v;
		// 0,1 - номер дисковода
		nfdd = (uint8_t)(v & 3);
		fdd = &fdds[nfdd];
		// 3 - подготовка, если = 1
		// 4 - сторона диска, 1 = верх
		dosHead = (uint8_t)((v & 0x10) == 0);
		// 6 - плотность, 0 = двойная
		// 2 - сброс, если = 0
		if(!(v & CB_RESET)) update(ZX_UPDATE_RESET);
	}
	return true;
}

void zDevVG93::update(int param) {
	if(param == ZX_UPDATE_RESET) {
		fdd = &fdds[nfdd];
		dosSts = ST_NOTRDY;
		csec = nullptr;
		rq = R_INTRQ;
		fdd->motor = 0;
		VG93_STT_SET(S_IDLE, S_IDLE);
	}
}

// извлечь
void zDevVG93::eject(int num) {
	fdds[num].eject();
}

zString8 zDevVG93::path(int num, cstr _path, cstr _name) {
	if(_path) fdds[num].path = _path;
	if(_name) fdds[num].name = _name;
	return fdds[num].name;
}

int zDevVG93::is_readonly(int num, int write) {
	if(write != -1) fdds[num].readOnly = write != 0;
	return fdds[num].readOnly;
}

int zDevVG93::read_sector(int num, int sec) {
	if(!fdds[num].valid()) return 0;
	auto sector(fdds[num].sector(0, 0, sec));
	if(sector) memcpy(speccy->tmpBuf, sector->content, 256); else memset(speccy->tmpBuf, 0, 256);
	return sector != nullptr;
}

void zDevVG93::trap(uint16_t pc) {
	auto cpu(speccy->dev<zCpuMain>());
	if(pc == 0x1D53) {
		// RUN
		zString nmProg;
		auto adr(cpu->ptr(cpu->rm16(23825)));
		if(*adr++ == 247) {
			if(*adr++ == 0xD) nmProg = "boot";
			else {
				auto e((uint8_t*)strchr((char*)adr, '\"'));
				if(e) {
					auto len((int)(e - adr));
					nmProg = zString(adr, z_min(len, 8));
				}
			}
			auto trim(strcmp(nmProg, "boot") == 0);
			// определить название диска
			if(trim) nmProg = fdd->path;
			speccy->programName(nmProg, trim);
		}
	} 
	if(speccy->trapDos) {
		if(pc >= 0x3E01) {
			if(pc == 0x3E01 && cpu->_rm8(0x3E01) == 0x0D) {
				// no delays 15869,16032
				cpu->a = cpu->c = 1;
			} else if(pc == 0x3FEC && cpu->_rm8(0x3FED) == 0xA2 && ((states & 15) == S_READ || ((states >> 4) == S_READ))) {
				if(rq & R_DRQ) {
					cpu->_wm8(cpu->hl, dosDat);
					cpu->hl++, cpu->b--;
					rq &= ~R_DRQ; dosSts &= ~ST_DRQ;
				}
				while(rwlen) {
					// move others
					cpu->_wm8(cpu->hl, *rwptr++); rwlen--;
					cpu->hl++; cpu->b--;
				}
				// skip INI
				cpu->pc += 2;
				crc = csec->crcContent();
			} else if(pc == 0x3FD1 && cpu->_rm8(0x3FD2) == 0xA3 && (rq & R_DRQ) && (rwlen > 1) &&
				((states & 15) == S_WRITE || ((states >> 4) == S_WRITE))) {
				while(rwlen > 1) {
					*rwptr++ = cpu->_rm8(cpu->hl++);
					rwlen--; cpu->b--;
				}
				// skip OUTI
				cpu->pc += 2;
			}
		}
	}
}

