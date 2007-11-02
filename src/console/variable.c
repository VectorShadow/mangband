#include "angband.h"

/* Console global variables */

sockbuf_t ibuf;

char server_name[80] = "undefined";
char pass[40] = "undefined";

term *ang_term[8];
u32b window_flag[8];

byte color_table[256][4];

cptr ANGBAND_SYS;

s16b command_cmd;
s16b command_dir;

cptr ANGBAND_DIR;
cptr ANGBAND_DIR_APEX;
cptr ANGBAND_DIR_BONE;
cptr ANGBAND_DIR_DATA;
cptr ANGBAND_DIR_EDIT;
cptr ANGBAND_DIR_FILE;
cptr ANGBAND_DIR_HELP;
cptr ANGBAND_DIR_INFO;
cptr ANGBAND_DIR_SAVE;
cptr ANGBAND_DIR_USER;
cptr ANGBAND_DIR_XTRA;
