
#pragma once

static zStyle styles_zxactionbar[] = {
	{ Z_STYLES, z.R.style.bar },
	{ Z_BACKGROUND, R.drawable.zx_icons },
	{ Z_BACKGROUND_TILES, R.integer.zxBar },
	{ Z_BACKGROUND_COLOR | ZT_END, 0xFFFFFFFF }
};

static zStyle styles_debbut[] = {
	{ Z_STYLES, z.R.style.tools },
	{ Z_SIZE, 0x05000500 },
	{ Z_FOREGROUND_SCALE, 27524 },
	{ Z_ICON_SCALE, 21626 },
	{ Z_TEXT_FOREGROUND_COLOR, 0xFFFFFFFF },
	{ Z_TEXT_SIZE, 14 },
	{ Z_TEXT_STYLE, ZS_TEXT_BOLD },
	{ Z_ICON | ZT_THEME, Z_THEME_ICONS },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_PADDING | ZT_END, 0x00000000 }
};

static zStyle styles_debflags[] = {
	{ Z_SIZE, 0x01000000 },
	{ Z_TEXT_SIZE, 11 },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_TEXT_FOREGROUND_COLOR, 0xFFFFFFFF },
	{ Z_IPADDING | ZT_END, 0x02010201 }
};

static zStyle styles_debform[] = {
	{ Z_STYLES, z.R.style.form },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_MODE, ZS_FORM_NONE },
	{ Z_MASK_TILES | ZT_END, z.R.integer.rect }
};

static zStyle styles_debedit[] = {
	{ Z_SIZE, 0x0C000C00 },
	{ Z_FOREGROUND | ZT_THEME, Z_THEME_COLOR },
	{ Z_FOREGROUND_SCALE, 16383 },
	{ Z_TEXT_DISTANCE, 0 },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_PADDING, 0x01010101 },
	{ Z_IPADDING, 0x01010101 },
	{ Z_MODE, ZS_EDIT_ZX_DATA },
	{ Z_TEXT_LINES, 1 },
	{ Z_TEXT_SIZE, 15 },
	{ Z_TEXT_FOREGROUND_COLOR, 0xFF000000 },
	{ Z_GRAVITY | ZT_END, ZS_GRAVITY_CENTER }
};

static zStyle styles_debeditread[] = {
	{ Z_STYLES, R.style.debEdit },
	{ Z_BEHAVIOR, 0 },
	{ Z_DECORATE | ZT_END, ZS_READ_ONLY }
};

static zStyle styles_zxslider[] = {
	{ Z_STYLES, z.R.style.slider },
	{ Z_PADDING, 0x02020202 },
	{ Z_SIZE | ZT_END, 0x00000500 }
};

static zStyle styles_zxcheck[] = {
	{ Z_STYLES, z.R.style.checkbox },
	{ Z_FOREGROUND_SCALE, 36044 },
	{ Z_PADDING, 0x01010101 },
	{ Z_TEXT_DISTANCE, 3 },
	{ Z_TEXT_SIZE, 14 },
	{ Z_TEXT_STYLE, ZS_TEXT_BOLD },
	{ Z_TEXT_FOREGROUND_COLOR | ZT_END, 0xFFFFFFFF }
};

static zStyle styles_zxkeyb[] = {
	{ Z_STYLES, z.R.style.keyboardBase },
	{ Z_FOREGROUND_SCALE, 32767 },
	{ Z_SIZE, 0x01000000 },
	{ Z_ICON | ZT_THEME | ZT_END, Z_THEME_ICONS }
};

static zStyle styles_setribbon[] = {
	{ Z_FBO, true },
	{ Z_BACKGROUND_TILES, z.R.integer.nullDrop },
	{ Z_SELECTOR | ZT_THEME, Z_THEME_COLOR_SELECTOR },
	{ Z_DURATION, 30 },
	{ Z_DECORATE, ZS_ALWAYS_SEL | ZS_SCROLLBAR },
	{ Z_SCROLLBAR_FADE, false },
	{ Z_SCROLLBAR_TILES, z.R.integer.rect },
	{ Z_PADDING | ZT_END, 0x05050505 }
};

static zStyle styles_diskheadtext[] = {
	{ Z_TEXT_SIZE, 15 },
	{ Z_TEXT_STYLE, ZS_TEXT_BOLD },
	{ Z_TEXT_FOREGROUND_COLOR, 0xFFFFFFFF },
	{ Z_GRAVITY | ZT_END, ZS_GRAVITY_CENTER }
};

static zStyle styles_dispcolortext[] = {
	{ Z_TEXT_SHADOW_OFFS, 0x00020002 },
	{ Z_TEXT_SIZE, 12 },
	{ Z_TEXT_LINES, 2 },
	{ Z_TEXT_STYLE, ZS_TEXT_NORMAL },
	{ Z_TEXT_FOREGROUND_COLOR, 0xFFFFFFFF },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_MARGINS, 0x01010101 },
	{ Z_BEHAVIOR | ZT_END, ZS_CLICKABLE }
};

static zStyle styles_bpedit[] = {
	{ Z_FOREGROUND_TILES, z.R.integer.edit },
	{ Z_FOREGROUND_SCALE, 36044 },
	{ Z_ICON_GRAVITY, ZS_GRAVITY_END | ZS_GRAVITY_VCENTER },
	{ Z_ICON_TILES, z.R.integer.iconEditEx },
	{ Z_ICON_SCALE, 39321 },
	{ Z_TEXT_DISTANCE, 1 },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_PADDING, 0x01010101 },
	{ Z_IPADDING, 0x03010301 },
	{ Z_MODE, ZS_EDIT_ZX_DATA },
	{ Z_TEXT_LINES, 1 },
	{ Z_TEXT_SIZE, 16 },
	{ Z_GRAVITY | ZT_END, ZS_GRAVITY_START | ZS_GRAVITY_VCENTER }
};

static zStyle styles_zxtools[] = {
	{ Z_STYLES, z.R.style.tools },
	{ Z_SIZE, 0x05000500 },
	{ Z_FOREGROUND_SCALE, 21626 },
	{ Z_ICON_SCALE, 21626 },
	{ Z_TEXT_FOREGROUND_COLOR, 0xFFFFFFFF },
	{ Z_ICON | ZT_THEME, Z_THEME_ICONS },
	{ Z_PADDING | ZT_END, 0x02020202 }
};

static zStyle styles_pokbutton[] = {
	{ Z_STYLES, z.R.style.button },
	{ Z_FOREGROUND_SCALE, 21626 },
	{ Z_ICON | ZT_THEME | ZT_END, Z_THEME_ICONS }
};

static zStyle styles_pokribbon[] = {
	{ Z_STYLES, R.style.setRibbon },
	{ Z_DECORATE | ZT_END, ZS_SCROLLBAR | ZS_OVERSCROLL }
};

static zStyle styles_deblisttext[] = {
	{ Z_TEXT_FONT, z.R.drawable.font_mono },
	{ Z_TEXT_SIZE, 16 },
	{ Z_TEXT_STYLE, ZS_TEXT_NORMAL | ZS_TEXT_MONO },
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_THEME_COLOR },
	{ Z_GRAVITY, ZS_GRAVITY_START },
	{ Z_IPADDING | ZT_END, 0x00020002 }
};

static zStyle styles_browslisttext[] = {
	{ Z_TEXT_SIZE, 20 },
	{ Z_TEXT_STYLE, ZS_TEXT_NORMAL },
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_THEME_COLOR },
	{ Z_GRAVITY, ZS_GRAVITY_START },
	{ Z_PADDING, 0x01010101 },
	{ Z_TEXT_ELLIPSIS | ZT_END, ZS_ELLIPSIS_END }
};

static zStyle styles_textcapt[] = {
	{ Z_TEXT_SIZE, 16 },
	{ Z_TEXT_ELLIPSIS, ZS_ELLIPSIS_MIDDLE },
	{ Z_TEXT_LINES, 2 },
	{ Z_TEXT_STYLE, ZS_TEXT_BOLD },
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_THEME_COLOR_TEXT_TEXT },
	{ Z_GRAVITY | ZT_END, ZS_GRAVITY_START }
};

static zStyle styles_browsbut[] = {
	{ Z_STYLES, z.R.style.tools },
	{ Z_ICON | ZT_THEME, Z_THEME_ICONS },
	{ Z_PADDING | ZT_END, 0x02020202 }
};

static zStyle styles_mythemedark[] = {
	{ Z_STYLES, z.R.style.themeDark },
	{ Z_THEME_ICONS, R.drawable.zx_icons },
	{ Z_THEME_ACTION_BAR | ZT_END, R.style.zxActionBar }
};

static zStyle styles_mythemelight[] = {
	{ Z_STYLES, z.R.style.themeLight },
	{ Z_THEME_ICONS, R.drawable.zx_icons },
	{ Z_THEME_ACTION_BAR | ZT_END, R.style.zxActionBar }
};

static zStyle styles_formasm[] = {
	{ Z_STYLES, z.R.style.form },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_MODE | ZT_END, ZS_FORM_OK }
};

static zStyle styles_zlinearlayout1[] = {
	{ Z_PADDING | ZT_END, 0x03030303 }
};

static zStyle styles_zviewform1[] = {
	{ Z_STYLES, z.R.style.form },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_MODE | ZT_END, ZS_FORM_NONE }
};

static zStyle styles_bpeditrange1[] = {
	{ Z_STYLES | ZT_END, R.style.bpEdit }
};

static zStyle styles_bpspinops[] = {
	{ Z_ADAPTER_DATA, R.string.bp_ops },
	{ Z_STYLES | ZT_END, z.R.style.spinner }
};

static zStyle styles_bpspincond[] = {
	{ Z_ADAPTER_DATA, R.string.bp_cond },
	{ Z_STYLES | ZT_END, z.R.style.spinner }
};

static zStyle styles_zviewtext1[] = {
	{ Z_STYLES | ZT_END, R.style.diskHeadText }
};

static zStyle styles_bplist[] = {
	{ Z_STYLES | ZT_END, R.style.setRibbon }
};

static zStyle styles_browsspin[] = {
	{ Z_ADAPTER_DATA, R.string.browser_flt },
	{ Z_STYLES | ZT_END, z.R.style.spinner }
};

static zStyle styles_browsdisk[] = {
	{ Z_STYLES, R.style.diskHeadText },
	{ Z_GRAVITY | ZT_END, ZS_GRAVITY_START | ZS_GRAVITY_VCENTER }
};

static zStyle styles_browsedit[] = {
	{ Z_STYLES, R.style.bpEdit },
	{ Z_MODE | ZT_END, ZS_EDIT_TEXT }
};

static zStyle styles_browsopen[] = {
	{ Z_STYLES | ZT_END, R.style.browsBut }
};

static zStyle styles_browschk[] = {
	{ Z_STYLES | ZT_END, R.style.zxCheck }
};

static zStyle styles_zviewform2[] = {
	{ Z_STYLES | ZT_END, R.style.debForm }
};

static zStyle styles_zlinearlayout2[] = {
	{ Z_PADDING | ZT_END, 0x01010101 }
};

static zStyle styles_zlinearlayout3[] = {
	{ Z_GRAVITY | ZT_END, ZS_GRAVITY_VCENTER }
};

static zStyle styles_butbp[] = {
	{ Z_STYLES | ZT_END, R.style.debBut }
};

static zStyle styles_editasm[] = {
	{ Z_STYLES, R.style.debEdit },
	{ Z_MODE | ZT_END, ZS_EDIT_TEXT }
};

static zStyle styles_textfs[] = {
	{ Z_STYLES | ZT_END, R.style.debFlags }
};

static zStyle styles_editpc[] = {
	{ Z_STYLES | ZT_END, R.style.debEdit }
};

static zStyle styles_editts[] = {
	{ Z_STYLES | ZT_END, R.style.debEditRead }
};

static zStyle styles_zlinearlayout7[] = {
	{ Z_GRAVITY | ZT_END, ZS_GRAVITY_HCENTER }
};

static zStyle styles_ll8[] = {
	{ Z_VISIBLED | ZT_END, false }
};

static zStyle styles_llgs[] = {
	{ Z_VISIBLED, false },
	{ Z_GRAVITY | ZT_END, ZS_GRAVITY_VCENTER }
};

static zStyle styles_zviewtext2[] = {
	{ Z_STYLES | ZT_END, R.style.textCapt }
};

static zStyle styles_sellist[] = {
	{ Z_STYLES | ZT_END, R.style.browsListText }
};

static zStyle styles_zviewtext3[] = {
	{ Z_TEXT_STYLE | ZT_END, ZS_TEXT_BOLD }
};

static zStyle styles_mainsystem[] = {
	{ Z_STYLES | ZT_END, R.style.zxSlider }
};

static zStyle styles_zlinearlayout5[] = {
	{ Z_GRAVITY | ZT_END, ZS_GRAVITY_CENTER }
};

static zStyle styles_soundspinchip[] = {
	{ Z_ADAPTER_DATA, R.string.AY_YM },
	{ Z_STYLES, z.R.style.spinner },
	{ Z_SIZE | ZT_END, 0x00000A00 }
};

static zStyle styles_soundspinchannels[] = {
	{ Z_ADAPTER_DATA, R.string.MONO_ABC_ACB_BAC_BCA_CAB_CBA },
	{ Z_STYLES | ZT_END, z.R.style.spinner }
};

static zStyle styles_soundspinfreq[] = {
	{ Z_ADAPTER_DATA, R.string._48000_44100_22050 },
	{ Z_STYLES | ZT_END, z.R.style.spinner }
};

static zStyle styles_joyspinlyt[] = {
	{ Z_ADAPTER_DATA, R.string.joy_names },
	{ Z_STYLES | ZT_END, z.R.style.spinner }
};

static zStyle styles_joyspinleft[] = {
	{ Z_ADAPTER_DATA, R.string.key_names },
	{ Z_STYLES | ZT_END, z.R.style.spinner }
};

static zStyle styles_disptextbl[] = {
	{ Z_BACKGROUND, 0xFF000000 },
	{ Z_STYLES | ZT_END, R.style.dispColorText }
};

static zStyle styles_disptextb[] = {
	{ Z_BACKGROUND, 0xFF0000FF },
	{ Z_STYLES | ZT_END, R.style.dispColorText }
};

static zStyle styles_disptextr[] = {
	{ Z_BACKGROUND, 0xFFFF0000 },
	{ Z_STYLES | ZT_END, R.style.dispColorText }
};

static zStyle styles_disptextm[] = {
	{ Z_BACKGROUND, 0xFFFF00FF },
	{ Z_STYLES | ZT_END, R.style.dispColorText }
};

static zStyle styles_disptextg[] = {
	{ Z_BACKGROUND, 0xFF00FF00 },
	{ Z_STYLES | ZT_END, R.style.dispColorText }
};

static zStyle styles_disptextc[] = {
	{ Z_BACKGROUND, 0xFF00FFFF },
	{ Z_STYLES | ZT_END, R.style.dispColorText }
};

static zStyle styles_disptexty[] = {
	{ Z_BACKGROUND, 0xFFFFFF00 },
	{ Z_STYLES | ZT_END, R.style.dispColorText }
};

static zStyle styles_disptextw[] = {
	{ Z_BACKGROUND, 0xFFFFFFFF },
	{ Z_STYLES | ZT_END, R.style.dispColorText }
};

static zStyle styles_diskspindisk[] = {
	{ Z_ADAPTER_DATA, R.string.disks },
	{ Z_STYLES | ZT_END, z.R.style.spinner }
};

static zStyle styles_helptext[] = {
	{ Z_ASSET | ZT_END, R.string.help_index_html }
};

static zStyle styles_formtzx[] = {
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_MODE, ZS_FORM_NONE },
	{ Z_BEHAVIOR | ZT_END, 0 }
};

static zStyle styles_tzxmsg[] = {
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_TEXT_LINES, 2 },
	{ Z_TEXT_FOREGROUND_COLOR, 0xFFFFFFFF },
	{ Z_TEXT_SIZE, 24 },
	{ Z_VISIBLED, false },
	{ Z_TEXT_SHADOW_OFFS, 0x00020002 },
	{ Z_BEHAVIOR | ZT_END, 0 }
};

static zStyle styles_speccy[] = {
	{ Z_BEHAVIOR, ZS_CLICKABLE | ZS_LCLICKABLE },
	{ Z_FOREGROUND_COLOR, 0xFFFFFFFF },
	{ Z_FOREGROUND_TILES | ZT_END, 0 }
};

static zStyle styles_speccyfps[] = {
	{ Z_GRAVITY, ZS_GRAVITY_END },
	{ Z_TEXT_FOREGROUND_COLOR, 0xFFFFFFFF },
	{ Z_TEXT_SIZE, 28 },
	{ Z_TEXT_SHADOW_OFFS, 0x00020002 },
	{ Z_BEHAVIOR | ZT_END, 0 }
};

static zStyle styles_speccystatus[] = {
	{ Z_BEHAVIOR, 0 },
	{ Z_STYLES | ZT_END, z.R.style.menuImage }
};

