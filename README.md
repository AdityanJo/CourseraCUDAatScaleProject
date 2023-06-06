# Image histogram matching for Aerials

## Description
Project for Coursera CUDA at Scale course https://www.coursera.org/learn/cuda-at-scale-for-the-enterprise/

A NPP CUDA sample that demonstrates how to merge a pair of images to perform image blending.

Image blending involves using opacity values for two images and computing the pixel value of the resultant image as a weighted sum of the two images' pixel values.

Consider the two matrices/images below with opacity values 

```
       [[1 2 3]              [[7 8 9]
  A =   [4 5 6]    and  B =   [1 3 5]
        [7 8 9]]              [2 5 3]]

  Usually opacity values are complementary in a range of (0-1) allowing us to compute the resultant image C such that 
  
  C = alpha * A + (1-alpha) * B

  Ex: if alpha = 0.3 (meaning A will contribute lower leading to more info from image B)

         [[5.2 6.2 7.2]
    c =   [1.9 3.6 5.3]
          [3.5 5.9 4.8]]
  
```

Some ways this algorithm can be implemented on CUDA are:
- Flatten and use conventional linear addressing to run the weighted sum 
- Subdivide into smaller problems that can be executed in a parallel fashion by dividing the large images into smaller chunks 

For the purpose of this course we will be using NPP which offers a wide range of image processing functions one of which is nppiAlphaCompC_8u_C1R, 
it takes two images and two alphas on how the images should be weighted and blends them and scales them back to the image maximum pixel value. 


## Key Concepts 

Performance Strategies, Image Processing, NPP Library 

## Supported SM Architectures 

[SM 3.5 ](https://developer.nvidia.com/cuda-gpus)  [SM 3.7 ](https://developer.nvidia.com/cuda-gpus)  [SM 5.0 ](https://developer.nvidia.com/cuda-gpus)  [SM 5.2 ](https://developer.nvidia.com/cuda-gpus)  [SM 6.0 ](https://developer.nvidia.com/cuda-gpus)  [SM 6.1 ](https://developer.nvidia.com/cuda-gpus)  [SM 7.0 ](https://developer.nvidia.com/cuda-gpus)  [SM 7.2 ](https://developer.nvidia.com/cuda-gpus)  [SM 7.5 ](https://developer.nvidia.com/cuda-gpus)  [SM 8.0 ](https://developer.nvidia.com/cuda-gpus)  [SM 8.6 ](https://developer.nvidia.com/cuda-gpus)

## Supported OSes 

Linux 

## Supported CPU Architecture 

x86_64

## CUDA APIs involved 

## Dependencies needed to build/run
[FreeImage](../../README.md#freeimage), [NPP](../../README.md#npp)

## Prerequisites

Download and install the [CUDA Toolkit 11.4](https://developer.nvidia.com/cuda-downloads) for your corresponding platform.
Make sure the dependencies mentioned in [Dependencies]() section above are installed.

## Build and Run 

The Linux samples are built using makefiles. To use the makefiles, change the current directory to the sample directory you wish to build, and run make:
```
$ cd <sample_dir>
$ make
```
The samples makefiles can take advantage of certain options:
*   **dbg=1** - build with debug symbols
    ```
    $ make dbg=1
    ```
*   **SMS="A B ..."** - override the SM architectures for which the sample will be built, where `"A B ..."` is a space-delimited list of SM architectures. For example, to generate SASS for SM 50 and SM 60, use `SMS="50 60"`.
    ```
    $ make SMS="50 60"
    ```

*  **HOST_COMPILER=<host_compiler>** - override the default g++ host compiler. See the [Linux Installation Guide](http://docs.nvidia.com/cuda/cuda-installation-guide-linux/index.html#system-requirements) for a list of supported host compilers.
```
    $ make HOST_COMPILER=g++
```

## Build and Run - Docker based

Docker offers a way to build an environment to get all your dependencies without having to install everything on baremetal, this way you can experiment with several versions of CUDA for instance. See [NVIDIA container installation](https://docs.nvidia.com/datacenter/cloud-native/container-toolkit/install-guide.html)

This repo contains a dockerfile which you can use to build a docker image and use to run the program and generate outputs 

```
# Change to cloned repo location
cd $PATH_TO_CLONED_REPO
# Build the container image
docker build -t image-blending:latest -f docker/Dockerfile .
# Launch the container (the -v allows us to mount a volume from host to the container allow us to copy files/outputs to/from the docker environment)
docker run --gpus all --rm -it -v /tmp:/tmp image-blending:latest

# Once inside container you can run the program as it comes pre-compiled as part of the image
/app/bin/x86_64/linux/release/imageBlendingNPP -input1=/app/data/samples/1.1.01.tiff -input2=/app/data/samples/1.5.05.tiff -output=/tmp/output_a1_50_a2_250.pgm -alpha1=50 -alpha2=250

```

## Usage 

Once you have either the baremetal or the docker based CLI setup you can use the CLI in this way 
```
<path_to_binary>/imageBlendingNPP -input1=<path_to_image1> -input2=<path_to_image2> -output=<path_to_store_result_img> -alpha1=<int> -alpha2=<int>
```

Here the if alpha1 is larger than alpha2 the weightage of image1 would be more compared to image2. If alpha2 is smaller than alpha2 the weightage of image1 would be lower compared to image2. If they are equal the images would be blended in a equal weightage. 

The repo comes with two example inputs and two reference outputs of what it would look like when varying the alpha levels in favor on one or the other. The references section has options on how to view the PGM files generated. 

## Personal Notes 

### Rubric Check

Code Repository 
    - URL: https://github.com/AdityanJo/CourseraCUDAatScaleProject
    - README : Describes how to run and multiple options to run as well as CLI usage details
    - Google Style C++ : Used clang-format-9 to comply with the style format

Proof of execution artifacts:
    - Added log outputs for the outputs that are part of the repo for the example commands shown in this readme

Code Project Description:
    - Added description for the underlying algorithm and underlying NPP algorithm used

## References 

- Template codes taken from https://github.com/PascaleCourseraCourses/CUDAatScaleForTheEnterpriseCourseProjectTemplate
- https://homepages.inf.ed.ac.uk/rbf/HIPR2/blend.htm
- https://docs.nvidia.com/cuda/npp/nppi_conventions_lb.html
- Dataset used : https://sipi.usc.edu/database/database.php?volume=textures
- https://homepages.inf.ed.ac.uk/rbf/HIPR2/blend.htm
- https://smallpond.ca/jim/photomicrography/pgmViewer/index.html
- https://marketplace.visualstudio.com/items?itemName=ngtystr.ppm-pgm-viewer-for-vscode