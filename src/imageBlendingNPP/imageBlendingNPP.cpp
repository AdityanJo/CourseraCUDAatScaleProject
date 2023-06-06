/* Copyright (c) 2019, NVIDIA CORPORATION. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of NVIDIA CORPORATION nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#define WINDOWS_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#pragma warning(disable : 4819)
#endif

#include <Exceptions.h>
#include <ImageIO.h>
#include <ImagesCPU.h>
#include <ImagesNPP.h>

#include <string.h>
#include <fstream>
#include <iostream>

#include <cuda_runtime.h>
#include <npp.h>
#include <nppi_geometry_transforms.h>
#include <nppi_arithmetic_and_logical_operations.h>

#include <helper_cuda.h>
#include <helper_string.h>

// From boxFilterNPP.cpp example code 
bool printfNPPinfo(int argc, char *argv[])
{
  const NppLibraryVersion *libVer = nppGetLibVersion();

  printf("NPP Library Version %d.%d.%d\n", libVer->major, libVer->minor,
         libVer->build);

  int driverVersion, runtimeVersion;
  cudaDriverGetVersion(&driverVersion);
  cudaRuntimeGetVersion(&runtimeVersion);

  printf("  CUDA Driver  Version: %d.%d\n", driverVersion / 1000,
         (driverVersion % 100) / 10);
  printf("  CUDA Runtime Version: %d.%d\n", runtimeVersion / 1000,
         (runtimeVersion % 100) / 10);

  // Min spec is SM 1.0 devices
  bool bVal = checkCudaCapabilities(1, 0);
  return bVal;
}

int main(int argc, char* argv[])
{
  printf("%s Starting...\n\n", argv[0]);

  try 
  {
    char *inputPathLeft, *inputPathRight, *outputPath, *alphaVal;
    std::string sFilenameLeft, sFilenameRight, sFilenameOutput;

    unsigned int dIn1Alpha = 128;
    unsigned int dIn2Alpha = 128;

    findCudaDevice(argc, (const char **)argv);

    if (printfNPPinfo(argc, argv) == false)
    {
      exit(EXIT_SUCCESS);
    }

    if (checkCmdLineFlag(argc, (const char **)argv, "input1"))
    {
      getCmdLineArgumentString(argc, (const char **)argv, "input1", &inputPathLeft);
    }
    else 
    {
       inputPathLeft = sdkFindFilePath("../../data/Lena.pgm", argv[0]);
    }

    if (checkCmdLineFlag(argc, (const char **)argv, "input2"))
    {
      getCmdLineArgumentString(argc, (const char **)argv, "input2", &inputPathRight);
    }
    else 
    {
       inputPathRight = sdkFindFilePath("../../data/Lena.pgm", argv[0]);
    }

    if (checkCmdLineFlag(argc, (const char **)argv, "output"))
    {
      getCmdLineArgumentString(argc, (const char **)argv, "output", &outputPath);
    }
    else 
    {
       outputPath = sdkFindFilePath("../../data/output.pgm", argv[0]);
    }

    if (checkCmdLineFlag(argc, (const char **)argv, "alpha1"))
    {
      getCmdLineArgumentString(argc, (const char **)argv, "alpha1", &alphaVal);
      dIn1Alpha = atoi(alphaVal);
    }

    if (checkCmdLineFlag(argc, (const char **)argv, "alpha2"))
    {
      getCmdLineArgumentString(argc, (const char **)argv, "alpha2", &alphaVal);
      dIn2Alpha = atoi(alphaVal);
    }

    if (inputPathLeft)
    {
      sFilenameLeft = inputPathLeft;
    }
    else
    {
      sFilenameLeft = "../../data/Lena.pgm";
    }

    if (inputPathRight)
    {
      sFilenameRight = inputPathRight;
    }
    else
    {
      sFilenameRight = "../../data/Lena.pgm";
    }

    if(outputPath) 
    {
       sFilenameOutput = outputPath;
    }
    else 
    {
       sFilenameOutput = "output.pgm";
    }

    // if we specify the filename at the command line, then we only test
    // sFilename[0].
    int file_errors = 0;
    std::ifstream infileLeft(sFilenameLeft.data(), std::ifstream::in);

    if (infileLeft.good())
    {
      std::cout << "imageBlendingNPP opened: <" << sFilenameLeft.data()
                << "> successfully!" << std::endl;
      file_errors = 0;
      infileLeft.close();
    }
    else
    {
      std::cout << "imageBlendingNPP unable to open: <" << sFilenameLeft.data() << ">"
                << std::endl;
      file_errors++;
      infileLeft.close();
    }

    std::ifstream infileRight(sFilenameRight.data(), std::ifstream::in);

    if (infileRight.good())
    {
      std::cout << "imageBlendingNPP opened: <" << sFilenameRight.data()
                << "> successfully!" << std::endl;
      file_errors = 0;
      infileRight.close();
    }
    else
    {
      std::cout << "imageBlendingNPP unable to open: <" << sFilenameRight.data() << ">"
                << std::endl;
      file_errors++;
      infileRight.close();
    }

    if (file_errors > 0)
    {
      exit(EXIT_FAILURE);
    }


    // declare a host image object for an 8-bit grayscale image 
    npp::ImageCPU_8u_C1 oHostSrcLft;
    npp::ImageCPU_8u_C1 oHostSrcRt;

    // load gray-scale image from disk
    npp::loadImage(sFilenameLeft, oHostSrcLft);
    npp::loadImage(sFilenameRight, oHostSrcRt);

    // declare a device image and copy construct from the host image,
    // i.e. upload host to device
    npp::ImageNPP_8u_C1 oDeviceSrcLft(oHostSrcLft);
    npp::ImageNPP_8u_C1 oDeviceSrcRt(oHostSrcRt);

    
    // create struct with ROI size
    NppiSize oSizeROILft = {(int)oDeviceSrcLft.width(), (int)oDeviceSrcLft.height()};
    NppiSize oSizeROIRt = {(int)oDeviceSrcRt.width(), (int)oDeviceSrcRt.height()};

    if(((int)oSizeROILft.width)!=((int)oSizeROIRt.width) || ((int)oSizeROILft.height)!=((int)oSizeROIRt.height))
    {
       std::cerr << "Input files aren't of same shape, retry again with input images of same shapes\n";
       std::cerr << "Aborting." << std::endl;
       
       nppiFree(oDeviceSrcLft.data());
       nppiFree(oDeviceSrcRt.data());
       exit(EXIT_FAILURE);
    }

    NppiSize oSrcSize = {(int)oDeviceSrcLft.width(), (int)oDeviceSrcLft.height()};

    // allocate device image of appropriately reduced size
    npp::ImageNPP_8u_C1 oDeviceDst(oSizeROILft.width, oSizeROILft.height);

    NPP_CHECK_NPP(nppiAlphaCompC_8u_C1R(oDeviceSrcLft.data(), oDeviceSrcLft.pitch(), dIn1Alpha, oDeviceSrcRt.data(), oDeviceSrcRt.pitch(), dIn2Alpha, oDeviceDst.data(), oDeviceDst.pitch(), oSizeROILft, NPPI_OP_ALPHA_XOR));
       
    // declare a host image for the result
    npp::ImageCPU_8u_C1 oHostDst(oDeviceDst.size());
    // and copy the device result data into it
    oDeviceDst.copyTo(oHostDst.data(), oHostDst.pitch());
std::cout << "Saved image: " << sFilenameOutput << std::endl;
    saveImage(sFilenameOutput, oHostDst);
    

    nppiFree(oDeviceSrcLft.data());
    nppiFree(oDeviceSrcRt.data());
    nppiFree(oDeviceDst.data());
  }
  catch (npp::Exception &rException)
  {
    std::cerr << "Program error! The following exception occurred: \n";
    std::cerr << rException << std::endl;
    std::cerr << "Aborting." << std::endl;

    exit(EXIT_FAILURE);
  }
  catch (...)
  {
    std::cerr << "Program error! An unknow type of exception occurred. \n";
    std::cerr << "Aborting." << std::endl;

    exit(EXIT_FAILURE);
    return -1;
  }

  return 0;

}
