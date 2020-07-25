#pragma once

/* ****************************************************************************
*    This file configures the compilation options in Vivado/Vitis HLS
*    to switch between the debug and release mode
**************************************************************************** */

#ifdef __SYNTHESIS__
// Comment the following macro to enable release mode
//  #define DBG_SYNTH
#endif // __SYNTHESIS__