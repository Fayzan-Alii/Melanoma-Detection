#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>
#include <stb_image.h>
#include <stb_image_write.h>

int main()
{
    cl_platform_id platform_id;
    cl_device_id device_id;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;
    cl_mem img_mem, img_grey_mem;
    cl_int err;

    // Get the first platform and device
    err = clGetPlatformIDs(1, &platform_id, NULL);
    err |= clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error getting platform or device.");
        return -1;
    }

    size_t max_work_group_size;
    err = clGetDeviceInfo(device_id, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(max_work_group_size), &max_work_group_size, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error getting device info.");
        return -1;
    }

    printf("Max work group size: %zu\n", max_work_group_size);

    // Create a context
    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &err);
    if (err != CL_SUCCESS)
    {
        printf("Error creating context.");
        return -1;
    }

    // Create a command queue
    queue = clCreateCommandQueue(context, device_id, 0, &err);
    if (err != CL_SUCCESS)
    {
        printf("Error creating command queue.");
        return -1;
    }

    // Create the program
    const char *source = "__kernel void color_to_gray(__read_only image2d_t img, __write_only image2d_t img_gray) {"
                         "    int x = get_global_id(0);"
                         "    int y = get_global_id(1);"
                         "    sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;"
                         "    uint4 pixel = read_imageui(img, sampler, (int2)(x, y));"
                         "    int red = pixel.x;"
                         "    int green = pixel.y;"
                         "    int blue = pixel.z;"
                         "    int gray = (int)(0.299f * red + 0.587f * green + 0.114f * blue);"
                         "    int alpha = 255;"
                         "    write_imageui(img_gray, (int2)(x, y), (uint4)(gray, gray, gray, alpha));"
                         "}";
    program = clCreateProgramWithSource(context, 1, &source, NULL, &err);
    if (err != CL_SUCCESS)
    {
        printf("Error creating program.");
        return -1;
    }

    // Build the program
    err = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error building program.");
        return -1;
    }

    // Create the kernel
    kernel = clCreateKernel(program, "color_to_gray", &err);
    if (err != CL_SUCCESS)
    {
        printf("Error creating kernel.");
        return -1;
    }

    // Load the image
    int width, height, channels;
    unsigned char *img = stbi_load("ISIC_0082004.jpg", &width, &height, &channels, STBI_rgb_alpha);
    if (!img)
    {
        printf("Error loading image.\n");
        return -1;
    }

    // Create the input image object
    cl_image_format format;
    format.image_channel_order = CL_RGBA;
    format.image_channel_data_type = CL_UNORM_INT8;
    img_mem = clCreateImage2D(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, &format, width, height, 0, img, &err);
    if (err != CL_SUCCESS)
    {
        printf("Error creating input image memory object.");
        return -1;
    }

    // Create the output image object
    img_grey_mem = clCreateImage2D(context, CL_MEM_WRITE_ONLY, &format, width, height, 0, NULL, &err);
    if (err != CL_SUCCESS)
    {
        printf("Error creating output image memory object.");
        return -1;
    }

    // Set the kernel arguments
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &img_mem);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &img_grey_mem);
    if (err != CL_SUCCESS)
    {
        printf("Error setting kernel arguments.");
        return -1;
    }

    // Execute the kernel
    size_t global_size[2] = {width, height};
    size_t max_local_size = sqrt(4096);  // Maximum local size in one dimension
        size_t local_size[2] = {1, 1};  // Start with the smallest possible local size

        // Increase the local size in each dimension as much as possible
        for (size_t i = 2; i <= max_local_size; i++)
        {
            if (global_size[0] % i == 0 && global_size[1] % i == 0)
            {
                local_size[0] = i;
                local_size[1] = i;
            }
            else
            {
                break;
            }
        }
    printf("Local size: {%zu, %zu}\n", local_size[0], local_size[1]);
    printf("Global size: {%zu, %zu}\n", global_size[0], global_size[1]);
    err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global_size, local_size, 0, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error executing kernel.");
        return -1;
    }

    // Read the output image back into host memory
    unsigned char *img_grey = (unsigned char *)malloc(width * height * 4 * sizeof(unsigned char));
    size_t origin[3] = {0, 0, 0};
    size_t region[3] = {width, height, 1};
    err = clEnqueueReadImage(queue, img_grey_mem, CL_TRUE, origin, region, 0, 0, img_grey, 0, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error reading output image from device.");
        return -1;
    }

    // Save the output image
    stbi_write_jpg("output_image.jpg", width, height, 4, img_grey, 100);

    // Clean up
if (img_mem != NULL)
{
    clReleaseMemObject(img_mem);
}

if (img_grey_mem != NULL)
{
    clReleaseMemObject(img_grey_mem);
}

if (kernel != NULL)
{
    clReleaseKernel(kernel);
}

if (program != NULL)
{
    clReleaseProgram(program);
}

if (queue != NULL)
{
    clReleaseCommandQueue(queue);
}

if (context != NULL)
{
    clReleaseContext(context);
}

// Release the images
if (img != NULL)
{
    stbi_image_free(img);
}

if (img_grey != NULL)
{
    free(img_grey);
}

    return 0;
}