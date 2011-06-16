#include "UCTTree.h"
#include "Poco/Hashmap.h"
#include "SuperGoGame.h"
#include "GoBoard.h"
#include "GoBoardUtil.h"
#include "util/util.h"
#include <assert.h>

#undef max
using Poco::HashMap;
using GoBoardUtil::getGoNeighbors;

Random UCTTree::rand;

UCTTree::UCTTree(SuperGoGame* game, int numNodes) {
	this->game = game;
	this->preprocChildren = game->preprocessChildren;
	this->numNodes = numNodes;
	node = new UCTNode[numNodes];
	freeList.clear();
	freeList.reserve(numNodes);
	for(int i=0; i<numNodes; ++i) {
		freeList.push_back(i);
		node[i].level = RESERVED;
	}

	UCTNode* n = allocateNode();
	n->level = 0;
	root = n - node;
	board = &game->board;


}


void UCTTree::prune(int count) {
	for(int i=0; i<numNodes; ++i) {
		if (node[i].level != RESERVED && node[i].visitCount < count) {
			node[i].level = RESERVED;
			freeList.push_back(i);
		}
	}
}


void UCTTree::deleteSubtree(UCTNode* n) {
	for(vector<UCTNode*>::iterator it = n->children.begin(); it != n->children.end(); ++it) {
		deleteSubtree(*it);
	}
	recycleNode(n);
}

// result is #black-stone - #white-stone
void UCTTree::updateStat(vector<SgPoint>& seqIn, vector<SgPoint>& seqOut, COUNT result) {
	// TODO assert the color

	int MAX_DEPTH = numeric_limits<int>::max();
	int moveDepth[2][SG_MAXPOINT];
	fill(moveDepth[0], moveDepth[0] + SG_MAXPOINT, MAX_DEPTH);
	fill(moveDepth[1], moveDepth[1] + SG_MAXPOINT, MAX_DEPTH);

	int rootLevel = rootNode()->level;
	int k = 1;
	vector<SgPoint>::iterator it;
	for(it = seqIn.begin(); it != seqIn.end(); ++it) {
		++k;
	}
	for(it = seqOut.begin(); it != seqOut.end(); ++it) {
		int player = 1 - (rootLevel + k) % 2;

		if (moveDepth[player][*it] == MAX_DEPTH) {
			moveDepth[player][*it] = k;
		}
		++k;
	}


	VALUE delta = 0.5;
	if (result > game->getKomi()) delta = 1;
	else if (result < game->getKomi()) delta = 0;

	COUNT weight = game->resultWeight(result);
	// from root go down
	UCTNode* p = rootNode();
	p->updateVisit(weight, delta);
	for(vector<int>::iterator it = seqIn.begin(); it != seqIn.end(); ++it) {
		vector<UCTNode*>& children = p->children;
		vector<UCTNode*>::iterator j = children.begin();
		UCTNode* next = NULL;


		for(; j != children.end(); ++j) {
			UCTNode& c = *(*j);
			if (c.move == *it) {
				next = &c;
				c.updateVisit(weight, delta);
			}
			else {
				int d = moveDepth[p->level % 2][c.move];
				if (d != MAX_DEPTH)
					c.updateRave(1 - (double)(d) / k, delta);
			}
		}
		poco_assert(next != NULL);

		p = next;
	}

}

void UCTTree::update(SgPoint move) {
	// TODO maybe the move is pass, and we simply reconstruct a new tree
	poco_assert(move != SG_NULLMOVE);
	UCTNode* next = NULL;
	UCTNode* n = rootNode();
	vector<UCTNode*>& children = n->children;
	for(vector<UCTNode*>::iterator it = children.begin(); it != children.end(); ++it) {
		if ((*it)->move == move) {
			next = *it;
			continue;
		}
		deleteSubtree(*it);
	}

	// A PASS MOVE, maybe
	if (next == NULL) {
		next = allocateNode();
		next->level = n->level + 1;
	}
	recycleNode(n);

	root = next - node;
}

inline bool IsSimpleEye(const GoBoard& bd, SgPoint p,
                                   SgBlackWhite c)
{
    // Function is inline despite its large size, because it returns quickly
    // on average, which makes the function call an overhead

    SgBlackWhite opp = SgOppBW(c);
    if (bd.HasEmptyNeighbors(p) || bd.HasNeighbors(p, opp))
        return false;
    SgArrayList<SgPoint,2> anchors;

    SgArrayList<SgPoint, 4> nb_p;
    getGoNeighbors(bd, p, nb_p);
    for (SgArrayList<SgPoint, 4>::Iterator it(nb_p); it; ++it)
    {
        SgPoint nbPoint = *it;
        if (bd.IsBorder(nbPoint))
            continue;
        poco_assert(bd.GetColor(nbPoint) == c);
        SgPoint nbAnchor = bd.Anchor(nbPoint);
        if (! anchors.Contains(nbAnchor))
        {
            if (anchors.Length() > 1)
                return false;
            anchors.PushBack(nbAnchor);
        }
    }
    if (anchors.Length() == 1)
        return true;
    for (GoBoard::LibertyIterator it(bd, anchors[0]); it; ++it)
    {
        SgPoint lib = *it;
        if (lib == p)
            continue;
        bool isSecondSharedEye = true;
        SgArrayList<SgPoint,2> foundAnchors;
        SgArrayList<SgPoint, 4> nb_lib;
        getGoNeighbors(bd, p, nb_lib);
        for (SgArrayList<SgPoint, 4>::Iterator it2(nb_lib); it2; ++it2)
        {
            SgPoint nbPoint = *it2;
            if (bd.IsBorder(nbPoint))
                continue;
            if (bd.GetColor(nbPoint) != c)
            {
                isSecondSharedEye = false;
                break;
            }
            SgPoint nbAnchor = bd.Anchor(nbPoint);
            if (! anchors.Contains(nbAnchor))
            {
                isSecondSharedEye = false;
                break;
            }
            if (! foundAnchors.Contains(nbAnchor))
                foundAnchors.PushBack(nbAnchor);
        }
        if (isSecondSharedEye && foundAnchors.Length() == 2)
            return true;
    }
    return false;
}

bool UCTTree::tryExpand(GoBoard* board, UCTNode* node, SuperGoGame* game, BoardState& state) {
	if (node->fullyExpanded) {
		vector<UCTNode*> children = node->children;
		if (children.size() > 1) {
			swap(children[0], children[rand.next(children.size())]);
		}
		return true;
	}

	SgBlackWhite toPlay = node->level % 2 == 0 ? SG_BLACK : SG_WHITE;
	bool asChild[SG_MAXPOINT];
	fill(asChild, asChild + SG_MAXPOINT, false);
	vector<UCTNode*>& children = node->children;
	for (vector<UCTNode*>::iterator it = children.begin(); it != children.end(); ++it) {
		asChild[(*it)->move] = true;
	}

	int total = board->Size() * board->Size();
	if (node->level < total)
		children.reserve(total - node->level + 10);

	vector<UCTNode*> newChildren;
	for (GoBoard::Iterator it(*board); it; ++it) {
		if (!asChild[*it] && board->GetColor(*it) == SG_EMPTY
			&& board->IsLegal(*it) && !IsSimpleEye(*board, *it, toPlay)) {
				UCTNode* n = allocateNode();
				if (n == NULL) return false;

				n->level = node->level+1;
				n->move = *it;
				preprocess(board, n);
				children.push_back(n);
				newChildren.push_back(n);
		}
	}
	preprocessChildren(board, node, newChildren);
	node->fullyExpanded = true;

	if (children.empty()) {
		COUNT res = game->evaluate(board);
		if (res > game->getKomi()) state = BLACK_WIN;
		else if (res == game->getKomi()) state = DRAW;
		else state = WHITE_WIN;

		return true;
	}

	// do some initialization
	{
		vector<UCTNode*> children = node->children;
		if (children.size() > 1) {
			swap(children[0], children[rand.next(children.size())]);
		}

	}

//	Logger& logger = Util::getTreeLogger();
//	if (logger.getLevel() >= 7) {
//		logger.debug("children");
//		for(int i=0; i<node->children.size(); ++i) {
//			poco_debug_f2(logger, "move = %d, level = %d", node->children[i]->move, node->children[i]->level);
//		}
//
//	}
	return true;
}
