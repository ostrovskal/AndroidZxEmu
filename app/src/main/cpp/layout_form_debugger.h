// Автоматическая генерация кода для макета
	auto debugger((zFormDebugger*)new zFormDebugger(styles_zviewform2, 0, nullptr, nullptr, 0, false));
	debugger->onInit(false);
	auto debugger01((zLinearLayout*)debugger->attach(new zLinearLayout(styles_zlinearlayout2, 0, true), 
			VIEW_MATCH, VIEW_MATCH)); 
	auto debugger02((zCellLayout*)debugger01->attach(new zCellLayout(styles_default, 0, szi(10, 0), 0_dp), 
			VIEW_MATCH, VIEW_WRAP)); 
	debugger02->attach(new zViewButton(styles_butbp, R.id.butBp, 0, R.integer.iconZxBp), 
			1, 1);
	debugger02->attach(new zViewButton(styles_butbp, R.id.butBpList, 0, R.integer.iconZxBps), 
			1, 0, 1, 1);
	debugger02->attach(new zViewButton(styles_butbp, R.id.butStep, 0, R.integer.iconZxTraceStep), 
			2, 0, 1, 1);
	debugger02->attach(new zViewButton(styles_butbp, R.id.butStepIn, 0, R.integer.iconZxTraceIn), 
			3, 0, 1, 1);
	debugger02->attach(new zViewButton(styles_butbp, R.id.butStepOut, 0, R.integer.iconZxTraceExit), 
			4, 0, 1, 1);
	debugger02->attach(new zViewButton(styles_butbp, R.id.butPlay, 0, R.integer.iconZxPlay), 
			5, 0, 1, 1);
	debugger02->attach(new zViewButton(styles_butbp, R.id.butRadix, R.string.DEC, -1), 
			6, 0, 1, 1);
	debugger02->attach(new zViewButton(styles_butbp, R.id.butList, R.string.DA, -1), 
			7, 0, 1, 1);
	debugger02->attach(new zViewButton(styles_butbp, R.id.butSize, R.string._16, -1), 
			8, 0, 1, 1);
	debugger02->attach(new zViewButton(styles_butbp, R.id.butCpu, R.string.CPU, -1), 
			9, 0, 1, 1);
	auto debugger12((zLinearLayout*)debugger01->attach(new zLinearLayout(styles_default, 0, false), 
			VIEW_MATCH, VIEW_WRAP)); 
	auto debugger03((zLinearLayout*)debugger12->attach(new zLinearLayout(styles_default, 0, true), 
			0, 45, VIEW_MATCH, VIEW_MATCH)); 
	debugger03->attach(new zViewEdit(styles_editasm, R.id.editAsm, 0), 
			VIEW_MATCH, VIEW_WRAP);
	auto debugger04((zLinearLayout*)debugger03->attach(new zLinearLayout(styles_default, 0, false), 
			VIEW_MATCH, VIEW_WRAP)); 
	debugger04->attach(new zViewText(styles_textfs, R.id.textFS, R.string.S), 
			VIEW_MATCH, VIEW_MATCH);
	debugger04->attach(new zViewText(styles_textfs, R.id.textFZ, R.string.Z), 
			VIEW_MATCH, VIEW_MATCH);
	debugger04->attach(new zViewText(styles_textfs, R.id.textFY, R.string.Y), 
			VIEW_MATCH, VIEW_MATCH);
	debugger04->attach(new zViewText(styles_textfs, R.id.textFH, R.string.H), 
			VIEW_MATCH, VIEW_MATCH);
	debugger04->attach(new zViewText(styles_textfs, R.id.textFX, R.string.X), 
			VIEW_MATCH, VIEW_MATCH);
	debugger04->attach(new zViewText(styles_textfs, R.id.textFP, R.string.P), 
			VIEW_MATCH, VIEW_MATCH);
	debugger04->attach(new zViewText(styles_textfs, R.id.textFN, R.string.N), 
			VIEW_MATCH, VIEW_MATCH);
	debugger04->attach(new zViewText(styles_textfs, R.id.textFC, R.string.C), 
			VIEW_MATCH, VIEW_MATCH);
	debugger04->attach(new zViewText(styles_textfs, R.id.textIFF, R.string.IFF), 
			VIEW_WRAP, VIEW_MATCH);
	debugger04->attach(new zViewText(styles_textfs, R.id.textDOS, R.string.DOS), 
			VIEW_WRAP, VIEW_MATCH);
	auto debugger14((zLinearLayout*)debugger03->attach(new zLinearLayout(styles_zlinearlayout3, 0, false), 
			VIEW_MATCH, VIEW_WRAP)); 
	debugger14->attach(new zViewText(styles_textfs, R.id.textPC, R.string.PC), 
			VIEW_WRAP, VIEW_WRAP);
	debugger14->attach(new zViewEdit(styles_editpc, R.id.editPC, 0), 
			VIEW_MATCH, VIEW_WRAP);
	debugger14->attach(new zViewText(styles_textfs, R.id.textSP, R.string.SP), 
			VIEW_WRAP, VIEW_WRAP);
	debugger14->attach(new zViewEdit(styles_editpc, R.id.editSP, 0), 
			VIEW_MATCH, VIEW_WRAP);
	auto debugger24((zLinearLayout*)debugger03->attach(new zLinearLayout(styles_zlinearlayout3, 0, false), 
			VIEW_MATCH, VIEW_WRAP)); 
	debugger24->attach(new zViewText(styles_textfs, R.id.textI, R.string.I), 
			VIEW_WRAP, VIEW_WRAP);
	debugger24->attach(new zViewEdit(styles_editpc, R.id.editI, 0), 
			VIEW_MATCH, VIEW_WRAP);
	debugger24->attach(new zViewText(styles_textfs, R.id.textR, R.string.R), 
			VIEW_WRAP, VIEW_MATCH);
	debugger24->attach(new zViewEdit(styles_editpc, R.id.editR, 0), 
			VIEW_MATCH, VIEW_WRAP);
	debugger24->attach(new zViewText(styles_textfs, R.id.textIM, R.string.IM), 
			VIEW_WRAP, VIEW_MATCH);
	debugger24->attach(new zViewEdit(styles_editpc, R.id.editIM, 0), 
			VIEW_MATCH, VIEW_WRAP);
	auto debugger34((zLinearLayout*)debugger03->attach(new zLinearLayout(styles_zlinearlayout3, 0, false), 
			VIEW_MATCH, VIEW_WRAP)); 
	debugger34->attach(new zViewText(styles_textfs, R.id.textTS, R.string.TS), 
			VIEW_WRAP, VIEW_MATCH);
	debugger34->attach(new zViewEdit(styles_editts, R.id.editTS, 0), 
			VIEW_MATCH, VIEW_WRAP);
	debugger34->attach(new zViewEdit(styles_editts1, R.id.editTS1, 0), 
			VIEW_WRAP, VIEW_WRAP);
	auto debugger13((zLinearLayout*)debugger12->attach(new zLinearLayout(styles_default, R.id.ll16, true), 
			0, 55, VIEW_MATCH, VIEW_WRAP)); 
	auto debugger44((zLinearLayout*)debugger13->attach(new zLinearLayout(styles_default, 0, false), 
			VIEW_MATCH, VIEW_WRAP)); 
	auto debugger05((zLinearLayout*)debugger44->attach(new zLinearLayout(styles_zlinearlayout6, 0, true), 
			VIEW_WRAP, VIEW_MATCH)); 
	debugger05->attach(new zViewText(styles_textfs, R.id.textAF, R.string.AF), 
			VIEW_WRAP, VIEW_MATCH);
	debugger05->attach(new zViewText(styles_textfs, R.id.textBC, R.string.BC), 
			VIEW_WRAP, VIEW_MATCH);
	debugger05->attach(new zViewText(styles_textfs, R.id.textDE, R.string.DE), 
			VIEW_WRAP, VIEW_MATCH);
	debugger05->attach(new zViewText(styles_textfs, R.id.textHL, R.string.HL), 
			VIEW_WRAP, VIEW_MATCH);
	debugger05->attach(new zViewText(styles_textfs, R.id.textIX, R.string.IX), 
			VIEW_WRAP, VIEW_MATCH);
	auto debugger15((zLinearLayout*)debugger44->attach(new zLinearLayout(styles_default, 0, true), 
			VIEW_MATCH, VIEW_MATCH)); 
	debugger15->attach(new zViewEdit(styles_editpc, R.id.editAF, 0), 
			VIEW_MATCH, VIEW_WRAP);
	debugger15->attach(new zViewEdit(styles_editpc, R.id.editBC, 0), 
			VIEW_MATCH, VIEW_WRAP);
	debugger15->attach(new zViewEdit(styles_editpc, R.id.editDE, 0), 
			VIEW_MATCH, VIEW_WRAP);
	debugger15->attach(new zViewEdit(styles_editpc, R.id.editHL, 0), 
			VIEW_MATCH, VIEW_WRAP);
	debugger15->attach(new zViewEdit(styles_editpc, R.id.editIX, 0), 
			VIEW_MATCH, VIEW_WRAP);
	auto debugger25((zLinearLayout*)debugger44->attach(new zLinearLayout(styles_zlinearlayout6, 0, true), 
			VIEW_WRAP, VIEW_MATCH)); 
	debugger25->attach(new zViewText(styles_textfs, R.id.textAF_, R.string.AF_), 
			VIEW_WRAP, VIEW_MATCH);
	debugger25->attach(new zViewText(styles_textfs, R.id.textBC_, R.string.BC_), 
			VIEW_WRAP, VIEW_MATCH);
	debugger25->attach(new zViewText(styles_textfs, R.id.textDE_, R.string.DE_), 
			VIEW_WRAP, VIEW_MATCH);
	debugger25->attach(new zViewText(styles_textfs, R.id.textHL_, R.string.HL_), 
			VIEW_WRAP, VIEW_MATCH);
	debugger25->attach(new zViewText(styles_textfs, R.id.textIY, R.string.IY), 
			VIEW_WRAP, VIEW_MATCH);
	auto debugger35((zLinearLayout*)debugger44->attach(new zLinearLayout(styles_default, 0, true), 
			VIEW_MATCH, VIEW_MATCH)); 
	debugger35->attach(new zViewEdit(styles_editpc, R.id.editAF_, 0), 
			VIEW_MATCH, VIEW_WRAP);
	debugger35->attach(new zViewEdit(styles_editpc, R.id.editBC_, 0), 
			VIEW_MATCH, VIEW_WRAP);
	debugger35->attach(new zViewEdit(styles_editpc, R.id.editDE_, 0), 
			VIEW_MATCH, VIEW_WRAP);
	debugger35->attach(new zViewEdit(styles_editpc, R.id.editHL_, 0), 
			VIEW_MATCH, VIEW_WRAP);
	debugger35->attach(new zViewEdit(styles_editpc, R.id.editIY, 0), 
			VIEW_MATCH, VIEW_WRAP);
	auto debugger23((zLinearLayout*)debugger12->attach(new zLinearLayout(styles_ll8, R.id.ll8, true), 
			0, 55, VIEW_MATCH, VIEW_WRAP)); 
	auto debugger54((zLinearLayout*)debugger23->attach(new zLinearLayout(styles_default, 0, false), 
			VIEW_MATCH, VIEW_WRAP)); 
	auto debugger45((zLinearLayout*)debugger54->attach(new zLinearLayout(styles_zlinearlayout6, 0, true), 
			VIEW_WRAP, VIEW_MATCH)); 
	debugger45->attach(new zViewText(styles_textfs, R.id.textF, R.string.F), 
			VIEW_WRAP, VIEW_MATCH);
	debugger45->attach(new zViewText(styles_textfs, R.id.textC, R.string.C), 
			VIEW_WRAP, VIEW_MATCH);
	debugger45->attach(new zViewText(styles_textfs, R.id.textE, R.string.E), 
			VIEW_WRAP, VIEW_MATCH);
	debugger45->attach(new zViewText(styles_textfs, R.id.textL, R.string.L), 
			VIEW_WRAP, VIEW_MATCH);
	debugger45->attach(new zViewText(styles_textfs, R.id.textXL, R.string.XL), 
			VIEW_WRAP, VIEW_MATCH);
	auto debugger55((zLinearLayout*)debugger54->attach(new zLinearLayout(styles_default, 0, true), 
			VIEW_MATCH, VIEW_MATCH)); 
	debugger55->attach(new zViewEdit(styles_editpc, R.id.editF, 0), 
			VIEW_MATCH, VIEW_WRAP);
	debugger55->attach(new zViewEdit(styles_editpc, R.id.editC, 0), 
			VIEW_MATCH, VIEW_WRAP);
	debugger55->attach(new zViewEdit(styles_editpc, R.id.editE, 0), 
			VIEW_MATCH, VIEW_WRAP);
	debugger55->attach(new zViewEdit(styles_editpc, R.id.editL, 0), 
			VIEW_MATCH, VIEW_WRAP);
	debugger55->attach(new zViewEdit(styles_editpc, R.id.editXL, 0), 
			VIEW_MATCH, VIEW_WRAP);
	auto debugger65((zLinearLayout*)debugger54->attach(new zLinearLayout(styles_zlinearlayout6, 0, true), 
			VIEW_WRAP, VIEW_MATCH)); 
	debugger65->attach(new zViewText(styles_textfs, R.id.textA, R.string.A), 
			VIEW_WRAP, VIEW_MATCH);
	debugger65->attach(new zViewText(styles_textfs, R.id.textB, R.string.B), 
			VIEW_WRAP, VIEW_MATCH);
	debugger65->attach(new zViewText(styles_textfs, R.id.textD, R.string.D), 
			VIEW_WRAP, VIEW_MATCH);
	debugger65->attach(new zViewText(styles_textfs, R.id.textH, R.string.H), 
			VIEW_WRAP, VIEW_MATCH);
	debugger65->attach(new zViewText(styles_textfs, R.id.textXH, R.string.XH), 
			VIEW_WRAP, VIEW_MATCH);
	auto debugger75((zLinearLayout*)debugger54->attach(new zLinearLayout(styles_default, 0, true), 
			VIEW_MATCH, VIEW_MATCH)); 
	debugger75->attach(new zViewEdit(styles_editpc, R.id.editA, 0), 
			VIEW_MATCH, VIEW_WRAP);
	debugger75->attach(new zViewEdit(styles_editpc, R.id.editB, 0), 
			VIEW_MATCH, VIEW_WRAP);
	debugger75->attach(new zViewEdit(styles_editpc, R.id.editD, 0), 
			VIEW_MATCH, VIEW_WRAP);
	debugger75->attach(new zViewEdit(styles_editpc, R.id.editH, 0), 
			VIEW_MATCH, VIEW_WRAP);
	debugger75->attach(new zViewEdit(styles_editpc, R.id.editXH, 0), 
			VIEW_MATCH, VIEW_WRAP);
	auto debugger85((zLinearLayout*)debugger54->attach(new zLinearLayout(styles_zlinearlayout6, 0, true), 
			VIEW_WRAP, VIEW_MATCH)); 
	debugger85->attach(new zViewText(styles_textfs, R.id.textF_, R.string.F_), 
			VIEW_WRAP, VIEW_MATCH);
	debugger85->attach(new zViewText(styles_textfs, R.id.textC_, R.string.C_), 
			VIEW_WRAP, VIEW_MATCH);
	debugger85->attach(new zViewText(styles_textfs, R.id.textE_, R.string.E_), 
			VIEW_WRAP, VIEW_MATCH);
	debugger85->attach(new zViewText(styles_textfs, R.id.textL_, R.string.L_), 
			VIEW_WRAP, VIEW_MATCH);
	debugger85->attach(new zViewText(styles_textfs, R.id.textYL, R.string.YL), 
			VIEW_WRAP, VIEW_MATCH);
	auto debugger95((zLinearLayout*)debugger54->attach(new zLinearLayout(styles_default, 0, true), 
			VIEW_MATCH, VIEW_MATCH)); 
	debugger95->attach(new zViewEdit(styles_editpc, R.id.editF_, 0), 
			VIEW_MATCH, VIEW_WRAP);
	debugger95->attach(new zViewEdit(styles_editpc, R.id.editC_, 0), 
			VIEW_MATCH, VIEW_WRAP);
	debugger95->attach(new zViewEdit(styles_editpc, R.id.editE_, 0), 
			VIEW_MATCH, VIEW_WRAP);
	debugger95->attach(new zViewEdit(styles_editpc, R.id.editL_, 0), 
			VIEW_MATCH, VIEW_WRAP);
	debugger95->attach(new zViewEdit(styles_editpc, R.id.editYL, 0), 
			VIEW_MATCH, VIEW_WRAP);
	auto debugger105((zLinearLayout*)debugger54->attach(new zLinearLayout(styles_zlinearlayout6, 0, true), 
			VIEW_WRAP, VIEW_MATCH)); 
	debugger105->attach(new zViewText(styles_textfs, R.id.textA_, R.string.A_), 
			VIEW_WRAP, VIEW_MATCH);
	debugger105->attach(new zViewText(styles_textfs, R.id.textB_, R.string.B_), 
			VIEW_WRAP, VIEW_MATCH);
	debugger105->attach(new zViewText(styles_textfs, R.id.textD_, R.string.D_), 
			VIEW_WRAP, VIEW_MATCH);
	debugger105->attach(new zViewText(styles_textfs, R.id.textH_, R.string.H_), 
			VIEW_WRAP, VIEW_MATCH);
	debugger105->attach(new zViewText(styles_textfs, R.id.textYH, R.string.YH), 
			VIEW_WRAP, VIEW_MATCH);
	auto debugger115((zLinearLayout*)debugger54->attach(new zLinearLayout(styles_default, 0, true), 
			VIEW_MATCH, VIEW_MATCH)); 
	debugger115->attach(new zViewEdit(styles_editpc, R.id.editA_, 0), 
			VIEW_MATCH, VIEW_WRAP);
	debugger115->attach(new zViewEdit(styles_editpc, R.id.editB_, 0), 
			VIEW_MATCH, VIEW_WRAP);
	debugger115->attach(new zViewEdit(styles_editpc, R.id.editD_, 0), 
			VIEW_MATCH, VIEW_WRAP);
	debugger115->attach(new zViewEdit(styles_editpc, R.id.editH_, 0), 
			VIEW_MATCH, VIEW_WRAP);
	debugger115->attach(new zViewEdit(styles_editpc, R.id.editYH, 0), 
			VIEW_MATCH, VIEW_WRAP);
	auto debugger22((zLinearLayout*)debugger01->attach(new zLinearLayout(styles_llgs, R.id.llGS, false), 
			VIEW_MATCH, VIEW_WRAP)); 
	debugger22->attach(new zViewText(styles_textfs, R.id.textGsCmd, R.string.cmd), 
			VIEW_WRAP, VIEW_WRAP);
	debugger22->attach(new zViewEdit(styles_editts, R.id.editGsCmd, 0), 
			VIEW_MATCH, VIEW_WRAP);
	debugger22->attach(new zViewText(styles_textfs, R.id.textGsSts, R.string.sts), 
			VIEW_WRAP, VIEW_WRAP);
	debugger22->attach(new zViewEdit(styles_editts, R.id.editGsSts, 0), 
			VIEW_MATCH, VIEW_WRAP);
	debugger22->attach(new zViewText(styles_textfs, R.id.textGsDat, R.string.dat), 
			VIEW_WRAP, VIEW_WRAP);
	debugger22->attach(new zViewEdit(styles_editts, R.id.editGsDat, 0), 
			VIEW_MATCH, VIEW_WRAP);
	debugger22->attach(new zViewText(styles_textfs, R.id.textGsOut, R.string.out), 
			VIEW_WRAP, VIEW_WRAP);
	debugger22->attach(new zViewEdit(styles_editts, R.id.editGsOut, 0), 
			VIEW_MATCH, VIEW_WRAP);
	debugger22->attach(new zViewText(styles_textfs, R.id.textGsPage, R.string.page), 
			VIEW_WRAP, VIEW_WRAP);
	debugger22->attach(new zViewEdit(styles_editts, R.id.editGsPage, 0), 
			VIEW_MATCH, VIEW_WRAP);
	auto debugger32((zLinearLayout*)debugger01->attach(new zLinearLayout(styles_zlinearlayout3, R.id.llPorts, false), 
			VIEW_MATCH, VIEW_WRAP)); 
	debugger32->attach(new zViewText(styles_textfs, R.id.text7FFD, R.string._7FFD), 
			VIEW_WRAP, VIEW_WRAP);
	debugger32->attach(new zViewEdit(styles_editts, R.id.edit7FFD, 0), 
			VIEW_MATCH, VIEW_WRAP);
	debugger32->attach(new zViewText(styles_textfs, R.id.text1FFD, R.string._1FFD), 
			VIEW_WRAP, VIEW_WRAP);
	debugger32->attach(new zViewEdit(styles_editts, R.id.edit1FFD, 0), 
			VIEW_MATCH, VIEW_WRAP);
	debugger01->attach(new zRibbonDebugger(styles_bplist, R.id.listDebugger, true), 
			VIEW_MATCH, VIEW_MATCH);
