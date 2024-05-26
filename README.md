# Melanoma-Detection-
Optimal Image preprocessing on “The ISIC 2020 Challenge Dataset” for Skin Lesion Analysis Towards Melanoma(Cancer) Detection using OpenCL
# Optimal Image Preprocessing on “The ISIC 2020 Challenge Dataset” for Skin Lesion Analysis Towards Melanoma Detection using OpenCL

## Objective
The objective of this project is to provide hands-on experience with parallel and distributed computing using OpenCL by converting a dataset of colored images of skin lesions into grayscale images.

## The ISIC 2020 Challenge Dataset
The ISIC dataset is vital for dermatology and medical image analysis, providing a diverse collection of high-quality skin lesion images. It supports the development of diagnostic systems, segmentation algorithms, and deep learning models for automated lesion detection and classification. By aiding early skin cancer detection and diagnosis, the dataset significantly improves patient outcomes globally. Its accessibility and breadth make it indispensable for advancing research in dermatology, oncology, and medical imaging.

Converting the ISIC dataset to grayscale images simplifies image representation, aiding analysis and processing such as lesion segmentation. Grayscale conversion reduces complexity by removing color information, enhancing computational efficiency and facilitating tasks like lesion detection.

The dataset contains dermoscopic training images of unique benign and malignant skin lesions from over 2,000 patients. Each image is associated with one of these individuals using a unique patient identifier. For this project, we are only using the Test Set available at [ISIC 2020 Test JPEG](https://isic-challenge-data.s3.amazonaws.com/2020/ISIC_2020_Test_JPEG.zip) which contains 10,982 JPEG images of different sizes.

## Problem Description
You are provided with a dataset of colored images of skin lesions in various JPEG format. The task is to convert these colored images to grayscale images using OpenCL parallel computing.

Greyscale conversion involves transforming each pixel of the image from its RGB (Red, Green, Blue) representation to a single intensity value representing the luminance.

## Instructions
The detailed requirements are mentioned in the sections provided below:

### Setup Environment
Install necessary OpenCL SDK and drivers on your machine. Ensure that you have access to a GPU that supports OpenCL for optimal performance.

### Dataset Preparation
Download the mentioned dataset of colored images of skin lesions. Familiarise yourself with the dataset structure and image formats. For simplification only the JPEG format is selected.

### OpenCL Implementation
Write OpenCL kernels to perform the conversion of colored images to grayscale images. Ensure that your kernels can handle images of varying sizes. Implement efficient memory allocation and data transfer strategies for optimal performance.

### Image Processing
Load each colored image from the dataset into memory. Apply the OpenCL kernel to convert the colored image to grayscale. Save the resulting grayscale images to the disk.

## Libraries Used
The program uses the `stb_image.h` and `stb_image_write.h` libraries for image loading and saving operations. These libraries are used because they provide a simple API for image manipulation in C, as OpenCV API is not available in C.

```c
#include <stb_image.h>
#include <stb_image_write.h>
```

`stb_image.h` is used to load images into memory. It supports most common image file formats and provides a simple way to read pixel data.

`stb_image_write.h` is used to write the resulting grayscale images back to disk. It supports writing several file formats including JPEG, PNG, TGA, BMP, and GIF.

## Programs Available
There are two C programs available in this repository:

1. A program for converting a single image of your choice to grayscale.
2. A program for converting all the images in a folder to grayscale. This program is more efficient when you have a GPU as it can process multiple images simultaneously.