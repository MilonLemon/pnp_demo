/*
 * Utils.h
 *
 *  Created on: Mar 28, 2014
 *      Author: Edgar Riba
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <iostream>
#include <cv.h>


// Draw a text with the question point
void drawQuestion(cv::Mat image, cv::Point3f point, cv::Scalar color);

// Draw a text in the left of the image
void drawText(cv::Mat image, std::string text, cv::Scalar color);
void drawText2(cv::Mat image, std::string text, cv::Scalar color);

// Draw a text with the number of registered points
void drawCounter(cv::Mat image, int n, int n_max, cv::Scalar color);

// Draw the points and the coordinates
void drawPoints(cv::Mat image, std::vector<cv::Point2f> &list_points_2d, std::vector<cv::Point3f> &list_points_3d, cv::Scalar color);

void draw2DPoints(cv::Mat image, std::vector<cv::Point2f> &list_points, cv::Scalar color);

void drawObjectMesh(cv::Mat image, const ObjectMesh *objMesh, PnPProblem *pnpProblem, cv::Scalar color);

// Draw an arrow into the image
void drawArrow(cv::Mat image, cv::Point2i p, cv::Point2i q, cv::Scalar color, int arrowMagnitude = 9, int thickness=1, int line_type=8, int shift=0);

// Draw the 3D coordinate axes
void draw3DCoordinateAxes(cv::Mat image, cv::Scalar color);

// Compute the 2D points with the esticv::Mated pose
std::vector<cv::Point2f> verification_points(PnPProblem *p);

void computeKeyPoints(const cv::Mat image, std::vector<cv::KeyPoint> &keypoints, cv::Mat &descriptors);

#endif /* UTILS_H_ */
