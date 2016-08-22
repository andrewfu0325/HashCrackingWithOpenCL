# Hash Cracking with OpenCL #

* The repository consists of four files: main.cpp, md5.cl, genKey.h and genKey.cpp.
* We use brutal-force method to crack the md5 hashing with the help of OpenCL.
* With one single kernel file(md5.cl), we can dispatch the same kernel code onto either CPU or GPU.
* We split the amount of work for CPU and GPU statically(offline) by passing command line argument (See below).

### Usage
        1. EXPORT CL_HEADER=/WHRER/cl.h/IS/
        2. Make sure libOpenCL.so is in your LD_LIBRARY_PATH
        3. make
        4. echo -n STRING_TO_CRACK | md5sum
        5. ./a.out [GPU_WORK](MIN=0, MAX=16) [MD5_STRING]

### Auto-testing 
        ./go.sh [MD5_STRING]
        Suggest use a 6-letter (or up) word to see the differences
        Ex. md5(little)=aae6635e044ac56046b2893a529b5114
