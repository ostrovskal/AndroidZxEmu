
#include "../sshCommon.h"
#include "zDevs.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
//                                      МАГНИТОФОН                                             //
/////////////////////////////////////////////////////////////////////////////////////////////////

inline bool isPulses(int tp) { return (tp == TZX_PULSES || tp == TZX_CSW || tp == TZX_RECORD); }

zDevTape::zDevTape() {
	memset(pulses, 0, sizeof(pulses));
	cpu = speccy->dev<zCpuMain>();
	clear();
}

void zDevTape::clear() {
	for(auto i = 0 ; i < blks.size(); i++) SAFE_A_DELETE(blks[i].data);
	blks.clear(); pth.empty();
	speccy->recTape = false; current = npulses = 0;
	speccy->tapeAllIndex = 0;
	iwaves = waves = ipilot = 0;
	stopPlay();
}

void zDevTape::update(int param) {
	if(param == ZX_UPDATE_RESET) {
		current = 0;
		for(int i = 0 ; i < blks.size(); i++) blks[i].use = 0;
		speccy->tapeIndex = 0;
		if(speccy->resetTape) clear(); else stopPlay();
	}
	zDevBeeper::update(param);
}

void zDevTape::startPlay() {
	stopPlay();
	if(current < blks.size()) {
		edge = cpu->ts;
		speccy->playTape = true;
	}
}

void zDevTape::startRecord() {
	stopRecord();
	speccy->recTape = true;
}

void zDevTape::stopRecord() {
	rpos = 0; npc = -1; speccy->recTape = false;
}

void zDevTape::stopPlay() {
	speccy->playTape = false;
	edge = 0x7FFFFFFFFFFFFFFFLL;
	iblock = 0; bit = 0x40; ctape = 0;
	stopRecord();
}

bool zDevTape::write(u16, u8 val, u32 ticks) {
	if(speccy->recTape) {
		auto pc(cpu->pcb); auto cur(cpu->ts + ticks);
		if(npc != -1 && abs(npc - pc) > 80) {
			if(rpos > 16) addNormalBlock(tmpBuf, rpos / 16).type = TZX_SAVE;
			npc = -1; speccy->recTape = false;
		} else if(npc == -1) {
			ctape = 0;
			npc = pc;
		} else {
			zDevBeeper::_write(val, ticks, false);
			auto div(speccy->turboTick(79, true));
			auto w((u32)((cur - edge) * div));
			if(ctape == 0) {
				if(w == 23 || w == 21 || w == 11 || w == 10) {
					bit = 1 << (7 - ((rpos % 16) / 2));
					val = tmpBuf[rpos >> 4];
					if(w < 20) val &= ~bit; else val |= bit;
					tmpBuf[rpos++ >> 4] = val;
				}
			} else {
				if(w == 29 || w == 27) ctape++;
				else if(w == 8 || w == 9 || w == 10) ctape = 0, rpos = 0;
			}
		}
		edge = cur;
	}
	return false;
}

bool zDevTape::read(u16, u8* ret, u32 ticks) {
	if(speccy->playTape || speccy->recTape) {
		auto pc(cpu->pcb);
		if(npc == -1 && pc != 662) { npc = pc; ctape = 0; } else {
			if(abs(npc - pc) > 256) {
				auto ptr(cpu->ptr(pc - 2)); auto p(dwordLE(&ptr));
				if(p != 0xFEDB7F3E) {
					ctape++;
					if(ctape > 300) stopPlay();
					return false;
				} else npc = pc;
			}
			ctape = 0;
		}
		if(speccy->playTape) {
			auto cur(cpu->ts + ticks);
			while(cur > edge) edge += getImpulse();
			_write(bit >> 3, ticks, false);
			*ret |= bit;
		}
	}
	return false;
}

zDevTape::BLK_TAPE& zDevTape::addNormalBlock(u8* data, u32 size) {
	auto& blk(addBlock(TZX_NORMAL, data, size, 18));
	auto nwp((u16)((*data < 128) ? 8064 : 3224));
	blk.nip = 2; blk.nib = 2;
	blk.nwp = nwp * blk.nip; blk.pause = 1000;
	blk.nwb = (u32)((size << 3) * blk.nib);
	auto buf = blk.data;
	*buf++ = 0; *buf++ = indexOfPulse(2168); *buf++ = indexOfPulse(0);
	*buf++ = 0; *buf++ = indexOfPulse(667);  *buf++ = indexOfPulse(735);
	blk.twb = buf;
	*buf++ = 0; *buf++ = indexOfPulse(855);  *buf++ = indexOfPulse(855);
	*buf++ = 0; *buf++ = indexOfPulse(1710); *buf++ = indexOfPulse(1710);
	blk.pilots = buf; setWaveRLE(&buf, 0, (u16)(nwp - 1)); setWaveRLE(&buf, 1, 1);
	blk.bits = buf;
	return blk;
}

zDevTape::BLK_TAPE& zDevTape::addBlock(u8 type, u8* data, u32 size, u32 add_size) {
	BLK_TAPE blk;
	memset(&blk, 0, sizeof(BLK_TAPE));
	size += add_size;
	if(data && size) {
		blk.data = new u8[size];
		memcpy(blk.data + add_size, data, size - add_size);
	}
	blk.type = type;
	blk.size = size;
	return blks += blk;
}

bool zDevTape::nextBlock() {
	if(iblock > 0) return true;
	bool stop(false), exit(false);
	while(current < blks.size() && !exit) {
		auto& blk(blks[current]);
		auto type(blk.type);
		blk.use = 1;
		switch(type) {
			case TZX_PAUSE:
				if(blk.pause > 0) return true;
				stop = true;
				current++;
				break;
			case TZX_STOP_48K:
				if(speccy->is48k()) {
					stop = exit = true;
					speccy->indexBlkUI = current;
					modifySTATE(ZX_T_UI, 0);
				} else current++;
				break;
			case TZX_LEVEL:
				bit = (u8)((!blk.level) << 6);
				break;
			case TZX_JUMP:
				current += (int)blk.count;
				break;
			case TZX_LOOP_START:
				blk.index = blk.count;
				loop = current++;
				break;
			case TZX_LOOP_END:
				blk = blks[loop];
				blk.index--;
				current = (u16)(blk.index > 0 ? loop + 1 : current + 1);
				break;
			case TZX_CALL:
				blk.index = 0;
				call = current;
				current += (int)*(u16*)blk.data;
				break;
			case TZX_RETURN:
				blk = blks[call];
				blk.index++;
				current = (u16)(call + (blk.index < blk.count ? (int)*(u16*)(blk.data + blk.index * 2) : 1));
				break;
			case TZX_MESSAGE: case TZX_SELECT:
				modifySTATE(ZX_T_UI, 0);
				speccy->indexBlkUI = current;
				exit = (type == TZX_SELECT);
				if(!stop) stop = exit;
				current++;
				break;
			case TZX_GROUP_START: case TZX_GROUP_END:
			case TZX_TEXT: case TZX_ARCHIVE:
			case TZX_CUSTOM: case TZX_Z: case TZX_HARDWARE:
				current++;
				break;
			default:
				exit = true;
				// если пульсы - не сбрасывать
				if(!isPulses(type)) iwaves = ipilot = 0;
				waves = 0;
				tw = blk.twp;
				ni = blk.nip;
				break;
		}
	}
	if(!stop) stop = current >= blks.size();
	if(stop) stopPlay();
	return !stop;
}

u32 zDevTape::getImpulse() {
	BLK_TAPE* blk(nullptr);
	u32 impulse(0), signal(0);
	frame->setStatus(R.integer.iconZxCasette);
	while(!impulse) {
		if(!nextBlock()) return 0;
		blk = &blks[current];
		auto ifile(speccy->tapeIndex);
		// необработанные импульсы
		if(isPulses(blk->type)) {
			if(!iwaves) ipilot = ifile;
			if(ifile < (blk->nwp + ipilot)) {
				wave = blk->pilots[ifile++ - ipilot];
				if(!wave) wave = blk->pilots[ifile++ - ipilot], impulse = pulses[wave]; else impulse = wave * blk->rate;
				speccy->tapeIndex = ifile;
				iwaves = iblock = 1;
			} else {
				current++;
				iblock = iwaves = 0;
				impulse = 3500U * blk->pause;
			}
			if(!impulse) continue;
			break;
		}
		auto pilot = blk->nwp;
		if(!waves) {
			if(iblock < pilot) {
				wave = blk->pilots[ipilot++];
				waves = *(u16*)(blk->pilots + ipilot) * blk->nip;
				ipilot += 2;
			} else {
				if(iblock < (pilot + blk->nwb)) {
					if(iblock == pilot) ni = blk->nib, waves = blk->nwb, tw = blk->twb;
				} else {
					current++;
					iblock = 0;
					impulse = 3500U * blk->pause;
				}
			}
			iwaves = 0;
		}
		if(waves > 0) {
			if(iblock >= pilot) {
				// биты
				auto ni8 = ni << 3;
				auto bt = blk->bits[iwaves / ni8];
				auto nb = 1 << (7 - ((iwaves % ni8) / ni));
				wave = (u8)((bt & nb) != 0);
			}
			auto t = tw + wave * (1 + ni);
			auto d = (iwaves % ni); if(!d) signal = (*t) & 3U;
			impulse = pulses[*(t + 1 + d)];
			iblock++; waves--; iwaves++;
			speccy->tapeIndex = ifile + 1;
		}
	}
	switch(signal) {
		// edge
		case 0: bit ^= 0x40; break;
		// low
		case 2: bit = 0x00; break;
		// high
		case 3: bit = 0x40; break;
	}
	impulse = (u32)(impulse * (speccy->machine->cpuClock / 3500000.0f));
	return speccy->turboDiv(impulse);
}

void zDevTape::setCurrentBlock(int index) {
	if(index >= blks.size()) index = blks.size() - 1;
	if(index < 0) index = 0;
	current = (u16)index;
	speccy->tapeIndex = resolveCountImpulses(index);
//	for(int i = 0 ; i < index; i++) blks[i].use = 1;
//	for(; index < blks.size(); index++) blks[index].use = 0;
	iwaves = waves = 0;
	stopPlay();
}

u8* zDevTape::statePrivate(u8* ptr, bool restore) {
	if(restore) {
		if(strncmp((char*)ptr, "SERG TAPE", 9)) return nullptr;
		ptr += 9;
		bit = *ptr++; wave = *ptr++; ni = *ptr++; speccy->playTape = *ptr++;
		current = wordLE(&ptr); call = wordLE(&ptr); loop = wordLE(&ptr); npulses = wordLE(&ptr);
		iwaves = dwordLE(&ptr); waves = dwordLE(&ptr); rpos = dwordLE(&ptr);
		iblock = dwordLE(&ptr); ipilot = dwordLE(&ptr);
		speccy->tapeIndex = dwordLE(&ptr); edge = qwordLE(&ptr);
		memcpy(pulses, ptr, sizeof(pulses)); ptr += sizeof(pulses);
		pth = (cstr)ptr; ptr += pth.size() + 1;
	} else {
		z_memcpy(&ptr, "SERG TAPE", 9);
		*ptr++ = bit; *ptr++ = wave; *ptr++ = ni; *ptr++ = (u8)speccy->playTape;
		wordLE(&ptr, current); wordLE(&ptr, call); wordLE(&ptr, loop); wordLE(&ptr, npulses);
		dwordLE(&ptr, iwaves); dwordLE(&ptr, waves); dwordLE(&ptr, rpos);
		dwordLE(&ptr, iblock); dwordLE(&ptr, ipilot); dwordLE(&ptr, speccy->tapeIndex);
		qwordLE(&ptr, edge);
		z_memcpy(&ptr, pulses, sizeof(pulses));
		z_memcpy(&ptr, pth.buffer(), pth.size() + 1);
	}
	return ptr;
}

int zDevTape::setWaveRLE(u8** ptr, u8 wave, u16 rep) {
	auto p = *ptr;
	*p++ = wave; wordLE(&p, rep); *ptr = p;
	return rep;
}

u8 zDevTape::indexOfPulse(u16 pulse) {
	auto n = npulses;
	for(auto i = 0; i < n; i++) {
		if(pulses[i] == pulse) return (u8)i;
	}
	if(n < 255) {
		pulses[npulses++] = pulse;
	} else {
		// ошибка - массив импульсов переполнен
	}
	return (u8)n;
}

void zDevTape::trap(bool load) {
	if(load) {
		if(current == 0) {
			startPlay();
			speccy->programName(pth, true);
		}
	} else {
		// быстрая запись
		if(speccy->speedTape) {
			addNormalBlock(tmpBuf, (int)(cpu->speedSave() - tmpBuf)).type = TZX_SAVE;
		} else {
			startRecord();
		}
	}
}

i32 zDevTape::resolveCountImpulses(int count) {
	if(blks.size() < count) count = (u16)blks.size();
	u32 c(0);
	for(int i = 0 ; i < count; i++) {
		auto& blk(blks[i]);
		switch(blk.type) {
			case TZX_NORMAL: case TZX_TURBO: case TZX_UNION: case TZX_PURE_DATA:
				c += blk.nwb;
			case TZX_CSW: case TZX_PULSES: case TZX_TONE: case TZX_RECORD:
				c += blk.nwp;
				break;
		}
	}
	return (int)c;
}

void zDevTape::execUI(int index) {
	current = (u16)index;
	startPlay();
}

u8* zDevTape::state(u8* buf, bool restore) {
	auto ptr(buf);
	BLK_TAPE* blk(nullptr);
	if(restore) {
		clear();
		if(!(ptr = statePrivate(buf, restore))) return nullptr;
		auto cblks(wordLE(&ptr)), _tw(wordLE(&ptr));
        auto ext(z_extension(pth));
		for(int i = 0 ; i < cblks; i++) {
			auto twb(wordLE(&ptr));
			auto pilots(dwordLE(&ptr)), bits(dwordLE(&ptr)), size(dwordLE(&ptr));
			auto nwp(dwordLE(&ptr)), nwb(dwordLE(&ptr));
			auto type(*ptr++), nip(*ptr++), nib(*ptr++), use(*ptr++);
			auto pause(wordLE(&ptr));
			if(ext == ZX_FMT_CSW || ext == ZX_FMT_WAV) {
				if(!speccy->load(pth, 0)) return nullptr;
				blk = &blks[blks.size() - 1];
				size -= blk->size;
			} else {
				blk = &addBlock(type, ptr, size, 0);
			}
			blk->nip = nip; blk->nib = nib; blk->use = use; blk->pause = pause;
			blk->nwp = nwp; blk->nwb = nwb; blk->twb = blk->data + twb;
			blk->pilots = blk->data + pilots; blk->bits = blk->data + bits;
			ptr += size;
		}
		if(z_crc(buf, ptr - buf) != wordLE(&ptr)) return nullptr;
		if(current < cblks) tw = _tw + blks[current].data;
		speccy->tapeAllIndex = resolveCountImpulses(cblks);
	} else {
		ptr = statePrivate(ptr, restore);
		auto count(blks.size());
		wordLE(&ptr, (u16)count); wordLE(&ptr, (u16)(tw - (current < count ? blks[current].data : tw)));
        auto ext(z_extension(pth));
		for(int i = 0 ; i < count; i++) {
			blk = &blks[i];
			wordLE(&ptr, (u16)(blk->twb - blk->data));
			dwordLE(&ptr, (int)(blk->pilots - blk->data)); dwordLE(&ptr, (int)(blk->bits - blk->data));
			dwordLE(&ptr, blk->size); dwordLE(&ptr, blk->nwp); dwordLE(&ptr, blk->nwb);
			*ptr++ = blk->type; *ptr++ = blk->nip; *ptr++ = blk->nib; *ptr++ = blk->use;
			wordLE(&ptr, blk->pause);
			if(ext != ZX_FMT_CSW && ext != ZX_FMT_WAV) z_memcpy(&ptr, blk->data, blk->size);
		}
		wordLE(&ptr, z_crc(buf, ptr - buf));
	}
	return ptr;
}

bool zDevTape::open(u8* ptr, size_t size, int type) {
    clear(); bool ret(false);
    switch(type) {
        case ZX_FMT_CSW: ret = openCSW(ptr, size); break;
        case ZX_FMT_WAV: ret = openWAV(ptr, size); break;
        case ZX_FMT_TAP: ret = openTAP(ptr, size); break;
        case ZX_FMT_TZX: ret = openTZX(ptr, size); break;
    }
    speccy->tapeIndex = 0;
    if(ret) speccy->tapeAllIndex = resolveCountImpulses(65535); else clear();
    return ret;
}

u8* zDevTape::save(int type) {
    switch(type) {
        case ZX_FMT_CSW: return saveCSW();
        case ZX_FMT_WAV: return saveWAV();
        case ZX_FMT_TAP: return saveTAP();
        case ZX_FMT_TZX: return saveTZX();
    }
    return nullptr;
}

void zDevTape::writePulse(u32 count, int what, int plen, int volume) {
    auto const lsize(512 * 1024);
    auto wptr(tmpBuf);
    if(plen == 0) {
        // force flush
        if(wcur) wfile.write(wptr, wcur);
        return;
    }
    auto len(count * plen);
    if((wcur + len) >= lsize) {
        // flush
        wfile.write(wptr, wcur);
        wcur = 0;
    }
    while(len >= lsize) {
        z_memset2(&wptr, (u16)what, lsize / 2);
        wptr = tmpBuf;
        len -= lsize;
    }
    auto ptr(&wptr[wcur]);
    if(plen == 1) {
        z_memset(&ptr, (u8)what, count);
    } else {
        z_memset2(&ptr, (u16)(what ? volume : -volume), count);
    }
    wcur = (u32)(ptr - wptr);
}

u16 zDevTape::indexIfSaveBlock() {
    for(u16 i = 0 ; i < blks.size(); i++) {
        if(blks[i].type == TZX_SAVE) return i;
    }
    return 0;
}

bool zDevTape::openCSW(u8* pth, size_t) {
    zFile fcsw;
    if(!fcsw.open((const char*)pth, true))
        return false;
    // загрузить заголовок
    auto ptr(&tmpBuf[512 * 1024]);
    if(!fcsw.read(nullptr, ptr, 32)) return false;
    auto head((HEAD_CSW*)ptr);
    if(strncmp(head->signature, "Compressed Square Wave", 22) != 0) return false;
    if(head->rle != 1) return false;
    auto rate(3500000U / head->freq); int sz;
    if(!(head->what & 1)) indexOfPulse(1);
    // цикл по кускам файла
    while(fcsw.read(&sz, ptr, 256 * 1024, -1, 0)) {
        auto buf(ptr), ptre(buf + sz), data(ptr);
        u32 total(0);
        while(buf < ptre) {
            auto idx(*buf++); auto pulse((u32)idx); pulse *= rate;
            if(pulse == 0) {
                pulse = (u32)(*(u32*)buf / rate); buf += 4;
                data[total++] = 0; idx = indexOfPulse((u16)pulse);
            }
            data[total++] = idx;
        }
        auto& blk(addBlock(TZX_CSW, data, total, 0));
        blk.pilots = blk.data; blk.rate = rate; blk.nwp = total; blk.pause = 0;
    }
    return true;
}

bool zDevTape::openWAV(u8* pth, size_t size) {
    zFile fwav;
    if(!fwav.open((cstr)pth, true)) return false;
    // загрузить заголовок
    auto ptr(&tmpBuf[512 * 1024]);
    if(!fwav.read(nullptr, ptr, sizeof(HEAD_WAV))) return false;
    if(strncmp((char*)ptr, "RIFF", 4) != 0) return false;
    auto head((HEAD_WAV*)ptr);
    auto chann(head->nChannels - 1);
    auto freq(head->nSamplesPerSec);
    auto bits((u8)((head->wBitsPerSample - 8) >> 3));
    auto len(head->nDataSize);
    if(head->wFormatTag != 16 && chann > 1 && freq != 44100 && freq != 22050 && bits > 1)
        return false;
    auto const rate(3500000 / freq);
    u32 nbuffer(512 * 1024);
    auto data(new u8[nbuffer]);
    u8 bit(0), bit1(0xff);
    u32 total(0), rle(1); int sz;
    // цикл по кускам файла
    while(fwav.read(&sz, ptr, 512 * 1024, -1, 0)) {
        auto buf(ptr), bufe(ptr + sz);
        u16 ch1, ch2, val;
        while(buf < bufe) {
            if(bits) {
                ch1 = wordLE(&buf);
                if(!chann) ch2 = 0; else ch2 = wordLE(&buf) >> 1, ch1 >>= 1;
                val = (u16)(ch1 + ch2); bit = (u8)(val < 32768);
            } else {
                ch1 = *buf++;
                if(!chann) ch2 = 0; else ch2 = (*buf++) >> 1, ch1 >>= 1;
                val = (u8)(ch1 + ch2); bit = (u8)(val > 127);
            }
            if(bit1 == 0xff) bit1 = bit;
            if(bit1 != bit) {
                if(rle > 255) { data[total++] = 0; rle = indexOfPulse((u16)rle); }
                data[total++] = (u8)rle;
                rle = 0; bit1 = bit;
            }
            rle++;
            if(total + 2 > nbuffer) {
                // увеличить буфер
                nbuffer = total + 512 * 1024;
                auto tmp(new u8[nbuffer]);
                memcpy(tmp, data, total);
                delete[] data; data = tmp;
            }
        }
    }
    // last
    if(rle > 255) { auto p(rle * rate); data[total++] = 0; rle = indexOfPulse((u16)(p)); }
    data[total++] = (u8)rle;
    auto& blk(addBlock(TZX_RECORD, data, total, 0));
    blk.pilots = blk.data; blk.pause = 0; blk.rate = rate; blk.nwp = total;
    delete[] data;
    return true;
}

u8* zDevTape::saveCSW() {
    // заголовок
    if(!wfile.open(settings->makePath("temp.tmp", FOLDER_CACHE), false))
        return nullptr;
    wcur = 32; HEAD_CSW head; auto length(0U);
    memcpy(&head.signature, "Compressed Square Wave", 22);
    head.version_minor = 0x01; head.version_major = 0x1A; head.freq = 45454;//44100;
    head.rle = 1; head.what = 1U; head.unk = 1;
    memcpy(tmpBuf, &head, sizeof(HEAD_CSW));
    // запомнить исходные данные
    statePrivate(&tmpBuf[512 * 1024], false);
    ipilot = iwaves = iblock = waves = 0;
    bit = 0x40; speccy->playTape = true;
    current = indexIfSaveBlock();
    speccy->tapeIndex = resolveCountImpulses(current);
    auto rate = 3500000U / head.freq;
    while(speccy->playTape) {
        auto pulse(speccy->turboTick(getImpulse(), true));
        auto w(pulse / rate);// + ((pulse % 79U) != 0);
        if(w > 255) {
            writePulse(1, 0, 1);
            w = pulses[pulse] * rate;
            writePulse(1, ((w >> 0x00) & 0xFFFF), 2);
            writePulse(1, ((w >> 0x10) & 0xFFFF), 2);
            length += 5;
        } else writePulse(1, w, 1);
        length++;
    }
    writePulse(0, 0, 0);
    // восстановить исходные данные
    statePrivate(&tmpBuf[512 * 1024], true);
    wfile.close();
    return (u8*)1;
}

u8* zDevTape::saveWAV() {
    // заголовок
    if(!wfile.open(settings->makePath("temp.tmp", FOLDER_CACHE), false))
        return nullptr;
    HEAD_WAV wav{ 'FFIR', 0, 'EVAW', ' tmf', 16, 1, 1, 44100U, 88200U, 2, 16, 'atad', 0 };
    wcur = sizeof(wav); auto length(0U);
    memcpy(tmpBuf, &wav, wcur);
    // запомнить исходные данные
    statePrivate(&tmpBuf[512 * 1024], false);
    ipilot = iwaves = iblock = waves = 0;
    bit = 0x40; speccy->playTape = true;
    current = indexIfSaveBlock();
    speccy->tapeIndex = resolveCountImpulses(current);
    while(speccy->playTape) {
        auto pulse(speccy->turboTick(getImpulse(), true) / 79);
        writePulse(pulse, bit, 2);
        length += pulse * 2;
    }
    writePulse(1, 1, 0);
    // восстановить исходные данные
    statePrivate(&tmpBuf[512 * 1024], true);
    // записать длину файла
    wfile.seek(40, SEEK_SET);
    wfile.write(&length, 4);
    length = wfile.length() - 8;
    wfile.seek(4, SEEK_SET);
    wfile.write(&length, 4);
    wfile.close();
    return (u8*)1;
}

bool zDevTape::openTAP(u8* ptr, size_t size) {
    auto buf(ptr);
    while(buf < ptr + size) {
        auto sz(wordLE(&buf));
        if(!sz) break;
        addNormalBlock(buf, sz);
        buf += sz;
    }
    return (buf == ptr + size);
}

bool zDevTape::openTZX(u8* ptr, size_t dsize) {
    if(strncmp((char*)ptr, "ZXTape!\x1a", 8) != 0) return false;
    auto buf(ptr + 10), bufe(ptr + dsize);
    int i, j;
    u16 tmp0; u32 size; u8* dat;
    zDevTape::BLK_TAPE* blk;
    // 0 импульс в 0
    indexOfPulse(0);
    // цикл по блокам
    while(buf < bufe) {
        auto tzx(*buf++);
        switch(tzx) {
            case TZX_NORMAL:
                size = *(u16*)(buf + 2);
                addNormalBlock(buf + 4, size).pause = *(u16*)(buf);
                buf += size + 4LL;
                break;
            case TZX_TURBO:
                size = *(u32*)(buf + 15) & 0xFFFFFF;
                blk = &addBlock(tzx, buf + 18, size, 18);
                blk->nip = 2; blk->nib = 2;
                tmp0     = *(u16*)(buf + 10);
                blk->nwp = (tmp0 + 1U) * blk->nip;
                blk->nwb = (((size - 1) << 3) + buf[12]) * blk->nib;
                dat = blk->data;
                dat[0] = 0; dat[1] = indexOfPulse(wordLE(&buf)); dat[2] = indexOfPulse(0);
                dat[3] = 0; dat[4] = indexOfPulse(wordLE(&buf)); dat[5] = indexOfPulse(wordLE(&buf));
                blk->twb = dat += 6;
                dat[0] = 0; dat[1] = indexOfPulse(wordLE(&buf)); dat[2] = dat[1];
                dat[3] = 0; dat[4] = indexOfPulse(wordLE(&buf)); dat[5] = dat[4];
                blk->pilots = dat += 6; setWaveRLE(&dat, 0, tmp0); setWaveRLE(&dat, 1, 1);
                blk->bits = dat; blk->pause = *(u16*)(buf + 3);
                buf += size + 8LL;
                break;
            case TZX_CSW: {
                size = dwordLE(&buf);
                auto pause(wordLE(&buf));
                auto rate(dwordLE(&buf) & 0xFFFFFF);
                //auto rle    = buf[9];
                auto npulse(dwordLE(&buf));
                auto csw(buf); dat = csw;
                auto _csw(csw), cswe(csw + (size - 14));
                u32 count(0);
                while(csw < cswe) {
                    auto pulse = (u32)*csw++;
                    auto idx = (u8)pulse;
                    pulse *= rate;
                    if(pulse == 0) {
                        pulse = (u32)(*(u32*)csw / rate); csw += 4;
                        *_csw++ = 0; idx = indexOfPulse((u16)pulse);
                        count++;
                    }
                    *_csw++ = idx;
                    count++;
                }
                if(count != npulse) {
                    //DLOG("TZX_TZX count:%i pulse:%i", count, npulse);
                    npulse = count;
                }
                blk = &addBlock(tzx, dat, npulse, 0);
                blk->pilots = blk->data;
                blk->rate = rate;
                blk->nwp = size;
                blk->pause = pause;
                buf += size + 4LL;
                break;
            }
            case TZX_RECORD: {
                auto rate(*(u16*)(buf));                // Количество Т-состояний на выборку (бит данных)
                size = *(u32*)(buf + 5) & 0xFFFFFF;    // Длина данных образцов
                auto pause(*(u16*)(buf + 2));
                auto last(buf[4]); buf += 8;
                auto len(((size - 1) << 3) + last);
                auto data(new u8[len]);
                auto v1((*buf & 128) != 0);
                u32 rle(1), nn(0);
                for(u32 i = 1; i < len; i++) {
                    auto bt = *(buf + (i >> 3));
                    auto v = (bt & (1 << (7 - (i % 8)))) != 0;
                    if(v != v1) {
                        if(rle > 255) { auto p = rle * rate; data[nn++] = 0; rle = indexOfPulse((u16)(p)); }
                        data[nn++] = (u8)rle; v1 = v;
                        //if(rle < 3) { nn--; }
                        rle = 0;
                    }
                    rle++;
                }
                blk = &addBlock(tzx, data, nn, 0);
                blk->pilots = blk->data;
                blk->pause = pause;
                blk->rate = rate;
                blk->nwp = nn;
                buf += size;
                delete[] data;
                break;
            }
            case TZX_UNION: {
                size = dwordLE(&buf);
                auto ps(wordLE(&buf));
                auto np(dwordLE(&buf)), nb(dwordLE(&buf));
                auto cwp(buf[11]), cwb(buf[17]);
                auto nip(buf[10]), nib(buf[16]);
                dat = buf;
                auto alph(buf + 4);
                if(np) {
                    for(i = 0; i < cwp; i++) {
                        *dat++ = *alph++;
                        for(j = 0 ; j < nip; j++) { *dat++ = indexOfPulse(wordLE(&alph)); }
                    }
                }
                auto pilots(dat - buf);
                u32 nwp(0);
                for(u32 i = 0 ; i < np; i++) {
                    auto w(*alph++);
                    nwp += setWaveRLE(&dat, w, wordLE(&alph));
                }
                auto twb(dat - buf);
                if(nb) {
                    for(i = 0; i < cwb; i++) {
                        *dat++ = *alph++;
                        for(j = 0 ; j < nib; j++) { *dat++ = indexOfPulse(wordLE(&alph)); }
                    }
                }
                auto sz((nip + 1) * cwp + (nib + 1) * cwb + np * 3U);
                blk = &addBlock(tzx, alph, (int)(size - (alph - (buf + 4))), sz);
                memcpy(blk->data, buf, dat - buf);
                blk->nip = nip; blk->nib = nib;
                blk->nwp = nwp * nip; blk->nwb = nb * nib;
                blk->pilots = blk->data + pilots;
                blk->twb = blk->data + twb;
                blk->bits = blk->data + (dat - buf);
                blk->pause = ps;
                buf += size + 4LL;
                break;
            }
            case TZX_TONE:
                blk = &addBlock(tzx, buf, 5, 0);
                blk->nwp = *(u16*)(buf + 2); blk->nwb = 0;
                blk->nip = 1; blk->nib = 0;
                dat = blk->data; *dat++ = 0; *dat++ = indexOfPulse(*(u16*)buf);
                blk->twb = dat; blk->pilots = dat; setWaveRLE(&dat, 0, (u16)blk->nwp);
                blk->bits = dat; buf += 4;
                break;
            case TZX_PULSES:
                dat = buf;
                tmp0 = *buf++; dat = buf;
                for(i = 0 ; i < tmp0; i++) {
                    auto pulse(*(u16*)buf);
                    *buf++ = 0; *buf++ = indexOfPulse(pulse);
                }
                tmp0 <<= 1;
                blk = &addBlock(tzx, dat, tmp0, 0);
                blk->pilots = dat;
                blk->nwp = tmp0;
                break;
            case TZX_PURE_DATA:
                size = *(u32*)(buf + 7) & 0xFFFFFF;
                blk = &addBlock(tzx, buf + 10, size, 6);
                blk->nip = 0; blk->nib = 2;
                blk->nwp = 0; blk->nwb = (((size - 1) << 3) + buf[4]) * blk->nib;
                dat = blk->data; blk->twb = dat; blk->pilots = nullptr;
                dat[0] = 0; dat[1] = indexOfPulse(*(u16*)(buf + 0)); dat[2] = dat[1];
                dat[3] = 0; dat[4] = indexOfPulse(*(u16*)(buf + 2)); dat[5] = dat[4];
                blk->bits = dat + 6;
                blk->pause = *(u16*)(buf + 5);
                buf += size + 10LL;
                break;
            case TZX_PAUSE:
                // пауза
                addBlock(tzx, nullptr, 0, 0).pause = wordLE(&buf);
                break;
            case TZX_GROUP_START:
                tmp0 = *buf++;      // длина имени
                addBlock(tzx, buf, tmp0, 0);
                buf += tmp0;
                break;
            case TZX_GROUP_END:
                addBlock(tzx, nullptr, 0, 0);
                break;
            case TZX_JUMP:
                // смещение к блоку
                addBlock(tzx, nullptr, 0, 0).count = wordLE(&buf);
                break;
            case TZX_LOOP_START:
                // количество повторений
                addBlock(tzx, nullptr, 0, 0).count = wordLE(&buf);
                break;
            case TZX_LOOP_END:
                addBlock(tzx, nullptr, 0, 0);
                break;
            case TZX_CALL:
				// количество вызовов
                tmp0 = wordLE(&buf);
                addBlock(tzx, buf, tmp0 * 2U, 0).count = tmp0;
                buf += tmp0 * 2LL;
                break;
            case TZX_RETURN:
                addBlock(tzx, nullptr, 0, 0);
                break;
            case TZX_SELECT:
                size = wordLE(&buf);
                addBlock(tzx, buf, size, 0);
                buf += size;
                break;
            case TZX_LEVEL:
            case TZX_STOP_48K:
                size = dwordLE(&buf);
                blk = &addBlock(tzx, buf, size, 0);
                if(tzx == TZX_LEVEL) blk->level = buf[0];
                buf += size;
                break;
            case TZX_TEXT:
				// длина текста
                tmp0 = *buf++;
                addBlock(tzx, buf, tmp0, 0);
                buf += tmp0;
                break;
            case TZX_MESSAGE:
                tmp0 = *buf++; // время отображения сообщения
                size = *buf++; // длина сообщения
                addBlock(tzx, buf, size, 0).pause = (u16)(tmp0 == 0 ? 5 : tmp0 + 1);
                buf += size;
                break;
            case TZX_ARCHIVE:
                size = wordLE(&buf); // длина всего блока
                addBlock(tzx, buf, size, 0);
                buf += size;
                break;
            case TZX_HARDWARE:
                tmp0 = (u16)(*buf++ * 3);      // количество структур, описывающих оборудование
                addBlock(tzx, buf, tmp0, 0);
                buf += tmp0;
                break;
            case TZX_CUSTOM:
                size = *(u32*)(buf + 16) + 20; // длина всего блока
                addBlock(tzx, buf, size, 0);
                buf += size;
                break;
            case TZX_Z:
                addBlock(tzx, buf, 9, 0);
                buf += 9;
                break;
            default:
                ILOG("Unknown block <%X> TZX!", buf[-1]);
                buf += dsize;
                break;
        }
    }
    return (buf == bufe);
}

u8* zDevTape::saveTAP() {
	if(blks.isEmpty()) return nullptr;
    auto ptr(tmpBuf); auto idx(indexIfSaveBlock());
    while(idx < blks.size()) {
        auto& blk(blks[idx++]);
        if(blk.type == TZX_NORMAL || blk.type == TZX_SAVE) {
            auto size(blk.size - (blk.bits - blk.data));
            wordLE(&ptr, (u16)size); z_memcpy(&ptr, blk.bits, size);
        }
    }
    wordLE(&ptr, 0);
    return ptr;
}

u8* zDevTape::saveTZX() {
	if(blks.isEmpty()) return nullptr;
    auto ptr(&tmpBuf[0]); u32 tmp;
    auto i(indexIfSaveBlock());
    z_memcpy(&ptr, "ZXTape!\x1a\x01\x14", 10);
    for(; i < blks.size(); i++) {
        auto blk(&blks[i]);
        auto type(blk->type);
        auto data(blk->data);
        auto size(blk->size);
        *ptr++ = type & 63;
        auto bpos(0);
        switch(type) {
            case TZX_RECORD: {
                wordLE(&ptr, (u16)blk->rate);
                wordLE(&ptr, blk->pause);
                auto plast = ptr++;
                auto psize = ptr; ptr += 3;
                auto rec = true;
                auto count = (u8)blk->index;
                for(int n = 0; n < count; n++) {
                    auto pulse = blk->twp[data[n * 3] * 2 + 1];
                    auto rep = (u32)(*(u16*)(data + i * 3LL + 1) * pulse);
                    while(rep-- > 0) {
                        auto bit = 1 << (7 - (bpos % 8));
                        auto ppos = bpos / 8;
                        if(rec) ptr[ppos] |= bit; else ptr[ppos] &= ~bit;
                        bpos++;
                    }
                    rec ^= 1;
                }
                auto last = bpos % 8;
                size = (u32)(((bpos / 8) + (last != 0)) & 0xFFFFFF00) | (*(u32*)(psize) & 0x000000FF);
                *plast = (u8)(last ? last : 8);
                dwordLE(&psize, size);
                break;
            }
            case TZX_UNION: {
                auto npp(blk->nip), npd(blk->nib);
                auto asp = (int)(blk->pilots - blk->twp) / (npp + 1);
                auto asd = (int)(blk->bits - blk->twb) / (npd + 1);
                auto totp = (int)(blk->twb - blk->pilots) / 3;
                auto totd = blk->nwb / npd;
                size = 18 + ((int)(blk->bits - blk->data)) + totd;
                dwordLE(&ptr, size);
                wordLE(&ptr, blk->pause);
                dwordLE(&ptr, totp);
                *ptr++ = npp;
                *ptr++ = (u8)asp;
                dwordLE(&ptr, totd);
                *ptr++ = npd;
                *ptr++ = (u8)asd;
                for(int n = 0 ; n < asp; n++) {
                    auto w = (1 + npp) * n;
                    *ptr++ = blk->twp[w];
                    for(int j = 0; j < npp; j++) wordLE(&ptr, pulses[blk->twp[w + j + 1]]);
                }
                z_memcpy(&ptr, blk->pilots, totp * 3LL);
                for(int n = 0 ; n < asd; n++) {
                    auto w = (1 + npd) * n;
                    *ptr++ = blk->twb[w];
                    for(int j = 0; j < npd; j++) wordLE(&ptr, pulses[blk->twb[w + j + 1]]);
                }
                z_memcpy(&ptr, blk->bits, totd);
            }
			break;
            case TZX_PULSES:
                size = blk->nwp / 2; *ptr++ = (u8)size;
                for(u32 n = 0; n < size; n++) wordLE(&ptr, pulses[data[n * 2 + 1]]);
                break;
            case TZX_TONE:
                wordLE(&ptr, pulses[blk->data[1]]);
                wordLE(&ptr, (u16)blk->nwp);
                break;
            case TZX_PURE_DATA:
                wordLE(&ptr, pulses[blk->data[1]]);
                wordLE(&ptr, pulses[blk->data[4]]);
                tmp = blk->nwb / blk->nib; type = (u8)(tmp % 8);
                *ptr++ = (u8)(type ? type : 8);
                wordLE(&ptr, blk->pause); dwordLE(&ptr, size); ptr--;
                z_memcpy(&ptr, blk->bits, tmp + (size_t)(type != 0));
                break;
            case TZX_SAVE:
            case TZX_NORMAL:
                size = (blk->nwb / blk->nib) >> 3;
                wordLE(&ptr, blk->pause);
                wordLE(&ptr, (u16)size);
                z_memcpy(&ptr, blk->bits, size);
                break;
            case TZX_TURBO:
                wordLE(&ptr, pulses[blk->twp[1]]);
                wordLE(&ptr, pulses[blk->twp[4]]);
                wordLE(&ptr, pulses[blk->twp[5]]);
                wordLE(&ptr, pulses[blk->twb[1]]);
                wordLE(&ptr, pulses[blk->twb[4]]);
                wordLE(&ptr, (u16)((blk->nwp - 1) / blk->nip));
                size = blk->nwb / blk->nib; tmp = size % 8; size /= 8;
                *ptr++ = (u8)(tmp ? tmp : 8);
                wordLE(&ptr, blk->pause); dwordLE(&ptr, size); ptr--;
                z_memcpy(&ptr, blk->bits, size + (size_t)(tmp != 0));
                break;
            case TZX_PAUSE:
                wordLE(&ptr, blk->pause);
                break;
            case TZX_GROUP_START:
                *ptr++ = (u8)size;
                z_memcpy(&ptr, data, size);
                break;
            case TZX_JUMP:
            case TZX_LOOP_START:
                *(u16*)ptr++ = blk->count; ptr++;
                break;
            case TZX_CALL:
                size = blk->count / 2U;
                wordLE(&ptr, (u16)size);
                z_memcpy(&ptr, data, size);
                break;
            case TZX_SELECT:
                wordLE(&ptr, (u16)size);
                z_memcpy(&ptr, data, size);
                break;
            case TZX_LEVEL:
            case TZX_STOP_48K:
                dwordLE(&ptr, size);
                z_memcpy(&ptr, data, size);
                break;
            case TZX_TEXT:
                *ptr++ = (u8)size;
                z_memcpy(&ptr, data, size);
                break;
            case TZX_MESSAGE:
                *ptr++ = (u8)(blk->pause / 1000);
                *ptr++ = (u8)size;
                z_memcpy(&ptr, data, size);
                break;
            case TZX_ARCHIVE:
                wordLE(&ptr, (u16)size);
                z_memcpy(&ptr, data, size);
                break;
            case TZX_HARDWARE:
                *ptr++ = (u8)(size / 3);
                z_memcpy(&ptr, data, size);
                break;
            case TZX_CUSTOM:
                z_memcpy(&ptr, data, 16); size -= 20;
                dwordLE(&ptr, size);
                z_memcpy(&ptr, data + 20, size);
                break;
            case TZX_Z:
                z_memcpy(&ptr, data, size);
                break;
        }
    }
    return ptr;
}
