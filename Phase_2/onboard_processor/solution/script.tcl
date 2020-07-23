############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2019 Xilinx, Inc. All Rights Reserved.
############################################################
open_project onboard_processor
set_top processor
add_files onboard_processor/src/architecture/architecture_helpers.cpp -cflags "-Ionboard_processor/include"
add_files onboard_processor/src/stages/commit.cpp -cflags "-Ionboard_processor/include"
add_files onboard_processor/src/debug/debug_helpers.cpp -cflags "-Ionboard_processor/include"
add_files onboard_processor/src/debug/debugger.cpp -cflags "-Ionboard_processor/include"
add_files onboard_processor/src/stages/decode.cpp -cflags "-Ionboard_processor/include"
add_files onboard_processor/src/stages/fetch.cpp -cflags "-Ionboard_processor/include"
add_files onboard_processor/src/architecture/immediate_packing.cpp -cflags "-Ionboard_processor/include"
add_files onboard_processor/src/stages/issue.cpp -cflags "-Ionboard_processor/include"
add_files onboard_processor/src/pipeline.cpp -cflags "-Ionboard_processor/include"
add_files onboard_processor/src/processor.cpp -cflags "-Ionboard_processor/include"
add_files onboard_processor/src/components/register_map.cpp -cflags "-Ionboard_processor/include"
add_files -tb onboard_processor/tb/processor_tb.cpp -cflags "-Ionboard_processor/include -Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
open_solution "solution"
set_part {xc7z020-clg400-1}
create_clock -period 15 -name default
config_sdx -target none
config_export -format ip_catalog -rtl verilog -vivado_optimization_level 2 -vivado_phys_opt place -vivado_report_level 0
set_clock_uncertainty 12.5%
#source "./onboard_processor/solution/directives.tcl"
csim_design
csynth_design
cosim_design
export_design -rtl verilog -format ip_catalog
