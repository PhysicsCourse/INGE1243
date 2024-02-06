#include "MGRLIB\mgr.h"


extern FILE *f_tmp, *f_tmp0, *f_tmp1, *f_tmp2, *f_cal, *f_tab, *f_ps;
extern char *Titre0, Titre1[30], Titre2[15], Titre3[38], Titre4[41], Mois[12][4],
	    Graph_String[22], Format_String[22], Nombre_String[16],
	    MGR_dir[128], FD_Tmp[10], FC_List[40], FC_Sort[40], FC_HCum[40],
	    FC_Tmp0[40], FC_Tmp1[40], FC_Tabl[40], Flag_MCR, ComBuf[TAILLE_BUF],
	    Str[MAXSTR], Car, FF_In[MAXPATH], FC_Cal[MAXPATH],
	    FC_HCum[40], *FF_Tmp0, *FF_Tmp1, ExpGIFName[MAXPATH], ExpPSName[MAXPATH],
	    F_drv[MAXDRIVE], F_dir[MAXDIR], F_nam[MAXFILE], F_ext[MAXEXT],
	    FC_In[10][MAXPATH], FTick[10], FTitre[80],
	    FC_Out[MAXPATH], FF_Out[MAXPATH], Fmt[MAXFI][4*MAXCNX+1], Header[2],
	    SUnit[MAXCNX][8], SName[MAXCNX][17], Label[MAXCNX][25],
	    ChDraw[4*MAXCNX], c1, AllCh[MAXCNX+1], AllChN[2*MAXCNX],
	    OC, OS[2], LCommand[MAXSTR],
	    ImpFmt[MAXCOL+1], ExpFmt[MAXCOL+1], DataFmt[10], FInfo[MAXINFO][MAXSTR],
	    Phase[4], Doodson[20];
extern unsigned char DataOut[MAXCNX], NbInfo, NDAS;
extern signed char PEcran, GdView, Souris, EcranGraph, NbGs, DMode, DAS, EpLigne, Calib,
		   DCh, Menu, Canx, Append, TimeMode, Polar, NTOver, Sort, Cnx[MAXFI], AcPM,
		   ExpPS, FISO, XInfo, Shot, Merge, AutoFmt, RNaN, HiCum, Info,
		   NbGn, GnC, Gn[MAXCNX], Ge[MAXCNX], Gt[MAXCNX], Gm[MAXCNX],
		   DRef, RepCh, Fml, NbFml, ChAff, VGAT, CalTable[MAXCNX];
extern int Mx, My, Yff, Wi1, Wi2, CMode, VGAMode, NbPage,
	   NbIF, Divisor[MAXCNX], Gx1, Gx2, Gxs, Gy1[MAXCNX], Gy2[MAXCNX], Gys[MAXCNX], Gc[MAXCNX],
	   Site, UDAS, NumBaud, DataBit, StopBit, Parity;
extern unsigned nf, NbFiles, NbSF, Com, NbHiCum, ComTete, ComFin, Port;

extern unsigned long Decim, X, Y, NbY, Filtre, PtTable[2][MAXCNX];
extern long MDiff, MaxVal, Offset[MAXCNX], Frequence[MAXCNX],
	    FStamp[MAXFI];
extern double Rapx, Rapy;
extern double NaN, NbSec[5], Date1970, TTick[20], Acmin, Acmax, Acqu[MAXFI],
	      Cd[2][MAXCNX], Data[2][MAXCNX], Don[2][MAXCNX], DD[6],
	      Bruit[MAXCNX], Cal_F[MAXCNX], Cal_Cst[MAXCNX],
	      Cal_x0[2][MAXCNX], Cal_x1[2][MAXCNX], Cal_x2[2][MAXCNX], Cal_x3[2][MAXCNX],
	      STime, Zoom, CBTime, CETime, Dte[2], Spike,
	      Vmax[MAXCNX], Vmin[MAXCNX],
	      TDmax[MAXFI], TDmin[MAXFI], Tmax, Tmin, Zmin, Zmax,
	      Period, HarmS[MAXHARM][MAXCNX], HarmC[MAXHARM][MAXCNX];
extern struct palettetype Pal;
extern union REGS regs;
extern struct SREGS sregs;
extern void interrupt (*old_RS)();
extern DacPalette256 RVB;
extern time_t Horloge;



//**************************************************************************
// ANSI2OEM: remplace les caractŠres sp‚ciaux ANSI (Windows) par OEM (DOS)
//**************************************************************************

void ANSI2OEM(char *sf)
{
 int i;

 for (i = 0; i<strlen(sf); i++) {
  switch (sf[i]) {
  case 'à': sf[i] = '…';  break;
  case 'â': sf[i] = 'ƒ';  break;
  case 'ä': sf[i] = '„';  break;
  case 'é': sf[i] = '‚';  break;
  case 'è': sf[i] = 'Š';  break;
  case 'ê': sf[i] = 'ˆ';  break;
  case 'ë': sf[i] = '‰';  break;
  case 'î': sf[i] = 'Œ';  break;
  case 'ï': sf[i] = '‹';  break;
  case 'ô': sf[i] = '“';  break;
  case 'ö': sf[i] = '”';  break;
  case 'ù': sf[i] = '—';  break;
  case 'û': sf[i] = '–';  break;
  case 'ç': sf[i] = '‡';  break;
  case '°': sf[i] = 'ø';  break;
  case 'µ': sf[i] = 'æ';  break;
  case '²': sf[i] = 'ý';  break;
  }
 }
}



int IsCanal(char cn)
{
 if (isdigit(cn)) return (cn - '0');
 if (isupper(cn) && cn-'A'+9<MAXCNX) return (cn - 'A' + 10);
 return (0);
}


char CarCan(int id)
{
 if (id<9) return (id + '1');
 if (id>=9 && id<MAXCNX) return (id - 9 + 'A');
 return (0);
}


//**************************************************************************
// Beep: Emet un bip sonore.
//**************************************************************************

void Beep(void)
{
 sound(1000); delay(50); nosound();
}


//**************************************************************************
// Aff_Horloge: met … jour l'horloge si les secondes ont chang‚.
//**************************************************************************

void Aff_Horloge(void)
{
 time_t tt;

 if (time(&tt)!=Horloge) {
  time(&Horloge);
  Aff_Souris(3);
  Aff_Pos(regs.x.cx, regs.x.dx);
 }
}


//**************************************************************************
// IsDedans: teste si la position graphique (ipx, ipy) se trouve dans la
// fenˆtre (ipx1, ipy1)-(ipx2,ipy2).
//**************************************************************************

int IsDedans(int ipx, int ipy, int ipx1, int ipx2, int ipy1, int ipy2)
{
 return ((ipx >= ipx1)&&(ipx <= ipx2)&&(ipy >= ipy1)&&(ipy <= ipy2));
}


void Vide(int ipx1, int ipx2, int ipy1, int ipy2, int cl)
{
 setfillstyle(SOLID_FILL, cl);
 bar(ipx1, ipy1, ipx2, ipy2);
}


//**************************************************************************
// Efface_Menu: efface la ligne de menu complŠte si m==0, en laissant la
// barre de Working() si m!=0.
//**************************************************************************

void Efface_Menu(char m)
{
 Vide(0, Mx - (m!=0)*150, My - 10, My, BLACK);
}


//**************************************************************************
// Limite_Nom: raccourcit si n‚cessaire la chaŒne *s1 … n caractŠres en
// ajoutant "..." devant (pour affichage) et renvoie le r‚sultat dans *s2.
//**************************************************************************

void Limite_Nom(char *s1, char *s2, int n)
{
 int i;

 i = strlen(s1) - n;
 if (i>0) sprintf(s2, "...%s", &s1[i+3]);
 else strcpy(s2, s1);
}


//**************************************************************************
// Inverse_Tmp: inverse les 2 pointeurs sur fichiers temporaires.
//**************************************************************************

void Inverse_Tmp(void)
{
 if (FF_Tmp0==FC_Tmp0) {
  FF_Tmp0 = FC_Tmp1;
  FF_Tmp1 = FC_Tmp0;
 } else {
  FF_Tmp0 = FC_Tmp0;
  FF_Tmp1 = FC_Tmp1;
 }
}


//**************************************************************************
// Init_Coul: initialise le tableau des couleurs de tous les canaux Gc[],
// en utilisant un ordre pr‚d‚fini de 10 couleurs.
//**************************************************************************

void Init_Coul(void)
{
 int i,
     CBase[MAXCLR] = { LIGHTRED,	// Canal 1
		YELLOW,		// Canal 2
		LIGHTGREEN,  	// Canal 3
		LIGHTCYAN,   	// Canal 4
		LIGHTBLUE,   	// Canal 5
		LIGHTMAGENTA, 	// Canal 6
		RED,	      	// Canal 7
		BROWN,      	// Canal 8
		GREEN,      	// Canal 9
		CYAN,	    	// Canal A
		BLUE,	       	// Canal B
		MAGENTA };       	// Canal C

 for (i = 0; i<MAXCNX; i++) Gc[i] = CBase[i%MAXCLR];
}


double Max(double value1, double value2)
{ return ((value1 > value2) ? value1 : value2); }

double Min(double value1, double value2)
{ return ((value1 < value2) ? value1 : value2); }

double Frac(double value)
{ return (value - floor(value)); }

double Round(double value)
{ return (ceil(value -.5)); }


//**************************************************************************
// Decode_Mois: remplace les noms de mois par leur valeur num‚rique.
//**************************************************************************

void Decode_Mois(char *sf)
{
 char s0[3];
 char *p;
 int i;

 for (i = 0; i<12; i++) {
  p = strstr(strupr(sf), strupr(Mois[i]));
  if (p!=NULL) {
   sprintf(s0, "%02d", i + 1);
   p[0] = ' ';  p[1] = s0[0];  p[2] = s0[1];
  }
 }
}


//**************************************************************************
// Pg: ‚crit un texte (ps) en mode graphique … la position (px, py) ou bien
// si valeurs n‚gatives:
//	-11 = position courante
//	-12 = centr‚
//	-13 = justifi‚ gauche ou haut
//	-14 = justifi‚ droite ou bas
//	-15 = ligne suivante (py)
//	-16 = ligne suivante avec interligne (py)
//**************************************************************************

void Pg(char *ps, int px, int py)
{
 int w, h, wh, lvx, lvy, pd;
 struct viewporttype Vue;
 struct textsettingstype Txt;

 gettextsettings(&Txt);
 pd = Txt.direction;
 getviewsettings(&Vue);
 lvx = Vue.right - Vue.left;
 lvy = Vue.bottom - Vue.top;
 w = textwidth(ps);
 h = textheight(ps);
 if (Txt.font==2) h += 2;
 if (pd) { wh = w; w = h; h = wh; }
 switch (px) {
 case -11: px = getx(); break;
 case -12: px = lvx/2 - w/2; break;
 case -13: px = 0; break;
 case -14: px = lvx - w; break;
 }
 switch (py) {
 case -11: py = gety(); break;
 case -12: py = lvy/2 - h/2; break;
 case -13: py = 0; break;
 case -14: py = lvy - h;  break;
 case -15: py = gety() + 1.25*h; break;
 case -16: py = gety() + 1.5*h; break;
 }
 outtextxy(px, py, ps);
 if (pd) moveto(px, py - h);
 else moveto(px + w, py);
}


//**************************************************************************
// Pg2: ‚crit un texte en 2 couleurs (d‚termin‚s par '@') avec la fonction
// Pg(sg, px, py) en 2 modes:
//	- cm = 0: gris et blanc (pages d'aide);
//	- cm = 1: bleu et vert (messages).
// InterprŠte ‚galement les mots "Alt", "Ctrl" et "Shift" et les ‚crit en
// exposant.
//**************************************************************************

void Pg2(char *sg, int px, int py, int cm)
{
 int cl0, cl1, ok = 1;
 char *p;

 if (!cm) {
  cl0 = LIGHTGRAY;  cl1 = WHITE;
 } else {
  cl0 = GREEN;  cl1 = LIGHTCYAN;
 }
 settextstyle(cm, 0, 0);
 setcolor(cl0);
 if (sg[0]=='@') setcolor(cl1);
 p = strtok(sg, "@");
 Pg(p, px, py);
 do {
  if (ok) { if (getcolor()==cl0) setcolor(cl1);
  else setcolor(cl0); }
  if ((p = strtok(NULL, "@")) == NULL) break;
  if (cm && strstr(p, "Alt")) {
   settextstyle(cm, 0, 2);
   Pg("Alt", -11, -11);
   settextstyle(cm, 0, 0);
   p += 3;
  }
  if (cm && strstr(p, "Shift")) {
   settextstyle(cm, 0, 2);
   Pg("Shift", -11, -11);
   settextstyle(cm, 0, 0);
   p += 5;
  }
  if (cm && strstr(p, "Ctrl")) {
   settextstyle(cm, 0, 2);
   Pg("Ctrl", -11, -11);
   settextstyle(cm, 0, 0);
   p += 4;
  }
  Pg(p, -11, -11);
  if (p[strlen(p)+1] == '@') { Pg("@", -11, -11); ok = 0; }
  else ok = 1;
 } while (1);
}


//**************************************************************************
// Pt2: ‚crit un texte en 2 couleurs (d‚termin‚s par '@') en mode texte.
//**************************************************************************

void Pt2(char *st)
{
 int cl[2] = {LIGHTGRAY, WHITE}, c = 0, ok = 1;
 char *p;

 if (st[0]=='@') c = 1;
 textcolor(cl[c]);
 p = strtok(st, "@");
 cputs(p);
 do {
  if (ok) {
   c = (c+1)%2;
   textcolor(cl[c]);
  }
  if ((p = strtok(NULL, "@")) == NULL) break;
  cputs(p);
  if (p[strlen(p)+1] == '@') { cputs("@"); ok = 0; }
  else ok = 1;
 } while (1);
 textcolor(cl[0]);
}


//**************************************************************************
// Aff_Message: affiche la chaŒne *sm … l'‚cran en fonction de EcranGraph:
//	- m == 0: en laissant la place pour Working();
//	- m == 1: nouvelle ligne;
//	- m == 2: message d'erreur;
//	- b != 0: en ‚mettant un bip.
//**************************************************************************

void Aff_Message(char *sm, char m, char b)
{
 if (b) Beep();
 if (EcranGraph) {
  Efface_Menu(m);
  Pg2(sm, -13, -14, 2);
 } else {
  switch (m) {
  case 0: cputs("\n     ");  break;
  case 1: cputs("\r      ");  break;
  case 2: Pt2("\r\n@Error:@ ");  break;
  }
  Pt2(sm);
 }
}


void Efface_Graph(void)
{
 Aff_Souris(2);
 setfillstyle(SOLID_FILL, BLACK);
 Vide(2, Mx - 2, Yff + 2, My - 14, BLACK);
 Aff_Souris(1);
}


//**************************************************************************
// Working: affiche la jauge de travail … partir des variables entre 0 et 1
//	p1 = vert (donn‚es origine);
//	p2 = rouge (donn‚es trait‚es).
//**************************************************************************

void Working(double p1, double p2)
{
 char s0[6], swk[6] = "|/-\\";
 int wk = 100;

 if (p2==-1.) p2 = p1;
 if (p1==0.) {
  if (EcranGraph) {
   Vide(Mx - wk - 1, Mx - 1, My - 8, My, DARKGRAY);
   setcolor(WHITE);
   rectangle(Mx - wk - 2, My - 9, Mx, My);
  }
  Wi1 = -1;  Wi2 = -1;
 }
 if (floor(100*p1)>Wi1) {
  Wi1 = 100*p1;
  if (EcranGraph) {
   Vide(Mx - wk -1, Mx - wk - 1 + Wi1, My - 5, My - 8, GREEN);
   settextstyle(0, 0, 0);  setcolor(GREEN);
   sprintf(s0, "%3d %%", Wi1);
   setcolor(BLACK);  Pg("ÛÛÛ", Mx - 148, -14);
   setcolor(LIGHTGREEN);  Pg(s0, Mx - 148, -14);
  } else {
   cprintf("\r %3d%%", Wi1);
  }
 }
 if (floor(100*p2)>Wi2) {
  Wi2 = 100*p2;
  if (EcranGraph) {
   Vide(Mx - wk - 1, Mx - wk - 1 + Wi2, My - 1, My - 4, RED);
  }
 }
 if (!EcranGraph) {
  if (p1==1.) cprintf("\r ");
  else cprintf("\r%c", swk[(int)(X%4l)]);
 }
}


//**************************************************************************
// IsTime: teste si la chaŒne de format (*sf) contient au moins un caractŠre
// de r‚f‚rence temporelle et renvoie 1 dans ce cas, sinon 0.
//**************************************************************************

int IsTime(char *sf)
{
 int ii, ok;

 for (ii = 0, ok = 0; ii<strlen(sf); ii++) {
  if (strchr(Format_String, sf[ii]) < strchr(Format_String, 'i')) {
   if (!isdigit(sf[ii])) ok = 1;
  }
 }
 return ok;
}


//**************************************************************************
// Decode_Time: d‚code la chaŒne *sf (time_string = YYMMDDhhnnss) et renvoie
// la date et l'heure dans un double. D‚code ‚galement *sf comme argument de
// l'option "/yn:" qui interprŠte la date … partir du nom des fichiers.
//**************************************************************************

double Decode_Time(char *sf)
{
 int i, j, k, tt[7] = {0, 0, 1, 0, 0, 0, 0};
 char *as, Inter[7][12], s0[20], s1[2];
 time_t temps;
 struct tm date2;

 temps = 0l;
 date2 = *gmtime(&temps);
 if (atol(sf)) {
  sscanf(sf, "%2d%2d%2d%2d%2d%2d%3d",
       &tt[0], &tt[1], &tt[2], &tt[3], &tt[4], &tt[5], &tt[6]);
 } else {
  // ===== Option "/yn:" = fabriquer AllTime … partir du nom de fichier
  for (i = 0; i<7; i++) strcpy(Inter[i], "");
  fnsplit(FF_In, F_drv, F_dir, F_nam, F_ext);
  as = strchr(sf, '\\');
  if (as!=NULL) strcpy(s0, &F_dir[strlen(F_dir)-(int)(as-sf)-1]);
  else strcpy(s0, F_nam);
  for (i = 0, k = 0; i<strlen(sf); i++) {
   if (sf[i]=='\\') { strcpy(s0, F_nam); k = i + 1; }
   if (sf[i]=='.') { strcpy(s0, F_ext); k = i; }
   j = (int) (strchr(Format_String, sf[i]) - Format_String);
   if (j>=0 && j<7) {
    if (j==6) j = 2;
    sprintf(s1, "%c", s0[i-k]);
    strcat(Inter[j], s1);
   }
  }
  for (i = 0; i<7; i++) {
   //printf("Inter[%i] = \"%s\" \n", i, Inter[i]);
   if (strlen(Inter[i])) tt[i] = atoi(Inter[i]);
  }
  //exit(1);
 }
 // ===== si ann‚e … 4 chiffres: retour … 2 chiffres (DOS)!
 if (tt[0]>=1900) tt[0] -= 1900;
 // ===== ann‚es de 00 … 69 => 2000 … 2069...
 if (tt[0]<70) tt[0] += 100;
 // ===== dans le format date, le mois doit ˆtre entre 0 et 11...
 if (tt[1]) tt[1]--;
 date2.tm_year = tt[0];
 date2.tm_mon = tt[1];
 date2.tm_mday = tt[2];
 date2.tm_hour = tt[3];
 date2.tm_min = tt[4];
 date2.tm_sec = tt[5];
 return (mktime(&date2) + tt[6]/MSEC);
}


//**************************************************************************
// Faire_Time: renvoie une chaŒne *st (time_string = YYMMDDhhnnss) … partir
// de la date et l'heure dans un double t.
//**************************************************************************

void Faire_Time(double t, char *st)
{
 time_t temps;
 struct tm date2;

 temps = floor(t);
 date2 = *gmtime(&temps);
 strftime(st, 20, "%y%m%d%H%M%S", &date2);
}

//**************************************************************************
// CalcDegFmt: d‚finit la variable (DegFmt) donnant le degr‚ de r‚f‚rence
// temporelle de la chaŒne de format (*sf).
//**************************************************************************

signed char CalcDegFmt(char *sf)
{
 int ii, ok;

 for (ii = 0, ok = 10; ii<strlen(sf); ii++) {
  if (strchr("Yytg", sf[ii])) ok = 0;
  if (strchr("jfmb", sf[ii])) ok = min(ok, 1);
  if (sf[ii]=='d') ok = min(ok, 2);
  if (strchr("hlkq", sf[ii])) ok = min(ok, 3);
  if (sf[ii]=='n') ok = min(ok, 4);
  if (sf[ii]=='s') ok = min(ok, 5);
 }
 return ok;
}


//**************************************************************************
// New_Calib_File: ‚crit dans le fichier (f_cal) pr‚alablement ouvert, les
// paramŠtres d'un fichier de calibration vierge. Lance ensuite l'‚diteur
// de textes DOS pour les modifications.
//**************************************************************************

void New_Calib_File(void)
{
 char s0[MAXSTR];
 int i, j;

 for (i = 0; i<76; i++) fputs("%", f_cal);
 fputs("\n% æGRAPH CALIBRATION FILE: Please modify parameters, save the file and exit\n", f_cal);
 fputs("%\n", f_cal);
 fputs("%                   A0 + A1*x + A2*x^2 + A3*x^3\n", f_cal);
 fputs("% d = FACT * TABLE( --------------------------- ) + CST\n", f_cal);
 fputs("%                   B0 + B1*x + B2*x^2 + B3*x^3\n", f_cal);
 fputs("%\n", f_cal);
 fputs("% where d is calibrated data, x is original data, or computed frequency (in\n", f_cal);
 fputs("% Hz) for DAS data. TABLE indicates a calibration table filename with 2\n", f_cal);
 fputs("% columns YI = TABLE(XI). See \"mgr.pdf\" file for more information.\n", f_cal);
 for (i = 0; i<76; i++) fputs("%", f_cal);
 sprintf(s0, "\n# TITLE: CALIBRATED DATA USING %s\n", FC_Cal);
 fputs(s0, f_cal);
 fputs("# VALID:\n", f_cal);
 fputs("# LAG: 0\n", f_cal);
 fputs("SENSOR_NAME  UNIT  DIV  FREQ  FACT  CST  A0   A1   A2   A3   B0   B1   B2   B3   TABLE XI YI\n", f_cal);
 for (j = 0; j<Canx; j++) {
  sprintf(s0, "Chan_%c      ", CarCan(j));
  if (DAS) fprintf(f_cal, "%s Hz    2    0     1     0    0    1    0    0    1    0    0    0    -     1  2\n", s0);
  else     fprintf(f_cal, "%s -     1    0     1     0    0    1    0    0    1    0    0    0    -     1  2\n", s0);
 }
 fclose(f_cal);
 if (EcranGraph) closegraph();
 if (spawnlp(P_WAIT, "edit", "edit", FC_Cal, NULL) == -1) {
  sprintf(s0, "\r\nCannot find MS-DOS Editor... Please edit first the new "
	 "calib. file \"%s\".", FC_Cal);
  Sortie(s0);
 }
 Calib /= 5;
}


//**************************************************************************
// Lire_Calib: ouvre le fichier (FC_Cal) et y lit tous les paramŠtres
// connus. Recherche le domaine de validit‚ compatible avec le temps courant
// (*tc) et lit alors les variables: Divisor[i], Frequence[i], Cal_F[i],
// Cal_Cst[i], Cal_x0[i], Cal_x1[i], Cal_x2[i], Cal_x3[i] (2 dimensions),
// SUnit[i], SName[i]), puis d‚finit Offset[i].
// Si un fichier de table de calibration est d‚fini, ‚criture du fichier
// temporaire (FC_Tabl) et mise … jour de PtTable[][i].
//**************************************************************************

void Lire_Calib(double *tc, int m)
{
 FILE *f_tab1;
 char *p, s0[128], s1[128], sl[MAXSTR], st[MAXSTR], nouv,
      ftable[MAXCNX][MAXPATH], fmt[128];
 int i, j, k, xy[2][MAXCNX];
 double x, y, t[10];

 f_cal = fopen(FC_Cal, "rt");
 //f_tab2 = fopen(FC_Tabl, "wb");
 f_tab = fopen(FC_Tabl, "wt");
 do {
  if (fgets(sl, 256, f_cal) == NULL) break;
  if ((p = strstr(strupr(sl), "# TITLE:")) != NULL)
   strcpy(FTitre, &p[8]);
  if ((p = strstr(strupr(sl), "# VALID:")) != NULL) {
   strcpy(s0, "");  strcpy(s1, "");
   sscanf(&p[8], "%s %s", &s0, &s1);
   if (strlen(s0)) { CBTime = Decode_Time(s0);  CETime = MAXLONG; }
   if (strlen(s1)) CETime = Decode_Time(s1);
  }
  if ((*tc>=CBTime && *tc<CETime) || TimeMode) {
   if (Calib>0 && m && (p = strstr(strupr(sl), "# LAG:"))!=NULL) {
    *tc -= STime*NbSec[3];
    sscanf(&p[6], "%lf", &STime);
    *tc += STime*NbSec[3];
   }
   if (strstr(strupr(sl), "DIV ")!=NULL && sl[0]!='%') {
    nouv = 0;
    if (strstr(sl, "FACT ")) nouv = 1;
    if (strstr(sl, "A0 ") || strstr(sl, "Y0 ")) nouv = 2;
    if (strstr(sl, "TABLE ")) nouv = 3;
    for (i = 0; i<Canx; i++) {
     if (fgets(sl, 256, f_cal) == NULL) {
      for (j = i; j<Canx; j++) {
       Cal_x1[0][j] = 1.;
       Cal_x0[1][j] = 1.;
      }
      break;
     }
     switch (nouv) {
      case 3:
      sscanf(sl, "%s %s %d %ld %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %s %d %d",
	    &s1, &s0, &Divisor[i], &Frequence[i],
	    &Cal_F[i], &Cal_Cst[i],
	    &Cal_x0[0][i], &Cal_x1[0][i], &Cal_x2[0][i], &Cal_x3[0][i],
	    &Cal_x0[1][i], &Cal_x1[1][i], &Cal_x2[1][i], &Cal_x3[1][i],
	    &ftable[i], &xy[0][i], &xy[1][i]);
      PtTable[0][i] = 0;
      if (i==0) PtTable[1][i] = 0;
      else PtTable[1][i] = PtTable[1][i-1] + PtTable[0][i-1];
      // lecture de la table d'interpolation
      if (strcmp(ftable[i],"-")) {
       if ((f_tab1 = fopen(ftable[i],"rt")) != NULL) {
	strcpy(fmt, "%lf");
	for (k = 1; k<max(xy[0][i],xy[1][i]); k++) strcat(fmt, " %lf");
	do {
	 if (fgets(st, 256, f_tab1)==NULL) break;
	 if (st[0]!='#' && st[0]!='%') {
	  sscanf(st, fmt, &t[0], &t[1], &t[2], &t[3], &t[4], &t[5],
			  &t[6], &t[7], &t[8], &t[9]);
	  x = t[xy[0][i]-1];
	  y = t[xy[1][i]-1];
	  fwrite(&x, sizeof(x), 1, f_tab);
	  fwrite(&y, sizeof(y), 1, f_tab);
	  //fprintf(f_tab, "%lg\t%lg\n", x, y);
	  PtTable[0][i]++;
	 }
	}
	while (1);
	fclose(f_tab1);
	CalTable[i] = 1;
       }
      }
      break;
      case 2:
      sscanf(sl, "%s %s %d %ld %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
	    &s1, &s0, &Divisor[i], &Frequence[i],
	    &Cal_F[i], &Cal_Cst[i],
	    &Cal_x0[0][i], &Cal_x1[0][i], &Cal_x2[0][i], &Cal_x3[0][i],
	    &Cal_x0[1][i], &Cal_x1[1][i], &Cal_x2[1][i], &Cal_x3[1][i]);
	    break;
      case 1:
      sscanf(sl, "%d %ld %lf %lf %lf %lf %lf %s %s",
	    &Divisor[i], &Frequence[i],
	    &Cal_F[i], &Cal_x0[0][i], &Cal_x1[0][i], &Cal_x2[0][i], &Cal_x3[0][i],
	    &s0, &s1);
      Cal_x0[1][i] = 1.;  break;
      case 0:
      sscanf(sl, "%d %ld %lf %lf %lf %lf %s %s",
	    &Divisor[i], &Frequence[i],
	    &Cal_x0[0][i], &Cal_x1[0][i], &Cal_x2[0][i], &Cal_x3[0][i],
	    &s0, &s1);
      Cal_F[i] = 1.;  Cal_x0[1][i] = 1.;  break;
     }
     if (DAS>0 && Calib>0 && Frequence[i]>0) {
      Offset[i] = (Frequence[i]*Acqu[nf]/(double)Divisor[i])/(double)MaxVal;
      DataOut[i+DCh] = 1;
     }
     if (Frequence[i]==-1l) Offset[i] = 0l;
     switch (Calib) {
      case -1: case 3: case 4: strcpy(SUnit[i], s0);  break;
      case 1: strcpy(SUnit[i], "Hz");  break;
      case 2: strcpy(SUnit[i], "V");  break;
     }
     if (strlen(s1)>1) strcpy(SName[i], s1);
    }
    break;
   }
  }
 }
 while (1);
 fclose(f_cal);
 fclose(f_tab);
}


//**************************************************************************
// Calib_Data: calibre la donn‚e d'entr‚e en fonction des paramŠtres.
//**************************************************************************

void Calib_Data(double *dd, int ii)
{
 long n;
 double num, den, xy[2], xy1[2], xy2[2];

 if (DAS) *dd *= Divisor[ii];
 if (DAS && Frequence[ii]!=-1l) *dd /= Decim*Acqu[nf];
 if (Calib==2 || Calib==3) {
  num = Cal_x0[0][ii] + (*dd)*(Cal_x1[0][ii] + (*dd)*(Cal_x2[0][ii] + (*dd)*Cal_x3[0][ii]));
  den = Cal_x0[1][ii] + (*dd)*(Cal_x1[1][ii] + (*dd)*(Cal_x2[1][ii] + (*dd)*Cal_x3[1][ii]));
  if (den!=0.) *dd = num/den;
  else *dd = NaN;
  // relit la table de calibration (interpolation lin‚aire)
  if (CalTable[ii]) {
   fseek(f_tab, PtTable[1][ii]*2*sizeof(double), SEEK_SET);
   for (n = 0; n<PtTable[0][ii]; n++) {
    fread(xy, sizeof(xy), 1, f_tab);
    if (n==0) {
     xy1[0] = MAXDOUBLE;
     xy2[0] = -MINDOUBLE;
    } else {
     if (xy[0] >= (*dd) && (xy[0] - (*dd)) < (xy1[0] - (*dd))) { xy1[0] = xy[0];  xy1[1] = xy[1]; }
     if (xy[0] <= (*dd) && ((*dd) - xy[0]) < ((*dd) - xy2[0])) { xy2[0] = xy[0];  xy2[1] = xy[1]; }
    }
   }
   if (xy2[0]!= xy1[0]) *dd = (xy2[1] - xy1[1])*((*dd) - xy1[0])/(xy2[0] - xy1[0]) + xy1[1];
   else if (xy1[0]==MAXDOUBLE || xy2[0]==-MINDOUBLE) *dd = NaN;
	else *dd = xy2[1];
  }
 }
 if ((Calib==3 || Calib==4) && *dd != NaN) *dd = (*dd)*Cal_F[ii] + Cal_Cst[ii];
 /*	 printf("%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n",
	    Cal_F[ii], Cal_Cst[ii],
	    Cal_x0[0][ii], Cal_x1[0][ii], Cal_x2[0][ii], Cal_x3[0][ii],
	    Cal_x0[1][ii], Cal_x1[1][ii], Cal_x2[1][ii], Cal_x3[1][ii]); */
}


//**************************************************************************
// Faire_Formule: cr‚ation d'un nouveau canal contenant une combinaison des
// canaux existants … partir de la chaŒne *sf (Formula). Cr‚ation ou mise …
// jour de toutes les variables n‚cessaires : SName[], SLabel[], AllCh,
// AllChN, ChDraw, ... permutation des fichiers temporaires et retour …
// DMode = 0. D‚finit Fml = 1 pour poser la question "replace channel ?" lors
// d'une nouvelle formule.
//**************************************************************************

void Faire_Formule(char *sf)
{
 char s0[80], s1[80];
 int i;

 strncpy(s0, sf, 20); s0[20] = NULL;
 switch (DMode) {
  case 1:
   strcpy(s1, "off");
   break;
  case 2:
   if (DRef==-1) strcpy(s1, "lin");
   else sprintf(s1, "xy%c", CarCan(DRef));
   break;
  case 3:
   strcpy(s1, "cum");
   break;
  case 4:
   sprintf(s1, "d%ld", Filtre);
   break;
  case 5:
   sprintf(s1, "f%ld", Filtre);
   break;
  case 6:
   sprintf(s1, "h%ld", Filtre);
   break;
  default: strcpy(s1, "");
 }
 i = Canx - RepCh;
 sprintf(SName[i], "%s[%s]", s1, s0);
 sprintf(Label[i], "%s %c", SName[i], CarCan(i));
 Gt[Canx] = 1;  Gm[Canx] = 0;
 if (!RepCh) {
  sprintf(s0, "%c", CarCan(Canx));  sprintf(s1, "%s,", s0);
  strcat(AllCh, s0);  strcat(AllChN, s1);
  if (!NbFml || !strcmp(ChDraw, AllCh)) {
   if (ChDraw[strlen(ChDraw)-1]!=',') strcat(ChDraw, ",");
   strcat(ChDraw, s0);
  }
  Canx++;
 }

 // ===== Lance la routine graphique qui calculera la formule
 Aff_Graphique(2);

 Inverse_Tmp();

 if (!RepCh) {
  for (nf = 0; nf<NbFiles; nf++) Cnx[nf]++;
  nf = 0;
 }
 MAJ_DV();
 MAJ_AD();
 if (Canx>=4) ChAff = Canx - 4;
 if (DMode==4 || DMode==5) Eff_Filter();
 DMode = 0;
 Fml = 1;
 if (TimeMode==3) Calc_Harmonic(Canx-1, 0);
}


//**************************************************************************
// Faire_Fusion: relie le fichier tampon FF_Tmp0 contenant plusieurs
// fichiers concat‚n‚s et cr‚e un nouveau fichier tampon FF_Tmp1 avec les
// donn‚es fusionn‚es, compl‚t‚es par des NaN le cas ‚ch‚ant. Utilise pour
// cela les tableaux FStamp[], Cnx[].
//**************************************************************************

void Faire_Fusion(void)
{
 char s0[80];
 int i, ok;
 long st[MAXCNX];
 size_t n;
 double tf, tt[MAXCNX], dd[MAXCNX];
 struct c_tm { time_t sec;
	 unsigned int fsec; } ct;

 // ===== Les variables locales sont dimensionn‚es … MAXCNX et non MAXFI
 //       puisque la fusion sert lorsque Append = 0.

 sprintf(s0, "Merging the @%d@ files...", NbFiles);
 Aff_Message(s0, 0, 0);

 for (nf = 0; nf<NbFiles; nf++) st[nf] = FStamp[nf];
 Y = 0l;
 if ((f_tmp0 = fopen(FF_Tmp0, "rb")) == NULL) Sortie("Tmp0 ORB.");
 if ((f_tmp1 = fopen(FF_Tmp1, "wb")) == NULL) Sortie("Tmp1 OWB.");

 do {

  // ===== lecture du temps tt[] pour chaque fichier: mise … jour de tf
  for (nf = 0, tf = Tmax, ok = 0; nf<NbFiles; nf++) {
   fseek(f_tmp0, st[nf], SEEK_SET);
   n = fread(&ct.sec, 4, 1, f_tmp0);
   if (n>0 && ct.sec!=-1l) {
    fread(&ct.fsec, 2, 1, f_tmp0);
    tt[nf] = ct.sec + ct.fsec/MSEC;
    if (tt[nf] <= tf) {
     tf = tt[nf];
     ok = 1;
    }
   } else {
    tt[nf] = MAXDOUBLE;
   }
  }

  if (!ok) break;

  // ===== initialisation de tous les canaux avec NaN
  for (i = 0; i<Canx; i++) dd[i] = NaN;

  // ===== relecture (ajout de 6 octets au fseek pour sauter le temps)
  for (nf = 0, DCh = 0; nf<NbFiles; nf++) {
   if (nf) DCh += Cnx[nf-1];
   if (tt[nf]==tf) {
    fseek(f_tmp0, st[nf]+6, SEEK_SET);
    fread(&dd[DCh], 8, Cnx[nf], f_tmp0);
    st[nf] = ftell(f_tmp0);
   }
  }

  // ===== ‚criture de la ligne finale
  ct.sec = floor(tf);
  ct.fsec = Frac(tf)*MSEC;
  fwrite(&ct.sec, 4, 1, f_tmp1);
  fwrite(&ct.fsec, 2, 1, f_tmp1);
  fwrite(&dd, 8, Canx, f_tmp1);
  Y++;
  Working((tf-Tmin)/(Tmax-Tmin), -1);
 } while (tf<Tmax);

 NbY = Y;
 Working(1., -1.);
 fclose(f_tmp0);  fclose(f_tmp1);

 Inverse_Tmp();

 // ===== Mise … jour des variables
 FF_Tmp0 = FC_Tmp1;  FF_Tmp1 = FC_Tmp0;
 Merge = 0;  Append = 1;  NbFiles = 1;
 Cnx[0] = Canx;  Acqu[0] = Acmax = Acmin;  TDmin[0] = Tmin;  TDmax[0] = Tmax;
 strcpy(ExpFmt, "ymdhns*");
}


//**************************************************************************
// Faire_Tri: relie le fichier tampon FF_Tmp0 en fonction des informations
// de pointeur dans FC_Sort et cr‚‚ un nouveau fichier tampon FF_Tmp1
// contenant toutes les donn‚es tri‚es par ordre de date et avec les
// recouvrements exclus (NTOver = 1). Les marques de d‚but de fichiers
// sont supprim‚es (dans le cas de fichiers multiples) et NbFiles = 1.
//**************************************************************************

void Faire_Tri(void)
{
 char s0[80], ok = 1, i;
 long start = -1l, stamp, stamp0, stamp1 = 0l,
      stt, stp0, stp1, dtemin;
 double te[2] = {0., 0.}, dd;
 struct c_tm { time_t sec;
	 unsigned int fsec; } ct;

 sprintf(s0, "Sorting the data...");
 Aff_Message(s0, 0, 0);

 /* sprintf(s0, "sort < %s > %s", FC_Sort0, FC_Sort);
 system(s0);
 remove(FC_Sort0); */

 X = 0l; Y = 0l; nf = 0;
 if ((f_tmp0 = fopen(FF_Tmp0, "rb")) == NULL) Sortie("Tmp0 ORB.");
 if ((f_tmp1 = fopen(FF_Tmp1, "wb")) == NULL) Sortie("Tmp1 OWB.");
 if ((f_tmp2 = fopen(FC_Sort, "rb")) == NULL) Sortie("Sort ORB.");
 do {
  stamp = ftell(f_tmp0);
  if (stamp==stamp1) {
   // ----- Remet le pointeur en d‚but de fichier
   fseek(f_tmp2, 0, SEEK_SET);
   dtemin = MAXLONG;
   do {
    if (fread(&stt, sizeof(stt), 1, f_tmp2)==NULL) break;
    fread(&stp0, sizeof(stp0), 1, f_tmp2);
    fread(&stp1, sizeof(stp1), 1, f_tmp2);
    if (stt>start && stt<=dtemin) {
     dtemin = stt;
     stamp0 = stp0;  stamp1 = stp1;
    }
   } while (1);
   start = dtemin;
   fseek(f_tmp0, stamp0, SEEK_SET);
  }
  if (fread(&ct.sec, 4, 1, f_tmp0)<1) break;
  if (ct.sec==-1l) fread(&ct.sec, 4, 1, f_tmp0);
  fread(&ct.fsec, 2, 1, f_tmp0);
  if (NTOver) {
   te[1] = te[0];
   te[0] = ct.sec + ct.fsec/MSEC;
   if (X && te[0]<=te[1]) { ok = 0;  te[0] = te[1]; } else ok = 1;
  }
  if (ok) {
   fwrite(&ct.sec, 4, 1, f_tmp1);
   fwrite(&ct.fsec, 2, 1, f_tmp1);
   Y++;
  }
  for (i = 0; i<Cnx[nf]; i++) {
   if (fread(&dd, 8, 1, f_tmp0)<1) break;
   if (ok) fwrite(&dd, 8, 1, f_tmp1);
  }
  Working(X/(double)NbY, Y/(double)NbY);
  X++;
 }
 while (X<NbY);
 NbY = Y;
 Working(1., -1.);
 fclose(f_tmp0);  fclose(f_tmp1);  fclose(f_tmp2);

 Inverse_Tmp();

 // ===== Mise … jour des variables
 Sort = 1;  NTOver = 1;  nf = 0;  NbFiles = 1;
}


//**************************************************************************
// Faire_Spike: relie le fichier tampon FF_Tmp0 et cr‚e un nouveau fichier
// tampon FF_Tmp1 contenant toutes les donn‚es sans les piques:
//	- calcule le bruit std[] (‚cart type d‚riv‚e) … partir de Bruit[];
//	- calcule la moyenne des 2 derniŠres donn‚es dans moy2[];
//	- si abs(Data[0][]-Data[1][]) ET abs(Data[0][]-moy2[]) > std[]*Spike
//	  => la donn‚e Data[0][] est supprim‚e.
// Ceci permet de ne pas retirer abusivement de donn‚es lorsque le bruit est
// proche du seuil std[]*Spike.
// Vmax[], Vmin[] et Bruit[] sur les nouvelles donn‚es sont recalcul‚es.
//**************************************************************************

void Faire_Spike(void)
{
 char s0[80], i, di;
 double dt, dd, dd2, std[MAXCNX], moy2[MAXCNX];
 struct c_tm { time_t sec;
	 unsigned int fsec; } ct;

 sprintf(s0, "Remove spikes from data (@%lg@ RMS)...", Spike);
 Aff_Message(s0, 0, 0);

 for (i = 0; i<Canx; i++) {
  std[i] = Spike*sqrt(Bruit[i]/(double)NbY);
  Bruit[i] = 0.;
 }

 X = 0l; nf = 0;  DCh = 0;
 if ((f_tmp0 = fopen(FF_Tmp0, "rb")) == NULL) Sortie("Tmp0 ORB.");
 if ((f_tmp1 = fopen(FF_Tmp1, "wb")) == NULL) Sortie("Tmp1 OWB.");
 do {
  if (fread(&ct.sec, 4, 1, f_tmp0)<1) break;
  if (ct.sec==-1l) {
   if (!Append) DCh += Cnx[nf];
   nf++;
   fwrite(&ct.sec, 4, 1, f_tmp1);
   fread(&ct.sec, 4, 1, f_tmp0);
  }
  fread(&ct.fsec, 2, 1, f_tmp0);
  if (X) Dte[1] = Dte[0];
  Dte[0] = ct.sec + ct.fsec/MSEC;
  if (X==0l) Dte[1] = Dte[0];
  dt = Dte[0] - Dte[1];
  fwrite(&ct.sec, 4, 1, f_tmp1);
  fwrite(&ct.fsec, 2, 1, f_tmp1);
  for (i = 0; i<Cnx[nf]; i++) {
   di = i + DCh;
   if (X && Data[0][di]!=NaN) Data[1][di] = Data[0][di];
   if (fread(&Data[0][di], 8, 1, f_tmp0)<1) break;
   if (X==0l && Data[0][di]!=NaN) {
    Vmax[di] = Vmin[di] = Data[0][di];
    Data[1][di] = Data[0][di];
   }
   if (Data[0][di]!=NaN) {
    if (X && Data[1][di]!=NaN) {
     dd = Data[0][di] - Data[1][di];
     dd2 = Data[0][di] - moy2[di];
    } else {
     dd = 0.;
     dd2 = 0.;
    }
    if (dt!=0. && Acqu[nf]!=0.) {
     dd *= Acqu[nf]/dt;
     dd2 *= Acqu[nf]/dt;
    }
    if (fabs(dd)>std[di] && fabs(dd2)>std[di]) Data[0][di] = Data[1][di];
    else Bruit[di] += dd*dd;
    Vmax[di] = Max(Vmax[di], Data[0][di]);
    Vmin[di] = Min(Vmin[di], Data[0][di]);
   }
   fwrite(&Data[0][di], 8, 1, f_tmp1);
   moy2[di] = (Data[0][di] + Data[1][di])/2.;
  }
  Working(X/(double)NbY, -1.);
  X++;
 }
 while (X<NbY);
 Working(1., -1.);
 fclose(f_tmp0);  fclose(f_tmp1);

 Inverse_Tmp();

 MAJ_DV();
 MAJ_AD();
}


//**************************************************************************
// Calc_Harmonic: calcule les sinus et cosinus des MAXHARM premiŠres
// harmoniques de donn‚es type "Phase", … partir des variables globales
// Dte[0] et Data[0][i] (en cours de lecture dans le fichier temporaire).
// 	- m=1 : remplit HarmS[][i] et HarmC[][i];
//	- m=0 : met la phase dans HarmS[][i] et l'amplitude dans HarmC[][i].
//
// Modifi‚ d'aprŠs "HICUM3.C" par A. Somerhausen, ORB.
//**************************************************************************

void Calc_Harmonic(int i, char m)
{
 double ts, tc, t0;
 int n;

 if (!HiCum) {
  if (Period>0.) t0 = Dte[0]/Period;
  else t0 = Phase_Doodson(Dte[0], DD);
  t0 *= 2*M_PI;
 } else t0 = M_PI*Dte[0]/180.;
 for (n = 0; n<MAXHARM; n++) {
  if (m) {
   HarmS[n][i] += sin(t0*(n+1.))*Data[0][i];
   HarmC[n][i] += cos(t0*(n+1.))*Data[0][i];
  } else {
   tc = HarmC[n][i];
   ts = HarmS[n][i];
   if (tc!=0.) {
    HarmS[n][i] = atan2(ts, tc)/M_PI*180.;
    HarmC[n][i] = 2*sqrt(ts*ts + tc*tc)/(double)NbY;
   }
  }
 }
}


//**************************************************************************
// Phase_Doodson: renvoie une phase exprim‚e en nombre de cycles … partir du
// temps t (Unix) et du tableau de valeurs *dd (nombre de Doodson).
//
// Modifi‚ d'aprŠs "HICUM3.C" par A. Somerhausen, ORB.
//**************************************************************************

double Phase_Doodson(double t, double *dd)
{
 long double Alpha, TJ, SW, HW, PW, NW, PSW, TW, LL = 4.3581;

 /* TJ = (floorl(365.25*AA) + floorl(30.6001*(MM+1.0)) + JJ
	  + 1720996.5 - floorl(AA/100.0) + floorl(floorl(AA/100.0)/4.0)
	  + hh/24.0 + mm/1440.0 + ss/86400.0 - 2415020.5) / 36525.0; */

 // ===== Temps Julien (en jours depuis le 23 Nov. -4713 … 12:00:00 TU
 // calcul‚ … partir du temps unix t (en secondes depuis le 1er Jan. 1970
 // … 00:00:00) :
 // TJ = (long double)t/86400. + 2440587.5;
 // puis rapport‚ au 1er Jan. 1900 et exprim‚ en siŠcle.

 TJ = (25567. + (long double)t/86400.)/36525.;

 SW = 277.022362 + 481267.883142*TJ + 0.0011333*TJ*TJ + 0.000001889*TJ*TJ*TJ;
 HW = 280.189501 + 36000.768925*TJ + 0.0003027*TJ*TJ;
 PW = 334.385258 + 4069.034034*TJ - 0.0103249*TJ*TJ - 0.0000125*TJ*TJ*TJ;
 NW = 100.843202 + 1934.142008*TJ - 0.002078*TJ*TJ - 0.000002*TJ*TJ*TJ;
 PSW= 281.220868 + 1.719175*TJ + 0.0004527*TJ*TJ + 0.000033*TJ*TJ*TJ;

 //HW = fmodl(HW, 360.);
 //SW = fmodl(SW, 360.);
 //PW = fmodl(PW, 360.);
 //NW = fmodl(NW, 360.);
 //PSW= fmodl(PSW,360.);
 /* HW -= floorl(HW/360.)*360.;
 SW -= floorl(SW/360.)*360.;
 PW -= floorl(PW/360.)*360.;
 NW -= floorl(NW/360.)*360.;
 PSW -= floorl(PSW/360.)*360.; */

 /* TW = (hh+mm/60.0)*15.0 + HW - SW + LL; */
 TW = fmodl(t/3600., 24.)*15. + HW - SW + LL;
 //TW = fmodl(TW, 360.);
 //TW -= floorl(TW/360.)*360.;

 Alpha = dd[0]*TW + dd[1]*SW + dd[2]*HW + dd[3]*PW + dd[4]*NW + dd[5]*PSW;
 Alpha += 36000.;

 //  S1,K1 	+90
 //  Saros,M1 	+180
 //  P1,O1,Q1	-90
 //  Longitude >0 EST

 // O1
 if (dd[0]==1. && dd[1]==-1. && dd[2]==0. && dd[3]==0. && dd[4]==0. && dd[5]==0.)
    Alpha += 90.;

 // K1
 if (dd[0]==1. && dd[1]==1. && dd[2]==0. && dd[3]==0. && dd[4]==0. && dd[5]==0.)
    Alpha -= 90.;

 // P1
 if (dd[0]==1. && dd[1]==1. && dd[2]==-2. && dd[3]==0. && dd[4]==0. && dd[5]==0.)
    Alpha += 90.;

 // Semi diurne
 if (dd[0]==2.) Alpha += 180.0;

 /* Alpha = floorl(Alpha + .5); */
 //Alpha = fmodl(Alpha, 360.);
 Alpha /= 360.;
 //Alpha -= floorl(Alpha/360.)*360.;

 return ((double)Alpha);
}


//**************************************************************************
// Calc_HiCum: calcule un histogramme p‚riodique … partir des donn‚es en
// cours (dans *FF_Tmp0) et de la phase calcul‚e Phase_Doodson(). Utilise le
// fichier temporaire *FC_HiCum pour ‚crire le tableau de phases. Cr‚ation
// d'un nouveau fichier tampon contenant Canx+1 canaux de NbHiCum valeurs
// (360 par d‚faut). Passe en type de fichier "Phase".
//**************************************************************************

int Calc_HiCum(void)
{
 char s0[80], i, di;
 unsigned int n;
 long stamp;
 /* double huge (**pha), entier, tt, dd; */
 double pha[MAXCNX], nb = 0., entier, dd;
 struct c_tm { time_t sec;
	 unsigned int fsec; } ct;

 /* if ((pha = farmalloc(Canx*NbHiCum*sizeof(double))) == NULL) {
  Aff_Message("Not enough memory to allocate HiCum buffer.", 0, 1);
  return(0);
 } */
 sprintf(s0, "Computing HiCum (@%s@)...", Doodson);
 Aff_Message(s0, 0, 0);

 /* for (n = 0; n<NbHiCum; n++) for (i = 0; i<Canx; i++) pha[n][i] = 0.; */
 X = 0l;  Y = 0l;  nf = 0;  DCh = 0;
 if ((f_tmp0 = fopen(FF_Tmp0, "rb")) == NULL) Sortie("Tmp0 ORB.");
 if ((f_tmp1 = fopen(FC_HCum, "wb+")) == NULL) Sortie("Tmp1 OWB.");
 for (n = 0; n<NbHiCum; n++) {
  fwrite(&nb, sizeof(double), 1, f_tmp1);
  for (i = 0; i<Canx; i++) {
   pha[i] = 0.;
   fwrite(&pha[i], sizeof(double), 1, f_tmp1);
  }
 }
 do {
  if (fread(&ct.sec, 4, 1, f_tmp0)<1) break;
  if (kbhit()) if (getch()==ESC) {
   fclose(f_tmp0);  fclose(f_tmp1);
   return(0);
  }
  if (ct.sec==-1l) {
   if (!Append) DCh += Cnx[nf];
   nf++;
   fread(&ct.sec, 4, 1, f_tmp0);
  }
  fread(&ct.fsec, 2, 1, f_tmp0);
  Dte[1] = Dte[0];
  Dte[0] = ct.sec + ct.fsec/MSEC;
  if (X==0l) Dte[1] = Dte[0];
  if (Period>0.) n = floor(NbHiCum*modf(Dte[0]/Period, &entier));
  else n = floor(NbHiCum*Frac(Phase_Doodson(Dte[0], DD)));
  //if (nb[n]==1) Y++;
  stamp = n*sizeof(double)*(Canx + 1.);
  fseek(f_tmp1, stamp, SEEK_SET);
  fread(&nb, 8, 1, f_tmp1);
  fread(pha, 8, Canx, f_tmp1);
  nb++;
  for (i = 0; i<Cnx[nf]; i++) {
   di = i + DCh;
   Data[1][di] = Data[0][di];
   if (fread(&dd, 8, 1, f_tmp0)<1) break;
   Data[0][di] = dd;
   if (X==0l) Data[1][di] = Data[0][di];
   Eval_RL(di, Don, Data, Dte);
   pha[di] += Don[0][di];
   if (X==0l) Don[1][di] = Don[0][di];
   /*pha[n][DCh+i] += dd; */
  }
  fseek(f_tmp1, stamp, SEEK_SET);
  fwrite(&nb, 8, 1, f_tmp1);
  fwrite(pha, 8, Canx, f_tmp1);
  Working(X/(double)NbY, -1.);
  X++;
 }
 while (X<NbY);

 fclose(f_tmp0);
 if ((f_tmp0 = fopen(FF_Tmp1, "wb")) == NULL) Sortie("Tmp1 OWB.");

 Init_Variables();
 ct.fsec = 0;  X = 0l;  Y = 0l;  Decim = 1l;  DAS = 0;  Calib = 0;
 NbFiles = 1;  Append = 1;  nf = 0;  Canx++;  Cnx[0] = Canx;  TimeMode = 3;
 Acmin = Acmax = 360./(double)NbHiCum;  Acqu[0] = 0.;  AcPM = 0;
 //Acmin = Acmax = Acqu[0] = 0.;
 NTOver = 0;
 if (DMode!=6) DMode = 0;
 fseek(f_tmp1, 0l, SEEK_SET);
 for (n = 0; n<NbHiCum; n++) {
  fread(&nb, 8, 1, f_tmp1);
  fread(pha, 8, Canx-1, f_tmp1);
  if (nb>0.) {
   Dte[0] = n*360./(double)NbHiCum;
   for (i = 0; i<Canx-1; i++) Cd[0][i] = pha[i]/nb;
   Cd[0][Canx-1] = nb;
   MAJ_Data(1);
  }
 }
 fclose(f_tmp0);  fclose(f_tmp1);
 strcpy(SName[Canx-1], "HiCum_Count");
 sprintf(Label[Canx-1], "%s %c", SName[Canx-1], CarCan(Canx-1));
 strcpy(ExpFmt, "i*");
 NbY = Y;

 /* Annule_Styles(ChDraw);
 strcpy(s0, ChDraw);
 if (s0[0]==',') sprintf(ChDraw, ",|%s", &s0[1]);
 else sprintf(ChDraw, "|%s", s0); */
 Zoom = 1.;
 Calc_Variables();

 /* for (n = 0; n<NbHiCum; n++) {
  if (nb[n]>0) for (i = 0; i<Canx; i++) pha[n][i] /=(double)nb[n];
 }
 ct.fsec = 0;
 for (n = 0; n<NbHiCum; n++) {
  ct.sec = n;
  fwrite(&ct.sec, 4, 1, f_tmp1);
  fwrite(&ct.fsec, 2, 1, f_tmp1);
  for (i = 0; i<Canx; i++) fwrite(&pha[n][i], sizeof(double), 1, f_tmp1);
  //fwrite(pha[n], sizeof(double), Canx, f_tmp1);
 } */

 /* farfree(pha); */

 /*
 if ((f_tmp1 = fopen(FF_Tmp1, "rb")) == NULL) Sortie("Tmp1 OWB.");
 if ((f_tmp0 = fopen("zozo.dat", "wt")) == NULL) Sortie("ZOZO problem.");
 do {
  if (fread(&ct.sec, 4, 1, f_tmp1)<1) break;
  fread(&ct.fsec, 2, 1, f_tmp1);
  tt = ct.sec + ct.fsec/MSEC;
  fread(Cd[0], 8, Canx, f_tmp1);
  fprintf(f_tmp0, "%lg ", tt);
  for (i = 0; i<Canx; i++) fprintf(f_tmp0, "%lg ", Cd[0][i]);
  fprintf(f_tmp0, "\n");
 } while(1);
 fclose(f_tmp0);  fclose(f_tmp1); */

 Inverse_Tmp();

 Working(1., -1.);
 return(1);
}


//**************************************************************************
// Decode_Doodson: decode la chaŒne *ss pour remplir le DD[]. La chaŒne peut
// contenir soit le nombre de Doodson ("xxx.xxx"), soit le nom d'une onde
// r‚pertori‚e dans le fichier "mgr_tide.dat", soit un nombre entier
// d‚finissant Period (en multiple de Acmin).
//
// Modifi‚ d'aprŠs "HICUM3.C" par A. Somerhausen, ORB.
//**************************************************************************

void Decode_Doodson(char *ss)
{
 FILE *fid;
 char s0[80], s1[128], dd[10], wn[10], FF_Tide[128];
 int i;

 strupr(ss);
 for (i = 0; i<6; i++) DD[i] = 0.;
 Period = 0.;
 sprintf(s0, "Wave %s", ss);

 // ===== Cherche le nom de l'onde dans le fichier
 sprintf(FF_Tide, "%smgr_tide.dat", MGR_dir);
 if ((fid = fopen(FF_Tide,"rt"))!=NULL) {
  do {
   if (fgets(s1, 128, fid)==NULL) break;
   if (s1[0]!='#') {
    sscanf(s1, "%s %s", &dd, &wn);
    if (!strcmp(ss, wn)) {
     strcpy(ss, dd);
     break;
    }
   }
  } while (!feof(fid));
  fclose(fid);
 }

 // ===== Argument de Doodson "xxx.xxx"
 if (strlen(ss)==7 && strchr(ss, '.')!=NULL) {
  for (i = 5; i>=0; i--) {
   DD[i] = (double)(ss[i+(i>2)]-'0');
   DD[i] -= 5.;
  }
  DD[0] += 5.;
 }
 /*else {
  if (!strcmp(ss, "S1")) { DD[0] = 1.;  DD[1] = 1.;  DD[2] = -1.; }
  else if (!strcmp(ss, "O1")) { DD[0] = 1.;  DD[1] = -1.; }
  else if (!strcmp(ss, "K1")) { DD[0] = 1.;  DD[1] = 1.; }
  else if (!strcmp(ss, "P1")) { DD[0] = 1.;  DD[1] = 1.;  DD[2] = -2.; } */
  else {
  Period = atof(ss)*Acmin;
  if (Period==0.) Period = NbSec[2];
  sprintf(s0, "Period %g s", Period);
 }
 strcpy(ss, s0);
}


//**************************************************************************
// Entrer_Chaine: affiche (*st) et permet l'‚dition de la chaŒne (*sv) avec
// des restrictions suivant le mode (md):
// 	0 = tous caractŠres permis
//	1 = chaŒne de caractŠre dans (*sc)
// 	2 = r‚ponse un seul caractŠre dans (*sc)
// 	3 = Formula_String
// 	4 = mode temporaire pour constante (formule)...
//      5 = mode nom de fichier sortie (teste la validit‚ en ‚criture)
// Dans tous les modes, ENTER ou ESC terminent la saisie et BACKSPACE ou la
// flŠche gauche effacent le dernier caractŠre de (*sv).
// Si la saisie se termine par ESC, la fonction renvoie 0, sinon 1.
//**************************************************************************

int Entrer_Chaine(char *st, char *sc, char *sv, int md)
{
 char cc = c1, sl, s1[MAXSTR];
 int ok = 1;

 Aff_Souris(2);
 do {
  Efface_Menu(0);
  if (cc==0x08 || cc==LEFT) {
   sl = strlen(sv);
   if (sl) {
    if (sv[sl-1]=='>') md = 4;
    if (sv[sl-1]=='<') md = 3;
    sv[sl-1] = NULL;
   }
   cc = 0;
  }
  else if (cc) {
   sprintf(s1, "%c", cc);
   if (md!=2 || strlen(sv)<1) strcat(sv, s1);
   else strcpy(sv, s1);
  }
  if (strlen(sv)) sprintf(s1, "%s@%s@", st, sv);
  else strcpy(s1, st);
  Pg2(s1, -13, -14, 2);
  do {
   if (kbhit()) {
    cc = getch();
    if (md==3 && cc=='<') { md = 4; break; }
    if (md==4 && cc=='>') { md = 3; break; }
    if (strchr(sc, cc) || strchr(AllCh, toupper(cc))) break;
    if (md==4 && strchr(Nombre_String, cc)) break;
    if (cc==CR && md!=4) break;
    if (cc==0x08 || cc==LEFT || cc == ESC) break;
    if ((md==0 || md==5) && cc>31) break;
    Beep();
   }
  }
  while (1);
 }
 while ((cc!=CR || md==4) && cc!=ESC);
 if (cc==ESC) { strcpy(sv, "");  ok = 0; }
 if (md==5 && cc!=ESC) {
  if (fopen(sv, "wt") == NULL) {
   Efface_Menu(0);
   Beep();
   Pg2("@Error:@ Filename not valid!", -13, -14, 0);
   strcpy(sv, "");
   while (!kbhit());
  } else remove(sv);
 }
 Aff_Souris(1);
 return(ok);
}


//**************************************************************************
// Grid: renvoie une ‚chelle norm utilis‚e pour les grilles, d‚termin‚e …
// partir de l'amplitude maximale d et du facteur p:
//	0 = ‚chelle en Y (tient compte de Rapy et NbGs);
//	1 = ‚chelle en X (tient compte de Rapx);
//	2 = ‚chelle de phase en X (0 … 360ø);
//**************************************************************************

double Grid(double d, char p)
{
 double norm;
 int pas;

 if (p) d *= Rapx;
 else d *= NbGs*Rapy;
 if (d==0.) d = 1.;
 if (p!=2) {
  norm = pow10(floor(log10(d)));
  pas = ceil(d/norm);
  if (pas<=1) return (norm*.1);
  if (pas==2) return (norm*.2);
  if (pas<=5) return (norm*.5);
 } else {
  if (d>300.) return (60.);
  if (d>90.) return (30.);
  if (d>60.) return (10.);
  if (d>30.) return (5.);
  if (d>10.) return (2.);
  else return (1.);
 }
 return (norm);
}


//**************************************************************************
// TGrid: renvoie une ‚chelle temporelle utilis‚e pour la grille en X,
// et d‚finit un vecteur de grille TTick[], … partir du temps min l1 et
// max l2. Voir aussi Aff_Legende().
//**************************************************************************

double TGrid(double l1, double l2)
{
 double l = l2 - l1, tl;
 int i, j;
 time_t temps;
 struct tm date2;

 if (l<=100.*NbSec[2]) tl = NbSec[2]*Grid(l/NbSec[2], 1); // 100 j  => fraction
 if (l<=10.*NbSec[2]) tl = NbSec[2];     // 10 j   =>  1 j
 if (l<=2.*NbSec[2]) tl = 21600.;        //  2 j   =>  6 h
 if (l<=NbSec[2]) tl = 7200.;            //  1 j   =>  2 h
 if (l<=43200.) tl = NbSec[3];           // 12 h   =>  1 h
 if (l<=21600.) tl = 1800.;              //  6 h   => 30 min
 if (l<=10800.) tl = 900.;       	 //  3 h   => 15 min
 if (l<=NbSec[3]) tl = 300.;             //  1 h   =>  5 min
 if (l<=900.) tl = 120.;                 // 15 min =>  2 min
 if (l<=600.) tl = 60.;                  // 10 min =>  1 min
 if (l<=300.) tl = 20.;                  //  5 min => 20 sec
 if (l<=120.) tl = 10.;                  //  2 min => 10 sec
 if (l<=60.) tl = 5.;                    //  1 min =>  5 sec
 if (l<=20.) tl = 2.;                    // 20 sec =>  2 sec
 if (l<=10.) tl = Grid(l, 0);	         // 10 sec => fraction d‚cimale
 if (l<Decim*Acmin) tl = Decim*Acmin;

 if (l<=100.*NbSec[2]) {
  TTick[0] = tl*ceil(l1/tl);
  for (i = 1; i<20; i++) TTick[i] = TTick[i-1] + tl;
 } else {
  l /= 30.*NbSec[2];  // l et tl seront exprim‚s en mois
  //if (l>36.) tl = 12.*Grid(l/12., 0);   // > 3 ans  => fraction d‚cimale
  if (l>240.) tl = 120.;                 // > 20 ans  => 10 ans
  if (l<=240.) tl = 60.;                 //   20 ans  => 5 ans
  if (l<=120.) tl = 12.;                 //   10 ans  => 1 an
  if (l<=36.) tl = 6.;                   //    3 ans  => 6 mois
  if (l<=18.) tl = 2.;		   //  1.5 an   => 2 mois
  if (l<=10.) tl = 1.;		   //   10 mois => 1 mois
  temps = ceil(l1);  date2 = *gmtime(&temps);
  if (tl>=12.) {  // arrondi de l'ann‚e de r‚f‚rence
   date2.tm_year = ceil(date2.tm_year*12/tl)*tl/12;
   date2.tm_mon = -1;
  }
  else if (tl>=6.) date2.tm_mon = 6*ceil(date2.tm_mon/6.) - 1;
  j = date2.tm_mon + 1;
  date2.tm_mday = 1; date2.tm_hour = 0; date2.tm_min = 0; date2.tm_sec = 0;
  for (i = 0; i<20; i++) {
   while (j>11) { j -= 12;  date2.tm_year++; };
   date2.tm_mon = j;
   TTick[i] = mktime(&date2);
   j += (int)tl;
  }
  tl *= 30.*NbSec[2];  // tl exprim‚ en secondes
 }
 if (tl>=1.) strcpy(FTick, "%M:%S");
 if (tl>=60.) strcpy(FTick, "%Hh%M");
 if (tl>=NbSec[2]) strcpy(FTick, "%d %b");
 if (tl>=30.*NbSec[2]) strcpy(FTick, "%b %Y");
 if (tl>=360.*NbSec[2]) strcpy(FTick, "%Y");
 return tl;
}


//**************************************************************************
// Decode_Text: d‚code la chaŒne st (protection des titres et auteurs).
//**************************************************************************

void Decode_Text(char *st)
{
 int i;
 for (i = 0; i<strlen(st); i++) st[i] = ~(st[i] - 256) + 32;
}


//**************************************************************************
// Aff_Souris: appelle l'interruption DOS 0x33 pour la gestion des fonctions
// de la souris. Les modes admis (f) sont:
//	0 = initialise la souris
//	1 = fait apparaitre le curseur
//	2 = cache le curseur
//	3 = ‚tat des boutons et la position
//	4 = d‚place le curseur
//	5 = ‚tat des boutons et nombre de pressions
//	6 = idem pour relachement des boutons
//	7 et 8 = limite le d‚placement horiz. et vert.
//	9 = d‚finit la forme du curseur
//**************************************************************************

int Aff_Souris(int f)
{
 if ((Souris || f==0)) {
  if (VGAT==0 || (f!=1 && f!=2)) {
   regs.x.ax = f;
   int86(0x33, &regs, &regs);
  }
  if (f==0) regs.x.bx = 0;
  //if (f==1 || f==2) Draw_Souris(regs.x.cx, regs.x.dx);
  return(regs.x.ax);
 } else return(0);
}


//**************************************************************************
// Draw_Souris: trace la souris … l'emplacement x,y (en mode XOR)
//**************************************************************************

void Draw_Souris(int x, int y)
{
 setwritemode(XOR_PUT);
 setcolor(WHITE);
 setlinestyle(SOLID_LINE, 1, 1);
 line(x - 4, y, x + 4, y);
 line(x, y - 4, x, y + 4);
 //line(x - 2, y - 2, x + 2, y + 2);
 //line(x + 2, y - 2, x - 2, y + 2);
 //line(x, y, x, y);
 //line(x, y+1, x+1, y+1);
 //line(x, y+2, x+2, y+2);
 //line(x, y+3, x+3, y+3);
 //line(x, y+4, x+4, y+4);
 setwritemode(COPY_PUT);
 setlinestyle(SOLID_LINE, 1, EpLigne);
}


int huge DetectVGAMode()
{
  int Vdi;

  Vdi = VGAMode;
  return Vdi;
}


void Couleur_Ecran(signed char ce)
{
 int i;
 unsigned char RGB0[16][3] = {
  { 0, 0, 0}, 	//  0 BLACK
  { 0, 0,45},   //  1 BLUE
  { 0,45, 0},   //  2 GREEN
  { 0,45,45},	//  3 CYAN
  {45, 0, 0},	//  4 RED
  {45, 0,45},   //  5 MAGENTA
  {45,45, 0},   //  6 BROWN
  {45,45,45},   //  7 LIGHTGRAY
  {31,31,31},   //  8 DARKGRAY
  { 0, 0,63},   //  9 LIGHTBLUE
  { 0,63, 0},   // 10 LIGHTGREEN
  { 0,63,63},   // 11 LIGHTCYAN
  {63, 0, 0},   // 12 LIGHTRED
  {63, 0,63},   // 13 LIGHTMAGENTA
  {63,63, 0},   // 14 YELLOW
  {63,63,63}};  // 15 WHITE


 switch (ce) {
 case 0: // Couleurs par d‚faut: 16 / fond noir
  if (VGAT==0) setallpalette(&Pal);
  else for (i = 0; i<16; i++) {
   RVB[i][0] = RGB0[i][0];
   RVB[i][1] = RGB0[i][1];
   RVB[i][2] = RGB0[i][2];
  }
  break;
 case 1: // gris clair + blanc / fond noir
  for (i = 1; i<9; i++) {
   if (VGAT==0) setpalette(i, LIGHTGRAY);
   else {
    RVB[i][0] = RGB0[LIGHTGRAY][0];
    RVB[i][1] = RGB0[LIGHTGRAY][1];
    RVB[i][2] = RGB0[LIGHTGRAY][2];
   }
  }
  for (i = 9; i<16; i++) {
   if (VGAT==0) setpalette(i, WHITE);
   else {
    RVB[i][0] = RGB0[WHITE][0];
    RVB[i][1] = RGB0[WHITE][1];
    RVB[i][2] = RGB0[WHITE][2];
   }
  }
  break;
 case 2: // gris fonc‚ + noir / fond blanc
  if (VGAT==0) setpalette(0, WHITE);
  else {
   RVB[0][0] = RGB0[WHITE][0];
   RVB[0][1] = RGB0[WHITE][1];
   RVB[0][2] = RGB0[WHITE][2];
  }
  for (i = 1; i<9; i++) {
   if (VGAT==0) setpalette(i, DARKGRAY);
   else {
    RVB[i][0] = RGB0[DARKGRAY][0];
    RVB[i][1] = RGB0[DARKGRAY][1];
    RVB[i][2] = RGB0[DARKGRAY][2];
   }
  }
  for (i = 9; i<16; i++) {
   if (VGAT==0) setpalette(i, BLACK);
   else {
    RVB[i][0] = RGB0[BLACK][0];
    RVB[i][1] = RGB0[BLACK][1];
    RVB[i][2] = RGB0[BLACK][2];
   }
  }
  break;
 case 3: // 16 couleurs fonc‚es / fond blanc
   RVB[0][0] = RGB0[WHITE][0];
   RVB[0][1] = RGB0[WHITE][1];
   RVB[0][2] = RGB0[WHITE][2];
   RVB[15][0] = RGB0[BLACK][0];
   RVB[15][1] = RGB0[BLACK][1];
   RVB[15][2] = RGB0[BLACK][2];
  for (i = 1; i<7; i++) {
   RVB[i][0] = RGB0[i+8][0];
   RVB[i][1] = RGB0[i+8][1];
   RVB[i][2] = RGB0[i+8][2];
  }
  for (i = 9; i<15; i++) {
   RVB[i][0] = RGB0[i-8][0];
   RVB[i][1] = RGB0[i-8][1];
   RVB[i][2] = RGB0[i-8][2];
  }
  break;
 }
 if (VGAT==1) setvgapalette256(&RVB);
}


//**************************************************************************
// Init_Ecran: initialise l'‚cran VGA et d‚finit les variables : Mx, My,
// Rapx, Rapy. Affiche le logo, trace les boites, installe le gestionnaire
// de souris et red‚finit sa forme, initialise les couleurs.
//**************************************************************************

void Init_Ecran(void)
{
 char s0[128];
 int gdriver = DETECT, gmode, errorcode, ftx = 250, fty = 120;

 if (VGAT != -1) initgraph(&gdriver, &gmode, "");
 errorcode = graphresult();
 if (errorcode!=grOk) {
  sprintf(s0, "Graphics error: %s\n", grapherrormsg(errorcode));
  Sortie(s0);
 }
 Mx = getmaxx();
 My = getmaxy();
 Rapx = 640/(double)Mx;
 Rapy = 480/(double)My;
 if (VGAT==0) getpalette(&Pal);
 else setvgapalette256(&RVB);
 //setbkcolor(BLACK);
 Init_Boite();
 Couleur_Ecran(CMode);

 // ===== Affiche le logo (pendant l'importation de donn‚es).
 setviewport((Mx - ftx)/2, (My - fty)/2, (Mx + ftx)/2, (My + fty)/2, 0);
 Vide(0, ftx, 0, fty, WHITE);
 setcolor(DARKGRAY);
 rectangle(1, 1, ftx - 1, fty - 1);
 settextstyle(2, 0, 0);  setcolor(BLACK);
 Pg(Titre4, -12, 10);
 settextstyle(1, 0, 0);
 setcolor(BLACK);  Pg(Titre0, 20, 20);
 setcolor(CYAN);  Pg(Titre0, 19, 19);
 settextstyle(2, 0, 0);
 setcolor(RED);
 Pg(Titre1, -12, 62);
 Pg(Titre2, -12, 74);
 Pg(Titre3, -12, 98);
 setviewport(0, 0, Mx, My, 0);

 // ===== Initialisation du gestionnaire de souris
 if (NbFiles || Com) {
  if (Aff_Souris(0)) {
   Souris = 1;
   regs.x.cx = Mx/2; regs.x.dx = My/2; Aff_Souris(4);
   Aff_Souris(1);
  }
 }
 EcranGraph = 1;
}


static int gpixel(int x, int y)
{
 return getpixel(x, y);
}


void Save_GIF_File(void)
{

    int q,                      /* Counter */
	bits_colors = 2,        /* Bits pr primary color */
	color,                  /* Temporary color value */
	red[NUMCOLORS],         /* Red component for each color */
	green[NUMCOLORS],       /* Green component for each color */
	blue[NUMCOLORS];        /* Blue component for each color */


    /*
     *  Get the color palette, and extract the red, green and blue
     *  components for each color. In the EGA palette, colors are
     *  stored as bits in bytes:
     *
     *      00rgbRGB
     *
     *  where r is low intensity red, R is high intensity red, etc.
     *  We shift the bits in place like
     *
     *      000000Rr
     *
     *  for each component
     */
    if (VGAT==0) {
	    getpalette(&Pal);
	    for (q = 0; q < NUMCOLORS; q++) {
		color = Pal.colors[q];
		red[q]   = ((color & 4) >> 1) | ((color & 32) >> 5);
		green[q] = ((color & 2) >> 0) | ((color & 16) >> 4);
		blue[q]  = ((color & 1) << 1) | ((color & 8) >> 3);
	    }
    } else bits_colors = 6;
    GIF_Create(ExpGIFName, Mx, My, NUMCOLORS, bits_colors);
    for (q = 0; q < NUMCOLORS; q++) {
	if (VGAT) GIF_SetColor(q, RVB[q][0], RVB[q][1], RVB[q][2]);
	else GIF_SetColor(q, red[q], green[q], blue[q]);
    }
    GIF_CompressImage(0, 0, -1, -1, gpixel);
    GIF_Close();
}

//**************************************************************************
// Decode_Format: ajoute une ‚toile en fin de chaine *sg si elle ne
// comporte aucun canal (s'applique … ImpFmt ou ExpFmt), remplace les
// anciens caractŠres par le formalisme [] et renvoie le nombre de colonnes
//(nombre n‚gatif si l'‚toile est pr‚sente).
//**************************************************************************

int Decode_Format(char *sg)
{
 char *p, s0[128];
 int i, ok, c = 0;

 for (i = 0, ok = 0; i<strlen(sg); i++) {
  if (IsCanal(sg[i]) || sg[i]=='*') ok = 1;
  else if (sg[i]=='[') {
   strcpy(s0, &sg[i]);
   p = strtok(s0, "[]");
   i += strlen(p) + 1;
  }
  if (sg[i] != '*') c++;
 }
 if (ok==0) strcat(sg, "*");
 if (strchr(sg, '*')) return (-c);
 else return(c);
}


//**************************************************************************
// Decode_Ligne: … partir de la chaŒne d'information (sf) envoy‚e par la
// fonction Format(), d‚code les diff‚rentes en-tˆtes et essentiellement la
// ligne "# CHAN" qui d‚finit ImpFmt, SName[] et SUnit[].
//**************************************************************************

void Decode_Ligne(char *sf)
{
 char s0[MAXSTR], s1[MAXSTR];
 char c, *p, *p1, *p2, st[MAXCOL], n = 0, i;
 struct c_tm { time_t sec;
	 unsigned int fsec; } ct;

 if (DAS == 2) DAS = 0;		// Non DAS si aucune option sp‚cifique...
 if (strstr(sf, "# INFO:")==sf) {
  strcpy(FInfo[(NbInfo++)%MAXINFO], &sf[8]);
 }
 if (strstr(sf, "# SITE:")==sf) Site = atoi(&sf[8]);
 if (strstr(sf, "# UDAS:")==sf) UDAS = atoi(&sf[8]);
 if (strstr(sf, "# TITL:")==sf && !Calib) {
  strcpy(FTitre, &sf[8]);
  p = strchr(FTitre, CR);
  if (p!=NULL) p[0] = 0;
  ANSI2OEM(FTitre);
 }
 if (strstr(sf, "# SHOT:")==sf) {
  DAS = 1;
  MaxVal = atol(&sf[8]);
 }
 if (strstr(sf, "# SAMP:")==sf) {

  // ===== si 2Šme en-tˆte dans le fichier: concat‚nation
  if (NbSF>0) {
   nf++;  NbFiles++;
   ct.sec = -1l;
   fwrite(&ct.sec, 4, 1, f_tmp0);
   FStamp[nf] = ftell(f_tmp0);
   Append = 0;
   DCh += Cnx[nf-1];
   X = 0l;
  }
  Acqu[nf] = atof(&sf[8]);
  Acmin = Min(Acmin, Acqu[nf]);
  Acmax = Max(Acmax, Acqu[nf]);
  c = sf[strlen(sf)-1];
  if (c=='m') Acqu[nf] *= NbSec[4];
  if (c=='h') Acqu[nf] *= NbSec[3];
  NbSF++;
 }
 if (strstr(sf, "# CHAN:")==sf) {
  strcpy(st, "");
  strcpy(s0, sf),
  p = strtok(&s0[8], " ");
  while (p) {
   p1 = strchr(p, CR);  if (p1) p1[0] = 0;
   p1 = strchr(p, LF);  if (p1) p1[0] = 0;
   if (strlen(p)==0);
   else if (!strcmp(p, "BINARY")) strcat(st, "0");
   else if (!strcmp(p, "ID")) strcat(st, "i");
   else if (!strcmp(p, "DEG") || !strcmp(p, "RAD")) {
	 strcat(st, "i");
	 TimeMode = 3;
	 strcpy(Phase, strlwr(p));
	}
   else if (!strcmp(p, "DATENUM")) strcat(st, "t");
   else if (!strcmp(p, "YY") || !strcmp(p, "YF")) strcat(st, "[yy]");
   else if (!strcmp(p, "YYYY")) strcat(st, "y");
   else if (!strcmp(p, "MM") || !strcmp(p, "MO")) strcat(st, "m");
   else if (!strcmp(p, "DD")) strcat(st, "d");
   else if (!strcmp(p, "HH")) strcat(st, "h");
   else if (!strcmp(p, "MI") || !strcmp(p, "NN")) strcat(st, "n");
   else if (!strcmp(p, "SS") || !strcmp(p, "UnixTime")) strcat(st, "s");
   else if (!strcmp(p, "ND1") || !strcmp(p, "D1") || !strcmp(p, "NDAY1")) strcat(st, "f");
   else if (!strcmp(p, "ND0") || !strcmp(p, "D0") || !strcmp(p, "NDAY0")) {
	 strcat(st, "f");
	 STime += 24.;
	}
   else if (!strcmp(p, "BB")) strcat(st, "b");
/* else if (!strcmp(p, "YYMMDDHHNNSS")) strcat(st, "t");
   else if (!strcmp(p, "YYYYMMDD")) strcat(st, "g");
   else if (!strcmp(p, "HHNNSS")) strcat(st, "l");
   else if (!strcmp(p, "HHNN")) strcat(st, "k");
   else if (!strcmp(p, "HH.NN")) strcat(st, "q"); */
   else if (strspn(p, "YMDHNSJ.")==strlen(p)) {
    sprintf(s0, "[%s]", p);
    strcat(st, strlwr(s0));
   } else {
    if (!Calib) {
     strcpy(s1, p);
     if ((p1 = strstr(s1, "_("))!=NULL) {
      if ((p2 = strstr(p1, ")"))!=NULL) p2[0] = NULL;
      strcpy(SUnit[n+DCh], p1 + 2);
      p1[0] = NULL;
      ANSI2OEM(SUnit[n+DCh]);
     }
     sscanf(s1, "%s", &SName[n+DCh]);
     ANSI2OEM(SName[n+DCh]);
    }
    sprintf(s1, "%c", CarCan(n++));
    strcat(st, s1);
   }
   p = strtok(NULL, " ");
  }
  if (AutoFmt==1) strcpy(ImpFmt, st);
  if (st[0]=='0') strcpy(ImpFmt, "BINARY");
  if (NbSF>1) {
   Cnx[nf] = n;  Canx += n;
   strcpy(Fmt[nf], "");
   for (i = 0; i<strlen(ImpFmt); i++) strcat(Fmt[nf], "%lf ");
  }
 }
}


//**************************************************************************
// Faire_Nom_Export: construit un nom de fichier automatique (s'il n'a pas
// ‚t‚ totalement sp‚cifi‚ par l'option "/o:") en fonction des fichiers d‚j…
// existants et le stocke dans FF_Out.
//**************************************************************************

void Faire_Nom_Export(void)
{
 FILE *fid;
 char *p, s0[5];
 int i, exp = 0;

 do {
  strcpy(FF_Out, FC_Out);
  strcpy(s0, "");
  if (strstr(FC_Out, "*")) {
   if ((p = strstr(FF_In,".")) == NULL) strcpy(FF_Out, FF_In);
   else {
    i = strlen(FF_In) - strlen(p);
    strncpy(FF_Out, FF_In, i);
    FF_Out[i] = NULL;
   }
  }
  if (strstr(FC_Out, ".")) break;
  sprintf(s0, ".$%02d", exp++%100);
  strcat(FF_Out, s0);
  if ((fid = fopen(FF_Out, "rt")) != NULL) fclose(fid);
 }
 while (fid!=NULL);
}


//**************************************************************************
// Export_Data: exporte dans le fichier FF_Out toutes les donn‚es de
// l'interval de temps affich‚, suivant le format ExpFmt (compl‚t‚ si
// incomplet). Ecrit une en-tˆte suivant Header et concatŠne ou fusionne les
// fichiers multiples suivant Merge.
//**************************************************************************

void Export_Data(void)
{
 FILE *f_out;
 char *p, s0[MAXSTR], s1[MAXSTR], c, tfr,
      ey = OC, em = OC, eh = OC, en = OC;
 int i, j, k;
 double dn[2][MAXCNX], de[2][MAXCNX], te[2] = {0.,0.};
 struct tm date;
 struct c_tm { time_t sec;
	 unsigned int fsec; } ct;
 time_t tt;

 if (Merge) Faire_Fusion();

 // ===== ouverture des fichiers (temporaire et export)
 if ((f_tmp0 = fopen(FF_Tmp0, "rb")) == NULL) Sortie("Tmp0 ORB pb.");
 if ((f_out = fopen(FF_Out, "wt")) == NULL) Sortie("Cannot open output file.");

 if (EcranGraph) Efface_Menu(0);
 if (!strlen(ExpFmt)) strcpy(ExpFmt, ImpFmt);
 Decode_Format(ExpFmt);
 Limite_Nom(FF_Out, s0, 22/Rapx);
 sprintf(s1, "Exporting '@%s@' with format '@%s@'...", s0, ExpFmt);
 Aff_Message(s1, 0, 0);

 // ===== d‚finition des formats: nombres et s‚parateurs
 if (FISO) {
  if (strstr(ExpFmt,"ym") || strstr(ExpFmt,"yj")) ey = '-';
  if (strstr(ExpFmt,"md")) em = '-';
  if (strstr(ExpFmt,"hn")) eh = ':';
  if (strstr(ExpFmt,"ns")) en = ':';
 }
 if (!MDiff) sprintf(Fmt[0], "%s%c", "%05.f", OC);
 else sprintf(Fmt[0], "%s%c", DataFmt, OC);
 if (Frac(Acmin)) sprintf(Fmt[1], "%s%c", "%03.9lf", OC);
 else sprintf(Fmt[1], "%s%c", "%03.6lf", OC);

 // ===== ‚criture de l'en-tˆte EDAS
 if (strcmp(Header,"n")) {
  tt = time(NULL);
  date = *gmtime(&tt);
  strftime(s0, 20, "%Y-%m-%d %X", &date);
  fprintf(f_out, "# DATE: %s\n", s0);
  fprintf(f_out, "# PROG: %s\\mgr %s\n", getcwd(s0, 128), LCommand);
  if (Site) fprintf(f_out, "# SITE: %04d\n", Site);
  if (UDAS) fprintf(f_out, "# UDAS: %03d\n", UDAS);
  if (strlen(FTitre)) fprintf(f_out, "# TITL: %s\n", FTitre);
  if (DAS && Calib<=0) fprintf(f_out, "# SHOT: %ld\n", MaxVal);
  if (NbInfo && !XInfo) {
   for (i = 0; i<NbInfo; i++) fprintf(f_out, "# INFO: %s", FInfo[i]);
  }
  if (TimeMode==3) {
   if (HiCum) {
    sprintf(s0, "# INFO: HiCum analysis (%s)\n", Doodson);
    fprintf(f_out, "%s", s0);
   }
   for (i = 0; i<Canx; i++) {
    sprintf(s0, "# INFO: Chan. %d Harm.", i+1);
    for (j = 0; j<MAXHARM; j++) {
     sprintf(s1, " #%d: Phase = %+06.2fø Ampli = %g,", j+1, HarmS[j][i], HarmC[j][i]);
     strcat(s0, s1);
    }
    s0[strlen(s0)-1] = '\n';
    fprintf(f_out, "%s", s0);
   }
  }
 }

 X = Y = 0l; nf = 0; DCh = 0;
 do {
  te[1] = te[0];

  // ===== lecture du temps (signed long int)
  if (fread(&ct.sec, 4, 1, f_tmp0)<1) break;

  // ===== si =-1 : marque de d‚but d'un fichier concat‚n‚
  if (ct.sec==-1l) {
   if (!Append) {
    DCh += Cnx[nf];
    Y = 0l;
   }
   nf++;
   fread(&ct.sec, 4, 1, f_tmp0); // vraie lecture du temps
  }

  // ===== lecture de la fraction de temps (float)
  fread(&ct.fsec, 2, 1, f_tmp0);
  te[0] = ct.sec + ct.fsec/MSEC;
  if (Y==0l) te[1] = te[0];

  // ===== lecture des donn‚es (1 double par canal)
  for (i = 0; i<Cnx[nf]; i++) {
   de[1][i] = de[0][i];
   if (fread(&de[0][i], 8, 1, f_tmp0)<1) break;
   if (Y==0l) de[1][i] = de[0][i];
   Eval_RL(i, dn, de, te);
  }

  // ===== ‚criture des en-tˆtes SAMP et CHAN
  if (Y==0l && strcmp(Header,"n")) {
   if (Acqu[nf]) fprintf(f_out, "# SAMP: %gs\n", Decim*Acqu[nf]);
   strcpy(s0, "# CHAN:");
   for (i = 0; i<strlen(ExpFmt); i++) {
    j = IsCanal(c = ExpFmt[i]);
    if (c=='i') if (TimeMode==3) strcat(s0, " DEG"); else strcat(s0, " ID");
    if (c=='y' || c=='Y') strcat(s0, " YYYY");
    if (c=='m') strcat(s0, " MM");
    if (c=='d') strcat(s0, " DD");
    if (c=='h') strcat(s0, " HH");
    if (c=='n') strcat(s0, " NN");
    if (c=='s') strcat(s0, " SS");
    if (c=='j' || c=='f') strcat(s0, " D1");
    if (c=='b') strcat(s0, " BB");
    //if (c=='t') strcat(s0, " YYMMDDHHNNSS");
    if (c=='t') strcat(s0, " DATENUM");
    if (c=='g') strcat(s0, " YYYYMMDD");
    if (c=='l') strcat(s0, " HHNNSS");
    if (c=='k') strcat(s0, " HHNN");
    if (c=='q') strcat(s0, " HH.NN");
    if (c=='x') strcat(s0, " No_Data");
    if (c=='[') {
     strcpy(s1, &ExpFmt[i]);
     p = strtok(s1, "[]");
     strcat(s0, " ");  strcat(s0, strupr(p));
     i += strlen(s1);
    }
    if (j>0 && j<=Cnx[nf]) {
     sprintf(s1, " %s_(%s)", SName[j+DCh-1], SUnit[j+DCh-1]);
     strcat(s0, s1);
    }
    if (c=='*') for (j = 0; j<Cnx[nf]; j++) {
		  sprintf(s1, " %s_(%s)", SName[j+DCh], SUnit[j+DCh]);
		  strcat(s0, s1);
		 }
   }
   fprintf(f_out, "%s\n", s0);
  }

  // ===== ‚criture de la ligne de donn‚es (si dans la fenˆtre de temps)
  if (te[0]>=Zmin && te[0]<=Zmax) {
   date = *gmtime(&ct.sec);
   strcpy(Str, "");
   for (i = 0; i<strlen(ExpFmt); i++) {
    c = ExpFmt[i];
    j = IsCanal(c);
    if (ExpFmt[i+1]=='.') { tfr = 1; i++; } else tfr = 0;
    if (c=='i') { sprintf(s0, "%lg%c", te[0], OC); strcat(Str, s0); }
    //if (c=='y') { sprintf(s0, "%02d%c", date.tm_year, OC); strcat(Str s0); }
    if (c=='y' || c=='Y') { sprintf(s0, "%04d%c", date.tm_year + 1900, ey); strcat(Str, s0); }
    if (c=='m') { sprintf(s0, "%02d%c", date.tm_mon + 1, em); strcat(Str, s0); }
    if (c=='b') { strftime(s0, 5, "%b", &date); strcat(Str, strupr(s0)); strcat(Str, OS); }
    if (c=='d') {
     if (strchr(ExpFmt, 'm') || strchr(ExpFmt, 'b'))
      sprintf(s0, "%02d%c", date.tm_mday, OC);
     else if (strchr(ExpFmt, 'y')) sprintf(s0, "%03d%c", date.tm_yday + 1, OC);
	  else sprintf(s0, "%ld%c", ct.sec/(long)NbSec[2] + 1l, OC);
     if (tfr) {
      sprintf(s1, Fmt[1], atof(s0) + Frac((ct.sec + ct.fsec/MSEC)/NbSec[2]));
      strcpy(s0, s1);
     }
     strcat(Str, s0);
    }
    if (c=='h') {
     if (strchr(ExpFmt, 'd') || strchr(ExpFmt, 'j'))
      sprintf(s0, "%02d%c", date.tm_hour, eh);
     else sprintf(s0, "%ld%c", ct.sec/(long)NbSec[3], OC);
     if (tfr) {
      sprintf(s1, Fmt[1], atof(s0) + Frac((ct.sec + ct.fsec/MSEC)/NbSec[3]));
      strcpy(s0, s1);
     }
     strcat(Str, s0);
    }
    if (c=='n') {
     if (strchr(ExpFmt, 'h')) sprintf(s0, "%02d%c", date.tm_min,en);
     else if (strchr(ExpFmt, 'd') || strchr(ExpFmt, 'j'))
	   sprintf(s0, "%ld%c", ct.sec%(long)(NbSec[4]*NbSec[2]), OC);
	  else sprintf(s0, "%ld%c", ct.sec/(long)NbSec[4], OC);
     if (tfr) {
      sprintf(s1, Fmt[1], atof(s0) + Frac((ct.sec + ct.fsec/MSEC)/NbSec[4]));
      strcpy(s0, s1);
     }
     strcat(Str, s0);
    }
    if (c=='s') {
     if (strchr(ExpFmt, 'n')) sprintf(s0, "%02d", date.tm_sec);
     else if (strchr(ExpFmt, 'h')) sprintf(s0, "%ld", ct.sec%(long)NbSec[3]);
	  else if (strchr(ExpFmt, 'd') || strchr(ExpFmt, 'j'))
		sprintf(s0, "%ld", ct.sec%(long)NbSec[2]);
     //else if (strchr(ExpFmt, 'y')) sprintf(s0, "%ld", ct.sec - date.tm_yday*NbSec[2]);
	       else sprintf(s0, "%ld", ct.sec + 1);
     if (ct.fsec) { sprintf(s1, ".%03d%c", ct.fsec, OC); strcat(s0, s1); }
     else strcat(s0, OS);
     strcat(Str, s0);
    }
    if (c=='j') {
     if (!tfr) {
      sprintf(s0,"%03d%c", date.tm_yday + 1, OC);
      strcat(Str, s0);
     } else c = 'f';
    }
    if (c=='f') {
     sprintf(s0, Fmt[1], date.tm_yday + 1
			 + Frac((ct.sec + ct.fsec/MSEC)/NbSec[2]));
     strcat(Str, s0);
    }
    //if (c=='t') { strftime(s0, 20, "%y%m%d%H%M%S", &date); strcat(Str, s0); strcat(Str, OS); }
    if (c=='t') {
     sprintf(s0, "%10.5f", (ct.sec + ct.fsec/MSEC)/(double)NbSec[2] + Date1970);
     strcat(Str, s0); strcat(Str, OS);
    }
    if (c=='g') { strftime(s0, 10, "%Y%m%d", &date); strcat(Str, s0); strcat(Str, OS); }
    if (c=='l') { strftime(s0, 10, "%H%M%S", &date); strcat(Str, s0); strcat(Str, OS); }
    if (c=='k') { strftime(s0, 10, "%H%M", &date); strcat(Str, s0); strcat(Str, OS); }
    if (c=='q') { strftime(s0, 10, "%H.%M", &date); strcat(Str, s0); strcat(Str, OS); }

    if (c=='[') {
     while ((c = ExpFmt[i++])!=']') {
      if (c== '\0') break;
      k = 1;
      strcpy(s1, "000000000000");
      if (tolower(c)=='y') {
       while ((c = tolower(ExpFmt[i++]))=='y') k++;
       strftime(s0, 10, "%Y", &date);
       strcat(s1, s0);
       strcat(Str, &s1[strlen(s1)-k]);
       i--;
      }
      else if (tolower(c)=='m') {
       while ((c = tolower(ExpFmt[i++]))=='m') k++;
       strftime(s0, 10, "%m", &date);
       strcat(s1, s0);
       strcat(Str, &s1[strlen(s1)-k]);
       i--;
      }
      else if (tolower(c)=='d') {
       while ((c = tolower(ExpFmt[i++]))=='d') k++;
       strftime(s0, 10, "%d", &date);
       strcat(s1, s0);
       strcat(Str, &s1[strlen(s1)-k]);
       i--;
      }
      else if (tolower(c)=='h') {
       while ((c = tolower(ExpFmt[i++]))=='h') k++;
       strftime(s0, 10, "%H", &date);
       strcat(s1, s0);
       strcat(Str, &s1[strlen(s1)-k]);
       i--;
      }
      else if (tolower(c)=='n') {
       while ((c = tolower(ExpFmt[i++]))=='n') k++;
       strftime(s0, 10, "%M", &date);
       strcat(s1, s0);
       strcat(Str, &s1[strlen(s1)-k]);
       i--;
      }
      else if (tolower(c)=='s') {
       while ((c = tolower(ExpFmt[i++]))=='s') k++;
       strftime(s0, 10, "%S", &date);
       strcat(s1, s0);
       strcat(Str, &s1[strlen(s1)-k]);
       i--;
      }
      else if (tolower(c)=='j') {
       while ((c = tolower(ExpFmt[i++]))=='j') k++;
       strftime(s0, 10, "%j", &date);
       strcat(s1, s0);
       strcat(Str, &s1[strlen(s1)-k]);
       i--;
      }
      else if (c=='.') strcat(Str, ".");
      //else if (c!='[')
      // sprintf(s0, "%c",c);
      // strcat(Str, s0);
   /* else if (j>0 && j<Cnx[nf]) {
       while ((c = ExpFmt[i++])==(j+'0')) k++;
       sprintf(s0, "%%0%d
       sprintf(s1, "%", k);
       strcpy(s0, &s1[strlen(s1)-k]);
       strcat(Str, s0);
       i--;
      } */
     }
     strcat(Str, OS);
     i--;
    }

    if (c=='x') { sprintf(s0, Fmt[0], NaN); strcat(Str, s0); }

    if (j>0 && j<=Cnx[nf]) {
     if (Shot) sprintf(s0, "%05.f%c", MaxVal*Frac(dn[0][j-1]/(double)MaxVal + 1000.), OC);
     else sprintf(s0, Fmt[0], dn[0][j-1]);
     strcat(Str, s0);
    }
    if (c=='*') for (j = 0; j<Cnx[nf]; j++) {
     if (Shot) sprintf(s0, "%05.f%c", MaxVal*Frac(dn[0][j]/(double)MaxVal + 1000.), OC);
     else sprintf(s0, Fmt[0], dn[0][j]);
     strcat(Str, s0);
    }
   }
   Str[strlen(Str)-1] = LF;	// remplace le dernier espace par LF
   fputs(Str, f_out);
  }
  Working(X/(double)NbY, -1.);
  X++;
  Y++;
 }
 while (X<NbY);
 Working(1., -1.);
 fclose(f_tmp0);
 fclose(f_out);
}


//**************************************************************************
// Aff_Menu: Affiche le menu en mode graphique, et d‚finit Menu si m != -1:
//	0 = menu principal
//	1 = menu File
//	2 = menu Data
//	3 = menu Graph
//	4 = menu View
//	5 = menu Screen
//	6 = menu Info
//	7 = menu Help
//      8 = menu DataFit
//**************************************************************************

void Aff_Menu(int m)
{
 char s0[256];

 if (m!=-1) Menu = m;
 Aff_Souris(2);
 Efface_Menu(0);
 switch (Menu) {
  case 0:
   sprintf(s0,"Menu (@Alt@):  @F@ile   D@A@ta   @V@iew   @G@raph   Scree@N@   "
	      "@I@nfo   @H@elp     "
	      "@1@..@%c , - * rygcbm v e@ Draw  @ñ <-> Esc@ @Z@oom", CarCan(Canx-1));
   break;
  case 1:
   strcpy(s0, "File:  @F1@ Export   @ShiftF1@ Make GIF   ");
   if (NbFiles>1 && !Append) strcat(s0, "@AltF1@ Merge   ");
   if (!Sort && Append) strcat(s0, "@CtrlF1@ Sort data   ");
   strcat(s0, "@F10@ E@X@it");
   break;
  case 2:
   strcpy(s0, "Data (@Alt@):  @O@ffset   @L@inear   @D@erivative   @C@umSum   Fil@T@er   ");
   if (TimeMode==3) strcat(s0, "@W@aveHarm   ");
   strcat(s0, "@S@pike       @F3@ Undo");
   if (Append) strcat(s0, "   @F8@ Formula");
   if (!TimeMode) strcat(s0, "   @ShiftF8@ HiCum");
   break;
  case 3:
   strcpy(s0, "Graph:  @F2@ Info  @AltF2@ Names  @ShiftF2@ Title  @CtrlF2@ Reset Colors"
	      "  @F6@ Line  @CtrlF6@ Marker  @ShiftF6@ Grid  @AltF6@ Bold");
   break;
  case 4:
   strcpy(s0, "View:  @F4@ Normalize   @F5@ Magnify   @ShiftF5@ Equal scales");
   if (!DMode) strcat(s0, "   @F7@ DataFit");
   if (DAS) strcat(s0, "   @ShiftF7@ OverScale");
   break;
  case 5:
   strcpy(s0, "Screen:  @F9@ Colour/B&W   @ShiftF9@ VGA Mode   @CtrlF9@ Full Screen");
   break;
  case 6:
   sprintf(s0, "Info:   @1@..@%c * @ or mouse to select channels   @Enter@ to valid", CarCan(Canx-1));
   if (TimeMode==3) strcat(s0, "   @F2@ or @AltI@ to view harmonics");
   strcat(s0,"     @Esc@ to return");
   break;
  case 7:
   sprintf(s0, "Help:   @0@..@%d <->@ Select pages    @PgDn PgUp@ Scroll                       "
	      "                         @Esc@ Exit", NbPage);
   break;
  case 8:
   sprintf(s0, "Data Fit Preview:   @F7@ Next/Exit     @ENTER@ Apply correction");
   break;
  default: strcpy(s0, "");
 }
 Pg2(s0, -13, -14, 2);
 Aff_Souris(1);
}



//**************************************************************************
// Annule_Styles: remplace tout caractŠre de style ou de couleur dans *sg
// (ChDraw) par un espace (neutre)
//**************************************************************************

void Annule_Styles(char *sg)
{
 int i;

 for (i = 0; i<strlen(sg); i++)
  if (strchr(Graph_String, sg[i])>Graph_String+7) sg[i] = ' ';
}



//**************************************************************************
// FF1: touche "F1" = exportation des donn‚es dans un fichier.
//**************************************************************************

void FF1(void)
{
 char s0[128], sca[MAXCNX];
 int i;

 // ===== Construit la liste de canaux affich‚s
 strcpy(sca, "");
 for (i = 0; i<Canx; i++) {
  if (Gn[i]) {
   sprintf(s0, "%c", CarCan(i));
   strcat(sca, s0);
  }
 }

 Faire_Nom_Export();
 if (Entrer_Chaine("Output file name: ", "", FF_Out, 5)) {
  if (NbFiles>1 && Append==0) {
   if (Merge) strcpy(s0, "y");  else  strcpy(s0, "n");
   Entrer_Chaine("Merge files (Y/N): ","yn", s0, 2);
   if (!strcmp(strupr(s0),"Y")) {
    Merge = 1;
    sprintf(ExpFmt, "ymdhns%s", sca);
   }
   else Merge = 0;
  }
  if (TimeMode==1) sprintf(ExpFmt, "%s", sca);
  else if (TimeMode==2 || TimeMode==3) sprintf(ExpFmt, "i%s", sca);
  else if (FISO) sprintf(ExpFmt, "ymdhns%s", sca);
  else if (Calib>0 || !Append) sprintf(ExpFmt, "yj.%s", sca);
  if (!strlen(ExpFmt)) strcpy(ExpFmt, ImpFmt);
  strcpy(s0, ExpFmt);
  if (Entrer_Chaine("Output file format: ", Format_String, s0, 1)) strcpy(ExpFmt, s0);
  if (NbFiles==1 || Append==1 || Merge==1) Entrer_Chaine("Include text header (Y/N): ", "yn", Header, 2);
  if (strlen(ExpFmt)) Export_Data();
 }
 Aff_Menu(-1);
}


//**************************************************************************
// Aff_Aide: Affiche les pages d'aide … partir du fichier "mgr_help.dat".
//**************************************************************************

void Aff_Aide(int page)
{
 FILE *f_hlp;
 char cc, s0[128], FF_Help[128];
 int nbl = 33/Rapy, nlg0 = 0, nlg, nll, npg;

 if (PEcran) nbl += 5;
 sprintf(FF_Help, "%smgr_help.dat", MGR_dir);
 do {
  if ((f_hlp = fopen(FF_Help,"rt")) == NULL) {
   Aff_Message("Cannot open help file... Press any key to exit.", 0, 1);
   while (!kbhit());
   return;
  }
  fscanf(f_hlp, "%d", &NbPage);
  Efface_Graph();
  Aff_Menu(7);
  Pg("", 20, Yff);
  nlg = nll = 0;
  do {
   fgets(s0, 128, f_hlp);
   if (!strcmp(s0, "##")) break;
   if (s0[0]=='#') npg = atoi(&s0[1]);
   else {
    if (npg==page || (npg==-1 && page<MENUPAGE)) {
     if (s0[0]!='.') s0[strlen(s0)-1] = NULL;
     else strcpy(s0, " ");
     if (nlg<nbl && nll>=nlg0) { Pg2(s0, 20, -16, 0);  nlg++; }
     nll++;
    }
   }
  }
  while (1);
  fclose(f_hlp);
  if (nll>nlg && nlg==nbl) {
   Pg2("@ @", -13, My - 22, 0);
   Pg2("@ @", -14, My - 22, 0);
  }
  if (nlg0>0) {
   Pg2("@ @", -13, Yff + 4, 0);
   Pg2("@ @", -14, Yff + 4, 0);
  }
  if (page==NbPage) { sprintf(s0, "@%08ld@", SER);  Pg2(s0, -11, -11, 0); }
  while (!kbhit()) { if (!PEcran) Aff_Horloge(); }
  if ((cc = getch()) == 0) {
   cc = getch();
   if (cc==AltH || cc==AltX || cc==F10) break;
/* if (cc==AltP) {
    Efface_Menu(0);
    Pg2("Please wait... Printing help pages to standard printer.", -12, -14, 2);
    f_hlp = fopen(FF_Help,"rt");
    fscanf(f_hlp, "%d", &NbPage);
    do {
     fgets(s0, 128, f_hlp);
     if (!strcmp(s0, "##")) break;
     if (s0[0]=='.') fprintf(stdprn, "\r\n");
     else if (s0[0]=='#') {
      page = atoi(&s0[1]);
      //if (page>1 && page<=NbPage) fprintf(stdprn, "\f");
      if (page>1 && page<=NbPage) fprintf(stdprn, "\r\n\r\n");
      if (page==0) fprintf(stdprn, "ÄÄÄÄÄÄÄ þ Quick Help on %s %s ÄÄÄÄÄÄ\r\n\n",
				   Titre0, Titre1);
     } else {
      strcpy(s1, "");
      for (i = 0; i<strlen(s0); i++) if (s0[i]!='@' || s0[i+1]=='@') {
       sprintf(s2, "%c", s0[i]);
       strcat(s1, s2);
      }
      fprintf(stdprn, "%s\r", s1);
     }
    } while (1);
    fclose(f_hlp);
   } */
   if (cc==LEFT) { page = (page + NbPage)%(NbPage+1);  nlg0 = 0; }
   if (cc==RIGHT) { page = (page + 1)%(NbPage+1);  nlg0 = 0; }
   if (nlg==nbl) {
    if (cc==ARDN) nlg0 ++;
    if (cc==PGDN) nlg0 += nbl;
   }
   if (nlg0>0) {
    if (cc==ARUP) nlg0 --;
    if (cc==PGUP) nlg0 = max(0, nlg0 - nbl);
   }
  } else {
   page = (int)(cc - '0');
   nlg0 = 0;
   if (cc==ESC) break;
  }
  if (page<0 || page>NbPage) { page = 0;  Beep(); }
 } while (1);
 Menu = 0;
}


void Decode_v8(void)
{
 sscanf(Str, "!HI %d %d %lg %d %s %s %s %s", &Site, &UDAS, &Acqu[nf], &Cnx[nf],
	     &SName[0], &SName[1], &SName[2], &SName[3]);
 //for (i = 0; i<Cnx[nf]; i++) strcpy(SUnit[i], "Hz");
}


void Lire_RS(void)
{
 while (ComTete==ComFin) { 		// attente d'un caractŠre dans le buffer
  Aff_Horloge();
  if (kbhit()) if (getch() == ESC) Sortie("");
 }
 Car = ComBuf[ComTete++];
 if (ComTete==TAILLE_BUF) ComTete = 0;		// d‚calage du buffer
}


void Lire_Chaine(void)
{
 char s0[MAXSTR];

 strcpy(Str, ""); Car = 0;
 while (Car!=CR)  {
  sprintf(s0, "%c", Car);
  if (strlen(Str)<MAXSTR-1) strcat(Str, s0);     //  þ concat‚nation
  Lire_RS();                                     //  þ lecture d'un caractŠre
 }
}


void Ecrire_RS(char c)
{
  while ((inportb(Port + 5)&0x20)==0);   // registre d'envoi disponible
  outportb(Port, c);
}


void Ecrire_Chaine(char *sf)
{
 while (*sf) {
  delay(100);
  Ecrire_RS(*sf++);
 }
}


//void RS_int(void)
void interrupt RS_int(void)
{
 char etat, cc;

 outportb(0x20, 0x20);		// signal d'acquiescement

 etat = inportb(Port + 5);
 if (etat&1) {
  cc = inportb(Port);
  ComBuf[ComFin++] = cc;
  if (cc=='*' && NDAS==0) Ecrire_RS(cc);  // demande la donn‚e au æDAS...
  if (ComFin==TAILLE_BUF) ComFin = 0;
  if (ComFin==ComTete) {
   ComTete++;
   if (ComTete==TAILLE_BUF) ComTete = 0;
  }
 }
}


void Init_RS(void)
{
 unsigned char int_pic;
 int diviseur, octpar;
 const long baud[9] = {150L, 300L, 600L, 1200L, 2400L, 4800L, 9600L, 19200L, 38400L};

 diviseur = (int) (115200L / baud[NumBaud]);
 disable();
 inportb(0x21);			// sauvegarde de la ligne s‚rie
 outportb(Port + 3, 0x80);
 outportb(Port, diviseur);
 outportb(Port + 1, diviseur >> 8);
 octpar = DataBit;
 octpar |= StopBit << 2;
 octpar |= Parity << 3;
 outportb(Port + 3, octpar);

 old_RS = getvect(Com == 1 ? 0x0c:0x0b);
 setvect(Com==1 ? 0x0c:0x0b, RS_int); // Installer l'adresse de ma routine
 int_pic = inportb(0x21);             // Lire l'Interrupt Mask Registre du PIC
 int_pic &= ~(Com==1 ? 0x10:0x08);    // Autoriser IRQ4 et IRQ5
 /* int_pic &= Com==1 ? 0xef:0xf7; */
 outportb(0x21, int_pic);             // Et le renvoyer dans le PIC

 outportb(Port + 4, Flag_MCR=0x0b);   // DTR, RTS, OUT1 et OUT2 actifs
 /* outportb(Port + 4, 0x08); */
 outportb(Port + 1, 0);               // Interdire les INT momentan‚ment
 inportb(Port + 5);
 inportb(Port + 6);
 inportb(Port + 2);
 inportb(Port);
 outportb(Port + 1, 1);  	      // Autoriser les INT
 outportb(0x20, 0x20);                // Fin d'INT envoy‚ au PIC
 enable();
}


void AttendreCOM(void)
{
 char *p, s0[MAXSTR];
 int i;

 X = 0l; Y = 0l; Car = 0; nf = 0; NbFiles = 1; Info = 1; Decim = 1;
 Init_RS();
 Aff_Souris(2);
 if (NDAS) {
  Efface_Menu(0);
  sprintf(s0, "Trying connection to æDAS @nø%d@ on @Com%d@...", NDAS, Com);
  Pg2(s0, -13, -14, 2);
  sprintf(s0, "-%03d\r", NDAS);
  do {
   Ecrire_Chaine(s0);
   Lire_Chaine();
  } while (strstr(Str, "!HI ")==NULL);
  Decode_v8();
  Acmin = Acqu[nf];
  Canx = Cnx[nf];
 }
 do {
  Efface_Menu(0);
  sprintf(s0, "Waiting for æDAS on @Com%d@ : @%ld@ data received.", Com, X);
  Pg2(s0, -13, -14, 2);
  if (NDAS || X!=0l) Init_Texte();
  Lire_Chaine();
  if ((p = strstr(Str, "*")) != NULL || NDAS) {
   if (NDAS) {
    strcpy(Fmt[nf], "%lf %lf %lf %lf %lf %lf");
    for (i = 0; i<Cnx[nf]; i++) strcat(Fmt[nf], " %lf");
   } else {
    strcpy(Fmt[nf], "%12lf");
    Canx = Cnx[nf] = (strlen(p) - 13) / 6;
    for (i = 0; i<Cnx[nf]; i++) strcat(Fmt[nf], "%5lf");
   }
   if (Extract_Data(0)!=1) {
    if (!NDAS && X==1l) Acmin = Acqu[nf] = Dte[0] - Dte[1];
    //sprintf(s0, "%lf %lf",Dte[0],Dte[1]);  Pg(s0,-14,-14);
    if (NDAS && X==0l) {
     do { Ecrire_Chaine("#E1\r");  Lire_Chaine(); }
     while (strstr(Str, "!E1")==NULL);
    }
    MAJ_Data(1);
   }
  }
 } while (X<2l);
 NbY = Y;
 for (i = 0; i<Cnx[nf]; i++) sprintf(Label[i], "Channel %c", CarCan(i));
 fclose(f_tmp0);
 Aff_Souris(1);
 Info = 0;
}

/*int save_screen(void far *buf[4])
{
 unsigned size;
 int ystart = 0, yend, yincr, block;

 yincr = (My + 1) / 4;
 yend = yincr;
 //size = imagesize(0, ystart, Mx, yend);
 size = Mx*My*4;
 for (block = 0; block<=3; block++) {
  if ((buf[block] = farmalloc(size)) == NULL) return (0);
  getimage(0, ystart, Mx, yend, buf[block]);
  ystart = yend + 1;
  yend += yincr + 1;
 }
 return (1);
}*/

int save_screen(void far *buf)
{
 if ((buf = farmalloc(4+((long)Mx)*My)) == NULL) return (0);
 getimage(0, 0, Mx, My, buf);
 return (1);
}

/*void restore_screen(void far *buf[4])
{
 int ystart = 0, yend, yincr, block;

 yincr = (My + 1) / 4;
 yend = yincr;
 for (block = 0; block<=3; block++) {
  putimage(0, ystart, buf[block], COPY_PUT);
  farfree(buf[block]);
  ystart = yend + 1;
  yend += yincr + 1;
 }
}*/

void restore_screen(void far *buf)
{
 putimage(0, 0, buf, COPY_PUT);
 farfree(buf);
}


//**************************************************************************
// Grille: trace une ligne pointill‚e … la coordonn‚e relative vg1 suivant
// la valeur de dg:
//	dg = 0: axe Y
//	dg = 1: axe X
//	dg > 1: rectangle entre vg1 et vg2, remplissage dg (book)
//**************************************************************************

void Grille(double vg1, double vg2, int dg)
{
 double a, da = M_PI/16.;

 if (ExpPS) {
  f_ps = fopen(ExpPSName,"at");
  fprintf(f_ps, "appel Grille...\n");
  fclose(f_ps);
 } else {
  setlinestyle(USERBIT_LINE, 0xAAAA, 1);       // vrais pointill‚s!
  if (GdView) setcolor(BLACK);
  else setcolor(DARKGRAY);
  if (dg==0) {
   if (Polar) circle(Gx1 + Gxs/2, Gy2[GnC] - Gys[GnC]/2, vg1*Gys[GnC]/2);
   else {
    moveto(Gx1, Gy2[GnC] - vg1*Gys[GnC]);
    linerel(Gxs, 0);
    setlinestyle(SOLID_LINE, 1, 1);
    setcolor(LIGHTGRAY);
    linerel(-3, 0);
    moverel(3 - Gxs, 0);
    linerel(3, 0);
   }
  }
  if (dg==1) {
   if (Polar) {
    for (a = 0.; a<2.*M_PI; a+=da) {
     moveto(Gx1 + Gxs/2, Gy2[GnC] - Gys[GnC]/2);
     lineto(Gx1 + Gxs/2 + cos(a)*Gys[GnC]/2, Gy2[GnC] - Gys[GnC]/2 + sin(a)*Gys[GnC]/2);
    }
   } else {
    moveto(Gx1 + vg1*Gxs, Gy1[GnC]);
    linerel(0, Gys[GnC]);
    setlinestyle(SOLID_LINE, 1, 1);
    setcolor(LIGHTGRAY);
    linerel(0, -3);
    moverel(0, 3 - Gys[GnC]);
    linerel(0, 3);
   }
  }
  if (dg>1) {
   setfillstyle(LTSLASH_FILL + dg - 2, LIGHTGRAY);
   bar(Gxs*vg1 + Gx1, Gy1[0], Gxs*vg2 + Gx1, Gy2[NbGn]);
  }
 }
}


//**************************************************************************
// Ligne: trace une ligne entre (x1,y1) et (x2,y2), coordonn‚es relatives
// entre 0 et 1, avec le style s, le marqueur k, la couleur c et suivant
// le mode m:
//	m = 0: normal;
//	m = 1: barres d'erreur.
//**************************************************************************

void Ligne(double x1, double y1, double x2, double y2, char s, char k, int c, char m)
{
 int ix1, ix2, ixd, iy1, iy2;

 if (ExpPS) {
  ix1 = x1*Gxs;
  ix2 = x2*Gxs;
  ixd = Min(abs(ix2 - ix1), 1);
  iy1 = y1*Gys[GnC] - Gy1[GnC] + My;
  iy2 = y2*Gys[GnC] - Gy1[GnC] + My;

  f_ps = fopen(ExpPSName,"at");
  fprintf(f_ps, "0 0 0 c\n");
  if (s==2) {
   fprintf(f_ps, "[2 5] 0 setdash\n");
  }
  if (s==1 || s==2) {
   fprintf(f_ps, "%g lw\n", EpLigne*0.2);
   fprintf(f_ps, "%d %d m\n %d %d l\nstroke\n", ix1, iy1, ix2, iy2);
  }
  fclose(f_ps);
 } else {
  ix1 = x1*Gxs;
  ix2 = x2*Gxs;
  ixd = Min(abs(ix2 - ix1), 1);
  iy1 = (1 - y1)*Gys[GnC] + Gy1[GnC] - Gy1[0];
  iy2 = (1 - y2)*Gys[GnC] + Gy1[GnC] - Gy1[0];

  if (c!=WHITE && (y1>1. || y1<0. || y2>1. || y2<0.)) return;
  setcolor(c);

  // ==== ligne
  if (s==1) {
   moveto(ix1, iy1);
   lineto(ix2, iy2);
  }

  // ==== pointill‚s
  if (s==2) {
   setlinestyle(DOTTED_LINE, 1, EpLigne);
   moveto(ix1, iy1);
   lineto(ix2, iy2);
   setlinestyle(SOLID_LINE, 1, EpLigne);
  }

  // ==== point
  if (k==1) {
   putpixel(ix1, iy1, c);
   if (m) putpixel(ix2, iy2, c);
   if (EpLigne==3) {
    moveto(ix1 - 1, iy1);
    linerel(2, 0); // Carr‚s 3x3 pixels...
   }
  }

  // ==== croix
  if (k==2) {
   moveto(ix2 + 2, iy2 + 2);
   linerel(-4, -4);  moverel(4, 0);  linerel(-4, 4);
   if (m) {
    moveto(ix1 + 2, iy1 + 2);
    linerel(-4, -4);  moverel(4, 0);  linerel(-4, 4);
   }
  }

  // ==== cercle
  if (k==3) {
   circle(ix2, iy2, 2);
   if (m) circle(ix1, iy1, 2);
  }

  // ==== carr‚
  if (k==4) {
   moveto(ix2 + 2, iy2 + 2);
   linerel(-4, 0);  linerel(0, -4);  linerel(4, 0);  linerel(0, 4);
   if (m) {
    moveto(ix1 + 2, iy1 + 2);
    linerel(-4, 0);  linerel(0, -4);  linerel(4, 0);  linerel(0, 4);
   }
  }

  // ==== barres
  if (k==5) Vide(ix2 - ixd, ix2 + ixd, Gys[GnC] + Gy1[GnC] - Gy1[0], iy2, c);
 }
}


//**************************************************************************
// Pl: ‚crit le texte ou la l‚gende (ps) suivant la valeur de (d):
//      -1 = titre g‚n‚ral (efface d'abord)
//	 0 = tick Y … la position (p)	}
//	 1 = label Y                    } du graphe (GnC)
//	 2 = tick X … la position (p)   }
//	 3 = label X                    }
//	 4 = temps min.
//	 5 = temps max.
//	 6 = type de calcul (DMode)
//**************************************************************************

void Pl(char *ps, double p, int d)
{
 int w, h, px, py;

 if (ExpPS) {
  f_ps = fopen(ExpPSName,"at");
  fprintf(f_ps, "appel Pl...\n");
  fclose(f_ps);
 } else {
  switch (d) {
   case -1: settextstyle(2, 0, 6); break;
   case 1: case 6: settextstyle(2, 1, 0); break;
   default: settextstyle(2, 0, 0);
  }
  if (d==1 || d==3 || d==-1 || d==6) setcolor(WHITE);
  else setcolor(LIGHTGRAY);
  w = textwidth(ps);
  h = textheight(ps);
  switch (d) {
   case -1:
    px = Gx1 + (Gxs - w)/2;  py = Yff;
    Vide(2, Mx-2, Yff+2, Yff+2+h, BLACK);
    break;
   case 0: px = Gx1 - w;  py = Gy2[GnC] - Gys[GnC]*p - h/2 - 2;  break;
   case 1: px = 6;  py = Gy2[GnC] - (Gys[GnC] + w)/2;  break;
   case 2: px = Gx1 + Gxs*p - w/2;  py = Gy2[0] + 3;  break;
   case 3: px = Gx1 + (Gxs - w)/2;  py = Gy2[0] + h*2;  break;
   case 4: px = Gx1;  py = Gy2[0] + h*2;  break;
   case 5: px = Gx2 - w + 2;  py = Gy2[0] + h*2;  break;
   case 6: px = Mx - 14;  py = Gy2[0] - (Gys[0] + w)/2;  break;
  }
  outtextxy(px, py, ps);
  if (d==-1) outtextxy(px + 1, py, ps);
 }
}


void Save_PS_File(void)
{
 ExpPS = 1;
 if ((f_ps = fopen(ExpPSName,"wt"))!=NULL) {
  fprintf(f_ps, "%%!PS-Adobe-3.0\n/m {moveto} def\n/l {lineto} def\n/s {closepath stroke} def\n/S {stroke} def\n");
  fprintf(f_ps, "/c {setrgbcolor} def\n/lw {setlinewidth} def\n");
  fprintf(f_ps, "595 %d div dup scale\n", Mx);
  fclose(f_ps);
  if (!EcranGraph) cprintf("\r\n      Making Postscript file... '%s'", ExpPSName);
  Aff_Graphique(3);
  if (EcranGraph) Aff_Menu(-1);
  f_ps = fopen(ExpPSName,"at");
  fprintf(f_ps, "showpage\n%%EOF\n");
  fclose(f_ps);
 }
 ExpPS = 0;
}
