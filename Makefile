main:main.cpp genKey.o
	g++ -I${CL_HEADER} main.cpp genKey.o -lOpenCL
genKey.o:
	g++ genKey.cpp -c
clean:
	rm *.o a.out
