// width and height of the grid in pixels
#define WMGRID_WIDTH 10
#define WMGRID_HEIGHT 10

#include "control.c"
#include <X11/keysymdef.h>

// 1, 2, 3 associate with left, middle, and right click respectively
#define FOCUS_BUTTON 1

// See /usr/include/X11/keysymdef.h for a list of keysyms
// Key and button actions can support keybinds of up to MAX_KEYS keys
// Use {0} for no keybindings

#define MAX_KEYS 10

const KeySym MOD_KEY = XK_Super_L;

// Available modes: MODE_MOVE, MODE_RESIZE
static ButtonAction button_actions[] =
{
	/* mode			button 		keysyms*/
	{ MODE_MOVE,		1,	 	{0}},
	{ MODE_RESIZE,		3,	 	{0}}
};

// Available functions: close_win, move_px, resize_px, grid_align, grid_move, grid_resize
KeyAction key_actions[] =
{
	/* function		type		value		keysyms */
	{ close_win,		0,		0,		{XK_x} },
	{ grid_align,		0,		0,		{XK_a} },
	{ resize_px,		ARG_LEFT,	5,		{XK_Left, XK_Shift_L} },
	{ resize_px,		ARG_RIGHT,	5,		{XK_Right, XK_Shift_L} },
	{ resize_px,		ARG_UP,		5,		{XK_Up, XK_Shift_L} },
	{ resize_px,		ARG_DOWN,	5,		{XK_Down, XK_Shift_L} },
	{ move_px,		ARG_LEFT,	5,		{XK_Left} },
	{ move_px,		ARG_RIGHT,	5,		{XK_Right} },
	{ move_px,		ARG_UP,		5,		{XK_Up} },
	{ move_px,		ARG_DOWN,	5,		{XK_Down} }
};

// Make sure the command has an "&" at the end of it or else the window manager will hang
// until the command terminates.
static CommandAction command_actions[] =
{
	/* command				keysyms */
	{ "urxvt &",				{XK_Return}},
	{ "scrot &",				{XK_s}},
	{ "dmenu_run &",			{XK_space}}
};
