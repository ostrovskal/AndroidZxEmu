// Автоматическая генерация кода для макета
	auto assembler((zFormAssembler*)new zFormAssembler(styles_formasm, R.id.formAsm, styles_z_formcaption, styles_z_formfooter, R.string.Assembler, true));
	assembler->onInit(false);
	auto assembler01((zLinearLayout*)assembler->attach(new zLinearLayout(styles_zlinearlayout1, 0, false), 
			VIEW_MATCH, VIEW_MATCH)); 
