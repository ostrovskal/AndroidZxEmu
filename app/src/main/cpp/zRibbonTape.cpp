
#include "sshCommon.h"
#include "zRibbonTape.h"

static cstr hardInfo[] = { "compatible with-", "uses-", "compatible, but doesn\'t use-",
                           " incompatible with-", "-??" };

static cstr hardTypes[]= { "Computers:", "External storage:", "ROM/RAM type add-ons:",
                           "Sound devices:", "Joysticks:", "Mice:", "Other controllers:",
                           "Serial ports:", "Parallel ports:", "Printers:", "Modems:",
                           "Digitizers:", "Network adapters:", "Keyboards& keypads:",
                           "AD/DA converters:", "EPROM programmers:", "Graphics:" };
static cstr idsComputers[] 	= { "ZX Spectrum 16k", "ZX Spectrum 48k, Plus", "ZX Spectrum 48k ISSUE 1",
                                   "ZX Spectrum 128k+(Sinclair)", "ZX Spectrum 128k+2 (grey case)",
                                   "ZX Spectrum 128k+2A, +3", "Timex Sinclair TC-2048", "Timex Sinclair TS-2068",
                                   "Pentagon 128", "Sam Coupe", "Didaktik M", "Didaktik Gama", "ZX-80", "ZX-81",
                                   "ZX Spectrum 128k, Spanish version", "ZX Spectrum, Arabic version", "Microdigital TK 90-X",
                                   "Microdigital TK 95", "Byte", "Elwro 800–3", "ZS Scorpion 256", "Amstrad CPC 464",
                                   "Amstrad CPC 664", "Amstrad CPC 6128", "Amstrad CPC 464+", "Amstrad CPC 6128+",
                                   "Jupiter ACE", "Enterprise", "Commodore 64", "Commodore 128", "Inves Spectrum+",
                                   "Profi", "GrandRomMax", "Kay 1024", "Ice Felix HC 91", "Ice Felix HC 2000",
                                   "Amaterske RADIO Mistrum", "Quorum 128", "MicroART ATM", "MicroART ATM Turbo 2",
                                   "Chrome", "ZX Badaloc", "TS-1500", "Lambda", "TK-65", "ZX-97" };
static cstr idsRomRam[]     = { "Sam Ram", "Multiface ONE", "Multiface 128k", "Multiface+3", "MultiPrint",
                                "MB-02 ROM/RAM expansion", "SoftROM", "1k", "16k", "48k", "Memory in 8–16k used" };
static cstr idsJoys[]       = { "Kempston", "Cursor, Protek, AGF", "Sinclair 2 Left(12345)", "Sinclair 1 Right(67890)", "Fuller" };
static cstr idsGraphics[]   = { "WRX Hi-Res", "G007", "Memotech", "Lambda Colour" };
static cstr idsProgs[]      = { "Orme Electronics" };
static cstr idsKeypads[]    = { "Keypad for ZX Spectrum 128k" };
static cstr idsCnvs[]       = { "Harley Systems ADC 8.2", "Blackboard Electronics" };
static cstr idsNetworks[]   = { "ZX Interface 1" };
static cstr idsPrinters[]   = { "ZX Printer, Alphacom 32 & compatibles", "Generic printer", "EPSON compatible" };
static cstr idsModems[]     = { "Prism VTX 5000", "T/S 2050 or Westridge 2050" };
static cstr idsPports[]     = { "Kempston S", "Kempston E", "ZX Spectrum + 3", "Tasman", "DK\'Tronics",
                                "Hilderbay", "INES Printerface", "ZX LPrint Interface 3", "MultiPrint",
                                "Opus Discovery", "Standard 8255 chip with ports 31, 63, 95" };
static cstr idsSports[]     = { "ZX Interface 1", "ZX Spectrum 128k" };
static cstr idsControllers[]= { "Trickstick", "ZX Light Gun", "Zebra Graphics Tablet", "Defender Light Gun" };
static cstr idsMices[]      = { "AMX mouse", "Kempston mouse" };
static cstr idsDigitizers[] = { "RD Digital Tracer", "DK\'Tronics Light Pen", "British MicroGraph Pad", "Romantic Robot Videoface" };
static cstr idsSounds[]     = { "Classic AY hardware(compatible with 128k ZXs)", "Fuller Box AY sound hardware",
                                "Currah microSpeech", "SpecDrum", "AY ACB stereo(A+C=left, B+C=right); Melodik",
                                "AY ABC stereo(A+B=left, B+C=right)", "RAM Music Machine",
                                "Covox", "General Sound", "Intec Electronics Digital Interface B8001",
                                "Zon-X AY", "QuickSilva AY", "Jupiter ACE" };
static cstr idsStorages[]   = { "ZX Microdrive", "Opus Discovery", "MGT Disciple", "MGT Plus-D",
                                "Rotronics Wafadrive", "TR-DOS(BetaDisk)", "Byte Drive", "Watsford",
                                "FIZ", "Radofin", "Didaktik disk drives", "BS-DOS(MB-02)",
                                "ZX Spectrum+3 disk drive", "JLO(Oliger) disk interface",
                                "Timex FDD3000", "Zebra disk drive", "Ramex Millennia", "Larken",
                                "Kempston disk interface", "Sandy", "ZX Spectrum+3e hard disk",
                                "ZXATASP", "DivIDE", "ZXCF" };
static cstr* hardIds[]      = { idsComputers, idsStorages, idsRomRam, idsSounds, idsJoys, idsMices, idsControllers, idsSports,
                                idsPports, idsPrinters, idsModems, idsDigitizers, idsNetworks, idsKeypads, idsCnvs, idsProgs, idsGraphics };

static int tzxWidths[] = {
	130, 130, 100, 100, 100, 100,
	0, 0, 130, 100, 0, 0, 0, 0, 0, 0,
	100, 160, 80, 100,
	100, 80, 100, 70,
	160, 0, 80, 100, 0, 0, 0, 0,
	160, 160, 220, 220, 0, 160, 130,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 70
};

class zFabricTapeItem : public zBaseFabric {
public:
	explicit zFabricTapeItem() : zBaseFabric(styles_z_list_item) { }
	zView* make(zViewGroup* parent) override {
		// создать представление со всеми возможными элементами
		auto ll(new zLinearLayout(styles_tapeitem, 0, true));
        // index
        ll->attach(new zViewText(styles_tapeindex, R.id.tzxIndex, 0), VIEW_MATCH, VIEW_WRAP);
        // inner layout
        auto cnt((zLinearLayout*)ll->attach(new zLinearLayout(styles_default, 0, false), VIEW_MATCH, VIEW_MATCH));
        // tzx type
        cnt->attach(new zViewText(styles_tapetype, R.id.tzxType, R.string.failedLoad), 18_dp, VIEW_MATCH);
        // content
        auto vl((zLinearLayout*)cnt->attach(new zLinearLayout(styles_default, 0, true), VIEW_MATCH, VIEW_MATCH));
        // caption
        vl->attach(new zViewText(styles_tapecaption, R.id.tzxCaption, 0), VIEW_MATCH, VIEW_WRAP);
        // scroll
        auto scl((zScrollLayout*)vl->attach(new zScrollLayout(styles_default, R.id.tzxScroll, true), VIEW_MATCH, VIEW_MATCH));
        scl->setGravity(ZS_GRAVITY_CENTER);
        // items
        scl->attach(new zLinearLayout(styles_default, R.id.tzxSclItems, true), VIEW_MATCH, VIEW_MATCH);
        // pause
        ll->attach(new zViewText(styles_tapetext, R.id.tzxPause, 0), VIEW_MATCH, VIEW_WRAP);
        ll->attach(new zViewButton(styles_tapebut, R.id.tzxDelete, R.string.tzxDelete), VIEW_MATCH, VIEW_WRAP);
        // params
		ll->lps.set(0, 0, VIEW_WRAP, VIEW_MATCH);
		ll->onInit(false);
		return ll;
	}
};

class zAdapterTape : public zAdapterList {
public:
	explicit zAdapterTape(zRibbonTape* _tape) : zAdapterList({}, new zFabricTapeItem()), tape(_tape) { }
	[[nodiscard]] int getCount() const override { return speccy ? speccy->tapeCount : 0; }
	zView* getView(int position, zView* convert, zViewGroup* parent) override {
		auto nv((zLinearLayout*)createView(position, convert, parent, fabricBase, false));
		// в зависимости от типа блока построить разное представление
		return tape->makeBlock(nv, position);
	}
    zRibbonTape* tape{nullptr};
};

void zRibbonTape::onInit(bool _theme) {
	zViewRibbon::onInit(_theme);
	setAdapter(new zAdapterTape(this));
}

zLinearLayout* zRibbonTape::makeBlock(zLinearLayout* lyt, int position) {
    auto tape(speccy->dev<zDevTape>());
    auto blk(tape->blockInfo(position)); auto cur(speccy->tapeCurrent == position);
    auto type(blk->type); auto pause((float)blk->pause / 1000.0f);
    // назначить этот блок активным
    lyt->setOnClick([position, tape, this](zView*, int) {
        if(touch->isDblClicked()) { tape->setCurrentBlock(position); requestPosition(); }
    });
    // удаление блока
    lyt->idView<zViewButton>(R.id.tzxDelete)->setOnClick([position, tape, this](zView*, int) {
        tape->removeBlock(position); requestPosition();
        DLOG("remove block %i", position);
    });
    // установить ширину блока
    lyt->lps.set(0, 0, z_dp(tzxWidths[type - 16]), VIEW_MATCH);
    // макет содержимого
    content = lyt->idView<zLinearLayout>(R.id.tzxSclItems);
    content->removeAllViews(false);
    // заголовок
    caption = lyt->idView<zViewText>(R.id.tzxCaption);
    caption->updateStatus(ZS_VISIBLED, false);
    // пауза
    auto vPause(lyt->idView<zViewText>(R.id.tzxPause));
    vPause->setText("");
    // индекс блока
    lyt->idView<zViewText>(R.id.tzxIndex)->setText(tzxValue(++position))->drw[DRW_BK]->color = (cur ? z.R.color.blue : z.R.color.black);
    // тип tzx блока
    lyt->idView<zViewText>(R.id.tzxType)->setText(tzx_names[blk->type - 16])->setTextColorForeground(blk->use ? R.color.tzxTypeUse : R.color.tzxTypeUnuse);
    bool resetScroll(true), showPause(false);
    switch(type) {
        case TZX_JUMP: case TZX_LOOP_START: case TZX_CALL:
            tzxView(type == TZX_JUMP ? R.string.tzxIndex : R.string.tzxCount, tzxValue((type == TZX_JUMP ? position : 0) + (i16)blk->count));
            break;
        case TZX_PURE_DATA: case TZX_TURBO: case TZX_UNION:
            tzxView(R.string.tzxSize, tzxValue(blk->nwb / 8));
        case TZX_RECORD: case TZX_CSW: case TZX_PULSES: case TZX_TONE:
            if(type != TZX_PURE_DATA) tzxView(R.string.tzxPilot, tzxValue(blk->nwp));
            break;
        case TZX_LEVEL:
            tzxView(R.string.tzxLevel, (blk->level ? "high" : "low"));
            break;
        case TZX_MESSAGE: case TZX_PAUSE:
            showPause = true;
            break;
        case TZX_GROUP_START: case TZX_TEXT:
            tzxView(tzxText(blk->data, blk->size), "");
            break;
        case TZX_CUSTOM:
            tzxView(R.string.tzxSize, tzxValue(blk->size - 20));
            tzxView(tzxText(blk->data, 16), "");
            break;
        case TZX_SAVE: case TZX_NORMAL:
            tzxNormal(blk, position > 1 ? tape->blockInfo(position - 2) : nullptr);
            showPause = true;
            break;
        case TZX_ARCHIVE: case TZX_SELECT: case TZX_HARDWARE:
            resetScroll = false;
            type == TZX_HARDWARE ? tzxHardware(blk) : tzxArchSel(blk, position);
            break;
    }
    if(resetScroll) content->scroll.empty();
    vPause->setText(pause < 0.0009f ?   (showPause ? theme->findString(R.string.tzxStop) : zString8("")) :
                                        tzxValue(0, pause * (999.0f * (float)(type == TZX_MESSAGE) + 1.0f)));
    return lyt;
}

zViewText* zRibbonTape::tzxView(czs& name, czs& value) {
    auto v((zViewText*)content->attach(new zViewText(styles_tapetext, 0, 0), VIEW_MATCH, VIEW_WRAP));
    v->setText(name);
    if(value.isNotEmpty()) {
        v->setTextColorForeground(z.R.color.red);
        v->setGravity(ZS_GRAVITY_START);
        v = (zViewText*)content->attach(new zViewText(styles_tapetext, 0, 0), VIEW_MATCH, VIEW_WRAP);
        v->setText(value)->setTextColorForeground(z.R.color.yellow);
    } else {
        v->setTextColorForeground(z.R.color.white);
        v->setGravity(ZS_GRAVITY_HCENTER);
    }
    return v;
}

void zRibbonTape::tzxArchSel(zDevTape::BLK_TAPE* blk, int position) {
    auto buf(blk->data); auto count(*buf++); bool isSelect(blk->type == TZX_SELECT);
    u32 type; zString8 tag;
    for(auto i = 0 ; i < count; i++) {
        if(isSelect) type = *(i16*)buf, buf += 2; else type = *buf, buf++;
        int size(*buf++); auto text(tzxText(buf, size)); buf += size;
        if(isSelect) tag = z_fmt8("%i -> ", type + position);
        else {
            auto tags(theme->findArray(R.array.tzxArchive));
            if(type < 9) tag = tags[(int)type];
            else if(type == 255) tag = tags[9];
            else tag = tags[10];
        }
        tzxView(tag, text);
    }
}

void zRibbonTape::tzxHardware(zDevTape::BLK_TAPE* blk) {
    auto count(blk->size / 3); auto dat(blk->data);
    for(auto i = 0 ; i < count; i++) {
        auto type(*dat++), id(*dat++), info(*dat++);
        auto ids(hardIds[type]); auto ttype(hardTypes[type]), tid(ids[id]), tinfo(info > 3 ? "-??" : hardInfo[info]);
        tzxView(ttype, zString8(tinfo) + tid);
    }
}

void zRibbonTape::tzxNormal(zDevTape::BLK_TAPE* blk, zDevTape::BLK_TAPE* prev) {
    // 0      - флаг: заголовок(0), данные(?)
    // 1      - тип: бейсик(0), ч.массив(1), с.массив(2), код(3)
    // 2 - 11 - имя
    // 12     - длина данных
    // 14     - код(адрес), массивы(имя), бейсик(строка)
    // 16     - бейсик(длина)
    // 18     - CRC
    auto data(blk->bits); auto flag((i32)*data++); int type;
    if(flag == 0) {
        // надпись в заголовке - заголовок
        caption->setText("CAPTION")->updateStatus(ZS_VISIBLED, true);
        tzxView(tzxText(data + 1, 10), "")->setTextSize(17_dp);
    } else {
        if(prev) {
            auto flag1(flag); u32 param(0), size; flag = 255; data = prev->bits;
            if(prev->type == TZX_NORMAL) flag = *data, size = *(u16*)(data + 12), param = *(u16*)(data + 14);
            zString8 name;
            if(flag1 == 255 && flag == 0) {
                type = data[1];
                switch(type) {
                    case 0:
                        name = "BASIC";
                        tzxView(R.string.tzxLine, tzxValue(param));
                        break;
                    case 1: case 2:
                        name = type == 2 ? "S.ARRAY" : "N.ARRAY";
                        tzxView(R.string.tzxName, tzxText((u8*)&param, 2));
                        break;
                    case 3:
                        name = ((param >= 16384 && param < 22528) ? "SCREEN" : "BYTES");
                        tzxView(R.string.tzxAddress, tzxValue(param));
                        break;
                }
            } else {
                name = z_fmt8("BLOCK(%i)", flag1);
                size = blk->nwb / 16;
                tzxView(R.string.tzxPilot, tzxValue(blk->nwp));
            }
            caption->setText(name)->updateStatus(ZS_VISIBLED, true);
            tzxView(R.string.tzxSize, tzxValue(size));
        }
    }
}
