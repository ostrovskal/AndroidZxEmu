// Автоматическая генерация кода для макета
	auto selfile((zFormSelFile*)new zFormSelFile(styles_zviewform1, R.id.formSelFile, styles_z_formcaption, nullptr, R.string.Viybor_fayla, true));
	selfile->onInit(false);
	auto selfile01((zLinearLayout*)selfile->attach(new zLinearLayout(styles_zlinearlayout1, 0, true), 
			VIEW_MATCH, VIEW_MATCH)); 
	selfile01->attach(new zViewText(styles_zviewtext2, 0, R.string.Select_file_), 
			VIEW_MATCH, VIEW_WRAP);
	selfile01->attach(new zViewRibbon(styles_sellist, R.id.selList, true), 
			VIEW_MATCH, VIEW_MATCH);
