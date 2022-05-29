#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <graphics.h>
#include <math.h>
#include <alloc.h>
#include <time.h>
#include <dir.h>
#include <string.h>
#include <int1_3.h>
#include <grafdsp.h>
#include <dos.h>

#define ON_color  	15
#define OFF_color  	0
#define LIM_DIM_WIN  	420
#define LifeWinUP	45
#define LifeWinDW  	320


int GreenMenu[]= { 00,00,00,
		   55,63,55,
		   31,41,31,
		   12,28,12
		   };

char *MenuStart[] = { "A mid \"1\"",
		      "A random \"1\"",
		      "All \"1\"",
		      "Random p=0.5",
		      "Random p=0.xx",
		      "1í1í1í1í...",
		      "Draw",
		     };

char *MenuProb[] = { "p=0.01",
		     "p=0.02",
		     "p=0.03",
		     "p=0.05",
		     "p=0.10",
		     "p=0.20",
		     "p=0.40",
		     "p=0.80",
		     "p=0.85",
		     "p=0.95",
		     "p=0.99"
		      };



int MAXV = 158;
int MAXH = 158;
int DV   = 55;
int DH   = 55;
int prob = 0;
int deltaLW = LifeWinDW-LifeWinUP;
int wolf[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
int iciclo;
int jciclo;
unsigned long max= 32767; /* Max possible rule */

typedef char Gen[158+2][158+2];
union  REGS  regs,regs2;
struct SREGS sregs;
char *default_msg = "WA2D, Dec.'93: binary 2DCA, 4-cell neighborhood. Wolfram's rules 0..65535";

/* prototipi funzioni: */

void Desk		( void);
long Controllo		( Gen *gp, long ng);
void GenNulla		( Gen *gp);
void Out_gen		( int time, Gen *gp);
void Gen_wbit		( long wnum, int w_bit[16]);
void Itera_Rule		( Gen *vgp, Gen *ngp);
void Itera		( int gen_t, long w_num, int *w_bit, Gen *gp);
void CollegaBordi	( Gen *gp);
void StampaNumGen	( long ng);
void PSlide		( long wn );
long GetSlide		( long wn );
unsigned char SWProb	( float p );
void Field		( int x1, int y1, int len, char *titolo, char *text );
void OutCoord		( int x, int y, int xc, int yc );
char GetSpace		( int *nv, int *nh );
void Saluti		( void );
char DrawBit		( Gen *ng );


int main( int argc, char *argv[] )
{
  struct Gitem S[]  = { 10, 405, "W-",
		       342, 405, "W+",
			 5,  5,  "Quit",
		       402, 390, "New",
			14,  365, "Start gen >",
			304,  365, "Window",
			64,  405, "                              ",
			/*"Scroll" bar button*/
			402, 428, "Activate 65536 rules" };
  int Drive = 9,
      Modo  = 2, i;
  int gen_t  = 1,
      gent_hlp,
      prob_hlp;
  int nv, nh;
  long w_num = 32718;
  char buf[15];
  char item;
  int on65 = 0;
  Gen g[2];
  void *Menu=NULL;

  if( INIT_MOUSE()==1) {
   printf("\nSorry, this program requires the MOUSE. Buy it, stingy!...");
   exit(0);
   }

  DV 	= atoi(argv[1]);
  DH  	= atoi(argv[2]);
  w_num = atoi(argv[3]);
  if( DV == 0) 	DV = 32;
  if( DH == 0) 	DH = 32;
  if( w_num==0) w_num=31718;

  if( argc>2) {
    if( DV > MAXV) {
		 DV=MAXV;
		 printf("Max vertical # of cells:  %d!\n",MAXV);
		 }
    if( DH > MAXH) {
		 DH=MAXH;
		 printf("Max horizontal # of cells: %d!\n",MAXH);
		 }
    }
  printf("Options: WA2D [vert][horiz][rule], hit a key to begin...\n");
  getch();

  registerbgidriver( EGAVGA_driver);
  initgraph( &Drive, &Modo,"");
  MOUSE_LIMITS(0,0,639,479);
  MOUSECURSOR(1);
  Desk();
  for (i=0; i<=7; i++) GENbutton(S[i].x, S[i].y, S[i].Gi, BTmask);
  Cornice(64,405,144,430,GIU);
  strcpy( buf, MenuStart[gen_t-1]);
  Field(155, 366, 15, "Actual:", buf);

  Gen_wbit( w_num, wolf );
  PSlide( w_num );
  srand((unsigned int)time(NULL));

  Itera( gen_t, w_num, wolf, g);

  do {
  if( kbhit() ) {
		 Saluti();
		 exit(1);
		 }
  /*-Main loop-*/
  item = SELECTbutton( S, 8);
  if( item==1) {
		 w_num--;
		 if( w_num < 0) w_num = max;
		 PSlide(w_num);
		 Itera(gen_t, w_num, wolf, g);
		 } else
  if( item==2) {
		 w_num++;
		 if( w_num > max ) w_num = 0;
		 PSlide(w_num);
		 Itera(gen_t, w_num, wolf, g);
		 } else
  if( item==4) Itera(gen_t, w_num, wolf, g); else
  if( item==5) {
		/* Start Gen */
		BarraHelp("Choose the initial configuration of the universe...");
		gent_hlp = gen_t;
		gen_t = GetItemOpts(MOUSEPOS().x-35,
		S[4].y-60,130,MenuStart,7,Menu);
		if( gen_t==5) {
		   prob_hlp = prob;
		   prob = GetItemOpts(MOUSEPOS().x-20,
		   MOUSEPOS().y-135,70,MenuProb,11,Menu);
		   if( prob==0) prob = prob_hlp;
		}
		if( gen_t==0) gen_t = gent_hlp;
		if( gen_t==7) DrawBit(g);
		Itera(gen_t, w_num, wolf, g);
		if( gen_t==7) gen_t = gent_hlp;
	       } else
  if( item==6) {
		nv = DV;
		nh = DH;
		if( GetSpace( &nv, &nh)>=0) {
					     DV = nv;
					     DH = nh;
					     }
		Itera(gen_t, w_num, wolf, g);
		} else
  if( item==7) {
		w_num = GetSlide( w_num );
		PSlide(w_num);
		Itera(gen_t, w_num, wolf, g);
		}
  if( item==8) {
		on65 = 1 - on65;
		if( on65) max = 65535;
		     else max = 32767;
		GENbutton(S[7].x, S[7].y, S[7].Gi, BTmask+on65);
		w_num = w_num % max;
		PSlide( w_num );
		Itera(gen_t, w_num, wolf, g);
		}
  } while( item!=3 );
  Saluti();
  return 1;
}

void GenNulla(Gen *gp)      /*<1>*/
{
    int  v, h;
    for (v = 0; v <= DV-1; v++)
	for( h = 0; h <= DH-1; h++)
	(*gp)[v][h]=0;
}

unsigned char SWProb( float p)    /* Setta una cella con probabilita' p */
{
 int r;
 char ret = 0;

 r = random(10000);
 if( r < p*10000) ret=1;
 return ret;
}


void Start_Gen( int gen_type, Gen *gp)
{
  int v,h,i;
  char buf[50];
  float Dens[] = { 0.5,
		  0.01, 0.02, 0.03, 0.05, 0.10,
		  0.20, 0.40, 0.80, 0.85, 0.95, 0.99 };

  /* Un 1 a met…,
     Un 1 a caso,
     Tutti 1,
     Random p=0.5,
     Random p=0.xx,
     1í1í1í1í...,
     Draw */

  strcpy( buf, MenuStart[gen_type-1]);
  Field(155, 366, 15, "Actual:", buf);
  if( gen_type==7) return;
  for (v = 0; v <= DV-1; v++)
     for( h = 0; h <= DH-1; h++)
     {
       if( gen_type==1 || gen_type==2 ||
		gen_type==7) (*gp)[v][h] = 0; else
       if( gen_type==3) (*gp)[v][h] = 1; else
       if( gen_type==4) (*gp)[v][h] = SWProb(0.5); else
       if( gen_type==5) (*gp)[v][h] = SWProb(Dens[prob]); else
       if( gen_type==6) (*gp)[v][h] = h%2;
     }
  if( gen_type==1 ) (*gp)[DV/2][DH/2] = 1; else
  if( gen_type==2) (*gp)[random(DV-1)][random(DH-1)] = 1;
}

void Gen_wbit(long wnum, int w_bit[16]) /*Genera il binario del numero W*/
{
    char buf[80];                     		/*buffer per messaggio*/
    char digit[] = "0123456789";            /*validi per tutte le basi*/
    int i;
    long wn = wnum;

    for (i=0; i<=15; i++) w_bit[i] = 0;

    i = 0;
    while (wnum > 0) {
	    buf[i++] = digit[wnum%2];
	    w_bit[16-i] = wnum%2;
	    wnum /= 2;
	}
    sprintf(buf," Wolfram's rule %ld: %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",wn,
    w_bit[0], w_bit[1],
    w_bit[2], w_bit[3],
    w_bit[4], w_bit[5],
    w_bit[6], w_bit[7],
    w_bit[8], w_bit[9],
    w_bit[10], w_bit[11],
    w_bit[12], w_bit[13],
    w_bit[14], w_bit[15]
    );
    Cornice(8,440,392,460,SUPIENO);
    GText(10,446,1,buf);
}

void Itera_Rule(Gen *vgp, Gen *ngp)
{
  int v,h;
  char
      UP1,
    LF2, RG3,
      DW4;

  GenNulla(ngp);
  /*CollegaBordi(vgp);*/

  for (v = 1; v <= DV-2; v++)
    for (h = 1; h <= DH-2; h++) {
	    UP1=(*vgp)[v+1][h];
	    LF2=(*vgp)[v][h-1];
	    RG3=(*vgp)[v][h+1];
	    DW4=(*vgp)[v-1][h];
	    (*ngp)[v][h] = wolf[UP1*8+LF2*4+RG3*2+DW4];
    }

}

void Itera( int gen_t, long w_num, int *w_bit, Gen *gp)
{
 int i, time=0, gcorr = 0, gnew;
 char buf[10];

 Cornice(455,55,465+DH,65+DV,SUPIENO);
 Cornice(8,LifeWinUP,8+LIM_DIM_WIN,LifeWinDW,SUPIENO);
 sprintf(buf, "%dx%d", DV, DH);
 GText(455, 70+DV, 3, buf);
 Start_Gen( gen_t, gp);
 Out_gen(time, gp);
 Gen_wbit( w_num, w_bit);

 setcolor(0);
 MOUSE_LIMITS(0,320,639,479);
 BarraHelp("Computing iteration no.");
 do {
      time++;
      Cornice(205, 465, 250, 478, GIUPIENO);
      sprintf( buf, "%d", time);
      GText(210, 468, 0, buf);
      gnew = 1-gcorr;                         /*indice nuova gen <5>*/
      Itera_Rule(gp+gcorr,gp+gnew);
      Out_gen(time, gp+gnew);
      gcorr = gnew;                           /*cambia gen*/
      if( IN_MOUSE(0,0,999,999,1)==0
	  || IN_MOUSE(0,0,999,999,1)==1) break;
      if( time%(LIM_DIM_WIN-2)==0) Cornice(8,LifeWinUP,8+LIM_DIM_WIN,LifeWinDW,SUPIENO);
  } while( !kbhit());

 MOUSE_LIMITS(0,0,639,479);
}

void CollegaBordi(Gen *gp)    /*<1>*/
{
    int  h,v;

    for (h = 1; h <= DH; h++) {
        (*gp)[0][h] = (*gp)[DV][h];                 /*riporta sopra <2>*/
        (*gp)[DV+1][h] = (*gp)[1][h];               /*e sotto*/
    }
    for (v = 1; v <= DV; v++) {
        (*gp)[v][0] = (*gp)[v][DH];                 /*a sinistra <3>*/
        (*gp)[v][DH+1] = (*gp)[v][1];               /*e a destra*/
    }
    (*gp)[0][0] = (*gp)[DV][DH];                    /*angolo sup.sin. <4>*/
    (*gp)[DV+1][0] = (*gp)[1][DH];                  /*inf.sinistro*/
    (*gp)[0][DH+1] = (*gp)[DV][1];                  /*sup.destro*/
    (*gp)[DV+1][DH+1] = (*gp)[1][1];                /*inf.destro*/
}

void Out_gen(int time, Gen *gp)
{
  int  v,h;
  int dx=460, dy=60;
  int dxd=10, hg=0;
  long life=0;

  for (v = 0; v <= DV-1; v++) {                     /*per ogni riga*/
    for (h = 0; h <= DH-1; h++) {                 /*prepara linea*/
	if( (*gp)[v][h]==1) {
			putpixel(dx+h,dy+v,ON_color);
			life = life + 1;
			}
	else putpixel(dx+h,dy+v,OFF_color);
	}
    if( IN_MOUSE(0,0,999,999,1)==0
	  || IN_MOUSE(0,0,999,999,1)==1) break;
    }
  hg = life*deltaLW/(DV*DH);
  line(dxd+time%(LIM_DIM_WIN-2), LifeWinDW-hg ,
	dxd+time%(LIM_DIM_WIN-2), LifeWinDW-1);
}


void Desk( void)
{
 int i;

 NewColorMenu(GreenMenu);
 Win(0,0,640,480,2,3,1,"Binary 2D CA, 4-cell neighborhood",0);
 Cornice(8,358,392,398,SU);
 for (i=LifeWinUP+4; i<=LifeWinDW; i += 4) line(LIM_DIM_WIN+12, i,
						LIM_DIM_WIN+18, i);
}

void PSlide( long wn )
{
 int bl = 66;  /*Big-left        >bl±±±±±±±±±±sl±br±<  */
 int sl = 15;  /*Slide lungh.*/
 int br = 272; /*Big-right*/
 int ls = 253;
 int x=bl;

 Cornice(64,405,64+br,430,GIUPIENO);
 x = wn*ls/max;
 Cornice(bl+x,408, bl+x+sl, 428, SU);
}

long GetSlide( long wn )
{
 char ext = -1;
 int bl = 66;  /*Big-left        >bl±±±±±±±±±±sl±br±<  */
 int sl = 15;  /*Slide lungh.*/
 int br = 272; /*Big-right*/
 int ls = 253;
 unsigned long x, oldwn = wn;
 int fwb[16];

 BarraHelp("Left Click <- Confirm, Right Click -> Abort");
 Cornice(64,405,64+br,430,GIUPIENO);
 MOUSE_LIMITS(bl,405,bl+br-sl-4,430);
 Cornice(MOUSEPOS().x,408, MOUSEPOS().x+sl, 428, SU);
 wn = (MOUSEPOS().x-66)*max/ls;
 Gen_wbit( wn, fwb);
 WAIT_MOUSE();

 do {
 Cornice(64,405,64+br,430,GIUPIENO);
 wn = (MOUSEPOS().x-66)*max/ls;
 MOUSECURSOR(0);
 Cornice(MOUSEPOS().x,408, MOUSEPOS().x+sl, 428, SU);
 Gen_wbit( wn, fwb);
 MOUSECURSOR(1);
 WAIT_MOUSE();
 ext = IN_MOUSE(0,0,640,480,1);
 } while( ext==-1);
 if( ext==1) wn=oldwn;
 return wn;
}

void Group( int x1, int y1, int x2, int y2, char *text)
{
  Cornice(x1,y1,x2,y2,SU);
  setcolor( L_TASTO);
  line( x2+1,y1+1,x2+1,y2+1);
  line( x2-1,y2+1,x1-1,y2+1);
  setcolor(  D_TASTO);
  line( x1-1,y2, x1-1,y1-1);
  line( x1, y1-1, x2, y1-1);
  setfillstyle(1, N_TASTO);
  bar(x1+8,y1-3,x1+8+(strlen(text)+2)*8,y1+3);
  GText(x1+17,y1-3,3,text);
}

void Field( int x1, int y1, int len, char *titolo, char *text )
{
 GText( x1,y1,3, titolo);
 setfillstyle(1, L_TASTO);
 bar( x1, y1+11, x1+8*(len+1), y1+23);
 setcolor(  D_TASTO);
 rectangle(x1-1, y1+10, x1+8*(len+1)+1, y1+24);
 GText(x1+3, y1+14, 0, text);
}

void OutCoord(int x, int y, int xc, int yc)
{
  char cvx[8],cvy[8];

  Cornice(x,y-2,x+70,y+8,SUPIENO);
  itoa(xc,cvx,10);
  itoa(yc,cvy,10);
  strcat(cvx,",");
  GText(x+3,y,4,strcat(cvx,cvy));
}


char GetSpace( int *nv, int *nh )
{
   struct pointtype START, FINE;
   int ox = 460,
       oy = 60;
   char ok = 0;

   BarraHelp("Window sizing.                         Delta: ëx,ëy");
   MOUSECURSOR(0);
   setfillstyle(1, B_TASTO);
   bar(ox, oy, ox + MAXH, oy + MAXV );
   MOUSECURSOR(1);
   MOUSE_LIMITS(ox, oy, ox + MAXH, oy + MAXV );
   SET_MOUSE_POS( ox+DH, oy+DV );
   MOUSE_PRESSED(0);

   while( !MOUSE_PRESSED(0) ) {

	FINE = MOUSEPOS();

	   setlinestyle(DASHED_LINE, 1, 1);
	   setcolor(L_TASTO);
	   MOUSECURSOR(0);
	   rectangle(ox, oy, FINE.x, FINE.y);
	   MOUSECURSOR(1);
	   setlinestyle(SOLID_LINE, 1, 1);

	*nh = MOUSEPOS().x-ox;
	*nv = MOUSEPOS().y-oy;

	WAIT_MOUSE();

	   setcolor(B_TASTO);
	   MOUSECURSOR(0);
	   rectangle(ox, oy, FINE.x, FINE.y);
	   MOUSECURSOR(1);

	OutCoord(432,468,MOUSEPOS().x-ox, MOUSEPOS().y-oy);
	if( MOUSE_PRESSED(1)) ok=-1;
   }

   if( ok==-1) BarraHelp("The old dimension is kept...");
   MOUSE_LIMITS(0,0,639,460);
   setfillstyle(1, N_TASTO);
   MOUSECURSOR(0);
   bar(ox-10, oy-10, ox + MAXH + 10, oy + MAXV + 30);
   MOUSECURSOR(1);
   return ok;
}

void Saluti( void )
{
 textmode(C80);
 printf("Thank you for using WA2D, see you...\n");
 printf("The author is available on Internet at: magi@ghost.dsi.unimi.it");
}


char DrawBit( Gen *ng )
{
   int ox = 460,
       oy = 60, x, y;
   char ok = 0;

   BarraHelp("Draw the start configuration.  Left Click=1, Right Click=stop");
   MOUSE_PRESSED(0);
   setfillstyle(1, B_TASTO);
   GenNulla( ng );
   Out_gen( 1, ng );

   MOUSE_LIMITS(ox, oy, ox + DH-1, oy + DV-1);
   while( IN_MOUSE(0,0,640,480,1)!=1 && ok==0) {

   if( IN_MOUSE(0,0,640,480,1)==0) {
	x = MOUSEPOS().x;
	y = MOUSEPOS().y;
        MOUSECURSOR(0);
	putpixel( x, y, 1);
	MOUSECURSOR(1);

	(*ng)[y-oy][x-ox] = 1;
	}
   if( IN_MOUSE(0,0,640,480,1)==1) ok=1;
   }
   MOUSE_LIMITS(0,0,639,460);
   IN_MOUSE(0,0,640,480,1);
}