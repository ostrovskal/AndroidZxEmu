// Автоматическая генерация кода для макета
	auto mainLL((zLinearLayout*)root->attach(new zLinearLayout(styles_default, R.id.llMain, false), 
			VIEW_MATCH, VIEW_MATCH)); 
	auto mainLL01((zLinearLayout*)mainLL->attach(new zLinearLayout(styles_default, 0, true), 
			VIEW_MATCH, VIEW_MATCH)); 
	auto frame02((zSpeccyLayout*)mainLL01->attach(new zSpeccyLayout(styles_speccy, R.id.speccy, szi(352, 288)), 
			0, 70, VIEW_MATCH, VIEW_MATCH)); 
	frame02->attach(new zViewText(styles_speccyfps, R.id.speccyFps, 0), 
			ZS_GRAVITY_END | ZS_GRAVITY_BOTTOM, 0, VIEW_MATCH, VIEW_WRAP);
	frame02->attach(new zViewImage(styles_speccystatus, R.id.speccyStatus, R.integer.iconZxDisk), 
			ZS_GRAVITY_END | ZS_GRAVITY_TOP, 0, 48_dp, 48_dp);
