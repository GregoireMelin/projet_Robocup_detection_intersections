#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/video/background_segm.hpp"
#include "opencv2/video/tracking.hpp"

#include <iostream>
#include <sstream>

#include <fstream>
#include <cstdlib>
#include <vector>
#include <stdio.h>

using namespace cv;
using namespace std;

//Variables globales
Mat frame, frame_gray;
Mat dst_harris, dst_norm_harris, dst_norm_scaled_harris;
RNG rng(12345);
//Parametres relatifs aux traitements
  //Parametres de la detection de lignes par Canny
int edgeThresh = 1;
int lowThreshold = 10;//TODO : Modifier le seuil pour canny ici
int const max_lowThreshold = 0;
int ratio = 3;
int kernel_size = 3;
  //Parametres de la detection de coin par un Harris
int thresh = 200;
int max_thresh = 255;

int blockSize = 3;
int apertureSize = 3;
double k = 0.1;

//Element structurant pour les transformations morphologie
int morpho_size = 1;
Mat element = getStructuringElement(MORPH_ELLIPSE,Size(2 * morpho_size + 1, 2 * morpho_size + 1),Point(morpho_size, morpho_size));

void usage (const char *s)
{
    cout<<"Usage: "<<s<<"usage : ERROR"<<endl;
    exit(EXIT_FAILURE);
}
#define param 1
int main( int argc, char* argv[] )
{
  //Chargement de l'image
  frame = imread( argv[1]);
  if( !frame.data ){return -1;}

  Mat canny;
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;
  Mat drawing;

  int morpho_size = 2;
  Mat element = getStructuringElement(MORPH_ELLIPSE,Size(2 * morpho_size + 1, 2 * morpho_size + 1),Point(morpho_size, morpho_size));

  cvtColor(frame, frame_gray, CV_BGR2GRAY);
    //Leger flou sur l'image pour enlever le bruit de l'image
  blur( frame_gray, frame_gray, Size(3,3));
    //Detection des "contours grace a un filtre de Canny
  Canny( frame_gray, canny, thresh, thresh*2, 3 );

    //OUVERTURE : dilatation(erosion())
  //erode(frame_gray,frame_gray,element);
  //dilate(frame_gray,frame_gray,element);

    //FERMETURE : erosion(dilatation)
  //dilate(frame_gray,frame_gray,element);
  //erode(frame_gray,frame_gray,element);

  findContours( canny, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
  drawing = Mat::zeros( canny.size(), CV_8UC3);
  for( unsigned int i = 0; i< contours.size(); i++ )
   {
     Scalar color = Scalar(255,255,255);
     drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
   }
  imshow("detected lines",drawing);

  //Detection de Harris
  dst_harris = Mat::zeros( frame.size(), CV_32FC1 );
  cornerHarris( drawing, dst_harris, blockSize, apertureSize, k, BORDER_DEFAULT );
  //dilate(detected_edges,detected_edges,element);

    //Normalisation de l'image
  normalize( dst_harris, dst_norm_harris, 90, 255, NORM_MINMAX, CV_32FC1, Mat() );
  dst_norm_harris=dst_harris.clone();
  //imshow("Normalisation",dst_norm_harris);
  convertScaleAbs( dst_norm_harris, dst_norm_scaled_harris );
  //imshow("ConvertscaleAbs",dst_norm_scaled_harris);
    //Rendu visuel par dessin de cercle rouge sur les coins
  for( int j = 0; j < dst_norm_harris.rows ; j++ )
     {
       for( int i = 0; i < dst_norm_harris.cols; i++ )
          {
            if( (int) dst_norm_harris.at<float>(j,i) > thresh )
              {
               circle( dst_norm_scaled_harris, Point( i, j ), 5,  Scalar(0), 2, 255, 0 );
              }
          }
     }

  waitKey(0);

  return EXIT_SUCCESS;
}
