/*
 * Model.cpp
 *
 *  Created on: Apr 9, 2014
 *      Author: edgar
 */

#include "Model.h"
#include "CsvWriter.h"

Model::Model() : list_points2d_in_(0), list_points2d_out_(0), list_points3d_in_(0)
{
  n_correspondences_ = 0;
}

Model::~Model()
{
  // TODO Auto-generated destructor stub
}

void Model::add_correspondence(const cv::Point2f &point2d, const cv::Point3f &point3d)
{
  list_points2d_in_.push_back(point2d);
  list_points3d_in_.push_back(point3d);
  n_correspondences_++;
}

void Model::add_outlier(const cv::Point2f &point2d)
{
  list_points2d_out_.push_back(point2d);
}

void Model::add_descriptor(const cv::Mat &descriptor)
{
  descriptors_.push_back(descriptor);
}

void Model::add_keypoint(const cv::KeyPoint &kp)
{
  list_keypoints_.push_back(kp);
}


/** Save a CSV file and fill the object mesh */
void Model::save(const std::string path)
{
  cv::Mat points3dmatrix = cv::Mat(list_points3d_in_);
  cv::Mat points2dmatrix = cv::Mat(list_points2d_in_);
  //cv::Mat keyPointmatrix = cv::Mat(list_keypoints_);

  cv::FileStorage storage(path, cv::FileStorage::WRITE);
  storage << "points_3d" << points3dmatrix;
  storage << "points_2d" << points2dmatrix;
  storage << "keypoints" << list_keypoints_;
  storage << "descriptors" << descriptors_;

  storage.release();
}

/** Save a CSV file and fill the object mesh */
void Model::load(const std::string path)
{
  list_points3d_in_.clear();
  list_points2d_in_.clear();

  cv::FileStorage storage(path, cv::FileStorage::READ);

  cv::Mat points3d, points2d, keypoints;
  storage["points_3d"] >> points3d;
  storage["points_2d"] >> points2d;
  storage["keypoints"] >> list_keypoints_;
  storage["descriptors"] >> descriptors_;

  points3d.copyTo(list_points3d_in_);
  points2d.copyTo(list_points2d_in_);

  // load keypoints
  for (int i = 0; i < keypoints.cols; ++i) {
    cv::Vec<float, 7> v = keypoints.at< cv::Vec<float, 7> >(i,0);
    cv::KeyPoint kp(v[0], v[1], v[2], v[3], v[4], (int)v[5], (int)v[6]);
    list_keypoints_.push_back(kp);
  }

  storage.release();

  CsvWriter writter("../Data/box.xyz", " ");
  writter.writeXYZ(list_points3d_in_);
}


