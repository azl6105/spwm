void close_win(int type, int val, Display * display, Window focus)
{
	printf("Closing window\n");
	XDestroyWindow(display, focus);
}

void move_px(int dir, int px, Display * display, Window focus)
{
	printf("Moving window\n");

	XWindowAttributes attr;
	XGetWindowAttributes(display, focus, &attr);

	switch(dir)
	{
		case ARG_LEFT:
			XMoveResizeWindow(display, focus,
					attr.x - px,
					attr.y,
					attr.width,
					attr.height);
			break;
		case ARG_RIGHT:
			XMoveResizeWindow(display, focus,
					attr.x + px,
					attr.y,
					attr.width,
					attr.height);
			break;
		case ARG_UP:
			XMoveResizeWindow(display, focus,
					attr.x,
					attr.y - px,
					attr.width,
					attr.height);
			break;
		case ARG_DOWN:
			XMoveResizeWindow(display, focus,
					attr.x,
					attr.y + px,
					attr.width,
					attr.height);
			break;
	}

}

void resize_px(int dir, int px, Display * display, Window focus)
{
	printf("Resizing window\n");

	XWindowAttributes attr;
	XGetWindowAttributes(display, focus, &attr);

	switch(dir)
	{
		case ARG_LEFT:
			XMoveResizeWindow(display, focus,
					attr.x,
					attr.y,
					max(1, attr.width - px),
					attr.height);
			break;
		case ARG_RIGHT:
			XMoveResizeWindow(display, focus,
					attr.x,
					attr.y,
					attr.width + px,
					attr.height);
			break;
		case ARG_UP:
			XMoveResizeWindow(display, focus,
					attr.x,
					attr.y,
					attr.width,
					max(1, attr.height - px));
			break;
		case ARG_DOWN:
			XMoveResizeWindow(display, focus,
					attr.x,
					attr.y,
					attr.width,
					attr.height + px);
			break;
	}

}

void grid_align(int type, int val, Display * display, Window focus)
{
	printf("Aligning window with grid\n");
	
	XWindowAttributes attr;
	XGetWindowAttributes(display, focus, &attr);

	XMoveResizeWindow(display, focus,
			(attr.x / WMGRID_WIDTH) * WMGRID_WIDTH,
			(attr.y / WMGRID_HEIGHT) * WMGRID_HEIGHT,
			(attr.width / WMGRID_WIDTH) * WMGRID_WIDTH,
			(attr.height / WMGRID_HEIGHT) * WMGRID_HEIGHT);

}

void grid_move(int dir, int spaces, Display * display, Window focus)
{
	XWindowAttributes attr;
	XGetWindowAttributes(display, focus, &attr);

	XMoveResizeWindow(display, focus,
			(attr.x / WMGRID_WIDTH) * WMGRID_WIDTH,
			(attr.y / WMGRID_HEIGHT) * WMGRID_HEIGHT,
			attr.width,
			attr.height);

	if(dir == ARG_LEFT || dir == ARG_RIGHT)
	{
		move_px(dir, WMGRID_WIDTH*spaces, display, focus);
		return;
	}
	if(dir == ARG_UP || dir == ARG_DOWN)
	{
		move_px(dir, WMGRID_HEIGHT*spaces, display, focus);
		return;
	}
}

void grid_resize(int dir, int spaces, Display * display, Window focus)
{
	XWindowAttributes attr;
	XGetWindowAttributes(display, focus, &attr);

	grid_align(0, 0, display, focus);
	
	if(dir == ARG_LEFT || dir == ARG_RIGHT)
	{
		resize_px(dir, WMGRID_WIDTH*spaces, display, focus);
		return;
	}
	if(dir == ARG_UP || dir == ARG_DOWN)
	{
		resize_px(dir, WMGRID_HEIGHT*spaces, display, focus);
		return;
	}
}
