############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2018 Xilinx, Inc. All Rights Reserved.
############################################################
open_project transform
set_top transform
add_files ./pyramid.cpp -cflags "-I ../xfopencv/include -std=c++0x -D__SDSVHLS__"
open_solution "solution1"
set_part {xc7z020clg400-1} -tool vivado
create_clock -period 3 -name default
config_export -format ip_catalog -rtl verilog
csynth_design
export_design -rtl verilog -format ip_catalog
close_project
