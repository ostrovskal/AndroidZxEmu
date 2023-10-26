// Автоматическая генерация кода для макета
	auto browser((zFormBrowser*)new zFormBrowser(styles_zviewform1, R.id.formBrowser, styles_z_formcaption, styles_z_formfooter, R.string.formBrowser, true));
	browser->onInit(false);
	auto browser01((zLinearLayout*)browser->attach(new zLinearLayout(styles_default, 0, true), 
			VIEW_MATCH, VIEW_MATCH)); 
	auto browser02((zLinearLayout*)browser01->attach(new zLinearLayout(styles_default, 0, false), 
			VIEW_MATCH, VIEW_WRAP)); 
	browser02->attach(new zViewSelect(styles_browsspin, R.id.browsSpin), 
			0, 35, VIEW_WRAP, VIEW_WRAP);
	browser02->attach(new zViewText(styles_browsdisk, R.id.browsDisk, 0), 
			0, 65, VIEW_MATCH, VIEW_MATCH);
	auto browser12((zLinearLayout*)browser01->attach(new zLinearLayout(styles_default, 0, false), 
			VIEW_MATCH, VIEW_MATCH)); 
	auto browser03((zLinearLayout*)browser12->attach(new zLinearLayout(styles_default, 0, true), 
			VIEW_MATCH, VIEW_MATCH)); 
	browser03->attach(new zViewEdit(styles_browsedit, R.id.browsEdit, 0), 
			VIEW_MATCH, VIEW_WRAP);
	browser03->attach(new zViewRibbon(styles_bplist, R.id.browsList, true), 
			VIEW_MATCH, VIEW_MATCH);
	auto browser13((zLinearLayout*)browser12->attach(new zLinearLayout(styles_default, 0, true), 
			VIEW_WRAP, VIEW_MATCH)); 
	browser13->attach(new zViewButton(styles_browsopen, R.id.browsOpen, 0, R.integer.iconZxOpen), 
			VIEW_WRAP, VIEW_WRAP);
	browser13->attach(new zViewButton(styles_browsopen, R.id.browsSave, 0, R.integer.iconZxSave), 
			VIEW_WRAP, VIEW_WRAP);
	browser13->attach(new zViewButton(styles_browsopen, R.id.browsEject, 0, R.integer.iconZxEject), 
			VIEW_WRAP, VIEW_WRAP);
	browser13->attach(new zViewCheck(styles_browschk, R.id.browsChk, R.string.zip), 
			VIEW_WRAP, VIEW_WRAP);
	auto browser23((zLinearLayout*)browser12->attach(new zLinearLayout(styles_default, 0, true), 
			VIEW_WRAP, VIEW_MATCH)); 
	browser23->attach(new zViewButton(styles_browsopen, R.id.browsFolder, 0, R.integer.iconZxFolder1), 
			VIEW_WRAP, VIEW_WRAP);
	browser23->attach(new zViewButton(styles_browsopen, R.id.browsDel, 0, R.integer.iconZxDel), 
			VIEW_WRAP, VIEW_WRAP);
	browser23->attach(new zViewButton(styles_browsopen, R.id.browsTrDos, 0, R.integer.iconZxTrDos), 
			VIEW_WRAP, VIEW_WRAP);
	browser23->attach(new zViewCheck(styles_browschk, R.id.browsTape, R.string.settings_auto_tape), 
			VIEW_MATCH, VIEW_WRAP);
