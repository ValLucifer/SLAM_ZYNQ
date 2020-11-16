/***************************************************************************
Copyright (c) 2018, Xilinx, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation 
and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
may be used to endorse or promote products derived from this software 
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ***************************************************************************/

#include "comDescriptor.h"
const char bit_pattern_31_[256][4] =
{
8, -3, 9, 5,
4, 2, 7, -12,
-11, 9, -8, 2,
7, -12, 12, -13,
2, -13, 2, 12,
1, -7, 1, 6,
-2, -10, -2, -4,
-13, -13, -11, -8,
8, -1, 7, 6,
3, 2, 9, -10,
-12, 6, -8, 0,
9, -10, 14, -10,
4, -12, 0, 12,
2, -6, 0, 6,
0, -10, -1, -4,
-10, -15, -9, -9,
8, 0, 6, 8,
2, 3, 11, -8,
-13, 4, -8, -1,
11, -8, 16, -7,
6, -11, -2, 11,
3, -6, -1, 5,
1, -10, 0, -4,
-7, -16, -7, -11,
8, 1, 4, 9,
2, 3, 12, -6,
-14, 1, -7, -2,
12, -6, 17, -4,
8, -9, -5, 11,
4, -5, -2, 5,
3, -9, 0, -4,
-3, -18, -4, -12,
7, 3, 2, 9,
1, 4, 13, -3,
-14, -1, -7, -4,
13, -3, 17, 0,
10, -7, -7, 9,
5, -4, -3, 4,
5, -8, 1, -4,
0, -18, -2, -13,
6, 4, 0, 10,
0, 4, 13, 0,
-13, -4, -6, -5,
13, 0, 17, 2,
11, -5, -8, 8,
6, -3, -4, 4,
7, -7, 2, -3,
3, -18, 0, -13,
5, 6, -1, 10,
0, 4, 13, 1,
-12, -6, -4, -6,
13, 1, 16, 6,
12, -3, -10, 6,
6, -1, -5, 3,
8, -5, 2, -3,
7, -16, 3, -13,
4, 7, -3, 9,
-1, 4, 13, 4,
-10, -9, -3, -7,
13, 4, 15, 9,
13, 0, -11, 4,
7, 0, -5, 2,
9, -3, 3, -2,
10, -15, 5, -12,
3, 8, -4, 9,
-1, 4, 12, 6,
-9, -11, -2, -8,
12, 6, 13, 12,
13, 1, -12, 2,
7, 0, -6, 1,
10, -2, 4, -2,
13, -13, 7, -11,
1, 8, -6, 7,
-2, 3, 10, 9,
-6, -12, 0, -8,
10, 9, 10, 14,
12, 4, -12, 0,
6, 2, -6, 0,
10, 0, 4, -1,
15, -10, 9, -9,
0, 8, -8, 6,
-3, 2, 8, 11,
-4, -13, 1, -8,
8, 11, 7, 16,
11, 6, -11, -2,
6, 3, -5, -1,
10, 1, 4, 0,
16, -7, 11, -7,
-1, 8, -9, 4,
-3, 2, 6, 12,
-1, -14, 2, -7,
6, 12, 4, 17,
9, 8, -11, -5,
5, 4, -5, -2,
9, 3, 4, 0,
18, -3, 12, -4,
-3, 7, -9, 2,
-4, 1, 3, 13,
1, -14, 4, -7,
3, 13, 0, 17,
7, 10, -9, -7,
4, 5, -4, -3,
8, 5, 4, 1,
18, 0, 13, -2,
-4, 6, -10, 0,
-4, 0, 0, 13,
4, -13, 5, -6,
0, 13, -2, 17,
5, 11, -8, -8,
3, 6, -4, -4,
7, 7, 3, 2,
18, 3, 13, 0,
-6, 5, -10, -1,
-4, 0, -1, 13,
6, -12, 6, -4,
-1, 13, -6, 16,
3, 12, -6, -10,
1, 6, -3, -5,
5, 8, 3, 2,
16, 7, 13, 3,
-7, 4, -9, -3,
-4, -1, -4, 13,
9, -10, 7, -3,
-4, 13, -9, 15,
0, 13, -4, -11,
0, 7, -2, -5,
3, 9, 2, 3,
15, 10, 12, 5,
-8, 3, -9, -4,
-4, -1, -6, 12,
10, -9, 8, -2,
-6, 12, -11, 13,
-1, 13, -2, -12,
0, 7, -1, -6,
2, 10, 2, 3,
13, 12, 11, 7,
-8, 1, -7, -6,
-3, -2, -9, 10,
12, -6, 8, 0,
-9, 10, -14, 10,
-4, 12, 0, -12,
-2, 6, 0, -6,
0, 10, 1, 4,
10, 15, 9, 9,
-8, 0, -6, -8,
-2, -3, -11, 8,
13, -4, 8, 1,
-11, 8, -16, 7,
-6, 11, 2, -11,
-3, 6, 1, -5,
-1, 10, 0, 4,
7, 16, 7, 11,
-8, -1, -4, -9,
-2, -3, -12, 6,
14, -1, 7, 2,
-12, 6, -17, 4,
-8, 9, 5, -11,
-4, 5, 2, -5,
-3, 9, 0, 4,
3, 18, 4, 12,
-7, -3, -2, -9,
-1, -4, -13, 3,
14, 1, 7, 4,
-13, 3, -17, 0,
-10, 7, 7, -9,
-5, 4, 3, -4,
-5, 8, -1, 4,
0, 18, 2, 13,
-6, -4, 0, -10,
0, -4, -13, 0,
13, 4, 6, 5,
-13, 0, -17, -2,
-11, 5, 8, -8,
-6, 3, 4, -4,
-7, 7, -2, 3,
-3, 18, 0, 13,
-5, -6, 1, -10,
0, -4, -13, -1,
12, 6, 4, 6,
-13, -1, -16, -6,
-12, 3, 10, -6,
-6, 1, 5, -3,
-8, 5, -2, 3,
-7, 16, -3, 13,
-4, -7, 3, -9,
1, -4, -13, -4,
10, 9, 3, 7,
-13, -4, -15, -9,
-13, 0, 11, -4,
-7, 0, 5, -2,
-9, 3, -3, 2,
-10, 15, -5, 12,
-3, -7, 4, -9,
1, -4, -12, -6,
9, 10, 2, 8,
-12, -6, -13, -11,
-13, -1, 12, -2,
-7, 0, 6, -1,
-10, 2, -3, 2,
-12, 13, -7, 11,
-1, -8, 6, -7,
2, -3, -10, -9,
6, 12, 0, 8,
-10, -9, -10, -14,
-12, -4, 12, 0,
-6, -2, 6, 0,
-10, 0, -4, 1,
-15, 10, -9, 9,
0, -8, 8, -6,
3, -2, -8, -11,
4, 13, -1, 8,
-8, -11, -7, -16,
-11, -6, 11, 2,
-6, -3, 5, 1,
-10, -1, -4, 0,
-16, 7, -11, 7,
1, -8, 9, -4,
3, -2, -6, -12,
1, 14, -2, 7,
-6, -12, -4, -17,
-9, -8, 11, 5,
-5, -4, 5, 2,
-9, -3, -4, 0,
-18, 3, -12, 4,
3, -7, 9, -2,
4, -1, -3, -13,
-1, 14, -4, 7,
-3, -13, 0, -17,
-7, -10, 9, 7,
-4, -5, 4, 3,
-8, -5, -4, -1,
-18, 0, -13, 2,
4, -6, 10, 0,
4, 0, 0, -13,
-4, 13, -5, 6,
0, -13, 2, -17,
-5, -11, 8, 8,
-3, -6, 4, 4,
-7, -7, -3, -2,
-18, -3, -13, 0,
6, -5, 10, 1,
4, 0, 1, -13,
-6, 12, -6, 4,
1, -13, 6, -16,
-3, -12, 6, 10,
-1, -6, 3, 5,
-5, -8, -3, -2,
-16, -7, -13, -3,
7, -4, 9, 3,
4, 1, 4, -13,
-9, 10, -7, 3,
4, -13, 9, -15,
0, -13, 4, 11,
0, -7, 2, 5,
-3, -9, -2, -3,
-15, -10, -12, -5
};
void Descriptors(
		hls::stream<ap_uint<336> > &dataPackStreamIn,
		hls::stream<ap_axiu<32,1,1,1> > &descriptor_out,
		uint16_t img_width,
		uint16_t img_height,
		ap_uint<8> img_level
		)
{
#pragma HLS RESOURCE variable=bit_pattern_31_ core=ROM_1P
#pragma HLS ARRAY_PARTITION variable=bit_pattern_31_ complete dim=2
	unsigned char src_buf0[31][31];
#pragma HLS ARRAY_PARTITION variable=src_buf0 complete dim=1
#pragma HLS ARRAY_PARTITION variable=src_buf0 complete dim=2
	unsigned char src_buf1[31][31];
#pragma HLS ARRAY_PARTITION variable=src_buf1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=src_buf1 complete dim=2

	static hls::stream<ap_uint<5> > res_x0;
#pragma HLS STREAM variable=res_x0 depth=8 dim=1
	static hls::stream<ap_uint<5> > res_y0;
#pragma HLS STREAM variable=res_y0 depth=8 dim=1
	static hls::stream<ap_uint<5> > res_x1;
#pragma HLS STREAM variable=res_x1 depth=8 dim=1
	static hls::stream<ap_uint<5> > res_y1;
#pragma HLS STREAM variable=res_y1 depth=8 dim=1
	ap_uint<72> momVal=0;
	ap_int<24> m_10_int,m_01_int;
	ap_fixed<24,23> m_10,m_01;
	ap_uint<256> descriptor = 0;
	ap_uint<352> shift_reg = 0;
	ap_uint<12>posX,posY;
	unsigned char t0, t1;
	ap_uint<5> x0,y0,x1,y1;
	ap_uint<5> x0_res,y0_res,x1_res,y1_res;
	unsigned char validFlag;
	ap_uint<8> score;
	ap_fixed<14,3> unitAngle = 11.25;
	ap_fixed<14,3> halfUnitAngle = 5.625;
	for(int ss = 0;ss<img_height*(img_width+15);ss++)
	{
		ap_uint<336> datapack = dataPackStreamIn.read();
		ap_uint<248> winVal = datapack.range(247,0);
		m_10_int = datapack.range(271,248);
		m_01_int = datapack.range(295,272);
		posX = datapack.range(307,296);
		posY = datapack.range(319,308);
		validFlag = datapack.range(327,320);
		score = datapack.range(335,328);
		shift_reg.range(287,256) = (int)posX;
		shift_reg.range(319,288) = (int)posY;
		shift_reg.range(351,320) = (int)score;
		read_row_loop:
		for(ap_uint<6> k = 0;k<31;k++)
		{
#pragma HLS UNROLL
#pragma HLS LOOP_TRIPCOUNT MAX=31
			for(ap_uint<8> s = 0;s<31;s++)
			{
#pragma HLS UNROLL
				if(k!=30)
				{
					src_buf0[s][k] = src_buf0[s][k+1];
					src_buf1[s][k] = src_buf1[s][k+1];
				}
				else
				{
					src_buf0[s][30] = (unsigned char)winVal.range(s*8+7,s*8);
					src_buf1[s][30] = (unsigned char)winVal.range(s*8+7,s*8);
				}
			}
		}

		if(validFlag != 0)
		{
			m_10 = m_10_int;
			m_01 = m_01_int;
			ap_fixed<14,3> angle = hls::atan2(m_01, m_10);
			compute_loop:
			for(unsigned int i =0;i<256;i++)
			{
#pragma HLS PIPELINE
				y0_res = bit_pattern_31_[i][1] + 15;
				x0_res = bit_pattern_31_[i][0] + 15;
				y1_res = bit_pattern_31_[i][3] + 15;
				x1_res = bit_pattern_31_[i][2] + 15;
				res_y0.write(y0_res);
				res_x0.write(x0_res);
				res_y1.write(y1_res);
				res_x1.write(x1_res);
				y0 = res_y0.read();
				x0 = res_x0.read();
				y1 = res_y1.read();
				x1 = res_x1.read();
				t0 = src_buf0[y0][x0];
				t1 = src_buf1[y1][x1];
				descriptor.range(i,i) = t0 < t1;
			}
			ap_int<8> times = (ap_int<8>)(angle / unitAngle);
			ap_int<8> bitsToShift = times << 3;
			if(angle - (times * unitAngle) > halfUnitAngle)
				bitsToShift += 8;
			for (unsigned int i = 0; i < 256; i++)
#pragma HLS PIPELINE
				shift_reg[i] = descriptor[(i + bitsToShift) & 0x000000ff];
//			shift_reg(bitsToShift-1,0)=descriptor.range(bitsToShift-1,0);
//			descriptor.range(255-bitsToShift,0)=descriptor.range(255,bitsToShift);
//			descriptor.range(255,256-bitsToShift)=shift_reg(bitsToShift-1,0);
//			shift_reg.range(255, 0) =
//					(descriptor.range(255, 0) >> (256 - bitsToShift) | (descriptor.range(255, 0) << bitsToShift));
			output_loop:
			for (unsigned int i = 0; i < 11; i++)
			{
#pragma HLS PIPELINE
				ap_axiu<32,1,1,1> outVal;
				ap_uint<32> temp = shift_reg.range((i<<5)+31,i<<5);
				outVal.data = temp;
				outVal.keep = 0xf;
				if(posX == 4095 && posY == 4095 && i==10)
					outVal.last = 1;
				else
					outVal.last = 0;
				descriptor_out.write(outVal);
			}
		}
	}
}

template<int ROWS, int COLS, int DEPTH, int NPC, int WORDWIDTH, int TC, int WIN_SZ, int WIN_SZ_SQ>
void lineProcess(hls::stream<ap_uint<8> > & _src_mat,
		hls::stream<ap_uint<88> > &dataPackStreamIn,
		hls::stream<ap_uint<336> > &dataPackStreamOut,
		XF_SNAME(WORDWIDTH) buf[WIN_SZ][(COLS >> XF_BITSHIFT(NPC))],
		XF_PTNAME(DEPTH) src_buf[WIN_SZ][XF_NPIXPERCYCLE(NPC)+(WIN_SZ-1)],
		uint16_t img_width,
		uint16_t img_height,
		ap_uint<13> row_ind[WIN_SZ],
		ap_uint<13> row,
		ap_uint<8> win_size)
{
#pragma HLS INLINE

	XF_SNAME(WORDWIDTH) buf_cop[WIN_SZ];
#pragma HLS ARRAY_PARTITION variable=buf_cop complete dim=1

	uint16_t npc = XF_NPIXPERCYCLE(NPC);
	uint16_t col_loop_var = 0;
	ap_uint<248> winVal = 0;
	ap_uint<88> dataIn;
	if(npc == 1)
	{
		col_loop_var = (WIN_SZ>>1);
	}
	else
	{
		col_loop_var = 1;
	}
	for(int extract_px=0;extract_px<WIN_SZ;extract_px++)
	{
#pragma HLS LOOP_TRIPCOUNT min=WIN_SZ max=WIN_SZ
#pragma HLS unroll
		for(int ext_copy=0; ext_copy<npc + WIN_SZ - 1; ext_copy++)
		{
#pragma HLS unroll
			src_buf[extract_px][ext_copy] = 0;
		}
	}

	Col_Loop:
	for(ap_uint<13> col = 0; col < ((img_width)>>XF_BITSHIFT(NPC)) + col_loop_var; col++)
	{
#pragma HLS LOOP_TRIPCOUNT min=TC max=TC
#pragma HLS pipeline
#pragma HLS LOOP_FLATTEN OFF
		if(row < img_height && col < (img_width>>XF_BITSHIFT(NPC)))
			buf[row_ind[win_size-1]][col] = _src_mat.read(); // Read data
		if(NPC == XF_NPPC8)
		{
		}
		else
		{
			for(int copy_buf_var=0;copy_buf_var<WIN_SZ;copy_buf_var++)
			{
#pragma HLS LOOP_TRIPCOUNT min=WIN_SZ max=WIN_SZ
#pragma HLS UNROLL
				if(	(row >(img_height-1)) && (copy_buf_var>(win_size-1-(row-(img_height-1)))))
				{
					buf_cop[copy_buf_var] = buf[(row_ind[win_size-1-(row-(img_height-1))])][col];
				}
				else
				{
					buf_cop[copy_buf_var] = buf[(row_ind[copy_buf_var])][col];
				}
			}
			for(int extract_px=0;extract_px<WIN_SZ;extract_px++)
			{
#pragma HLS LOOP_TRIPCOUNT min=WIN_SZ max=WIN_SZ
#pragma HLS UNROLL
				if(col<img_width)
				{
					src_buf[extract_px][win_size-1] = buf_cop[extract_px];
					winVal.range((extract_px<<3)+7,extract_px<<3) = buf_cop[extract_px];
				}
				else
				{
					src_buf[extract_px][win_size-1] = src_buf[extract_px][win_size-2];
					winVal.range((extract_px<<3)+7,extract_px<<3) = src_buf[extract_px][win_size-2];
				}
			}
			if(img_height-1 == row-15 && img_width-1 == col-15)
			{
				dataIn= dataPackStreamIn.read();
				dataIn.range(59,48) = 4095;
				dataIn.range(71,60) = 4095;
				dataIn.range(79,72) = 255;
			}
			else if(col >= (WIN_SZ>>1))
			{
				dataIn= dataPackStreamIn.read();
			}
			else
			{
				dataIn = 0;
			}
			ap_uint<336> temp;
			temp.range(247,0) = winVal;
			temp.range(335,248) = dataIn;
			dataPackStreamOut.write(temp);
			for(int wrap_buf=0;wrap_buf<WIN_SZ;wrap_buf++)
			{
#pragma HLS UNROLL
#pragma HLS LOOP_TRIPCOUNT min=WIN_SZ max=WIN_SZ
				for(int col_warp=0; col_warp<WIN_SZ-1;col_warp++)
				{
#pragma HLS UNROLL
#pragma HLS LOOP_TRIPCOUNT min=WIN_SZ max=WIN_SZ
					if(col==0)
					{
						src_buf[wrap_buf][col_warp] = src_buf[wrap_buf][win_size-1];
					}
					else
					{
						src_buf[wrap_buf][col_warp] = src_buf[wrap_buf][col_warp+1];
					}
				}
			}
		}
	} // Col_Loop
}



template<int ROWS, int COLS, int DEPTH, int NPC, int WORDWIDTH, int TC,int WIN_SZ, int WIN_SZ_SQ>
void windowSliderProc(hls::stream<ap_uint<8> > &img,
		hls::stream<ap_uint<88> > &dataPackStreamIn,
		hls::stream<ap_uint<336> > &dataPackStreamOut,
		ap_uint<8> win_size,
		uint16_t img_height,
		uint16_t img_width
		)
{
	ap_uint<13> row_ind[WIN_SZ];
#pragma HLS ARRAY_PARTITION variable=row_ind complete dim=1


	uint16_t shift_x = 0;
	ap_uint<13> row, col;

	XF_PTNAME(DEPTH) src_buf[WIN_SZ][XF_NPIXPERCYCLE(NPC)+(WIN_SZ-1)];
#pragma HLS ARRAY_PARTITION variable=src_buf complete dim=1
#pragma HLS ARRAY_PARTITION variable=src_buf complete dim=2
	// src_buf1 et al merged

	XF_SNAME(WORDWIDTH) buf[WIN_SZ][(COLS >> XF_BITSHIFT(NPC))];
#pragma HLS ARRAY_PARTITION variable=buf complete dim=1
#pragma HLS RESOURCE variable=buf core=RAM_S2P_BRAM

	//initializing row index

	for(int init_row_ind=0; init_row_ind<win_size; init_row_ind++)
	{
#pragma HLS LOOP_TRIPCOUNT min=WIN_SZ max=WIN_SZ
		row_ind[init_row_ind] = init_row_ind;
	}

	read_lines:
	for(int init_buf=row_ind[win_size>>1]; init_buf <row_ind[win_size-1] ;init_buf++)
	{
#pragma HLS LOOP_TRIPCOUNT min=WIN_SZ max=WIN_SZ
		for(col = 0; col < img_width>>XF_BITSHIFT(NPC) ; col++)
		{
#pragma HLS LOOP_TRIPCOUNT min=TC max=TC
#pragma HLS pipeline
#pragma HLS LOOP_FLATTEN OFF
			buf[init_buf][col] = img.read();
		}
	}

	//takes care of top borders
	for(col = 0; col < img_width>>XF_BITSHIFT(NPC); col++)
	{
#pragma HLS LOOP_TRIPCOUNT min=TC max=TC
		for(int init_buf=0; init_buf < WIN_SZ>>1;init_buf++)
		{
#pragma HLS LOOP_TRIPCOUNT min=WIN_SZ max=WIN_SZ
#pragma HLS UNROLL
			buf[init_buf][col] = 0;//buf[row_ind[win_size>>1]][col];
		}
	}

	Row_Loop:
	for(row = (win_size>>1); row < img_height+(win_size>>1); row++)
	{
#pragma HLS LOOP_TRIPCOUNT min=ROWS max=ROWS

		lineProcess<ROWS, COLS, DEPTH, NPC, WORDWIDTH, TC, WIN_SZ, WIN_SZ_SQ>(img, dataPackStreamIn,dataPackStreamOut,buf, src_buf,img_width, img_height, row_ind, row,win_size);

		//update indices
		ap_uint<13> zero_ind = row_ind[0];
		for(int init_row_ind=0; init_row_ind<WIN_SZ-1; init_row_ind++)
		{
#pragma HLS LOOP_TRIPCOUNT min=WIN_SZ max=WIN_SZ
#pragma HLS UNROLL
			row_ind[init_row_ind] = row_ind[init_row_ind + 1];
		}
		row_ind[win_size-1] = zero_ind;
	} // Row_Loop
}

template<int NMS,int SRC_T,int ROWS, int COLS,int NPC=1>
void windowSlider(
		hls::stream<ap_uint<8> > &src,
		hls::stream<ap_uint<88> > &dataPackStreamIn,
		hls::stream<ap_axiu<32,1,1,1> > & descriptor_out,
		unsigned short _image_height,unsigned short _image_width,
		ap_uint<8> img_level)
{
#pragma HLS inline off
#pragma HLS DATAFLOW
	hls::stream<ap_uint<336> > dataPackStream;
	windowSliderProc<ROWS,COLS,XF_DEPTH(SRC_T,NPC),NPC,XF_WORDWIDTH(SRC_T,NPC),(COLS>>XF_BITSHIFT(NPC))+(31>>1),31, 31*31>(src, dataPackStreamIn,dataPackStream,31,_image_height,_image_width);
	Descriptors(dataPackStream,descriptor_out,_image_width,_image_height,img_level);
}

void comDescriptor_accel(hls::stream<ap_uint<8> > &srcStream,hls::stream<ap_uint<88> > & dataStreamIn,hls::stream<ap_axiu<32,1,1,1> > & descriptor_out)
{
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS INTERFACE axis register both port=srcStream
#pragma HLS INTERFACE axis register both port=dataStreamIn
#pragma HLS INTERFACE axis register both port=descriptor_out
	ap_uint<8> img_level = srcStream.read();
	ap_uint<88> img_level2 = dataStreamIn.read();
	unsigned short _image_height[4]={480,320,213,142};
	unsigned short _image_width[4]={640,427,284,190};
	windowSlider<1,XF_8UC1,HEIGHT,WIDTH,1>(srcStream,dataStreamIn,descriptor_out,_image_height[img_level],_image_width[img_level],img_level);
}
