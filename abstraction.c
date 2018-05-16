Bool node_is_nil(LinkedList * node)
{
	if(node == NULL) return True;
	return (node->next == NULL);
}

Bool all_keys_pressed(KeySym keysyms[], LinkedList * pressed_keys)
{
	//printf("Checking if all keys are pressed\n");

	int n=0;
	
	for(unsigned int i=0; i<MAX_KEYS && keysyms[i] != 0; i++)
	{
		n++;
		printf("Checking for %s. n=%d\n", XKeysymToString(keysyms[i]), n);
	}
	printf("Checking for %d keys\n", n);
	
	LinkedList *elt, *tmp;
	LL_FOREACH_SAFE(pressed_keys, elt, tmp)
	{
		printf("iterating\n");
		for(unsigned int i=0; i<MAX_KEYS && keysyms[i] != 0; i++)
		{
			if(keysyms[i] == elt->key)
			{	
				n--;
				break;
			}
		}
		if(n==0) return True;
	}
	return (n==0);
}

int n_keys_pressed(LinkedList * pressed_keys)
{
	//for debugging
	if(pressed_keys->next==NULL) return 0;
	return 1+n_keys_pressed(pressed_keys->next);
}

void press_key(KeySym key, LinkedList * pressed_keys)
{
	LinkedList * elt;
	int count=0;
	LinkedList * add;
	add = (LinkedList *)malloc(sizeof *add);
	add->key = key;
	LL_APPEND(pressed_keys, add);
	LL_COUNT(pressed_keys, elt, count);
	printf("%d keys pressed\n", count);
}

void release_key(KeySym key, LinkedList * pressed_keys)
{
	LinkedList *elt, *tmp;
	LL_FOREACH_SAFE(pressed_keys, elt, tmp)
	{
		if(elt->key == key) LL_DELETE(pressed_keys, elt);
	}

}

KeySym EventKeysym(XEvent event)
{
	KeySym keysym;
	int count;
	char buffer[10];
	
	XLookupString(&event.xkey, buffer, 10, &keysym, NULL);

	return keysym;
}

// I just copied this code from Alex Barker on StackOverflow, sorry Alex.

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <X11/X.h>
#include <X11/XKBlib.h>

KeySym KeycodeToKeysym(Display * display, KeyCode keycode, unsigned int event_mask) {
    KeySym keysym = NoSymbol;

    //Get the map
    XkbDescPtr keyboard_map = XkbGetMap(display, XkbAllClientInfoMask, XkbUseCoreKbd);
    if (keyboard_map) {
        //What is diff between XkbKeyGroupInfo and XkbKeyNumGroups?
        unsigned char info = XkbKeyGroupInfo(keyboard_map, keycode);
        unsigned int num_groups = XkbKeyNumGroups(keyboard_map, keycode);

        //Get the group
        unsigned int group = 0x00;
        switch (XkbOutOfRangeGroupAction(info)) {
            case XkbRedirectIntoRange:
                /* If the RedirectIntoRange flag is set, the four least significant
                 * bits of the groups wrap control specify the index of a group to
                 * which all illegal groups correspond. If the specified group is
                 * also out of range, all illegal groups map to Group1.
                 */
                group = XkbOutOfRangeGroupInfo(info);
                if (group >= num_groups) {
                    group = 0;
                }
            break;

            case XkbClampIntoRange:
                /* If the ClampIntoRange flag is set, out-of-range groups correspond
                 * to the nearest legal group. Effective groups larger than the
                 * highest supported group are mapped to the highest supported group;
                 * effective groups less than Group1 are mapped to Group1 . For
                 * example, a key with two groups of symbols uses Group2 type and
                 * symbols if the global effective group is either Group3 or Group4.
                 */
                group = num_groups - 1;
            break;

            case XkbWrapIntoRange:
                /* If neither flag is set, group is wrapped into range using integer
                 * modulus. For example, a key with two groups of symbols for which
                 * groups wrap uses Group1 symbols if the global effective group is
                 * Group3 or Group2 symbols if the global effective group is Group4.
                 */
            default:
                if (num_groups != 0) {
                    group %= num_groups;
                }
            break;
        }

        XkbKeyTypePtr key_type = XkbKeyKeyType(keyboard_map, keycode, group);
        unsigned int active_mods = event_mask & key_type->mods.mask;

        int i, level = 0;
        for (i = 0; i < key_type->map_count; i++) {
            if (key_type->map[i].active && key_type->map[i].mods.mask == active_mods) {
                level = key_type->map[i].level;
            }
        }

        keysym = XkbKeySymEntry(keyboard_map, keycode, level, group);
        XkbFreeClientMap(keyboard_map, XkbAllClientInfoMask, true);
    }

    return keysym;
}

int max(int a, int b)
{
	if(b > a) return b;
	return a;
}

