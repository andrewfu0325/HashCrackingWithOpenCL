
main:main.cpp genKey.o
	g++ -I/usr/local/cuda/include main.cpp genKey.o -lOpenCL
genKey.o:
	g++ genKey.cpp -c
clean:
	rm *.o a.out
