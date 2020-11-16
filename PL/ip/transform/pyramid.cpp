 #include "hls_video.h"
#define MAX_WIDTH  640
#define MAX_HEIGHT 480

template<int SRC_T, int ROWS,int COLS>
void Resize_opr_linear_level (
		hls::Mat<ROWS, COLS, SRC_T>	    &_src,
		unsigned short drows,unsigned short dcols,
		hls::stream<ap_uint<8> > & out_stream)
{
	hls::LineBuffer<2,COLS+1,hls::Scalar<HLS_MAT_CN(SRC_T),HLS_TNAME(SRC_T)> >	    k_buf;
	hls::Window<2,2,hls::Scalar<HLS_MAT_CN(SRC_T),HLS_TNAME(SRC_T)> >   win;
    short     srows=_src.rows;
    short     scols=_src.cols;
    ap_fixed<32,16,AP_RND>    	row_rate=((ap_fixed<32,16,AP_RND> )srows)/(ap_fixed<32,16,AP_RND>)drows;
    ap_fixed<32,16,AP_RND>    	col_rate=((ap_fixed<32,16,AP_RND> )scols)/(ap_fixed<32,16,AP_RND>)dcols;
    typename hls::filter2d_traits<HLS_TNAME(SRC_T), ap_fixed<20,2,AP_RND> ,4>::FILTER_CAST_T	u,v, u1,v1;

    hls::Scalar<HLS_MAT_CN(SRC_T),HLS_TNAME(SRC_T)> s, temp, d;
    ap_fixed<4,2,AP_RND> 		par=0.5;
    ap_fixed<20,10,AP_RND> 		offset_row=row_rate/2-par;
    ap_fixed<20,10,AP_RND> 		offset_col=col_rate/2-par;
    ap_fixed<32,16,AP_RND> 		fx=0;
    ap_fixed<32,16,AP_RND> 		fy=0;

    short     rows=srows > drows ? srows : (drows+1);
    short     cols=scols > dcols ? scols : (dcols+1);
    assert(rows<=ROWS || rows<=drows+1);
    assert(cols<=COLS || cols<=dcols+1);

    short x=0;
    short pre_fy=-10;
    short pre_fx=-10;
    bool row_rd=false;
    bool row_wr=false;

    for(short i= 0;i<rows;i++) {
        for(short j= 0;j<cols;j++) {
#pragma HLS LOOP_FLATTEN OFF
#pragma HLS DEPENDENCE array inter false
#pragma HLS PIPELINE

            bool col_rd=false;
            bool col_wr=false;

            short dy = row_rate>1 ? (short)(i/row_rate):(i-1);
            short dx = col_rate>1 ? (short)(j/col_rate):(j-1);

            fy =(dy)*row_rate+offset_row;
            fx =(dx)*col_rate+offset_col;

            short sx=(short)fx;
            short sy=(short)fy;
            if(fx-sx>0)
                u=fx-sx;
            else
                u=0;
            if(fy-sy>0)
                v=fy-sy;
            else
                v=0;
            u1=1-u;
            v1=1-v;
            if(sx>scols-1)
            {
                sx=scols-1;
                u=0;
            }
            if(sy>srows-1)
            {
                sy=srows-1;
                v=0;
            }
            if(j==0)
            {
                x=0;
                pre_fx=-10;
                if(row_rate>1)
                {
                    row_rd=true;
                    row_wr= (sy==(i-1)) ? true : false;
                }
                else
                {
                    if(i==0){
                        row_rd=true;
                    }
                    else if(sy!=pre_fy)
                    {
                        row_rd=true;
                        pre_fy=sy;
                    }
                    else {
                        row_rd=false;
                    }
                    row_wr= i>0? true: false;
                }
            }
            if(col_rate>1)
            {
                col_rd=true;
                col_wr= (sx==(j-1)) ? true : false;
            }else{
                if(j==0){
                    col_rd=true;
                }
                else if(sx!=pre_fx)
                {
                    col_rd=true;
                    pre_fx=sx;
                }
                else
                    col_rd=false;
                col_wr= j>0? true: false;
            }
            if(col_rd)
            {
                for(int r= 0;r<2;r++)
                {
                    win.val[r][1]=win.val[r][0];
                }
                if(row_rd)
                {

                    k_buf.val[1][x]=k_buf.val[0][x];
                    win.val[1][0]=k_buf.val[0][x];
                    if(sy<srows-1&&sx<scols-1)
                    {
                        _src >> s;
                        k_buf.val[0][x]=s;
                        win.val[0][0]=k_buf.val[0][x];
                    }
                    else if(sx>=scols-1&&sy<srows-1){
                        k_buf.val[0][x]=s;//border
                    }
                    else if(sy>=srows-1){
                        win.val[0][0]=k_buf.val[0][x];
                    }
                }
                else
                {
                    for(int r= 0;r<2;r++)
                    {
                        win.val[r][0]=k_buf.val[r][x];
                    }
                }

                x++;
            }
            if(row_wr && col_wr)
            {
                for(int k=0;k<HLS_MAT_CN(SRC_T);k++)
                {
                    typename hls::filter2d_traits<HLS_TNAME(SRC_T), ap_fixed<15,1,AP_RND> ,4>::ACCUM_T t=0;
                    typedef typename hls::fixed_type<HLS_TNAME(SRC_T)>::T SRCT;

                    t=((SRCT)win.val[1][1].val[k])*u1*v1+
                        ((SRCT)win.val[1][0].val[k])*v1*u+
                        ((SRCT)win.val[0][1].val[k])*u1*v+
                        ((SRCT)win.val[0][0].val[k])*u*v;
                    d.val[k]=hls::sr_cast<HLS_TNAME(SRC_T)>(t);
                }
                out_stream.write(d.val[0]);
            }
        }
    }
}

void transform_level(hls::stream<ap_axiu<8,1,1,1> > & in_stream, hls::stream<ap_uint<8> > & out_stream, ap_uint<8> img_level)
{
#pragma HLS dataflow
	unsigned short resize_height[4]={480,320,213,142};
	unsigned short resize_width[4]={640,427,284,190};
	hls::Mat<MAX_HEIGHT, MAX_WIDTH, HLS_8UC1> src_bw(480,640);
	for(int r = 0; r < 480; r++){
		for(int c = 0; c < 640; c++)
		{
#pragma HLS pipeline II=1
			hls::Scalar<1, unsigned char> pixel;
			pixel.val[0] = in_stream.read().data;
			src_bw.write(pixel);
		}
	}
	Resize_opr_linear_level(src_bw,resize_height[img_level],resize_width[img_level],out_stream);
}

void transform_level_0(hls::stream<ap_axiu<8,1,1,1> > & in_stream, hls::stream<ap_uint<8> > & out_stream)
{
#pragma HLS dataflow
	for(int r = 0; r < 480; r++){
		for(int c = 0; c < 640; c++)
		{
#pragma HLS pipeline II=1
			ap_uint<8> pixel;
			pixel = in_stream.read().data;
            out_stream.write(pixel);
		}
	}
}

void transform(hls::stream<ap_axiu<8,1,1,1> > & in_stream, hls::stream<ap_uint<8> > & out_stream){
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS INTERFACE axis register both port=in_stream
#pragma HLS INTERFACE axis register both port=out_stream
	ap_uint<8> img_level = in_stream.read().data;
	if (img_level!=0)
	{
		out_stream.write(img_level);
		transform_level(in_stream,out_stream,img_level);

	}
	else
	{
		out_stream.write(img_level);
		transform_level_0(in_stream,out_stream);
	}
}
