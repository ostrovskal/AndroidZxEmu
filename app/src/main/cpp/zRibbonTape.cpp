
#include "pch.h"
#include "zListTape.h"

static const char* hardInfo[] = {
	"compatible with - ", "uses - ", "compatible, but doesn\'t use -",
	" incompatible with - ", "-??"
};

static const char* hardTypes[] = {
		"Computers:", "External storage:", "ROM/RAM type add-ons:",
		"Sound devices:", "Joysticks:", "Mice:", "Other controllers:",
		"Serial ports:", "Parallel ports:", "Printers:", "Modems:",
		"Digitizers:", "Network adapters:", "Keyboards& keypads:",
		"AD/DA converters:", "EPROM programmers:", "Graphics:"
};
static const char* idsComputers[] = {
	"ZX Spectrum 16k", "ZX Spectrum 48k, Plus", "ZX Spectrum 48k ISSUE 1",
	"ZX Spectrum 128k + (Sinclair)", "ZX Spectrum 128k + 2 (grey case)",
	"ZX Spectrum 128k + 2A, +3", "Timex Sinclair TC - 2048", "Timex Sinclair TS - 2068",
	"Pentagon 128", "Sam Coupe", "Didaktik M", "Didaktik Gama", "ZX - 80", "ZX - 81",
	"ZX Spectrum 128k, Spanish version", "ZX Spectrum, Arabic version", "Microdigital TK 90 - X",
	"Microdigital TK 95", "Byte", "Elwro 800–3", "ZS Scorpion 256", "Amstrad CPC 464",
	"Amstrad CPC 664", "Amstrad CPC 6128", "Amstrad CPC 464 +", "Amstrad CPC 6128 +",
	"Jupiter ACE", "Enterprise", "Commodore 64", "Commodore 128", "Inves Spectrum +",
	"Profi", "GrandRomMax", "Kay 1024", "Ice Felix HC 91", "Ice Felix HC 2000",
	"Amaterske RADIO Mistrum", "Quorum 128", "MicroART ATM", "MicroART ATM Turbo 2",
	"Chrome", "ZX Badaloc", "TS - 1500", "Lambda", "TK - 65", "ZX - 97"
};
static const char* idsRomRam[] = {
		"Sam Ram", "Multiface ONE", "Multiface 128k", "Multiface + 3", "MultiPrint",
		"MB - 02 ROM / RAM expansion", "SoftROM", "1k", "16k", "48k", "Memory in 8–16k used"
};
static const char* idsJoys[] = { "Kempston", "Cursor, Protek, AGF", "Sinclair 2 Left(12345)", "Sinclair 1 Right(67890)", "Fuller" };
static const char* idsGraphics[] = { "WRX Hi - Res", "G007", "Memotech", "Lambda Colour" };
static const char* idsProgs[] = { "Orme Electronics" };
static const char* idsKeypads[] = { "Keypad for ZX Spectrum 128k" };
static const char* idsCnvs[] = { "Harley Systems ADC 8.2", "Blackboard Electronics" };
static const char* idsNetworks[] = { "ZX Interface 1" };
static const char* idsPrinters[] = { "ZX Printer, Alphacom 32 & compatibles", "Generic printer", "EPSON compatible" };
static const char* idsModems[] = { "Prism VTX 5000", "T/S 2050 or Westridge 2050" };
static const char* idsPports[] = {
		"Kempston S", "Kempston E", "ZX Spectrum + 3", "Tasman", "DK\'Tronics",
		"Hilderbay", "INES Printerface", "ZX LPrint Interface 3", "MultiPrint",
		"Opus Discovery", "Standard 8255 chip with ports 31, 63, 95"
};
static const char* idsSports[] = { "ZX Interface 1", "ZX Spectrum 128k" };
static const char* idsControllers[] = { "Trickstick", "ZX Light Gun", "Zebra Graphics Tablet", "Defender Light Gun" };
static const char* idsMices[] = { "AMX mouse", "Kempston mouse" };
static const char* idsDigitizers[] = { "RD Digital Tracer", "DK\'Tronics Light Pen", "British MicroGraph Pad", "Romantic Robot Videoface" };
static const char* idsSounds[] = {
		"Classic AY hardware(compatible with 128k ZXs)", "Fuller Box AY sound hardware",
		"Currah microSpeech", "SpecDrum", "AY ACB stereo(A + C = left, B + C = right); Melodik",
		"AY ABC stereo(A + B = left, B + C = right)", "RAM Music Machine",
		"Covox", "General Sound", "Intec Electronics Digital Interface B8001",
		"Zon - X AY", "QuickSilva AY", "Jupiter ACE"
};
static const char* idsStorages[] = {
		"ZX Microdrive", "Opus Discovery", "MGT Disciple", "MGT Plus - D",
		"Rotronics Wafadrive", "TR - DOS(BetaDisk)", "Byte Drive", "Watsford",
		"FIZ", "Radofin", "Didaktik disk drives", "BS - DOS(MB - 02)",
		"ZX Spectrum + 3 disk drive", "JLO(Oliger) disk interface",
		"Timex FDD3000", "Zebra disk drive", "Ramex Millennia", "Larken",
		"Kempston disk interface", "Sandy", "ZX Spectrum + 3e hard disk",
		"ZXATASP", "DivIDE", "ZXCF",
};

static const char** hardIds[] = {
	idsComputers, idsStorages, idsRomRam, idsSounds, idsJoys, idsMices,
	idsControllers, idsSports, idsPports, idsPrinters, idsModems,
	idsDigitizers, idsNetworks, idsKeypads, idsCnvs, idsProgs, idsGraphics
};

static int tzxWidths[] = {
	120, 120, 80, 80, 80, 80,
	0, 0, 120, 80, 0, 0, 0, 0, 0, 0,
	80, 150, 60, 80,
	80, 60, 80, 50,
	150, 0, 60, 80, 0, 0, 0, 0,
	150, 150, 230, 230, 0, 150, 120,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 50
};

BEGIN_MSG_MAP(zListTape, zWnd)
	ON_WM_PAINT()
	ON_WM_DRAWITEM()
	ON_WM_MOUSEWHEEL()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
	ON_WM_HSCROLL()
	ON_WM_SIZE()
	ON_WM_SETFONT()
END_MSG_MAP()

void zListTape::init() {
	items = &speccy->dev<zDevTape>()->blks;
	countItems = items->size();
	totalWidth = resolveSizeItems(0);
	zbmi = Z_BITMAPINFO{ { { sizeof(BITMAPINFOHEADER), 64, -32, 1, USHORT(32), BI_RGB, 0, 0, 0, 0, 0 }, {0, 0, 0, 0}, },
						{6, 7, 6, 7}, {0, 0, 64, 32}, DIB_RGB_COLORS, SRCCOPY };
	scrollToCurrent();
}

void zListTape::scrollToCurrent() {
	entryItem = speccy->dev<zDevTape>()->current;
	int pos(0);
	for(int i = 0; i < entryItem; i++) pos += itemWidth(i);
	pos = correctPosition(false, correctPosition(true, pos));
	changeScroll(pos, totalWidth, wndRect.width());
	invalidate(true);
}

void zListTape::changeScroll(int& offs, int count, int page) {
	SCROLLINFO si;
	memset(&si,		0, sizeof(SCROLLINFO));
	si.cbSize		= sizeof(SCROLLINFO);
	si.fMask		= SIF_ALL;
	if(GetScrollInfo(hWnd, SB_HORZ, &si)) {
		page--;
		int delta(count - page);
		bool visible = delta > 0;
		offs = (visible ? (offs > delta ? delta : offs) : 0);

		si.nPage = (visible ? page : 0);
		si.nMin = 0;
		si.nMax = (visible ? count : 0);
		si.nPos = (visible ? offs : 0);

		SetScrollInfo(hWnd, SB_HORZ, &si, true);
		invalidate();
	}
}

void zListTape::onScroll(UINT code, UINT p, HWND hWndScroll, bool vertical) {
	SCROLLINFO si;
	memset(&si, 0, sizeof(SCROLLINFO));
	si.cbSize	= sizeof(SCROLLINFO);
	si.fMask	= SIF_ALL;
	if(GetScrollInfo(hWnd, SB_HORZ, &si)) {
		int pos(si.nPos); p = si.nTrackPos;
		switch(code) {
			case SB_LINEDOWN:		pos += 10; break;
			case SB_LINEUP:			pos -= 10; break;
			case SB_PAGEDOWN:		pos += si.nPage; break;
			case SB_PAGEUP:			pos -= si.nPage; break;
			case SB_THUMBTRACK:		
			case SB_THUMBPOSITION:	pos = p; break;
		}
		pos				 = correctPosition(false, correctPosition(true, pos));
		if(pos			 != si.nPos) {
			si.nPos		 = pos;
			si.nTrackPos = pos;
			SetScrollPos(hWnd, SB_HORZ, pos, true);
			invalidate();
		}
	}
}

void zListTape::makeMemDC(int cx, int cy) {
	if(hbmMem) {
		DeleteObject(hbmMem);
		DeleteDC(hdcMem);
	}
	if(cx && cy) {
		auto hdc(GetDC(hWnd));
		hbmMem = CreateCompatibleBitmap(hdc, cx, cy);
		hdcMem = CreateCompatibleDC(hdc);
		ReleaseDC(hWnd, hdc);
	}
}

void zListTape::onSize(UINT type, int nWidth, int nHeight) { 
	if(nWidth > 0 && nHeight > 0) {
		wndRect.set(0, 0, nWidth, nHeight);
		makeMemDC(nWidth, nHeight);
	}
}

void zListTape::onPaint() {
	PAINTSTRUCT ps;
	auto hdc(BeginPaint(hWnd, &ps));
	DRAWITEMSTRUCT dis{ ODT_LISTBOX, IDI_LIST_TAPE, 0, ODA_DRAWENTIRE, ODS_DEFAULT, (HWND)NULL, hdcMem, { 0, 0, 0, 0 }, 0 };
	auto rc((zRect*)&dis.rcItem);
	auto entry(entryItem);
	*rc = wndRect; rc->offset(-deltaWidth, 0);
	rc->padding(1, 1);
	::SetBkMode(hdcMem, TRANSPARENT);
	auto hOld(SelectObject(hdcMem, hbmMem));
	while(rc->left < wndRect.right && entry < countItems) {
		dis.itemID		= entry;
		dis.itemData	= itemType(entry);
		dis.itemState	= (entry == selItem ? ODS_SELECTED : ODS_DEFAULT);
		rc->right		= rc->left + itemWidth(entry);
		SendMessage(hWnd, WM_DRAWITEM, IDI_LIST_TAPE, (LPARAM)&dis);
		rc->offset(rc->width(), 0);
		entry++;
	}
	BitBlt(hdc, 0, 0, wndRect.width(), wndRect.height(), hdcMem, 0, 0, SRCCOPY);
	SelectObject(hdcMem, hOld);
	ReleaseDC(hWnd, hdc);
	EndPaint(hWnd, &ps);
}

void zListTape::onMouse(UINT message, UINT vkKey) {
	SetFocus(hWnd);
	int idx(entryItem - 1), x(-deltaWidth);
	while(x <= pt.x) x += itemWidth(++idx);
	selItem = idx;
	if(message == WM_LBUTTONDOWN) {
	} else if(message == WM_LBUTTONDBLCLK) {
		// назначить этот блок активным
		speccy->dev<zDevTape>()->setCurrentBlock(idx);
	}
	invalidate();
}

void zListTape::onKey(UINT message, UINT nVirtKey, UINT nRepeat, UINT nFlags) {
	int code(-1);
	switch(nVirtKey) {
		case VK_LEFT:	code = SB_LINEUP; break;
		case VK_RIGHT:	code = SB_LINEDOWN; break;
		case VK_NEXT:	code = SB_PAGEDOWN; break;
		case VK_PRIOR:	code = SB_PAGEUP; break;
	}
	if(code >= 0) onScroll(code, 0, nullptr, false);
}

void zListTape::onSetFont(HFONT hfont, BOOL redraw) {
	hFont	 = hfont;
	if(redraw) invalidate();
}

int zListTape::onWheel(UINT vkKey, short delta) {
	onScroll(SB_THUMBTRACK, entryItem + (delta < WHEEL_DELTA ? 3 : -3), nullptr, false);
	return TRUE;
}

int zListTape::onEraseBackground(HDC hdc) {
	auto old(SelectObject(hdcMem, hbmMem));
	FillRect(hdcMem, &wndRect, (HBRUSH)(COLOR_WINDOWFRAME + 1));
	SelectObject(hdcMem, old);
	return 0;
}

int zListTape::itemWidth(int idx) const {
	return tzxWidths[items->get(idx).type - 16]; 
}

int zListTape::correctPosition(bool pass, int pos) {
	if(pos < 0) pos = 0;
	int current(0), width(1), idx(0);
	for(idx = 0 ; idx < countItems; idx++) {
		width = itemWidth(idx);
		if((current + width) > pos) break;
		current += width;
	}
	deltaWidth = ((pos - current) % width);
	if(pass) {
		current = -deltaWidth;
		for(int i = idx; i < countItems; i++) current += itemWidth(i);
		if(current < wndRect.width()) {
			current = wndRect.width() - current;
			pos -= current;
			if(pos < 0) pos = 0;
		}
	}
	entryItem = idx;
	return pos;
}

int zListTape::resolveSizeItems(int idx) const {
	int width(0);
	for(; idx < countItems; idx++) width += itemWidth(idx);
	return width;
}

void zListTape::onDrawItem(int id, DRAWITEMSTRUCT* dis) {
	auto hDC(dis->hDC);
	auto item(&items->get(dis->itemID));
	auto rc(&dis->rcItem);
	auto isSel((dis->itemState & ODS_SELECTED) != 0);
	zRect r(*rc);
	SetBkMode(hDC, TRANSPARENT);
	drawTzxName(hDC, &r, item->type - 16, item->use);
	r.left = r.right + 2; r.right = rc->right - 2;
	drawTzxIndex(hDC, &r, dis->itemID, isSel);
	r.top = r.bottom; r.bottom = rc->bottom;
	ssh_patch9(hDC, &r, settings->ioIcons + 49152, &zbmi);
	r.padding(2, 0);
	// отрисовка данных блока, в зависимости от его типа
	auto type(item->type);
	auto pause(item->pause / 1000.0f);
	switch(type) {
		case TZX_JUMP:
			drawTzxText(hDC, &r, 1, "index:", makeTzxValue((dis->itemID + 1) + (int16_t)item->count, RADIX_DEC));
			break;
		case TZX_LOOP_START:
		case TZX_CALL:
			drawTzxText(hDC, &r, 1, (item->type == TZX_JUMP ? "index:" : "count:"), makeTzxValue(item->count, RADIX_DEC));
			break;
		case TZX_PURE_DATA:
		case TZX_TURBO:
		case TZX_UNION: 
			drawTzxText(hDC, &r, 3, "bits:", makeTzxValue(item->nwb, RADIX_DEC));
		case TZX_RECORD:
		case TZX_CSW: 
		case TZX_PULSES: 
		case TZX_TONE: 
			if(type != TZX_PURE_DATA) drawTzxText(hDC, &r, 1, "pilot:", makeTzxValue(item->nwp, RADIX_DEC));
			drawTzxText(hDC, &r, 9, "pause:", makeTzxValue(0, RADIX_FLT, pause));
			break;
		case TZX_ARCHIVE: 
		case TZX_SELECT:
			drawArchSel(hDC, &r, item, dis->itemID + 1);
			break;
		case TZX_PAUSE: 
			drawTzxText(hDC, &r, 1, "delay:", (pause < 0.0009f ? "stop" : makeTzxValue(0, RADIX_FLT, pause)));
			break;
		case TZX_LEVEL: 
			drawTzxText(hDC, &r, 1, "level:", (item->level ? "high" : "low")); 
			break;
		case TZX_MESSAGE:  
			drawTzxText(hDC, &r, 1, "pause:", makeTzxValue(0, RADIX_FLT, pause * 1000.0f));
		case TZX_GROUP_START: 
		case TZX_TEXT: 
			drawTzxText(hDC, &r, -1, makeTzxText(item->data, item->size), ""); 
			break;
		case TZX_HARDWARE: 
			drawHardware(hDC, &r, item); 
			break;
		case TZX_CUSTOM: 
			drawCustom(hDC, &r, item); 
			break;
		case TZX_SAVE:
		case TZX_NORMAL: 
			drawNormal(hDC, &r, dis->itemID);
			break;
	}
}

void zListTape::resolveRectIndex(int idx, zRect* r, zRect* ret, bool lft) const {
	ret->left = r->left + (lft ? (!(idx & 1)) * 10 + 5 : 5); ret->right = r->right;
	ret->top = r->top + idx * 16; ret->bottom = ret->top + 16;
}

const char* zListTape::makeTzxValue(int value, int radix, float fvalue) const {
	return ssh_ntos((radix == RADIX_DEC ? &value : (int*)&fvalue), radix, false);
}

zString zListTape::makeTzxText(uint8_t* ptr, int size) const {
	zString text(ptr, size);
	text.trim(); return text.crop(50);
}

void zListTape::drawTzxText(HDC hDC, zRect* r, int idx, const char* name, const char* value) {
	zRect rt;
	SelectObject(hDC, settings->getFont(FONT_TAPE_DATA));
	if(idx == -1) {
		SetTextColor(hDC, 0xFFFFFF);
		rt = *r;
		DrawText(hDC, name, -1, &rt, DT_CALCRECT | DT_WORDBREAK);
		int cy((r->height() - rt.height()) / 2);
		int cx((r->width() - rt.width()) / 2);
		rt.offset(cx, cy);
		DrawText(hDC, name, -1, &rt, DT_CENTER | DT_VCENTER | DT_WORDBREAK);
	} else {
		resolveRectIndex(idx, r, &rt);
		SetTextColor(hDC, (idx & 1) ? 0x0000FF : 0x00FFFF);
		DrawText(hDC, name, -1, &rt, DT_SINGLELINE | DT_LEFT);
		SetTextColor(hDC, (idx & 1) ? 0x00FFFF : 0x0000FF);
		rt.offset(0, rt.height()); rt.left = (r->left + (idx & 1) * 10 + 5);
		DrawText(hDC, value, -1, &rt, DT_SINGLELINE | DT_LEFT);
	}
}

void zListTape::drawCaption(HDC hDC, zRect* r, const char* name) {
	SelectObject(hDC, settings->getFont(FONT_TAPE_CAPTION));
	SetTextColor(hDC, 0x00FFFF);
	zRect rt(*r); rt.top++; rt.bottom = rt.top + 20;
	fillSolidRect(hDC, &rt, RGB(127, 0, 0));
	DrawText(hDC, name, -1, &rt, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
}

void zListTape::drawNormal(HDC hDC, zRect* r, int index) {
	// 0      - флаг: заголовок(0), данные(?)
	// 1      - тип: бейсик(0), ч.массив(1), с.массив(2), код(3)
	// 2 - 11 - имя
	// 12     - длина данных
	// 14     - код(адрес), массивы(имя), бейсик(строка)
	// 16     - бейсик(длина)
	// 18     - CRC
	auto item(&items->get(index));
	auto data(item->bits);
	auto flag(*data++), type(*data++);
	auto pause(item->pause / 1000.0f);
	if(flag == 0) {
		// надпись в заголовке - заголовок
		drawCaption(hDC, r, "CAPTION");
		drawTzxText(hDC, r, 3, "name:", makeTzxText(data, 10));
		drawTzxText(hDC, r, 5, "size:", makeTzxValue(wordLE(data + 10), RADIX_DEC));
	} else {
		if(index > 0) {
			auto flag1(flag); int param(0); flag = 255;
			auto itemp(&items->get(index - 1)); data = itemp->bits;
			if(itemp->type == TZX_NORMAL) flag = *data, param = wordLE(data + 14);
			zString name;
			if(flag1 == 255 && flag == 0) {
				type = data[1];
				switch(type) {
					case 0: name = "BASIC"; break;
					case 1: name = "N.ARRAY"; break;
					case 2: name = "S.ARRAY"; break;
					case 3: name = ((param >= 16384 && param < 22528) ? "SCREEN" : "BYTES"); break;
				}
			} else {
				name = zString::fmt("BLOCK(%i)", flag1);
				type = 255;
			}
			drawCaption(hDC, r, name.str());
			switch(type) {
				case 0:
					drawTzxText(hDC, r, 3, "line:", makeTzxValue(param, RADIX_DEC));
					break;
				case 1: case 2:
					drawTzxText(hDC, r, 3, "name:", zString((uint8_t*)&param, 2));
					break;
				case 3:
					drawTzxText(hDC, r, 3, "address:", makeTzxValue(param, RADIX_DEC));
					break;
				default:
					drawTzxText(hDC, r, 3, "pilot:", makeTzxValue(item->nwp, RADIX_DEC));
					drawTzxText(hDC, r, 5, "bits:", makeTzxValue(item->nwb / 2, RADIX_DEC));
					break;
			}
		}
	}
	drawTzxText(hDC, r, 9, "pause:", makeTzxValue(0, RADIX_FLT, pause));
}

void zListTape::drawCustom(HDC hDC, zRect* r, zDevTape::BLK_TAPE* item) {
	drawTzxText(hDC, r, 1, "size:", makeTzxValue(item->size - 20, RADIX_DEC));
	drawTzxText(hDC, r, -1, makeTzxText(item->data, 16), nullptr);
}

void zListTape::drawHardware(HDC hDC, zRect* r, zDevTape::BLK_TAPE* item) {
	auto count(item->size / 3), idx(0U);
	auto dat(item->data);
	zRect rt;
	SelectObject(hDC, settings->getFont(FONT_TAPE_DATA));
	for(uint32_t i = 0 ; i < count; i++) {
		auto type(*dat++), id(*dat++), info(*dat++);
		auto ids(hardIds[type]);
		auto ttype(hardTypes[type]), tid(ids[id]), tinfo(info > 3 ? "-??" : hardInfo[info]);
		resolveRectIndex(idx++, r, &rt, false);
		SetTextColor(hDC, 0x0000FF);
		DrawText(hDC, ttype, -1, &rt, DT_SINGLELINE | DT_LEFT | DT_END_ELLIPSIS);
		resolveRectIndex(idx++, r, &rt);
		rt.left += 10;
		SetTextColor(hDC, 0x00FFFF);
		DrawText(hDC, zString(tinfo) + tid, -1, &rt, DT_SINGLELINE | DT_LEFT | DT_END_ELLIPSIS);
		rt.left -= 10;
	}
}

void zListTape::drawArchSel(HDC hDC, zRect* r, zDevTape::BLK_TAPE* item, int index) {
	auto buf(item->data); auto count(*buf++);
	bool isSelect(item->type == TZX_SELECT);
	int type, idx(isSelect), offs(isSelect ? 30 : 53);
	zRect rt;
	SelectObject(hDC, settings->getFont(FONT_TAPE_DATA));
	for(int i = 0 ; i < count; i++) {
		if(isSelect) type = *(int16_t*)buf, buf += 2; else type = *buf, buf++;
		int size(*buf++); auto text(makeTzxText(buf, size)); buf += size;
		zString tag;
		if(isSelect) tag = zString::fmt("%i -> ", type + index);
		else {
			switch(type) {
				case 0: tag = "Title:"; break;
				case 1: tag = "Publisher:"; break;
				case 2: tag = "Author:"; break;
				case 3: tag = "Year:"; break;
				case 4: tag = "Language:"; break;
				case 5: tag = "Type:"; break;
				case 6: tag = "Price:"; break;
				case 7: tag = "Protection:"; break;
				case 8: tag = "Origin:"; break;
				case 255:tag= "Comment:"; break;
				default: tag= "Info:"; break;
			}
		}
		SetTextColor(hDC, 0x0000FF);
		resolveRectIndex(idx++, r, &rt, false);
		DrawText(hDC, tag, tag.length(), &rt, DT_SINGLELINE | DT_LEFT | DT_END_ELLIPSIS);
		rt.left += offs;
		SetTextColor(hDC, 0x00FFFF);
		DrawText(hDC, text, text.length(), &rt, DT_SINGLELINE | DT_LEFT | DT_END_ELLIPSIS);
		rt.left -= offs;
	}
}

void zListTape::drawTzxIndex(HDC hDC, zRect* r, int index, bool sel) {
	r->bottom = r->top + 9;
	auto cur(speccy->dev<zDevTape>()->current == index);
	fillSolidRect(hDC, r, cur ? RGB(255, 0, 255) : (sel ? RGB(95, 0, 95) : RGB(0, 0, 0)));
	SelectObject(hDC, settings->getFont(FONT_TAPE_INDEX));
	SetTextColor(hDC, 0x808080);
	index++;
	DrawText(hDC, ssh_ntos(&index, RADIX_DEC, false), -1, r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

void zListTape::drawTzxName(HDC hDC, zRect* r, int type, bool use) {
	zString name(tzx_names[type]);
	zRect rct(*r); r->left -= 2; r->right = r->left + 17;
	FillRect(hDC, r, (HBRUSH)(COLOR_WINDOWFRAME + 1));
	r->left += 2; r->right -= 2;
	fillSolidRect(hDC, &rct, RGB(48, 48, 0));
	SelectObject(hDC, settings->getFont(FONT_TAPE_NAME));
	SetTextColor(hDC, use ? 0x6F6F6F : 0x00AFAF);

	DrawText(hDC, name.str(), -1, &rct, DT_CALCRECT | DT_SINGLELINE);
	int cy(rct.height()), y = (r->height() - name.length() * cy) / 2;
	rct.top = y; rct.bottom = rct.top + cy;
	rct.right = r->right;

	for(int i = 0 ; i < name.length(); i++) {
		char sym(name[i]);
		DrawText(hDC, &sym, 1, &rct, DT_CENTER | DT_SINGLELINE);
		rct.offset(0, rct.height());
	}
}
