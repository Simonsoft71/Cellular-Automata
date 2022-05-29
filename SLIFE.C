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
#include "jconio.c"

#define VIVA  1
#define MORTA ' '


#define ON_color  15
#define OFF_color  0

typedef char Gen[120+2][120+2];     /*matrice di generazione, <2> <3>*/

int DV = 25, DH = 25;
int iciclo,jciclo;
union  REGS      regs,regs2;
struct SREGS     sregs;
char *default_msg = "Test sulla mappa bit di 'e' e di 'pi'";

int GreenMenu[]= { 00,00,00,
		   58,63,58,
		   21,41,21,
		   18,30,17
		   };

/* prototipi funzioni: */

void Desk( void);
long Controllo(Gen *gp, long ng);
void GenNulla(Gen *gp);
void GenCasuale(Gen *gp);
void StampaGen(Gen *gp);
void NuovaGen(Gen *vgp, Gen *ngp);
void CollegaBordi(Gen *gp);
void StampaNumGen(long ng);

int lg=2;

int main( int argc, char *argv[] )
{
   int Drive = 9;
   int Modo  = 2;
   char item;


    Gen   g[2];       /*due generazioni successive di cellule <1>*/
    int   gcorr;      /*generazione corrente (0/1)*/
    int   gnew;       /*e nuova generazione*/
    long  ngen;       /*contatore generazioni*/



    DV = atoi(argv[1]);
    DH = atoi(argv[2]);
    if( DV==0 && DH==0) { DH=25; DV=25; }
    if( argc>2)
    {
     if( DV>100) {
		 DV=100;
		 printf("Celle verticali: al massimo 100!\n");
		 }
     if( DH>100) {
		 DH=100;
		 printf("Celle orizzontali: al massimo 100!\n");
		 }
     }
    printf("*** SLife [vert][orizz], primi un tasto per iniziare...\n");
    getch();
    srand((unsigned int)time(NULL));
    registerbgidriver( EGAVGA_driver);
    initgraph( &Drive, &Modo,"");

    Desk();
    gcorr = 0;                                      /*inizia con gen 0*/
    GenCasuale(g+gcorr);                            /*estrae cellule <3>*/
    moveto(lg,460);
    StampaGen(g+gcorr);                             /*e le mostra*/
    ngen = 0;                                       /*azzera contatore*/

    Jclrkey();                                      /*pulisce buffer*/
    do {
        ngen = Controllo(g+gcorr,ngen);             /*comando/edit <4>*/
        if (ngen != -1L) {
            gnew = 1-gcorr;                         /*indice nuova gen <5>*/
            NuovaGen(g+gcorr,g+gnew);               /*calcola nuova gen*/
            StampaGen(g+gnew);
            gcorr = gnew;                           /*cambia gen*/
            ngen++;                                 /*e la conta*/
        }
    } while (ngen != -1L);

    textmode(C80);
    return 1;
}

/* Note su main:
    <1> g[] ä un array di due elementi, ciascuno dei quali ä una matrice
        rettangolare (vedi nota 1 delle dichiarazioni). Questo ä un esempio
        di come l'uso di typedef possa migliorare molto la leggibilitÖ.
    <2> Il solito avviamento del generatore di numeri pseudocasuali.
    <3> g+gcorr equivale a: &g[gcorr], cioä all'indirizzo dell'elemento gcorr
        dell'array g. Questo elemento ä a sua volta un array bidimensionale,
        che viene quindi passato per riferimento (indirizzo del suo primo
        elemento), ed ä modificabile dalla funzione chiamata.
    <4> La Controllo consente di avanzare passo-passo o di continuo, e di
        editare la generazione corrente. Torna -1L se ä stato richiesto di
	terminare l'esecuzione del programma.
    <5> gcorr (generazione corrente) e gnew (nuova generazione) vengono
        scambiate ad ogni ciclo, e si alternano tra 0 e 1. La nuova
        generazione viene quindi calcolata alternativamente in g[0] e g[1].
*/


long Controllo(Gen *gp, long ng)
{
    enum { SP = 32, ESC = 27,                       /*codici tasti <1>*/
	   SU2 = 1072, GIU2 = 1080,
           SIN = 1075, DES = 1077,
           F1 = 1059, F5 = 1063, F6 = 1064 };
    enum { POSINFO = 15 };                          /*pos. informazioni*/

    static int  passo = 1;        /*flag di passo-passo, si ferma se vero*/

    int  pos;       /*posizione corrispondente sul video*/
    char *pc;       /*pointer a cellula corrente <3>*/
    int  c,esce;
    char buf[81];

    if (kbhit()) {                                  /*se premuto tasto <4>*/
        Jclrkey();                                  /*toglie da buffer*/
        passo = 1;                                  /*e si ferma*/
    }
    if (passo) {                                    /*se deve fermarsi*/
	/*sprintf(buf,"%s%s%s",istr1,istr2,istr3);
	*/    /*mostra istruzioni*/
        esce = 0;                                   /*esce a richiesta*/

        do {                                        /*aspetta comando*/
	    c = Jgetkey(1);
            switch(c) {                             /*esegue tasto*/
            case F1:                                /*un passo:*/
                esce = 1;
                break;
            case '\r':                              /*vai di continuo:*/
                passo = 0;
                esce = 1;
                break;
            case F5:                                /*clear:*/
            case F6:                                /*random:*/
                if (c == F5) {
                    GenNulla(gp);
                } else {
                    GenCasuale(gp);
		}
		Desk();
		lg=2;
		moveto(lg,460);
                StampaGen(gp);
                ng = 0;                             /*azzera generazione*/
                break;
            case ESC:                               /*fine:*/
                esce = 1;
                ng = -1L;
                break;
            }
        } while (! esce);
    }
    return(ng);                                     /*numero generazione*/
}

/********************************************************************
 * GenNulla: azzera la generazione *gp.
 ********************************************************************/

void GenNulla(Gen *gp)      /*<1>*/
{
    int  v;

    for (v = 1; v <= DV; v++) {                     /*per ogni riga*/
        memset((*gp)[v]+1,0,DH);                    /*la azzera <2>*/
    }
}

/* Note su GenNulla:
    <1> gp ä un pointer ad una variabile di tipo Gen, cioä una matrice di
        cellule (char).
    <2> Riempie i caratteri da 1 a DH della stringa (*gp)[v] con zeri, cioä
        azzera una riga della matrice di cellule (vedi nota 5 di Controllo).
*/


/********************************************************************
 * GenCasuale: estrae generazione casuale in *gp.
 ********************************************************************/

void GenCasuale(Gen *gp)
{
    int  estraz;    /*numero di estrazioni (cellule vive)*/
    int  v,h;

    GenNulla(gp);                                   /*azzera tutto*/
    estraz = (DH*DV)/5;                             /*20% di cellule vive*/
    while (estraz--) {                              /*estrae coordinate <1>*/
        v = 1+rand()%DV;                            /*vert. tra 1 e DV*/
        h = 1+rand()%DH;                            /*orizz. tra 1 e DH*/
        (*gp)[v][h] = 1;                            /*mette cellula viva*/
    }
}

/* Note su GenCasuale:
    <1> Piuttosto che stabilire per ogni cellula se ä viva o morta, ä molto
        pió rapido azzerarle (ucciderle) tutte, poi estrarre un certo numero
        di cellule vive in posizioni casuali (conviene se la percentuale di
        cellule vive ä bassa rispetto al totale).*/


/********************************************************************
 * NuovaGen: calcola nuova generazione *ngp a partire dalla vecchia
 *           generazione *vgp.
 ********************************************************************/

void NuovaGen(Gen *vgp, Gen *ngp)     /*<1>*/
{
    char *rprec;     /*pointer alla riga precedente <2>*/
    char *riga;      /*pointer alla riga corrente*/
    char *rsucc;     /*pointer alla riga successiva*/
    int  tprec;      /*somma valori terna precedente (a sinistra) <3>*/
    int  tcorr;      /*somma valori terna corrente*/
    int  cc;         /*cellula corrente*/
    int  nc;         /*nuova cellula*/
    int  nv;         /*numero vicini*/
    int  v,h;

    GenNulla(ngp);                                  /*azzera la nuova*/
    CollegaBordi(vgp);                              /*riporta bordi <4>*/

    for (v = 1; v <= DV; v++) {                     /*per ogni riga*/
        rprec = (*vgp)[v-1];                        /*punta a riga prec.<5>*/
        riga = (*vgp)[v];                           /*a riga corrente*/
        rsucc = (*vgp)[v+1];                        /*e a riga successiva*/
        tprec = (*rprec++)+(*riga++)+(*rsucc++);    /*tot.terna sinistra <6>*/
        tcorr = (*rprec++)+(*riga++)+(*rsucc++);    /*tot.terna corrente*/

        for (h = 1; h <= DH; h++) {                 /*per ogni colonna*/
            cc = *(riga-1);                         /*cellula corrente <7>*/
            nv = tprec+tcorr-cc;                    /*subtot. prime 2 terne*/
            tprec = tcorr;                          /*fa scorrere terne*/
            tcorr = (*rprec++)+(*riga++)+(*rsucc++);/*calcola successiva*/
            nv += tcorr;                            /*n. totale vicini*/

	    if (nv == 3) {                          /*nasce o resta <8>*/
		nc = 1;
	    } else if (nv == 2) {                   /*resta*/
                nc = cc;
            } else {                                /*muore*/
                nc = 0;
            }
            (*ngp)[v][h] = nc;                      /*cambia cellula <9>*/
        }
    }
}

/* Note su NuovaGen:
    <2> L'algoritmo di Life comporta il calcolo del numero di cellule vive
        nelle 8 caselle attorno a ciascuna cellula (i vicini). Questa
        funzione lo fa in modo efficiente, usando una tecnica per somma di
        terne (che non dettagliamo in queste note). In ogni caso, rprec deve
        puntare inizialmente alla riga precedente quella corrente, riga deve
        puntare alla riga corrente, e rsucc alla riga successiva.
    <3> tprec conterrÖ la somma dei valori delle tre cellule a sinistra di
        quella corrente. PoichÇ ogni cellula puï contenere solo 0 o 1, questo
        ä il numero dei vicini di sinistra.
    <4> La CollegaBordi riporta nella cornice attorno alla matrice di
        cellule (indici 0, DV+1, DH+1) le cellule presenti nel bordo opposto
        della matrice. Questo consente di ottenere un effetto 'toroide',
        cioä di un piano continuo: i bordi destro e sinistro si toccano, e
        cosç pure il bordo superiore ed inferiore.
    <5> (*vgp)[v-1] ä l'indirizzo del primo char della riga v-1, cioä
        &(*vgp)[v-1][0] (vedi nota 3 delle dichiarazioni).
    <6> Calcola la somma delle tre cellule di sinistra, ciascuna puntata dal
        suo pointer, ed avanza tutti e tre i pointer.
    <7> In questo momento, riga punta alla cellula a destra di quella
        corrente, che ä quindi puntata da (riga-1).
    <8> Questo ä il cuore dell'algoritmo: la decisione se far nascere o
        morire la cellula sulla base del numero di vicini.
    <9> Il valore calcolato viene scritto nella nuova generazione. Il
        procedimento ä ripetuto per tutte le cellule.
*/


/********************************************************************
 * CollegaBordi: riporta i bordi della generazione *gp accanto ai
 *               bordi opposti, per produrre un effetto toroidale
 *               (piano finito ma illimitato).
 ********************************************************************/

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

/********************************************************************
 * StampaGen: mostra generazione *gp su video.
 ********************************************************************/

void StampaGen(Gen *gp)
{
    char *riga;           /*pointer a una riga dell'array di cellule*/
    int  v,h;
    char buf[10];
    int dx=615-DH, dy=60;
    long int life=0, help=0;
    static next10=0, life10=-1, factor=1;


    next10++;
    for (v = 1; v <= DV; v++) {                     /*per ogni riga*/
	riga = (*gp)[v];  /*punta alla riga <4>*/
        for (h = 1; h <= DH; h++) {                 /*prepara linea*/
	if( riga[h]==1) {
			putpixel(dx+h,dy+v,ON_color);
			life++;
			}
		   else putpixel(dx+h,dy+v,OFF_color);
	}
    }
    if( life>400) factor=life/400;
    lineto(lg,460-life/factor);
    lg++;
    if( lg>638) {
      Desk();
      lg=2;
      moveto(lg,460);
    }
    if( (next10 % 18)==0) {
      help=460-life/factor;
      sprintf(buf,"%d",life);
      GText(lg+5,435-life/factor,1,buf);
      moveto(lg-1, help);
    }
    if( (next10 % 8)==0) {
			 if( life==life10) {
			   Beep(500,8);
			   GText(30,70,3,"Sistema stabile?");
			   }
			   else Beep(300,8);
			 life10=life;
			 }
}

void StampaNumGen(long ng)
{
    char buf[81];   /*<1>*/
    sprintf(buf,"gen: %-7ld",ng);
    outtextxy(1,25,buf);
}

void Desk( void)
{
 char buf[80];

 sprintf(buf,"Matrice di %dx%d cellule", DV, DH);
 NewColorMenu(GreenMenu);
 Win(0,0,640,480,2,3,1,"Life per Teoria dei Sistemi",0);
 Win(600-DH, 40, 630, 76+DV, 2,3,1,"Matrice",0);
 GText(10,30,0,buf);
 GText(10,45,0,"Evoluzione temporale del numero di cellule vive:");
}