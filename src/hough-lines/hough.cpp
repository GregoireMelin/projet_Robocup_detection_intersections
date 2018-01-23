#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>

using namespace std;
using namespace cv;

Mat src, dst;
Mat brightHSV, medianFilter, gaussianFilter;

//Element structurant des transformations morphologiques
int morpho_size = 2;
Mat element = getStructuringElement(MORPH_ELLIPSE,Size(2 * morpho_size + 1, 2 * morpho_size + 1),Point(morpho_size, morpho_size));

/** @function main */
int main( int argc, char** argv )
{
  /// Chargement de l'image
  src = imread(argv[1]);
  //Filtrage median
  medianBlur(src,src,7);
  //Affichage de l'image d'origine
  imshow("original",src);
  imwrite("original_filtered_median.png",src);

  cvtColor(src, brightHSV, COLOR_BGR2HSV);
  Vec3b bgrPixel(30, 110, 110);
  Mat3b bgr (bgrPixel);
  /// Convert pixel values to other color spaces.
  Mat3b hsv,ycb,lab;
  cvtColor(bgr, hsv, COLOR_BGR2HSV);
  //cvtColor(bgr, lab, COLOR_BGR2Lab);
  //Get back the vector from Mat
  Vec3b hsvPixel(hsv.at<Vec3b>(0,0));
  int thresh = 40;
  Scalar minBGR = Scalar(bgrPixel.val[0] - thresh, bgrPixel.val[1] - thresh, bgrPixel.val[2] - thresh);
  Scalar maxBGR = Scalar(bgrPixel.val[0] + thresh, bgrPixel.val[1] + thresh, bgrPixel.val[2] + thresh);
  Mat maskBGR, resultBGR;
  inRange(src, minBGR, maxBGR, maskBGR);
  bitwise_and(src, src, resultBGR, maskBGR);
  Scalar minHSV = Scalar(hsvPixel.val[0] - thresh, hsvPixel.val[1] - thresh, hsvPixel.val[2] - thresh);
  Scalar maxHSV = Scalar(hsvPixel.val[0] + thresh, hsvPixel.val[1] + thresh, hsvPixel.val[2] + thresh);
  Mat maskHSV, resultHSV;
  inRange(brightHSV, minHSV, maxHSV, maskHSV);
  bitwise_and(brightHSV, brightHSV, resultHSV, maskHSV);
  //Affichage
  imshow("Result HSV", resultHSV);
  imwrite("image_HSV.png",resultHSV);
  //Element structurant pour les transformations morphologie
  int morpho_size = 2;
  Mat element = getStructuringElement(MORPH_ELLIPSE,Size(2 * morpho_size + 1, 2 * morpho_size + 1),Point(morpho_size, morpho_size));
  namedWindow( "Meanshift", 0 );
  //medianBlur(src,src,3);
  GaussianBlur(src, src, Size(5,5), 2, 2);
  /*Parameters:

    src – The source 8-bit, 3-channel image.
    dst – The destination image of the same format and the same size as the source.
    sp – The spatial window radius.
    sr – The color window radius.
    maxLevel – Maximum level of the pyramid for the segmentation.
    termcrit – Termination criteria: when to stop meanshift iterations.
   */

   /// MEAN SHIFT FILTERING
   pyrMeanShiftFiltering( resultHSV, resultHSV, 20, 50, 0.1);
   ///DILATATION DES REGIONS DETECTEES
   dilate(resultHSV,resultHSV,element);
   dilate(resultHSV,resultHSV,element);
   dilate(resultHSV,resultHSV,element);
   dilate(resultHSV,resultHSV,element);
   dilate(resultHSV,resultHSV,element);
   dilate(resultHSV,resultHSV,element);
   dilate(resultHSV,resultHSV,element);
   dilate(resultHSV,resultHSV,element);
   dilate(resultHSV,resultHSV,element);

   ///Conversion de l'image pour un seuillage binaire
   cvtColor(resultHSV,resultHSV,COLOR_HSV2BGR);
   cvtColor(resultHSV,resultHSV,COLOR_BGR2GRAY);
   ///Seuillage binaire
   threshold(resultHSV, resultHSV, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

   ///Affichage
   imwrite("image_segmentee_meanshift.png",resultHSV);
   imshow("img origine",src);
   imshow( "Meanshift", resultHSV );


   //Hough lines
   Mat cdst;
   Canny(resultHSV, dst, 50, 200, 3);
   imshow("Canny output",dst);
   cvtColor(dst, cdst, CV_GRAY2BGR);
   vector<Vec2f> lines;
   HoughLines(dst, lines, 1, CV_PI/180, 100, 0, 0 );

   for( size_t i = 0; i < lines.size(); i++ )
   {
     float rho = lines[i][0], theta = lines[i][1];
     Point pt1, pt2;
     double a = cos(theta), b = sin(theta);
     double x0 = a*rho, y0 = b*rho;
     pt1.x = cvRound(x0 + 1000*(-b));
     pt1.y = cvRound(y0 + 1000*(a));
     pt2.x = cvRound(x0 - 1000*(-b));
     pt2.y = cvRound(y0 - 1000*(a));
     line( cdst, pt1, pt2, Scalar(0,0,255), 3, CV_AA);
   }

   //Affichage
   imshow("detected lines",cdst);

   //On enleve les parties de l'image qui ne sont pas des lignes detectées lors de la transformée de Hough.
   for (int j = 0; j < cdst.rows; j++)
     for (int i = 0; i <cdst.cols; i++)
     {
       Vec3b pixelVal = cdst.at<Vec3b>(j,i);
       if(pixelVal.val[2] >= 1 && pixelVal.val[1] >= 1 && pixelVal[0] >= 1)
         cdst.at<Vec3b>(j,i) = Vec3b(0,0,0);
     }




  //Affichages
   imshow("detected_lines_pic.png",cdst);
   imwrite("detected_lines_pic.png",cdst);

   waitKey(0);
   return 0;
}
