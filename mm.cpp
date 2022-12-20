#include <iostream>
#include <opencv2/opencv.hpp>
#include <random>
#include "opencv2/core.hpp"
#include <opencv2/core/simd_intrinsics.hpp>
#include <sys/time.h>
#include <omp.h>
#define N 1024
using namespace cv::hal_baseline;
int main()
{
  
  cv::Mat A = cv::Mat::zeros(N,N,CV_32FC1);
  cv::Mat B = cv::Mat::zeros(N,N,CV_32FC1);
  cv::Mat C = cv::Mat::zeros(N,N,CV_32FC1);
  cv::Mat ref_C = cv::Mat::zeros(N,N,CV_32FC1);
  cv::Mat err = cv::Mat::zeros(N,N,CV_32FC1);
  for(int i=0;i<N;i++){
    for(int j=0;j<N;j++){
      A.at<float>(i,j) = rand()/32767.0;
      B.at<float>(i,j) = rand()/32767.0;
      // A.at<float>(i,j) = 1.0;
      // B.at<float>(i,j) = 1.0;
    }
  }
  float *c=(float*)C.data;
  float *a=(float*)A.data;
  float *b=(float*)B.data;

  struct timeval time1;
  struct timeval time2;
  float diff_time;
  gettimeofday(&time1, NULL);
  // //SIMDなし実行
#pragma omp parallel for
  for(int i=0;i<N;i++){
    for(int k=0;k<N;k++){
      for(int j=0;j<N;j++){
	c[((i*N)+j)] += a[((i*N)+k)] * b[((k*N)+j)];
      }
    }
  }
  gettimeofday(&time2, NULL);
  diff_time = time2.tv_sec - time1.tv_sec +  (float)(time2.tv_usec - time1.tv_usec) / 1000000;
  printf("diff: %f[s]\n", diff_time);
#ifdef CV_SIMD
#if CV_SIMD256
  //SIMD実行
  gettimeofday(&time1, NULL);
#pragma omp parallel for
  for(int i=0;i<N;i++){
    for(int j=0;j<N;j+=8){
      v_float32x8 c_vec = vx_setall_f32(0);
      for(int k=0;k<N;k++){
	float *ptr = b + j + k * N;
	v_float32x8 b_vec = v256_load(ptr);
	v_float32x8 a_vec = vx_setall_f32(a[(i*N)+k]);
	// c_vec += a_vec*b_vec;
	c_vec = v_fma(a_vec,b_vec,c_vec);
      }
      v_store(c+i*N+j,c_vec);      
    }
  }
  gettimeofday(&time2, NULL);
  diff_time = time2.tv_sec - time1.tv_sec +  (float)(time2.tv_usec - time1.tv_usec) / 1000000;
  printf("diff: %f[s]\n", diff_time);
#endif
#endif
  //OpenCV実行
  gettimeofday(&time1, NULL);
  ref_C=A*B;
  gettimeofday(&time2, NULL);
  diff_time = time2.tv_sec - time1.tv_sec +  (float)(time2.tv_usec - time1.tv_usec) / 1000000;
  printf("diff: %f[s]\n", diff_time);
  std::cout<< ref_C.at<float>(100,0) <<"," << C.at<float>(100,0) <<std::endl;
  cv::absdiff(C,ref_C,err);
  err = err/cv::abs(ref_C);
  
  double minVal, maxVal;
  cv::Point minLoc, maxLoc;
  cv::minMaxLoc(err, &minVal, &maxVal, &minLoc, &maxLoc);
  std::cout << "Max Relative Error:" << maxVal << std::endl;
}
