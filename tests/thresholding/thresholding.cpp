#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>
#include<iostream>

using namespace cv;

/// Variables globales
Mat src, src_gray;
Mat grad_x, grad_y;
Mat abs_grad_x, abs_grad_y;
Mat grad;
int scale = 1;
int delta = 0;
int ddepth = CV_16S;

//Element structurant pour les transformations morphologie
int morpho_size = 2;
Mat element = getStructuringElement(MORPH_ELLIPSE,Size(2 * morpho_size + 1, 2 * morpho_size + 1),Point(morpho_size, morpho_size));

//Fonction : main()
int main( int argc, char** argv ){

  //Chargement de l'image
  src = imread( argv[1]);
  if( !src.data ){return -1;}

  //FAST NL-MEANS DENOISING
  GaussianBlur( src, src, Size(3,3), 0, 0, BORDER_DEFAULT );
  cvtColor( src, src_gray, CV_BGR2GRAY );
  imshow("Image finale",src_gray);

  //OUVERTURE : dilatation(erosion())
  //erode(frame_gray,frame_gray,element);
  //dilate(frame_gray,frame_gray,element);
  //FERMETURE : erosion(dilatation)
  //dilate(frame_gray,frame_gray,element);
  //erode(frame_gray,frame_gray,element);
  //GaussianBlur( src, src, Size(3,3), 0, 0, BORDER_DEFAULT );
  //cvtColor( src, src_gray, CV_BGR2GRAY );
  //FERMETURE : erosion(dilatation)
  dilate(src_gray,src_gray,element);
  erode(src_gray,src_gray,element);
  /// Gradient X
  //Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
  Sobel( src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
  convertScaleAbs( grad_x, abs_grad_x );
  /// Gradient Y
  //Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
  Sobel( src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
  convertScaleAbs( grad_y, abs_grad_y );
  /// Total Gradient (approximate)
  addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );
  //OUVERTURE : dilatation(erosion)
  //dilate(grad,grad,element);
  //erode(grad,grad,element);
  //Egalisation d'histogramme
  //equalizeHist(grad,grad);
  //Seuillage binaire
  threshold(grad, grad, 128, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
  dilate(grad,grad,element);
  ///dilate(grad,grad,element);
  //dilate(grad,grad,element);


  imshow("original picture",src);
  //namedWindow("image finale", CV_WINDOW_AUTOSIZE );
  //imshow("image finale",grad);
  //imwrite("image_NB_test.png",grad);




  waitKey(0);
  return 0;
}
