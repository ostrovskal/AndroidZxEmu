//
// Created by User on 20.07.2023.
//

#include "sshCommon.h"
#include "zFormBrowser.h"
#include <filesystem>
#include <thread>

class zAdapterBrowser : public zAdapterList {
public:
    explicit zAdapterBrowser() : zAdapterList({}, new zFabricListItem(styles_browslisttext)) { }
    zView* getView(int position, zView* convert, zViewGroup* parent) override {
        auto nv(convert);
        if(!nv) nv = fabricBase->make(parent);
        nv->id = position; nv->updateStatus(ZS_TAP, false);
        auto t(getItem(position));
        if(t.indexOf("(") != -1) t = t.substrBeforeLast("(", t).substrBeforeLast(".") + "(" + t.substrAfterLast("(");
        ((zViewText*)nv)->setText(t);
        return nv;
    }
};

i32 zFormBrowser::updateVisible(bool set) {
    if(set) setFilter(filter, true);
    return zViewForm::updateVisible(set);
}

i32 zFormBrowser::touchEventDispatch(AInputEvent *event) {
    if(th) return TOUCH_CONTINUE;
    return zViewForm::touchEventDispatch(event);
}

void zFormBrowser::onInit(bool _theme) {
    zViewForm::onInit(_theme);
    if(isOpen) {
        static int ids[] = { R.id.browsOpen, R.id.browsFolder, R.id.browsSave, R.id.browsDel, R.id.browsEject, R.id.browsTrDos };
        for(int i = 0 ; i < 6; i++) {
            but[i] = idView<zViewButton>(ids[i]);
            but[i]->setOnClick([this](zView* v, int) { onCommand(v->id); });
        }
        edt = idView<zViewEdit>(R.id.browsEdit);
        lst = idView<zViewRibbon>(R.id.browsList);
        flt = idView<zViewSelect>(R.id.browsSpin); flt->setItemSelected(speccy->browseFlt);
        szip= idView<zViewCheck>(R.id.browsChk);
        idView<zViewCheck>(R.id.browsTape)->setOnClick([](zView* v, int l) { speccy->tapeStartStop = v->isChecked(); })->checked(speccy->tapeStartStop);
        flt->setOnChangeSelected([this](zView*, int s) { setFilter(s); });
        edt->setOnChangeText([this](zView*, int) { updateControls(); });
        lst->setAdapter(new zAdapterBrowser());
        lst->setOnClick([this](zView*, int s) {
            auto txt(lst->getAdapter()->getItem(s));
            auto dbl(touch->isDblClicked());
            if(txt == "...") {
                if(dbl) {
                    current = current.left(current.count() - 1).substrBeforeLast("/");
                    current.slash(); updateList();
                }
            } else if(txt.startsWith("[") && txt.endsWith("]")) {
                if(dbl) {
                    current += txt.substr(1, txt.count() - 2);
                    current.slash(); updateList();
                }
            } else {
                edt->setText(txt.substrBeforeLast("("));
                if(dbl) onCommand(R.id.browsOpen);
            }
        });
    }
}

void zFormBrowser::setFilter(int _f, bool force) {
    if(filter != _f || force) {
        diskPath.empty();
        auto old(filter);
        zString8 _flt; filter = _f;
        if(filter == FLT_NET) {
            if(old != FLT_NET) current.empty();
            root = "/ZX/ZxGames/";
            // определить список расширений
            _flt = "zip|sna|z80|tap|tzx|csw|wav|ezx|zzz|td0|trd|scl|fdi|udi|$b|$c";
        } else {
            if(old == FLT_NET) current.empty();
            root = settings->makePath("", FOLDER_ROOT) + "files/";
            // определить список расширений
            if(filter == FLT_SNAP) {
                _flt = "sna|z80|ezx";
            } else if(filter == FLT_TAP) {
                _flt = "tap|tzx|csw|wav";
            } else {
                // disk
                speccy->diskOperation(ZX_DISK_OPS_GET_READONLY, filter - FLT_DSK, diskPath);
                diskPath = z_trimName(diskPath, true);
                _flt = "td0|trd|scl|fdi|udi|$b|$c";
            }
        }
        // запросить список файлов/папок
        fltExt = _flt.split("|");
        idView<zViewText>(R.id.browsDisk)->setText(diskPath);
    }
    updateList();
    // обновить контролы
    updateControls();
    // запомнить фильтр
    speccy->browseFlt = filter;
}

void zFormBrowser::makeList(const zArray<zFile::zFileInfo>& arr) {
    edt->setText("");
    // отфильтровать: папки сначала, файлы по типу, zip - смотреть содержимое
    files.free(); zString8Array folders;
    zString8 pth, ext; int size;
    for(auto& f: arr) {
        pth = f.path; size = (int)f.usize;
        pth = pth.substrAfter(root + current);
        pth = pth.substrBeforeLast("/", pth);
        if(f.attr & S_IFDIR) {
            folders += pth;
        } else {
            ext = pth.substrAfterLast(".");
            if(f.zip) {
                // посмотреть содержимое
                zFile fl;
                if(fl.open(f.path, true)) {
                    zFile::zFileInfo zfi{};
                    if(fl.info(zfi)) {
                        ext = zfi.path.substrAfterLast(".");
                    } else continue;
                }
            }
            ext.lower();
            if(ext.indexOf(fltExt.get_data(), fltExt.size()) == -1) continue;
            files += pth + z_fmt8("(%s)", z_kilo(size).str());
        }
    }
    if(files.isNotEmpty()) files.sort(0, files.size() - 1, 0, true, [this](int l, int r) {
            auto s1(files[l].substrBeforeLast(".").lower());
            auto s2(files[r].substrBeforeLast(".").lower());
            return strcmp(s1, s2) <= 0;
    });
    if(folders.isNotEmpty()) folders.sort(0, folders.size() - 1, 0, true, [&folders](int l, int r) {
        return strcmp(folders[l], folders[r]) <= 0;
    });
    for(int i = 0 ; i < folders.size(); i++) files.insert(i, "[" + folders[i] + "]");
    if(current.isNotEmpty()) files.insert(0, "...");
    lst->getAdapter()->clear(false);
    lst->getAdapter()->addAll(files);
}

bool zFormBrowser::checkAuth() {
    bool ret(true);
    if(timeAuth < z_timeMillis()) {
        ret = dbx->auth("", "", "");
        if(ret) {
            auto tm(dbx->getRequest().json().getNode("time"));
            timeAuth = z_timeMillis() + tm->time() / 1000000;
        }
    }
    return ret;
}

void zFormBrowser::updateList() {
    if(filter != FLT_NET) {
        makeList(zFile::find(root + current, ""));
        return;
    }
    if(!dbx) { dbx = new zCloudMail("ostrov_skal", "JcnhjdRfnmrf2015"); timeAuth = 0; }
    if(th) return;
    th = new std::thread([this]() {
        theApp->getForm(FORM_WAIT)->updateStatus(ZS_VISIBLED, true);
        zArray<zFile::zFileInfo> fi;
        if(checkAuth()) {
            fi = dbx->getFiles(root + current);
        } else {
            zViewManager::showToast(theme->findString(R.string.netNotFound));
        }
        SAFE_DELETE(th); theApp->getForm(FORM_WAIT)->updateStatus(ZS_VISIBLED, false);
        makeList(fi);
    });
    th->detach();
}

void zFormBrowser::updateControls() {
    static int openIcons[] = { R.integer.iconZxOpen, R.integer.iconZxOpen, R.integer.iconZxWeb, R.integer.iconZxInsert,
                           R.integer.iconZxInsert, R.integer.iconZxInsert, R.integer.iconZxInsert };
    auto txt(edt->getText()), _pth(root + current + txt);
    auto isDir(std::filesystem::is_directory(_pth.str())), isFile(txt.indexOf(".") != -1);
    auto isExist(std::filesystem::exists(_pth.str())), isNet(filter == FLT_NET);
    edt->updateStatus(ZS_DISABLED, isNet);
    // 1. открыть
    but[BUT_OPEN]->setIcon(openIcons[filter]);
    but[BUT_OPEN]->updateStatus(ZS_DISABLED, txt.isEmpty() || isDir || !isExist && !isNet);
    // 2. создать папку
    but[BUT_FOLDER]->updateStatus(ZS_DISABLED, txt.isEmpty() || isFile || isExist || isNet);
    // 3.
    but[BUT_EJECT]->updateStatus(ZS_DISABLED, filter <= FLT_NET || diskPath == "Empty");
    // 4. удалить
    but[BUT_DEL]->updateStatus(ZS_DISABLED, txt.isEmpty() || !isExist || isNet);
    // 5. сохранить
    auto isSave(isNet || isExist || z_extension(txt) == -1);
    but[BUT_SAVE]->updateStatus(ZS_DISABLED, isSave);
    szip->updateStatus(ZS_DISABLED, isSave);
    // 6. вызвать DOS
    but[BUT_TRDOS]->updateStatus(ZS_DISABLED, filter <= FLT_NET);
    requestLayout();
}


void zFormBrowser::onCommand(int id) {
    auto path(root + current + edt->getText());
    switch(id) {
        case R.id.browsOpen:
            switch(filter) {
                case FLT_SNAP:
                case FLT_TAP:
                    frame->send(ZX_MESSAGE_LOAD, 0, 0, path);
                    close(z.R.id.ok); break;
                case FLT_NET:
                    if(th) break;
                    th = new std::thread([this, path]() {
                        theApp->getForm(FORM_WAIT)->updateStatus(ZS_VISIBLED, true);
                        if(checkAuth()) {
                            if(dbx->download(path, settings->makePath("", FOLDER_FILES))) {
                                zViewManager::showToast(theme->findString(R.string.download) + path.substrAfterLast("/"));
                            }
                        }
                        SAFE_DELETE(th);
                        theApp->getForm(FORM_WAIT)->updateStatus(ZS_VISIBLED, false);
                    });
                    th->detach(); break;
                default:
                    zxCmd(ZX_CMD_DISK_OPS, ZX_DISK_OPS_INSERT, filter - FLT_DSK, path);
                    setFilter(filter, true); break;
            }
            break;
        case R.id.browsFolder:
            std::filesystem::create_directory(path.str());
            updateList();
            break;
        case R.id.browsSave:
            frame->send(ZX_MESSAGE_SAVE, szip->isChecked() << 7, 0, path);
            close(z.R.id.ok);
           break;
        case R.id.browsDel:
            std::filesystem::remove_all(path.str());
            updateList();
            break;
        case R.id.browsEject:
            zxCmd(ZX_CMD_DISK_OPS, ZX_DISK_OPS_EJECT, filter - FLT_DSK);
            setFilter(filter, true);
            break;
        case R.id.browsTrDos:
            frame->send(ZX_MESSAGE_DISK, ZX_DISK_OPS_TRDOS);
            close(z.R.id.ok);
            break;
    }
}