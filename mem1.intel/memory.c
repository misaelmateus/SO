
/****************************************************************************/
/*                                                                          */
/* 			     Module MEMORY                                  */
/* 			External Declarations 	                            */
/*                                                                          */
/****************************************************************************/


/* OSP constants */

#define MAX_PAGE       16                 /* max size of page tables        */
#define MAX_FRAME      32                 /* size of the physical memory    */
#define PAGE_SIZE      512                /* size of a page in bytes        */

#define   COST_OF_PAGE_TRANSFER      6  /* cost of reading page  from drum  */


/* external enumeration constants */

typedef enum {
    false, true                         /* the boolean data type            */
} BOOL;

typedef enum {
    read, write                         /* type of actions for I/O requests */
} IO_ACTION;

typedef enum {
    load, store                         /* types of memory reference        */
} REFER_ACTION;

typedef enum {
    running, ready, waiting, done       /* types of status                  */
} STATUS;

typedef enum {
    iosvc, devint,                      /* types of interrupt               */
    pagefault, startsvc,
    termsvc, killsvc,
    waitsvc, sigsvc, timeint
} INT_TYPE;



/* external type definitions */

typedef struct page_entry_node PAGE_ENTRY;
typedef struct page_tbl_node PAGE_TBL;
typedef struct event_node EVENT;
typedef struct ofile_node OFILE;
typedef struct pcb_node PCB;
typedef struct iorb_node IORB;
typedef struct int_vector_node INT_VECTOR;
typedef struct frame_node FRAME;



/* external data structures */

extern struct frame_node {
    BOOL   free;        /* = true, if free                                  */
    PCB    *pcb;        /* process to which the frame currently belongs     */
    int    page_id;     /* vitrual page id - an index to the PCB's page tbl */
    BOOL   dirty;       /* indicates if the frame has been modified         */
    int    lock_count;  /* number of locks set on page involved in an       */
                        /* active I/O                                       */
    int    *hook;       /* can hook up anything here                        */
};

extern struct page_entry_node {
    int    frame_id;    /* frame id holding this page                       */
    BOOL   valid;       /* page in main memory : valid = true; not : false  */
    int    *hook;       /* can hook up anything here                        */
};

extern struct page_tbl_node {
    PCB    *pcb;        /* PCB of the process in question                   */
    PAGE_ENTRY page_entry[MAX_PAGE];
    int    *hook;       /* can hook up anything here                        */
};

extern struct pcb_node {
    int    pcb_id;         /* PCB id                                        */
    int    size;           /* process size in bytes; assigned by SIMCORE    */
    int    creation_time;  /* assigned by SIMCORE                           */
    int    last_dispatch;  /* last time the process was dispatched          */
    int    last_cpuburst;  /* length of the previous cpu burst              */
    int    accumulated_cpu;/* accumulated CPU time                          */
    PAGE_TBL *page_tbl;    /* page table associated with the PCB            */
    STATUS status;         /* status of process                             */
    EVENT  *event;         /* event upon which process may be suspended     */
    int    priority;       /* user-defined priority; used for scheduling    */
    PCB    *next;          /* next pcb in whatever queue                    */
    PCB    *prev;          /* previous pcb in whatever queue                */
    int    *hook;          /* can hook up anything here                     */
};

extern struct iorb_node {
    int    iorb_id;     /* iorb id                                          */
    int    dev_id;      /* associated device; index into the device table   */
    IO_ACTION action;   /* read/write                                       */
    int    block_id;    /* block involved in the I/O                        */
    int    page_id;     /* buffer page in the main memory                   */
    PCB    *pcb;        /* PCB of the process that issued the request       */
    EVENT  *event;      /* event used to synchronize processes with I/O     */
    OFILE  *file;       /* associated entry in the open files table         */
    IORB   *next;       /* next iorb in the device queue                    */
    IORB   *prev;       /* previous iorb in the device queue                */
    int    *hook;       /* can hook up anything here                        */
};

extern struct int_vector_node {
    INT_TYPE cause;           /* cause of interrupt                         */
    PCB    *pcb;              /* PCB to be started (if startsvc) or pcb that*/
			      /* caused page fault (if fagefault interrupt) */
    int    page_id;           /* page causing pagefault                     */
    int    dev_id;            /* device causing devint                      */
    EVENT  *event;            /* event involved in waitsvc and sigsvc calls */
    IORB   *iorb;             /* IORB involved in iosvc call                */
};



/* extern variables */

extern INT_VECTOR Int_Vector;           /* interrupt vector         	     */
extern PAGE_TBL *PTBR;                  /* page table base register 	     */
extern FRAME Frame_Tbl[MAX_FRAME];      /* frame table              	     */
extern int Prepage_Degree;		/* global degree of prepaging (0-10) */



/* external routines */

extern siodrum(/* action, pcb, page_id, frame_id */);
/*  IO_ACTION   action;
    PCB         *pcb; 
    int         page_id, frame_id;  */
extern int get_clock();
extern gen_int_handler();





/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*                              Module MEMORY                               */
/*                            Internal Routines                             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

typedef struct Pagina{
	int id;
	struct Pagina *next;
	struct Pagina *prev;
}Pagina;

typedef struct Fila{
	int tamanho;
	Pagina *inicio;
	Pagina *fim;
}Fila;

Fila PTFila;

Pagina *newPage(int id){
  Pagina *page = (Pagina *)malloc(sizeof(Pagina));
  if( page == NULL)
      exit(EXIT_FAILURE);
  page->id = id;
  return page;
}

void insereFrame(int id){
    Pagina *page = newPage(id);
    if(PTFila.tamanho == 0){ // se a fila estiver vazia faz o nó ser o primeiro, onde o inicio e fim aponta para o mesmo no
		page->next = page;
		page->prev = page;
		PTFila.inicio = page; // faz o inicio apontar para o novo nó
		PTFila.fim = page; // faz o fim apontar para o novo nó
		PTFila.tamanho=1;
	}else{ // se a fila tiver pelo menos um elemento então vou fazer apenas o novo nó apontar para o inicio e o apontar para o penultimo
		page->next = PTFila.inicio; // faz o proximo do novo nó apontar para o inicio
		page->prev = PTFila.fim; // faz o anterior do novo nó apontar para o fim
		page->next->prev = page; // faz inicio apontar para o novo fim
		page->prev->next = page;  // faz o antigo fim apontar para o novo nó
		PTFila.fim = page; // faz o novo nó se tornar o fim
		PTFila.tamanho++; // aumenta o tamanho da fila
	}
}

int removeFrame(){
	Pagina *page;
	page = PTFila.inicio;
	if( PTFila.tamanho == 1){
		PTFila.fim = NULL;
		PTFila.inicio = NULL;
		PTFila.tamanho = 0;
	}else{
		page->prev->next = page->next; // faz o ultimo elemento apontar para o proximo do inicio
		page->next->prev = page->prev; // faz proximo do inicio apontar para o fim
		PTFila.inicio = page->next; // faz o inicio da fila ser o proximo do inicio
		PTFila.tamanho--;
	}
	return page->id;
}

int segundaChance(){
    int page_id = removeFrame();
    while( *(Frame_Tbl[page_id].hook) != 0 || Frame_Tbl[page_id].lock_count > 0){
      *(Frame_Tbl[page_id].hook)  = 0;
      insereFrame(page_id);
      page_id = removeFrame();
    }
    *(Frame_Tbl[page_id].hook) = 1;
    return page_id;
}

void memory_init() // serve para iniciar as estruturas que serão utilizadas
{
	int i=0;
	while( i < MAX_FRAME){ // Frames não referenciados recentemente... Bit R com 0
	  Frame_Tbl[i].hook = (int *)malloc(sizeof(int));
	  if(Frame_Tbl[i].hook == NULL)
	       exit(EXIT_FAILURE);
	  *(Frame_Tbl[i].hook) = 0;
	  i++;
	}
}



void prepage(pcb)
PCB *pcb;
{
// como é por demanda então não precisa implementar já que o manual diz que por default eh por demanda se tiver em branco
// implementar so se a prepaging estiver implementada
}



int start_cost(pcb)
PCB *pcb;
{

}



void deallocate(pcb)
PCB *pcb;
{

}



void get_page(pcb,page_id)
PCB *pcb;
int page_id;
{

}



void lock_page(iorb)
IORB *iorb;
{

}



void unlock_page(iorb)
IORB  *iorb;
{

}



void refer(logic_addr,action)
int logic_addr;
REFER_ACTION action;
{

}

/* end of module */
