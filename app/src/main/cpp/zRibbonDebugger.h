//
// Created by User on 22.07.2023.
//

#pragma once

constexpr int MODE_PC	= 0;
constexpr int MODE_SP	= 1;
constexpr int MODE_DT	= 2;

constexpr int SFLAG_PC	= 1;
constexpr int SFLAG_SP	= 2;
constexpr int SFLAG_DT	= 4;
constexpr int SSEL_PC	= 8;
constexpr int SSEL_SP	= 16;
constexpr int SSEL_DT	= 32;
constexpr int SFLAG_SEL = 64;

class zRibbonDebugger : public zViewRibbon {
    friend class zAdapterDbgList;
public:
    zRibbonDebugger(zStyle* _styles, int _id, bool _vert);
    // параметры прокрутки
    virtual int computeScrollOffset(bool _vert) const override { return entries[speccy->debugMode]; }
    virtual int computeScrollRange(bool _vert) const override { return 65535; }
    virtual int computeScrollExtent(bool _vert) const override { return items.size(); }
    void setMode(int m, zCpu* _cpu);
    void onInit(bool _theme) override;
    void update(int data, int flags);
    void initItem(zViewText* nv, int idx);
    int entries[3]{}, selItems[3]{};
    zString8 cmdAsm{};
protected:
    virtual i32 onTouchEvent() override;
    virtual void onLayout(crti &position, bool changed) override;
    u16 itemPC(zCpu* cpu, u16 entry, char* buf);
    u16 itemDT(zCpu* cpu, u16 entry, char* buf);
    u16 itemSP(zCpu* cpu, u16 entry, char* buf) const;
    bool correctSP();
    bool correctDT();
    virtual bool scrolling(int delta) override;
    zArray<int> items{};
    int countCols{0}, countData{0};
};
