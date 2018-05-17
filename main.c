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
	
	// vars for binding queues
	int n;
	unsigned int k;
	Bind active_binds[sizeof(key_actions)/sizeof(KeyAction)+sizeof(command_actions)/sizeof(CommandAction)];
	Bind * max_key_bind;
	PointerMode active_modes[sizeof(button_actions)/sizeof(ButtonAction)];
	PointerMode * max_pointer_mode;
	Bool cursor_activated = False;

	LinkedList * pressed_keys = NULL;

	if( (display=XOpenDisplay(NULL)) == NULL )
	{
		(void) fprintf(stderr, "spwm: Cannot connect to X server %s\n",
				XDisplayName(NULL));
		exit(-1);	
	}

	screen_num = DefaultScreen(display);
	root_win = RootWindow(display, screen_num);
	focus_win = root_win;
		
	
	XGrabButton(display, AnyButton, AnyModifier, root_win, True,
			ButtonPressMask|ButtonReleaseMask|PointerMotionMask,
			GrabModeAsync, GrabModeAsync,
			root_win,
			None);
	
	XSelectInput(display, focus_win, ButtonPressMask|ButtonReleaseMask|PointerMotionMask);
	XGrabKey(display, XKeysymToKeycode(display, MOD_KEY),
			AnyModifier, root_win, True,
			GrabModeAsync, GrabModeAsync);
	/*
	XGrabPointer(display, root_win, False,
			ButtonPressMask|ButtonReleaseMask|PointerMotionMask|EnterWindowMask,
			GrabModeAsync, GrabModeAsync,
			root_win,
			None, CurrentTime);
	*/
	for(;;)
	{
		XNextEvent(display, &event);
	
		
		switch(event.type)
		{
			case KeyPress:
				printf("key pressed\n");
				list_keys_pressed(pressed_keys);
				if(event.xkey.keycode == XKeysymToKeycode(display, MOD_KEY))
				{
					printf("Mod Key Pressed\n");
					XGrabKeyboard(display, root_win, True,
							GrabModeAsync, GrabModeAsync,
							CurrentTime);
					
					mod_pressed = True;
				}
				pressed_keys = press_key(KeycodeToKeysym(display, event.xkey.keycode, 0), pressed_keys);
				
				if(mod_pressed)
				{
					k=0;
					max_key_bind = NULL;
					for(unsigned int i=0; i<sizeof(key_actions)/sizeof(KeyAction); i++)
					{
						if(focus_win == root_win) break;

						if(all_keys_pressed(key_actions[i].keysyms, pressed_keys))
						{
							n=0;
							for(int j=0; j<MAX_KEYS && key_actions[i].keysyms[j] != 0; j++) n++;

							active_binds[k].keyaction = &key_actions[i];
						        active_binds[k].commandaction = NULL;
							active_binds[k].n_keys = n;
							k++;
						}
					}
					for(unsigned int i=0; i<sizeof(command_actions)/sizeof(CommandAction); i++)
					{
						if(all_keys_pressed(command_actions[i].keysyms, pressed_keys))
						{
							n=0;
							for(int j=0; j<MAX_KEYS && command_actions[i].keysyms[j] != 0; j++) n++;

							active_binds[k].keyaction = NULL;
							active_binds[k].commandaction = &command_actions[i];
							active_binds[k].n_keys = n;
							k++;
						}
					}
					for(unsigned int i=0; i<k; i++)
					{
						if(max_key_bind == NULL) max_key_bind = &active_binds[i];
						if(max_key_bind->n_keys < active_binds[i].n_keys)
						{
							max_key_bind = &active_binds[i];
						}
					}
					if(max_key_bind != NULL && max_key_bind->keyaction == NULL)
					{
						system(max_key_bind->commandaction->command);
					}
					else if(max_key_bind != NULL && max_key_bind->commandaction == NULL)
					{
						printf("Running command with %d keys\n", max_key_bind->n_keys);
						max_key_bind->keyaction->func(
								max_key_bind->keyaction->arg_type,
								max_key_bind->keyaction->arg_val,
								display, focus_win);
					}
				}
				break;

			case KeyRelease:
				pressed_keys = release_key(KeycodeToKeysym(display, event.xkey.keycode, 0), pressed_keys);
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
					cursor_activated = False;
					max_pointer_mode = NULL;
					k=0;
					if(event.xbutton.button == FOCUS_BUTTON && !mod_pressed && event.xbutton.subwindow)
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
							n=0;
							for(unsigned int j=0; j<MAX_KEYS && button_actions[i].keysyms[j] !=0; j++) n++;
							active_modes[k].type = button_actions[i].mode_switch;
							active_modes[k].n_keys = n;
							k++;

							if(!cursor_activated)
							{
								init_press = event.xbutton;
								XGetWindowAttributes(display, init_press.subwindow, &attr);
								cursor_activated = True;
							}	
						}
					}
					for(unsigned int i=0; i<k; i++)
					{
						if(max_pointer_mode == NULL) max_pointer_mode = &active_modes[i];
						if(max_pointer_mode->n_keys < active_modes[i].n_keys)
						{
							max_pointer_mode = &active_modes[i];
						}
					}
					if(max_pointer_mode != NULL) wmmode = max_pointer_mode->type;
				}
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

			case MODE_GRID_MOVE:
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
							((attr.x+dx)/WMGRID_WIDTH)*WMGRID_WIDTH,
							((attr.y+dy)/WMGRID_HEIGHT)*WMGRID_HEIGHT,
							attr.width,
							attr.height);
				}
				break;

			case MODE_GRID_RESIZE:
				if(event.type == ButtonRelease)
				{
					wmmode = MODE_DEFAULT;
					break;
				}
				if(event.type == MotionNotify)
				{
					dx = ((event.xbutton.x_root - init_press.x_root)/WMGRID_WIDTH)*WMGRID_WIDTH;
					dy = ((event.xbutton.y_root - init_press.y_root)/WMGRID_HEIGHT)*WMGRID_HEIGHT;

					XMoveResizeWindow(display, init_press.subwindow,
							attr.x,
							attr.y,
							max(1, attr.width+dx),
							max(1, attr.height+dy));
				}
				break;

		}
		//XSendEvent(display, event.xbutton.subwindow, True, 
		//		ButtonPressMask|ButtonReleaseMask|PointerMotionMask, &event);
		XAllowEvents(display, ReplayPointer, CurrentTime);
	}
	return 1;

}
