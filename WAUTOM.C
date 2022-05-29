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

#define BOT_IT_WIN 420   /* Bottom of iteration window */
#define LIM_DIM_WIN 380  /* Limite Win Dinamica */
#define MAXITER 30000
#define MAXLEN 215   /* Lunghezza massima dello 'stream'
			(lo 'stream' e' la successione delle cellule */

/* Dichiarazioni external per libreria <grafdsp.h> */
int    iciclo,jciclo;
union  REGS      regs,regs2;
struct SREGS     sregs;
char *default_msg = "Automi cellulari elementari (1-dimensionali) binari. By Simone Maggi";

int BluMenu[] = { 00,00,00,
		  48,56,63,
		  37,38,59,
		  18,0,42
		  };

/* Prototipi delle funzioni */

void Gen_wbit   ( int wnum, int w_bit[8]);
void Start_Gen  ( int gen_type, int s_len, unsigned char *stream);
unsigned
char SWProb 	( float p);
void Out_Gen    ( int time, int s_len, unsigned char *stream);
void itera_rule ( int s_len, int w_bit[8], unsigned char *stream);
void Itera	( int gen_t, int w_num, int *w_bit, unsigned char *stream);
void Atm_Show	( int beg_n, int end_n, int *w_bit, unsigned char *stream);
void PSlide	( int wn );
void Field	( int x1, int y1, int len, char *titolo, char *text );
void Desk	( void);
void Group	( int x1, int y1, int x2, int y2, char *text);

int n_iter = 410;
int s_len = 200;     /* Lunghezza stream, 30 di default */
int prob = 0;	     /* Densita' uniforme con p=..., default 0 (p=0.5) */
int cyclic = 0;

char *MenuStart[] = { "Un \"1\" a met…",
		      "Un \"1\" a caso",
		      "Tutti \"1\"",
		      "Random p=0.5",
		      "Random p=0.xx",
		      "1í1í1í1í...",
		      "1íí1íí1íí..",
		      "11íí11íí..."
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
 int w_num = 90;     /* Numero di Wolfram di default */
 int gen_t = 1;      /* Start gen_type = random di default */
 int w_bit[8] = { 0,0,0,0,1,1,1,1 };
 int i, j, item, gent_hlp, prob_hlp;
 unsigned char *stream;

struct Gitem S[]  = { 10, 405, "W-",
		      150, 405, "W+",
		      5,  5,  "Quit",
		      212, 405, "Nuovo",
		      14,  365, "Start gen >",
		      14,  320, "Window",
		      14,  275, "N Iter",
		      64,  405, "      ",        /*"Scroll" bar button*/
		      210, 320, "Cyclic" };
 void *Menu=NULL;
 char buf[15] = "";


 if( INIT_MOUSE()==1) {
  printf("\nSpiacente, per questo programma serve il MOUSE\
  (comprane uno tirchione!)...");
  exit(0);
  }

 registerbgidriver( EGAVGA_driver);
 initgraph( &Drive, &Modo,"");
 MOUSE_LIMITS(0,0,639,479);
 MOUSECURSOR(1);
 Desk();
 for (i=0; i<=8; i++) GENbutton(S[i].x, S[i].y, S[i].Gi, BTmask);
 Cornice(64,405,144,430,GIU);
 PSlide(w_num);
 sprintf( buf, "%d", n_iter);
 Field( S[6].x+100, S[6].y+2, 6, "Attuale:", buf);
 sprintf( buf, "%d", s_len);
 Field( S[5].x+100, S[5].y+2, 6, "Attuale:", buf);


 stream = (char *) malloc (s_len+4);
 Itera( gen_t, w_num, w_bit, stream);
 do { /*-Main loop-*/
  item = SELECTbutton( S, 9);
  if( item==1) {
		 w_num--;
		 if( w_num<0) w_num=255;
		 PSlide(w_num);
		 Itera(gen_t, w_num, w_bit, stream);
		 } else
  if( item==2) {
		 w_num++;
		 if( w_num>255) w_num=0;
		 PSlide(w_num);
		 Itera(gen_t, w_num, w_bit, stream);
		 } else
  if( item==4) {
		Itera(gen_t, w_num, w_bit, stream);
		} else
  if( item==5) {
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
		Itera(gen_t, w_num, w_bit, stream);
	       } else
  if( item==6)
	      {
	       BarraHelp("Immetti la dimensione della nuova finestra...");
	       do {
		itoa(s_len,buf,10);
		strcpy(buf,XInput(S[5].x+102,S[5].y+16,0,2,buf,4));
		s_len = atoi(buf);
		if( s_len > MAXLEN-5) BarraHelp("Massima dimesione stringa: 210!");
	       } while (s_len > MAXLEN-5);
	       Desk();
	       MOUSE_LIMITS(0,0,639,479);
	       MOUSECURSOR(1);
	       Desk();
	       Field( S[5].x+100, S[5].y+2, 6, "Attuale:", buf);
	       sprintf( buf, "%d", n_iter);
	       Field( S[6].x+100, S[6].y+2, 6, "Attuale:", buf);
	       for (i=0; i<=7; i++) GENbutton(S[i].x, S[i].y, S[i].Gi, BTmask);
	       GENbutton(S[8].x, S[8].y, S[8].Gi, BTmask+cyclic);
	       Cornice(64,405,144,430,GIU);
	       PSlide(w_num);
	       Itera(gen_t, w_num, w_bit, stream);
	       } else
  if( item==7) {
	       BarraHelp("Immetti il numero di iterazioni tra 1 e 30000");
	       do {
		itoa(n_iter,buf,10);
		strcpy(buf,XInput(S[6].x+102,S[6].y+16,0,2,buf,5));
		n_iter = abs(atoi(buf));
	       } while (n_iter > MAXITER);
	       Desk();
	       MOUSE_LIMITS(0,0,639,479);
	       MOUSECURSOR(1);
	       Desk();
	       Field( S[6].x+100, S[6].y+2, 6, "Attuale:", buf);
	       sprintf( buf, "%d", s_len);
	       Field( S[5].x+100, S[5].y+2, 6, "Attuale:", buf);
	       for (i=0; i<=7; i++) GENbutton(S[i].x, S[i].y, S[i].Gi, BTmask);
	       GENbutton(S[8].x, S[8].y, S[8].Gi, BTmask+cyclic);
	       Cornice(64,405,144,430,GIU);
	       PSlide(w_num);
	       Itera(gen_t, w_num, w_bit, stream);
	       } else
  if( item==8) { /*Slide bar ++4*/
		w_num += 15;
		if( w_num>255) w_num=0;
		PSlide(w_num);
		Itera(gen_t, w_num, w_bit, stream);
		}
  if( item==9) {
		cyclic=1-cyclic;
		GENbutton(S[8].x, S[8].y, S[8].Gi, BTmask+cyclic);
                Itera(gen_t, w_num, w_bit, stream);
		}
  } while( item!=3 || kbhit()==1);
  textmode(C80);
  printf("Grazie per aver usato WAUTOM e arrivederci...\n");
  printf("L'autore Š raggiungibile su Internet all'indirizzo: magi@ghost.dsi.unimi.it");
 return 0;
}



void Gen_wbit(int wnum, int w_bit[8]) /*Genera il binario del numero W*/
{
    char buf[80];                     		/*buffer per messaggio*/
    char digit[] = "0123456789";            /*validi per tutte le basi*/
    int i, wn = wnum;

    for (i=0; i<=7; i++) w_bit[i] = 0;

    i = 0;
    while (wnum > 0) {
	    w_bit[i] = wnum%2;
	    i++;
	    wnum /= 2;
	}
    sprintf(buf," Regola Wolfram %d: %d%d%d%d%d%d%d%d",wn,
    w_bit[7], w_bit[6],
    w_bit[5], w_bit[4],
    w_bit[3], w_bit[2],
    w_bit[1], w_bit[0]);
    Cornice(8,440,330,460,SUPIENO);
    GText(10,446,1,buf);
}

void Start_Gen(int gen_type, int s_len, unsigned char *stream)
{
 int i;
 char buf[50];
 float Dens[] = { 0.5,
		  0.01, 0.02, 0.03, 0.05, 0.10,
		  0.20, 0.40, 0.80, 0.85, 0.95, 0.99 };
 /*
 1- Un 1 a met…,
 2- Un 1 a caso,
 3- Tutti 1,
 4- Random,
 5- Uniforme con p=...
 6- 1í1í1í1í...
 7- 1íí1íí1íí..
 8- 11íí11íí...
 */

 strcpy( buf, MenuStart[gen_type-1]);
 Field(155, 366, 15, "Attuale:", buf);
 stream[0]=0;
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
 int i, ncell_on = 0;
 int dx = 625-s_len, dy = 40;
 int dxd = 10, dyd = 260;
 double ro = 0;
 char buf[25];

/* printf("\centerline{%d ", time-1); */
 if( time%LIM_DIM_WIN==0) Cornice(8,45,400,260,SUPIENO);
 for (i=1; i<=s_len; i++) {
	if( stream[i]==1) {
	/* printf("*");	*/
	ncell_on++;
	} /* else printf("."); */
	putpixel(dx+i, dy+time%BOT_IT_WIN, stream[i]);
 /* Stampa una stringa di bit come pixel */
 }
 ro = (double)ncell_on/(double)s_len;
 sprintf(buf,"ro=%1.4f ",ro);
 Cornice(310,418,405,430,GIUPIENO);
 GText(315,420,3,buf);
 putpixel(dxd+time%LIM_DIM_WIN,dyd-ncell_on-1,0);
/* printf("}\n"); */
}


void itera_rule( int s_len, int w_bit[8], unsigned char *stream)
{
 int punt = cyclic;
 int a, b, c, risp;
 unsigned char ng[MAXLEN];

 ng[0]=0;

 /* Inizio iperpiano */
 if( cyclic) {
	a = stream[s_len];
	b = stream[punt];
	c = stream[punt+1];
	risp = w_bit[ a*4+b*2+c ];
	ng[punt] = risp;
 }

 do{
 a = stream[punt];
 b = stream[punt+1];
 c = stream[punt+2];
 risp = w_bit[ a*4+b*2+c ];
 ng[punt+1] = risp;
 punt++;
 } while (punt <= s_len-cyclic);

 /* "Fine" iperpiano */
 if( cyclic) {
	 a = stream[s_len-1];
	 b = stream[s_len];
	 c = stream[1];
	 risp = w_bit[ a*4+b*2+c ];
	 ng[s_len] = risp;
 }

 for (punt=1; punt<=s_len; punt++)
 stream[punt] = ng[punt];
}

void Desk( void)
{

 NewColorMenu(BluMenu);
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

void Itera( int gen_t, int w_num, int *w_bit, unsigned char *stream)
{
 int i, time=1;

 Cornice(620-s_len,35,632,50+n_iter,SUPIENO);
 Cornice(8,45,400,260,SUPIENO);
 Start_Gen(gen_t, s_len, stream);
 Out_Gen( time ,s_len, stream);
 Gen_wbit( w_num, w_bit);
 MOUSE_LIMITS(0,0,600-s_len,479);
 for (i=1; i<=n_iter; i++)
  {
   time++;
   if( IN_MOUSE(0,0,999,999,1)==0
   || IN_MOUSE(0,0,999,999,1)==1) break;
   itera_rule(s_len, w_bit, stream);
   Out_Gen( time, s_len, stream);
   if( time%BOT_IT_WIN==0) Cornice(620-s_len,35,632,50+n_iter,SUPIENO);
   }
 MOUSE_LIMITS(0,0,639,479);
}

void PSlide( int wn )
{
 int bl = 66;  /*Big-left        >bl±±±±±±±±±±sl±br±<  */
 int sl = 15;  /*Slide lungh.*/
 int br = 142; /*Big-right*/
 int ls = br-bl-sl+1, x=bl;

 Cornice(64,405,144,430,GIUPIENO);
 x = wn*ls/256;
 Cornice(bl+x,408, bl+x+sl, 428, SU);
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