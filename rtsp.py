import pbcvt
import cv2
rtsp = pbcvt.Rtsp("rtsp://admin:tensorsight666@192.168.1.77:554/h264/ch1/main/av_stream","admin","tensorsight666")
print(" input parameters successfully ! ")
temp = rtsp.rtsp_main()
print(" loading finished ! ")
while temp:
        img = rtsp.ret_to_python()
        if img is None:
                print(" img null ! ")
                res = rtsp.close()
        img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        cv2.imshow("test", img)
        cv2.waitKey(10)
