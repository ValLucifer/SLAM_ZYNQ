// ==============================================================
// File generated on Fri Nov 13 23:32:25 +0800 2020
// Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC v2018.3 (64-bit)
// SW Build 2405991 on Thu Dec  6 23:38:27 MST 2018
// IP Build 2404404 on Fri Dec  7 01:43:56 MST 2018
// Copyright 1986-2018 Xilinx, Inc. All Rights Reserved.
// ==============================================================
`timescale 1 ns / 1 ps

module fast_accel_ama_adCeG_DSP48_1 (
    input clk,
    input rst,
    input ce,
    input  [12 - 1:0] in0,
    input  [12 - 1:0] in1,
    input  [5 - 1:0] in2,
    input  [23 - 1:0] in3,
    output [23 - 1:0]  dout);


wire signed [18 - 1:0]     b;
wire signed [25 - 1:0]     a;
wire signed [25 - 1:0]     d;
wire signed [48 - 1:0]     c;
wire signed [43 - 1:0]     m;
wire signed [48 - 1:0]     p;
wire signed [25 - 1:0]     ad;
reg  signed [43 - 1:0]     m_reg;
reg  signed [25 - 1:0]     ad_reg;
reg  signed [18 - 1:0]     b_reg;

assign a = $unsigned(in0);
assign d = $unsigned(in1);
assign b = $signed(in2);
assign c = $signed(in3);

assign ad = a + d;
assign m  = ad_reg * b_reg;
assign p  = m_reg + c;

always @(posedge clk) begin
    if (ce) begin
        m_reg  <= m;
        ad_reg <= ad;
        b_reg  <= b;
    end
end

assign dout = p;

endmodule
`timescale 1 ns / 1 ps
module fast_accel_ama_adCeG(
    clk,
    reset,
    ce,
    din0,
    din1,
    din2,
    din3,
    dout);

parameter ID = 32'd1;
parameter NUM_STAGE = 32'd1;
parameter din0_WIDTH = 32'd1;
parameter din1_WIDTH = 32'd1;
parameter din2_WIDTH = 32'd1;
parameter din3_WIDTH = 32'd1;
parameter dout_WIDTH = 32'd1;
input clk;
input reset;
input ce;
input[din0_WIDTH - 1:0] din0;
input[din1_WIDTH - 1:0] din1;
input[din2_WIDTH - 1:0] din2;
input[din3_WIDTH - 1:0] din3;
output[dout_WIDTH - 1:0] dout;



fast_accel_ama_adCeG_DSP48_1 fast_accel_ama_adCeG_DSP48_1_U(
    .clk( clk ),
    .rst( reset ),
    .ce( ce ),
    .in0( din0 ),
    .in1( din1 ),
    .in2( din2 ),
    .in3( din3 ),
    .dout( dout ));

endmodule

