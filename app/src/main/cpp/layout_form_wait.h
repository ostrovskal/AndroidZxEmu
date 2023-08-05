// Автоматическая генерация кода для макета
	auto wait((zViewForm*)new zViewForm(styles_formtzx, R.id.formWait, nullptr, nullptr, 0, false));
	wait->onInit(false);
	wait->attach(new zViewProgress(styles_z_circularprogress, R.id.waitPrg, 0, szi(0, 1), 0, false), 
			VIEW_MATCH, VIEW_MATCH);
