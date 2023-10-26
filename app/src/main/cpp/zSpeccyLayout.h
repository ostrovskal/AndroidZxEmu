//
// Created by User on 20.07.2023.
//

#pragma once

#include "emu/zSpeccy.h"

class zSpeccyLayout : public zViewSurface {
    friend class zSpeccy;
public:
    zSpeccyLayout(zStyle* _styles, int _id, cszi& sz) : zViewSurface(_styles, _id, sz) { }
    virtual ~zSpeccyLayout();
    // начальная инициализация
    bool                init();
    // обработка хэндлера
    void                processHandler();
    // отправить сообщение
    void                send(int what, int a1 = 0, int a2 = 0, cstr s = nullptr);
    void                stateTools(int action, int id = 0);
    void                onCommand(int id, zMenuItem* mi);
    void                activateDebugger();
    void                setStatus(int img) { _status->setImage(img); }
    void                setCaption();
    void                setParamControllers();
protected:
    // уведомление о событии
    virtual void        notifyEvent(HANDLER_MESSAGE* msg) override;
    virtual void        updateTexture() override;
    // цикл сообщений
    zHandler            handler{};
    zViewController*    ac{nullptr};
    zViewController*    cc{nullptr};
    zViewText*          capt{nullptr};
    zViewText*          fps{nullptr};
    zMenuGroup*         menu{nullptr};
    zViewFormMessage*   fm{nullptr};
    zViewImage*         _status{nullptr};
    zViewButton*        tapeTurbo{nullptr};
    zViewButton*        tapePlay{nullptr};
    zViewProgress*      tapeProgress{nullptr};
    zLinearLayout*      tapeLyt{nullptr};
};

inline zSpeccyLayout*   frame(nullptr);
inline zSpeccy*         speccy(nullptr);
