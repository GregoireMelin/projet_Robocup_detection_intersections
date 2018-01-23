#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

cv::Point2i GetIntersection(const cv::Point2i &o1, const cv::Point2i &p1, const cv::Point2i &o2, const cv::Point2i &p2) {
  cv::Point2i x = o2 - o1;
  cv::Point2i d1 = p1 - o1;
  cv::Point2i d2 = p2 - o2;

  // Calculate cross product
  double cross = d1.x * d2.y - d1.y * d2.x;

  if (abs(cross) < 1e-8) {
      return cv::Point2i();
  }

  // Calculate angle between lines
  double angle_diff = atan2(p1.y - o1.y, p1.x - o1.x) - atan2(p2.y - o2.y, p2.x - o2.x);

  if (fabs(angle_diff) < 15.0 * CV_PI / 180.0) {
      return cv::Point2i();
  }

  // Calculate intersection point
  double t1 = (x.x * d2.y - x.y * d2.x) / cross;
  cv::Point2i r(o1 + d1 * t1);

  if (r.x > 0 && r.x < 800 && r.y > 0 && r.y < 448) {
    return r;
  } else {
    return cv::Point2i();
  }
}

int GetIntersectType(const cv::Point2i &o1, const cv::Point2i &p1, const cv::Point2i &o2, const cv::Point2i &p2, const cv::Point2i &i) {
  double DO1I = norm(o1 - i);
  double DP1I = norm(p1 - i);
  double DO2I = norm(o2 - i);
  double DP2I = norm(p2 - i);

  double DP1O1 = norm(p1 - o1);
  double DP2O2 = norm(p2 - o2);

  int c = 0;
  double max_dist = 10.0;

  if (DO1I < max_dist) c++; // Si O1 == I
  if (DP1I < max_dist) c++; // Si P1 == I
  if (DO2I < max_dist) c++; // Si O2 == I
  if (DP2I < max_dist) c++; // Si P2 == I
  // C = 0 : Croix
  // C = 1 : T
  // C = 2 : Coin
  // C >= 3 : Err

  if (fabs(DO1I + DP1I - DP1O1) > max_dist || fabs(DO2I + DP2I - DP2O2) > max_dist) {
      c = 3;
  }
  return c;
}

Mat filter(Mat src) 
{
  Mat picture_HSV;
  cvtColor(src, picture_HSV, COLOR_BGR2HSV);

  Vec3b bgrPixel(30, 110, 110); //average value of pixel from a line in bgr
  Mat3b bgr(bgrPixel);

  /// Convert pixels values to HSV color spaces.
  Mat3b hsv;
  cvtColor(bgr, hsv, COLOR_BGR2HSV);

  Vec3b hsvPixel(hsv.at<Vec3b>(0,0));

  int thresh = 40;

  Scalar minHSV = Scalar(hsvPixel.val[0] - thresh, hsvPixel.val[1] - thresh, hsvPixel.val[2] - thresh);
  Scalar maxHSV = Scalar(hsvPixel.val[0] + thresh, hsvPixel.val[1] + thresh, hsvPixel.val[2] + thresh);

  Mat maskHSV, resultHSV;
  inRange(picture_HSV, minHSV, maxHSV, maskHSV);
  bitwise_and(picture_HSV, picture_HSV, resultHSV, maskHSV);

  cv::imshow("Result HSV filter", resultHSV);

  return resultHSV;
}

int main(int argc, char *argv[]) {
  if (argc != 2) return EXIT_FAILURE;

  cv::Mat img_rgb = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);

  cv::medianBlur(img_rgb, img_rgb, 9);

  Mat img = filter(img_rgb);

  cv::imshow("algo", img);

  cv::Mat element_alg = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
  cv::morphologyEx(img, img, cv::MORPH_OPEN, element_alg);
  cv::morphologyEx(img, img, cv::MORPH_CLOSE, element_alg);

  cv::imshow("algo clos", img);

  cvtColor(img,img,COLOR_HSV2BGR);
  cvtColor(img,img,COLOR_BGR2GRAY);

  cv::threshold(img, img, 0, 255, cv::THRESH_BINARY + cv::THRESH_OTSU);

  cv::medianBlur(img, img, 9);

  cv::imshow("tresh", img);

  cv::Mat skel(img.size(), CV_8UC1, cv::Scalar(0));
  cv::Mat temp, eroded;

  cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));

  do {
    cv::erode(img, eroded, element);
    cv::dilate(eroded, temp, element); // temp = open(img)
    cv::subtract(img, temp, temp);
    cv::bitwise_or(skel, temp, skel);
    eroded.copyTo(img);
  } while (cv::countNonZero(img) != 0);

  cv::imshow("final", skel);

  cv::vector<cv::Vec4i> lines;

  cv::HoughLinesP(skel, lines, 10.0, 2 * CV_PI/180.0, 80, 10, 30);

  std::cout << "Found : " << lines.size() << std::endl;

  cv::vector<cv::Point2i> intersections;

  cv::vector<Mat> intersections_types;

  for(size_t i = 0; i < lines.size(); i++) {
      cv::Point2i oi = cv::Point2i(lines[i][0], lines[i][1]);
      cv::Point2i pi = cv::Point2i(lines[i][2], lines[i][3]);

      line(img_rgb, oi, pi, cv::Scalar(0,0,255), 1, 8);

      for(size_t j = 0; j < lines.size(); j++) {

          cv::Point2i oj = cv::Point2i(lines[j][0], lines[j][1]);
          cv::Point2i pj = cv::Point2i(lines[j][2], lines[j][3]);

          if (lines[i] != lines[j]) {
              cv::Point2i intersect = GetIntersection(oi, pi, oj, pj);
              int c = GetIntersectType(oi, pi, oj, pj, intersect);

              if (intersect != cv::Point2i()) {
                  bool already = false;
                  for (size_t k = 0; k < intersections.size(); k++) {
                      if (norm(intersections[k] - intersect) < 50.0) {
                          std::cout << intersect << std::endl;
                          std::cout << intersections_types[k] << c << std::endl;

                          if (c < 3) intersections_types[k].at<uchar>(c)++;

                          already = true;
                          break;
                      }
                  }

                  if (!already && c < 3) {
                          Mat votes = Mat(1, 3, CV_8U, cv::Scalar(0));
                          intersections.push_back(intersect);
                          votes.at<uchar>(c)++;
                          intersections_types.push_back(votes);
                  }
              }
          }
      }
  }

  for (size_t k = 0; k < intersections.size(); k++) {
    Mat v = intersections_types[k];

    std::cout << v << std::endl;

    cv::Point min_loc;
    cv::Point max_loc;
    cv::minMaxLoc(v, NULL, NULL, NULL, &max_loc);

    std::cout << min_loc << max_loc << std::endl;

    int c = max_loc.x;
    cv::Point2i intersect = intersections[k];
    if (c == 0) {
        circle(img_rgb, intersect, 10, cv::Scalar(0, 255, 0));
    } else if (c == 1) {
        circle(img_rgb, intersect, 10, cv::Scalar(255, 0, 0));
    } else if (c == 2) {
        circle(img_rgb, intersect, 10, cv::Scalar(255, 0, 255));
    } else {
        circle(img_rgb, intersect, 10, cv::Scalar(0, 0, 255));
    }
  }
  cv::imshow("final2", img_rgb);

  cv::waitKey(0);
  return 0;
}
