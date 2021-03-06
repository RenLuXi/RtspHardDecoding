OBJS = rtsp.o pyboostcvconverter.o
target = pbcvt.so
CC = aarch64-linux-gnu-g++

temp_dir := build/temp.linux-aarch64-3.7
$(shell if [ ! -e $(temp_dir) ];then mkdir -p $(temp_dir); fi)

$(target): $(OBJS)
	$(CC) -pthread -shared -Wl,-O1 -Wl,-Bsymbolic-functions -Wl,-z,relro -Wl,-z,relro -g -fstack-protector-strong -Wformat -Werror=format-security -Wdate-time -D_FORTIFY_SOURCE=2 build/temp.linux-aarch64-3.7/*.o -lboost_python3 -o pbcvt.so -lrockchip_rtsp -lrockchip_mpp -lcurl -lrockchip_rga -lpthread -lopencv_core -lopencv_highgui -lopencv_imgcodecs

rtsp.o: class_rtsp.cpp
	$(CC) -pthread -DNDEBUG -g -fwrapv -O2 -Wall -g -fstack-protector-strong -Wformat -Werror=format-security -Wdate-time -D_FORTIFY_SOURCE=2 -fPIC -I/usr/include/python3.7m -c class_rtsp.cpp -o build/temp.linux-aarch64-3.7/rtsp.o

pyboostcvconverter.o: pyboostcvconverter/pyboostcvconverter.cpp
	$(CC) -pthread -DNDEBUG -g -fwrapv -O2 -Wall -g -fstack-protector-strong -Wformat -Werror=format-security -Wdate-time -D_FORTIFY_SOURCE=2 -fPIC -I/usr/include/python3.7m -c pyboostcvconverter/pyboostcvconverter.cpp -o build/temp.linux-aarch64-3.7/pyboostcvconverter.o

.PHONY: clean
clean:
	rm -rf build/temp.linux-aarch64-3.7/*.o  $(target)
