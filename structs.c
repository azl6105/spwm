typedef struct
{
	void (*func)(int, int, Display*, Window);
		// type, value, display, focus;
	unsigned int arg_type;
	unsigned int arg_val;
	KeySym keysyms[10];
} KeyAction;

typedef struct
{
	uint8_t mode_switch;
	unsigned int button;
	KeySym keysyms[10];
} ButtonAction;

typedef struct
{
	char * command;
	KeySym keysyms[10];
} CommandAction;

typedef struct LinkedList
{
	KeySym key;
	struct LinkedList * next;
} LinkedList;

typedef struct
{
	KeyAction * keyaction;
	CommandAction * commandaction;
	unsigned int n_keys;
} Bind;

typedef struct
{
	int type;
	unsigned int n_keys;
} PointerMode;
