//
// Created by lucy on 2020/10/29.封装版本
// 说明：完成于2020/11/02,类
//　bug修复已完成
//
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <thread>
#include <memory.h>
#include <sys/time.h>
#include <queue>
#include <ctime>

#define RTSP_DEBUG 0

#define PY_ARRAY_UNIQUE_SYMBOL pbcvt_ARRAY_API
#include <boost/python.hpp>
#include "pyboostcvconverter/pyboostcvconverter.hpp"
#if (PY_VERSION_HEX >= 0x03000000)
static void *init_ar() {
#else
static void init_ar(){
#endif
    Py_Initialize();

    import_array();
    return NUMPY_IMPORT_ARRAY_RETVAL;
}

using namespace std;

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <rockchip/rockchip_rtsp.h>
#include <rockchip/rockchip_mpp.h>
extern "C" {
#include <rockchip/rockchip_rga.h>
}

static MppDecoder *mpp_dec = NULL;
static RockchipRga *rga;
int resize_w = 1000,resize_h = 1000;
static int frame_size = resize_w * resize_h * 3;
unsigned char *frame_rgb = NULL;
clock_t time_start=0;
clock_t time_end;

// 打印当前时间
// time_t t=time(0);
// tm *dt=localtime(&t);
// cout<<dt->tm_hour<<":"<<dt->tm_min<<":"<<dt->tm_sec<<endl;
unsigned long get_time(void)
{
    struct timeval ts;
    gettimeofday(&ts, NULL);
    return (ts.tv_sec * 1000 + ts.tv_usec / 1000);
}

void onRtspHandle(unsigned char *buf, size_t len)
{
    #if RTSP_DEBUG
    std::cout << "frame recived " << len << std::endl;
    #endif
    mpp_dec->ops->enqueue(mpp_dec, buf, len);
}


namespace pbcvt {
    using namespace boost::python;
    struct Rtsp
    {
    public:
        RtspClient rtsp_client;

        Rtsp(string url,string user,string pwd):rtsp_client(url, user, pwd){
            frame_rgb = (unsigned char *)malloc(frame_size);
            
            cout << "[ RTSP_URL  ] " << url << endl;
            cout << "[ RTSP_USER ] " << user << endl;
            cout << "[ RTSP_PWD  ] " << pwd << endl;
        }

        PyObject *ret_to_python(void);
        bool rtsp_main(void);
        int close(void);

    };

    BOOST_PYTHON_MODULE(pbcvt)
            {
                    init_ar();
            to_python_converter<cv::Mat,pbcvt::matToNDArrayBoostConverter>();
            matFromNDArrayBoostConverter();
            class_<Rtsp>("Rtsp",init<string,string,string>())
                .def("rtsp_main", &Rtsp::rtsp_main)
                .def("ret_to_python", &Rtsp::ret_to_python)
                .def("close", &Rtsp::close);
            }

    PyObject * Rtsp::ret_to_python(void)
    {
        #if RTSP_DEBUG
            time_start=clock();
        #endif
        int ret;
        DecFrame *frame = mpp_dec->ops->dequeue_timeout(mpp_dec, 300);
        if (frame == NULL) {
            return NULL;
        }
        cv::Mat img(resize_h , resize_w  , CV_8UC3, frame_rgb);
        if (!frame_rgb) {
            return NULL;
        }
        rga->ops->setSrcBufferPtr(rga, frame->data);
        ret = rga->ops->go(rga);
        if (!ret) {
            mpp_dec->ops->freeFrame(frame);
            PyObject *res = pbcvt::fromMatToNDArray(img);
            #if RTSP_DEBUG
                time_end=clock();
                cout<<"[ c++ ] "<<(double)(time_end-time_start)/CLOCKS_PER_SEC<<"s"<<endl;
            #endif
            return res;
        }
        else {
            return NULL;
        }

    }

    bool Rtsp::rtsp_main(void) {
        rtsp_client.setDataCallback(onRtspHandle);
        mpp_dec = MppDecoderCreate(DECODE_TYPE_H264);
        if (!mpp_dec) {
            std::cout << "MppDecoderCreate error!\n" << std::endl;
            return false;
        }

        rga = RgaCreate();
        if (!rga) {
            MppDecoderDestroy(mpp_dec);
            std::cout << "rgaCreate error!\n" << std::endl;
            return false;
        }
        int run = 1;
        rtsp_client.enable();
        while (run) {
            DecFrame *frame = mpp_dec->ops->dequeue_timeout(mpp_dec, 300);
            if (frame != NULL){
                run = 0;
                int width = frame->width;
                int height = frame->height;

                rga->ops->initCtx(rga);
                rga->ops->setRotate(rga, RGA_ROTATE_NONE);
                rga->ops->setSrcFormat(rga, V4L2_PIX_FMT_NV12, width, height);
                rga->ops->setDstFormat(rga, V4L2_PIX_FMT_RGB24, resize_w, resize_h);

                mpp_dec->ops->freeFrame(frame);

                rga->ops->setDstBufferPtr(rga, frame_rgb);
                #if RTSP_DEBUG
                    cout<<" true "<< endl;
                #endif
                return true;
            }
            else {
                std::cout << "frame NULL!\n" << std::endl;
            }
        }
    }

    int Rtsp::close(void)
    {
        rtsp_client.disable();
        RgaDestroy(rga);

        MppDecoderDestroy(mpp_dec);

        free(frame_rgb);
        return 0;
    }
}



