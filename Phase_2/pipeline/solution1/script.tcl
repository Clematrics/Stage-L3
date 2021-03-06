############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2019 Xilinx, Inc. All Rights Reserved.
############################################################
open_project pipeline
set_top pipeline
add_files pipeline/include/common.hpp
add_files pipeline/src/debugger.cpp
add_files pipeline/include/debugger.hpp
add_files pipeline/src/decode.cpp
add_files pipeline/include/decode.hpp
add_files pipeline/src/disasm.cpp
add_files pipeline/include/disasm.hpp
add_files pipeline/src/fetch.cpp
add_files pipeline/include/fetch.hpp
add_files pipeline/src/helpers.cpp
add_files pipeline/include/helpers.hpp
add_files pipeline/src/instructions.cpp
add_files pipeline/include/instructions.hpp
add_files pipeline/include/json.hpp
add_files pipeline/src/pipeline.cpp
add_files pipeline/include/pipeline.hpp
add_files pipeline/src/register_file.cpp
add_files pipeline/include/register_file.hpp
add_files -tb pipeline/tb/pipeline_tb.cpp -cflags "-Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
open_solution "solution1"
set_part {xc7z020-clg400-1}
create_clock -period 10 -name default
config_sdx -target none
config_export -format ip_catalog -rtl verilog -vivado_optimization_level 2 -vivado_phys_opt place -vivado_report_level 0
set_clock_uncertainty 12.5%
#source "./pipeline/solution1/directives.tcl"
csim_design
csynth_design
cosim_design
export_design -rtl verilog -format ip_catalog
