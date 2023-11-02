// Автоматическая генерация кода для макета
	auto tzx((zFormTZX*)new zFormTZX(styles_formtzx, R.id.formTzx, nullptr, nullptr, 0, false));
	tzx->onInit(false);
	auto tzx01((zLinearLayout*)tzx->attach(new zLinearLayout(styles_zlinearlayout5, 0, false), 
			VIEW_MATCH, VIEW_MATCH)); 
	tzx01->attach(new zViewText(styles_tzxmsg, R.id.tzxMsg, 0), 
			VIEW_MATCH, VIEW_MATCH);
	auto tzx02((zLinearLayout*)tzx01->attach(new zLinearLayout(styles_ll8, R.id.tzxSel, true), 
			VIEW_WRAP, VIEW_WRAP)); 
