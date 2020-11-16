// (c) Copyright 1995-2020 Xilinx, Inc. All rights reserved.
// 
// This file contains confidential and proprietary information
// of Xilinx, Inc. and is protected under U.S. and
// international copyright and other intellectual property
// laws.
// 
// DISCLAIMER
// This disclaimer is not a license and does not grant any
// rights to the materials distributed herewith. Except as
// otherwise provided in a valid license issued to you by
// Xilinx, and to the maximum extent permitted by applicable
// law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND
// WITH ALL FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES
// AND CONDITIONS, EXPRESS, IMPLIED, OR STATUTORY, INCLUDING
// BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, NON-
// INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE; and
// (2) Xilinx shall not be liable (whether in contract or tort,
// including negligence, or under any other theory of
// liability) for any loss or damage of any kind or nature
// related to, arising under or in connection with these
// materials, including for any direct, or any indirect,
// special, incidental, or consequential loss or damage
// (including loss of data, profits, goodwill, or any type of
// loss or damage suffered as a result of any action brought
// by a third party) even if such damage or loss was
// reasonably foreseeable or Xilinx had been advised of the
// possibility of the same.
// 
// CRITICAL APPLICATIONS
// Xilinx products are not designed or intended to be fail-
// safe, or for use in any application requiring fail-safe
// performance, such as life-support or safety devices or
// systems, Class III medical devices, nuclear facilities,
// applications related to the deployment of airbags, or any
// other applications that could lead to death, personal
// injury, or severe property or environmental damage
// (individually and collectively, "Critical
// Applications"). Customer assumes the sole risk and
// liability of any use of Xilinx products in Critical
// Applications, subject only to applicable laws and
// regulations governing limitations on product liability.
// 
// THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS
// PART OF THIS FILE AT ALL TIMES.
// 
// DO NOT MODIFY THIS FILE.


// IP VLNV: xilinx.com:hls:transform:1.0
// IP Revision: 2011132329

(* X_CORE_INFO = "transform,Vivado 2018.3" *)
(* CHECK_LICENSE_TYPE = "orb_fpga_transform_1_0,transform,{}" *)
(* CORE_GENERATION_INFO = "orb_fpga_transform_1_0,transform,{x_ipProduct=Vivado 2018.3,x_ipVendor=xilinx.com,x_ipLibrary=hls,x_ipName=transform,x_ipVersion=1.0,x_ipCoreRevision=2011132329,x_ipLanguage=VERILOG,x_ipSimLanguage=MIXED}" *)
(* IP_DEFINITION_SOURCE = "HLS" *)
(* DowngradeIPIdentifiedWarnings = "yes" *)
module orb_fpga_transform_1_0 (
  ap_clk,
  ap_rst_n,
  in_stream_TVALID,
  in_stream_TREADY,
  in_stream_TDATA,
  in_stream_TDEST,
  in_stream_TKEEP,
  in_stream_TSTRB,
  in_stream_TUSER,
  in_stream_TLAST,
  in_stream_TID,
  out_stream_V_V_TVALID,
  out_stream_V_V_TREADY,
  out_stream_V_V_TDATA
);

(* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME ap_clk, ASSOCIATED_BUSIF in_stream:out_stream_V_V, ASSOCIATED_RESET ap_rst_n, LAYERED_METADATA xilinx.com:interface:datatypes:1.0 {CLK {datatype {name {attribs {resolve_type immediate dependency {} format string minimum {} maximum {}} value {}} bitwidth {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 1} bitoffset {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 0}}}}, FREQ_HZ 100000000, PHASE 0.000, \
CLK_DOMAIN orb_fpga_processing_system7_0_0_FCLK_CLK0, INSERT_VIP 0" *)
(* X_INTERFACE_INFO = "xilinx.com:signal:clock:1.0 ap_clk CLK" *)
input wire ap_clk;
(* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME ap_rst_n, POLARITY ACTIVE_LOW, LAYERED_METADATA xilinx.com:interface:datatypes:1.0 {RST {datatype {name {attribs {resolve_type immediate dependency {} format string minimum {} maximum {}} value {}} bitwidth {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 1} bitoffset {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 0}}}}, INSERT_VIP 0" *)
(* X_INTERFACE_INFO = "xilinx.com:signal:reset:1.0 ap_rst_n RST" *)
input wire ap_rst_n;
(* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 in_stream TVALID" *)
input wire in_stream_TVALID;
(* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 in_stream TREADY" *)
output wire in_stream_TREADY;
(* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 in_stream TDATA" *)
input wire [7 : 0] in_stream_TDATA;
(* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 in_stream TDEST" *)
input wire [0 : 0] in_stream_TDEST;
(* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 in_stream TKEEP" *)
input wire [0 : 0] in_stream_TKEEP;
(* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 in_stream TSTRB" *)
input wire [0 : 0] in_stream_TSTRB;
(* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 in_stream TUSER" *)
input wire [0 : 0] in_stream_TUSER;
(* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 in_stream TLAST" *)
input wire [0 : 0] in_stream_TLAST;
(* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME in_stream, TDATA_NUM_BYTES 1, TDEST_WIDTH 1, TID_WIDTH 1, TUSER_WIDTH 1, LAYERED_METADATA undef, HAS_TREADY 1, HAS_TSTRB 1, HAS_TKEEP 1, HAS_TLAST 1, FREQ_HZ 100000000, PHASE 0.000, CLK_DOMAIN orb_fpga_processing_system7_0_0_FCLK_CLK0, INSERT_VIP 0" *)
(* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 in_stream TID" *)
input wire [0 : 0] in_stream_TID;
(* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 out_stream_V_V TVALID" *)
output wire out_stream_V_V_TVALID;
(* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 out_stream_V_V TREADY" *)
input wire out_stream_V_V_TREADY;
(* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME out_stream_V_V, TDATA_NUM_BYTES 1, TUSER_WIDTH 0, LAYERED_METADATA xilinx.com:interface:datatypes:1.0 {CLK {datatype {name {attribs {resolve_type immediate dependency {} format string minimum {} maximum {}} value {}} bitwidth {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 1} bitoffset {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 0}}} TDATA {datatype {name {attribs {resolve_type immediate depende\
ncy {} format string minimum {} maximum {}} value {}} bitwidth {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 8} bitoffset {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 0} integer {signed {attribs {resolve_type immediate dependency {} format bool minimum {} maximum {}} value false}}}} TDATA_WIDTH 8}, TDEST_WIDTH 0, TID_WIDTH 0, HAS_TREADY 1, HAS_TSTRB 0, HAS_TKEEP 0, HAS_TLAST 0, FREQ_HZ 100000000, PHASE 0.000, CL\
K_DOMAIN orb_fpga_processing_system7_0_0_FCLK_CLK0, INSERT_VIP 0" *)
(* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 out_stream_V_V TDATA" *)
output wire [7 : 0] out_stream_V_V_TDATA;

  transform inst (
    .ap_clk(ap_clk),
    .ap_rst_n(ap_rst_n),
    .in_stream_TVALID(in_stream_TVALID),
    .in_stream_TREADY(in_stream_TREADY),
    .in_stream_TDATA(in_stream_TDATA),
    .in_stream_TDEST(in_stream_TDEST),
    .in_stream_TKEEP(in_stream_TKEEP),
    .in_stream_TSTRB(in_stream_TSTRB),
    .in_stream_TUSER(in_stream_TUSER),
    .in_stream_TLAST(in_stream_TLAST),
    .in_stream_TID(in_stream_TID),
    .out_stream_V_V_TVALID(out_stream_V_V_TVALID),
    .out_stream_V_V_TREADY(out_stream_V_V_TREADY),
    .out_stream_V_V_TDATA(out_stream_V_V_TDATA)
  );
endmodule
