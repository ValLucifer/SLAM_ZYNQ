// ==============================================================
// File generated on Fri Nov 13 23:36:32 +0800 2020
// Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC v2018.3 (64-bit)
// SW Build 2405991 on Thu Dec  6 23:38:27 MST 2018
// IP Build 2404404 on Fri Dec  7 01:43:56 MST 2018
// Copyright 1986-2018 Xilinx, Inc. All Rights Reserved.
// ==============================================================

`timescale 1ns/1ps

module comDescriptor_accShg
#(parameter
    ID = 1,             // core ID, unused in RTL
    din0_WIDTH = 352,   // data bitwidth
    din1_WIDTH = 352,     // shift control bitwidth
    dout_WIDTH = 352,   // output bitwidth
    OP = 1,             // opcode: 0-shl, 1-lshr, 2-ashr
    NUM_STAGE = 2       // stage number
)(
    input  wire                  clk,
    input  wire                  reset,
    input  wire                  ce,
    input  wire [din0_WIDTH-1:0] din0,
    input  wire [din0_WIDTH-1:0] din1,
    output reg  [dout_WIDTH-1:0] dout
);

//------------------------Parameter----------------------
localparam K = 1+((din1_WIDTH - 1)/NUM_STAGE);
localparam LATENCY = NUM_STAGE -1;

//------------------------Local signal-------------------
reg  [dout_WIDTH-1:0] dout_array[LATENCY-1:0];
reg  [din1_WIDTH-1:0] din1_cast_array[LATENCY-1:0];
wire [din1_WIDTH-1:0] din1_cast;
wire [din1_WIDTH-1:0] din1_mask;

//------------------------Body---------------------------
always @(posedge clk) begin
    if (reset == 1'b1) begin
        dout_array[0] <= {din0_WIDTH{1'b0}};
        din1_cast_array[0] <= {din1_WIDTH{1'b0}};
    end else if (ce) begin
        case (OP)
            0: dout_array[0] <= din0<<(din1_cast&(din1_mask<<(LATENCY*K)));
            1: dout_array[0] <= din0>>(din1_cast&(din1_mask<<(LATENCY*K)));
            2: dout_array[0] <= $signed(din0)>>>(din1_cast&(din1_mask<<(LATENCY*K)));
            default: dout_array[0] <= dout_array[0];
        endcase
        din1_cast_array[0] <= din1_cast;
    end
end

genvar i;
generate for (i=1; i<LATENCY; i=i+1) begin:pipeshift
    always @(posedge clk) begin
        if (reset == 1'b1) begin
            dout_array[i] <= {din0_WIDTH{1'b0}};
            din1_cast_array[i] <= {din1_WIDTH{1'b0}};
        end else if (ce) begin
            case (OP)
                0: dout_array[i] <= dout_array[i-1]<<(din1_cast_array[i-1]&(din1_mask<<((LATENCY-i)*K)));
                1: dout_array[i] <= dout_array[i-1]>>(din1_cast_array[i-1]&(din1_mask<<((LATENCY-i)*K)));
                2: dout_array[i] <= $signed(dout_array[i-1])>>>(din1_cast_array[i-1]&(din1_mask<<((LATENCY-i)*K)));
                default: dout_array[i] <= dout_array[i];
            endcase
            din1_cast_array[i] <= din1_cast_array[i-1];
        end
    end
end endgenerate

always @(*) begin
    case (OP)
        0: dout = dout_array[LATENCY-1]<<(din1_cast_array[LATENCY-1]&din1_mask);
        1: dout = dout_array[LATENCY-1]>>(din1_cast_array[LATENCY-1]&din1_mask);
        2: dout = $signed(dout_array[LATENCY-1])>>>(din1_cast_array[LATENCY-1]&din1_mask);
        default: dout = {dout_WIDTH{1'b0}};
    endcase
end

assign din1_mask = {{(din1_WIDTH-K){1'b0}},{K{1'b1}}};
assign din1_cast = $unsigned(din1[din1_WIDTH-1:0]);

endmodule
