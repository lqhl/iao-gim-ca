#ifndef GTP_H
#define GTP_H

#include <stdarg.h>
#include <stdio.h>
#include <fstream>

using namespace std;

#define EMPTY        SG_EMPTY
#define WHITE        SG_WHITE
#define BLACK        SG_BLACK

/* Maximum allowed line length in GTP. */
#define GTP_BUFSIZE 1000

/* Status returned from callback functions. */
#define GTP_QUIT    -1
#define GTP_OK       0
#define GTP_FATAL    1

/* Whether the GTP command was successful. */
#define GTP_SUCCESS  0
#define GTP_FAILURE  1

/* Function pointer for callback functions. */
typedef int (*gtp_fn_ptr)(char *s);

/* Function pointer for vertex transform functions. */
typedef void (*gtp_transform_ptr)(int ai, int aj, int *bi, int *bj);

extern ofstream fout;

/* Elements in the array of commands required by gtp_main_loop. */
struct gtp_command {
  const char *name;
  gtp_fn_ptr function;
};

void gtp_main_loop(struct gtp_command commands[], FILE *gtp_input,
		   FILE *gtp_dump_commands);
void gtp_internal_set_boardsize(int size);
void gtp_set_vertex_transform_hooks(gtp_transform_ptr in,
				    gtp_transform_ptr out);
void gtp_mprintf(const char *format, ...);
void gtp_printf(const char *format, ...);
void gtp_start_response(int status);
int gtp_finish_response(void);
int gtp_success(const char *format, ...);
int gtp_failure(const char *format, ...);
void gtp_panic(void);
int gtp_decode_color(char *s, int *color);
int gtp_decode_coord(char *s, int *m, int *n);
int gtp_decode_move(char *s, int *color, int *i, int *j);
void gtp_print_vertices(int n, int movei[], int movej[]);
void gtp_print_vertex(int i, int j);

#endif