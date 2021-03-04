import cv2
import pbcvt
'''
pbcvt封装库API

Rtsp(rtsp,user,passwd)
初始化
:param rtsp:rtsp流地址
:param user:用户名
:param passwd:密码
:returns:

rtsp_main()
捕获码流并初始化配置
:returns: ture or false

ret_to_python()
：returns: NULL or NDArray

close()
：returns: 0 代表成功关闭
'''
rtsp = pbcvt.Rtsp("rtsp://av_stream","admin","123456")
print(" input parameters successfully ! ")
temp = rtsp.rtsp_main()
while temp:
        img = rtsp.ret_to_python()
        if img is None:
                print(" img null ! ")
               # res = rtsp.close()
                continue
        img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        cv2.imshow("test", img)
        cv2.waitKey(10)
