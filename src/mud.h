/*
 * This is the main headerfile
 */

#ifndef MUD_H
#define MUD_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <zlib.h>
#include <stdarg.h>

#include "list.h"
#include "stack.h"

/************************
 * Standard definitions *
 ************************/

/* define TRUE and FALSE */
#ifndef FALSE
#define FALSE   0
#endif
#ifndef TRUE
#define TRUE    1
#endif

/* A few globals */
#define PULSES_PER_SECOND    10                   /* must divide 1000 : 4, 5 or 8 works */
#define MAX_BUFFER         2000                   /* seems like a decent amount         */
#define MAX_INPUT_LEN      2000
#define MAX_OUTPUT        20000                   /* well shoot me if it isn't enough   */
#define MAX_HELP_ENTRY     4096                   /* roughly 40 lines of blocktext      */
#define MUDPORT            9009                   /* just set whatever port you want    */
#define FILE_TERMINATOR    "EOF"                  /* end of file marker                 */
#define COPYOVER_FILE      "../txt/copyover.dat"  /* tempfile to store copyover data    */
#define EXE_FILE           "../src/SocketMud"     /* the name of the mud binary         */

/* Connection states */
#define STATE_NEW_NAME         0
#define STATE_NEW_PASSWORD     1
#define STATE_VERIFY_PASSWORD  2
#define STATE_ASK_PASSWORD     3
#define STATE_PLAYING          4
#define STATE_CLOSED           5

/* Thread states - please do not change the order of these states    */
#define TSTATE_LOOKUP          0  /* Socket is in host_lookup        */
#define TSTATE_DONE            1  /* The lookup is done.             */
#define TSTATE_WAIT            2  /* Closed while in thread.         */
#define TSTATE_CLOSED          3  /* Closed, ready to be recycled.   */

/* player levels */
#define LEVEL_GUEST            1  /* Dead players and actual guests  */
#define LEVEL_PLAYER           2  /* Almost everyone is this level   */
#define LEVEL_ADMIN            3  /* Any admin without shell access  */
#define LEVEL_GOD              4  /* Any admin with shell access     */

/* Communication Ranges */
#define COMM_LOCAL             0  /* same room only                  */
#define COMM_LOG              10  /* admins only                     */

/* define simple types */
typedef  unsigned char     bool;
typedef  short int         sh_int;


/******************************
 * End of standard definitons *
 ******************************/

/***********************
 * Defintion of Macros *
 ***********************/

#define IS_ADMIN(dMob)          ((dMob->level) > LEVEL_PLAYER ? TRUE : FALSE)

#define IREAD(sKey, sPtr)             \
{                                     \
  if (!strcasecmp(sKey, word))        \
  {                                   \
    int sValue = fread_number(fp);    \
    sPtr = sValue;                    \
    found = TRUE;                     \
    break;                            \
  }                                   \
}

#define SREAD(sKey, sPtr)             \
{                                     \
  if (!strcasecmp(sKey, word))        \
  {                                   \
    sPtr = fread_string(fp);          \
    found = TRUE;                     \
    break;                            \
  }                                   \
}


/***********************
 * End of Macros       *
 ***********************/

/******************************
 * New structures             *
 ******************************/

/* type defintions */
typedef struct  dSocket       D_SOCKET;
typedef struct  dMobile       D_MOBILE;
typedef struct  help_data     HELP_DATA;
typedef struct  lookup_data   LOOKUP_DATA;
typedef struct  event_data    EVENT_DATA;

// MTH 1.5 type definitions

typedef struct  mud_data      MUD_DATA;
typedef struct  mth_data      MTH_DATA;

/* the actual structures */

struct dSocket
{
  D_MOBILE      * player;
  MTH_DATA      * mth; // MTH 1.5
  LIST          * events;
  char          * hostname;
  char            inbuf[MAX_BUFFER];
  char            outbuf[MAX_OUTPUT];
  char            telbuf[MAX_BUFFER];
  char            next_command[MAX_BUFFER];
  bool            bust_prompt;
  int             flags;
  sh_int          lookup_status;
  sh_int          state;
  int             control;
  int             top_output;
};

struct dMobile
{
  D_SOCKET      * socket;
  LIST          * events;
  char          * name;
  char          * password;
  sh_int          level;
};

struct help_data
{
  time_t          load_time;
  char          * keyword;
  char          * text;
};

struct typCmd
{
  char      * cmd_name;
  void     (* cmd_funct)(D_MOBILE *dMOb, char *arg);
  sh_int      level;
};

typedef struct buffer_type
{
  char   * data;        /* The data                      */
  int      len;         /* The current len of the buffer */
  int      size;        /* The allocated size of data    */
} BUFFER;

/* here we include external structure headers */
#include "event.h"

/******************************
 * End of new structures      *
 ******************************/

/***************************
 * Global Variables        *
 ***************************/

extern  STACK       *   dsock_free;       /* the socket free list               */
extern  LIST        *   dsock_list;       /* the linked list of active sockets  */
extern  STACK       *   dmobile_free;     /* the mobile free list               */
extern  LIST        *   dmobile_list;     /* the mobile list of active mobiles  */
extern  LIST        *   help_list;        /* the linked list of help files      */
extern  const struct    typCmd tabCmd[];  /* the command table                  */
extern  bool            shut_down;        /* used for shutdown                  */
extern  char        *   greeting;         /* the welcome greeting               */
extern  char        *   motd;             /* the MOTD help file                 */
extern  int             control;          /* boot control socket thingy         */
extern  time_t          current_time;     /* let's cut down on calls to time()  */

/*************************** 
 * End of Global Variables *
 ***************************/

/***********************
 *    MCCP support     * MTH 1.5 Obsolete
 ***********************/
/*
extern const unsigned char compress_will[];
extern const unsigned char compress_will2[];

#define TELOPT_COMPRESS       85
#define TELOPT_COMPRESS2      86

#define COMPRESS_BUF_SIZE  10000
*/
/***********************
 * End of MCCP support *
 ***********************/

/***********************************
 * Prototype function declerations *
 ***********************************/

/* more compact */
#define  D_S         D_SOCKET
#define  D_M         D_MOBILE

#define  buffer_new(size)             __buffer_new     ( size)
#define  buffer_strcat(buffer,text)   __buffer_strcat  ( buffer, text )

char  *crypt                  ( const char *key, const char *salt );

/*
 * socket.c
 */
int   init_socket             ( void );
bool  new_socket              ( int sock );
void  close_socket            ( D_S *dsock, bool reconnect );
bool  read_from_socket        ( D_S *dsock );
bool  text_to_socket          ( D_S *dsock, const char *txt, int length );  /* sends the output directly */
void  text_to_buffer          ( D_S *dsock, const char *txt );  /* buffers the output        */
void  text_to_mobile          ( D_M *dMob, const char *txt );   /* buffers the output        */
void  next_cmd_from_buffer    ( D_S *dsock );
bool  flush_output            ( D_S *dsock );
void  handle_new_connections  ( D_S *dsock, char *arg );
void  clear_socket            ( D_S *sock_new, int sock );
void  recycle_sockets         ( void );
void *lookup_address          ( void *arg );

/*
 * interpret.c
 */
void  handle_cmd_input        ( D_S *dsock, char *arg );

/*
 * io.c
 */
void    log_descriptor_printf ( D_S *d, char *fmt, ...);
void    log_string            ( const char *txt, ... );
void    bug                   ( const char *txt, ... );
time_t  last_modified         ( char *helpfile );
char   *read_help_entry       ( const char *helpfile );     /* pointer         */
char   *fread_line            ( FILE *fp );                 /* pointer         */
char   *fread_string          ( FILE *fp );                 /* allocated data  */
char   *fread_word            ( FILE *fp );                 /* pointer         */
int     fread_number          ( FILE *fp );                 /* just an integer */

/* 
 * strings.c
 */
char   *one_arg               ( char *fStr, char *bStr );
char   *strdup                ( const char *s );
int     strcasecmp            ( const char *s1, const char *s2 );
bool    is_prefix             ( const char *aStr, const char *bStr );
char   *capitalize            ( char *txt );
BUFFER *__buffer_new          ( int size );
void    __buffer_strcat       ( BUFFER *buffer, const char *text );
void    buffer_free           ( BUFFER *buffer );
void    buffer_clear          ( BUFFER *buffer );
int     bprintf               ( BUFFER *buffer, char *fmt, ... );

/*
 * help.c
 */
bool  check_help              ( D_M *dMob, char *helpfile );
void  load_helps              ( void );

/*
 * utils.c
 */
bool  check_name              ( const char *name );
void  clear_mobile            ( D_M *dMob );
void  free_mobile             ( D_M *dMob );
void  communicate             ( D_M *dMob, char *txt, int range );
void  load_muddata            ( bool fCopyOver );
char *get_time                ( void );
void  copyover_recover        ( void );
D_M  *check_reconnect         ( char *player );


/*
 * action_safe.c
 */
void  cmd_say                 ( D_M *dMob, char *arg );
void  cmd_quit                ( D_M *dMob, char *arg );
void  cmd_shutdown            ( D_M *dMob, char *arg );
void  cmd_commands            ( D_M *dMob, char *arg );
void  cmd_who                 ( D_M *dMob, char *arg );
void  cmd_help                ( D_M *dMob, char *arg );
void  cmd_compress            ( D_M *dMob, char *arg );
void  cmd_save                ( D_M *dMob, char *arg );
void  cmd_copyover            ( D_M *dMob, char *arg );
void  cmd_linkdead            ( D_M *dMob, char *arg );

/*
 * mccp.c MTH 1.5 obsolete
 */
/*
bool  compressStart           ( D_S *dsock, unsigned char teleopt );
bool  compressEnd             ( D_S *dsock, unsigned char teleopt, bool forced );
*/
/*
 * save.c
 */
void  save_player             ( D_M *dMob );
D_M  *load_player             ( char *player );
D_M  *load_profile            ( char *player );



/*****************************************************************************
 *                                                                           *
 * MTH 1.5                                                                   *
 *                                                                           *
 *****************************************************************************/

// Bitvectors

#define BV01            (1   <<  0)  
#define BV02            (1   <<  1) 
#define BV03            (1   <<  2) 
#define BV04            (1   <<  3)   
#define BV05            (1   <<  4)    
#define BV06            (1   <<  5)
#define BV07            (1   <<  6)
#define BV08            (1   <<  7)
#define BV09            (1   <<  8)
#define BV10            (1   <<  9)

#define HAS_BIT(var, bit)       ((var)  & (bit))
#define SET_BIT(var, bit)       ((var) |= (bit))
#define DEL_BIT(var, bit)       ((var) &= (~(bit)))
#define TOG_BIT(var, bit)       ((var) ^= (bit))


MUD_DATA *mud;

//	Mud data, structure containing global variables.

struct mud_data
{
	int                   boot_time;
	int                   port;
	int                   total_plr;
	int                   msdp_table_size;
	int                   mccp_len;
	unsigned char       * mccp_buf;
};

struct mth_data
{
	struct msdp_data ** msdp_data;
	char              * proxy;
	char              * terminal_type;
	char                telbuf[MAX_INPUT_LEN];
	int                 teltop;
	long long           mtts;
	int                 comm_flags;
	short               cols;
	short               rows;
	z_stream          * mccp2;
	z_stream          * mccp3;
};

/*
	telopt.c
*/

void        announce_support         ( D_S *d );
int         translate_telopts        ( D_S *d, unsigned char *src, int srclen, unsigned char *out, int outlen );
int         write_mccp2              ( D_S *d, char *txt, int length );
void        end_mccp2                ( D_S *d );
void        end_mccp3                ( D_S *d );
void        send_echo_on             ( D_S *d );
void        send_echo_off            ( D_S *d );


/*
	mth.c
*/

void        init_mth(void);
void        init_mth_socket( D_S *d );
void        uninit_mth_socket( D_S *d );
int         total_players           ( void );
int         cat_sprintf              ( char *dest, char *fmt, ... );
void        arachnos_devel           ( char *fmt, ... );
void        arachnos_mudlist         ( char *fmt, ... );

/*
	msdp.c
*/

void        init_msdp_table               ( void );
void        msdp_update_var               ( D_S *d, char *var, char *fmt, ... );
void        msdp_update_var_instant       ( D_S *d, char *var, char *fmt, ... );

/*
	color.c
*/

int substitute_color(char *input, char *output, int colors);

#endif /* MUD_H */
