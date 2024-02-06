/*************************************************************************/
/*       GGG                h                                            */
/* æ  æ G    r rr aaaa ppp  hhh                                          */
/* æ  æ G  G rr    a a p  p h  h                                         */
/* æ  æ G  G r    a  a p  p h  h                                         */
/* æææ   GGG r     aaa ppp  h  h                                         */
/* æ                   p                                                 */
/*                                                                       */
/* æGRAPH: Programme de visualisation et traitement de fichiers          */
/* de donn‚es ASCII. Visualisation temps r‚el des systŠmes æDAS et nDAS. */
/* Fichiers projets: MGR.C + MGR2.C + MGR.H                              */
/*          + SVGAUTIL.C (& .H) + SVGA256.H + GIFSAVE.C (& .H)           */
/* Date de cr‚ation: septembre 1995.                                     */
/* Date de mise … jour: novembre 2006.                                   */
/* Auteur: Fran‡ois Beauducel <beauducel@ipgp.jussieu.fr>                */
/*                                                                       */
/* Conventions sur les noms utilis‚s:                                    */
/* 	- tout majuscule = constantes compilateur (voir "mgr.h");        */
/* 	- 1Šre lettre majuscule = variables globales ou proc‚dures;      */
/*	- tout minuscule = variables locales.                            */
/*************************************************************************/

#include "MGRLIB\mgr.h"

struct tm *info_tm;
struct ffblk info_fl;
struct palettetype Pal;

struct SREGS sregs;
union REGS regs;
void interrupt RS_int();
void interrupt (*old_RS)();
//void far *Ptr[4];
void far *Screen;

FILE *f_in, *f_tmp0, *f_tmp1, *f_tmp2, *f_tmp, *f_cal, *f_tab, *f_ps;
char Titre0[13] = "9ØÍÞÏ×ÿ©îñæ",
     Titre1[30] = "!ÿÍº³º¾¬ºÿîÿ!ÿÙÝóÿÞª¸ÿíïïïÿ!",
     Titre2[15] = "!ÿÖÏØÏÿ!ÿÐÍÝÿ!",
     Titre3[38] = "!ÿÚò²¾¶³åÿ½º¾ª»ª¼º³ß¶¯¸¯ñµª¬¬¶ºªñ¹­ÿ!",
     Titre4[41] = "ÚÊÍÐÏÚÞÑÿÛÞËÞÿÞÜÎÊÖÌÖËÖÐÑÿÌÆÌËÚÒÿÏÍÐÕÚÜË",
     Mois[12][4] = { "Jan","Feb","Mar","Apr","May","Jun",
		     "Jul","Aug","Sep","Oct","Nov","Dec" },

     // ----- Styles de trac‚ d‚finis … partir de Graph_String[7]
     Graph_String[22] = "0*-vep#,n/:.oxs|rygcbm",
     Format_String[22] = "ymdhnsjfbtklqgix*.[]Y",
     Nombre_String[16] = "0123456789eE+-.",
     DStyle[4] = " Ä:",
     DMark[7] = " .xoþÞ",
     FD_Tmp[10] = "C:\\",
     FC_List[MAXPATH] = "__mgr_l.tmp",
     FC_Sort[MAXPATH] = "__mgr_s.tmp",
     FC_HCum[MAXPATH] = "__mgr_h.tmp",
     FC_Tmp0[MAXPATH] = "__mgr_0.tmp",
     FC_Tmp1[MAXPATH] = "__mgr_1.tmp",
     FC_Undo[MAXPATH] = "__mgr_u.tmp",
     FC_Tabl[MAXPATH] = "__mgr_t.tmp",
     *FF_Tmp0, *FF_Tmp1,
     OC = ' ', c1, Car, Flag_MCR,
     OS[2] = " ", ComBuf[TAILLE_BUF], Str[MAXSTR],
     LCommand[MAXSTR], MGR_dir[128], FTitre[80],
     FC_In[10][MAXPATH], FN_In[20], FF_In[MAXPATH],
     FC_Out[MAXPATH], FF_Out[MAXPATH],
     FC_Cal[MAXPATH], FC_Book[MAXPATH], ExpGIFName[MAXPATH], ExpPSName[MAXPATH],
     F_drv[MAXDRIVE], F_dir[MAXDIR], F_nam[MAXFILE], F_ext[MAXEXT],
     BegTime[16], EndTime[16], AllTime[64], BegZoom[16], EndZoom[16],
     ChDraw[4*MAXCNX], AllCh[MAXCNX+1], AllChN[2*MAXCNX], Formula[2*MAXCNX+1],
     Fmt[MAXFI][4*MAXCNX+1], Header[2],
     ImpFmt[MAXCOL+1], ExpFmt[MAXCOL+1], DataFmt[10],
     SUnit[MAXCNX][8], SName[MAXCNX][17], Label[MAXCNX][25],
     FInfo[MAXINFO][MAXSTR], FTick[10], IFml[MAXCNX][30], UndoName[20],
     Phase[4] = "deg", Doodson[20];
unsigned char FData = 1, Zn, Ze, NbInfo, DataOut[MAXCNX], NDAS,
	      NbCol;
signed char Bin, DAS, XMinus, OSView, PEcran, RLView, GdView, EpLigne = 1,
     TCenter, XText, Book, NTOver, NTTest, Virgule, Append, AutoFmt = 1,
     Calib, TimeMode, TTime = 1, Polar, Test, DfN, RNaN, Shot, XInfo,
     Canx, Cnx[MAXFI], AcPM, Fml, NbFml, RepCh, Sort, Debug,
     DCh, DRef = -1, EcranGraph, Gt[MAXCNX], Gm[MAXCNX],
     Gv = -1, NbGr[MAXCNX], Gr1, NbGn = 1, GnC, Gn[MAXCNX], Ge[MAXCNX],
     Gs[MAXCNX], NbGs, ChAff, SSOK, Souris, Info, Menu, DegFmt,
     NbCR = 1, FullScl, DMode, Finir, ExpFile, PassL, ExpGIF, ExpPS,
     FISO, Merge, HiCum, Undo, AutoScl, VGAT = -1, CalTable[MAXCNX];
unsigned nf, NbFiles, NbSF, ComTete, ComFin, Com, Port, NbHiCum = 360;
int Jg[MAXCNX], NbIF, NbColFmt, Zc, NbPage = 1,
    LBar1, LBar2, Xff, Yff, Wi1, Wi2,
    Mx = 640, My = 400, Gx1, Gx2, Gxs, Gy1[MAXCNX], Gy2[MAXCNX], Gys[MAXCNX],
    Xi1, Xi2, Yi1, Yi2,
    Divisor[MAXCNX], CMode, VGAMode = 2, Gc[MAXCNX],
    Site, UDAS,
    NumBaud, DataBit, StopBit, Parity;
long MDiff = 40000l, MaxVal = 100000l, Offset[MAXCNX], Frequence[MAXCNX],
     FStamp[MAXFI];
unsigned long TF_Tot, TCal, TCal2, TEstim, Decim = 1l, Filtre = 10l,
	      NbY, X, Y, Sn[MAXCNX], ExData,
	      NbSauts[MAXCNX], NbDO[MAXCNX], NbNul[MAXCNX], PtTable[2][MAXCNX];
double Rapx = 1., Rapy = 1.,
       NbSec[5] = { 31536000., 2592000., 86400., 3600., 60.}, Date1970 = 719529.,
       ChV[MAXCOL], STime, Zoom, NaN = 1e-38, Zp, Zs, Zt,
       DfxV[MAXCNX], Spike,
       DfmV[MAXCNX], DfnV[MAXCNX], Dfmax = MAXDOUBLE, Dfmin = -MAXDOUBLE,
       Gmin[MAXCNX], Gmax[MAXCNX], Gscl[MAXCNX], Gpas[MAXCNX], TTick[20],
       Data[2][MAXCNX], Cd[2][MAXCNX], Don[2][MAXCNX],
       Bruit[MAXCNX], Resol[MAXCNX], ResolMin = MAXDOUBLE,
       Sx[MAXCNX], Sy[MAXCNX], Sxy[MAXCNX], Sx2[MAXCNX], Sy2[MAXCNX],
       RLa[MAXCNX], RLb[MAXCNX], RLz[MAXCNX],
       huge (*RLm)[MAXCNX], huge (*RLc)[2],
       Vmax[MAXCNX], Vmin[MAXCNX],
       Dmax[MAXCNX], Dmin[MAXCNX],
       Emax[MAXCNX], Emin[MAXCNX],
       Amax[MAXCNX], Amin[MAXCNX], Ascl[MAXCNX], Apas[MAXCNX],
       Cal_F[MAXCNX], Cal_Cst[MAXCNX], Cal_x0[2][MAXCNX], Cal_x1[2][MAXCNX], Cal_x2[2][MAXCNX], Cal_x3[2][MAXCNX],
       Acqu[MAXFI], Acmin, Acmax, iT,
       TDmax[MAXFI], TDmin[MAXFI], Tmax, Tmin, Tscl,
       Zmax, Zmin, Zscl, Zpas, Dte[2],
       Period, DD[6], HarmS[MAXHARM][MAXCNX], HarmC[MAXHARM][MAXCNX],
       BTime, ETime = MAXLONG, ATime, CBTime, CETime = MAXLONG;
long double ttt;
time_t Horloge;
DacPalette256 RVB;

void main(void)
{
 char formula_String[16] = "cstlevwr+-./\\*0", fcmd[128],
      s0[MAXSTR], s1[80], c2, s2[128], ok = 1;
 int i, j, k, ni = 0, spx = 0, spy = 0, spxx, spyy, spt, start = 0;
 long stamp;
 double t, t2, xx1, xx2, yy1, yy2;

 putenv("TZ=GMT0");
 tzset();

 Init_Variables();

 strcpy(ImpFmt, "*");
 strcpy(FC_Out, "*");
 strcpy(ChDraw, ",*");
 strcpy(Header, "y");
 strcpy(DataFmt, "%1.8g");

 // ===== R‚pertoire de MGR.EXE
 strcpy(s0, searchpath(_argv[0]));
 fnsplit(s0, F_drv, F_dir, F_nam, F_ext);
 sprintf(MGR_dir, "%s%s", F_drv, F_dir);

 // ===== D‚codage de MGR.INI (si existe)
 sprintf(s0, "%smgr.ini",MGR_dir);
 if ((f_tmp = fopen(s0, "rt")) != NULL) {
  while (fscanf(f_tmp,"%s",&s0)!=EOF) Decode_Arguments(s0);
  fclose(f_tmp);
 }

 // ===== D‚codage de la ligne de commande DOS...
 if (_argc>1) for (i = 1; i<_argc; i++) {
  if (_argv[i][0]=='@') {
   strcpy(fcmd,&_argv[i][1]);
   if ((f_tmp = fopen(fcmd, "rt")) == NULL) {
    sprintf(s0, "Cannot open script '@%s@'", fcmd);
    Sortie(s0);
   }
   while (fscanf(f_tmp,"%s",&s0)!=EOF) {
    if (s0[0]!='%' && s0[0]!='@') Decode_Arguments(s0);
   }
   fclose(f_tmp);
  } else {
   Decode_Arguments(_argv[i]);
  }
 }

 // Installation des drivers ‚cran:
 //   "EGAVGA.OBJ" et "SVGA256.OBJ" … inclure au projet
 registerbgidriver(EGAVGA_driver);
 if (VGAT != 0) {
  installuserdriver("Svga256", DetectVGAMode);
  if (registerfarbgidriver(Svga256_fdriver) >= 0) VGAT = 1;
 }
 registerbgifont(triplex_font);
 registerbgifont(small_font);

 // ===== ComplŠte le format si inexistant ou incomplet
 Decode_Format(ImpFmt);

 // ===== D‚finit les noms complets de fichier temporaires
 strcpy(s0, FD_Tmp);  strcat(s0, FC_List);  strcpy(FC_List, s0);
 strcpy(s0, FD_Tmp);  strcat(s0, FC_Sort);  strcpy(FC_Sort, s0);
 strcpy(s0, FD_Tmp);  strcat(s0, FC_HCum);  strcpy(FC_HCum, s0);
 strcpy(s0, FD_Tmp);  strcat(s0, FC_Tabl);  strcpy(FC_Tabl, s0);

 // ===== D‚code les chaŒnes de titre et les complŠte (… partir de VER et VAN)
 Decode_Text(Titre0);
 Decode_Text(Titre1);
 sprintf(s0, "%1.2f%7.2f", VER, VAN);
 j = atoi(&s0[9]) - 1;
 for (i = 0; i<=2; i++) Titre0[8+i] = s0[i];
 Titre1[10] = s0[3];
 for (i = 0; i<=2; i++) Titre1[18+i] = Mois[j][i];
 for (i = 0; i<=3; i++) Titre1[22+i] = s0[4+i];
 //strcpy(tt1, "- Release 2 - FB Aug 2000 -");
 Decode_Text(Titre2);
 Decode_Text(Titre3);
 Decode_Text(Titre4);

 // ===== Affichage des infos sous DOS...
 textcolor(LIGHTCYAN);  cprintf("\r\n\r\n%s", Titre0);
 textcolor(LIGHTRED);  cprintf(" %s\r\n", Titre1);
 textcolor(LIGHTGRAY);
 strcpy(s0, "Reading file header(s)");
 if (!AutoFmt) {
  sprintf(s1, " with format '@%s@'", ImpFmt);
  strcat(s0, s1);
 }
 strcat(s0, "...\r");
 Pt2(s0);

 // ===== Si aucun fichier … importer: affichage de l'aide
 if (!NbIF && !Com) { Init_Ecran(); Aff_Aide(0); Sortie(""); }

 // ===== Pr‚paration de la calibration
 if (Calib) {
  if (strchr(FC_Cal,'.') == NULL) strcat(FC_Cal, ".clb");
  if ((f_cal = fopen(FC_Cal, "rt")) == NULL) {
   if ((f_cal = fopen(FC_Cal, "wt")) == NULL) {
    sprintf(s0, "Cannot open calib. '@%s@'", FC_Cal);
    Sortie(s0);
   }
   Calib *= 5; // il faudra cr‚er un nouveau fichier
  }
 }

 remove(FC_List); // au cas o— car le fichier sera ouvert en "append"...

 // ===== Lecture des en-tˆtes de fichiers … importer
 if (!Com) {
  for (ni = 0; ni<NbIF; ni++) {
   if (findfirst(FC_In[ni], &info_fl, 0) || !_fullpath(FF_In, FC_In[ni], MAXPATH)) {
    sprintf(s0, "Cannot find '@%s@'.", FC_In[ni]);
    Sortie(s0);
   }
   if ((f_tmp = fopen(FC_List, "at")) == NULL) Sortie("List OAT.");
   fputs("#\n", f_tmp);  fclose(f_tmp);
   sprintf(s0, "dir %s /a-d /o /b /l >> %s", FC_In[ni], FC_List);
   system(s0);
   fnsplit(FF_In, F_drv, F_dir, F_nam, F_ext);
   fnmerge(FC_In[ni], F_drv, F_dir, NULL, NULL);
   // FC_In[ni] contient le PATH...
  }
  ni = -1; nf = 0; NbSF = 0; k = -1;
  if ((f_tmp = fopen(FC_List, "rt")) == NULL) exit(1);
  do {
   if (fgets(Str, 64, f_tmp) == NULL) break;
   if (Str[0]=='#') {
    ni++;
   } else {
    sscanf(Str, "%s", &FN_In);
    sprintf(FF_In, "%s%s", FC_In[ni], FN_In);
    findfirst(FF_In, &info_fl, 0);
    TF_Tot += info_fl.ff_fsize;
    if (!stricmp(searchpath(FC_Out), FF_In))
     Sortie("Unvalid output file.");
    if (strlen(AllTime)) ATime = Decode_Time(AllTime);

    Lire_3Data();

    if (FData || nf==0) {
     Acmin = Acmax = Acqu[0];
    } else {
     Acmin = Min(Acmin, Acqu[nf]);
     Acmax = Max(Acmax, Acqu[nf]);
    }
    for (i = 0; i<Cnx[nf]; i++) {
     k = i;
     if (!Append && nf) k += Canx;
     if (!strcmp(SName[k], "")) strcpy(SName[k], FN_In);
    }
    nf++; NbSF = 0;
   }
  } while (nf<MAXFI && Canx<MAXCNX);
  fclose(f_tmp);
  NbFiles = nf;
  for (nf = 0; nf<NbFiles; nf++) {
   if (Append) Canx = Max(Canx, Cnx[nf]);
   else Canx += Cnx[nf];
  }
  if (NbFiles>1 && Append==0 && Merge==0) strcpy(Header,"y");

  // ===== Tests sur la validit‚ des fichiers (nombre de canaux)
  if (NbFiles==1) Append = 1;  // pour plus tard (NTTest)...
  if (Canx==0) Sortie("No data channel to import.");
  if (Append) for (nf = 0; nf<NbFiles; nf++) {
   if (Canx!=Cnx[nf] && Cnx[nf]!=0) {
    sprintf(s0, "Cannot append %d files with # channel nb.", NbFiles);
    Sortie(s0);
   }
  }
 }

 if (!DAS) MaxVal = 0l;
 if (!IsTime(ImpFmt)) {
  TimeMode = 2;
  if (!strchr(ImpFmt, 'i')) TimeMode = 1;
 }
 DegFmt = CalcDegFmt(ImpFmt);


 // ===== Ouverture des fichiers tampon
 strcpy(s0, FD_Tmp);  strcat(s0, FC_Tmp0);  strcpy(FC_Tmp0, s0);
 strcpy(s0, FD_Tmp);  strcat(s0, FC_Tmp1);  strcpy(FC_Tmp1, s0);
 FF_Tmp0 = FC_Tmp0;
 FF_Tmp1 = FC_Tmp1;
 if (((f_tmp0 = fopen(FF_Tmp0, "w+b")) == NULL)
     ||((f_tmp1 = fopen(FF_Tmp1, "w+b")) == NULL)) {
  sprintf(s0, "Cannot write on '@%s@'...", FD_Tmp);
  Sortie(s0);
 }
 if (IsFree(FD_Tmp[0] - 'A') == 2) {
  sprintf(s0, "Not enough space on '@%s@'...", FD_Tmp);
  Sortie(s0);
 }

 if (Test==1) Sortie("");

 // ===== Passe en mode graphique
 if (!ExpFile && !ExpPS && !Test) Init_Ecran();

 if (Com) AttendreCOM();

 if (TimeMode) {
  if (strlen(BegTime)) BTime = atof(BegTime);
  if (strlen(EndTime)) ETime = atof(EndTime);
 } else {
  if (strlen(BegTime)) BTime = Decode_Time(BegTime);
  if (strlen(EndTime)) ETime = Decode_Time(EndTime);
 }

 if (abs(Calib)>4) { New_Calib_File();  Init_Ecran(); }
 if (Calib) { nf = 0;  Lire_Calib(&Dte[0], 1); }

 Init_Texte();

 // ===== Importe les donn‚es (sauf si mode temps r‚el sur Com)
 if (!Com) {
  ni = -1; nf = 0; DCh = 0;

  if ((f_tmp = fopen(FC_List, "rt")) == NULL) Sortie("List ORT.");
  if (Calib && (f_tab = fopen(FC_Tabl, "rb")) == NULL) Sortie("Tabl ORB.");

  if (Append) if ((f_tmp2 = fopen(FC_Sort, "wb")) == NULL) Sortie("Sort OWB.");
  while (nf<NbFiles) {
   if (fgets(Str, 20, f_tmp) == NULL) break;
   if (Str[0]=='#') {
    ni++;
   } else {
    sscanf(Str, "%s", &FN_In);
    sprintf(FF_In, "%s%s", FC_In[ni], FN_In);
    if (strlen(AllTime)) ATime = Decode_Time(AllTime);
    NbSF = 0;

    Lire_Fichier_Origine();

    nf++;
   }
  }
  Working(1., 1.);

  // ===== Ecrit la valeur du pointeur dans le fichier Sort
  if (Append) {
   stamp = ftell(f_tmp0);
   fwrite(&stamp, sizeof(stamp), 1, f_tmp2);
   fclose(f_tmp2);
  }
  fclose(f_tmp);
  fclose(f_tmp0);  fclose(f_tmp1);
  if (Calib) fclose(f_tab);

  // ===== Op‚rations sur les fichiers (options)
  if (Sort && Append) Faire_Tri();
  if (Merge && !Append) Faire_Fusion();
 }
 if (iT!=0.) TimeMode = 0;
 if (NbY<2) {
  sprintf(s0, "Need more than @%d@ data to proceed.", NbY);
  Sortie(s0);
 }

 if (strlen(BegZoom) || strlen(EndZoom)) {
  if (strlen(BegZoom)) Zmin = Decode_Time(BegZoom);
  else Zmin = Tmin;
  if (strlen(EndZoom)) Zmax = Decode_Time(EndZoom);
  else Zmax = Tmax;
  if (Zmax<=Zmin) Zmax = Tmax;
  Zoom = (Tmax-Tmin)/(Zmax-Zmin);
 } else {
  Zoom = 1.;
 }
 Calc_Variables();
 Faire_Label();
 Init_Coul();

 nf = 0;
 if (!NTTest) NTOver = 1;
 if (DMode==4 || DMode==5) if (Init_Filter()) DMode = 0;

 // ===== Calcul des formules de la ligne de commande
 if (NbFml && Append) {
  for (i = 0; i<NbFml; i++) {
   strcpy(Formula, IFml[i]);
   //Init_Axes();
   Faire_Formule(Formula);
  }
 }
 if (HiCum && !Com) {
  Decode_Doodson(Doodson);
  Calc_HiCum();
 }
 if (TimeMode!=3 && DMode==6) DMode = 0;

 if (Spike>0.) Faire_Spike();

 if (ExpFile || ExpPS || Test) {
  if (ExpFile) { Faire_Nom_Export();  Export_Data(); }
  if (ExpPS) { Init_Axes();  Save_PS_File(); }
  if (Test) {
   strcpy(s0, "\nName                RMS_Diff  Resol  Mean  Linear  Valid");
   if (DAS>0) strcat(s0, "  OvScl  OvRan");
   printf(s0);
   for (i = 0; i<Canx; i++) {
    printf("\n%-17s  % 8lg  ", Label[i], sqrt(Bruit[i]/(double)NbY));
    if (Resol[i]!=MAXDOUBLE) printf("% 8lg  ", Resol[i]);
    else printf("?      ");
    printf("%+8lg  %+8lg  ", RLz[i], RLa[i]);
    if (Acmin==0.) j = 100;
    else j = (100*Acmin*Decim*(NbY - NbNul[i] - 1))/Tscl;
    printf("%3d%%", j);
    if (DAS>0) printf("  %4d  %4d", NbSauts[i], NbDO[i]);
   }
  }
  Sortie("");
 }
 if (DMode && DMode!=5) {
  Init_Axes();
  Aff_Graphique(1);
  FF5();
 }
 Init_Texte();

 // ===== boucle principale du programme
 do {
  Init_Axes();			// calcul des variables
  Aff_Legendes();          	// trace les l‚gendes
  Aff_Graphique(0);             // trace les courbes
  Aff_Menu(-1);

  // --- boucle d'attente du programme = interprŠte les mouvements et
  // fonctions de la souris et les fonctions au clavier. Met … jour des
  // variables et dans la plupart des cas, sort de la boucle par un break et
  // dans ce cas retrace le graphique complet.

  do {
  if (Souris) {
   Aff_Souris(3);
   Aff_Horloge();
   if (ok || regs.x.cx!=spx || regs.x.dx!=spy) {
    // Pour certains modes graphiques, dessin de la souris
    if (VGAT==1 && VGAMode>=0) {
     if (start) Draw_Souris(spx, spy);
     Draw_Souris(regs.x.cx, regs.x.dx);
     start = 1;
    }
    spx = regs.x.cx;  spy = regs.x.dx;
    if (!Info || ok) Aff_Pos(spx, spy);
    ok = 0;
   }
   //************************************* Souris: test bouton gauche
   regs.x.bx = 0; Aff_Souris(5);
   if (regs.x.bx) {
    spx = regs.x.cx;  spy = regs.x.dx;
    if (Info && IsDedans(spx, spy, Xi1, Xi2, Yi2 - 10, Yi2)) {
     // click sur ENTER dans fenˆtre Info:
     Faire_ChDraw();
     break;
    }
    delay(DBLCLIC); regs.x.bx = 0; Aff_Souris(5);
    if (Info && IsDedans(spx, spy, Xi1, Xi2, Yi1, Yi2)) {
     // click dans fenˆtre Info:
     j = floor((spy - Yi1)/12. - 2.7);
     if (j>=0 && j<Canx) {
      if (regs.x.bx) {
       for (i = 0; i<Canx; i++) Gn[i] = 0;
       Gn[j] = 1;
      } else {
       if (Gn[j]==0) Gn[j] = 1;
       else Gt[j] = (Gt[j] + 1)%NBGT;
      }
     }
     Refresh_Info();
    } else {
     if (!Info && IsDedans(spx, spy, Gx1, LBar1, Gy1[0] - 9, Gy1[0] - 4)) {
      // click … gauche de l'ascenseur temps:
      if (regs.x.bx) {
       Zmin = Tmin;
       Zmax = Zmin + Zscl;
      } else {
       Zmin = Max(Tmin, Zmin - Zscl/2);
       Zmax = Min(Tmax, Zmin + Zscl);
      }
      break;
     } else if (!Info && IsDedans(spx, spy, LBar2, Gx2, Gy1[0] - 9, Gy1[0] - 4)) {
      // click … droite de l'ascenseur temps:
      if (regs.x.bx)  {
       Zmax = Tmax;
       Zmin = Zmax - Zscl;
      } else {
       Zmax = Min(Tmax, Zmax + Zscl/2);
       Zmin = Max(Tmin, Zmax - Zscl);
      }
      break;
     } else if (!Info && IsGraph(spx, spy) && Zscl>1) {
      // click dans le graphique:
      if (regs.x.bx) FF5();
      else {
       spxx = spx;  spyy = spy;
       Draw_Souris(spx, spy);
       setlinestyle(SOLID_LINE, 1, 1);
       do {
	if (Gv != -1) break;
	regs.x.cx = Gx1;  regs.x.dx = Gx2;  Aff_Souris(7);
	regs.x.cx = Gy1[0];  regs.x.dx = Gy2[0];
	Aff_Souris(8);  Aff_Souris(3);
	if ((regs.x.cx != spxx)||(regs.x.dx != spyy)) {
	Aff_Pos(regs.x.cx, regs.x.dx);
	setcolor(WHITE);  setwritemode(1);
	line(spxx, Gy1[0], spxx, Gy2[0]);
	line(regs.x.cx, Gy1[0], regs.x.cx, Gy2[0]);
	setwritemode(0);
	line(spx, Gy1[0], spx, Gy2[0]);
	/*for (i = Gy1[0]; i <= Gy2[0]; i++) {
	 putpixel(spxx, i, ~getpixel(spxx, i));
	 putpixel(regs.x.cx, i, ~getpixel(regs.x.cx, i));
	 putpixel(spx, i, WHITE);
	} */
	spxx = regs.x.cx;  spyy = regs.x.dx;
	Aff_Souris(1);
	}
       } while (regs.x.bx);
       setlinestyle(SOLID_LINE, 1, EpLigne);
       if (spx>spxx) { spt = spxx; spxx = spx; spx = spt; }
       if (spy>spyy) { spt = spyy; spyy = spy; spy = spt; }
       if (spxx - spx>1) {
	Zmin += Zscl*((spx - Gx1)/(double)Gxs);
	Zmax -= Zscl*((Gx2 - spxx)/(double)Gxs);
	Zoom = Tscl/fabs(Zmax - Zmin);
	break;
       }
       t2 = Zscl*((spx - Gx1)/(double)Gxs) + Zmin;
       t = (Tscl/(Zoom *= 2.))/2.;
       Zmax = t2 + t;  Zmin = t2 - t;  CalcZ();
       // repositionnement de la souris au point de cliquage...
       regs.x.cx = Gxs*((t2 - Zmin)/Zscl) + Gx1;
       regs.x.dx = spy;  Aff_Souris(4);
      }
      break;
     } //else Aff_Info(); if (!Info && !SSOK) break;
    }
   }
   //************************************* Souris: test bouton droite
   regs.x.bx = 1; Aff_Souris(5);
   if (regs.x.bx) {
    spx = regs.x.cx;  spy = regs.x.dx;
    delay(DBLCLIC); regs.x.bx = 1; Aff_Souris(5);
    if (Info && IsDedans(spx, spy, Xi1, Xi2, Yi1, Yi2)) {
     // click dans fenˆtre Info:
     j = floor((spy - Yi1)/12. - 2.7);
     if (regs.x.bx) {
      for (i = 0, ok = 0; i<Canx; i++) if (Gn[i]==0) ok = 1;
      for (i = 0; i<Canx; i++) if (ok) Gn[i] = 1;
      else Gn[i] = 0;
     } else {
      if (j>=0 && j<Canx) Gn[j]++;
     }
     Refresh_Info();
    } else {
     if (IsGraph(spx, spy)) {
      // click dans le graphique:
      if (regs.x.bx && Zoom>1.) {
       t = Tscl;
       Zoom = 1.;
       MAJ_AD();
      } else {
       spxx = spx;  spyy = spy;
       do {
	regs.x.cx = Gx1; regs.x.dx = Gx2; Aff_Souris(7);
	for (GnC = 1; GnC<=NbGn; GnC++) {
	if (IsDedans(spx, spy, Gx1, Gx2, Gy1[GnC], Gy2[GnC])) break;
       }
       regs.x.cx = Gy1[GnC];  regs.x.dx = Gy2[GnC];
       Aff_Souris(8);  Aff_Souris(3);
       if (regs.x.cx!=spxx || regs.x.dx!=spyy) {
	Aff_Pos(regs.x.cx, regs.x.dx);
	Aff_Souris(2);  setcolor(WHITE);  setwritemode(1);
	setlinestyle(SOLID_LINE, 1, 1);
	rectangle(spx, spy, spxx, spyy);
	rectangle(spx, spy, regs.x.cx, regs.x.dx);
	setwritemode(0);
	setlinestyle(SOLID_LINE, 1, EpLigne);
       }
       spxx = regs.x.cx;  spyy = regs.x.dx;  Aff_Souris(1);
      } while (regs.x.bx);
      regs.x.cx = 0; regs.x.dx = Mx; Aff_Souris(7);
      if (spx>spxx) { spt = spxx; spxx = spx; spx = spt; }
      if (spy>spyy) { spt = spyy; spyy = spy; spy = spt; }
      if (spxx-spx>1) {
       xx1 = (spx - Gx1)/(double)Gxs;
       xx2 = (spxx - Gx2)/(double)Gxs;
       switch (Gv) {
       case -1:
	Zmin += Zscl*xx1;
	Zmax += Zscl*xx2;
	Zoom = Tscl/fabs(Zmax - Zmin);
	break;
       default:
	Amin[Gv] = Ascl[Gv]*xx1 + Amin[Gv];
	Amax[Gv] = Ascl[Gv]*xx2 + Amax[Gv];
	Ascl[Gv] = Amax[Gv] - Amin[Gv];
	if (Ascl[Gv]==0.) Ascl[Gv] = 1.;
	Apas[Gv] = Grid(Ascl[Gv], 1);
       }
       if (spyy-spy>1) for (i = 0; i<Canx; i++) {
	if (Gn[i]==GnC) {
	 Amin[i] = Gmin[GnC-1] - Gscl[GnC-1]*((spyy - Gy2[GnC])/(double)Gys[GnC]);
	 Amax[i] = Gmax[GnC-1] - Gscl[GnC-1]*((spy - Gy1[GnC])/(double)Gys[GnC]);
	 Ascl[i] = Amax[i] - Amin[i];
	 if (Ascl[i]==0.) Ascl[i] = 1.;
	 Apas[i] = Grid(Ascl[i], 0);
	}
       }
       break;
      }
      Zoom = Max(Zoom/2., 1.); t = (Tscl/Zoom)/2.;
     }
     t2 = Zscl*((spx - Gx1)/(double)Gxs) + Zmin;
     Zmax = t2 + t;  Zmin = t2 - t;  CalcZ();
     // repositionnement de la souris au point de cliquage...
     switch (Gv) {
     case -1:
      regs.x.cx = Gxs*((t2 - Zmin)/Zscl) + Gx1;
      break;
     default:
      regs.x.cx = spx;
     }
     regs.x.dx = spy;  Aff_Souris(4);
    }
    break;
   }
  }
 }
 regs.x.cx = 0;  regs.x.dx = Mx;  Aff_Souris(7);
 if (Info) {
  regs.x.cx = 0;
  regs.x.dx = My;
 } else {
  regs.x.cx = Yff;
  regs.x.dx = My - 12;
 }
 Aff_Souris(8);
 if (Com && ComTete!=ComFin) {
  while (ComTete!=ComFin) {
   Lire_Chaine();
   if ((strstr(Str, "*")) != NULL || NDAS) {
    f_tmp0 = fopen(FF_Tmp0, "a+b");
    X = 0l;
    if (Extract_Data(0)!=1) { MAJ_Data(1); NbY++; }
    fclose(f_tmp0);
   }
  }
  Init_Texte();
  if (Zoom==1.) Tscl = Tmax - Tmin;
  MAJ_DV();  FF5();
  break;
 }
 if (kbhit()) {
  c1 = getch();
  ok = 0;

  // ===== CaractŠres ‚tendus
  if (c1==0) c2 = getch();
  else c2 = 0;

  // ===== AccŠs aux sous menus
  if (c2==AltF) Aff_Menu(1);
  if (c2==AltA || c2==F3) Aff_Menu(2);
  if (c2==AltG) Aff_Menu(3);
  if (c2==AltV) Aff_Menu(4);
  if (c2==AltN) Aff_Menu(5);

  if (c2==AltI || c2==F2) {
   if (Info==0) {
    start = 0;
    Aff_Menu(6);
    Aff_Info(1);
   } else {
    if (TimeMode==3) {
     Info = Info%2 +1;
     Refresh_Info();
    }
   }
  }
  if (c2==AltH) {
   Aff_Souris(2);
   SSOK = save_screen(Screen);
   Aff_Aide(MENUPAGE);
   if (SSOK) restore_screen(Screen);
   else break;
   Aff_Souris(1);
  }
  if (c2==F1) FF1();
  if (c2 == MajF1) {
   strcpy(s0, "");
   Entrer_Chaine("Enter a filename for GIF export: ", NULL, s0, 5);
   if (strlen(s0)) {
    strcpy(ExpGIFName, s0);
    Aff_Menu(-1);
    Aff_Horloge();
    Aff_Souris(2);
    Save_GIF_File();
    Aff_Souris(1);
    sprintf(s0, "Screen has been saved in GIF file '@%s@'.", ExpGIFName);
    Aff_Message(s0, 0, 1);
   } else {
    Aff_Menu(-1);
   }
  }
  if (c2 == AltP) {
   strcpy(s0, "");
   Entrer_Chaine("Enter a filename for PostScript export: ", NULL, s0, 5);
   if (strlen(s0)) {
    strcpy(ExpPSName, s0);
    Save_PS_File();
   } else {
    Aff_Menu(-1);
   }
  }
  if (c2==AltF1 && NbFiles>1 && !Append) {
   strcpy(s0, "y");
   Entrer_Chaine("Merge the files (Y/N): ", "yn", s0, 2);
   if (!strcmp(strlwr(s0), "y")) {
    Faire_Fusion();
    Init_Texte();
    break;
   }
   Aff_Menu(-1);
  }
  if (c2==CtrlF1 && !Sort && Append) {
   strcpy(s0, "y");
   Entrer_Chaine("Sort the data (Y/N): ", "yn", s0, 2);
   if (!strcmp(strlwr(s0), "y")) {
    Faire_Tri();
    break;
   }
   Aff_Menu(-1);
  }
  if (c2==MajF2) {
   strcpy(s2, "Enter Title: ");
   if (strlen(FTitre)) ok = 1;
   else ok = 0;
   strcpy(s0, FTitre);
   if (Entrer_Chaine(s2, "", s0, 0)) {
    strcpy(FTitre, s0);
    if (ok && strlen(FTitre)) {
     Pl(FTitre, 0., -1);
    } else {
     Init_Texte();
     break;
    }
   }
   Aff_Menu(-1);
  }
  if (c2==AltF2) {
   for (i = 0; i<Canx; i++) {
    sprintf(s2, "Enter Channel %c Name: ", CarCan(i));
    strcpy(s0, SName[i]);
    if (Entrer_Chaine(s2, "", s0, 0)) strcpy(SName[i], s0);
    else break;
    sprintf(s2, "Enter Channel %c Unit: ", CarCan(i));
    strcpy(s0, SUnit[i]);
    if (Entrer_Chaine(s2, "", s0, 0)) strcpy(SUnit[i], s0);
    else break;
   }
   Faire_Label();
   Init_Texte();
   break;
  }
  if (c2==CtrlF2) {
   Init_Coul();
   Annule_Styles(ChDraw);
   break;
  }
  if (c2==F3 && DMode) {
   strcpy(s0, "y");
   sprintf(s2, "Undo %s (Y/N): ", UndoName);
   Entrer_Chaine(s2, "yn", s0, 2);
   if (!strcmp(strlwr(s0), "y")) {
    if (DMode==4 || DMode==5) Eff_Filter();
    DMode = 0;
    MAJ_DV();
    FF5();
    break;
   } else {
    Aff_Menu(-1);
   }
  }

  // ===== Data Fitting
  if (c2==AltO || c2==AltL || c2==AltD || c2==AltC || c2==AltT
      || (c2==AltW && TimeMode==3) || (c1==CR && RLView)) {
   ok = 1;
   // ----- recalcule les paramŠtres si la fenˆtre a chang‚...
   if (DMode) {
    for (i = 0; i<Canx; i++) Calc_RL(i);
    if (DMode==4 || DMode==5) Eff_Filter();
   }
   if (c2==AltO || (RLView==1 && c1==CR)) {
    if (DMode!=1) {
     for (i = 0; i<Canx; i++) { Emin[i] -= RLz[i];  Emax[i] -= RLz[i]; }
     MAJ_DE();  FF5();  DMode = 1;
     strcpy(UndoName, "Offset Correction");
     break;
    }
   }
   if (c2==AltL || (RLView==2 && c1==CR)) {
    DMode = 2;
    strcpy(UndoName, "Linear Correction");
   }
   if (c2==AltC) { DMode = 3;  strcpy(UndoName, "Cumulative Sum"); }
   if (c2==AltD) {
    DMode = 4;
    strcpy(UndoName, "Derivate");
    strcpy(s2, "Approximate Derivative = ");
    sprintf(s0, "2");
   }
   if (c2==AltT) {
    DMode = 5;
    strcpy(UndoName, "Filtering");
    strcpy(s2, "Filtering (moving average) = ");
    sprintf(s0, "%ld", Filtre);
   }
   if (c2==AltW || (RLView>2 && c1==CR)) {
    DMode = 6;
    strcpy(UndoName, "Harmonic Correction");
    if (c1==CR) {
     Filtre = pow(2., RLView-3);
    } else {
     sprintf(s2, "Enter harmonic binary combination (@1@ to @%g@) = ", pow(MAXHARM, 2.) - 1.);
     sprintf(s0, "1");
    }
   }
   if (DMode==4 || DMode==5 || (DMode==6 && c1!=CR)) {
    if (Entrer_Chaine(s2, "0123456789", s0, 1)) {
     Filtre = atol(s0);
     if (DMode==4 || DMode==5) {
      if (Init_Filter()) {
       ok = 0;
       DMode = 0;
       Aff_Message("Not enough memory!...", 0, 1);
      }
     } else {
      if (Filtre<1) Filtre = 1l;
      if (Filtre>=pow(MAXHARM, 2.)) Filtre = pow(MAXHARM, 2.) - 1l;
     }
    } else {
     ok = 0;
     DMode = 0;
     Aff_Menu(-1);
    }
   }
   if (DMode==5) break; // gagner du temps: les donn‚es filtr‚es sont
			// forc‚ment dans l'interval des donn‚es brutes!
   if (DMode) Aff_Menu(2);
   if (ok) {
    Aff_Graphique(1);
    MAJ_DE();  FF5();
    break;
   }
  }

  // ===== Mode ‚chelle normalis‚e
  if (c2==F4) {
   FullScl = (FullScl + 1)%2;
   FF5();
   break;
  }
  // ===== Ajustement des ‚chelles (magnify)
  if (c2==F5) { FF5();  break; }
  // ===== Egalisation des ‚chelles
  if (c2==MajF5) {
   FullScl = 0;
   yy1 = Gscl[0];
   for (i=1; i<NbGn; i++) if (yy1<=Gscl[i]) {  yy1 = Gscl[i];
					       yy2 = Gpas[i];  }
   for (i=0; i<Canx; i++) if (Gn[i]) {
    Amin[i] = yy2*floor(Gmin[Gn[i]-1]/yy2);
    Amax[i] = Amin[i] + yy1;
    Ascl[i] = yy1;
    Apas[i] = Grid(Ascl[i], 0);
   }
   break;
  }
  if (c2==F6 || c2==CtrlF6) {
   if (Info) {
    if (c2==F6) FF6(1, 0);
    else FF6(0, 1);
    Refresh_Info();
   } else {
    do {
     if (c2==F6) j = FF6(1, 0);
     else j = FF6(0, 1);
     strcpy(s0, "Current Line & Marker Type (@ENTER@ when done): @F6@ ");
     switch (Gt[j]) {
     case 0: strcat(s0, "none       "); break;
     case 1: strcat(s0, "Solid Line "); break;
     case 2: strcat(s0, "Dotted Line"); break;
     }
     strcat(s0, "  &  @CtrlF6@ ");
     switch (Gm[j]) {
     case 0: strcat(s0, "none"); break;
     case 1: strcat(s0, "Dots"); break;
     case 2: strcat(s0, "Cross"); break;
     case 3: strcat(s0, "Circles"); break;
     case 4: strcat(s0, "Squares"); break;
     case 5: strcat(s0, "Bars"); break;
     }
     Efface_Menu(0);
     Pg2(s0, -13, -14, 2);
     while (!kbhit());
     c1 = getch(); if (!c1) c2 = getch();
    } while (c1!=CR && c1!=ESC);
    if (c1==CR) break;
    else { FF6(-1, -1);  Aff_Menu(-1); }
   }
  }

   // ===== Grille ON/OFF
   if (c2==MajF6) { GdView = (GdView + 1)%2;  break; }

   // ===== Epaisseur des traits
   if (c2==AltF6) {
    if (EpLigne==1) EpLigne = 3;
    else EpLigne = 1;
    Init_Boite();
    Init_Texte();
    break;
   }

   // ===== Visualisation de la moyenne, r‚gression lin‚aire et harmoniques
   if (c2==F7) {
    Aff_Souris(2);
    if (!DMode) {
     // ----- Retrace la r‚gression actuelle en mode XOR (efface)
     if (RLView) Trace_RL(RLView);
     RLView = (RLView + 1)%(3 + (TimeMode==3)*MAXHARM);
     // ----- Trace la nouvelle r‚gression
     if (RLView) {
      Trace_RL(RLView);
      Aff_Menu(8);
     } else {
      Vide(Mx - 12, Mx - 2, Gy1[0], Gy2[0], BLACK);
      Aff_Menu(0);
     }
    } else {
     // ----- Calcul de la p‚riode
     if ((DMode==1 || DMode==2 || DMode==4) && NbGn==1 && NbGr[0]==1 && Zc>2) {
      Vide(Mx - 12, Mx - 2, Gy1[0], Gy2[0], BLACK);
      sprintf(s0, "Period = %lg ñ %1.4f sec (%d counted)",
		 Zp, sqrt(Zs/floor((Zc - 1)/2.)), (Zc - 1)/2);
      Pl(s0, 0., 6);
     }
    }
    Aff_Souris(1);
   }

   // ===== Visualisation des limites d'‚chelle (mode DAS)
   if (c2==MajF7 && DAS) {
    OSView = (OSView + 1)%2;
    break;
   }

   // ===== Calcul de formules
   if ((c2==F8 || c2==F12) && Append) {
    if (Fml) {
     sprintf(s2, "Replace channel %c (Y/N): ", CarCan(Canx-1));
     strcpy(s0, "y");
     Entrer_Chaine(s2, "yn", s0, 2);
     if (s0[0] == 'y') RepCh = 1;
     else RepCh = 0;
    }
    sprintf(s2, "Enter formula for channel %c: ", CarCan(Canx - RepCh));
    strcpy(s0, "");
    Entrer_Chaine(s2, formula_String, s0, 3);
    if (strlen(s0)) {
     strcpy(Formula, s0);
     Faire_Formule(Formula);
     break;
    } else {
     Aff_Menu(-1);
    }
   }

   // ===== Correction des piques
   if (c2==AltS) {
    if (Spike>0.) sprintf(s0, "%lg", Spike);
    else strcpy(s0, "10");
    if (Entrer_Chaine("Remove spikes (STD): ", Nombre_String, s0, 1)) {
     Spike = atof(s0);
     if (Spike<=0.) Spike = 10;
     Faire_Spike();
     break;
    }
    Aff_Menu(2);
   }

   // ===== Calcul de HiCum
   if ((c2==MajF8 || c2==AltF8) && !TimeMode) {
    strcpy(s2, "Enter the wave name, Doodson argument or integer period = ");
    //strcpy(s0, Doodson);
    strcpy(s0, "");
    if (Entrer_Chaine(s2, "", s0, 0)) {
     if (strlen(s0)) {
      strcpy(Doodson, s0);
      Decode_Doodson(Doodson);
      //TimeMode = 3;
      if (c2==MajF8) {
       strcpy(s2, "Hicum: Enter the number of samples: ");
       sprintf(s0, "%d", NbHiCum);
       if (Entrer_Chaine(s2, "0123456789", s0, 1)) {
	NbHiCum = atoi(s0);
	if (NbHiCum<MINHICUM) NbHiCum = 360;
       }
       HiCum = 1;
       Calc_HiCum();
       Init_Texte();
       break;
      }
      Init_Texte();
     }
    }
    Aff_Menu(-1);
   }

   if (c2==MajF9 && VGAT) {
    strcpy(s2, "Enter VGA Video Mode (@0@ to @4@): ");
    strcpy(s0, "2");
    Entrer_Chaine(s2, "01234", s0, 2);
    if (strlen(s0)) {
     VGAMode = atoi(s0);
     closegraph();  Init_Ecran();  Init_Texte();  break;
    } else {
     Aff_Menu(-1);
    }
   }
   if (c2==CtrlF9) {
    PEcran = (PEcran + 1)%2;
    clearviewport();
    Init_Boite();
    Init_Texte();
    break;
   }
   if (c2==F9) {
    CMode = (CMode + 1)%4;
    Couleur_Ecran(CMode);
   }
   if (c2==F10) {
    Efface_Menu(0);
    strcpy(s0, "Do you really want to quit (@ENTER@ to confirm):");
    Pg2(s0, -13, -14, 2);
    while (!kbhit());
    if (getch()==CR) { Finir = 1;  break; }
    else Aff_Menu(-1);
   }
   if (c2==AltX) { Finir = 1;  break; }
   if (c2==PGDN) {
    ChAff = (ChAff + 1)%Canx;
    Aff_Noms(ChAff);
   }
   if (c2==PGUP) {
    ChAff = (Canx + ChAff - 1)%Canx;
    Aff_Noms(ChAff);
   }
   if (c2==ARDN) if (FUD(0)) break;
   if (c2==ARUP) if (FUD(1)) break;
   if (c2==LEFT && Zmin>Tmin) {
    Zmin = Max(Tmin, Zmin - Zscl/2.);
    Zmax = Min(Tmax, Zmin + Zscl);
    break;
   }
   if (c2==RIGHT && Zmax<Tmax) {
    Zmax = Min(Tmax, Zmax + Zscl/2.);
    Zmin = Max(Tmin, Zmax - Zscl);
    break;
   }
   if (c2==HOME && Zmin>Tmin) {
    Zmin = Tmin;
    Zmax = Zmin + Zscl;
    break;
   }
   if (c2==END && Zmax<Tmax) {
    Zmax = Tmax;
    Zmin = Zmax - Zscl;
    break;
   }
   if (c2==AltZ) {
    Efface_Menu(0);
    if (TimeMode==0) {
     strcpy(s2, "Enter zoom start date (YYMMDDhhnnss): ");
     Faire_Time(Zmin, s0);
     if (Entrer_Chaine(s2, "0123456789", s0, 1)) {
      Zmin = Decode_Time(s0);
     }
    } else {
     strcpy(s2, "Enter zoom start index: ");
     sprintf(s0, "%lg", Zmin);
     if (Entrer_Chaine(s2, "0123456789", s0, 1)) {
      Zmin = atof(s0);
     }
    }
    Efface_Menu(0);
    if (TimeMode==0) {
     strcpy(s2, "Enter zoom end date (YYMMDDhhnnss): ");
     Faire_Time(Zmax, s0);
     if (Entrer_Chaine(s2, "0123456789", s0, 1)) {
      Zmax = Decode_Time(s0);
     }
    } else {
     strcpy(s2, "Enter zoom end index: ");
     sprintf(s0, "%lg", Zmax);
     if (Entrer_Chaine(s2, "0123456789", s0, 1)) {
      Zmax = atof(s0);
     }
    }
    if (Zmax<=Zmin) Zmax = Tmax;
    Zoom = (Tmax-Tmin)/(Zmax-Zmin);
    CalcZ();
    break;
   }

  // ===== ESC sur fenˆtre Info
  if (Info && c1==ESC) {
   Aff_Info(0);
   if (!SSOK) break;
  }
  // ===== ENTER sur fenˆtre Info
  if (Info && c1==CR) {
   Faire_ChDraw();
   break;
  }
  if (c1=='+' && Zscl>1) {
   Zoom *= 2;
   t = (Zmax - Zmin - Tscl/Zoom)/2.;
   Zmax -= t;  Zmin += t;
   break;
  }
  if (c1=='-' && Zoom>1.) {
   Zoom = Max(Zoom/2., 1.);
   t = (Zmax - Zmin - Tscl/Zoom)/2.;
   Zmax -= t;  Zmin += t;
   break;
  }
  if (c1==ESC) {
   if (Menu) Aff_Menu(0);
   else {
    if (Zoom>1.) {
     Zoom = 1.; MAJ_AD();
     break;
    }
   }
  }
  if (c1=='*' && Info) {
   for (i = 0, ok = 0; i<Canx; i++) {
    if (Gn[i]==0) ok = 1;
   }
   for (i = 0; i<Canx; i++) {
    if (ok) Gn[i] = 1;
    else Gn[i] = 0;
   }
   Refresh_Info();
  }

   // ===== Si canal ou caractŠre de Graph_String: ‚dition de ChDraw
   j = IsCanal(c1) - 1;
   if ((j>=0 && j<Canx) || (c1 && strchr(Graph_String, c1))) {
    if (Info) {
     if (c1==',') {
      for (i = 0; i<Canx; i++) Gn[i] = i + 1;
     } else {
      if (Gn[j]==0) Gn[j] = 1;
      else Gt[j] = (Gt[j] + 1)%NBGT;
     }
     Refresh_Info();
    } else {
     strcpy(s0, "");
     if (Entrer_Chaine("Channels to draw: ", Graph_String, s0, 1)) {
      strcpy(ChDraw, s0);  break;
     } else {
      Aff_Menu(-1);
     }
    }
   }
  }
  if (ExpGIF) {
   Aff_Pos(Mx, My);
   Aff_Souris(2);
   Save_GIF_File();
   Finir = 1;
   break;
  }
  } while (1); // on sort de cette boucle sans fin par un "break"...

  if (AutoScl) FF5();
 }
 while (Finir==0);
 Sortie("");
}


//**************************************************************************
// Decode_Arguments: d‚code la chaŒne *ss issue de la ligne de commande ou
// d'un fichier de commande, et initialise les options du programme (toutes
// en variables globales...)
//**************************************************************************

void Decode_Arguments(char *ss)
{
 char *p;
 int j, k;

 strcat(LCommand, ss); strcat(LCommand, " ");

 // ===== argument sans "/" ou "-" comme 1er caractŠre = nom de fichier de donn‚es
 if (strstr(ss,"/")==NULL && ss[0]!='-') {
  strcpy(FC_In[NbIF], ss);
  NbIF++;
 }

 // ===== argument avec "/" = remplac‚ par "-" pour ‚volution et compatibilit‚
 if (ss[0]=='/') ss[0] = '-';

 // ===== liste des options reconnues
 if (!strcmp(ss, "-a")) Append = 1;
 if (!strcmp(ss, "-as")) AutoScl = 1;
 if (strstr(ss, "-b:")==ss) strcpy(BegTime,&ss[3]);
 if (strstr(ss, "-bk:")==ss) { strcpy(FC_Book, &ss[4]);  Book = 1; }
 if (strstr(ss, "-c:")==ss) { strcpy(FC_Cal, &ss[3]);  Calib = 3; }
 if (strstr(ss, "-cf:")==ss) { strcpy(FC_Cal, &ss[4]);  Calib = 1; }
 if (strstr(ss, "-cg:")==ss) { strcpy(FC_Cal, &ss[4]);  Calib = 4; }
 if (strstr(ss, "-cn:")==ss) { strcpy(FC_Cal, &ss[4]);  Calib = -1; }
 if (strstr(ss, "-cv:")==ss) { strcpy(FC_Cal, &ss[4]);  Calib = 2; }
 if (strstr(ss, "-com")==ss) {
  Com = atoi(&ss[4]);
  if (Com!=2) Com = 1;
  Port = Com == 1 ? 0x3f8:0x2f8;
  NumBaud = 6; DataBit = 2; StopBit = 0; Parity = 0; AutoFmt = 2;
  if (ss[5]==':') {
   NDAS = atoi(&ss[6]);
   strcpy(ImpFmt, "ymdhns");
   DataBit = 3; AutoFmt = 4;
  } else {
   strcpy(ImpFmt, "[yymmddhhnnss]");
  }
 }
 if (strstr(ss, "-d:")==ss) Decim = atol(&ss[3]);
 if (!strcmp(ss, "-das")) {
  DAS = 1;
  AutoFmt = 3;
  strcpy(ImpFmt, "hn");
 }
 if (!strcmp(ss, "-dc")) DMode = 3;
 if (!strcmp(ss, "-debug")) Debug = 1;
 if (strstr(ss, "-dd")==ss) {
  if (strstr(ss, "-dd:")==ss) Filtre = atol(&ss[4]);
  else Filtre = 2;
  if (Filtre>1) DMode = 4;
 }
 if (strstr(ss, "-df:")==ss) {
  Filtre = atol(&ss[4]);
  if (Filtre>1) DMode = 5;
 }
 if (strstr(ss, "-dh")==ss) {
  if (strstr(ss, "-dh:")==ss) Filtre = atol(&ss[4]);
  else Filtre = 1l;
  if (Filtre>0) DMode = 6;
 }
 if (!strcmp(ss, "-dl")) DMode = 2;
 if (!strcmp(ss, "-do")) DMode = 1;
 if (strstr(ss, "-e:")==ss) strcpy(EndTime,&ss[3]);
 if (strstr(ss, "-ed:")==ss) ExData = atol(&ss[4]);
 if (strstr(ss, "-f:")==ss) {
  strcpy(IFml[NbFml],&ss[3]);
  NbFml++;
 }
 if (!strcmp(ss, "-fs")) FullScl = 1;
 if (strstr(ss, "-fx")==ss) {
  p = strchr(ss,':');
  for (k = 3; k<(p-ss+1); k++) {
   j = IsCanal(ss[k]);
   if (j) DfxV[j-1] = atof(&p[1]);
  }
  DfN = 1;
 }
 if (strstr(ss, "-g:")==ss) strcpy(ChDraw, &ss[3]);
 if (strstr(ss, "-gif")==ss) {
  if (ss[4] == ':') strcpy(ExpGIFName, &ss[5]);
  else strcpy(ExpGIFName,"mgr.gif");
  ExpGIF = 1;
 }
 if (!strcmp(ss, "-gd")) GdView = 1;
 if (strstr(ss, "-hicum:")==ss) {
  strcpy(Doodson, &ss[7]);
  HiCum = 1;
 }
 if (strstr(ss, "-hs:")==ss) {
  NbHiCum = atoi(&ss[4]);
  if (NbHiCum<MINHICUM) NbHiCum = 360;
 }
 if (strstr(ss, "-i:")==ss) {
  strcpy(ImpFmt, &ss[3]);
  if (AutoFmt!=3) { DAS = 0;  AutoFmt = 0; }
 }
 if (!strcmp(ss, "-ic")) Virgule = 1;
 if (!strcmp(ss, "-im")) XMinus = 1;
 if (!strcmp(ss, "-iso")) FISO = 1;
 if (strstr(ss, "-it:")==ss) iT = atof(&ss[4]);
 if (!strcmp(ss, "-l")) {
  DAS = 1;
  AutoFmt = 2;
  strcpy(ImpFmt, "[yymmddhhnnss]");
  NbCR = 2;
  PassL = 1;  // premiŠre ligne de donn‚es toujours … ‚liminer...
 }
 if (!strcmp(ss, "-lw")) EpLigne = 3;
 if (strstr(ss, "-m:")==ss) {
  DAS = 1;
  MDiff = atol(&ss[3]);
  if (MDiff > MaxVal/2) MDiff = MaxVal - MDiff;
 } else {
  if (strstr(ss, "-m")==ss) DAS = 0;
 }
 if (strstr(ss, "-mdas")==ss) {
  strcpy(ImpFmt, "ymdhns");
  DAS = 1;
  AutoFmt = 4;
 }
 if (strstr(ss, "-merge")==ss) Merge = 1;
 if (strstr(ss, "-nan")==ss) {
  if (ss[4] == ':') NaN = atof(&ss[5]);
  RNaN = 1;
 }
 if (!strcmp(ss, "-nh")) strcpy(Header, "n");
 if (!strcmp(ss, "-nt")) NTOver = 1;
 if (!strcmp(ss, "-novga")) VGAT = 0;
 if (strstr(ss, "-o:")==ss) strcpy(FC_Out, &ss[3]);
 if (strstr(ss, "-os:")==ss) {
   OC = (char)atoi(&ss[4]);
   if (OC!=0 && OC!=10 && OC!=13 && OC!=100 && OC!=101 && (OC<48 || OC>57)) OS[0] = OC;
   else OC = ' ';
 }
 if (!strcmp(ss, "-pe")) PEcran = 1;
 if (strstr(ss, "-phase")==ss) {
  TimeMode = 3;
  if (strstr(strlwr(ss), "rad")) strcpy(Phase, "rad");
 }
 if (strstr(ss, "-ps")==ss) {
  if (ss[3] == ':') strcpy(ExpPSName, &ss[4]);
  else strcpy(ExpPSName,"mgr.ps");
  ExpPS = 1;
 }
 if (strstr(ss, "-spike")==ss) {
  Spike = 10.;
  if (strstr(ss, "-spike:")==ss) Spike = atof(&ss[7]);
  if (Spike<=0.) Spike = 0.;
 }
 if (!strcmp(ss, "-r")) TCenter = 1;
 if (!strcmp(ss, "-reunion")) {
  strcpy(ImpFmt, "n*");
  Bin = 3;  DAS = 0;  TTime = 1;
 }
 if (strstr(ss, "-s:")==ss) STime = atof(&ss[3]);
 if (strstr(ss, "-sc:")==ss) CMode = atoi(&ss[4])%4;
 if (!strcmp(ss, "-shot")) Shot = 1;
 if (strstr(ss, "-shot:")==ss) MaxVal = atol(&ss[6]);
 if (!strcmp(ss, "-sort")) Sort = 1;
 if (!strcmp(ss, "-t") || strstr(ss, "-t:")==ss) {
  ExpFile = 1;
  if (ss[2]==':') strcpy(ExpFmt, &ss[3]);
 }
 if (strstr(ss, "-test")==ss) {
  if (strstr(ss, "-test:")==ss) Test = atoi(&ss[6]) + 1;
  else Test = 1;
 }
 if (strstr(ss, "-title:")==ss) strcpy(FTitre, &ss[7]);
 if (strstr(ss, "-tf:")==ss) strcpy(DataFmt, &ss[4]);
 if (strstr(ss, "-tmp")==ss) {
  if (strstr(ss, "-tmp:")==ss) strcpy(FD_Tmp, &ss[5]);
  else strcpy(FD_Tmp, "");
 }
 if (!strcmp(ss, "-tr")) TTime = 0;
 if (strstr(ss, "-vga:")==ss) VGAMode = atoi(&ss[5]);
 if (strstr(ss, "-vm")==ss) {
  if (ss[3]==':') {
   Dfmax = atof(&ss[4]);
  } else {
   p = strchr(ss,':');
   for (k = 3; k<(p-ss+1); k++) {
    j = IsCanal(ss[k]);
    if (j) DfmV[j-1] = atof(&p[1]);
   }
  }
  DfN = 1;
 }
 if (strstr(ss, "-vn")==ss) {
  if (ss[3]==':') {
   Dfmin = atof(&ss[4]);
  } else {
   p = strchr(ss,':');
   for (k = 3; k<(p-ss+1); k++) {
    j = IsCanal(ss[k]);
    if (j) DfnV[j-1] = atof(&p[1]);
   }
  }
  DfN = 1;
 }
 if (strstr(ss, "-wave:")==ss) {
  strcpy(Doodson, &ss[6]);
 }
 if (strstr(ss, "-x:")==ss) PassL = atoi(&ss[3]);
 if (!strcmp(ss, "-xi")) XInfo = 1;
 if (!strcmp(ss, "-xt")) XText = 1;
 if (strstr(ss, "-y:")==ss) strcpy(AllTime,&ss[3]);
 if (strstr(ss, "-yn:")==ss) strcpy(AllTime,&ss[4]);
 if (strstr(ss, "-zb:")==ss) strcpy(BegZoom,&ss[4]);
 if (strstr(ss, "-ze:")==ss) strcpy(EndZoom,&ss[4]);
}


//**************************************************************************
// Lire_3Data: ouvre le fichier FF_In et en extrait les 3 premiŠres
// donn‚es valides. Pour les types de fichiers:
//  - Bin = 0 (ASCII): … partir de la chaŒne de format ImpFmt, d‚finit le
//    format de lecture pour sscanf fmt[], le nombre de canaux Cnx[];
//  - Bin = 1 (Binaire MDAS): pas utilis‚;
//  - Bin = 2 (Binaire æDAS HI): d‚finit Cnx[] et Acqu[];
//  - Bin = 3 (Binaire REUNION): ‚quivalent "n1-G".
// Si cela n'a pas encore ‚t‚ fait, d‚finit la p‚riode d'acquisition
// Acqu[] et ‚ventuellement le paramŠtre d'approximation AcPM.
// En mode test, affiche les donn‚es … l'‚cran.
//**************************************************************************

void Lire_3Data(void)
{
 char s0[MAXSTR];
 int i;
 double t;

 if ((f_in = fopen(FF_In, "rb"))==NULL) {
  sprintf(s0, "Cannot open '@%s@'.", FF_In);
  Sortie(s0);
 }
 if (Lire_XLignes()) {
  if (AutoFmt==3) Sortie("Bad DAS format: no date header.");
  else return;
 }
 Y = 0l;
 do {
  if (Bin!=3) if (fgets(Str, MAXSTR, f_in) == NULL) break;
  if (strstr(Str, "!HI ")) {		// format binaire æDAS
   Decode_v8();
   Bin = 2; strcpy(ImpFmt, "HI-æDAS");
   DAS = -1;  MDiff = MaxVal = pow(2,24);  PassL = 0;
   strcpy(ExpFmt, "ymdhns*");
   strcpy(FInfo[(NbInfo++)%MAXINFO], &Str[4]);
   break;
  }
  if (Test && !Bin) cputs(Str);
  if (Extract_Data(1)!=1) Y++;
  if (TimeMode==1) break;
  if (Y==2) t = Dte[0] - Dte[1];
 }
 while (Y<3);
 fclose(f_in);
 if (TimeMode!=1) {
  if (Acqu[nf]==0.) {              // n'a pas ‚t‚ d‚finit dans l'en-tˆte...
   Acqu[nf] = Dte[0] - Dte[1];
   if (Acqu[nf]!=t && Acqu[nf]!=0. && t!=0.) {
    if (fabs(log(fabs(Acqu[nf]/t)))>log(1.5)) Acqu[nf] = 0.;
    else AcPM = 1;
   }
  }
  if (!Append || nf==0) Dte[0] = Dte[1] - t;
 }
 for (i = 0; i<Cnx[nf]; i++) Cd[0][i] = Cd[1][i];
 if (DAS && !Test && Calib>0 && (Acqu[nf]==0. || AcPM==1))
  Sortie("Variable acquisition period. Cannot calibrate DAS.");
 if (Test) {
  sprintf(s0, "\r\n@%s@ : ", FF_In);
  Pt2(s0);
  if (AutoFmt) sprintf(s0, "'@%s@' format. @%d@ chan.", ImpFmt, Cnx[nf]);
  else sprintf(s0, "@%d@ column.", NbCol);
  Pt2(s0);
  if (Acqu[nf]!=0.) {
   sprintf(s0, " @%lg@ sec.", Acqu[nf]);
   Pt2(s0);
  }
 }
 if (Site) sprintf(FTitre, "STATION %04d - æDAS %03d", Site, UDAS);
}


//**************************************************************************
// Lire_XLignes: d‚code la date ATime pour les fichiers DAS (AutoFmt==3),
// puis lit les PassL premiŠres lignes du fichier ouvert = exclusion.
//**************************************************************************

int Lire_XLignes(void)
{
 char s0[15];
 int i, j, k;

 if (AutoFmt==3) {
  if (fgets(Str, MAXSTR, f_in) == NULL) return 1;
  if (Format(Str)) return 1;
  sscanf(Str, "%d %d %d", &i, &j, &k);
  switch (DegFmt) {
   case 1: sprintf(s0, "%02d", k);  break;
   case 2: sprintf(s0, "%02d%02d", k, j);  break;
   case 3: sprintf(s0, "%02d%02d%02d", k, j, i);  break;
  }
  if (DegFmt) ATime = Decode_Time(s0);
 }
 for (i = 0; i<PassL; i++) {
  if (Bin==3) fread(&k, 2, 13, f_in);
  else if (fgets(Str, MAXSTR, f_in) == NULL) return 1;
 }
 return 0;
}


//**************************************************************************
// Lire_Fichier_Origine: ouvre le fichier FF_In, lit toutes les donn‚es
// valides et les envoie … la fonction MAJ_Data(). Construit le fichier
// FC_Sort pour un tri des donn‚es.
//**************************************************************************

void Lire_Fichier_Origine(void)
{
 char s0[MAXSTR], s1[MAXSTR];
 unsigned char obin[12];
 int i, j, k = 0, fe = 0;
 long start, stamp;
 struct c_tm { time_t sec;
	 unsigned int fsec; } ct;

 if ((f_in = fopen(FF_In, "rb")) == NULL) Sortie("Input ORB.");
 Limite_Nom(FF_In, s1, 30/Rapx);
 sprintf(s0, "Importing '@%s@' (@%d@/%d). @Esc@ to abort.", s1, nf+1, NbFiles);
 Aff_Message(s0, 1, 0);

 Lire_XLignes();

 if (Bin==2) fgets(Str, MAXSTR, f_in);
 X = 0l; Y = 0l; ttt = 0.l;

 // =====  … partir du 2Šme fichier, inscrit un marqueur (-1) dans le tampon
 if (nf) {
  ct.sec = -1l;
  fwrite(&ct.sec, 4, 1, f_tmp0);
  FStamp[nf] = ftell(f_tmp0);
  if (!Append) DCh += Cnx[nf-1];
 }

 // ===== recalcule les Offset[] si les fichiers DAS ont des Acqu[] diff‚rents
 if (DAS>0 && Calib>0 && nf>0 && Append) {
  for (i = 0; i<Cnx[nf]; i++) {
   Offset[i] = (Cd[0][i]*Acqu[nf]/Acqu[nf-1])/MaxVal;
  }
 }

 // ===== boucle de lecture du fichier
 do {
  if (kbhit()) if (getch()==ESC) {
   fclose(f_in);
   fclose(f_tmp0);
   if (Append) fclose(f_tmp2);
   Sortie("");
  }

  if (Bin==2) {
   if (fread(&obin, 1, 3*Cnx[nf], f_in) < 3*Cnx[nf]) break;
   if (obin[0]==0xFD && obin[1]==0xFD) fread(&obin, 1, 3*Cnx[nf], f_in);
   if (obin[0]==0xFE && obin[1]==0xFE) { fe = 1;  break; }
   if (obin[0]==0xFF && obin[1]==0xFF) {
    Dte[0] = ((obin[2]*256l + obin[3])*256l + obin[4])*256l + obin[5];
    j = 1;  k++;
   } else {
    for (i = 0; i<Cnx[nf]; i++) {
     Cd[0][i] = (obin[i*3]*256l + obin[i*3+1])*256l + obin[i*3+2];
    }
    if (j==0) Dte[0] += Acqu[nf];
    else j = 0;
    j = Test_Data();
   }
  } else {
   if (Bin==0) if (fgets(Str, MAXSTR, f_in)==NULL) break;
   j = Extract_Data(0);
  }

  if (j==0) {
   if (Calib && (Dte[0]<CBTime || Dte[0]>=CETime)) Lire_Calib(&Dte[0], 1);

   // ===== Ecriture des marques pour le tri des donn‚es
   if (Append) {
    if (X==0l || Dte[0]<Dte[1] || (Dte[0]-Dte[1])>1.5*Acqu[nf]) {
     stamp = ftell(f_tmp0);
     start = Dte[0];
     if (X==0l) {
      fwrite(&start, sizeof(start), 1, f_tmp2);
      fwrite(&stamp, sizeof(stamp), 1, f_tmp2);
     } else {
      fwrite(&stamp, sizeof(stamp), 1, f_tmp2);
      fwrite(&start, sizeof(start), 1, f_tmp2);
      fwrite(&stamp, sizeof(stamp), 1, f_tmp2);
     }
    }
   }

   // ===== mise … jour de toutes les variables et ‚criture dans le tampon
   MAJ_Data(1);

   FData = 0;
  }
  if (j==-1) break;
  Working(TCal/(double)TF_Tot, TCal2/(double)TF_Tot);
  switch (Bin) {
   case 0: TCal += strlen(Str) + NbCR; break;
   case 2: TCal += 3*Cnx[nf]; break;
   case 3: TCal += 2 + 1.5*Cnx[nf]; break;
  }
 }
 while (1);
 NbY += Y;
 fclose(f_in);
 if (Bin==2) {
  sprintf(s0, "time stamp %04d", k);
  if (fe) sprintf(FInfo[NbInfo], "%s - EOF OK\n", s0);
  else sprintf(FInfo[NbInfo], "%s - EOF not found\n", s0);
  if (NbInfo<10) NbInfo++;
 }
}


//**************************************************************************
// MAJ_Data: Calibre les donn‚es courantes se trouvant dans Dte[] et Cd[][]
// et met … jour toutes les variables: Y, DataOut[], NbDO[], Offset[],
// Vmax[], Vmin[], Resol[], Bruit[], TDmax[], TDmin[], Tmax, Tmin, ResolMin.
// Si m==1, les inscrit en binaire dans le fichier tampon f_tmp0
// pr‚alablement ouvert.
//**************************************************************************

void MAJ_Data(char m)
{
 double ddn;
 long diff;
 int i, di;
 struct c_tm { time_t sec;
	 unsigned int fsec; } ct;

 X++;
 for (i = 0; i<Cnx[nf]; i++) {
  di = i + DCh;
  if (X>1 && Cd[0][i]!=NaN) {
   if (DAS>0) {
    if (!DataOut[di]) diff = Cd[0][i] - Cd[1][i] + Offset[di]*MaxVal;
    else diff = 0l;
    if (labs(diff)<(MaxVal - MDiff) && labs(diff)>MDiff) {
     DataOut[di] = 1;
     NbDO[di]++;
    } else {
     DataOut[di] = 0;
     if (labs(diff)>=(MaxVal - MDiff)) {
      Offset[di] -= Round(diff/(double)MaxVal);
      NbSauts[di]++;
     }
    }
   }
   //if (Cd[0][i]==0.)  NbDO[di]--;
  }
  if (Cd[0][i]!=NaN) {
   if (DAS>0) Cd[0][i] += Offset[di]*MaxVal;
   if ((DAS>0 && Cd[0][i]==0.) || Cd[0][i]==NaN) NbNul[di]++;
   if (Decim>1 && X>1) Data[0][di] += Cd[0][i];
   else Data[0][di] = Cd[0][i];
   if (X%Decim == 0l) {
    if (!DAS && X>1 && Frequence[di]!=-1) Data[0][di] /= (double)Decim;
    if (Calib>0) Calib_Data(&Data[0][di], di);
    if (Y==0l && (!Append || FData || nf==0)) {
     Vmax[di] = Vmin[di] = Data[0][di];
     Data[1][di] = Data[0][di];
    }
    Vmax[di] = Max(Vmax[di], Data[0][di]);
    Vmin[di] = Min(Vmin[di], Data[0][di]);
    if (Data[1][di]!=NaN) {
     ddn = Data[0][di] - Data[1][di];
     if (ddn!=0.) Resol[di] = Min(Resol[di], fabs(ddn));
     Bruit[di] += ddn*ddn;
    }
   }
  }
 if (TimeMode==3) Calc_Harmonic(di, 1);
 }
 ttt += Dte[0] - Tmin;
 if (X%Decim == 0l) {
  if (Decim>1) Dte[0] = (double)(ttt/Decim) + Tmin;
  if (Y==0l && (FData || nf==0)) Tmax = Tmin = Dte[0];
  TDmax[nf] = Max(Dte[0], TDmax[nf]);
  TDmin[nf] = Min(Dte[0], TDmin[nf]);
  Tmax = Max(Tmax, TDmax[nf]);
  Tmin = Min(Tmin, TDmin[nf]);

  // ===== ‚criture des donn‚es dans le fichier tampon binaire
  if (m) {
   ct.sec = floor(Dte[0]);  ct.fsec = Frac(Dte[0])*MSEC;
   fwrite(&ct.sec, 4, 1, f_tmp0);
   fwrite(&ct.fsec, 2, 1, f_tmp0);
  }
  for (i = 0; i<Cnx[nf]; i++) {
   di = i + DCh;
   if (m) fwrite(&Data[0][di], 8, 1, f_tmp0);
   ResolMin = Min(ResolMin, Resol[di]);
   if (Data[0][di]!=NaN) MAJ_RL(di, Dte[0] - Tmin, Data[0][di]);
   Data[1][di] = Data[0][di];
   Data[0][di] = 0.;
  }
  ttt = 0.l;
  Y++;
 }
 switch (Bin) {
  case 0: TCal2 += strlen(Str) + NbCR;  break;
  case 2: TCal2 += 3*Cnx[nf];  break;
  case 3: TCal2 += 2 + 1.5*Cnx[nf];  break;
 }
}


void Init_Variables(void)
{
 int i;

 for (i = 0; i<MAXCNX; i++) {
  Bruit[i] = 0.;
  Resol[i] = MAXDOUBLE;
  DfnV[i] = -MAXDOUBLE;
  DfmV[i] = MAXDOUBLE;
  DfxV[i] = MAXDOUBLE;
  Vmax[i] = -MAXDOUBLE;
  Vmin[i] = MAXDOUBLE;
 }
 for (i = 0; i<MAXFI; i++) {
  TDmax[i] = -MAXDOUBLE;
  TDmin[i] = MAXDOUBLE;
 }
 strcpy(AllCh, "");
 strcpy(AllChN, "");
}

void Calc_Variables(void)
{
 char s0[5];
 int i;

 Tscl = Tmax - Tmin;
 MAJ_DV();
 MAJ_AD();
 CalcZ();
 for (i = 0; i<Canx; i++) {
  Calc_RL(i);
  AllCh[i] = CarCan(i);
  sprintf(s0, "%c,", CarCan(i));
  strcat(AllChN, s0);
  Gt[i] = 1;  Gm[i] = 0;
  if (TimeMode==3) Calc_Harmonic(i, 0);
 }
 AllCh[i] = NULL;
 if (strlen(ChDraw)==0) strcpy(ChDraw, AllCh);
}


//**************************************************************************
// Extract_Data: d‚code la chaŒne de caractŠre *Str en fonction du format
// ImpFmt. Si md == 1 (appel de Lire_3Data), compte d'abord le nombre de
// colonnes NbCol, puis d‚finit la chaŒne fmt[] pour le sscanf().
// D‚finit Dte[0] et Cd[0][i] et appelle Test_Data(): retourne 0 si ces deux
// variables sont valides, 1 si les Cd[0][i] ne sont pas valides et 2 si seul
// Dte[0] ne permet pas le calcul de Acqu[].
//**************************************************************************

int Extract_Data(int md)
{
 char c, s0[80], s1[80], s2[80], *p, *p1;
 unsigned char ob[3];
 int i, i0, j, k, tmp, tmp1;
 struct tm date, date2;
 struct c_tm { time_t sec;
	 unsigned int fsec; } ct;

 Dte[1] = Dte[0];  Dte[0] = 0.;
 for (i = 0; i<Cnx[nf]; i++) Cd[1][i] = Cd[0][i];
 if (AutoFmt==2) {
  if (Str[0]!='*') return 1;
  else for (i = 0; i<strlen(AllTime); i++) Str[i+1] = AllTime[i];
 }
 if (Format(Str)) return 1;
 if (md && Bin==0) {
  p = Str;
  NbColFmt = Decode_Format(ImpFmt);

  // ===== ignore les premiers espaces de *Str
  while (p[0]==' ') p++;

  // ===== calcule le nombre de colonnes NbCol (dans *Str)
  NbCol = 0;
  while ((p = strstr(p, " ")) != NULL) {
   NbCol++;
   do p++; while (p[0]==' ');
  }

  // ===== v‚rifie l'‚galit‚ entre le nombre de colonnes ImpFmt et NbCol
  //       et calcule le nombre de canaux Cnx[]
  if (NbColFmt<=0) {
   Cnx[nf] = NbCol + NbColFmt;
   if (Cnx[nf]>MAXCNX) Cnx[nf] = MAXCNX;
   NbColFmt = Cnx[nf] - NbColFmt;
   //if (Cnx[nf]<1) Erreur_Format();
  } else {
   //if (NbCol!=strlen(ImpFmt)) Erreur_Format();
   for (i = 0; i<strlen(ImpFmt); i++) {
    Cnx[nf] = 0;
    j = IsCanal(ImpFmt[i]);
    if (j>Cnx[nf]) Cnx[nf] = j;
   }
  }

  // ===== construit fmt[] pour le sscanf(Str, ...)
  strcpy(Fmt[nf], "");
  for (i = 0; i<NbColFmt; i++) strcat(Fmt[nf], "%lf ");
 }

 switch (Bin) {
  case 0: sscanf(Str, Fmt[nf],
	  &ChV[0], &ChV[1], &ChV[2], &ChV[3], &ChV[4], &ChV[5],
	  &ChV[6], &ChV[7], &ChV[8], &ChV[9], &ChV[10], &ChV[11],
	  &ChV[12], &ChV[13], &ChV[14], &ChV[15], &ChV[16],
	  &ChV[17], &ChV[18], &ChV[19], &ChV[20], &ChV[21],
	  &ChV[22], &ChV[23], &ChV[24], &ChV[25], &ChV[26],
	  &ChV[27], &ChV[28], &ChV[29], &ChV[30], &ChV[31],
	  &ChV[32], &ChV[33], &ChV[34], &ChV[35], &ChV[36],
	  &ChV[37], &ChV[38], &ChV[39]);  break;
  case 3: if (fread(&ob, 1, 2, f_in)<2) return -1;
	  ChV[0] = ob[1] + ob[0]*256.;
	  for (i = 0; i<8; i++) {
	   if (fread(&ob, 1, 3, f_in)<3) return -1;
	   ChV[2*i+1] = ob[0] + (ob[1]/16)*256.;
	   ChV[2*i+2] = ob[2] + (ob[1]%16)*256.;
	  }
	  Cnx[nf] = 16;  break;
 }
 ct.sec = (long) floor(ATime);  ct.fsec = (int) (Frac(ATime)*MSEC);
 date = *gmtime(&ct.sec);
 for (i0 = 0, i = 0; i0<strlen(ImpFmt); i0++) {
  c = ImpFmt[i0];
  if (c=='y' || c=='Y') {
   if (ChV[i]>=1970) ChV[i] -= 1900;
   if (ChV[i]<70) ChV[i] += 100;    // 00 … 69 => ann‚es 2000 … 2069!
   date.tm_year = (int) ChV[i];
  }
  if (c=='m' || c=='b') { date.tm_mon = (int) ChV[i] - 1; }
  if (c=='d') {
   date.tm_mday = (int) ChV[i];
   ct.sec = mktime(&date) + (long) Round(NbSec[2]*Frac(ChV[i]));
   date = *gmtime(&ct.sec);
  }
  if (c=='j' || c=='f') {
   ct.sec = mktime(&date);
   if (Acmin==ceil(Acmin)) {
    ct.sec += Round(NbSec[2]*(ChV[i] - 1.));
   } else {
    ct.sec += (long) floor(NbSec[2]*(ChV[i] - 1.));
    ct.fsec += (int) Frac(NbSec[2]*(ChV[i] - 1.));
    if (ct.fsec>=MSEC) { ct.sec++; ct.fsec -= MSEC; }
   }
   date = *gmtime(&ct.sec);
  }
  if (c=='h') {
   ct.sec = mktime(&date) + (long) Round(NbSec[3]*ChV[i]);
   date = *gmtime(&ct.sec);
  }
  if (c=='n') {
   ct.sec = mktime(&date) + 60*ChV[i];
   date = *gmtime(&ct.sec);
  }
  if (c=='s') {
   ct.sec = mktime(&date) + floor(ChV[i]);
   ct.fsec += Frac(ChV[i])*MSEC;
   if (ct.fsec>=MSEC) { ct.sec++; ct.fsec -= MSEC; }
   date = *gmtime(&ct.sec);
  }
  if (c=='t') {
   //sprintf(s0, "%-12lf", ChV[i]);
   //ct.sec = (long) Decode_Time(s0);
   ct.sec = (long) floor(NbSec[2]*(ChV[i] - Date1970));
   ct.fsec = (int) Frac(NbSec[2]*(ChV[i] - Date1970));
   date = *gmtime(&ct.sec);
  }
  if (c=='k' || c=='l' || c=='q') {
   if (c=='q') ChV[i] *= 10000.;
   if (c=='k') ChV[i] *= 100.;
   sprintf(s0, "%06ld", (long) ChV[i]);
   sscanf(s0, "%02d%02d%02d", &date.tm_hour, &date.tm_min, &date.tm_sec);
  }
  if (c=='g') {
   sprintf(s0, "%08ld", (long) ChV[i]);
   sscanf(s0, "%04d%02d%02d", &tmp, &tmp1, &date.tm_mday);
   date.tm_year = tmp - 1900;
   date.tm_mon = tmp1 - 1;
  }
  if (c=='[') {
   strcpy(s1, &ImpFmt[i0]);
   p = strtok(s1, "[]");
   p1 = strchr(p, '.');
   if (p1) sprintf(s2, "%%0%d.%df", strlen(p), strlen(p1)-1);
   else sprintf(s2, "%%0%d.f", strlen(p));
   sprintf(s0, s2, ChV[i]);
   //cprintf("p = %s ; format = %s ; ChV[i] = %g ; s0 = %s\n\r", p, s2, ChV[i], s0);
   for (k = 0; k<strlen(p); k++) {
    if (p[k]=='y') {
     j = 1;
     while (p[k+j]=='y') j++;
     strcpy(s2, &s0[k]);
     s2[j] = '\0';
     tmp = atoi(s2);
     if (tmp>1900) date.tm_year = tmp - 1900;
     else date.tm_year = tmp;
     k += j - 1;
     //cprintf("s2 = %s ;", s2);
    }
    if (p[k]=='m') {
     j = 1;
     while (p[k+j]=='m') j++;
     strcpy(s2, &s0[k]);
     s2[j] = '\0';
     date.tm_mon = atoi(s2) - 1;
     k += j - 1;
     //cprintf("s2 = %s ;", s2);
    }
    if (p[k]=='d') {
     j = 1;
     while (p[k+j]=='d') j++;
     strcpy(s2, &s0[k]);
     s2[j] = '\0';
     date.tm_mday = atoi(s2);
     k += j - 1;
     //cprintf("s2 = %s ;", s2);
    }
    if (p[k]=='j') {
     j = 1;
     while (p[k+j]=='j') j++;
     strcpy(s2, &s0[k]);
     s2[j] = '\0';
     ct.sec = mktime(&date) + NbSec[2]*(atoi(s2) - 1.);
     date = *gmtime(&ct.sec);
     k += j - 1;
     //cprintf("s2 = %s ;", s2);
    }
    if (p[k]=='h') {
     j = 1;
     while (p[k+j]=='h') j++;
     strcpy(s2, &s0[k]);
     s2[j] = '\0';
     date.tm_hour = atoi(s2);
     k += j - 1;
     //cprintf("s2 = %s ;", s2);
    }
    if (p[k]=='n') {
     j = 1;
     while (p[k+j]=='n') j++;
     strcpy(s2, &s0[k]);
     s2[j] = '\0';
     date.tm_min = atoi(s2);
     k += j - 1;
     //cprintf("s2 = %s ;", s2);
    }
    if (p[k]=='s') {
     j = 1;
     while (p[k+j]=='s') j++;
     strcpy(s2, &s0[k]);
     s2[j] = '\0';
     date.tm_sec = atoi(s2);
     k += j - 1;
     //cprintf("s2 = %s ;", s2);
    }
   }
   i0 += strlen(p) + 1;
  }
  if (c=='i') Dte[0] = ChV[i];

  // ===== importation des donn‚es canal
  j = IsCanal(c);
  if (j>0 && j<=Cnx[nf]) Cd[0][j-1] = ChV[i];
  if (c=='*') {
   for (j = 0; j<Cnx[nf]; j++) Cd[0][j] = ChV[i+j];
   break;  // le reste de la chaŒne ImpFmt est ignor‚...
  }
  i++;
 }

 if (TimeMode || iT) {
  if (Dte[0]==0.) {
   if (iT) {
    Dte[0] = ATime + Y*iT;
    if (Append) Dte[0] += NbY*iT;
   } else {
    if (TimeMode==1) {
     Dte[0] = Y + 1;
     if (Append) Dte[0] += NbY;
    }
   }
  }
 } else {
  Dte[0] = mktime(&date) + STime*NbSec[3] + ct.fsec/MSEC;
 }
 if (X && Dte[0]<=Dte[1]) {
  if (NTOver && !Sort && (Y>0 || nf>0)) {
   Dte[0] = Dte[1];
   for (i = 0; i<Cnx[nf]; i++) Cd[0][i] = Cd[1][i];
   return (1);
  }
  if (DegFmt && !TTime) {
   Dte[0] += NbSec[DegFmt-1];
   ATime += NbSec[DegFmt-1];
  } else {
   NTTest = 1;
  }
 }
 if (DegFmt) {
  date2 = date;
  date.tm_year++;
  NbSec[0] = mktime(&date) - mktime(&date2);
  date.tm_year--;
  date.tm_mon++;
  NbSec[1] = mktime(&date) - mktime(&date2);
 }
 //printf("%lf %lf %lf %lf\n", Cd[0][0], Cd[0][1], Cd[0][2], Cd[0][3]);
 return (Test_Data());
}


//**************************************************************************
// Test_Data: Test sur la ligne de donn‚es venant d'ˆtre lue dans le fichier
// et d‚cod‚e (date Dte[] et valeurs brutes Cd[][]). Exclusion de toute la
// ligne ou remplacement des valeurs par NaN (si RNaN == 1) si:
//    - diff(Cd[]) > DfxV[] (option /fxN:).
//    - tous les canaux sont nuls (donn‚es DAS non binaires),
//    - certains canaux sont en dehors des bornes (options /vn et /vm),
//    - la date est en dehors des bornes (options /e: et /b:).
//**************************************************************************

int Test_Data(void)
{
 int i, di, ok;
 double d2;

 for (i = 0, ok = 0; i<Cnx[nf]; i++) {
  di = i + DCh;

  // ===== Elimination des lignes de 0 pour les donn‚es æDAS
  if (!Com && DAS>0 && Cd[0][i]==0.) ok++;

  // ===== Test sur les donn‚es en dehors des bornes (/vn et /vm)
  if (Cd[0][i]<Dfmin || Cd[0][i]>Dfmax
      || Cd[0][i]<DfnV[di] || Cd[0][i]>DfmV[di]) {
   if (RNaN) { Cd[0][i] = NaN;  DataOut[di] = 1;  ok++; }
   else ok = Cnx[nf];
  }


  // ===== Test sur les donn‚es d‚passant la d‚riv‚e (/fx)
  if (Y && Cd[0][i]!=NaN && !DataOut[di]) {
   d2 = Cd[0][i] - Cd[1][i];
   if (DAS>0) {
    d2 += Offset[di]*MaxVal;
    if (labs(d2)>=(MaxVal - MDiff)) d2 -= MaxVal*Round(d2/(double)MaxVal);
   }
   if (Acqu[nf]!=0. && Dte[0]>Dte[1]) d2 *= Acqu[nf]/(Dte[0] - Dte[1]);
   if (fabs(d2)>DfxV[di]) {
    if (RNaN) { Cd[0][i] = NaN;  DataOut[di] = 1;  ok++; }
    else ok = Cnx[nf];
   }
  }
 }

 // ===== Si toute la ligne de donn‚es est … exclure...
 if (ok>=Cnx[nf]) {
  Dte[0] = Dte[1];
  for (i = 0; i<Cnx[nf]; i++) Cd[0][i] = Cd[1][i];
  return 1;
 } else {
  if (TCenter) Dte[0] -= (Decim*Acqu[nf]) / 2.;
  if (BTime<ETime) {
   if (Dte[0]<BTime || Dte[0]>ETime) return 2;
  } else {
   if (Dte[0]<BTime && Dte[0]>ETime) return 2;
  }
  if (ExData) if (((long)Dte[0])%ExData) return 2;
  return 0;
 }
}


//**************************************************************************
// Erreur_Format: stoppe le programme si le format d'entr‚e (ImpFmt) ne
// correspond pas aux nombre de colonnes d‚tect‚es (NbCol).
//**************************************************************************

void Erreur_Format()
{
 char s0[128];

 fclose(f_in);
 sprintf(s0, "Bad input format \"%s\": %d column", ImpFmt, NbCol);
 if (NbCol==1) strcat(s0, " found or possibly binary format.");
 else strcat(s0, "s found.");
 strcat(s0, "\n\r       See import options help.");
 //if (Test) cprintf("not ok with \"%s\".", se); else
 Sortie(s0);
}


//**************************************************************************
// Format: transforme la chaŒne de caractŠre (sf) en fonction des options de
// format (XText, Virgule, XMinus) pour que toutes les colonnes ne soient
// que des chiffres s‚par‚s par des espaces. Retourne 1 si la chaŒne est
// devenue une ligne d'espaces, 0 sinon.
//**************************************************************************

int Format(char *sf)
{
 char guill = 0;
 int i, nbk = 0, lch = strlen(sf);

 if (sf[0]=='#') { Decode_Ligne(sf);  return (1); }
 if (Bin) return (0);
 if (strchr(ImpFmt, 'b')) Decode_Mois(sf);
 for (i = 0; i<lch; i++) {
  if (sf[i]=='"') guill = (guill + 1)%2;
  if (XText && ((isalpha(sf[i])!=0 && tolower(sf[i])!='e')
      || (iscntrl(sf[i])!=0 && isspace(sf[i])==0))) return (1);
  if (Virgule && sf[i]==',') sf[i] = '.';
  if (strchr(Nombre_String, sf[i])==NULL
      || (XMinus && sf[i]=='-')
      || (guill && sf[i]=='.')) { sf[i] = ' ';  nbk++; }
  if (sf[i]=='-' && i>0 && sf[i-1]!=' '
     && tolower(sf[i-1])!='e') { sf[i] = ' ';  nbk++; }
 }
 if (nbk!=lch) return (0);
 else return (1);
}


//**************************************************************************
// MAJ_DE: Mise … jour des variables Dpas[], Dmax[], Dmin[]
// relatives aux donn‚es … partir des variables Emax[] et Emin[] calcul‚es
// lors du trac‚ du graphique actuel.
//**************************************************************************

void MAJ_DE(void)
{
 int i;

 for (i = 0; i<Canx; i++) {
  Dmax[i] = Emax[i];//ceil(Emax[i]/Dpas[i])*Dpas[i];
  Dmin[i] = Emin[i];//floor(Emin[i]/Dpas[i])*Dpas[i];
 }
}


//**************************************************************************
// MAJ_DV: Mise … jour des variables Dpas[], Dmax[], Dmin[]
// relatives aux donn‚es … partir des variables Vmax[] et Vmin[] relatives
// aux donn‚es originales.
//**************************************************************************

void MAJ_DV(void)
{
 int i;

 for (i = 0; i<Canx; i++) {
  Dmax[i] = Emax[i] = Vmax[i];//ceil(Vmax[i]/Dpas[i])*Dpas[i];
  Dmin[i] = Emin[i] = Vmin[i];//floor(Vmin[i]/Dpas[i])*Dpas[i];
 }
}


//**************************************************************************
// MAJ_AD: Mise … jour des variables Apas[], Amax[], Amin[] et Ascl[]
// relatives aux donn‚es … tracer … partir des variables Dpas[], Dmax[],
// Dmin[] relatives aux donn‚es.
//**************************************************************************

void MAJ_AD(void)
{
 int i;

 for (i = 0; i<Canx; i++) {
  Amax[i] = Dmax[i];
  Amin[i] = Dmin[i];
  Ascl[i] = Dmax[i] - Dmin[i];
  if (Ascl[i]==0.) Ascl[i] = 1.;
  Apas[i] = Grid(Ascl[i], 0);
 }
}


//**************************************************************************
// MAZ_RL: Mise … z‚ro des variables pour le calcul de correction d'offset
// et correction lin‚aire.
//**************************************************************************

void MAZ_RL(int i)
{
 Sx[i] = 0.;
 Sy[i] = 0.;
 Sxy[i] = 0.;
 Sy2[i] = 0.;
 Sx2[i] = 0.;
 Sn[i] = 0l;
}


//**************************************************************************
// MAJ_RL: Mise … jour des compteurs pour le calcul de correction d'offset
// et correction lin‚aire.
//**************************************************************************

void MAJ_RL(int i, double lrx, double lry)
{
 Sx[i] += lrx;
 Sy[i] += lry;
 Sxy[i] += lrx*lry;
 Sy2[i] += lry*lry;
 Sx2[i] += lrx*lrx;
 Sn[i]++;
}


//**************************************************************************
// Calc_RL: Calcul des paramŠtres pour la correction d'offset et la
// correction lin‚aire. Moyenne RLz[] et droite RLa[] *x + RLb[].
//**************************************************************************

void Calc_RL(int i)
{
 double RLtmp;

 if (Sn[i]) {
  RLz[i] = Sy[i]/Sn[i];
  RLtmp = (Sn[i]*Sx2[i] - Sx[i]*Sx[i]);
  RLb[i] = (RLtmp == 0.) ? NaN:(Sy[i]*Sx2[i] - Sx[i]*Sxy[i]) / RLtmp;
  RLtmp = Sx2[i];
  RLa[i] = (RLtmp == 0.) ? NaN:(Sxy[i] - RLb[i]*Sx[i])/RLtmp;
 }
}


//**************************************************************************
// Eval_RL: Calcul sur les donn‚es din[][i] et tin[] en fonction de DMode et
// renvoi du r‚sultat dans **dout. Attention: utilise la variable globale X
// pour reconnaŒtre la premiŠre donn‚e.
//**************************************************************************

void Eval_RL(int i, double dout[2][MAXCNX], double din[2][MAXCNX], double tin[2])
{
 char s0[MAXHARM+1];
 int j;
 unsigned long li;

 if (din[0][i]==NaN) din[0][i] = din[1][i];
 dout[1][i] = dout[0][i];
 switch (DMode) {
  case 1:   // ===== Correction d'offset
   dout[0][i] = din[0][i] - RLz[i];
   break;
  case 2:   // ===== Correction lin‚aire
   if (DRef==-1) {
    dout[0][i] = din[0][i] - (tin[0] - Tmin)*RLa[i] - RLb[i];
   } else {
    if (din[0][DRef]==NaN) din[0][DRef] = din[1][DRef];
    if (i!=DRef) {
     dout[0][i] = din[0][i] - din[0][DRef]*RLa[i] - RLb[i] + RLz[i];
    } else {
     dout[0][i] = din[0][i];
    }
   }
   break;
  case 3:   // ===== Calcul de somme cumul‚e
   if (X==0l) dout[1][i] = 0.;
   dout[0][i] = din[0][i] + dout[1][i];
   break;
  case 4:   // ===== Calcul de d‚riv‚e sur 2 points (sinon calcul de filtre)
   if (Filtre==2l) {
    dout[0][i] = din[0][i] - din[1][i];
    if (TimeMode!=1 && tin[0]!=tin[1]) dout[0][i] /= (tin[0] - tin[1]);
    if (Acqu[nf]!=0.) dout[0][i] *= Acqu[nf];
    break;
   }
  case 5:   // ===== Calcul de filtre
   dout[0][i] = 0.;
   RLm[(int)(X%Filtre)][i] = din[0][i];
   if (X==0l) for (li = 0l; li<Filtre; li++) RLm[(int)li][i] = din[0][i];
   for (li = 0l; li<Filtre; li++)
    dout[0][i] += RLc[(int)((li-X%Filtre+Filtre)%Filtre)][0] * RLm[(int)li][i];
   if (X==0l) dout[1][i] = dout[0][i];
   break;
  case 6:   // ===== Correction d'harmoniques
   dout[0][i] = din[0][i];
   itoa((int)Filtre, s0, 2);
   for (j = 0; j<strlen(s0); j++) {
    if (s0[strlen(s0)-j-1]!='0') dout[0][i] -= HarmC[j][i]*cos((tin[0]*(j+1.) - HarmS[j][i])*M_PI/180.);
   }
   break;
  default:
   dout[0][i] = din[0][i];
 }
}


//**************************************************************************
// Init_Filter: si Filtre>1, alloue de la m‚moire pour les 2 tableaux RLm[]
// et RLc[][] utilis‚s pour les filtres:
// 	- DMode = 4 (diff‚rence)
//	- DMode = 5 (moyenne glissante)
// sur Filtre donn‚es cons‚cutives et d‚finit les coefficients de RLc[][].
//**************************************************************************

int Init_Filter(void)
{
 unsigned long li;

 // ===== Attention: Coefficients de RLc[][] d‚finis en boucle inverse...
 //       RLc[i][0] appliqu‚ … la donn‚e (Filtre - i) modulo Filtre;
 //       RLc[i][1] pour filtres r‚cursifs.
 if (Filtre<2) return 1;
 else {
  RLm = farmalloc(Filtre*sizeof(*RLm));
  RLc = farmalloc(Filtre*sizeof(*RLc));
  if (RLm==NULL || RLc==NULL) return 1;
  for (li = 0l; li<Filtre; li++) RLc[(int)li][0] = 0.;
  switch (DMode) {
  case 4:
   RLc[0][0] = 1./(double)Filtre;
   RLc[1][0] = -1./(double)Filtre;
   break;
  case 5:
   for (li = 0l; li<Filtre; li++) RLc[(int)li][0] = 1./(double)Filtre;
   break;
  }
 }
 return (0);
}


//**************************************************************************
// Eff_Filter: LibŠre la m‚moire pour les 2 tableaux RLm[] et RLc[][].
//**************************************************************************

void Eff_Filter(void)
{
 farfree(RLm);
 farfree(RLc);
}


//**************************************************************************
// Init_Axes: D‚codage de la chaŒne ChDraw. Calcule ensuite les paramŠtres
// g‚n‚raux de chaque graphe: Gpas[], Gmax[], Gmin[], Gscl[], Gy1[] et Gy2[].
//**************************************************************************

void Init_Axes(void)
{
 int i, j, k;
 double dmax;

 // ===== remplace tous les '#' par des ',' dans ChDraw (pour v <= 1.5c)
 for (i = 0; i<strlen(ChDraw); i++) if (ChDraw[i]=='#') ChDraw[i] = ',';
 Decode_ChDraw();
 for (i = 0; i<Canx; i++) {
  if (Gn[i]) {
   k = Gn[i] - 1;
   if (Jg[k]==0) { Gmax[k] = Amax[i];  Gmin[k] = Amin[i]; }
   Gmax[k] = Max(Gmax[k], Amax[i]);
   Gmin[k] = Min(Gmin[k], Amin[i]);
   if (Ge[i]) {
    j = Ge[i]-1;
    dmax = Max(fabs(Amax[j]), fabs(Amin[j]));
    Gmax[k] += dmax;
    Gmin[k] -= dmax;
   }
   /* if (Gm[i]==5) {
    Gmax[k] = Max(Gmax[k], 0.);
    Gmin[k] = Min(Gmin[k], 0.);
   } */
   Jg[k]++;
  }
 }                                              // TOUS CANAUX AFFICHES:
 for (i = 0; i<NbGn; i++) {
  Gpas[i] = Grid((Gmax[i] - Gmin[i])/(double)Gs[i], 0);    // pas
  Gmax[i] = ceil(Gmax[i]/Gpas[i])*Gpas[i];                 // maximum
  Gmin[i] = floor(Gmin[i]/Gpas[i])*Gpas[i];                // minimum
  Gscl[i] = Gmax[i] - Gmin[i];                             // ‚chelle
  if (Gscl[i]==0.) Gscl[i] = 1.;
  Gys[i+1] = Gs[i]*(Gys[0] - 10*(NbGn - 1))/(double)NbGs;
  if (!i) Gy1[i+1] = Gy1[0];
  else Gy1[i+1] = Gy2[i] + 10;
  Gy2[i+1] = Gy1[i+1] + Gys[i+1];
 }
 CalcZ();
}


//**************************************************************************
// Aff_Legendes: Affiche le titre, trace les bordures, axes, grilles,
// l‚gendes (nom et unit‚), et ascenseurs pour tous les graphes.
//**************************************************************************

void Aff_Legendes(void)
{
 char s0[128], s1[128], s2[128];
 double g, gt;
 double t;
 int i, j, Lift, ok;
 struct tm date;
 struct c_tm { time_t sec;
	 unsigned int fsec; } ct;

 if (!ExpPS) {
  Aff_Souris(2);
  setwritemode(0);
  if (Info) Aff_Info(0);
  Efface_Graph();
 }
 if ((Book || Calib) && Gv==-1) Aff_Book();

 if (strlen(FTitre)) Pl(FTitre, 0., -1);
 Aff_Noms(ChAff);
 //settextstyle(2, 1, 0);
 if (!PEcran && !ExpPS) {
  Vide(Gx1, Gx2, Gy1[0] - 9, Gy1[0] - 4, DARKGRAY);
  LBar1 = Gx1 + Gxs*((Zmin - Tmin)/Tscl);
  LBar2 = Gx1 + Gxs*((Zmax - Tmin)/Tscl);
  Vide(LBar1, LBar2, Gy1[0] - 9, Gy1[0] - 4, LIGHTGRAY);
 }
 switch (TimeMode) {
  case 1:
   sprintf(s0, "ÀÄ raw %lg", Zmin);
   sprintf(s1, "raw %lg ÄÙ", Zmax);
   break;
  case 2:
   sprintf(s0, "ÀÄ data %lg", Zmin);
   sprintf(s1, "data %lg ÄÙ", Zmax);
   break;
  case 3:
   sprintf(s0, "ÀÄ %lg %s", Zmin, Phase);
   sprintf(s1, "%lg %s ÄÙ", Zmax, Phase);
   break;
  default:
   ct.sec = floor(Zmin);  ct.fsec = Frac(Zmin)*MSEC;  date = *gmtime(&ct.sec);
   strftime(s0, 40, "ÀÄ %d %b %Y %X", &date);
   if (Frac(Acmin)) { sprintf(s2, ".%04d", ct.fsec); strcat(s0, s2); }
   ct.sec = floor(Zmax);  ct.fsec = Frac(Zmax)*MSEC;  date = *gmtime(&ct.sec);
   strftime(s1, 40, "%d %b %Y %X", &date);
   if (Frac(Acmin)) { sprintf(s2, ".%04d", ct.fsec); strcat(s1, s2); }
   strcat(s1, " ÄÙ");
 }
 Pl(s0, 0., 4);
 Pl(s1, 0., 5);
 if (!ExpPS) {
  setcolor(WHITE);
  if (Zoom!=ceil(Zoom)) sprintf(s0, "x%1.1f", Zoom);
  else sprintf(s0, "x%1.f", Zoom);
  Pg(s0, 4, My - 30);
  if (NTOver) Pg("NT ", -14, My - 30);
 }

 // ===== Pour chaque graphique:
 for (GnC = 1; GnC<=NbGn; GnC++) {
  // Nom, unit‚ et nø des canaux:
  strcpy(s0, "");
  if (NbGr[GnC-1]==1) {
   for (i = 0; i<Canx; i++) if (Gn[i]==GnC) Gr1 = i;
   if (Ge[Gr1]) sprintf(s0, "%sñ%c", Label[Gr1], CarCan(Ge[Gr1]-1));
   else sprintf(s0, "%s", Label[Gr1]);
  }
  if (textwidth(s0)>Gys[GnC] || NbGr[GnC-1]>1) {
   for (i = 0, strcpy(s0, ""), strcpy(s1, " ("); i<Canx; i++) {
    if (Gn[i]==GnC) {
     if (Ge[i]) sprintf(s2, " %cñ%c", CarCan(i), CarCan(Ge[i]-1));
     else sprintf(s2, " %c", CarCan(i));
     strcat(s0, s2);
     if (!FullScl || NbGr[GnC-1]==1) {
      for (j = 0, ok = 1; j<i; j++)
       if (Gn[j]==GnC && !strcmp(SUnit[i], SUnit[j])) ok = 0;
      if (ok) {
       sprintf(s2, "%s, ", SUnit[i]);
       strcat(s1, s2);
      }
     }
    }
   }
   if (!strcmp(&s1[strlen(s1)-3], "(, ")) strcpy(s1, "");
   else strcpy(&s1[strlen(s1)-2], ")");
   if (FullScl && NbGr[GnC-1]>1) strcpy(s1, " (% Full Scale)");
   strcat(s0, s1);
  }
  Pl(s0, 0., 1);
  // Echelle axe Y:
  if (FullScl) {
   if (NbGr[GnC-1]==1) {
    if (Polar) {
     Pl("0 ", 0.5, 0);
     sprintf(s0, "%lg ", Max(fabs(Amin[Gr1]), fabs(Amax[Gr1])));
     Pl(s0, 1., 0);
    } else {
     sprintf(s0, "%lg ", Amin[Gr1]);
     Pl(s0, 0., 0);
     sprintf(s0, "%lg ", Amax[Gr1]);
     Pl(s0, 1., 0);
    }
   } else {
    gt = Grid(1., 0);
    for (g = .0; g<=1.; g += gt) {
     Grille(g, 0., 0);
     sprintf(s0, "%lg ", 100*g);
     if (Polar) Pl(s0, g/2.+.5, 0);
     else Pl(s0, g, 0);
    }
   }
  } else {
   if (Polar) {
    for (g = 0.; g<=Max(fabs(Gmin[GnC-1]), fabs(Gmax[GnC-1])); g += Gpas[GnC-1]) {
     gt = g/Max(fabs(Gmin[GnC-1]), fabs(Gmax[GnC-1]));
     if (fabsl(g)<Gpas[GnC-1]/2.) g = 0.;
     if (g>0.) Grille(gt, 0., 0);
     sprintf(s0, "%lg ", g);  Pl(s0, gt/2.+.5, 0);
    }
   } else {
    for (g = Gmin[GnC-1]; g<=Gmax[GnC-1]; g += Gpas[GnC-1]) {
     gt = (g - Gmin[GnC-1])/Gscl[GnC-1];
     if (fabsl(g)<Gpas[GnC-1]/2.) g = 0.;
     if (g>Gmin[GnC-1] && g<Gmax[GnC-1]) Grille(gt, 0., 0);
     sprintf(s0, "%lg ", g);  Pl(s0, gt, 0);
    }
   }
  }
  // Ascenseur Y:
  if (!PEcran && NbGr[GnC-1]<11) {
   if (NbGr[GnC-1]<4) {
    Lift = 6;
   } else {
    if (NbGr[GnC-1]==4) Lift = 8;
    else Lift = NbGr[GnC-1];
   }
   Vide(Gx2 + 4, Gx2 + 3 + Lift, Gy1[GnC], Gy2[GnC], DARKGRAY);
   for (i = 0, j = 0; i<Canx; i++) {
    if (Gn[i]==GnC) {
     if (Amax[i]<=Dmax[i] && Amin[i]>=Dmin[i] && Dmax[i]!=Dmin[i])
      Vide(Gx2 + 4 + Lift*j/NbGr[GnC-1], Gx2 + 3 + Lift*(j + 1)/NbGr[GnC-1],
		 Gy1[GnC] - Gys[GnC]*(Amax[i] - Dmax[i])/(Dmax[i] -Dmin[i]),
		 Gy1[GnC] - Gys[GnC]*(Amin[i] - Dmax[i])/(Dmax[i] - Dmin[i]), Gc[i]);
     j++;
    }
   }
  }

  // ===== Echelle axe X:
  switch (Gv) {
  case -1:
   if (TimeMode) {
    if (TimeMode==3) Zpas = Grid((double)(Zmax - Zmin), 2);
    else Zpas = Grid((double)(Zmax - Zmin), 1);
    for (t = Zpas*ceil(Zmin/Zpas); t<=Zmax; t += Zpas) {
     gt = (t - Zmin)/Zscl;
     Grille(gt, 0., 1);
     if (Polar) {} else {
      sprintf(s0, "%lg", t);
      Pl(s0, gt, 2);
     }
    }
    switch (TimeMode) {
     case 1: strcpy(s0, "Raw Index");  break;
     case 2: strcpy(s0, "Data Index");  break;
     case 3: sprintf(s0, "Phase (%s)", Phase); break;
    }
    Pl(s0, 0., 3);
   } else {
    Zpas = TGrid(Zmin, Zmax);
    for (i = 0; i<20; i++) {
     gt = (TTick[i] - Zmin)/Zscl;
     if (TTick[i]<=Zmax && TTick[i]>=Zmin) Grille(gt, 0., 1);
     ct.sec = floor(TTick[i]);  ct.fsec = Frac(TTick[i])*MSEC;
     date = *gmtime(&ct.sec);
     if (Zpas<1.) {
      sprintf(s0, "%02d", date.tm_sec);
      sprintf(s1, ".%04d", ct.fsec/MSEC);
      strcat(s0, s1);
     } else {
      strftime(s0, 10, FTick, &date);
     }
     if (TTick[i]<=Zmax && TTick[i]>=Zmin) Pl(s0, gt, 2);
     strcpy(s0, "Time");
     if (TCenter) strcat(s0, " (centred)");
     Pl(s0, 0., 3);
    }
   }
   break;

  // ===== Graphe XY (canaux en fonction de Gv)
  default:
   Apas[Gv] = Grid(Ascl[Gv], 1);
   for (g = Apas[Gv]*ceil(Amin[Gv]/Apas[Gv]); g<=Amax[Gv]; g += Apas[Gv]) {
    gt = (g - Amin[Gv])/Ascl[Gv];
    if (fabs(g)<Apas[Gv]/2.) g = 0.;
    if (g>Amin[Gv] && g<Amax[Gv]) Grille(gt, 0., 1);
    if (Polar) {} else {
     sprintf(s0, "%lg", g);
     Pl(s0, gt, 2);
    }
   }
   if (Ge[Gv]) sprintf(s0, "%sñ%c", Label[Gv], CarCan(Ge[Gv]-1));
   else sprintf(s0, "%s", Label[Gv]);
   Pl(s0, 0., 3);
  }
  setcolor(LIGHTGRAY);
  setlinestyle(SOLID_LINE, 1, 1);
  if (Polar) circle(Gx1 + Gxs/2, Gy2[GnC] - Gys[GnC]/2, Gys[GnC]/2);
  else rectangle(Gx1, Gy1[GnC], Gx2, Gy2[GnC]);
 }

 // ===== Effacer entre les graphes:
 for (GnC = 1; GnC<NbGn; GnC++) {
  Vide(Gx1, Gx2, Gy2[GnC] + 1, Gy1[GnC+1] - 1, BLACK);
 }
 switch (DMode) {
  case 1:
   strcpy(s0, "OFFSET CORRECTION");
   if (NbGn==1 && NbGr[0]==1) {
    sprintf(s1, " (mean = %1.8g)", RLz[Gr1]);
    strcat(s0, s1);
   }
   break;
  case 2:
   if (DRef==-1) strcpy(s0, "TIME LINEAR CORRECTION");
   else sprintf(s0, "CH. %c LINEAR CORRECTION", CarCan(DRef));
   if (NbGn==1 && NbGr[0]==1) {
    sprintf(s1, " (y = %1.8g *x %+1.8g)", RLa[Gr1], RLb[Gr1]);
    strcat(s0, s1);
   }
   break;
  case 3:
   strcpy(s0, "CUMULATIVE SUM");
   break;
  case 4:
   if (Filtre==2) {
    strcpy(s0, "APPROXIMATE DERIVATIVE");
    if (TimeMode==0) {
     if (Acmin!=0.) strcpy(s1, " (per acqu. period)");
     else strcpy(s1, " (per second)");
     strcat(s0, s1);
    }
   } else {
    sprintf(s0, "DIFFERENCES ON %ld DATA", Filtre);
   }
   break;
  case 5:
   sprintf(s0, "AVERAGE FILTER ON %ld DATA", Filtre);
   break;
  case 6:
   sprintf(s0, "WAVE HARMONICS %ld CORRECTION", Filtre);
   break;
  default: strcpy(s0, "");
 }
 Pl(s0, 0., 6);
 setlinestyle(SOLID_LINE, 1, EpLigne);
 Aff_Souris(1);
}


//**************************************************************************
// Aff_Graphique: ouvre et lit le fichier tampon FF_Tmp0 et trace les
// courbes sur le graphe appropri‚, avec la couleur Gc[] et le style de
// ligne Gn[]. Appelle toutes les routines de mise … jour pour les calculs
// prochains. Le mode mdc permet:
//	0 = le trac‚ simple,
//	1 = le calcul des maxima et minima lors des corrections,
//	2 = l'‚criture d'un nouveau fichier tampon qui contient une formule,
//	3 = la cr‚ation d'un fichier graphique Postscript.
//**************************************************************************

void Aff_Graphique(char mdc)
{
 char *p, cc, md, s0[128];
 unsigned char nf, nfy;
 int i, j, k, di, j1, j2, tw;
 double ddn, xx1, xx2, yy1, yy2, zt0, zt1, xx, ff, t0, t1;
 struct c_tm { time_t sec;
	 unsigned int fsec; } ct;

 if (EcranGraph) {
  Aff_Souris(2);
  Efface_Menu(0);
  strcpy(s0, "Please Wait...");
  if (mdc==0) strcat(s0, " @Esc@ to stop.");
  Pg2(s0, -13, -14, 2);
  setviewport(Gx1, Gy1[0], Gx2, Gy2[0], 1);
 } else {
  if (mdc==2) cprintf("\r\n      Computing data...");
 }

 // ==== V‚rifie qu'il n'y a pas de canaux invisibles
 for (i = 0; i<Canx; i++) if (Gn[i] && Gt[i]==0 && Gm[i]==0) Gt[i] = 1;

 // ==== Boucle principale: lecture du fichier tampon
 X = 0l;  Y = 0l;
 nf = 0; NbSF = 0; nfy = 0;  DCh = 0;
 if ((f_tmp0 = fopen(FF_Tmp0, "rb")) == NULL) Sortie("Tmp0 ORB.");
 if (mdc==2) {
  if ((f_tmp1 = fopen(FF_Tmp1, "w+b")) == NULL) Sortie("Tmp1 OWB.");
 }
 do {
  if (X == 0l) Dte[1] = Tmin;
  else Dte[1] = Dte[0];
  fread(&ct.sec, 4, 1, f_tmp0);
  while (ct.sec==-1l) {
   if (!Append) {
    DCh += Cnx[nf];  Dte[1] = Tmin;  Y = 0l;
   }
   nf++;
   fread(&ct.sec, 4, 1, f_tmp0);
  }
  fread(&ct.fsec, 2, 1, f_tmp0);
  if (mdc==2) {
   fwrite(&ct.sec, 4, 1, f_tmp1);
   fwrite(&ct.fsec, 2, 1, f_tmp1);
  }
  Dte[0] = ct.sec + ct.fsec/MSEC;
  for (i = 0; i<Cnx[nf]; i++) {
   di = i + DCh;
   Data[1][di] = Data[0][di];
   fread(&ddn, 8, 1, f_tmp0);
   if (mdc!=2 || !RepCh || i!=Cnx[nf] - 1) {
    if (mdc==2) fwrite(&ddn, 8, 1, f_tmp1);
    Data[0][di] = ddn;
    if (X==0l) Data[1][di] = Data[0][di];
   }
  }
  for (i = 0; i<Cnx[nf]; i++) {
   di = i + DCh;
   if (mdc!=2 || !RepCh || i!=Cnx[nf] - 1) Eval_RL(di, Don, Data, Dte);
   if (X==0l) Don[1][di] = Don[0][di];
  }

  // ===== Formule (mode 2): calcul de Data[][] … partir de *Formula.
  if (mdc==2) {
   md = 0;  i = 0;  ddn = 0.;  k = Canx - 1;
   p = strchr(Formula, 'w');
   if (p) tw = IsCanal(p[1]) - 1;
   else tw = -1;
   do {
    cc = Formula[i];
    switch (cc) {
    case '+': md = 0; break;
    case '-': md = 1; break;
    case '.': md = 2; if (!i) ddn = 1.; break;
    case '/': md = 3; if (!i) ddn = 1.; break;
    case '\\': md = 4; if (!i) ddn = 1.; break;
    case 'v': md = 5; if (!i) ddn = 1.; break;
    case 'w': i++; break;
    case 'l': if (ddn>0.) ddn = log10(ddn); break;
    case 'e': ddn = exp(ddn); break;
    case 'c': ddn = cos(ddn); break;
    case 'r': ddn = rand()/(double)RAND_MAX; break;
    case 's': ddn = sin(ddn); break;
    case 't': ddn = tan(ddn); break;
    case '<':
     strcpy(s0, &Formula[i+1]);
     xx = atof(strtok(s0, ">"));
     i += strlen(s0) + 1;
    default:
     j = IsCanal(cc) - 1;
     if (cc=='*') { j1 = 0; j2 = k - 1; }
     else { j1 = j2 = j; }
     for (j = j1; j<=j2; j++) {
      if (j>=0) xx = Don[0][j];
      else if (cc=='0') xx = Dte[0];
      switch (md) {
      case 0: ddn += xx; break;
      case 1: ddn -= xx; break;
      case 2: ddn *= xx; break;
      case 3:
       if (xx!=0.) ddn /= xx;
       else ddn = NaN;
       break;
      case 4:
       if (ddn!=0.) ddn = xx/ddn;
       else ddn = NaN;
       break;
      case 5:
       if ((Dte[0] != Dte[1])&&(Bruit[j] != 0.)) {
	ddn *= (Don[0][j] - Don[1][j])/(Dte[0] - Dte[1]);
	ddn /= sqrt(Bruit[j]/(double)NbY);
       } else {
	ddn = 0.;
       }
       break;
      default:;
      }
     }
    }
    i++;
   }
   while (i<strlen(Formula));

   // ===== Mise … jour des variables du nouveau canal
   Data[1][k] = Data[0][k];
   if (md==5) {
    if (X==0l) {
     Data[0][k] = 0.;
    } else {
     if ((ddn*(Don[0][tw] - Don[1][tw]) < .1*sqrt(Bruit[k]/(double)NbY))
	  || tw==-1) Data[0][k] += ddn;
    }
   } else {
    Data[0][k] = ddn;
   }
   if (X==0l) {
    Data[1][k] = Data[0][k];
    Vmin[k] = Vmax[k] = Data[0][k];
    Bruit[k] = 0.;
    Resol[k] = MAXDOUBLE;
   }
   Vmin[k] = Min(Vmin[k], Data[0][k]);
   Vmax[k] = Max(Vmax[k], Data[0][k]);
   ddn = Data[0][k] - Data[1][k];
   Bruit[k] += ddn*ddn;
   if (ddn!=0.) Resol[k] = Min(Resol[k], fabs(ddn));
   if (TimeMode==3) Calc_Harmonic(k, 1);
   fwrite(&Data[0][Canx-1], 8, 1, f_tmp1);
   **Don = **Data;
  }

  // ===== Si le temps de la donn‚e est dans la fenˆtre visualis‚e...
  if ((Dte[1]>=Zmin && Dte[1]<=Zmax) || (Dte[0]>=Zmin && Dte[0]<=Zmax)) {
   if (!Append) k = Cnx[nf];
   else k = Canx;
   for (i = 0; i<k; i++) {
    di = i + DCh;
    if (Data[0][di] != NaN) {
     if (Y==0l && (!Append || nfy==0)) MAZ_RL(di);
     if (Gv == -1) {
      MAJ_RL(di, Dte[0] - Tmin, Data[0][di]);
     } else {
      if (Data[0][Gv] != NaN) MAJ_RL(di, Data[0][Gv], Data[0][di]);
     }
     //if (Gm[di] == 5) Don[1][di] = 0.;
     if (Data[1][di] == NaN) Don[1][di] = Don[0][di];
     if (Don[0][di] != NaN) {
      if (Y==0l && (!Append || nfy==0)) Emax[di] = Emin[di] = Don[0][di];
      Emax[di] = Max(Emax[di], Don[0][di]);
      Emin[di] = Min(Emin[di], Don[0][di]);
      if (Ge[di]) {
       Emax[di] = Max(Emax[di], Don[0][di] + fabs(Don[0][Ge[di]-1]));
       Emin[di] = Min(Emin[di], Don[0][di] - fabs(Don[0][Ge[di]-1]));
      }
     }
    }
   }
   if (mdc==0 || mdc==3) for (i = 0; i<Cnx[nf]; i++) {
    di = i + DCh;
    if (Data[0][di]!=NaN && (Gv==-1 || Data[0][Gv]!=NaN)) {
     if (Gn[di]) {
      GnC = Gn[di];
      // mode polaire
      if (Polar) {
       switch (Gv) {
       case -1:
	if (!HiCum) {
	 if (Period>0.) {
	  t1 = Dte[1]/Period;
	  t0 = Dte[0]/Period;
	 } else {
	  t1 = Phase_Doodson(Dte[1], DD);
	  t0 = Phase_Doodson(Dte[0], DD);
	 }
	 t1 *= 2.*M_PI;
	 t0 *= 2.*M_PI;
	} else {
	 t1 = Dte[1]*M_PI/180.;
	 t0 = Dte[0]*M_PI/180.;
	}
	xx1 = cos(t1);
	xx2 = cos(t0);
	yy1 = sin(t1);
	yy2 = sin(t0);
	if (Data[1][di] == NaN) xx1 = xx2;
	break;
       default:
	xx1 = cos(Don[1][Gv]*M_PI/180.);
	xx2 = cos(Don[0][Gv]*M_PI/180.);
	yy1 = sin(Don[1][Gv]*M_PI/180.);
	yy2 = sin(Don[0][Gv]*M_PI/180.);
       }
       if (FullScl) ff = 2.*Max(fabs(Amin[di]),fabs(Amax[di]));
       else ff = 2.*Max(fabs(Gmin[GnC-1]),fabs(Gmax[GnC-1]));
       xx1 = .5 + xx1*Don[1][di]*(Gys[GnC-1]/(double)Gxs)/ff;
       xx2 = .5 + xx2*Don[0][di]*(Gys[GnC-1]/(double)Gxs)/ff;
       yy1 = .5 + yy1*Don[1][di]/ff;
       yy2 = .5 + yy2*Don[0][di]/ff;

       // ===== Trac‚ des barres d'erreur
       if (Ge[di]) {
	j = Ge[di]-1;
	Ligne(xx2, yy2+Don[0][j]/Gscl[GnC-1],xx2, yy2-Don[0][j]/Gscl[GnC-1], Gt[j], Gm[j], Gc[j], 1);
       }
       if (Gv!=-1) if (Ge[Gv]) {
	j = Ge[Gv]-1;
	Ligne(xx2+Don[0][j]/Ascl[Gv], yy2, xx2-Don[0][j]/Ascl[Gv], yy2, Gt[j], Gm[j], Gc[j], 1);
       }
      } else {
       // mode cart‚sien
       switch (Gv) {
       case -1:
	xx1 = (Dte[1] - Zmin)/Zscl;
	xx2 = (Dte[0] - Zmin)/Zscl;
	if (Data[1][di] == NaN) xx1 = xx2;
	break;
       default:
	xx1 = (Don[1][Gv] - Amin[Gv])/Ascl[Gv];
	xx2 = (Don[0][Gv] - Amin[Gv])/Ascl[Gv];
       }
       switch (FullScl) {
       case 0:
	yy1 = (Don[1][di] - Gmin[GnC-1])/Gscl[GnC-1];
	yy2 = (Don[0][di] - Gmin[GnC-1])/Gscl[GnC-1];
	break;
       case 1:
	yy1 = (Don[1][di] - Amin[di])/Ascl[di];
	yy2 = (Don[0][di] - Amin[di])/Ascl[di];
	break;
       }

       // ===== Trac‚ des barres d'erreur
       if (Ge[di]) {
	j = Ge[di]-1;
	Ligne(xx2, yy2+Don[0][j]/Gscl[GnC-1],xx2, yy2-Don[0][j]/Gscl[GnC-1], Gt[j], Gm[j], Gc[j], 1);
       }
       if (Gv!=-1) if (Ge[Gv]) {
	j = Ge[Gv]-1;
	Ligne(xx2+Don[0][j]/Ascl[Gv], yy2, xx2-Don[0][j]/Ascl[Gv], yy2, Gt[j], Gm[j], Gc[j], 1);
       }

       //if (Gm[di] == 5) yy1 = 0.;
      }
      // ===== Si la p‚riode d'acquisition est d‚pass‚e, pas de ligne
      if ((fabs(Dte[0] - Dte[1])<=1.5*Decim*Acqu[nf] && Gt[di])
	  || Gm[di]==5 || Acqu[nf]==0.) Ligne(xx1, yy1, xx2, yy2, Gt[di], Gm[di], Gc[di], 0);
      else Ligne(xx2, yy2, xx2, yy2, Gt[di], Gm[di], Gc[di], 0);

      // ===== En mode XY, indication de la derniŠre donn‚e par un cercle
      if ((Gv!=-1 || Polar) && (Dte[0]-Zmax)>=0.) Ligne(xx2, yy2, xx2, yy2, 0, 3, WHITE, 0);

      if (OSView && !DMode && !FullScl && NbGr[GnC-1]==1) {
       for (k = -20; k<=20; k++) {
	if (Calib>0 && (Dte[0]<CBTime || Dte[0]>=CETime)) {
	 Lire_Calib(&Dte[0], 0);
	}
	ddn = (k + Offset[GnC-1])*(double)MaxVal;
	if (Calib>0) Calib_Data(&ddn, GnC - 1);
	yy2 = (ddn - Gmin[GnC-1])/Gscl[GnC-1];
	if (yy2>0 && yy2<1) Ligne(xx2, yy2, xx2, yy2, 1, 0, LIGHTGRAY, 0);
       }
      }

      // ===== Comptage des passages par z‚ro (pour calculer la p‚riode)
      if ((DMode==1 || DMode==2 || DMode==4) && NbGn==1 && NbGr[0]==1) {
       Ze = (Don[0][di] > 0.);
       if (Y==0l) { Zn = Ze;  Zc = 0;  Zs = 0.; }
       if ((Ze + Zn)%2) {
	Zc++;
	if (Zc==1) {
	 zt0 = Dte[1] - (Dte[0] - Dte[1])*Don[1][di]/(Don[0][di] - Don[1][di]);
	} else {
	 if (Zc%2) {
	  zt1 = Dte[1] - (Dte[0] - Dte[1])*Don[1][di]/(Don[0][di] - Don[1][di]);
	  Zt = (zt1 - zt0)/floor((Zc - 1)/2.);
	  if (Zc>3) Zs += pow(Zt - Zp, 2.);
	  Zp = Zt;
	 }
	}
	Zn = Ze;
       }
      }
     }
    }
   }
   if (nf!=nfy) nfy++;
   Y++;
  }
  if (mdc!=2 && mdc!=3) {
   if (kbhit()) if (getch()==ESC) break;
   if (NTOver && Dte[0]>Zmax && Append) break; // on arrˆte de tracer...
  }
  if (!EcranGraph) Working(X/(double)NbY, Y/(double)NbY);
  X++;
 }
 while (X<NbY);
 fclose(f_tmp0);  fclose(f_tmp1);
 if (!EcranGraph) Working(1., 1.);
 if (DMode==0) {
  DRef = Gv;
  for (i = 0; i<Canx; i++) Calc_RL(i);
 }
 if (mdc==2) {
  i = Canx - 1;
  Dmax[i] = Vmax[i];// = ceil(Vmax[i]/Dpas[i])*Dpas[i];
  Dmin[i] = Vmin[i];// = floor(Vmin[i]/Dpas[i])*Dpas[i];
 }
 if (EcranGraph) {
  if (!DMode && RLView) Trace_RL(RLView);
  Aff_Souris(1);
  setviewport(0, 0, Mx, My, 0);
 }
 while (kbhit()) getch(); // vide le tampon clavier...
}


//**************************************************************************
// Aff_Book: trace des zones hachur‚es sur chaque graphe aux dates indiqu‚es
// soit dans un fichier Book *FC_Book, soit dans le fichier de calibration
// *FC_Cal sur les lignes commen‡ant par "# BOOK:".
//**************************************************************************

void Aff_Book(void)
{
 FILE *f_book;
 char s0[128], s1[128], ok;
 int bk;
 double z1, z2;

 if (Book) {
  if ((f_book = fopen(FC_Book, "rt")) == NULL) { Book = 0;  return; }
 } else {
  if ((f_book = fopen(FC_Cal, "rt")) == NULL) return;
 }
 do {
  ok = 0; bk = 0;
  if (fgets(Str, 80, f_book) == NULL) break;
  if (strstr(Str, "# BOOK:")) {
   sscanf(&Str[7], "%s %s %d", s0, s1, &bk);
   ok = 1;
  } else {
   if (Book) {
    sscanf(Str, "%s %s %d", s0, s1, &bk);
    ok = 1;
   }
  }
  if (ok) {
   z1 = Decode_Time(s0);
   z2 = Decode_Time(s1);
   ok = 0;
   if (z1>=Zmin && z1<=Zmax) { z2 = Min(z2, Zmax); ok = 1; }
   if (z2>=Zmin && z2<=Zmax) { z1 = Max(z1, Zmin); ok = 1; }
   if (ok) Grille((z1 - Zmin)/Zscl, (z2 - Zmin)/Zscl, 2 + bk);
  }
 } while (1);
 fclose(f_book);
}



//**************************************************************************
// FUD: flŠches "Up" et "Down" = d‚filement des graphes suivant l'axe Y;
// (Amin[]) et (Amax[]) modifi‚s de (Ascl[])/2 dans les limites de (Dmin[])
// et (Dmax[]). Si au moins 1 graphe est modifi‚, renvoie ok = 1.
//**************************************************************************

int FUD(int ud)
{
 int i, ok;

 for (i = 0; i<Canx; i++) {
  if (Gn[i]) switch (ud) {
  case 0: // [flŠche bas]
   if (Amin[i]>Dmin[i]) {
    Amin[i] = Max(Dmin[i], Amin[i] - Ascl[i]/2);
    Amax[i] = Min(Dmax[i], Amin[i] + Ascl[i]);
    ok = 1;
   }
   break;
  case 1: // [flŠche haut]
   if (Amax[i]<Dmax[i]) {
    Amax[i] = Min(Dmax[i], Amax[i] + Ascl[i]/2);
    Amin[i] = Max(Dmin[i], Amax[i] - Ascl[i]);
    ok = 1;
   }
  }
 }
 return (ok);
}


//**************************************************************************
// FF5: touche "F5" = ajustement des ‚chelles en fonction des donn‚es vues.
//**************************************************************************

void FF5(void)
{
 int i;

 for (i = 0; i<Canx; i++) {
/*
  Apas[i] = Grid(NbGn*(Emax[i] -Emin[i]), 0);          // pas canal [i]
  Amax[i] = ceil(Emax[i]/Apas[i])*Apas[i];          // maximum
  Amin[i] = floor(Emin[i]/Apas[i])*Apas[i];         // minimum
*/
  Amax[i] = Emax[i];
  Amin[i] = Emin[i];
  Ascl[i] = Amax[i] - Amin[i];                      // ‚chelle
  if (Ascl[i]==0.) Ascl[i] = 1.;
  Apas[i] = Grid(Ascl[i], 0);
 }
}


//**************************************************************************
// FF6: touche "F6" ou "CtrlF6" = modifie le type de trac‚ Gt[] et Gm[]
// pour tous les canaux affich‚s.
//**************************************************************************

int FF6(char pt, char pm)
{
 int i, j;

 for (i = 0, j = -1; i<Canx; i++) {
  if (Gn[i]) {
   if (j==-1) {
    j = i;
    Gt[i] = (Gt[i] + pt + NBGT)%NBGT;
    Gm[i] = (Gm[i] + pm + NBGM)%NBGM;
   } else {
    Gt[i] = Gt[j];
    Gm[i] = Gm[j];
   }
  }
 }
 Annule_Styles(ChDraw);
 return(j);
}


//**************************************************************************
// Trace_RL: Affiche sur les graphes en cours suivant rl:
//	1 = la valeur moyenne des donn‚es (droite ou point en XY);
//	2 = la droite de r‚gression lin‚aire;
//	>3 = harmoniques principales calcul‚es (TimeMode=3).
// Note: le trac‚ ‚tant fait en mode XOR, il suffit d'appeler 2 fois cette
// routine pour effacer.
//**************************************************************************

void Trace_RL(int rl)
{
 char s0[64], s1[64];
 int i, gt = 1, gm = 0;
 double xrl1 = 0., xrl2 = 1., yrl1, yrl2, ang, har;

 Vide(Mx - 12, Mx - 2, Gy1[0], Gy2[0], BLACK);
 setwritemode(1);
 setlinestyle(SOLID_LINE, 1, 1);
 //setlinestyle(USERBIT_LINE, 0xAAAA, 1);
 setviewport(Gx1, Gy1[0], Gx2, Gy2[0], 1);
 for (i = 0; i<Canx; i++) {
  GnC = Gn[i];
  switch (rl) {
  case 1:
   sprintf(s0, "Mean Value");
   if (Gv!=-1) {
    xrl1 = xrl2 = (RLz[Gv] - Amin[Gv])/Ascl[Gv];
    gt = 0;  gm = 2;
    sprintf(s1, " (x = %1.8g, y = %1.8g)", RLz[Gv], RLz[Gr1]);
   } else {
    sprintf(s1, " (y = %1.8g)", RLz[Gr1]);
   }
   yrl1 = yrl2 = RLz[i];
   break;
  case 2:
   sprintf(s0, "Linear Regression");
   sprintf(s1, " (y = %1.8g *x %+1.8g)", RLa[Gr1], RLb[Gr1]);
   if (Gv!=-1) {
    yrl1 = RLa[i]*Amin[Gv] + RLb[i];
    yrl2 = yrl1 + RLa[i]*Ascl[Gv];
   } else {
    yrl1 = RLb[i] + RLa[i]*(Zmin - Tmin);
    yrl2 = yrl1 + RLa[i]*Zscl;
   }
   break;
  default:
   sprintf(s0, "%s Harmonic #%d", Doodson, rl-2);
   sprintf(s1, " (pha = %+06.2fø amp = %1.8g)", HarmS[rl-3][Gr1], HarmC[rl-3][Gr1]);
   har = rl-2.;
   for (ang = Zmin; ang<=Zmax; ang+=Zscl/50.) {
    if (ang!=Zmin) {
     xrl1 = xrl2;
     yrl1 = yrl2;
    }
    xrl2 = (ang-Zmin)/Zscl;
    yrl2 = HarmC[rl-3][i]*cos(2.*M_PI*(ang*har - HarmS[rl-3][i])/360.);
    if (GnC && ang!=Zmin) {
     if (FullScl) Ligne(xrl1, yrl1/Ascl[i] + .5,
		      xrl2, yrl2/Ascl[i] + .5, gt, gm, WHITE, 0);
     else Ligne(xrl1, yrl1/Gscl[GnC-1] + .5,
	      xrl2, yrl2/Gscl[GnC-1] + .5, gt, gm, WHITE, 0);
    }
   }

   /*
   if (Gv!=-1) {
    yrl1 = yrl2 = fmod(HarmS[rl-3][Gv]+360., 360.);
    gt = 0;  gm = 2;
   } else {
    yrl1 = Gmin[GnC-1];  yrl2 = Gmax[GnC-1];
   }
   xrl1 = xrl2 = (fmod(HarmS[rl-3][i]+360., 360.) - Zmin)/Zscl; */
   break;
  }
  if (GnC && rl<3) {
   if (FullScl) Ligne(xrl1, (yrl1 - Amin[i])/Ascl[i],
		      xrl2, (yrl2 - Amin[i])/Ascl[i], gt, gm, WHITE, 0);
   else Ligne(xrl1, (yrl1 - Gmin[GnC-1])/Gscl[GnC-1],
	      xrl2, (yrl2 - Gmin[GnC-1])/Gscl[GnC-1], gt, gm, WHITE, 0);
  }
 }
 setwritemode(0);
 setlinestyle(SOLID_LINE, 1, EpLigne);
 setviewport(0, 0, Mx, My, 0);
 if (NbGn==1 && NbGr[0]==1) strcat(s0, s1);
 Pl(s0, 0., 6);
}


//**************************************************************************
// CalcZ: Calcule les paramŠtres de l'‚chelle temporelle du graphe: Zmin,
// Zmax et Zscl aprŠs un zoom.
//**************************************************************************

void CalcZ(void)
{
 double t2;

 if (Zoom==1.) { Zmin = Tmin; Zmax = Tmax; }
 Zscl = Zmax - Zmin;
 if (TimeMode!=3) {
  if (Zmax>Tmax) { Zmax = Tmax;  Zmin = Max(Zmax - Zscl, Tmin); }
  if (Zmin<Tmin) { Zmin = Tmin;  Zmax = Min(Zmin + Zscl, Tmax); }
  if (Zmin>Zmax) { t2 = Zmin;  Zmin = Zmax;  Zmax = t2; }
 }
 Zscl = Zmax - Zmin;  if (Zscl==0.) Zscl = Acmax;
}


//**************************************************************************
// Decode_ChDraw: d‚code la chaŒne graphique ChDraw pour mettre … jour les
// variables suivantes :
//	NbGn = nombre de graphe
//	NbGr[] = nombre de canaux par graphe
//	Gs[] = taille du graphe (nombre entier)
//	NbGs = taille totale des graphes
//	Gn[] = indice du graphe du canal (0 si non affich‚)
//	Gt[] = style de trac‚ du canal
//	Gm[] = style de marqueur du canal
//	Gc[] = couleur du canal
//	Ge[] = indice du canal erreur associ‚ (0 si inexistant)
//	Gv = indice du canal en X (-1 si temporel)
//
//	Note: AllCh = "1234" et AllChN = "1,2,3,4," (si 4 canaux)
//**************************************************************************

void Decode_ChDraw(void)
{
 char *p, s0[2], gvc = 0, gtc = 0, gmc = 0;
 int i, j, k, n, gcc = 0, gec = 0;

 Polar = 0;
 // ===== Remplace l'‚toile par la liste de tous les canaux
 p = strchr(ChDraw, '*');
 if (p) strcpy(p, AllCh);

 // ===== La virgule seule s‚pare tous les canaux
 if (!strcmp(ChDraw, ",")) {
  for (i = 0; i<Canx; i++) if (Gn[i]) {
   sprintf(s0, "%c", CarCan(i));
   strcat(ChDraw, s0);
  }
 }
 NbGn = 1;
 for (i = 0; i<Canx; i++) { Gn[i] = NbGr[i] = 0;  Gs[i] = 1;  Ge[i] = 0; }

 // ===== Boucle sur la chaŒne ChDraw
 for (i = 0, Gv = -1; i<strlen(ChDraw); i++) {
  j = IsCanal(ChDraw[i]);
  if (j>0 && j<=Canx) {
   if (gec) {
    Ge[gec-1] = j;
    gec = 0;
   } else {
    if (gvc) {
     Gv = j - 1;
     gvc = 0;
    } else {
     if (ChDraw[i+1]!='-') {
      Gn[j-1] = NbGn;
      NbGr[NbGn-1]++;
     }
    }
   }
   if (gtc) { Gt[j-1] = gtc - 1;  Gm[j-1] = 0; }
   if (gmc) { Gm[j-1] = gmc - 1;  if (!gtc) Gt[j-1] = 0; }
   if (gcc) Gc[j-1] = gcc;
  }
  if (i && ChDraw[i]==',' && ChDraw[i+1]!=NULL) {
   if (ChDraw[i+1]==',') Gs[NbGn-1]++;
   else NbGn++;
  }

  switch (ChDraw[i]) {

   // ===== Groupement de canaux
   case '-': if ((j = IsCanal(ChDraw[i-1]))==0) j = 1;
	     if ((n = IsCanal(ChDraw[i+1]))==0) n = Canx;
	     for (k = j; k<=n; k++) {
	      Gn[k-1] = NbGn;
	      NbGr[NbGn-1]++;
	      if (gtc) Gt[j-1] = gtc - 1;
	     }
	     i++;  break;

   // ===== Barre d'erreur
   case 'e': j = IsCanal(ChDraw[i-1]);
	     if (j>0 && j<=Canx) gec = j;  break;

   // ===== Graphes XY: le prochain canal sera la r‚f‚rence X
   case 'v': gvc = 1;  break;

   // ===== Graphes polaire: le prochain canal sera la phase
   case 'p': gvc = 1;  Polar = 1;  break;

   // ===== D‚codage des styles de trac‚ (valable pour les prochains canaux)
   case 'n': gtc = 1;  gmc = 1;  break;
   case '/': gtc = 2;  break;
   case ':': gtc = 3;  break;
   case '.': gmc = 2;  break;
   case 'x': gmc = 3;  break;
   case 'o': gmc = 4;  break;
   case 's': gmc = 5;  break;
   case '|': gmc = 6;  break;
   case 'r': gcc = LIGHTRED;  break;
   case 'y': gcc = YELLOW;  break;
   case 'g': gcc = LIGHTGREEN;  break;
   case 'c': gcc = LIGHTCYAN;  break;
   case 'b': gcc = LIGHTBLUE;  break;
   case 'm': gcc = LIGHTMAGENTA;  break;
  }
 }

 if (ChDraw[0]==',') {
  for (i = 0, NbGn = 0; i<Canx; i++) if (Gn[i]) {
   Gn[i] = ++NbGn;
   NbGr[NbGn-1] = 1;
   Gs[i] = 1;
  }
 }
 for (i = 0, NbGs = 0; i<NbGn; i++) { Jg[i] = 0;  NbGs += Gs[i]; }
}


//**************************************************************************
// Faire_ChDraw: construit la chaŒne graphique (ChDraw) … partir des
// variables Gn[] mises … jour depuis la fenˆtre Info.
//**************************************************************************

void Faire_ChDraw(void)
{
 char s0[2];
 int i;

 strcpy(ChDraw, "");
 for (i = 0; i<Canx; i++)
  if (Gn[i]) {
   if (i>0) if (Gn[i]!=Gn[i-1]) strcat(ChDraw, ",");
   sprintf(s0, "%c", CarCan(i));
   strcat(ChDraw, s0);
  }
 if (!strlen(ChDraw)) strcpy(ChDraw, AllCh);
}



//**************************************************************************
// Faire_Label: v‚rifie la validit‚ des variables SName[] et SUnit[] en
// rempla‡ant les ‚ventuels espaces par des undescores, puis construit la
// chaŒne Label[] pour tous les canaux.
//**************************************************************************

void Faire_Label(void)
{
 int i, j;
 char s0[25];

 for (i = 0; i<Canx; i++) {
  for (j = 0; j<strlen(SName[i]); j++) if (SName[i][j]==' ') SName[i][j] = '_';
  for (j = 0; j<strlen(SUnit[i]); j++) if (SUnit[i][j]==' ') SUnit[i][j] = '_';
  strcpy(Label[i], SName[i]);
  if (strlen(SUnit[i])) {
   sprintf(s0, " (%s)", SUnit[i]);
   strcat(Label[i], s0);
  }
  sprintf(s0, " %c", CarCan(i));
  strcat(Label[i], s0);
 }
}


//**************************************************************************
// Init_Boite: d‚finit (Xff) et (Yff) en fonction de (PEcran) et trace
// l'environnement graphique (boites externes).
//**************************************************************************

void Init_Boite(void)
{
 if (PEcran) {
  Yff = 0;
  setcolor(LIGHTGRAY);
  line(0, My - 12, Mx, My - 12);
 } else {
  Xff = 175;  Yff = 60;  // position des boŒtes sup‚rieures
  Vide(0, Xff - 2, 0, Yff - 2, WHITE);
  settextstyle(1, 0, 3);
  setcolor(BLACK);  Pg(Titre0, 7, -2);
  setcolor(CYAN);  Pg(Titre0, 6, -3);
  setcolor(WHITE);
  rectangle(Xff, 0, Mx, Yff - 2);
  rectangle(0, Yff, Mx, My - 12);
  setcolor(LIGHTGRAY);
  rectangle(Xff + 1, 1, Mx - 1, Yff - 3);
  rectangle(1, Yff + 1, Mx - 1, My - 13);
 }
}


//**************************************************************************
// Init_Texte: affiche le texte dans la boite sup‚rieure en fonction de tous
// les paramŠtres du (des) fichier(s) de donn‚es.
//**************************************************************************

void Init_Texte(void)
{
 char s0[80], s1[80], s2[80];

 // coordonn‚es de la fenˆtre graphique (d‚finie depuis les bords)
 Gx1 = 75;
 Gy1[0] = Yff + 13;
 if (strlen(FTitre)) Gy1[0] += 16;
 Gx2 = Mx - 28;
 Gy2[0] = My - 47;
 Gxs = Gx2 - Gx1;
 Gys[0] = Gy2[0] - Gy1[0];

 if (PEcran || !EcranGraph) return;

 Vide(Xff + 3, Mx - 200, 2, Yff - 4, BLACK);
 settextstyle(2, 0, 0);
 setcolor(WHITE);
 if (Com) {
  sprintf(s0, "æDAS nø%03d alive on Com%d : %ld data", UDAS, Com, NbY);
 } else {
  sprintf(s0, "%d file", NbFiles); if (NbFiles>1) strcat(s0, "s");
  if (Append && NbFiles>1) strcat(s0, " appended");
  sprintf(s1, " for %ld bytes", TF_Tot); strcat(s0, s1);
  if (Sort) strcat(s0, " þ Sorted");
 }
 Pg(s0, Xff + 6, 4);
 sprintf(s0, "Fmt %s", ImpFmt);
 if (Bin) {
  strcat(s0, " BIN");
 } else {
  switch (AutoFmt) {
   case 1: strcat(s0, " HEADER");  break;
   case 2: strcat(s0, " LTERM");  break;
   case 3: strcat(s0, " PC-DAS");  break;
   case 4: strcat(s0, " MDAS");  break;
  }
 }
 if (strlen(AllTime)) { sprintf(s1, " (%s)", AllTime);  strcat(s0, s1); }
 if (DAS) { sprintf(s1, " þ DAS (Max %ld)", MDiff); strcat(s0, s1); }
 else strcat(s0, " þ Not DAS");
 Pg(s0, Xff + 6, 16);
 sprintf(s0, "%d chan ", Canx);
 if (TimeMode) {
  switch (TimeMode) {
   case 1: sprintf(s1, "þ No Time (raw)");  break;
   case 2: sprintf(s1, "þ No Time (i = %lg)", Acmin);  break;
   case 3: sprintf(s1, "þ %g %s (Phase)", Acmin, Phase);  break;
  }
  if (iT!=0.) sprintf(s1, "þ > %lg sec", iT);
  strcat(s0, s1);
 } else {
  strcat(s0, "þ ");
  if (AcPM) strcat(s0, "ñ ");
  if (Acmin) sprintf(s1, "%lg sec", Acmin);
  else strcpy(s1, "? sec");
  if (NbFiles>1 && Acmin!=Acmax) {
   sprintf(s2, " to %lg sec", Acmax);
   strcat(s1, s2);
  }
  strcat(s0, s1);
 }
 if (Decim>1) { sprintf(s1, " (1:%ld)", Decim); strcat(s0, s1); }
 if (ExData) { sprintf(s1, " (extracted %ld)", ExData); strcat(s0, s1); }
 Pg(s0, Xff + 6, 28);
 if (Calib) {
  if (strlen(FC_Cal)>12) sprintf(s1, "...%s", &FC_Cal[strlen(FC_Cal)-12]);
  else strcpy(s1, FC_Cal);
 }
 switch (Calib) {
  case 3: case 4: sprintf(s0, "Calib '%s'", s1); break;
  case 1: sprintf(s0, "Calib in Hz '%s'", s1); break;
  case 2: sprintf(s0, "Calib in V '%s'", s1); break;
  case -1: sprintf(s0, "Named by '%s'", s1); break;
  default: strcpy(s0, "Not Cal.");
 }
 if (DfN) {
  sprintf(s1, " þ Filtering");
  strcat(s0, s1);
 }
 if (RNaN) {
  sprintf(s1, " þ NaN");
  strcat(s0, s1);
 }
 if (Spike!=0.) {
  sprintf(s1, " þ Spike (%lg)", Spike);
  strcat(s0, s1);
 }
 if (HiCum && TimeMode==3) {
  sprintf(s1, " þ HiCum %s", Doodson);
  strcat(s0, s1);
 }
 Pg(s0, Xff + 6, 40);
 Aff_Noms(0);
}


//**************************************************************************
// Aff_Pos: affiche soit la valeur des donn‚es … la position de la souris
// sur le graphe xs et ys, soit l'heure, soit une l‚gende. Le format
// d'affichage et la position d‚pendent du type de graphe et de PEcran.
//**************************************************************************

void Aff_Pos(int xs, int ys)
{
 char s0[40], ss0[50], ss1[50], ss2[50], ip = 0;
 int i;
 double g, t;
 struct tm date;
 struct c_tm { time_t sec;
	 unsigned int fsec; } ct;

 settextstyle(2, 0, 0);
 if (!IsGraph(xs, ys) || Info || !Souris) {
  ip = 1;
  setcolor(RED);
  date = *gmtime(&Horloge);
  strcpy(ss0, Titre1);
  strftime(ss1, 40, " %a %d %b %Y  %X", &date);
 } else {
  setcolor(BLACK);
  t = Zscl*((xs - Gx1)/(double)Gxs) + Zmin;
  if (Gv!=-1) {
   g = Ascl[Gv]*(xs - Gx1)/Gxs;
   if (Resol[Gv]==ceil(Resol[Gv])) g = Round(g);
   g += Amin[Gv];
   sprintf(ss0, "x: %1.8g", g);
  } else
  if (TimeMode) sprintf(ss0, "x: %lg", t);
  else {
   if (Acmin>=1. && !AcPM) t = Acmin*Round((t - Tmin)/Acmin) + Tmin;  // arrondi suivant Acqu
   ct.sec = floor(t);
   date = *gmtime(&ct.sec); ct.fsec = Frac(t)*MSEC;
   strftime(ss0, 40, "%d %b %Y (%j) %X", &date);
   if (Frac(Acmin)!=0.) { sprintf(ss2, ".%04d", ct.fsec);  strcat(ss0, ss2); }
  }
  for (GnC = 1; GnC<=NbGn; GnC++) {
   if (IsDedans(xs, ys, Gx1, Gx2, Gy1[GnC], Gy2[GnC])) {
    if (FullScl && NbGr[GnC-1]>1)
     sprintf(ss1, "y%c: %2.1f %%", CarCan(GnC - 1), 100.*(Gy2[GnC] - ys)/Gys[GnC]);
    else {
     if (FullScl) {
      for (i = 0; i<Canx; i++) if (Gn[i]==GnC) Gr1 = i;
      g = Ascl[Gr1]*(Gy2[GnC] - ys)/Gys[GnC] + Amin[Gr1];
      if (!DMode) g = ResolMin*Round((g - Amin[Gr1])/ResolMin) + Amin[Gr1];
     } else {
      g = Gscl[GnC-1]*(Gy2[GnC] - ys)/Gys[GnC] + Gmin[GnC-1];
      if (!DMode) g = ResolMin*Round((g - Gmin[GnC-1])/ResolMin) + Gmin[GnC-1];
     }
     sprintf(ss1, "y%c: %1.8g", CarCan(GnC - 1), g);
    }
    break;
   } else strcpy(ss1, "");
  }
 }
 if (PEcran) {
  Efface_Menu(0);
  //Vide(100, 640, My - 10, My, BLACK);
  if (ip) {
   settextstyle(2, 0, 0);  strcpy(s0, "");  Pg(s0, -13, -14);
   for (i = 0; i<Canx; i++) {
    if (Gn[i]) {
     sprintf(s0, " %c%c %s ", DStyle[Gt[i]], DMark[Gm[i]], Label[i]);
     setcolor(Gc[i]);  Pg(s0, -11, My - 11);
    }
   }
  } else {
   setcolor(LIGHTCYAN);  Pg(Titre0, -13, My - 11);
   setcolor(WHITE); Pg(ss0, 80, -11);
   Pg(" ", -11, -11);  Pg(ss1, -11, -11);
  }
 } else {
  Vide(0, Xff - 2, 28, 58, WHITE);
  Pg(ss0, 6, 28);  Pg(ss1, 6, 40);
 }
}


//**************************************************************************
// IsGraph: teste si la position graphique (ipx, ipy) se trouve dans
// l'espace des graphiques.
//**************************************************************************

int IsGraph(int ipx, int ipy)
{
 return (IsDedans(ipx, ipy, Gx1, Gx2, Gy1[0], Gy2[0]));
}


//**************************************************************************
// Aff_Noms: affiche les l‚gendes de 4 canaux dans la boite sup‚rieure.
//**************************************************************************

void Aff_Noms(int cn)
{
 char s0[40], s1[40];
 int i, j, k, ok;

 if (PEcran) return;
 Vide(Mx - 200, Mx - 2, 2, Yff - 4, BLACK);
 settextstyle(2, 0, 0);
 for (i = 0; i<Canx && i<4; i++) {
  j = (cn + i)%Canx;
  sprintf(s0, " %s ", Label[j]);
  for (k = 0, ok = 0; k<Canx; k++) if (Ge[k]==j+1) ok = 1;
  if (Gn[j] || ok) sprintf(s1, "%c%c ", DStyle[Gt[j]], DMark[Gm[j]]);
  else strcpy(s1, "   ");
  strcat(s0, s1);
  setcolor(Gc[j]);
  Pg(s0, -14, 12*i + 4);
 }
}


//**************************************************************************
// Aff_Info: suivant la valeur de md:
//	1 = pr‚pare la fenˆtre d'information sur les canaux, sauve l'‚cran
//	    en m‚moire, appelle Refresh_Info();
// 	0 = restaure l'‚cran depuis la m‚moire si SSOK==1.
//**************************************************************************

void Aff_Info(char md)
{
 Aff_Souris(2);
 switch (md) {
  case 1:
   SSOK = save_screen(Screen);
   Info = 1;
   Refresh_Info();

   break;
  case 0:
   Aff_Souris(5);
   if (SSOK) restore_screen(Screen);
   else Vide(Xi1 - 1, Xi2 + 1, Yi1 - 1, Yi2 + 1, BLACK);
   Info = 0;
   Aff_Menu(0);
   break;
 }
 Aff_Souris(1);
}


//**************************************************************************
// Refresh_Info: affiche ou met … jour les informations sur les canaux dans
// la fenˆtre pr‚par‚e par Aff_Info().
//**************************************************************************

void Refresh_Info(void)
{
 char s0[128];
 int i, j, pourcent, pcx, pcy, xis, yis,
     plx[9] = { 4, 35, 200, 270, 330, 390, 460, 505, 550 };

 Aff_Souris(2);
 settextstyle(0, 0, 0);
 xis = 490 + (DAS>0)*95 + (TimeMode==3)*100;
 yis = (3 + Canx)*12 + 10; // 35 canaux => yis = 466 !!
 Xi1 = (Mx - xis)/2; Yi1 = My/2 - yis/2;
 Xi2 = Xi1 + xis; Yi2 = Yi1 + yis;
 Vide(Xi1, Xi2, Yi1, Yi2, BLACK);
 setcolor(LIGHTGRAY);
 rectangle(Xi1, Yi1, Xi2, Yi2);
 setcolor(WHITE);
 rectangle(Xi1 - 1, Yi1 - 1, Xi2 + 1, Yi2 + 1);
 setviewport(Xi1, Yi1, Xi2, Yi2, 1);
 settextstyle(2, 0, 0);
 Pg("", plx[0], 0); Pg("Gr.", plx[0], -15);
 Pg("", plx[1], 0); Pg("Name", plx[1], -15);
 switch (Info) {
  case 1:
   Pg("RMS", plx[2], 0);  Pg("Difference", plx[2], -15);
   Pg("", plx[3], 0);  Pg("Resol.", plx[3], -15);
   Pg("Mean", plx[4], 0);  Pg("Value", plx[4], -15);
   Pg("Linear", plx[5], 0);  Pg("Coeff.", plx[5], -15);
   Pg("Valid", plx[6], 0);  Pg("Data", plx[6], -15);
   if (DAS>0) {
    Pg("Over", plx[7], 0);  Pg("Scale", plx[7], -15);
    Pg("Over", plx[8], 0);  Pg("Range", plx[8], -15);
   }
   break;
  case 2:
   for (j = 0; j<MAXHARM; j++) {
    pcx = 200 + j*(390/MAXHARM);
    sprintf(s0, "Harmonic nø%d", j+1);
    Pg(s0, pcx, 0);
    Pg("Phase  Ampli", pcx, -15);
   }
   break;
 }
 Pg("", 0, -15);
 for (i = 0; i<Canx; i++) {
  pcy = 12*(i + 2.7);
  setcolor(Gc[i]);
  if (Gn[i]) {
   sprintf(s0, "%c%c %c", CarCan(Gn[i] - 1), DStyle[Gt[i]], DMark[Gm[i]]);
   Pg(s0, plx[0], pcy);
  }
  Pg(Label[i], plx[1], pcy);
  switch (Info) {
   case 1:
    sprintf(s0, "%1.4lg", sqrt(Bruit[i]/(double)NbY));
    Pg(s0, plx[2], pcy);
    if (Resol[i]!=MAXDOUBLE) sprintf(s0, "%1.4lg", Resol[i]);
    else strcpy(s0, "?");
    Pg(s0, plx[3], pcy);
    sprintf(s0, "%+lg", RLz[i]);
    Pg(s0, plx[4], pcy);
    sprintf(s0, "%+1.4lg", RLa[i]);
    Pg(s0, plx[5], pcy);
    if (Acmin==0.) pourcent = 100;
    else pourcent = (100*Acmin*Decim*(NbY - NbNul[i] - 1))/Tscl;
    if (!strchr(Label[i], '[')) sprintf(s0, "%3d%%", pourcent);
    else strcpy(s0, "   -");
    Pg(s0, plx[6], pcy);
    if (DAS>0) {
     if (!strchr(Label[i], '[')) sprintf(s0, "%5d", NbSauts[i]);
     else strcpy(s0, "    -");
     Pg(s0, plx[7], pcy);
     if (!strchr(Label[i], '[')) sprintf(s0, "%5d", NbDO[i]);
     else strcpy(s0, "    -");
     Pg(s0, plx[8], pcy);
    }
    break;
   case 2:
    if (HarmC[j][i]==0.) break;
    for (j = 0; j<MAXHARM; j++) {
     pcx = 200 + j*(390/MAXHARM);
     sprintf(s0, "%+03.0fø  %6g", HarmS[j][i], HarmC[j][i]);
     Pg(s0, pcx, pcy);
    }
    break;
  }
 }
 setcolor(LIGHTGRAY);
 line(1, yis - 11, xis - 1, yis - 11);
 Vide(0, xis, yis - 10, yis, WHITE);
 setcolor(BLACK);
 Pg("E N T E R", -12, yis - 11);
 setviewport(0, 0, Mx, My, 0);
 Aff_Souris(1);
}


int IsFree(char cd)
{
 struct dfree libre;
 long disp;

 getdfree(cd + 1, &libre);
 if (libre.df_sclus==0xFFFF) return 1;
 disp = (long)libre.df_avail * (long)libre.df_bsec * (long)libre.df_sclus;
 if (Decim*disp<TF_Tot) return 2;
 return 0;
}



//**************************************************************************
// Efface_Tmp: suivant Debug, ‚crit le fichier de d‚bogage ou efface tous
// les fichiers temporaires.
//**************************************************************************

void Efface_Tmp(void)
{
 FILE *f;
 int i, j;
 time_t tt;
 struct tm date;
 char s0[MAXSTR];

 if (Debug) {
  /*
  Pt2("\r\nWriting debug info in '@$db_mgr.dat@'... ");
  if ((f = fopen("$db_mgr.dat", "wt"))==NULL) Sortie("Debug OWT.");
  tt = time(NULL); date = *gmtime(&tt);  strftime(s0, 20, "%Y-%m-%d %X", &date);
  fprintf(f, "# DATE: %s\n", s0);
  fprintf(f, "LCommand = \"%s\" \n", LCommand);
  fprintf(f, "Com = %d ; Port = %x ; VGAT = %d \n", Com, Port, VGAT);
  fprintf(f, "Bin = %d ; ImpFmt = \"%s\" ; ExpFmt = \"%s\" ; Header = \"%s\" \n", Bin, ImpFmt, ExpFmt, Header);
  fprintf(f, "PassL = %d ; NbCol = %d ; NbColFmt = %d \n", PassL, NbCol, NbColFmt);
  fprintf(f, "\nTimeMode = %d ; Acmin = %lf ; Acmax = %lf ; AcPM = %d ; Phase = \"%s\" \n", TimeMode, Acmin, Acmax, AcPM, Phase);
  fprintf(f, "Tmax = %lf ; Tmin = %lf ; Tscl = %lf \n", Tmax, Tmin, Tscl);
  fprintf(f, "AllTime = \"%s\" ; ATime = %lf \n", AllTime, ATime);
  fprintf(f, "DAS = %d ; MDiff = %ld ; MaxVal = %ld \n", DAS, MDiff, MaxVal);
  fprintf(f, "NaN = %lf ; Spike = %lf \n", NaN, Spike);
  fprintf(f, "ChDraw = \"%s\" ; Gv = %d ; NbGs = %d ; AllCh = \"%s\" ; AllChN = \"%s\" ; ChAff = %d \n",
     ChDraw, Gv, NbGs, AllCh, AllChN, ChAff);
  fprintf(f, "Zoom = %lf ; Zmax = %lf ; Zmin = %lf ; Zscl = %lf \n", Zoom, Zmax, Zmin, Zscl);
  fprintf(f, "DMode = %d ; Filtre = %ld ; RLView = %d ; FullScl = %d ; AutoScl = %d ; Info = %d \n", DMode, Filtre, RLView, FullScl, AutoScl, Info);
  fprintf(f, "Fml = %d ; Formula = \"%s\" ; DRef = %d \n", Fml, Formula, DRef);
  fprintf(f, "HiCum = %d ; Doodson = \"%s\" ; DD[] = { %g, %g, %g, %g, %g, %g } ; Period = %g ; NbHiCum = %d \n",
	 HiCum, Doodson, DD[0], DD[1], DD[2], DD[3], DD[4], DD[5], Period, NbHiCum);
  fprintf(f, "nf = %d ; X = %ld ; Y = %ld ; NbY = %ld ; DCh = %d \n", nf, X, Y, NbY, DCh);
  fprintf(f, "MGR_dir = \"%s\"\n", MGR_dir);
  fprintf(f, "\nFD_Tmp = \"%s\" \n", FD_Tmp);
  fprintf(f, "FC_Tmp0 = \"%s\" ; FF_Tmp0 = \"%s\" \n", FC_Tmp0, FF_Tmp0);
  fprintf(f, "FC_Tmp1 = \"%s\" ; FF_Tmp1 = \"%s\" \n", FC_Tmp1, FF_Tmp1);
  fprintf(f, "FC_List = \"%s\" ; FC_Sort = \"%s\" ; FC_HCum = \"%s\"  \n", FC_List, FC_Sort, FC_HCum);
  fprintf(f, "FC_Out = \"%s\" ; FF_Out = \"%s\" \n", FC_Out, FF_Out);
  fprintf(f, "FF_In = \"%s\" ; FN_In = \"%s\" \n", FF_In, FN_In);
  fprintf(f, "FC_Cal = \"%s\" \n", FC_Cal);
  fprintf(f, "\n\n*** NbIF = %d \n", NbIF);
  for (i = 0; i<NbIF; i++) {
   fprintf(f, "\n* %d\tFC_In[] = \"%s\" \n", i, FC_In[i]);
  }
  fprintf(f, "\n\n*** NbFiles = %d \n", NbFiles);
  for (i = 0; i < NbFiles; i++) {
   fprintf(f, "\n* %d\tFStamp[] = %ld \n", i, FStamp[i]);
   fprintf(f, "\tCnx[] = %d ; Acqu[] = %lf ; Fmt[] = \"%s\" \n",
      Cnx[i], Acqu[i], Fmt[i]);
   fprintf(f, "\tTDmin[] = %lf ; TDmax[] = %lf \n", TDmin[i], TDmax[i]);
  }
  fprintf(f, "\n\n*** NbFml = %d \n", NbFml);
  for (i = 0; i<NbFml; i++) {
   fprintf(f, "\n* %d\tIFml[] = \"%s\" \n", i, IFml[i]);
  }
  fprintf(f, "\n\n*** Canx = %d \n", Canx);
  for (i = 0; i < Canx; i++) {
   fprintf(f, "\n* %d\tSName[] = \"%s\" ; SUnit[] = \"%s\" ; Label[] = \"%s\" \n", i, SName[i], SUnit[i], Label[i]);
   fprintf(f, "\tRLz[] = %lf ; RLa[] = %lf ; RLb[] = %lf \n", RLz[i], RLa[i], RLb[i]);
   fprintf(f, "\tBruit[] = %lf ; Resol[] = %lf \n", Bruit[i], Resol[i]);
   fprintf(f, "\tDataOut[] = %d ; Offset[] = %ld  ; Frequence[] = %ld \n", DataOut[i], Offset[i], Frequence[i]);
   fprintf(f, "\tVmax[] = %lf ; Vmin[] = %lf \n", Vmax[i], Vmin[i]);
   fprintf(f, "\tEmax[] = %lf ; Emin[] = %lf \n", Emax[i], Emin[i]);
   fprintf(f, "\tDmax[] = %lf ; Dmin[] = %lf \n", Dmax[i], Dmin[i]);
   fprintf(f, "\tAmax[] = %lf ; Amin[] = %lf ; Ascl[] = %lf ; Apas[] = %lf \n", Amax[i], Amin[i], Ascl[i], Apas[i]);
   for (j = 0; j<2; j++) {
    fprintf(f, "\tCal_x0[%d][] = %lf ; Cal_x1[%d][] = %lf ; Cal_x2[%d][] = %lf ; Cal_x3[%d][] = %lf \n",
       j, Cal_x0[j][i], j, Cal_x1[j][i], j, Cal_x2[j][i], j, Cal_x3[j][i]);
   }
   for (j = 0; j<MAXHARM; j++) {
    fprintf(f, "\tHarmS[%d][] = %lf ; HarmC[%d][] = %lf \n", j, HarmS[j][i], j, HarmC[j][i]);
   }
   fprintf(f, "\tGn[] = %d ; Gt[] = %d ; Gm[] = %d, Gc[] = %d ; Ge[] = %d \n",
      Gn[i], Gt[i], Gm[i], Gc[i], Ge[i]);
  }
  fprintf(f,"\n*** NbGn = %d \n", NbGn);
  for (i = 0; i < NbGn; i++) {
   fprintf(f, "\n* %d\tNbGr[] = %d ; Gs = %d \n", i, NbGr[i], Gs[i]);
   fprintf(f, "\tGmax[] = %lf ; Gmin[] = %lf ; Gscl[] = %lf ; Gpas[] = %lf \n", Gmax[i], Gmin[i], Gscl[i], Gpas[i]);
  }
  fprintf(f,"\n*** NbInfo = %d \n", NbInfo);
  for (i = 0; i < NbInfo; i++) {
   fprintf(f, "\n* %d\tFInfo[] = \"%s\" \n", i, FInfo[i]);
  }
  fprintf(f,"\n*** Pal.size = %d \n", Pal.size);
  for (i = 0; i < NUMCOLORS; i++) {
   fprintf(f, "\n* %d\tPal.colors[] = %d \n", i, Pal.colors[i]);
  }
  fclose(f_in);
  cprintf("Done.\n");
 */
 } else {
  remove(FF_Tmp0);
  remove(FF_Tmp1);
  remove(FC_List);
  remove(FC_Sort);
  remove(FC_HCum);
  remove(FC_Tabl);
 }
}


//**************************************************************************
// Sortie: appelle Efface_Tmp() et quitte le programme et retourne … DOS,
// aprŠs avoir, le cas ‚ch‚ant:
//	- lib‚r‚ l'espace m‚moire r‚serv‚ aux filtres;
//	- ferm‚ la fenˆtre graphique;
//	- ferm‚ la connexion avec un æDAS et lib‚r‚ le port Com;
// Si *ss a une longueur non nulle, affiche un message d'erreur et exit(-1),
// sinon exit(1).
//**************************************************************************

void Sortie(char *ss)
{
 if (EcranGraph) closegraph();
 Efface_Tmp();

 if (strlen(ss)) {
  Aff_Message(ss, 2, 1);
  //cputs("\r\nHit any key to return to DOS...\r\n");
  //while (!kbhit());
  exit(-1);
 } else {
  if (DMode==4 || DMode==5) Eff_Filter();
  if (Com) {
   if (NDAS) Ecrire_Chaine("-\r");
   while (!(inportb(Port + 5)&0x20));
   disable();
   setvect(Com==1 ? 0x0c:0x0b, old_RS);
   enable();
  }
  exit(1);
 }
}

