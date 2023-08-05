#pragma once

class zDisAsm {
public:
	// парсер инструкции
	static u16 parser(zCpu* cpu, u16 PC, char* buffer, int flags);
	// сдвинуть позицию
	static int movePC(int entry, int delta, int count);
	// перейти
	static int jump(zCpu* cpu, u16 entry, int mode, bool isCall);
	// трассировка
	static void trace(zCpu* cpu, int mode);
	static u16 trAddr[2];
protected:
	// информация об операндах
	static int operand(int dst, int src, int& value);
	// парсер префикса/кода операции
	static void ocr();
private:
	static u16 pc;
};