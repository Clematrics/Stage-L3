############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2019 Xilinx, Inc. All Rights Reserved.
############################################################
open_project processor
set_top processor
add_files processor/src/units/alu.cpp -cflags "-Iprocessor/include"
add_files processor/src/architecture/architecture_helpers.cpp -cflags "-Iprocessor/include"
add_files processor/src/stages/commit.cpp -cflags "-Iprocessor/include"
add_files processor/src/debug/debug_helpers.cpp -cflags "-Iprocessor/include"
add_files processor/src/debug/debugger.cpp -cflags "-Iprocessor/include"
add_files processor/src/stages/decode.cpp -cflags "-Iprocessor/include"
add_files processor/src/architecture/disasm.cpp -cflags "-Iprocessor/include"
add_files processor/src/stages/fetch.cpp -cflags "-Iprocessor/include"
add_files processor/src/architecture/immediate_packing.cpp -cflags "-Iprocessor/include"
add_files processor/src/stages/issue.cpp -cflags "-Iprocessor/include"
add_files processor/src/units/memory_unit.cpp -cflags "-Iprocessor/include"
add_files processor/src/pipeline.cpp -cflags "-Iprocessor/include"
add_files processor/src/processor.cpp -cflags "-Iprocessor/include"
add_files processor/src/components/register_map.cpp -cflags "-Iprocessor/include"
add_files processor/src/stages/stage.cpp -cflags "-Iprocessor/include"
add_files processor/src/stages/write_back.cpp -cflags "-Iprocessor/include"
add_files -tb processor/tb/processor_tb.cpp -cflags "-Iprocessor/include"
open_solution "solution1"
set_part {xc7z020-clg400-1}
create_clock -period 10 -name default
#source "./processor/solution1/directives.tcl"
csim_design -setup
csynth_design
cosim_design
export_design -format ip_catalog
