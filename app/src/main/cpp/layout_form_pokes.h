// Автоматическая генерация кода для макета
	auto pokes((zFormPokes*)new zFormPokes(styles_formasm, R.id.formPokes, styles_z_formcaption, nullptr, R.string.formPokes, true));
	pokes->onInit(false);
	auto pokes01((zLinearLayout*)pokes->attach(new zLinearLayout(styles_zlinearlayout1, 0, true), 
			VIEW_MATCH, VIEW_MATCH)); 
	pokes01->attach(new zViewSelect(styles_z_spinner, R.id.pokesSpin), 
			VIEW_MATCH, VIEW_WRAP);
	pokes01->attach(new zViewRibbon(styles_pokeslist, R.id.pokesList, true), 
			VIEW_MATCH, VIEW_MATCH);
