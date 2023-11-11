#pragma once

class zRibbonTape: public zViewRibbon {
public:
	zRibbonTape(zStyle* _styles, int _id, bool _vert) : zViewRibbon(_styles, _id, _vert) { }
	virtual void onInit(bool _theme) override;
	virtual zString8 typeName() const override { return "zRibbonTape"; }
	zLinearLayout* makeBlock(zLinearLayout* lyt, int position);
protected:
	zLinearLayout* content{nullptr};
	zViewText* caption{nullptr};
	// TZX значение
	zString8 tzxValue(u32 ival, float fval = -1.0f) { return z_ntos((fval < 0 ? (int*)&ival : (int*)&fval), fval < 0 ? RADIX_DEC : RADIX_FLT, false); }
	// TZX текст
	zString8 tzxText(u8* p, u32 s) const { zString8 text(p, s); text.trim(); return text/*.crop(32)*/; }
	// сформировать новое текстовое представление
    zViewText* tzxView(int ids, czs& value) { return tzxView(theme->findString(ids), value); }
    zViewText* tzxView(czs& name, czs& value);
	void tzxArchSel(zDevTape::BLK_TAPE* blk, int position);
	void tzxHardware(zDevTape::BLK_TAPE* blk);
	void tzxNormal(zDevTape::BLK_TAPE* blk, zDevTape::BLK_TAPE* prev);
	void tzxCaption(czs& name);
};
