// Автоматическая генерация кода для макета
	auto mainLL((zLinearLayout*)root->attach(new zLinearLayout(styles_default, R.id.llMain, true), 
			VIEW_MATCH, VIEW_MATCH)); 
	auto frame01((zSpeccyLayout*)mainLL->attach(new zSpeccyLayout(styles_speccy, R.id.speccy, szi(352, 288)), 
			0, 70, VIEW_MATCH, VIEW_MATCH)); 
	frame01->attach(new zViewText(styles_speccyfps, R.id.speccyFps, 0), 
			ZS_GRAVITY_END | ZS_GRAVITY_BOTTOM, 0, VIEW_MATCH, VIEW_WRAP);
	frame01->attach(new zViewImage(styles_speccystatus, R.id.speccyStatus, R.integer.iconZxDisk), 
			ZS_GRAVITY_END | ZS_GRAVITY_TOP, 0, 48_dp, 48_dp);
