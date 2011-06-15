#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cstdio>

#include "GoBoard.h"
#include "SgPoint.h"
#include "SgBlackWhite.h"
#include "util/util.h"
#include "SuperGoGame.h"
#include "SuperGo.h"
#include "gtp.h"

using namespace std;

/* Forward declarations. */
static int gtp_protocol_version(char *s);
static int gtp_name(char *s);
static int gtp_version(char *s);
static int gtp_known_command(char *s);
static int gtp_list_commands(char *s);
static int gtp_quit(char *s);
static int gtp_boardsize(char *s);
static int gtp_clear_board(char *s);
static int gtp_komi(char *s);
static int gtp_fixed_handicap(char *s);
static int gtp_place_free_handicap(char *s);
static int gtp_set_free_handicap(char *s);
static int gtp_play(char *s);
static int gtp_genmove(char *s);
static int gtp_final_score(char *s);
static int gtp_final_status_list(char *s);
static int gtp_showboard(char *s);

ofstream fout("super-go-gtp-dump.txt");
int steps = 0;

/* List of known commands. */
static struct gtp_command commands[] = {
	{"protocol_version",		gtp_protocol_version},
	{"name",								gtp_name},
	{"version",						 gtp_version},
	{"known_command",				gtp_known_command},
	{"list_commands",				gtp_list_commands},
	{"quit",						 		gtp_quit},
	{"boardsize",						gtp_boardsize},
	{"clear_board",					gtp_clear_board},
	{"komi",						gtp_komi},
	{"fixed_handicap",	 		gtp_fixed_handicap},
	{"place_free_handicap", gtp_place_free_handicap},
	{"set_free_handicap",	 gtp_set_free_handicap},
	{"play",								gtp_play},
	{"genmove",						 gtp_genmove},
	{"final_score",				 gtp_final_score},
	{"final_status_list",	 gtp_final_status_list},
	{"showboard",						gtp_showboard},
	{NULL,									NULL}
};

SuperGoGame* game;

int
pk_main()
{
	/* Make sure that stdout is not block buffered. */
	setbuf(stdout, NULL);

	/* Process GTP commands. */
	FILE* outFile = fopen("commands.txt", "w");

	/* Inform the GTP utility functions about the initial board size. */
	gtp_internal_set_boardsize(SuperGoGame::BOARD_SIZE);

	gtp_main_loop(commands, stdin, outFile);

	return 0;
}

bool isInit = false;

void init() {
	if (!isInit) {
		isInit = true;
		remove("super-go-log.txt");
		Util::init("super-go.config");
		UctPatterns::init();

		game = new SuperGoGame();
		game->init();
		UCTTree::rand.seed(100);
	}
}

/* We are talking version 2 of the protocol. */
static int
gtp_protocol_version(char *s)
{
	return gtp_success("2");
}

static int
gtp_name(char *s)
{
	return gtp_success("SuperGo");
}

static int
gtp_version(char *s)
{
	return gtp_success(VERSION_STRING);
}

static int
gtp_known_command(char *s)
{
	int i;
	char command_name[GTP_BUFSIZE];

	/* If no command name supplied, return false (this command never
	 * fails according to specification).
	 */
	if (sscanf(s, "%s", command_name) < 1)
		return gtp_success("false");

	for (i = 0; commands[i].name; i++)
		if (!strcmp(command_name, commands[i].name))
			return gtp_success("true");

	return gtp_success("false");
}

static int
gtp_list_commands(char *s)
{
	int i;

	gtp_start_response(GTP_SUCCESS);

	for (i = 0; commands[i].name; i++)
		gtp_printf("%s\n", commands[i].name);

	gtp_printf("\n");
	return GTP_OK;
}

static int
gtp_quit(char *s)
{
	gtp_success("");
	return GTP_QUIT;
}

static int
gtp_boardsize(char *s)
{
	int boardsize;

	if (sscanf(s, "%d", &boardsize) < 1)
		return gtp_failure("boardsize not an integer");

	// TODO

	return gtp_success("");
}

static int
gtp_clear_board(char *s)
{
	// TODO
	return gtp_success("");
}

static int
gtp_komi(char *s)
{
	// TODO

	return gtp_success("");
}

/* Common code for fixed_handicap and place_free_handicap. */
static int
place_handicap(char *s, int fixed)
{
	// TODO
	return gtp_finish_response();
}

static int
gtp_fixed_handicap(char *s)
{
	return place_handicap(s, 1);
}

static int
gtp_place_free_handicap(char *s)
{
	return place_handicap(s, 0);
}

static int
gtp_set_free_handicap(char *s)
{
	// TODO
	return gtp_success("");
}

static int
gtp_play(char *s)
{
	init();
	int i, j;
	int color = EMPTY;

	if (!gtp_decode_move(s, &color, &i, &j))
		return gtp_failure("invalid color or coordinate");

	SG_ASSERT_BW(color);
	fout << (color == SG_BLACK ? "black " : "white ") << steps++ << ": " << i << ' ' << j << endl;
	fout.flush();

	if (i == -1 && j == -1)
		game->execute(SG_PASS, color);
	else
		game->execute(SgPointUtil::Pt(i, j), color);
	return gtp_success("2");
}

static int
gtp_genmove(char *s)
{
	init();
	int i, j;
	int color = EMPTY;

	if (!gtp_decode_color(s, &color))
		return gtp_failure("invalid color");

	SgPoint move = game->genMove();
	SG_ASSERT_BW(color);

	game->execute(move, color);

	if (move == SG_PASS) {
		i = j = -1;
	}
	else {
		i = Col(move);
		j = Row(move);
	}

	fout << (color == SG_BLACK ? "black " : "white ") << steps++ << ": " << i << ' ' << j << endl;
	fout.flush();

	gtp_start_response(GTP_SUCCESS);
	gtp_mprintf("%m", i, j);
	return gtp_finish_response();
}

/* Compute final score. We use area scoring since that is the only
 * option that makes sense for this move generation algorithm.
 */
static int
gtp_final_score(char *s)
{
	// TODO
	return gtp_success("0");
}

static int
gtp_final_status_list(char *s)
{
	// TODO
	return gtp_finish_response();
}

/* Write a row of letters, skipping 'I'. */
static void
letters(void)
{
	int i;

	printf("	");
	for (i = 0; i < board_size; i++)
		printf(" %c", 'A' + i + (i >= 8));
}

static int
gtp_showboard(char *s)
{
	game->board.printBoard(cout);
	return gtp_success("");
}
