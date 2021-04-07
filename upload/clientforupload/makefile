all:upload
upload:main.o GetIPAddress.o
	g++ -g -o upload main.o GetIPAddress.o
main.o:main.cpp UploadInclude.h GetIPAddress.h 
	g++ -g -c main.cpp
GetIPAddress.o:GetIPAddress.cpp UploadInclude.h GetIPAddress.h
	g++ -g -c GetIPAddress.cpp
clean:all
	rm all
