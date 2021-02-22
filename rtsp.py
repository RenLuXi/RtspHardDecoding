import pbcvt
import cv2
rtsp = pbcvt.Rtsp("rtsp address","user","passwd")
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
