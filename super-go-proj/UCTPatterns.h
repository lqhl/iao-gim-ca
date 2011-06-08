#ifndef UCTPATTERNS_H
#define UCTPATTERNS_H

#include "GoBoard.h"

class UctPatterns {

	static const int BLACK_PATTERN = 0, WHITE_PATTERN = 1, BOTH_PATTERN = 2,
			NO_PATTERN = 3;

	static const int POWER3_8 = 3 * 3 * 3 * 3 * 3 * 3 * 3 * 3;
	static const int POWER3_5 = 3 * 3 * 3 * 3 * 3;

	static bool centerCode[2][POWER3_8];
	static bool edgeCode[2][POWER3_8];

	static void init() {
		for (int i = 0; i < 2; ++i) {
			for (int j = 0; j < POWER3_8; ++j) {
				centerCode[i][j] = false;
				edgeCode[i][j] = false;
			}
		}

		int p[9];
		int q[9];
		for (int i = 0; i < POWER3_8; ++i) {
			int k = i;
			for (int j = 0; j < 9; ++j) {
				p[j] = k % 3;
				k = k / 3;
			}

			int pattern = NO_PATTERN;
			int t = NO_PATTERN;

			t = matchHane(p);
			if (t != NO_PATTERN) {
				poco_assert(pattern == NO_PATTERN)
					;
				pattern = t;
			}

			t = matchCut1(p);
			if (t != NO_PATTERN) {
				poco_assert(pattern == NO_PATTERN)
					;
				pattern = t;
			}

			t = matchCut2(p);
			if (t != NO_PATTERN) {
				poco_assert(pattern == NO_PATTERN)
					;
				pattern = t;
			}

			addCenterPattern(p, pattern);

		}

		for (int i = 6; i < 9; ++i)
			p[i] = SG_EMPTY;

		for (int i = 0; i < POWER3_5; ++i) {
			int k = i;
			for (int j = 0; j < 6; ++j) {
				p[j] = k % 3;
				k /= 3;
			}

			int pattern = matchEdge(p);

			addEdgePattern(p, pattern);
		}
	}

	// p and q maybe the same
	void rotate(int* p, int* q) {
		int t[9];
		for(int i=0; i<9; ++i) {
			int line = i / 3;
			int col = i % 3;
			t[col * 3 + (2 - line)] = p[i];
		}
		copy(t, t+9, q);
	}

	void mirror(int* p, int* q) {
		int t[9];
		for(int i=0; i<9; ++i) {
			int line = i / 3;
			t[(2 - line) * 3 + i % 3] = p[i];
		}

		copy(t, t+9, q);
	}

	void reverseColor(int* p, int* q) {
		int t[9];
		for(int i=0; i<9; ++i) {
			if (p[i] == SG_EMPTY) t[i] = p[i];
			else t[i] = 1 - p[i];
		}
		copy(t, t+9, q);
	}


	void addCenterPattern(int* p, int pattern) {
		int q[9];
		if (pattern == NO_PATTERN)
			return;

		for (int i = 0; i < 8; ++i) {
			if (i %2 == 0)
				rotate(p, p);
			else
				mirror(p, p);


			if (pattern == BOTH_PATTERN || pattern == BLACK_PATTERN)
				setCenterPattern(SG_BLACK, p);
			if (pattern == BOTH_PATTERN || pattern == WHITE_PATTERN)
				setCenterPattern(SG_WHITE, p);

			reverseColor(p, q);

			if (pattern == BOTH_PATTERN || pattern == BLACK_PATTERN)
				setCenterPattern(SG_WHITE, q);
			if (pattern == BOTH_PATTERN || pattern == WHITE_PATTERN)
				setCenterPattern(SG_BLACK, q);


		}
	}

	static void setCenterPattern(SgBlackWhite color, int* p) {
		int k = 0;
		for(int i=0; i<9; ++i) k = k * 3 + p[i];

		centerCode[color][k] = true;
	}


	static void mirror2(int* p, int* q) {
		int t[9];
		for(int i=0; i<9; ++i) {
			int col = i % 3;
			int line = i / 3;
			t[line * 3 + 2 - col] = p[i];
		}

		copy(t, t+9, q);
	}

	static void addEdgePatterns(int* p, int pattern) {
		int q[9];
		for(int i=0; i<2; ++i) {
			mirror2(p, p);

			if (pattern == BOTH_PATTERN || pattern == BLACK_PATTERN)
				setEdgePattern(SG_BLACK, p);
			if (pattern == BOTH_PATTERN || pattern == WHITE_PATTERN)
				setEdgePattern(SG_WHITE, p);

			reverseColor(p, q);
			if (pattern == BOTH_PATTERN || pattern == BLACK_PATTERN)
				setEdgePattern(SG_WHITE, q);
			if (pattern == BOTH_PATTERN || pattern == WHITE_PATTERN)
				setEdgePattern(SG_BLACK, q);


		}
	}

	static void setEdgePattern(SgBlackWhite color, int* p) {
		int k = 0;
		for(int i=0; i<6; ++i) {
			k = k * 3 + p[i];
		}

		edgeCode[color][p] = true;
	}

	// TODO
	static bool matchAny(GoBoard* bd, SgPoint p) {

	}

	static bool matchCenter(GoBoard* bd, SgPoint p) {

	}

	static bool matchEdge(GoBoard* bd, SgPoint p) {

	}

	static int matchHane(SG_BLACKWhite* p) {
		int f1 = p[0] == SG_BLACK && p[1] == SG_WHITE && p[2] == SG_BLACK
				&& p[3] == SG_EMPTY && p[4] == SG_EMPTY && p[5] == SG_EMPTY;
		if (f1)
			return BOTH_PATTERN;
		bool f2 = p[0] == SG_BLACK && p[1] == SG_WHITE && p[2] == SG_EMPTY
				&& p[3] == SG_EMPTY && p[4] == SG_EMPTY && p[5] == SG_EMPTY
				&& p[7] == SG_EMPTY;
		if (f2)
			return BOTH_PATTERN;

		bool f3 = p[0] == SG_BLACK && p[1] == SG_WHITE && p[3] == SG_BLACK
				&& p[4] == SG_EMPTY && p[5] == SG_EMPTY && p[7] == SG_EMPTY;
		if (f3)
			return BOTH_PATTERN;

		bool f4 = p[0] == SG_BLACK && p[1] == SG_WHITE && p[2] == SG_WHITE
				&& p[3] == SG_EMPTY && p[4] == SG_EMPTY && p[5] == SG_EMPTY
				&& p[7] == SG_EMPTY;

		if (f4)
			return BLACK_PATTERN;

		return NO_PATTERN;

	}

	static bool matchCut1(SG_BLACKWhite* p) {
		bool f1 = p[0] == SG_BLACK && p[1] == SG_WHITE && p[3] == SG_WHITE
				&& p[4] == SG_EMPTY;
		bool f2 = p[0] == SG_BLACK && p[1] == SG_WHITE && p[3] == SG_WHITE
				&& p[4] == SG_EMPTY && p[5] == SG_WHITE && p[7] == SG_EMPTY;
		bool f3 = p[0] == SG_BLACK && p[1] == SG_WHITE && p[3] == SG_WHITE
				&& p[4] == SG_EMPTY && p[5] == SG_EMPTY && p[7] == SG_WHITE;
		if (f1 && !f2 && !f3)
			return BOTH_PATTERN;

		return NO_PATTERN;
	}

	static bool matchCut2(SG_BLACKWhite* p) {
		bool f = p[1] == SG_BLACK && p[3] == SG_WHITE && p[4] == SG_EMPTY
				&& p[5] == SG_WHITE && p[6] != SG_WHITE && p[7] != SG_WHITE
				&& p[8] != SG_WHITE;
		return f ? BOTH_PATTERN : NO_PATTERN;
	}

	static bool matchEdge(SG_BLACKWhite* p) {
		bool f1 = p[0] == SG_BLACK && p[1] == SG_EMPTY && p[3] == SG_WHITE
				&& p[4] == SG_EMPTY;
		if (f1)
			return BOTH_PATTERN;

		bool f2 = p[1] == SG_BLACK && p[3] != SG_BLACK && p[4] == SG_EMPTY
				&& p[5] == SG_WHITE;
		if (f2)
			return BOTH_PATTERN;

		bool f3 = p[1] == SG_BLACK && p[2] == SG_WHITE && p[4] == SG_EMPTY;
		if (f3)
			return BLACK_PATTERN;

		bool f4 = p[1] == SG_BLACK && p[2] == SG_WHITE && p[4] == SG_EMPTY
				&& p[5] != SG_BLACK;
		if (f4)
			return WHITE_PATTERN;

		bool f5 = p[1] == SG_BLACK && p[2] == SG_WHITE && p[3] == SG_WHITE
				&& p[4] == SG_EMPTY && p[5] == SG_BLACK;
		if (f5)
			return WHITE_PATTERN;

		return NO_PATTERN;

	}
};

#endif
