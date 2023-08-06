// Автоматическая генерация кода для макета
	auto settings((zFormSettings*)new zFormSettings(styles_z_form, R.id.formSettings, styles_z_formcaption, styles_z_formfooter, R.string.form_settings, true));
	settings->onInit(false);
	auto settings01((zTabLayout*)settings->attach(new zTabLayout(styles_z_tabhost, 0, styles_z_tabwidget, ZS_GRAVITY_TOP), 
			0, 0)); 
	auto settings02((zTabWidget*)settings01->attach(new zTabWidget(styles_default, 0, 0, R.integer.iconZxComp), 
			VIEW_WRAP, VIEW_WRAP)); 
	auto settings03((zLinearLayout*)settings02->attach(new zLinearLayout(styles_default, 0, false), 
			VIEW_MATCH, VIEW_MATCH)); 
	auto settings04((zLinearLayout*)settings03->attach(new zLinearLayout(styles_zlinearlayout3, 0, true), 
			0, 1, VIEW_WRAP, VIEW_MATCH)); 
	settings04->attach(new zViewText(styles_zviewtext3, 0, R.string.settings_system), 
			VIEW_MATCH, VIEW_WRAP);
	settings04->attach(new zViewSlider(styles_mainsystem, R.id.mainSystem, 0, szi(3, 12), 6, false), 
			VIEW_MATCH, VIEW_WRAP);
	settings04->attach(new zViewText(styles_zviewtext3, 0, R.string.settings_border), 
			VIEW_MATCH, VIEW_WRAP);
	settings04->attach(new zViewSlider(styles_mainsystem, R.id.mainBorder, 0, szi(0, 6), 3, false), 
			VIEW_MATCH, VIEW_WRAP);
	settings04->attach(new zViewCheck(styles_browschk, R.id.mainDarkTheme, R.string.settings_dark_theme), 
			VIEW_MATCH, VIEW_WRAP);
	settings04->attach(new zViewCheck(styles_browschk, R.id.mainGsReset, R.string.settings_gs_reset), 
			VIEW_MATCH, VIEW_WRAP);
	settings04->attach(new zViewCheck(styles_browschk, R.id.mainTapeReset, R.string.settings_tape_reset), 
			VIEW_MATCH, VIEW_WRAP);
	settings04->attach(new zViewCheck(styles_browschk, R.id.mainAutoTape, R.string.settings_auto_tape), 
			VIEW_MATCH, VIEW_WRAP);
	auto settings14((zLinearLayout*)settings03->attach(new zLinearLayout(styles_zlinearlayout5, 0, true), 
			VIEW_WRAP, VIEW_MATCH)); 
	settings14->attach(new zViewText(styles_zviewtext3, 0, R.string.settings_joystick), 
			VIEW_MATCH, VIEW_WRAP);
	settings14->attach(new zViewSlider(styles_mainsystem, R.id.mainJoystick, 0, szi(0, 5), 3, false), 
			VIEW_MATCH, VIEW_WRAP);
	settings14->attach(new zViewText(styles_zviewtext3, 0, R.string.settings_keyboard), 
			VIEW_MATCH, VIEW_WRAP);
	settings14->attach(new zViewSlider(styles_mainsystem, R.id.mainKeyboard, 0, szi(0, 5), 3, false), 
			VIEW_MATCH, VIEW_WRAP);
	settings14->attach(new zViewCheck(styles_browschk, R.id.mainSwapMouse, R.string.settings_swap_mouse_buttons), 
			VIEW_MATCH, VIEW_WRAP);
	settings14->attach(new zViewCheck(styles_browschk, R.id.mainGigaScreen, R.string.settings_giga_screen), 
			VIEW_MATCH, VIEW_WRAP);
	settings14->attach(new zViewCheck(styles_browschk, R.id.mainTrapTRDOS, R.string.settings_trap_tr_dos), 
			VIEW_MATCH, VIEW_WRAP);
	settings14->attach(new zViewCheck(styles_browschk, R.id.mainShowFPS, R.string.settings_show_fps), 
			VIEW_MATCH, VIEW_WRAP);
	auto settings12((zTabWidget*)settings01->attach(new zTabWidget(styles_default, 0, 0, R.integer.iconZxSound), 
			VIEW_WRAP, VIEW_WRAP)); 
	auto settings13((zLinearLayout*)settings12->attach(new zLinearLayout(styles_default, 0, true), 
			VIEW_MATCH, VIEW_MATCH)); 
	auto settings24((zLinearLayout*)settings13->attach(new zLinearLayout(styles_default, 0, false), 
			VIEW_MATCH, VIEW_WRAP)); 
	settings24->attach(new zViewText(styles_default, 0, R.string.Chip), 
			VIEW_MATCH, VIEW_WRAP);
	settings24->attach(new zViewText(styles_default, 0, R.string.Channels), 
			VIEW_MATCH, VIEW_WRAP);
	settings24->attach(new zViewText(styles_default, 0, R.string.Frequency), 
			VIEW_MATCH, VIEW_WRAP);
	auto settings34((zLinearLayout*)settings13->attach(new zLinearLayout(styles_default, 0, false), 
			VIEW_MATCH, VIEW_WRAP)); 
	settings34->attach(new zViewSelect(styles_soundspinchip, R.id.soundSpinChip), 
			VIEW_MATCH, VIEW_WRAP);
	settings34->attach(new zViewSelect(styles_soundspinchannels, R.id.soundSpinChannels), 
			VIEW_MATCH, VIEW_WRAP);
	settings34->attach(new zViewSelect(styles_soundspinfreq, R.id.soundSpinFreq), 
			VIEW_MATCH, VIEW_WRAP);
	auto settings44((zLinearLayout*)settings13->attach(new zLinearLayout(styles_default, 0, false), 
			VIEW_MATCH, VIEW_MATCH)); 
	auto settings05((zLinearLayout*)settings44->attach(new zLinearLayout(styles_default, 0, true), 
			VIEW_MATCH, VIEW_MATCH)); 
	settings05->attach(new zViewCheck(styles_browschk, R.id.soundChkBeeper, R.string.Beeper), 
			VIEW_MATCH, VIEW_WRAP);
	settings05->attach(new zViewSlider(styles_mainsystem, R.id.soundSlrBeeper, 0, szi(0, 12), 5, false), 
			VIEW_MATCH, VIEW_MATCH);
	settings05->attach(new zViewCheck(styles_browschk, R.id.soundChkCovox, R.string.Covox), 
			VIEW_MATCH, VIEW_WRAP);
	settings05->attach(new zViewSlider(styles_mainsystem, R.id.soundSlrCovox, 0, szi(0, 63), 31, false), 
			VIEW_MATCH, VIEW_MATCH);
	auto settings15((zLinearLayout*)settings44->attach(new zLinearLayout(styles_default, 0, true), 
			VIEW_MATCH, VIEW_MATCH)); 
	settings15->attach(new zViewCheck(styles_browschk, R.id.soundChkAY, R.string.AY), 
			VIEW_MATCH, VIEW_WRAP);
	settings15->attach(new zViewSlider(styles_mainsystem, R.id.soundSlrAY, 0, szi(0, 15), 8, false), 
			VIEW_MATCH, VIEW_MATCH);
	settings15->attach(new zViewCheck(styles_browschk, R.id.soundChkGS, R.string.General_sound), 
			VIEW_MATCH, VIEW_WRAP);
	settings15->attach(new zViewSlider(styles_mainsystem, R.id.soundSlrGS, 0, szi(0, 63), 31, false), 
			VIEW_MATCH, VIEW_MATCH);
	auto settings22((zTabWidget*)settings01->attach(new zTabWidget(styles_default, 0, 0, R.integer.iconZxGamepad), 
			VIEW_WRAP, VIEW_WRAP)); 
	auto settings23((zLinearLayout*)settings22->attach(new zLinearLayout(styles_default, 0, true), 
			VIEW_MATCH, VIEW_WRAP)); 
	auto settings54((zLinearLayout*)settings23->attach(new zLinearLayout(styles_default, 0, false), 
			VIEW_MATCH, VIEW_WRAP)); 
	settings54->attach(new zViewText(styles_default, 0, R.string.Layout), 
			VIEW_MATCH, VIEW_WRAP);
	settings54->attach(new zViewText(styles_default, 0, R.string.Presets), 
			VIEW_MATCH, VIEW_WRAP);
	auto settings64((zLinearLayout*)settings23->attach(new zLinearLayout(styles_default, 0, false), 
			VIEW_MATCH, VIEW_WRAP)); 
	settings64->attach(new zViewSelect(styles_joyspinlyt, R.id.joySpinLyt), 
			VIEW_MATCH, VIEW_WRAP);
	settings64->attach(new zViewSelect(styles_z_spinner, R.id.joySpinPresets), 
			VIEW_MATCH, VIEW_WRAP);
	auto settings74((zLinearLayout*)settings23->attach(new zLinearLayout(styles_default, 0, false), 
			VIEW_MATCH, VIEW_WRAP)); 
	settings74->attach(new zViewText(styles_default, 0, R.string.Left), 
			VIEW_MATCH, VIEW_WRAP);
	settings74->attach(new zViewText(styles_default, 0, R.string.Up), 
			VIEW_MATCH, VIEW_WRAP);
	settings74->attach(new zViewText(styles_default, 0, R.string.Right), 
			VIEW_MATCH, VIEW_WRAP);
	settings74->attach(new zViewText(styles_default, 0, R.string.Down), 
			VIEW_MATCH, VIEW_WRAP);
	auto settings84((zLinearLayout*)settings23->attach(new zLinearLayout(styles_default, 0, false), 
			VIEW_MATCH, VIEW_WRAP)); 
	settings84->attach(new zViewSelect(styles_joyspinleft, R.id.joySpinLeft), 
			VIEW_MATCH, VIEW_WRAP);
	settings84->attach(new zViewSelect(styles_joyspinleft, R.id.joySpinUp), 
			VIEW_MATCH, VIEW_WRAP);
	settings84->attach(new zViewSelect(styles_joyspinleft, R.id.joySpinRight), 
			VIEW_MATCH, VIEW_WRAP);
	settings84->attach(new zViewSelect(styles_joyspinleft, R.id.joySpinDown), 
			VIEW_MATCH, VIEW_WRAP);
	auto settings94((zLinearLayout*)settings23->attach(new zLinearLayout(styles_default, 0, false), 
			VIEW_MATCH, VIEW_WRAP)); 
	settings94->attach(new zViewText(styles_default, 0, R.string.Y), 
			VIEW_MATCH, VIEW_WRAP);
	settings94->attach(new zViewText(styles_default, 0, R.string.X), 
			VIEW_MATCH, VIEW_WRAP);
	settings94->attach(new zViewText(styles_default, 0, R.string.A), 
			VIEW_MATCH, VIEW_WRAP);
	settings94->attach(new zViewText(styles_default, 0, R.string.B), 
			VIEW_MATCH, VIEW_WRAP);
	auto settings104((zLinearLayout*)settings23->attach(new zLinearLayout(styles_default, 0, false), 
			VIEW_MATCH, VIEW_WRAP)); 
	settings104->attach(new zViewSelect(styles_joyspinleft, R.id.joySpinY), 
			VIEW_MATCH, VIEW_WRAP);
	settings104->attach(new zViewSelect(styles_joyspinleft, R.id.joySpinX), 
			VIEW_MATCH, VIEW_WRAP);
	settings104->attach(new zViewSelect(styles_joyspinleft, R.id.joySpinA), 
			VIEW_MATCH, VIEW_WRAP);
	settings104->attach(new zViewSelect(styles_joyspinleft, R.id.joySpinB), 
			VIEW_MATCH, VIEW_WRAP);
	auto settings32((zTabWidget*)settings01->attach(new zTabWidget(styles_default, 0, 0, R.integer.iconZxDisplay), 
			VIEW_WRAP, VIEW_WRAP)); 
	auto settings33((zLinearLayout*)settings32->attach(new zLinearLayout(styles_default, 0, true), 
			VIEW_MATCH, VIEW_MATCH)); 
	auto settings114((zLinearLayout*)settings33->attach(new zLinearLayout(styles_zlinearlayout3, 0, false), 
			VIEW_MATCH, VIEW_WRAP)); 
	settings114->attach(new zViewText(styles_default, 0, R.string.Palettes_), 
			VIEW_WRAP, VIEW_WRAP);
	settings114->attach(new zViewSelect(styles_z_spinner, R.id.dispSpinPalettes), 
			VIEW_MATCH, VIEW_WRAP);
	settings114->attach(new zViewCheck(styles_browschk, R.id.dispChkAsm, R.string.Assembler_colors), 
			VIEW_MATCH, VIEW_WRAP);
	auto settings124((zLinearLayout*)settings33->attach(new zLinearLayout(styles_default, R.id.llSpeccyCols1, false), 
			VIEW_MATCH, VIEW_MATCH)); 
	settings124->attach(new zViewText(styles_disptextbl, R.id.dispTextBl, R.string.black), 
			VIEW_MATCH, VIEW_MATCH);
	settings124->attach(new zViewText(styles_disptextb, R.id.dispTextB, R.string.blue), 
			VIEW_MATCH, VIEW_MATCH);
	settings124->attach(new zViewText(styles_disptextr, R.id.dispTextR, R.string.red), 
			VIEW_MATCH, VIEW_MATCH);
	settings124->attach(new zViewText(styles_disptextm, R.id.dispTextM, R.string.purple), 
			VIEW_MATCH, VIEW_MATCH);
	settings124->attach(new zViewText(styles_disptextg, R.id.dispTextG, R.string.green), 
			VIEW_MATCH, VIEW_MATCH);
	settings124->attach(new zViewText(styles_disptextc, R.id.dispTextC, R.string.cyan), 
			VIEW_MATCH, VIEW_MATCH);
	settings124->attach(new zViewText(styles_disptexty, R.id.dispTextY, R.string.yellow), 
			VIEW_MATCH, VIEW_MATCH);
	settings124->attach(new zViewText(styles_disptextw, R.id.dispTextW, R.string.white), 
			VIEW_MATCH, VIEW_MATCH);
	auto settings134((zLinearLayout*)settings33->attach(new zLinearLayout(styles_default, R.id.llSpeccyCols2, false), 
			VIEW_MATCH, VIEW_MATCH)); 
	settings134->attach(new zViewText(styles_disptextbl, R.id.dispTextBrBl, R.string.bright_black), 
			VIEW_MATCH, VIEW_MATCH);
	settings134->attach(new zViewText(styles_disptextb, R.id.dispTextBrB, R.string.bright_blue), 
			VIEW_MATCH, VIEW_MATCH);
	settings134->attach(new zViewText(styles_disptextr, R.id.dispTextBrR, R.string.bright_red), 
			VIEW_MATCH, VIEW_MATCH);
	settings134->attach(new zViewText(styles_disptextm, R.id.dispTextBrM, R.string.bright_purple), 
			VIEW_MATCH, VIEW_MATCH);
	settings134->attach(new zViewText(styles_disptextg, R.id.dispTextBrG, R.string.bright_green), 
			VIEW_MATCH, VIEW_MATCH);
	settings134->attach(new zViewText(styles_disptextc, R.id.dispTextBrC, R.string.bright_cyan), 
			VIEW_MATCH, VIEW_MATCH);
	settings134->attach(new zViewText(styles_disptexty, R.id.dispTextBrY, R.string.bright_yellow), 
			VIEW_MATCH, VIEW_MATCH);
	settings134->attach(new zViewText(styles_disptextw, R.id.dispTextBrW, R.string.bright_white), 
			VIEW_MATCH, VIEW_MATCH);
	auto settings144((zLinearLayout*)settings33->attach(new zLinearLayout(styles_ll8, R.id.llAsmCols1, false), 
			VIEW_MATCH, VIEW_MATCH)); 
	settings144->attach(new zViewText(styles_disptextbl, R.id.dispTextBkg, R.string.backgrnd), 
			VIEW_MATCH, VIEW_MATCH);
	settings144->attach(new zViewText(styles_disptextr, R.id.dispTextSels, R.string.selection), 
			VIEW_MATCH, VIEW_MATCH);
	settings144->attach(new zViewText(styles_disptextb, R.id.dispTextText, R.string.text), 
			VIEW_MATCH, VIEW_MATCH);
	settings144->attach(new zViewText(styles_disptextm, R.id.dispTextNumber, R.string.number), 
			VIEW_MATCH, VIEW_MATCH);
	settings144->attach(new zViewText(styles_disptextg, R.id.dispTextStrs, R.string.string), 
			VIEW_MATCH, VIEW_MATCH);
	settings144->attach(new zViewText(styles_disptextc, R.id.dispTextSplits, R.string.splitters), 
			VIEW_MATCH, VIEW_MATCH);
	settings144->attach(new zViewText(styles_disptexty, R.id.dispTextComment, R.string.comment), 
			VIEW_MATCH, VIEW_MATCH);
	settings144->attach(new zViewText(styles_disptextw, R.id.dispTextOps, R.string.operators), 
			VIEW_MATCH, VIEW_MATCH);
	auto settings154((zLinearLayout*)settings33->attach(new zLinearLayout(styles_ll8, R.id.llAsmCols2, false), 
			VIEW_MATCH, VIEW_MATCH)); 
	settings154->attach(new zViewText(styles_disptextbl, R.id.dispTextCmds, R.string.commands), 
			VIEW_MATCH, VIEW_MATCH);
	settings154->attach(new zViewText(styles_disptextb, R.id.dispTextReg, R.string.registers), 
			VIEW_MATCH, VIEW_MATCH);
	settings154->attach(new zViewText(styles_disptextr, R.id.dispTextFlags, R.string.flags), 
			VIEW_MATCH, VIEW_MATCH);
	settings154->attach(new zViewText(styles_disptextm, R.id.dispTextLabels, R.string.labels), 
			VIEW_MATCH, VIEW_MATCH);
	settings154->attach(new zViewText(styles_disptextg, R.id.dispTextBkgLines, R.string.bkgrnd_lines), 
			VIEW_MATCH, VIEW_MATCH);
	settings154->attach(new zViewText(styles_disptextc, R.id.dispTextNumLines, R.string.number_lines), 
			VIEW_MATCH, VIEW_MATCH);
	settings154->attach(new zViewText(styles_disptexty, R.id.dispTextCurLine, R.string.current_lines), 
			VIEW_MATCH, VIEW_MATCH);
	settings154->attach(new zViewText(styles_disptextw, R.id.dispTextNulls, 0), 
			VIEW_MATCH, VIEW_MATCH);
	settings33->attach(new zViewSlider(styles_mainsystem, R.id.dispSlrR, 0, szi(0, 255), 0, false), 
			VIEW_MATCH, VIEW_MATCH);
	settings33->attach(new zViewSlider(styles_mainsystem, R.id.dispSlrG, 0, szi(0, 255), 0, false), 
			VIEW_MATCH, VIEW_MATCH);
	settings33->attach(new zViewSlider(styles_mainsystem, R.id.dispSlrB, 0, szi(0, 255), 0, false), 
			VIEW_MATCH, VIEW_MATCH);
	auto settings42((zTabWidget*)settings01->attach(new zTabWidget(styles_default, 0, 0, R.integer.iconZxCasette), 
			VIEW_WRAP, VIEW_WRAP)); 
	settings42->attach(new zViewRibbon(styles_bplist, R.id.casetList, false), 
			VIEW_MATCH, VIEW_MATCH);
	auto settings52((zTabWidget*)settings01->attach(new zTabWidget(styles_default, 0, 0, R.integer.iconZxDisk), 
			VIEW_WRAP, VIEW_WRAP)); 
	auto settings43((zLinearLayout*)settings52->attach(new zLinearLayout(styles_default, 0, true), 
			VIEW_MATCH, VIEW_MATCH)); 
	auto settings164((zLinearLayout*)settings43->attach(new zLinearLayout(styles_zlinearlayout3, 0, false), 
			VIEW_MATCH, VIEW_WRAP)); 
	settings164->attach(new zViewSelect(styles_diskspindisk, R.id.diskSpinDisk), 
			0, 1, VIEW_MATCH, VIEW_WRAP);
	settings164->attach(new zViewText(styles_default, R.id.diskTextFile, R.string.file_txt), 
			0, 3, VIEW_MATCH, VIEW_WRAP);
	settings164->attach(new zViewCheck(styles_browschk, R.id.diskChkReadOnly, R.string.Read_only), 
			0, 2, VIEW_MATCH, VIEW_WRAP);
	auto settings174((zLinearLayout*)settings43->attach(new zLinearLayout(styles_default, 0, false), 
			VIEW_MATCH, VIEW_WRAP)); 
	settings174->attach(new zViewText(styles_zviewtext1, 0, R.string.Name), 
			VIEW_MATCH, VIEW_WRAP);
	settings174->attach(new zViewText(styles_zviewtext1, 0, R.string.Type), 
			VIEW_MATCH, VIEW_WRAP);
	settings174->attach(new zViewText(styles_zviewtext1, 0, R.string.Address), 
			VIEW_MATCH, VIEW_WRAP);
	settings174->attach(new zViewText(styles_zviewtext1, 0, R.string.Size), 
			VIEW_MATCH, VIEW_WRAP);
	settings174->attach(new zViewText(styles_zviewtext1, 0, R.string._Sectors), 
			VIEW_MATCH, VIEW_WRAP);
	settings174->attach(new zViewText(styles_zviewtext1, 0, R.string.Sector), 
			VIEW_MATCH, VIEW_WRAP);
	settings174->attach(new zViewText(styles_zviewtext1, 0, R.string.Track), 
			VIEW_MATCH, VIEW_WRAP);
	settings43->attach(new zViewRibbon(styles_bplist, R.id.diskList, true), 
			VIEW_MATCH, VIEW_MATCH);
	auto settings62((zTabWidget*)settings01->attach(new zTabWidget(styles_default, 0, 0, R.integer.iconZxHelp), 
			VIEW_WRAP, VIEW_WRAP)); 
	settings62->attach(new zViewText(styles_helptext, R.id.helpText, 0), 
			VIEW_MATCH, VIEW_MATCH);
