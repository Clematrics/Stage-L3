/* ****************************************************************************
*    Source file for the issue stage
**************************************************************************** */

#include "stages/issue.hpp"

#include "debug/debug.hpp"

IssueStage::IssueStage() {}

void IssueStage::interface(DecodeToIssue from_decode, WriteBackToIssue from_write_back, CommitToIssue from_commit, IssueToWriteBack* to_write_back, bit_t* issue_ran) {
	#pragma HLS inline

	// bit_t do_smth = ;

	// if (do_smth) {


	// 	#ifndef __SYNTHESIS__
	// 	Debugger::add_cycle_event({
	// 		{ "Issue stage",
	// 			{
	// 				{ "",  },
	// 				{ "",  }
	// 			}
	// 		}
	// 	});
	// 	#endif // __SYNTHESIS__
	// }
	// else {
	// }
	// *issue_ran = do_smth;
}