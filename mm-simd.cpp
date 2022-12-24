#include <iostream>
#include <opencv2/opencv.hpp>
#include <random>
#include "opencv2/core.hpp"
#include <opencv2/core/simd_intrinsics.hpp>
#include <sys/time.h>
#include <omp.h>
using namespace cv::hal_baseline;
int main()
{
  
  cv::Mat A = cv::Mat::zeros(SIZE,SIZE,CV_32FC1);
  cv::Mat B = cv::Mat::zeros(SIZE,SIZE,CV_32FC1);
  cv::Mat C = cv::Mat::zeros(SIZE,SIZE,CV_32FC1);
  cv::Mat ref_C = cv::Mat::zeros(SIZE,SIZE,CV_32FC1);
  cv::Mat err = cv::Mat::zeros(SIZE,SIZE,CV_32FC1);
  for(int i=0;i<SIZE;i++){
    for(int j=0;j<SIZE;j++){
      A.at<float>(i,j) = rand()/32767.0;
      B.at<float>(i,j) = rand()/32767.0;
    }
  }
  float *c=(float*)C.data;
  float *a=(float*)A.data;
  float *b=(float*)B.data;

  struct timeval time1;
  struct timeval time2;
  float diff_time=0;

#ifdef CV_SIMD


  int step=v_float32().nlanes;
  std::cout<<"SIMD lanes:" << step<<std::endl;
  for(int s=0;s<ITER;s++){
    gettimeofday(&time1, NULL);
#pragma omp parallel for
    for(int i=0;i<SIZE;i++){
      for(int j=0;j<SIZE;j+=step){
	v_float32 c_vec = vx_setall_f32(0);
	for(int k=0;k<SIZE;k++){
	  float *ptr = b + j + k * SIZE;
	  v_float32 b_vec = vx_load(ptr);
	  v_float32 a_vec = vx_setall_f32(a[(i*SIZE)+k]);
	  c_vec = v_fma(a_vec,b_vec,c_vec);
	}
	v_store(c+i*SIZE+j,c_vec);      
      }
    }

  gettimeofday(&time2, NULL);
  diff_time += time2.tv_sec - time1.tv_sec +  (float)(time2.tv_usec - time1.tv_usec) / 1000000;
  }
  printf("diff: %f[s]\n", diff_time/ITER);
  diff_time = 0;
#endif
  //OpenCV実行
  for(int s=0;s<ITER;s++){
    gettimeofday(&time1, NULL);
    ref_C=A*B;
    gettimeofday(&time2, NULL);
  diff_time += time2.tv_sec - time1.tv_sec +  (float)(time2.tv_usec - time1.tv_usec) / 1000000;
  }
  printf("diff: %f[s]\n", diff_time/ITER);
  std::cout<< ref_C.at<float>(100,0) <<"," << C.at<float>(100,0) <<std::endl;
  cv::absdiff(C,ref_C,err);
  err = err/cv::abs(ref_C);
  
  double minVal, maxVal;
  cv::Point minLoc, maxLoc;
  cv::minMaxLoc(err, &minVal, &maxVal, &minLoc, &maxLoc);
  std::cout << "Max Relative Error:" << maxVal << std::endl;
}
