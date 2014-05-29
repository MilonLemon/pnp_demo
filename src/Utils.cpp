/*
 * Utils.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: Edgar Riba
 */

#include <iostream>
#include <boost/lexical_cast.hpp>

#include "PnPProblem.h"
#include "ModelRegistration.h"
#include "Utils.h"

#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/nonfree/features2d.hpp>


// For text
int fontFace = cv::FONT_ITALIC;
double fontScale = 0.75;
double thickness_font = 2;

// For circles
int lineType = 8;
int radius = 4;
double thickness_circ = -1;

// Draw a text with the question point
void drawQuestion(cv::Mat image, cv::Point3f point, cv::Scalar color)
{
  std::string x = boost::lexical_cast< std::string >((int)point.x);
  std::string y = boost::lexical_cast< std::string >((int)point.y);
  std::string z = boost::lexical_cast< std::string >((int)point.z);

  std::string text = " Where is point (" + x + ","  + y + "," + z + ") ?";
  cv::putText(image, text, cv::Point(25,50), fontFace, fontScale, color, thickness_font, 8);
}

// Draw a text with the number of entered points
void drawText(cv::Mat image, std::string text, cv::Scalar color)
{
  cv::putText(image, text, cv::Point(25,50), fontFace, fontScale, color, thickness_font, 8);
}

// Draw a text with the number of entered points
void drawText2(cv::Mat image, std::string text, cv::Scalar color)
{
  cv::putText(image, text, cv::Point(25,75), fontFace, fontScale, color, thickness_font, 8);
}

// Draw a text with the number of entered points
void drawCounter(cv::Mat image, int n, int n_max, cv::Scalar color)
{
  std::string n_str = boost::lexical_cast< std::string >(n);
  std::string n_max_str = boost::lexical_cast< std::string >(n_max);
  std::string text = n_str + " of " + n_max_str + " points";
  cv::putText(image, text, cv::Point(500,50), fontFace, fontScale, color, thickness_font, 8);
}

// Draw the points and the coordinates
void drawPoints(cv::Mat image, std::vector<cv::Point2f> &list_points_2d, std::vector<cv::Point3f> &list_points_3d, cv::Scalar color)
{
  for (unsigned int i = 0; i < list_points_2d.size(); ++i)
  {
    cv::Point2f point_2d = list_points_2d[i];
    cv::Point3f point_3d = list_points_3d[i];

    // Draw Selected points
    cv::circle(image, point_2d, radius, color, -1, lineType );

    std::string idx = boost::lexical_cast< std::string >(i+1);
    std::string x = boost::lexical_cast< std::string >((int)point_3d.x);
    std::string y = boost::lexical_cast< std::string >((int)point_3d.y);
    std::string z = boost::lexical_cast< std::string >((int)point_3d.z);
    std::string text = "P" + idx + " (" + x + "," + y + "," + z +")";

    point_2d.x = point_2d.x + 10;
    point_2d.y = point_2d.y - 10;
    cv::putText(image, text, point_2d, fontFace, fontScale*0.5, color, thickness_font, 8);
  }
}

// Draw only the points
void draw2DPoints(cv::Mat image, std::vector<cv::Point2f> &list_points, cv::Scalar color)
{
  for( size_t i = 0; i < list_points.size(); i++)
  {
    cv::Point2f point_2d = list_points[i];

    // Draw Selected points
    cv::circle(image, point_2d, radius, color, -1, lineType );
  }
}

void drawArrow(cv::Mat image, cv::Point2i p, cv::Point2i q, cv::Scalar color, int arrowMagnitude, int thickness, int line_type, int shift)
{
  //Draw the principle line
  cv::line(image, p, q, color, thickness, line_type, shift);
  const double PI = 3.141592653;
  //compute the angle alpha
  double angle = atan2((double)p.y-q.y, (double)p.x-q.x);
  //compute the coordinates of the first segment
  p.x = (int) ( q.x +  arrowMagnitude * cos(angle + PI/4));
  p.y = (int) ( q.y +  arrowMagnitude * sin(angle + PI/4));
  //Draw the first segment
  cv::line(image, p, q, color, thickness, line_type, shift);
  //compute the coordinates of the second segment
  p.x = (int) ( q.x +  arrowMagnitude * cos(angle - PI/4));
  p.y = (int) ( q.y +  arrowMagnitude * sin(angle - PI/4));
  //Draw the second segment
  cv::line(image, p, q, color, thickness, line_type, shift);
}

void draw3DCoordinateAxes(cv::Mat image, const std::vector<cv::Point2f> &list_points2d)
{
  cv::Scalar red(0, 0, 255);
  cv::Scalar green(0,255,0);
  cv::Scalar blue(255,0,0);
  cv::Scalar black(0,0,0);

  const double PI = 3.141592653;
  int length = 50;

  cv::Point2i origin = list_points2d[0];
  cv::Point2i pointX = list_points2d[1];
  cv::Point2i pointY = list_points2d[2];
  cv::Point2i pointZ = list_points2d[3];

  drawArrow(image, origin, pointX, red, 9, 2);
  drawArrow(image, origin, pointY, blue, 9, 2);
  drawArrow(image, origin, pointZ, green, 9, 2);
  cv::circle(image, origin, radius/2, black, -1, lineType );

}

// Draw the object mesh
void drawObjectMesh(cv::Mat image, const Mesh *mesh, PnPProblem *pnpProblem, cv::Scalar color)
{
  std::vector<std::vector<int> > list_triangles = mesh->getTrianglesList();
  for( size_t i = 0; i < list_triangles.size(); i++)
  {
    std::vector<int> tmp_triangle = list_triangles.at(i);

    cv::Point3f point_3d_0 = mesh->getVertex(tmp_triangle[0]);
    cv::Point3f point_3d_1 = mesh->getVertex(tmp_triangle[1]);
    cv::Point3f point_3d_2 = mesh->getVertex(tmp_triangle[2]);

    cv::Point2f point_2d_0 = pnpProblem->backproject3DPoint(point_3d_0);
    cv::Point2f point_2d_1 = pnpProblem->backproject3DPoint(point_3d_1);
    cv::Point2f point_2d_2 = pnpProblem->backproject3DPoint(point_3d_2);

    cv::line(image, point_2d_0, point_2d_1, color);
    cv::line(image, point_2d_1, point_2d_2, color);
    cv::line(image, point_2d_2, point_2d_0, color);
  }
}

// Compute the ORB keypoints and descriptors of a given image
void computeKeyPoints(const cv::Mat image, std::vector<cv::KeyPoint> &keypoints, cv::Mat &descriptors)
{

  cv::Mat image_gray;
  cv::cvtColor( image, image_gray, CV_RGB2GRAY );

  /* ORB parameters */
  int nfeatures = 1000;
  float scaleFactor = 1.2f;
  int nlevels = 8;
  int edgeThreshold = 31;
  int firstLevel = 0;
  int WTA_K = 2;
  int scoreType = cv::ORB::HARRIS_SCORE;
  int patchSize = 31;

  cv::ORB orb(nfeatures, scaleFactor, nlevels, edgeThreshold, firstLevel, WTA_K, scoreType, patchSize);

  //-- Step 1: Calculate keypoints
  orb.detect( image_gray, keypoints );

  //-- Step 2: Calculate descriptors (feature vectors)
  orb.compute( image_gray, keypoints, descriptors );

}

bool equal_point(const cv::Point2f &p1, const cv::Point2f &p2)
{
  return ( (p1.x == p2.x) && (p1.y == p2.y) );
}

double get_translation_error(const cv::Mat &t_true, const cv::Mat &t)
{
  return cv::norm( t_true - t ) / cv::norm(t);
}

double get_rotation_error(const cv::Mat &R_true, const cv::Mat &R)
{
  double error;
  int  method = 1;  // 0 Francesc - 1 Luis - 2 Alex
  if( method == 0 )
  {
    // convert Rotation matrix to quaternion
    double qw_true = sqrt( 1 + R_true.at<double>(0,0) + R_true.at<double>(1,1) + R_true.at<double>(2,2) ) / 2;
    double qw = sqrt( 1 + R.at<double>(0,0) + R.at<double>(1,1) + R.at<double>(2,2) ) / 2;
    double qx_true = R_true.at<double>(2,1) - R_true.at<double>(1,2) / 4*qw_true;
    double qx = (R.at<double>(2,1) - R.at<double>(1,2)) / 4*qw;
    double qy_true = R_true.at<double>(0,2) - R_true.at<double>(2,0) / 4*qw_true;
    double qy = (R.at<double>(0,2) - R.at<double>(2,0)) / 4*qw;
    double qz_true = R_true.at<double>(1,0) - R_true.at<double>(0,1) / 4*qw_true;
    double qz = (R.at<double>(1,0) - R.at<double>(0,1)) / 4*qw;

    cv::Mat q_true = cv::Mat::zeros(4, 1, cv::DataType<double>::type);
    cv::Mat q = cv::Mat::zeros(4, 1, cv::DataType<double>::type);
    q_true.at<double>(0) = qw_true;
    q_true.at<double>(1) = qx_true;
    q_true.at<double>(2) = qy_true;
    q_true.at<double>(3) = qz_true;
    q.at<double>(0) = qw;
    q.at<double>(1) = qx;
    q.at<double>(2) = qy;
    q.at<double>(3) = qz;

    error = cv::norm( q_true - q ) / cv::norm(q);
  }
  else if( method == 1 )
  {
    double error_max = 0;
    for(int i = 0; i < 3; ++i)
    {
      cv::Mat tmp = R_true.col(i).t() * R;
      double error_i = acos( tmp.at<double>(0) ) * 180 / CV_PI;
      if( error_i > error_max ) error_max = error_i;
    }
    error = error_max;
  }
  else if( method == 2 )
  {
    cv::Mat error_vec, error_mat;
    error_mat = R_true * cv::Mat(R.inv()).mul(-1);
    cv::Rodrigues(error_mat, error_vec);
    error = cv::norm(error_vec);
  }
  return error;
}

