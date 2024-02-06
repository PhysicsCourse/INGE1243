#include <graphics.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <io.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <dos.h>
#include <dir.h>
#include <process.h>
#include <time.h>
#include <values.h>
#include <alloc.h>
#include "MGRLIB\gifsave.h"
#include "MGRLIB\svgautil.h"
#include "MGRLIB\svga256.h"

// Num‚ro de version
#define VER 1.94
// Ann‚e et mois de la version
#define VAN 2006.10
// Num‚ro de s‚rie
#define SER 1234567890L
// Nb de couleurs pour le GIF
#define NUMCOLORS 16
// Num‚ro de page de l'aide "Menu"
#define MENUPAGE 7
// Nb max de canaux
#define MAXCNX 30
// Nb max de colonnes
#define MAXCOL 40
// Nb max de fichiers
#define MAXFI 400
// Nb max de couleurs des canaux
#define MAXCLR 12
// Longueur max des chaŒnes
#define MAXSTR 512
// Nb max des lignes infos
#define MAXINFO 10
// Nb harmoniques
#define MAXHARM 4
// Nb min HiCum
#define MINHICUM 4
// Nb de styles de lignes
#define NBGT 3
// Nb de styles de marqueurs
#define NBGM 6
// Dur‚e du double clic souris
#define DBLCLIC 300
// Valeur des fractions temporelles
#define MSEC 10000.
// Taille du buffer COM
#define TAILLE_BUF 2500

// Codes des touches
#define ESC 0x1b
#define LF 0x0a
#define CR 0x0d
#define F1 59
#define MajF1 84
#define CtrlF1 94
#define AltF1 104
#define F2 60
#define MajF2 85
#define AltF2 105
#define CtrlF2 95
#define F3 61
#define F4 62
#define F5 63
#define MajF5 88
#define CtrlF5 98
#define F6 64
#define MajF6 89
#define CtrlF6 99
#define AltF6 109
#define F7 65
#define MajF7 90
#define F8 66
#define MajF8 91
#define CtrlF8 101
#define AltF8 111
#define F9 67
#define MajF9 92
#define CtrlF9 102
#define AltF9 112
#define F10 68
#define F12 134
#define AltA 30
#define AltC 46
#define AltD 32
#define AltE 18
#define AltF 33
#define AltG 34
#define AltH 35
#define AltI 23
#define AltK 37
#define AltL 38
#define AltM 50
#define AltN 49
#define AltO 24
#define AltP 25
#define AltR 19
#define AltS 31
#define AltT 20
#define AltU 22
#define AltV 47
#define AltW 17
#define AltX 45
#define AltZ 44
#define PGDN 81
#define PGUP 73
#define ARDN 80
#define ARUP 72
#define LEFT 75
#define RIGHT 77
#define HOME 71
#define END 79

void Init_RS(void);
void Lire_RS(void);
void Lire_Chaine(void);
void Ecrire_Chaine(char *sf);
void Ecrire_RS(char c);
void AttendreCOM(void);

int huge DetectVGAMode(void);
//int save_screen(void far *buf[4]);
//void restore_screen(void far *buf[4]);
int save_screen(void far *buf);
void restore_screen(void far *buf);

double Max(double value1, double value2);
double Min(double value1, double value2);
double Frac(double value);
double Round(double value);

void Aff_Aide(int Page);
void Aff_Book(void);
void Aff_Graphique(char mdc);
void Aff_Horloge(void);
void Aff_Info(char md);
void Aff_Legendes(void);
void Aff_Menu(int m);
void Aff_Message(char *sm, char m, char b);
void Aff_Noms(int cn);
void Aff_Pos(int xs, int ys);
int Aff_Souris(int f);
void Annule_Styles(char *sg);
void ANSI2OEM(char *sf);
void Beep(void);
void Calc_Harmonic(int i, char m);
int Calc_HiCum(void);
void Calc_RL(int i);
signed char CalcDegFmt(char *sf);
void Calc_Variables(void);
void CalcZ(void);
void Calib_Data(double *dd, int ii);
char CarCan(int id);
void Couleur_Ecran(signed char ce);
void Decode_Arguments(char *ss);
void Decode_ChDraw(void);
void Decode_Doodson(char *ss);
int Decode_Format(char *sg);
void Decode_Ligne(char *sf);
void Decode_Mois(char *sf);
void Decode_Text(char *st);
double Decode_Time(char *sf);
void Decode_v8(void);
void Efface_Graph(void);
void Efface_Menu(char m);
void Efface_Tmp(void);
void Eff_Filter(void);
int Entrer_Chaine(char *st, char *sc, char *sv, int md);
void Erreur_Format(void);
void Eval_RL(int i, double dout[2][MAXCNX], double din[2][MAXCNX], double tin[2]);
void Export_Data(void);
int Extract_Data(int md);
void Faire_ChDraw(void);
void Faire_Formule(char *sf);
void Faire_Label(void);
void Faire_Menu(void);
void Faire_Nom_Export(void);
void Faire_Time(double t, char *st);
void Faire_Tri(void);
void Faire_Fusion(void);
void Faire_Spike(void);
void FF1(void);
void FF5(void);
int FF6(char pt, char pm);
int Format(char *sf);
int FUD(int ud);
double Grid(double d, char p);
void Grille(double vg1, double vg2, int dg);
void Init_Axes(void);
void Init_Boite(void);
void Init_Coul(void);
void Init_Ecran(void);
int Init_Filter(void);
void Init_Texte(void);
void Init_Variables(void);
int IsFree(char cd);
int IsTime(char *sf);
int IsCanal(char cn);
int IsDedans(int ipx, int ipy, int ipx1, int ipx2, int ipy1, int ipy2);
int IsGraph(int ipx, int ipy);
void Inverse_Tmp(void);
void Ligne(double x1, double y1, double x2, double y2, char s, char k, int c, char m);
void Limite_Nom(char *s1, char *s2, int n);
void Lire_3Data(void);
int Lire_XLignes(void);
void Lire_Calib(double *tc, int m);
void Lire_Fichier_Origine(void);
void MAJ_Data(char m);
void MAJ_AD(void);
void MAJ_DE(void);
void MAJ_DV(void);
void MAZ_RL(int i);
void MAJ_RL(int i, double lrx, double lry);
void New_Calib_File(void);
void Pg2(char *sg, int px, int py, int cm);
void Pg(char *ps, int px, int py);
double Phase_Doodson(double t, double *dd);
void Pl(char *ps, double p, int d);
void Pt2(char *st);
void Refresh_Info(void);
void Save_GIF_File(void);
void Save_PS_File(void);
void Sortie(char *ss);
int Test_Data(void);
void Trace_RL(int rl);
double TGrid(double l1, double l2);
void Vide(int ipx1, int ipx2, int ipy1, int ipy2, int cl);
void Working(double p1, double p2);
void Draw_Souris(int x, int y);
