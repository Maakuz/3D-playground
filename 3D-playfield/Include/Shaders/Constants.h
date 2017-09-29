#pragma once
/////////////////////BLOOM////////////////////////
#define BLOOM_INTENSITY 2.5
#define ORIGINAL_INTENSITY 0.9
#define BLOOM_SATURATION 0.7
#define ORIGINAL_SATURATION 1.0
#define BLOOM_THRESHOLD 0.7


//////////////////////////////GAUSSIAN FILTER///////////////////////////////
#define KERNEL_SIZE 7

static const float gaussianFilter[KERNEL_SIZE] =
{
	0.106289, 0.140321, 0.165770, 0.175240, 0.165770, 0.140321, 0.106289
};