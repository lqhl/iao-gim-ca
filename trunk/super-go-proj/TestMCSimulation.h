#ifndef TEST_MCSIMULATION_H
#define TEST_MCSIMULATION_H

#include "GoBoard.h"
#include "GoUctBoard.h"
#include "UCTSearchRunner.h"
#include "SgPoint.h"

using namespace SgPointUtil;

void testMCSimulation() {
	ofstream out("test-simulation.txt");

	GoBoard* board = new GoBoard(13);

	UCTSearchRunner *runner = new UCTSearchRunner(*board);

	runner->startPlayOut(board);

	for (int i = 0; i < 200; ++i) {

		if (i == 98)
			Debugger::enter();
		poco_debug_f1(runner->uctLogger, " simulation i = %d", i);
		GoPlayerMove move = runner->generateMove();

		if (move.Point() == SG_NULLMOVE || move.Point() == SG_PASS) {
			cout << "Null move or pass move has be generated at i = " << i
					<< endl;
			break;
		}

		out << "i = " << i << endl;
		out << "move: " << " row=" << Row(move.Point()) << " col=" << Col(
				move.Point()) << " color=" << move.Color();
		out << endl;

		runner->play(move);

		runner->playOutBoard.printBoard(out);
	}
}

// test simulation speed and robustness
void testMCSimulation2() {
	ofstream out("test-simulation.txt");

	GoBoard* board = new GoBoard(13);

	UCTSearchRunner *runner = new UCTSearchRunner(*board);

	for (int k = 0; k < 10000; ++k) {
		runner->startPlayOut(board);
		runner->rand.seed(k);

		for (int i = 0; i <1000; ++i) {


			GoPlayerMove move = runner->generateMove();

			if (move.Point() == SG_NULLMOVE || move.Point() == SG_PASS) {
				out << "Null move or pass move has be generated at i = " << i
						<< endl;
				break;
			}

			runner->play(move);

			//			out << "k = " << k << " i = " << i << endl;
			//			out << "move: " << " row=" << Row(move.Point()) << " col=" << Col(move.Point()) << " color=" << move.Color();
			//			out << endl;
			//			runner->playOutBoard.printBoard(out);
		}

		if (k % 100 == 0) cout << "#simulation = " << k << endl;
	}
}

#endif
