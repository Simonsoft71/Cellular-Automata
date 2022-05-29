#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <alloc.h>
#include <graphics.h>
#include <alloc.h>
#include <time.h>
#include <dir.h>
#include <string.h>
#include <int1_3.h>
#include <grafdsp.h>
#include <dos.h>

#define cromos 		32
#define BOT_IT_WIN 	420   /* Bottom of iteration window */
#define LIM_DIM_WIN 	380   /* Limite Win Dinamica */
#define MAXITER 	30000
#define MAXLEN 		215   /* Lunghezza massima dello 'stream'
			      (lo 'stream' e' la successione delle cellule */

/* Dichiarazioni external per libreria <grafdsp.h> */
int    iciclo,jciclo;
union  REGS      regs,regs2;
struct SREGS     sregs;
char *default_msg = "Automi cellulari elementari (1-dimensionali) binari. By Simone Maggi";


int RedMenu[] = { 00,00,00,
		  63,57,55,
		  60,35,35,
		  45,15,25
		  };

/* Prototipi delle funzioni */

void Gen_wbit   ( int w_a, int w_b, int w_c, int w_d, int w_bit[cromos]);
void Start_Gen  ( int gen_type, int s_len, unsigned char *stream);
unsigned char SWProb( float p);
void Out_Gen    ( int time, int s_len, unsigned char *stream);
void Itera_Rule ( int s_len, int w_bit[cromos], unsigned char *stream);
void Itera	( int gen_t, int w_a, int w_b, int w_c, int w_d,
		  int *w_bit, unsigned char *stream);
void Field	( int x1, int y1, int len, char *titolo, char *text );
void Desk	( void);
void Group	( int x1, int y1, int x2, int y2, char *text);

int n_iter = 280;
int s_len = 180;     /* Lunghezza stream, 30 di default */
int prob = 0;	     /* Densita' uniforme con p=..., default 0 (p=0.5) */
int cyclic = 0;
int w_a, w_b, w_c, w_d;
int max = 255;


char *MenuStart[] = { "Un \"1\" a metЕ",
		      "Un \"1\" a caso",
		      "Tutti \"1\"",
		      "Random p=0.5",
		      "Random p=0.xx",
		      "1н1н1н1н...",
		      "1нн1нн1нн..",
		      "11нн11нн..."
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

int main( int argc, char *argv[] )
{
 int Drive = 9;
 int Modo  = 2;
 int gen_t = 4;      /* Start gen_type = random di default */
 int w_bit[cromos] = { 0,0,0,0,0,0,0,0,
		       0,0,0,0,0,0,0,0,
		       0,0,0,0,0,0,0,0,
		       0,0,0,0,0,0,0,0 };

 int i, j, item, gent_hlp, prob_hlp;
 unsigned char *stream;

struct Gitem S[]  = { 10, 405, "-",
		      58, 405, "+",
		      111, 405, "-",
		      159, 405, "+",
		      212, 405, "-",
		      260, 405, "+",
		      313, 405, "-",
		      361, 405, "+",
		      313, 370, "Nuovo",
		      14,  365, "Start gen >",
		      14,  320, "Window",
		      14,  275, "N Iter",
		      210, 320, "Cyclic",
		      5,   5,   "Quit!" };
 int buttons  = 14;
 void *Menu   = NULL;
 char buf[15] = "";

 if( INIT_MOUSE()==1) {
  printf("\nSpiacente, per questo programma serve il MOUSE\
  (comprane uno tirchione!)...");
  exit(0);
  }

  if( (w_a = atoi(argv[1]))==0) w_a = 195;
  if( (w_b = atoi(argv[2]))==0) w_b = 25;
  if( (w_c = atoi(argv[3]))==0) w_c = 115;
  if( (w_d = atoi(argv[4]))==0) w_d = 122;

 registerbgidriver( EGAVGA_driver);
 initgraph( &Drive, &Modo,"");
 MOUSE_LIMITS(0,0,639,479);
 MOUSECURSOR(1);
 Desk();
 for (i=0; i<=buttons-1; i++) GENbutton(S[i].x, S[i].y, S[i].Gi, BTmask);
 sprintf( buf, "%d", n_iter);
 Field( S[buttons-3].x+100, S[buttons-3].y+2, 6, "Attuale:", buf);
 sprintf( buf, "%d", s_len);
 Field( S[buttons-4].x+100, S[buttons-4].y+2, 6, "Attuale:", buf);


 stream = (char *) malloc (s_len+5);
 Itera( gen_t, w_a, w_b, w_c, w_d, w_bit, stream);
 do { /*-Main loop-*/
  item = SELECTbutton( S, buttons);
  if( item==1) {
		 w_a--;
		 if( w_a < 0) w_a = max;
                 Itera(gen_t, w_a, w_b, w_c, w_d, w_bit, stream);
		 } else
  if( item==2) {
		 w_a++;
		 if( w_a > max) w_a=0;
                 Itera(gen_t, w_a, w_b, w_c, w_d, w_bit, stream);
		 } else
  if( item==3) {
		 w_b--;
		 if( w_b < 0) w_b = max;
                 Itera(gen_t, w_a, w_b, w_c, w_d, w_bit, stream);
		 } else
  if( item==4) {
		 w_b++;
		 if( w_b > max) w_b=0;
                 Itera(gen_t, w_a, w_b, w_c, w_d, w_bit, stream);
		 } else
  if( item==5) {
		 w_c--;
		 if( w_c < 0) w_c = max;
                 Itera(gen_t, w_a, w_b, w_c, w_d, w_bit, stream);
		 } else
  if( item==6) {
		 w_c++;
		 if( w_c > max) w_c=0;
                 Itera(gen_t, w_a, w_b, w_c, w_d, w_bit, stream);
		 } else
  if( item==7) {
		 w_d--;
		 if( w_d < 0) w_d = max;
                 Itera(gen_t, w_a, w_b, w_c, w_d, w_bit, stream);
		 } else
  if( item==8) {
		 w_d++;
		 if( w_d > max) w_d=0;
                 Itera(gen_t, w_a, w_b, w_c, w_d, w_bit, stream);
		 } else
  if( item==9) Itera(gen_t, w_a, w_b, w_c, w_d, w_bit, stream); else
  if( item==10) {
		BarraHelp("Scegli la configurazione iniziale della stringa di bit");
		gent_hlp = gen_t;
		gen_t = GetItemOpts(MOUSEPOS().x-35,
		S[4].y-60,130,MenuStart,8,Menu);
		if( gen_t==5) {
		   prob_hlp = prob;
		   prob = GetItemOpts(MOUSEPOS().x-20,
		   MOUSEPOS().y-135,70,MenuProb,11,Menu);
		   if( prob==0) prob = prob_hlp;
		}
		if( gen_t==0) gen_t = gent_hlp;
		Itera(gen_t, w_a, w_b, w_c, w_d, w_bit, stream);
	       } else
  if( item==11)
	      {
	       BarraHelp("Immetti la dimensione della nuova finestra...");
	       do {
		itoa(s_len,buf,10);
		strcpy(buf,XInput(S[buttons-4].x+102,S[buttons-4].y+16,0,2,buf,4));
		s_len = atoi(buf);
		if( s_len > MAXLEN-5) BarraHelp("Massima dimesione stringa: 210!");
	       } while (s_len > MAXLEN-5);
	       Desk();
	       MOUSE_LIMITS(0,0,639,479);
	       MOUSECURSOR(1);
	       Desk();
	       Field( S[buttons-4].x+100, S[buttons-4].y+2, 6, "Attuale:", buf);
	       sprintf( buf, "%d", n_iter);
	       Field( S[buttons-3].x+100, S[buttons-3].y+2, 6, "Attuale:", buf);
	       for (i=0; i<=buttons-1; i++)
	       GENbutton(S[i].x, S[i].y, S[i].Gi, BTmask);
	       GENbutton(S[12].x, S[12].y, S[12].Gi, BTmask+cyclic);
	       Itera(gen_t, w_a, w_b, w_c, w_d, w_bit, stream);
	       } else
  if( item==12) {
	       BarraHelp("Immetti il numero di iterazioni tra 1 e 30000");
	       do {
		itoa(n_iter,buf,10);
		strcpy(buf,XInput(S[buttons-3].x+102,S[buttons-3].y+16,0,2,buf,5));
		n_iter = abs(atoi(buf));
	       } while (n_iter > MAXITER);
	       Desk();
	       MOUSE_LIMITS(0,0,639,479);
	       MOUSECURSOR(1);
	       Desk();
	       Field( S[buttons-3].x+100, S[buttons-3].y+2, 6, "Attuale:", buf);
	       sprintf( buf, "%d", s_len);
	       Field( S[buttons-4].x+100, S[buttons-4].y+2, 6, "Attuale:", buf);
	       for (i=0; i<=buttons-1; i++)
	       GENbutton(S[i].x, S[i].y, S[i].Gi, BTmask);
	       GENbutton(S[12].x, S[12].y, S[12].Gi, BTmask+cyclic);
	       Itera(gen_t, w_a, w_b, w_c, w_d, w_bit, stream);
	       } else
  if( item==13) {
		/*
		cyclic=1-cyclic;
		GENbutton(S[12].x, S[12].y, S[12].Gi, BTmask+cyclic);
		Itera(gen_t, w_a, w_b, w_c, w_d, w_bit, stream);
		*/
		}
  } while( item!=buttons || kbhit()==1);
  textmode(C80);
  printf("Grazie per aver usato WAUTOM e arrivederci...\n");
  printf("L'autore К raggiungibile su Internet all'indirizzo: magi@ghost.dsi.unimi.it");
 return 0;
}



void Gen_wbit(int w_a, int w_b, int w_c, int w_d, int w_bit[cromos]) /*Genera il binario del numero W*/
{
    char buf[80];                     		/*buffer per messaggio*/
    int i, j, wn = w_a;

    for (i=0; i<=cromos-1; i++) w_bit[i] = 0;

    i = 0;
    wn=w_d;
    while (wn > 0) {
	    w_bit[i] = wn%2;
	    i++;
	    wn /= 2;
    }
    i = 8;
    wn=w_c;
    while (wn > 0) {
	    w_bit[i] = wn%2;
	    i++;
	    wn /= 2;
    }
    i = 16;
    wn=w_b;
    while (wn > 0) {
	    w_bit[i] = wn%2;
	    i++;
	    wn /= 2;
    }
    i = 24;
    wn=w_a;
    while (wn > 0) {
	    w_bit[i] = wn%2;
	    i++;
	    wn /= 2;
    }
    sprintf(buf,"%d=%d%d%d%d%d%d%d%d %d=%d%d%d%d%d%d%d%d %d=%d%d%d%d%d%d%d%d %d=%d%d%d%d%d%d%d%d",
    w_a,
    w_bit[31], w_bit[30], w_bit[29], w_bit[28],
    w_bit[27], w_bit[26], w_bit[25], w_bit[24],
    w_b,
    w_bit[23], w_bit[22], w_bit[21], w_bit[20],
    w_bit[19], w_bit[18], w_bit[17], w_bit[16],
    w_c,
    w_bit[15], w_bit[14], w_bit[13], w_bit[12],
    w_bit[11], w_bit[10], w_bit[9], w_bit[8],
    w_d,
    w_bit[7], w_bit[6], w_bit[5], w_bit[4],
    w_bit[3], w_bit[2], w_bit[1], w_bit[0]);
    Cornice(1,440,430,460,SUPIENO);
    GText(5,446,0,buf);
}

void Start_Gen(int gen_type, int s_len, unsigned char *stream)
{
 int i;
 char buf[50];
 float Dens[] = { 0.5,
		  0.01, 0.02, 0.03, 0.05, 0.10,
		  0.20, 0.40, 0.80, 0.85, 0.95, 0.99 };
 /*
 1- Un 1 a metЕ,
 2- Un 1 a caso,
 3- Tutti 1,
 4- Random,
 5- Uniforme con p=...
 6- 1н1н1н1н...
 7- 1нн1нн1нн..
 8- 11нн11нн...
 */

 strcpy( buf, MenuStart[gen_type-1]);
 Field(155, 366, 15, "Attuale:", buf);
 stream[0]=0;
 stream[1]=0;
 stream[s_len]=0;
 stream[s_len+1]=0;

 for (i=1; i<=s_len; i++)
 {
  if( gen_type==1 ||
      gen_type==8 ||
      gen_type==2) stream[i]=0; else
  if( gen_type==3) stream[i]=1; else
  if( gen_type==4) stream[i]=SWProb(0.5); else
  if( gen_type==5) stream[i]=SWProb(Dens[prob]); else
  if( gen_type==6) stream[i]=i%2; else
  if( gen_type==7) {
		    if( i%3==1) stream[i]=1;
			   else stream[i]=0;
		    }
}
 if( gen_type==1 ) stream[s_len/2]=1; else
 if( gen_type==2) stream[random(s_len-1)]=1;
 if( gen_type==8) {
		   i=1;
		   do {
		      stream[i]=1;
		      stream[i+1]=1;
		      i += 4;
		    } while( i<=s_len);
		   }
}

unsigned char SWProb( float p)    /* Setta una cella con probabilita' p */
{
 int r;
 char ret = 0;

 r = random(10000);
 if( r < p*10000) ret=1;
 return ret;
}


void Out_Gen(int time, int s_len, unsigned char *stream)
{
 int i, ncell1_on = 0;
 int dx = 625-s_len, dy = 40;
 int dxd = 10, dyd = 260;

 if( time%LIM_DIM_WIN==0) Cornice(8,45,400,260,SUPIENO);
 for (i=1; i<=s_len; i++) {
        if( stream[i]==1) {
	ncell1_on++;
	}
	putpixel(dx+i, dy+time%BOT_IT_WIN, stream[i]);
 }
 putpixel(dxd+time%LIM_DIM_WIN,dyd-ncell1_on-1,1);
}


void Itera_Rule( int s_len, int w_bit[cromos], unsigned char *stream)
{
 int punt = cyclic;
 int a, b, c, d, e, risp;
 unsigned char ng[MAXLEN];

 ng[0]=0;
 ng[1]=0;
 ng[s_len]=0;
 ng[s_len+1]=0;


 /* Inizio iperpiano */
 if( cyclic) {
	a = stream[s_len];
	b = stream[punt];
	c = stream[punt+1];
	d = stream[punt+2];
	e = stream[punt+3];
	risp = w_bit[ a*16+b*8+c*4+d*2+e ];
	ng[punt] = risp;
 }

 do{
 a = stream[punt];
 b = stream[punt+1];
 c = stream[punt+2];
 d = stream[punt+3];
 e = stream[punt+4];
 risp = w_bit[ a*16+b*8+c*4+d*2+e ];
 ng[punt+2] = risp;
 punt++;
 } while (punt <= s_len-cyclic-3);

 /* "Fine" iperpiano */
 if( cyclic) {
	 a = stream[s_len-1];
	 b = stream[s_len];
	 c = stream[1];
	 d = stream[2];
	 e = stream[3];
         risp = w_bit[ a*16+b*8+c*4+d*2+e ];
	 ng[1] = risp;
 }

 for (punt=1; punt<=s_len; punt++)
 stream[punt] = ng[punt];
}

void Desk( void)
{

 NewColorMenu(RedMenu);
 Win(0,0,640,480,2,3,1,"Automi cellulari per Teoria dei Sistemi",0);
 Cornice(8,267,300,309,SU);
 Cornice(8,313,300,354,SU);
 Cornice(8,358,300,398,SU);
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

void Itera( int gen_t, int w_a, int w_b, int w_c, int w_d,
		  int *w_bit, unsigned char *stream)
{
 int i, time=1;
 char buf[40];

 Cornice(620-s_len,35,632,60+n_iter,SUPIENO);
 Cornice(8,45,400,260,SUPIENO);
 sprintf(buf, "R=(%d,%d,%d,%d)", w_a, w_b, w_c, w_d);
 GText(630-s_len,46+n_iter, 2, buf);
 MOUSECURSOR(0);
 setfillstyle( 1,0);
 bar(9,46,399,259);
 MOUSECURSOR(1);
 Start_Gen(gen_t, s_len, stream);
 Out_Gen( time ,s_len, stream);
 Gen_wbit( w_a, w_b, w_c, w_d, w_bit);

 MOUSE_LIMITS(0,0,600-s_len,479);
 for (i=1; i<=n_iter; i++)
  {
   time++;
   if( IN_MOUSE(0,0,999,999,1)==0
   || IN_MOUSE(0,0,999,999,1)==1) break;
   Itera_Rule(s_len, w_bit, stream);
   Out_Gen( time, s_len, stream);
   if( time%BOT_IT_WIN==0) Cornice(620-s_len,35,632,50+n_iter,SUPIENO);
   }
 MOUSE_LIMITS(0,0,639,479);
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

/* WAUTOM history:
			v1.0 : Finito il 28/11/1993
			       Internet release : 30/11/1993
*/
