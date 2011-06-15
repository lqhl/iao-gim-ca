#ifndef UCT_TREE_H
#define UCT_TREE_H

#include "UCTTree.h"
#include "UCTNode.h"
#include "GoBoard.h"
#include "Poco/Random.h"
#include <vector>

using std::vector;
using Poco::Random;

class SuperGoGame;

class UCTTree {
public:
	UCTNode *node;

	// tree will be accessed by one thread at a time, so we share a random generator
	static Random rand;

	int numNodes;

	int root;

	vector<int> freeList;

	SuperGoGame* game;

	GoBoard* board;

	UCTTree(SuperGoGame* game, int numNodes);

	UCTNode* allocateNode() {
		//poco_assert(freeList.size() > 0);
		if (freeList.empty())
			return NULL;
		int i = freeList.back();
		freeList.pop_back();
		poco_assert(node[i].level == RESERVED);

		node[i].clear();
		return &node[i];
	}

	void prune(int count);

	UCTNode* rootNode() {
		return &node[root];
	}

	void update(SgPoint move); // update the tree by executing a move

	void deleteSubtree(UCTNode* n);

	void recycleNode(UCTNode* n) {
		poco_assert(n->level != RESERVED && n->level != UNUSED);
		n->level = RESERVED;
		freeList.push_back(n - node);
	}

	void updateStat(vector<SgPoint>& seqIn, vector<SgPoint>& seqOut,
			COUNT result);

	inline UCTNode* getNode(int ind) {
		UCTNode* node = &node[ind];
		poco_assert(node->level >= 0);
		return node;
	}

	bool tryExpand(GoBoard* board, UCTNode* node, SuperGoGame* game,
			BoardState& state);

	void preprocess(GoBoard* board, UCTNode* node) {
		poco_assert(board->Size() == UctPatterns::BOARD_SIZE);
		SgPoint p = node->move;

		bool blackMove = board->ToPlay() == SG_BLACK;
		if (UctPatterns::Line(p) == 3)
			node->updateRave(6.67, blackMove ? 1.0 : 0.0);
		if (UctPatterns::Line(p) == 1)
			node->updateRave(6.67, blackMove ? 0.0 : 1.0);
		if (UctPatterns::matchAny(board, p))
			node->updateRave(5.0, blackMove ? 1.0 : 0.0);
		if (UctPatterns::match2BadKogeima(board, p))
			node->updateRave(2.5, blackMove ? 0.0 : 1.0);
		if (UctPatterns::match2EmptyTriangle(board, p))
			node->updateRave(5, blackMove ? 0.0 : 1.0);

	}

	void preprocessChildren(GoBoard* board, vector<SgPoint>& moves) {
		vector<SgPoint> nakadeMoves;
		for (vector<SgPoint>::iterator it = moves.begin(); it != moves.end(); ++it)
			genNakade(board, *it, moves);
	}

	void PushBack(vector<SgPoint>& moves, SgPoint p) {
		if (board->IsLegal(p))
			moves.push_back(p);
	}


	void genNakade(GoBoard* board, SgPoint p,
			vector<SgPoint>& moves) {
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
				PushBack(moves, p);
			} else if (numEmptyNeighbors == 1) {
				for (SgNb4Iterator it(p); it; ++it)
					if (board->IsEmpty(*it)) {
						if (board->NumEmptyNeighbors(*it) != 2
								|| board->NumNeighbors(*it, toPlay) > 0)
							return;
						for (SgNb4Iterator it2(*it); it2; ++it2)
							if (board->IsEmpty(*it2) && *it2 != p) {
								if (board->NumEmptyNeighbors(*it2) == 1
										&& board->NumNeighbors(*it2, toPlay)
												== 0)
									PushBack(moves, *it);
								break;
							}
						break;
					}

			}
		}
	}

	void generateAtariCaptureMove(GoBoard* board, SgPoint p,
			vector<SgPoint>& moves) {
		if (board->GetColor(p) == SG_WHITE + SG_BLACK - board->ToPlay()
				&& board->InAtari(p)) {
			SgMove mv = board->TheLiberty(p);
			PushBack(moves, mv);
		}
	}

	void generateAtariDefenseMove(GoBoard* board, SgPoint p,
			vector<SgPoint>& moves) {
		SgBlackWhite toPlay = board->ToPlay();
		if (board->GetColor(p) != SG_WHITE + SG_BLACK - toPlay)
			return;
		if (board->NumNeighbors(p, toPlay) == 0)
			return;

		SgArrayList<SgPoint, 4> anchorList;
		for (SgNb4Iterator it(p); it; ++it) {
			if (board->GetColor(*it) != toPlay || !board->InAtari(*it))
				continue;
			SgPoint anchor = board->Anchor(*it);
			if (anchorList.Contains(anchor))
				continue;
			anchorList.PushBack(anchor);

			// Check if move on last liberty would escape the atari
			SgPoint theLiberty = board->TheLiberty(anchor);
			if (!GoBoardUtil::SelfAtari(*board, theLiberty))
				PushBack(moves, theLiberty);

			// we can escape atari if we can capture the opponent's blocks
			// Capture adjacent blocks
			for (GoAdjBlockIterator<GoBoard> it2(*board, anchor, 1); it2; ++it2) {
				SgPoint oppLiberty = board->TheLiberty(*it2);
				// If opponent's last liberty is not my last liberty, we know
				// that we will have two liberties after capturing (my last
				// liberty + at least one stone captured). If both last liberties
				// are the same, we already checked above with
				// GoBoardUtil::SelfAtari(theLiberty), if the move escapes the
				// atari
				if (oppLiberty != theLiberty)
					PushBack(moves, oppLiberty);
			}
		}
	}

	void generateLowLibertyMove(GoBoard* board, SgPoint p,
			vector<SgPoint>& moves) {

		const SgBlackWhite toPlay = board->ToPlay();

		if (board->GetColor(p) != SG_WHITE + SG_BLACK - toPlay) return;

		// take liberty of last move
		if (board->NumLiberties(p) == 2) {
			const SgPoint anchor = board->Anchor(p);
			selectLibertyMove(board, moves, anchor);
		}

		if (board->NumNeighbors(p, toPlay) != 0) {
			// play liberties of neighbor blocks
			SgArrayList<SgPoint, 4> ourLowLibBlocks;
			for (SgNb4Iterator it(p); it; ++it) {
				if (board->GetColor(*it) == toPlay && board->NumLiberties(*it)
						== 2) {
					const SgPoint anchor = board->Anchor(*it);
					if (!ourLowLibBlocks.Contains(anchor)) {
						ourLowLibBlocks.PushBack(anchor);
						selectLibertyMove(board, moves, anchor);
					}
				}
			}
		}

	}

	void selectLibertyMove(GoBoard* board,
			vector<SgPoint>& moves, SgPoint anchor) {
		// TODO simplified version
		SgPoint ignoreOther;
		if (!GoBoardUtil::IsSimpleChain(*board, anchor, ignoreOther))
			for (GoBoard::LibertyIterator it(*board, anchor); it; ++it)
				if (!GoBoardUtil::SelfAtari(*board, *it))
					PushBack(moves, *it);
	}

};
#endif
