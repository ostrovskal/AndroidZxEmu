#pragma once

class zListTape: public zViewRibbon {
public:
	zListTape(zStyle* _styles, int _id, bool _vert) : zViewRibbon(_styles, _id, _vert) { }
	void scrollToCurrent();
	// параметры прокрутки
	virtual int computeScrollOffset(bool _vert) const override { return entryItem; }
	virtual int computeScrollRange(bool _vert) const override { return countItems; }
	virtual int computeScrollExtent(bool _vert) const override { return items.size(); }
	virtual void onInit(bool _theme) override;
protected:
	void resolveRectIndex(int idx, zRect* r, zRect* ret, bool lft = true) const;
	const char* makeTzxValue(int value, int radix, float fvalue = 0.0f) const;
	zString makeTzxText(uint8_t* ptr, int size) const;
	void drawCaption(HDC hDC, zRect* r, const char* name);
	void drawTzxText(HDC hDC, zRect* r, int idx, const char* name, const char* value);
	void drawArchSel(HDC hDC, zRect* r, zDevTape::BLK_TAPE* item, int index);
	void drawHardware(HDC hDC, zRect* r, zDevTape::BLK_TAPE* item);
	void drawCustom(HDC hDC, zRect* r, zDevTape::BLK_TAPE* item);
	void drawNormal(HDC hDC, zRect* r, int index);
	void drawTzxIndex(HDC hDC, zRect* r, int index, bool sel);
	void drawTzxName(HDC hDC, zRect* r, int type, bool use);
	int itemType(int idx) const { return items->get(idx).type; }
	int itemWidth(int idx) const;
	int correctPosition(bool pass, int pos);
	int resolveSizeItems(int idx) const;
	void changeScroll(int& offs, int count, int page);
	z_msg void onSize(UINT type, int nWidth, int nHeight);
	z_msg void onPaint();
	z_msg void onDrawItem(int id, DRAWITEMSTRUCT* dis);
	z_msg void onScroll(UINT code, UINT p, HWND hWndScroll, bool vertical);
	int countItems{0};
	int selItem{0}, entryItem{0};
	int totalWidth{0}, deltaWidth{0};
	zArray<zDevTape::BLK_TAPE>* items{nullptr};
};
