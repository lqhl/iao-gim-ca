#include <math.h>
#include "UCTSearchRunner.h"
#include "SuperGoGame.h"
#include "Poco/Timestamp.h"
#include "util/util.h"
#include "UCTPatterns.h"
#include "GoBoardUtil.h"
#include <typeinfo>

using std::exception;

class UnsupportedOperationException: public exception {

};

using Poco::Timestamp;
UCTSearchRunner::UCTSearchRunner(SuperGoGame* game) :
	playOutBoard(game->board) {
	static int counter = 0;
	this->game = game;
	this->ravePara1 = 1. / game->CInitial;
	this->ravePara2 = 1. / game->CFinal;
	this->CUCT = game->CUCT;

	rand.seed(++counter);
}

void UCTSearchRunner::start() {
	thread.start(*this);
}

void UCTSearchRunner::run() {
	searchBoard(game->searchTime, game->numPlayOut, &game->board, game->tree,
			game->treeLock);
}

void UCTSearchRunner::searchBoard(int timeLimit, int numPlayOut,
		GoBoard* board, UCTTree* tree, RWLock* treeLock) {
	playOutBoard.Init(*board);

	long limit = (long) timeLimit * 1000;
	Timestamp timer;

	for (int i = 0;; ++i) {
		// break the buffer
		if (i % 100 == 0) {
			if (timer.elapsed() >= limit)
				break;
		}

		//		int level = board->getLevel();
		//
		//		vector<int> seq;
		//		searchInTree(tree, treeLock, board, seq);
		//		for (int j = 0; j < numPlayOut; ++j) {
		//			vector<int> seq2;
		//			int res = playOut(board, seq2);
		//
		//			upateStat(tree, treeLock, res, seq, seq2);
		//		}
		//
		//		board->rollBack(level);

	}
}

void UCTSearchRunner::searchInTree(UCTTree* tree, RWLock* treeLock,
		GoBoard* board, vector<GoPlayerMove>& seq) {

}

int UCTSearchRunner::playOut(GoBoard* board, vector<GoPlayerMove>& seq) {

	// some preparation work
	capture_candidates.clear();

	random_candidates.clear();
	for (typename BOARD::Iterator it(*board); it; ++it)
		if (board->IsEmpty(*it)) {
			size_t size = random_candidates.size();
			if (size == 0)
				random_candidates.push_back(p);
			else {
				SgPoint& swapPoint = random_candidates[rand.next(size)];
				m_candidates.push_back(swapPoint);
				swapPoint = p;
			}
		}

	return -1;
}

void UCTSearchRunner::generateCaptureMoves(GoUctBoard* board, vector<
		GoPlayerMove>& moves) {
	poco_assert(moves.empty());
	const SgBlackWhite opp = board->Opponent();
	// For efficiency reasons, this function does not check, if the same
	// move is generated multiple times (and will therefore played with
	// higher probabilty, if there are also other capture moves), because in
	// nearly all cases, there is zero or one global capture move on the
	// board. Most captures are done immediately by the atari heuristic
	for (size_t i = 0; i < capture_candidates.size(); ++i) {
		const SgPoint p = capture_candidates[i];
		if (!board->OccupiedInAtari(p)) {
			capture_candidates[i]
					= capture_candidates[capture_candidates.size() - 1];
			capture_candidates.pop_back();
			--i;
			continue;
		}
		if (board->GetColor(p) == opp)
			moves.push_back(board->TheLiberty(p));
	}
}

SgPoint UCTSearchRunner::generateRandomMove(GoUctBoard* board) {
	SgBlackWhite toPlay = board->ToPlay();
	size_t i = random_candidates.size();
	while (true) {
		if (i == 0)
			break;
		--i;
		SgPoint p = random_candidates[i];
		if (!board.IsEmpty(p)) {
			random_candidates[i] = random_candidates[random_candidates.size()
					- 1];
			random_candidates.pop_back();
			continue;
		}
		if (board->IsLegal(p)) {
			return p;
		}
	}
	return SG_NULLMOVE;
}

GoPlayerMove UCTSearchRunner::generateMove(GoUctBoard* board,
		SgBlackWhite toPlay) {
	poco_assert(ToPlay == board->ToPlay());

	vector<GoPlayerMove> moves;
	moves.clear();
	//	m_checked = false;

	SgPoint mv = SG_NULLMOVE;

	SgPoint lastMove = board->GetLastMove();
	if (mv == SG_NULLMOVE && !SgIsSpecialMove(lastMove) // skip if Pass or Null
			&& board->IsEmpty(m_lastMove) // skip if move was suicide
	) {
		if (generateNakadeMove(board, moves)) {
			//			m_moveType = GOUCT_NAKADE;
			mv = randomSelect(moves);
		}
		if (generateAtariCaptureMove(board, moves)) {
			//			m_moveType = GOUCT_ATARI_CAPTURE;
			mv = randomSelect(moves);
		}
		if (mv == SG_NULLMOVE && generateAtariDefenseMove(board, moves)) {
			//			m_moveType = GOUCT_ATARI_DEFEND;
			mv = randomSelect(moves);
		}
		if (mv == SG_NULLMOVE && generateLowLibertyMove(board, moves)) {
			//			m_moveType = GOUCT_LOWLIB;
			mv = randomSelect(moves);
		}
		if (mv == SG_NULLMOVE && generatePatternMove(board, moves)) {
			//			m_moveType = GOUCT_PATTERN;
			mv = randomSelect(moves);
		}
	}
	if (mv == SG_NULLMOVE) {
		//		m_moveType = GOUCT_CAPTURE;
		generateCaptureMoves(moves);
		mv = randomSelect(moves);

	}
	if (mv == SG_NULLMOVE) {
		//		m_moveType = GOUCT_RANDOM;
		mv = generateRandomMove(board);
	}

	if (mv == SG_NULLMOVE) {
		m_moveType = GOUCT_PASS;
		mv = SG_PASS;
	} else {
		SG_ASSERT(m_bd.IsLegal(mv));
		m_checked = CorrectMove(GoUctUtil::DoSelfAtariCorrection, mv,
				GOUCT_SELFATARI_CORRECTION);
		if (USE_CLUMP_CORRECTION && !m_checked)
			CorrectMove(GoUctUtil::DoClumpCorrection, mv,
					GOUCT_CLUMP_CORRECTION);
	}
	SG_ASSERT(m_bd.IsLegal(mv));
	SG_ASSERT(mv == SG_PASS || !m_bd.IsSuicide(mv));

	if (m_param.m_statisticsEnabled)
		UpdateStatistics();

	return mv;
}

void UCTSearchRunner::upateStat(UCTTree* tree, RWLock* treeLock, int result,
		vector<GoPlayerMove>& seqIn, vector<GoPlayerMove>& seqOut) {
	treeLock->writeLock();

	//update the RAVE values
	// first goes down seqOut and build a hash table
	// the goes down seqIn and update the rave values

	// update the move count
	// just goes down seqIn and update

	treeLock->unlock();
}

int UCTSearchRunner::selectChildrenUCT(UCTTree* tree, int n) {
	int k = -1;
	COUNT best = -1;
	UCTNode& node = tree->node[n];
	vector<int>& v = node.children;
	for (int i = 0; i < v.size(); ++i) {
		UCTNode& m = tree->node[v[i]];
		double raveWeight = m.raveCount / (ravePara1 + ravePara2 * m.raveCount);
		double moveWeight = m.visitCount;
		double uct = CUCT * sqrt(log(node.visitCount) / (m.visitCount + 1));
		double res = (raveWeight * m.raveValue + moveWeight * m.visitValue)
				/ (raveWeight + moveWeight) + uct;

		if (k == -1 || res > best) {
			best = res;
			k = v[i];
		}
	}
	return k;
}

int UCTSearchRunner::selectChildrenCount(UCTTree* tree, int n) {
	int k = -1;
	COUNT best = -1;
	UCTNode& node = tree->node[n];
	vector<int>& v = node.children;
	for (int i = 0; i < v.size(); ++i) {
		UCTNode& m = tree->node[v[i]];
		if (m.visitCount > best) {
			best = m.visitCount;
			k = v[i];
		}
	}
	return k;
}

int UCTSearchRunner::selectChildrenMEAN(UCTTree* tree, int n) {
	int k = -1;
	COUNT best = -1;
	UCTNode& node = tree->node[n];
	vector<int>& v = node.children;
	for (int i = 0; i < v.size(); ++i) {
		UCTNode& m = tree->node[v[i]];
		if (k == -1 || m.visitValue > best) {
			best = m.visitValue;
			k = v[i];
		}
	}
	return k;
}

int UCTSearchRunner::selectChildrenRAVE(UCTTree* tree, int n) {
	int k = -1;
	COUNT best = -1;
	UCTNode& node = tree->node[n];
	vector<int>& v = node.children;
	for (int i = 0; i < v.size(); ++i) {
		UCTNode& m = tree->node[v[i]];
		double raveWeight = m.raveCount / (ravePara1 + ravePara2 * m.raveCount);
		double moveWeight = m.visitCount;
		double res = (raveWeight * m.raveValue + moveWeight * m.visitValue)
				/ (raveWeight + moveWeight);
		if (k == -1 || res > best) {
			best = res;
			k = v[i];
		}
	}
	return k;
}

void genNakade(SgPoint p, GoUctBoard* board, vector<SgPoint>& moves) {
	SgBlackWhite toPlay = board->ToPlay();
	if (board->IsEmpty(p) && board->NumNeighbors(p, toPlay) == 0) {
		int numEmptyNeighbors = board->NumEmptyNeighbors(p);
		if (numEmptyNeighbors == 2) {
			int n = 0;
			for (SgNb4Iterator it(p); it; ++it)
				if (board->IsEmpty(*it)) {
					if (board->NumEmptyNeighbors(*it) != 1
							|| board->NumNeighbors(*it, toPlay) > 0)
						return;
					// find a empty neighbor that has 3 of toPlay stones around it
					if (++n > 2)
						break;
				}
			moves.push_back(p);
		} else if (numEmptyNeighbors == 1) {
			for (SgNb4Iterator it(p); it; ++it)
				if (board->IsEmpty(*it)) {
					if (board->NumEmptyNeighbors(*it) != 2
							|| board->NumNeighbors(*it, toPlay) > 0)
						return;
					for (SgNb4Iterator it2(*it); it2; ++it2)
						if (board->IsEmpty(*it2) && *it2 != p) {
							if (board->NumEmptyNeighbors(*it2) == 1
									&& board->NumNeighbors(*it2, toPlay) == 0)
								moves.push_back(*it);
							break;
						}
					break;
				}

		}
	}
}
bool UCTSearchRunner::generateNakadeMove(GoUctBoard* board,
		vector<SgPoint>& moves) {
	poco_assert(moves.empty());
	poco_assert(!SgIsSpecialMove(board->GetLastMove()));

	SgPoint last = board->GetLastMove();

	genNakade(last + SG_NS, board, moves);
	genNakade(last - SG_NS, board, moves);
	genNakade(last + SG_WE, board, moves);
	genNakade(last - SG_WE, board, moves);

	return !moves.empty();
}

bool UCTSearchRunner::generateAtariCaptureMove(GoUctBoard* board, vector<
		SgPoint>& moves) {

	poco_assert(! SgIsSpecialMove(board->GetLastMove()));
	if (board->InAtari(board->GetLastMove())) {
		SgMove mv = board->TheLiberty(board->GetLastMove());
		moves.push_back(mv);
		return true;
	}
	return false;

}

bool UCTSearchRunner::generateAtariDefenseMove(GoUctBoard* board, vector<
		SgPoint>& moves) {
	poco_assert(moves.empty());
	poco_assert(!SgIsSpecialMove(board->GetLastMove()));
	SgBlackWhite toPlay = board->ToPlay();
	if (board->NumNeighbors(board->GetLastMove(), toPlay) == 0)
		return false;
	SgArrayList<SgPoint, 4> anchorList;
	for (SgNb4Iterator it(board->GetLastMove()); it; ++it) {
		if (board->GetColor(*it) != toPlay || !board->InAtari(*it))
			continue;
		SgPoint anchor = board->Anchor(*it);
		if (anchorList.Contains(anchor))
			continue;
		anchorList.PushBack(anchor);

		// Check if move on last liberty would escape the atari
		SgPoint theLiberty = board->TheLiberty(anchor);
		if (!GoBoardUtil::SelfAtari(board, theLiberty))
			moves.push_back(theLiberty);

		// we can escape atari if we can capture the opponent's blocks
		// Capture adjacent blocks
		for (GoAdjBlockIterator<BOARD> it2(*board, anchor, 1); it2; ++it2) {
			SgPoint oppLiberty = board->TheLiberty(*it2);
			// If opponent's last liberty is not my last liberty, we know
			// that we will have two liberties after capturing (my last
			// liberty + at least one stone captured). If both last liberties
			// are the same, we already checked above with
			// GoBoardUtil::SelfAtari(theLiberty), if the move escapes the
			// atari
			if (oppLiberty != theLiberty)
				moves.push_back(oppLiberty);
		}
	}
	return !moves.empty();

}

bool UCTSearchRunner::generateLowLibertyMove(GoUctBoard* board,
		vector<SgPoint>& moves) {
	SgPoint lastMove = board->GetLastMove();
	poco_assert(! SgIsSpecialMove(lastMove));
	poco_assert(! board->IsEmpty(lastMove));
	poco_assert(moves.empty());

	const SgBlackWhite toPlay = board->ToPlay();

	// take liberty of last move
	if (board->NumLiberties(lastMove) == 2) {
		const SgPoint anchor = board->Anchor(lastMove);
		selectLibertyMoves(board, moves, anchor);
	}

	if (board->NumNeighbors(lastMove, toPlay) != 0) {
		// play liberties of neighbor blocks
		SgArrayList<SgPoint, 4> ourLowLibBlocks;
		for (SgNb4Iterator it(lastMove); it; ++it) {
			if (board->GetColor(*it) == toPlay && board->NumLiberties(*it) == 2) {
				const SgPoint anchor = board->Anchor(*it);
				if (!ourLowLibBlocks.Contains(anchor)) {
					ourLowLibBlocks.PushBack(anchor);
					selectLibertyMove(board, moves, anchor);
				}
			}
		}
	}

	return !moves.empty();

}

void GeneratePatternMove(GoUctBoard* board, vector<SgPoint>& moves, SgPoint p) {
	if (board->IsEmpty(p) && UctPatterns::matchAny(p)
			&& !GoBoardUtil::SelfAtari(m_bd, p))
		moves.push_back(p);
}

void GeneratePatternMove2(GoUctBoard* board, vector<SgPoint>& moves, SgPoint p) {
	if (board->IsEmpty(p) && !SgPointUtil::In8Neighborhood(lastMove, p)
			&& UctPatterns::matchAny(board, p) && !GoBoardUtil::SelfAtari(
			*board, p))
		moves.PushBack(p);
}
bool UCTSearchRunner::generatePatternMove(GoUctBoard* board,
		vector<SgPoint>& moves) {

	SgPoint lastMove = board->GetLastMove();
	SG_ASSERT(moves.empty());
	SG_ASSERT(!SgIsSpecialMove(lastMove));
	GeneratePatternMove(board, lastMove + SG_NS - SG_WE);
	GeneratePatternMove(board, lastMove + SG_NS);
	GeneratePatternMove(board, lastMove + SG_NS + SG_WE);
	GeneratePatternMove(board, lastMove - SG_WE);
	GeneratePatternMove(board, lastMove + SG_WE);
	GeneratePatternMove(board, lastMove - SG_NS - SG_WE);
	GeneratePatternMove(board, lastMove - SG_NS);
	GeneratePatternMove(board, lastMove - SG_NS + SG_WE);
	if (SECOND_LAST_MOVE_PATTERNS) {
		const SgPoint lastMove2 = board->Get2ndLastMove();
		if (!SgIsSpecialMove(lastMove2)) {
			GeneratePatternMove2(board, lastMove2 + SG_NS - SG_WE, lastMove);
			GeneratePatternMove2(board, lastMove2 + SG_NS, lastMove);
			GeneratePatternMove2(board, lastMove2 + SG_NS + SG_WE, lastMove);
			GeneratePatternMove2(board, lastMove2 - SG_WE, lastMove);
			GeneratePatternMove2(board, lastMove2 + SG_WE, lastMove);
			GeneratePatternMove2(board, lastMove2 - SG_NS - SG_WE, lastMove);
			GeneratePatternMove2(board, lastMove2 - SG_NS, lastMove);
			GeneratePatternMove2(board, lastMove2 - SG_NS + SG_WE, lastMove);
		}
	}
	return !moves.empty();

}

void UCTSearchRunner::captureCandidatesUpdateOnPlay(GoUctBoard* board) {
	SgPoint lastMove = board->GetLastMove();
	if (lastMove == SG_NULLMOVE || lastMove == SG_PASS)
		return;
	if (board->OccupiedInAtari(lastMove))
		capture_candidates.push_back(board->Anchor(lastMove));
	if (board->NumNeighbors(lastMove, board->ToPlay()) == 0)
		return;
	if (board->OccupiedInAtari(lastMove + SG_NS))
		capture_candidates.push_back(board->Anchor(lastMove + SG_NS));
	if (board->OccupiedInAtari(lastMove - SG_NS))
		capture_candidates.push_back(board->Anchor(lastMove - SG_NS));
	if (board->OccupiedInAtari(lastMove + SG_WE))
		capture_candidates.push_back(board->Anchor(lastMove + SG_WE));
	if (board->OccupiedInAtari(lastMove - SG_WE))
		capture_candidates.push_back(board->Anchor(lastMove - SG_WE));
}

void UCTSearchRunner::randomMoveUpdateOnPlay(GoUctBoard* board) {
	SgPoint lastMove = board->GetLastMove();
	const GoPointList& capturedStones = board->CapturedStones();
	if (!capturedStones.IsEmpty()) {
		// Don't remove stone played, too expensive, check later in Generate()
		// that generated point is still empty
		for (GoPointList::Iterator it(capturedStones); it; ++it) {
			size_t size = random_candidates.size();
			if (size == 0)
				random_candidates.push_back(p);
			else {
				SgPoint& swapPoint = random_candidates[rand.next(size)];
				m_candidates.push_back(swapPoint);
				swapPoint = p;
			}
		}
	}
}

void UCTSearchRunner::selectLibertyMove(GoUctBoard* board, vector<SgPoint>& moves, SgPoint achor) {
	// TODO simplified version
	SgPoint ignoreOther;
	if (!GoBoardUtil::IsSimpleChain(*board, block, ignoreOther))
		for (typename BOARD::LibertyIterator it(*board, block); it; ++it)
			if (!GoBoardUtil::SelfAtari(*board, *it))
				moves.push_back(*it);
}
