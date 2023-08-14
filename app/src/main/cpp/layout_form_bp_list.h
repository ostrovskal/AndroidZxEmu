// Автоматическая генерация кода для макета
	auto bplist((zFormBpList*)new zFormBpList(styles_zviewform1, 0, styles_z_formcaption, styles_z_formfooter, R.string.formBpList, true));
	bplist->onInit(false);
	auto bplist01((zLinearLayout*)bplist->attach(new zLinearLayout(styles_zlinearlayout1, 0, false), 
			VIEW_MATCH, VIEW_MATCH)); 
	auto bplist02((zLinearLayout*)bplist01->attach(new zLinearLayout(styles_default, 0, true), 
			VIEW_MATCH, VIEW_MATCH)); 
	auto bplist03((zLinearLayout*)bplist02->attach(new zLinearLayout(styles_default, 0, false), 
			VIEW_MATCH, VIEW_WRAP)); 
	bplist03->attach(new zViewText(styles_default, 0, R.string.bp_address1), 
			VIEW_MATCH, VIEW_WRAP);
	bplist03->attach(new zViewText(styles_default, 0, R.string.bp_address2), 
			VIEW_MATCH, VIEW_WRAP);
	bplist03->attach(new zViewText(styles_default, 0, R.string.bp_value), 
			VIEW_MATCH, VIEW_WRAP);
	bplist03->attach(new zViewText(styles_default, 0, R.string.bp_mask), 
			VIEW_MATCH, VIEW_WRAP);
	auto bplist13((zLinearLayout*)bplist02->attach(new zLinearLayout(styles_default, 0, false), 
			VIEW_MATCH, VIEW_WRAP)); 
	bplist13->attach(new zViewEdit(styles_bpeditrange1, R.id.bpEditRange1, 0), 
			VIEW_MATCH, VIEW_WRAP);
	bplist13->attach(new zViewEdit(styles_bpeditrange1, R.id.bpEditRange2, 0), 
			VIEW_MATCH, VIEW_WRAP);
	bplist13->attach(new zViewEdit(styles_bpeditrange1, R.id.bpEditValue1, 0), 
			VIEW_MATCH, VIEW_WRAP);
	bplist13->attach(new zViewEdit(styles_bpeditrange1, R.id.bpEditValue2, 0), 
			VIEW_MATCH, VIEW_WRAP);
	bplist02->attach(new zViewText(styles_default, 0, R.string.bp_condition), 
			VIEW_WRAP, VIEW_WRAP);
	auto bplist23((zLinearLayout*)bplist02->attach(new zLinearLayout(styles_default, 0, false), 
			VIEW_MATCH, VIEW_WRAP)); 
	bplist23->attach(new zViewSelect(styles_bpspinops, R.id.bpSpinOps), 
			VIEW_MATCH, VIEW_WRAP);
	bplist23->attach(new zViewSelect(styles_bpspincond, R.id.bpSpinCond), 
			VIEW_MATCH, VIEW_WRAP);
	auto bplist33((zLinearLayout*)bplist02->attach(new zLinearLayout(styles_default, 0, false), 
			VIEW_MATCH, VIEW_WRAP)); 
	bplist33->attach(new zViewText(styles_zviewtext1, 0, R.string.bp_address1), 
			VIEW_MATCH, VIEW_WRAP);
	bplist33->attach(new zViewText(styles_zviewtext1, 0, R.string.bp_address2), 
			VIEW_MATCH, VIEW_WRAP);
	bplist33->attach(new zViewText(styles_zviewtext1, 0, R.string.bp_value), 
			VIEW_MATCH, VIEW_WRAP);
	bplist33->attach(new zViewText(styles_zviewtext1, 0, R.string.bp_mask), 
			VIEW_MATCH, VIEW_WRAP);
	bplist33->attach(new zViewText(styles_zviewtext1, 0, R.string.bp_condition), 
			VIEW_MATCH, VIEW_WRAP);
	bplist33->attach(new zViewText(styles_zviewtext1, 0, R.string.bp_access), 
			VIEW_MATCH, VIEW_WRAP);
	bplist02->attach(new zViewRibbon(styles_bplist, R.id.bpList, true), 
			VIEW_MATCH, VIEW_MATCH);
