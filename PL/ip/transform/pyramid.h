#include "hls_video.h"
#include <ap_fixed.h>

#define MAX_WIDTH  640
#define MAX_HEIGHT 480
#define CHANNEL	   3

typedef hls::Scalar<1, unsigned char>                 GRAY_PIXEL;
typedef hls::Mat<MAX_HEIGHT, MAX_WIDTH, HLS_8UC1>     GRAY_IMAGE;

void pyramid(hls::stream<ap_axiu<8,1,1,1> > & in_stream, hls::stream<ap_uint<8> > & out_stream,hls::stream<ap_uint<8> > & out_stream2);
