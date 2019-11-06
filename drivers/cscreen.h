#define _SCREEN_H


// Functions for interacting with the screen directly
// This lists only the public functions

void _cscreen();
void _printc(char chr);
void _printec(char chr);
void _prints(char *str);
void _printes(char *str);
void _printbks();
void _onc();
void _offc();
void _setcpos(int offset);
int _getcpos();
int _getrowpos();
int _getcolpos();


