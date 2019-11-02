#define _SCREEN_H

// Functions for interacting with the screen directly
// This lists only the public functions

void _cscreen();
void _printc(char);
void _printec(char);
void _prints(char*);
void _printes(char*);
void _printbks();
void _onc();
void _offc();
void _setcpos(int);
int _getcpos();
int _getrowpos();
int _getcolpos();
