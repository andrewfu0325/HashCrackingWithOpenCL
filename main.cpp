
#include "genKey.h"
#include "time.h"
#include <CL/cl.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#define SUCCESS 0
#define FAILURE 1
#define ENABLE_PROFILEING 0
#define GPU_FILE_NAME "md5.cl"
#define CPU_FILE_NAME "md5.cl"
#define P64 16
#define GPU_KERNEL_NAME "md5"
#define CPU_KERNEL_NAME "md5"


#define GPU_ONLY 0
using namespace std;
int len = GPU + 1;

/* convert the kernel file into a string */
int convertToString(const char *filename, std::string& s)
{
  size_t size;
  char*  str;
  std::fstream f(filename, (std::fstream::in | std::fstream::binary));

  if (f.is_open())
  {
    size_t fileSize;
    f.seekg(0, std::fstream::end);
    size = fileSize = (size_t)f.tellg();
    f.seekg(0, std::fstream::beg);
    str = new char[size + 1];
    if (!str)
    {
      f.close();
      return 0;
    }

    f.read(str, fileSize);
    f.close();
    str[size] = '\0';
    s = str;
    delete[] str;
    return 0;
  }
  cout << "Error: failed to open file\n:" << filename << endl;
  return FAILURE;
}


inline int hex2dec(char h) {
  if (h >= '0' && h <= '9') {
    return h - '0';
  }
  else {
    return h - 'a' + 10;
  }
}
void hex_decode(unsigned char *buf, const char *hex) {
  for (int i = 0; i < 16; ++i) {
    buf[i] = hex2dec(hex[i * 2]) * 16 + hex2dec(hex[i * 2 + 1]);
  }
}

int main(int argc, char* argv[])
{
  if(argc != 2) {
    cout << "Usage: ./a.out [GPU_WORK](MIN=0, MAX=16)\n"
         << "The CPU and GPU will co-work to crack the md5 hashing\n"
         << "CPU_WORK = 16 - GPU_WORK\n";
     return FAILURE;
  }
  int GPU_EXE = atoi(argv[1]);
  if(GPU_EXE < 0 || GPU_EXE > 16) {
     cout << "The amount of work for GPU_EXE must be between 0 and 16\n";
     return FAILURE;
  }
  int CPU_EXE = P64 - GPU_EXE;

  /*Step1: Getting platforms and choose an available one.*/
  cl_uint numPlatforms;  //the NO. of platforms
  cl_platform_id *platforms;  //the chosen platform
  cl_event md5_evt;
  cl_int  status = clGetPlatformIDs(0, NULL, &numPlatforms);

  if (status != CL_SUCCESS)
  {
    cout << "Error: Getting platforms!" << endl;
    return FAILURE;
  }

  platforms = (cl_platform_id*)malloc(numPlatforms* sizeof(cl_platform_id));
  status = clGetPlatformIDs(numPlatforms, platforms, NULL);
  size_t retLen;

  /* Probe the platforms */
  for (unsigned i = 0; i < numPlatforms; i++){
    char *devName;
    clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, 0, NULL, &retLen);
    devName = (char*)malloc(retLen * sizeof(char));
    clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, retLen, devName, NULL);
    printf("The name of Device on Platform[%d]: %s\n", i, devName);
  }

  cl_device_id cpuDev, gpuDev;
  status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_CPU, 1, &cpuDev, NULL);
  if (status != CL_SUCCESS)
  {
    cout << status << endl;
    cout << "Error: Getting CPU device!" << endl;
    return FAILURE;
  }
  status = clGetDeviceIDs(platforms[1], CL_DEVICE_TYPE_GPU, 1, &gpuDev, NULL);
  if (status != CL_SUCCESS)
  {
    cout << "Error: Getting GPU device!" << endl;
    return FAILURE;
  }

  /*Step 3: Create context.*/
  cl_context gpuContext = clCreateContext(NULL, 1, &gpuDev, NULL, NULL, &status);
  if (status != CL_SUCCESS)
  {
    cout << "Error: Creating GPU context!" << endl;
    return FAILURE;
  }
  cl_context cpuContext = clCreateContext(NULL, 1, &cpuDev, NULL, NULL, &status);
  if (status != CL_SUCCESS)
  {
    cout << "Error: Creating CPU context!" << endl;
    return FAILURE;
  }

  /*Step 4: Creating command queue associate with the context.*/
#ifdef EANBLE_PROFILE
  cl_command_queue gpuCmmdQue = clCreateCommandQueue(gpuContext, gpuDev, CL_QUEUE_PROFILING_ENABLE, &status);
  if (status != CL_SUCCESS)
  {
    cout << "Error: Creating GPU command queue!" << endl;
    return FAILURE;
  }
  cl_command_queue cpuCmmdQue = clCreateCommandQueue(cpuContext, cpuDev, CL_QUEUE_PROFILING_ENABLE, &status);
  if (status != CL_SUCCESS)
  {
    cout << "Error: Creating CPU command queue!" << endl;
    return FAILURE;
  }
#else
  cl_command_queue gpuCmmdQue = clCreateCommandQueue(gpuContext, gpuDev, 0, &status);
  if (status != CL_SUCCESS)
  {
    cout << "Error: Creating GPU command queue!" << endl;
    return FAILURE;
  }
  cl_command_queue cpuCmmdQue = clCreateCommandQueue(cpuContext, cpuDev, 0, &status);
  if (status != CL_SUCCESS)
  {
    cout << status << endl;
    cout << "Error: Creating CPU command queue!" << endl;
    return FAILURE;
  }
#endif

  /*Step 5: Create program object */
  const char *gpuFileName = GPU_FILE_NAME;
  string sourceStr;
  status = convertToString(gpuFileName, sourceStr);
  const char *source = sourceStr.c_str();
  size_t gpuSourceSize[] = { strlen(source) };
  cl_program gpuProgram = clCreateProgramWithSource(gpuContext, 1, &source, gpuSourceSize, &status);
  if (status != CL_SUCCESS)
  {
    cout << "Error: Creating GPU program!" << endl;
    return FAILURE;
  }

  const char *cpuFileName = CPU_FILE_NAME;
  status = convertToString(cpuFileName, sourceStr);
  source = sourceStr.c_str();
  size_t cpuSourceSize[] = { strlen(source) };
  cl_program cpuProgram = clCreateProgramWithSource(cpuContext, 1, &source, cpuSourceSize, &status);
  if (status != CL_SUCCESS)
  {
    cout << "Error: Creating CPU program!" << endl;
    return FAILURE;
  }

  /*Step 6: Build program. */
  status = clBuildProgram(gpuProgram, 1, &gpuDev, NULL, NULL, NULL);
  if (status != CL_SUCCESS)
  {
    cout << "Error: Building GPU program!" << endl;
    return FAILURE;
  }
  status = clBuildProgram(cpuProgram, 1, &cpuDev, NULL, NULL, NULL);
  if (status != CL_SUCCESS)
  {
    cout << "Error: Building CPU program!" << endl;
    return FAILURE;
  }

  /*Step 7: Initial input,output for the host and create memory objects for the kernel*/
  cl_mem gpuKeyBuffer = clCreateBuffer(gpuContext, CL_MEM_READ_WRITE, K * MAXLENGTH * sizeof(char), NULL, &status);
  if (status != CL_SUCCESS)
  {
    cout << "Error: Creating Str Buffer!" << endl;
    return FAILURE;
  }

  cl_mem gpuHashBuffer = clCreateBuffer(gpuContext, CL_MEM_READ_ONLY, HASHLENGTH * sizeof(char), NULL, &status);
  if (status != CL_SUCCESS)
  {
    cout << "Error: Creating Str Buffer!" << endl;
    return FAILURE;
  }

  cl_mem cpuKeyBuffer = clCreateBuffer(cpuContext, CL_MEM_READ_WRITE, K * MAXLENGTH * sizeof(char), NULL, &status);
  if (status != CL_SUCCESS)
  {
    cout << "Error: Creating Str Buffer!" << endl;
    return FAILURE;
  }

  cl_mem cpuHashBuffer = clCreateBuffer(cpuContext, CL_MEM_READ_ONLY, HASHLENGTH * sizeof(char), NULL, &status);
  if (status != CL_SUCCESS)
  {
    cout << "Error: Creating Str Buffer!" << endl;
    return FAILURE;
  }

  /*Step 8: Create kernel object */
  cl_kernel gpuKernel = clCreateKernel(gpuProgram, GPU_KERNEL_NAME, &status);
  if (status != CL_SUCCESS)
  {
    cout << "Error: Creating GPU kernel!" << endl;
    return FAILURE;
  }
  cl_kernel cpuKernel = clCreateKernel(cpuProgram, CPU_KERNEL_NAME, &status);
  if (status != CL_SUCCESS)
  {
    cout << "Error: Creating CPU kernel!" << endl;
    return FAILURE;
  }


  /*Step 9: Sets Kernel arguments.*/
  status = clSetKernelArg(gpuKernel, 0, sizeof(cl_mem), (void *)&gpuKeyBuffer);
  if (status != CL_SUCCESS)
  {
    cout << "Error: Setting GPU Kernel arguments!" << endl;
    return FAILURE;
  }
  status = clSetKernelArg(gpuKernel, 2, sizeof(cl_mem), (void *)&gpuHashBuffer);
  if (status != CL_SUCCESS)
  {
    cout << "Error: Setting GPU Kernel arguments!" << endl;
    return FAILURE;
  }
  status = clSetKernelArg(cpuKernel, 0, sizeof(cl_mem), (void *)&cpuKeyBuffer);
  if (status != CL_SUCCESS)
  {
    cout << status << endl;
    cout << "Error: Setting CPU Kernel 1 arguments!" << endl;
    return FAILURE;
  }
  status = clSetKernelArg(cpuKernel, 2, sizeof(cl_mem), (void *)&cpuHashBuffer);
  if (status != CL_SUCCESS)
  {
    cout << "Error: Setting CPU Kernel 3 arguments!" << endl;
    return FAILURE;
  }

  /*Step 10: Enqueuing Write Buffers*/
  bool md5_fail = false;
  int key_id = 0; 
  unsigned char hash[17];
  string hash_in;
  cerr << "Enter MD5 hash to crack [ex. md5(deadbeef)=4f41243847da693a4f356c0486114bc6]: ";
  cin >> hash_in;
  hex_decode(hash, hash_in.c_str());
  char(*outputKey)[MAXLENGTH] = new char[K][MAXLENGTH];
  key = new char[K][MAXLENGTH];
  time_t tt = clock();
  status = clEnqueueWriteBuffer(gpuCmmdQue, gpuHashBuffer, CL_FALSE, 0, HASHLENGTH * sizeof(char), hash, 0, NULL, NULL);
  if (status != CL_SUCCESS)
  {
    cout << "Error: Enqueuing write gpuHashBuffer!" << endl;
    return FAILURE;
  }

#if !GPU_ONLY
  status = clEnqueueWriteBuffer(cpuCmmdQue, cpuHashBuffer, CL_FALSE, 0, HASHLENGTH * sizeof(char), hash, 0, NULL, NULL);
  if (status != CL_SUCCESS)
  {
    cout << status << endl;
    cout << "Error: Enqueuing write cpuHashBuffer!" << endl;
    return FAILURE;
  }
#endif
  cerr << "Length = " << len << " ";

  while (len <= MAXLENGTH){
    cerr << ".";
    size_t gsz = genKey();
    //printf("global size: %d\n", gsz);
    if (len <= 5){
      status = clEnqueueWriteBuffer(gpuCmmdQue, gpuKeyBuffer, CL_TRUE, 0, gsz * MAXLENGTH * sizeof(char), key, 0, NULL, NULL);
      if (status != CL_SUCCESS)
      {
        cout << "Error: Enqueuing write gpuKeyBuffer!" << endl;
        return FAILURE;
      }
    }
    else
    {
      status = clEnqueueWriteBuffer(gpuCmmdQue, gpuKeyBuffer, CL_TRUE, 0, ((gsz * GPU_EXE) / P64)*MAXLENGTH * sizeof(char), key, 0, NULL, NULL);
      if (status != CL_SUCCESS)
      {
        cout << "Error: Enqueuing write gpuKeyBuffer!" << endl;
        return FAILURE;
      }
      char(*cpu_key)[16] = &key[((gsz * GPU_EXE) / P64)];

#if !GPU_ONLY
      status = clEnqueueWriteBuffer(cpuCmmdQue, cpuKeyBuffer, CL_TRUE, 0, (gsz - (gsz * GPU_EXE) / P64)*MAXLENGTH * sizeof(char), cpu_key, 0, NULL, NULL);
      if (status != CL_SUCCESS)
      {
        cout << status << endl;
        cout << "Error: Enqueuing write cpuKeyBuffer!" << endl;
        return FAILURE;
      }
#endif
    }


    status |= clSetKernelArg(gpuKernel, 1, sizeof(int), (void *)&len);
    status |= clSetKernelArg(cpuKernel, 1, sizeof(int), (void *)&len);

    /*Step 11: Running the kernel.*/
    size_t group_sz = 512;
    if (len > 5) {
      size_t  gpu_gsz = (gsz * GPU_EXE) / P64;
      if (0 && (gsz * GPU_EXE / P64) % 512 == 0){
        status = clEnqueueNDRangeKernel(gpuCmmdQue, gpuKernel, 1, NULL, (const size_t*)&gpu_gsz, &group_sz, 0, NULL, NULL);
        if (status != CL_SUCCESS)
        {
          cout << "Error: Enqueuing GPU kernel!" << endl;
          return FAILURE;
        }
      }
      else{
        status = clEnqueueNDRangeKernel(gpuCmmdQue, gpuKernel, 1, NULL, (const size_t*)&gpu_gsz, NULL, 0, NULL, NULL);
        if (status != CL_SUCCESS)
        {
          cout << "Error: Enqueuing GPU kernel!" << endl;
          return FAILURE;
        }
      }

#if !GPU_ONLY
      size_t cpu_gsz = gsz - (gsz * GPU_EXE) / P64;
      size_t cpu_lsz = 2048;
      if (0 && (gsz * CPU_EXE / P64) % 2048 == 0){
        status = clEnqueueNDRangeKernel(cpuCmmdQue, cpuKernel, 1, NULL, (const size_t*)&cpu_gsz, &cpu_lsz, 0, NULL, NULL);
        if (status != CL_SUCCESS)
        {
          cout << "Error: Enqueuing CPU kernel!" << endl;
          return FAILURE;
        }
      }
      else{
        status = clEnqueueNDRangeKernel(cpuCmmdQue, cpuKernel, 1, NULL, (const size_t*)&cpu_gsz, NULL, 0, NULL, NULL);
        if (status != CL_SUCCESS)
        {
          cout << "Error: Enqueuing CPU kernel!" << endl;
          return FAILURE;
        }
      }
#endif
      status = clEnqueueReadBuffer(gpuCmmdQue, gpuKeyBuffer, CL_TRUE, 0, ((gsz * GPU_EXE) / P64)* MAXLENGTH * sizeof(char), outputKey, 0, NULL, NULL);
      if (status != CL_SUCCESS)
      {
        cout << gsz << endl;
        cout << "Error: Enqueuing read gpuKeyBuffer!" << endl;
        return FAILURE;
      }
#if !GPU_ONLY
      char(*cpu_outputKey)[16] = &outputKey[((gsz * GPU_EXE) / P64)];
      status = clEnqueueReadBuffer(cpuCmmdQue, cpuKeyBuffer, CL_TRUE, 0, (gsz - (gsz * GPU_EXE) / P64)* MAXLENGTH * sizeof(char), cpu_outputKey, 0, NULL, NULL);
      if (status != CL_SUCCESS)
      {
        cout << "Error: Enqueuing read cpuKeyBuffer!" << endl;
        return FAILURE;
      }
#endif
    }
    else
    {
      status = clEnqueueNDRangeKernel(gpuCmmdQue, gpuKernel, 1, NULL, (const size_t*)&gsz, NULL, 0, NULL, NULL);
      if (status != CL_SUCCESS)
      {
        cout << "Error: Enqueuing GPU kernel!" << endl;
        return FAILURE;
      }
      status = clEnqueueReadBuffer(gpuCmmdQue, gpuKeyBuffer, CL_TRUE, 0, gsz * MAXLENGTH * sizeof(char), outputKey, 0, NULL, NULL);
      if (status != CL_SUCCESS)
      {
        cout << "Error: Enqueuing read gpuKeyBuffer!" << endl;
        return FAILURE;
      }
    }


    md5_fail = true;
    for (key_id = 0; key_id < gsz; key_id++){
      if (outputKey[key_id][0] != 0) {
        key[key_id][len - 1] = outputKey[key_id][0];
        key[key_id][len - 2] = outputKey[key_id][1];
        md5_fail = false;
        break;
      }
    }
    if (!md5_fail)
      break;
    if (finish) {
      len++;
      cerr << endl << "Length = " << len << " ";
    }
  }
  cerr << endl << "Time: " << ((clock() - tt) * 1000) / CLOCKS_PER_SEC << " ms" << endl;

  if (!md5_fail){
    printf("Password: ");
    for (int i = 0; i < len; i++)
      printf("%c", key[key_id][i]);
    printf("\n");
  }
  else
    printf("Password not found.\n");


  delete key;
  delete outputKey;



  /*Step 12: Clean the resources.*/
  status = clReleaseKernel(gpuKernel);        //Release kernel.
  status = clReleaseKernel(cpuKernel);        //Release kernel.
  status = clReleaseProgram(gpuProgram);        //Release the program object.
  status = clReleaseProgram(cpuProgram);        //Release the program object.
  status = clReleaseMemObject(gpuKeyBuffer);
  status = clReleaseMemObject(gpuHashBuffer);
  status = clReleaseMemObject(cpuKeyBuffer);
  status = clReleaseMemObject(cpuHashBuffer);
  status = clReleaseCommandQueue(gpuCmmdQue);  //Release  Command queue.
  status = clReleaseCommandQueue(cpuCmmdQue);  //Release  Command queue.
  status = clReleaseContext(gpuContext);        //Release context.
  status = clReleaseContext(cpuContext);        //Release context.

  return SUCCESS;
}

