############################################################
## This file is generated automatically by Vitis HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2020 Xilinx, Inc. All Rights Reserved.
############################################################
open_project pipeline
set_top pipeline
add_files pipeline/include/common.hpp
add_files pipeline/include/config.hpp
add_files pipeline/src/decode.cpp
add_files pipeline/include/decode.hpp
add_files pipeline/src/fetch.cpp
add_files pipeline/include/fetch.hpp
add_files pipeline/src/pipeline.cpp
add_files pipeline/include/pipeline.hpp
add_files -tb pipeline/tb/decode_tb.cpp
add_files -tb pipeline/tb/fetch_tb.cpp
add_files -tb pipeline/tb/pipeline_tb.cpp
open_solution "pipeline" -flow_target vivado
set_part {xc7z020clg484-1}
create_clock -period 10 -name default
source "./pipeline/pipeline/directives.tcl"
csim_design
csynth_design
cosim_design
export_design -format ip_catalog
