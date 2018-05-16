#include "include.h"
#include "config.c"
#include "abstraction.c"

int main(void)
{

	Display * display;
	int screen_num;
	Window root_win;
	Window focus_win;
	XEvent event;
	uint8_t wmmode = MODE_DEFAULT;
	Bool mod_pressed = False;

	// vars for move and resize
	XButtonEvent init_press;
	XWindowAttributes attr;
	int dx;
	int dy;

	LinkedList * pressed_keys = NULL;

	if( (display=XOpenDisplay(NULL)) == NULL )
	{
		(void) fprintf(stderr, "miowm: Cannot connect to X server %s\n",
				XDisplayName(NULL));
		exit(-1);	
	}

	screen_num = DefaultScreen(display);
	root_win = RootWindow(display, screen_num);
	focus_win = root_win;
	
	XGrabPointer(display, root_win, False,
			ButtonPressMask|ButtonReleaseMask|PointerMotionMask,
			GrabModeAsync, GrabModeAsync,
			root_win,
			None, CurrentTime);
	/*
	XGrabButton(display, AnyButton, AnyModifier, root_win, True,
			ButtonPressMask|ButtonReleaseMask|PointerMotionMask,
			GrabModeAsync, GrabModeAsync,
			root_win,
			None);
	*/
	//XSelectInput(display, focus_win, ButtonPressMask|ButtonReleaseMask|ButtonMotionMask);
	XGrabKey(display, XKeysymToKeycode(display, MOD_KEY),
			AnyModifier, root_win, True,
			GrabModeAsync, GrabModeAsync);

	for(;;)
	{
		XNextEvent(display, &event);

		switch(event.type)
		{
			case KeyPress:
				printf("key pressed\n");
				if(event.xkey.keycode == XKeysymToKeycode(display, MOD_KEY))
				{
					printf("Mod Key Pressed\n");
					XGrabKeyboard(display, root_win, True,
							GrabModeAsync, GrabModeAsync,
							CurrentTime);
					
					mod_pressed = True;
				}
				press_key(KeycodeToKeysym(display, event.xkey.keycode, 0), pressed_keys);
				
				if(mod_pressed)
				{
					for(unsigned int i=0; i<sizeof(key_actions)/sizeof(KeyAction); i++)
					{
						if(focus_win == root_win) break;

						if(all_keys_pressed(key_actions[i].keysyms, pressed_keys))
						{
							printf("Keybind activated\n");
							key_actions[i].func(
									key_actions[i].arg_type,
									key_actions[i].arg_val,
									display,
									focus_win);
							break;
						}
					}
					for(unsigned int i=0; i<sizeof(command_actions)/sizeof(CommandAction); i++)
					{
						if(all_keys_pressed(command_actions[i].keysyms, pressed_keys))
						{
							printf("Running command\n");
							system(command_actions[i].command);
							break;
						}
					}
				}
				break;

			case KeyRelease:
				release_key(KeycodeToKeysym(display, event.xkey.keycode, 0), pressed_keys);
				if(event.xkey.keycode == XKeysymToKeycode(display, MOD_KEY))
				{
					printf("Mod Key Released\n");
					XUngrabKeyboard(display, CurrentTime);
					mod_pressed = False;
				}
				break;
		}

		switch(wmmode)
		{
			case MODE_DEFAULT:
				if(event.type == ButtonPress)
				{
					if(event.xbutton.button == FOCUS_BUTTON && !mod_pressed)
					{
						focus_win = event.xbutton.subwindow;
						XSetInputFocus(display, focus_win, RevertToNone, CurrentTime);
						XRaiseWindow(display, focus_win);
						break;
					}
					for(unsigned int i=0; i<sizeof(button_actions)/sizeof(ButtonAction); i++)
					{
						if(event.xbutton.button == button_actions[i].button &&
						   mod_pressed && 
						   event.xbutton.subwindow != None &&
						   all_keys_pressed(button_actions[i].keysyms, pressed_keys))
						{
							printf("Switching Pointer Mode\n");
							wmmode = button_actions[i].mode_switch;
							init_press = event.xbutton;
							XGetWindowAttributes(display, init_press.subwindow, &attr);
							break;
						}
					}
				}
				
			/*	
				XUngrabPointer(display, CurrentTime);
				
				XGrabPointer(display, event.xbutton.subwindow, False,
						ButtonPressMask|ButtonReleaseMask|PointerMotionMask,
						GrabModeAsync, GrabModeAsync,
						root_win,
						None, CurrentTime);
				*/
				break;

			case MODE_MOVE:
				if(event.type == ButtonRelease)
				{
					wmmode = MODE_DEFAULT;
					break;
				}
				if(event.type == MotionNotify)
				{
					dx = event.xbutton.x_root - init_press.x_root;
					dy = event.xbutton.y_root - init_press.y_root;
					
					XMoveResizeWindow(display, init_press.subwindow,
							attr.x + dx,
							attr.y + dy,
							attr.width,
							attr.height);
				}
				break;

			case MODE_RESIZE:
				if(event.type == ButtonRelease)
				{
					wmmode = MODE_DEFAULT;
					break;
				}
				if(event.type == MotionNotify)
				{
					dx = event.xbutton.x_root - init_press.x_root;
					dy = event.xbutton.y_root - init_press.y_root;
						
					XMoveResizeWindow(display, init_press.subwindow,
							attr.x,
							attr.y,
							max(1, attr.width + dx),
							max(1, attr.height + dy));
				}
				break;

		}
	}
	return 1;

}
