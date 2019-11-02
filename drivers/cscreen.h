#define _SCREEN_H

// Functions for interacting with the screen directly
// This lists only the public functions

void _cscreen();
void _ccscreen(int formats);
void _printcc(char chr, int formats);
void _printc(char chr);
void _printec(char chr);
void _printcs(char *str, int formats);
void _prints(char *str);
void _printes(char *str);
void _printbks();
void _onc();
void _offc();
void _setcpos(int offset);
void _setcposx(int x, int y);
int _getcpos();
int _getrowpos();
int _getcolpos();