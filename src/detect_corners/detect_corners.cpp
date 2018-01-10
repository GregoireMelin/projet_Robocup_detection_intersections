#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>

using namespace cv;

/// Global variables
Mat src, src_gray;
Mat dst, detected_edges;
Mat dst_harris, dst_norm_harris, dst_norm_scaled_harris;

///PARAMETERS
//Canny parameters
int edgeThresh = 1;
int lowThreshold = 10;//TODO : Modifier le seuil pour canny ici
int const max_lowThreshold = 100;
int ratio = 3;
int kernel_size = 3;
//Harris parameters
int thresh = 200;
int max_thresh = 255;

void CannyThreshold(int, void*){
  /// Reduce noise with a kernel 3x3
  blur(src_gray, detected_edges, Size(3,3));
  /// Canny detector
  Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size );
  /// Using Canny's output as a mask, we display our result
  dst = Scalar::all(0);
  src.copyTo( dst, detected_edges);

 }

/** @function main */
int main( int argc, char** argv ){
  /// Load an image
  src = imread( argv[1]);
  if( !src.data ){return -1;}

  // --- CANNY -----
  dst.create( src.size(), src.type() );
  cvtColor( src, src_gray, CV_BGR2GRAY );
  CannyThreshold(0, 0);

  // --- HARRIS -----
  dst_harris = Mat::zeros( src.size(), CV_32FC1 );
  /// Detector parameters
  int blockSize = 3;
  int apertureSize = 3;
  double k = 0.1;
  /// Detecting corners
  cornerHarris( detected_edges, dst_harris, blockSize, apertureSize, k, BORDER_DEFAULT );
  /// Normalizing
  normalize( dst_harris, dst_norm_harris, 0, 255, NORM_MINMAX, CV_32FC1, Mat() );
  convertScaleAbs( dst_norm_harris, dst_norm_scaled_harris );

  /// Drawing a circle around corners
  for( int j = 0; j < dst_norm_harris.rows ; j++ )
     {
       for( int i = 0; i < dst_norm_harris.cols; i++ )
          {
            if( (int) dst_norm_harris.at<float>(j,i) > thresh )
              {
               circle( dst_norm_scaled_harris, Point( i, j ), 5,  Scalar(0), 2, 8, 0 );
              }
          }
     }
  namedWindow( "I) Canny Edge Detection Result", CV_WINDOW_AUTOSIZE );
  imshow( "I) Canny Edge Detection Result", detected_edges );
  namedWindow( "II) Harris Corner Detection Result", CV_WINDOW_AUTOSIZE );
  imshow( "II) Harris Corner Detection Result", dst_norm_scaled_harris );
  waitKey(0);
  return 0;
}
