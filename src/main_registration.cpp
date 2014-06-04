#include <iostream>
#include <boost/lexical_cast.hpp>

#include "Mesh.h"
#include "Model.h"
#include "PnPProblem.h"
#include "RobustMatcher.h"
#include "ModelRegistration.h"
#include "Utils.h"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>


  /*
   * Set up the images paths
   */
  std::string img_path = "../Data/resized_IMG_3875.JPG";
  std::string ply_read_path = "../Data/box.ply";
  std::string write_path = "../Data/box.yml";

  // Boolean the know if the registration it's done
  bool end_registration = false;

  /*
   * Set up the intrinsic camera parameters: CANON
   */
  double f = 55;
  double sx = 22.3, sy = 14.9;
  double width = 718, height = 480;
  double params_CANON[] = { width*f/sx,   // fx
                            height*f/sy,  // fy
                            width/2,      // cx
                            height/2};    // cy


  // Setup the points to register in the image
  // In the order of the *.ply file and starting at 1
  int n = 7;
  int pts[] = {1, 2, 3, 5, 6, 7, 8};

  /*
   * Set up some basic colors
   */
  cv::Scalar red(0, 0, 255);
  cv::Scalar green(0,255,0);
  cv::Scalar blue(255,0,0);
  cv::Scalar yellow(0,255,255);

  /*
   * CREATE MODEL REGISTRATION OBJECT
   * CREATE OBJECT MESH
   * CREATE OBJECT MODEL
   * CREATE PNP OBJECT
   */
  ModelRegistration registration;
  Model model;
  Mesh mesh;
  PnPProblem pnp_registration(params_CANON);


// Mouse events for model registration
static void onMouseModelRegistration( int event, int x, int y, int, void* )
{
  if  ( event == cv::EVENT_LBUTTONUP )
  {
      int n_regist = registration.getNumRegist();
      int n_vertex = pts[n_regist];

      cv::Point2f point_2d = cv::Point2f(x,y);
      cv::Point3f point_3d = mesh.getVertex(n_vertex-1);

      bool is_registrable = registration.is_registrable();
      if (is_registrable)
      {
        registration.registerPoint(point_2d, point_3d);
        if( registration.getNumRegist() == registration.getNumMax() ) end_registration = true;
      }
  }
}

/*
 *   MAIN PROGRAM
 *
 */

int main(int, char**)
{

  std::cout << "!!!Hello Registration!!!" << std::endl;

  // load a mesh given the *.ply file path
  mesh.load(ply_read_path);

  // set parameters
  int numKeyPoints = 5000;

  //Instantiate robust matcher: detector, extractor, matcher
  RobustMatcher rmatcher;
  cv::FeatureDetector* detector = new cv::OrbFeatureDetector(numKeyPoints);
  rmatcher.setFeatureDetector(detector);


  /*
   * GROUND TRUTH OF THE FIRST IMAGE
   *
   * by the moment it is the reference image
   */

  // Create & Open Window
  cv::namedWindow("MODEL REGISTRATION", cv::WINDOW_KEEPRATIO);

  // Set up the mouse events
  cv::setMouseCallback("MODEL REGISTRATION", onMouseModelRegistration, 0 );

  // Open the image to register
  cv::Mat img_in = cv::imread(img_path, cv::IMREAD_COLOR);
  cv::Mat img_vis = img_in.clone();

  if (!img_in.data) {
    std::cout << "Could not open or find the image" << std::endl;
    return -1;
  }

  // Set the number of points to register
  int num_registrations = n;
  registration.setNumMax(num_registrations);

  std::cout << "Click the box corners ..." << std::endl;
  std::cout << "Waiting ..." << std::endl;

  // Loop until all the points are registered
  while ( cv::waitKey(30) < 0 )
  {
    // Refresh debug image
    img_vis = img_in.clone();

    // Current registered points
    std::vector<cv::Point2f> list_points2d = registration.get_points2d();
    std::vector<cv::Point3f> list_points3d = registration.get_points3d();

    // Draw current registered points
    drawPoints(img_vis, list_points2d, list_points3d, red);

    // If the registration is not finished, draw which 3D point we have to register.
    // If the registration is finished, breaks the loop.
    if (!end_registration)
    {
      // Draw debug text
      int n_regist = registration.getNumRegist();
      int n_vertex = pts[n_regist];
      cv::Point3f current_poin3d = mesh.getVertex(n_vertex-1);

      drawQuestion(img_vis, current_poin3d, green);
      drawCounter(img_vis, registration.getNumRegist(), registration.getNumMax(), red);
    }
    else
    {
      // Draw debug text
      drawText(img_vis, "END REGISTRATION", green);
      drawCounter(img_vis, registration.getNumRegist(), registration.getNumMax(), green);
      break;
    }

    // Show the image
    cv::imshow("MODEL REGISTRATION", img_vis);
  }


  /*
   *
   * COMPUTE CAMERA POSE
   *
   */

  std::cout << "COMPUTING POSE ..." << std::endl;

  // The list of registered points
  std::vector<cv::Point2f> list_points2d = registration.get_points2d();
  std::vector<cv::Point3f> list_points3d = registration.get_points3d();

  // Estimate pose given the registered points
  bool is_correspondence = pnp_registration.estimatePose(list_points3d, list_points2d, cv::ITERATIVE);
  if ( is_correspondence )
  {
    std::cout << "Correspondence found" << std::endl;

    // Compute all the 2D points of the mesh to verify the algorithm and draw it
    std::vector<cv::Point2f> list_points2d_mesh = pnp_registration.verify_points(&mesh);
    draw2DPoints(img_vis, list_points2d_mesh, green);

  } else {
    std::cout << "Correspondence not found" << std::endl << std::endl;
  }

  // Show the image
  cv::imshow("MODEL REGISTRATION", img_vis);

  // Show image until ESC pressed
  cv::waitKey(0);


   /*
    *
    * COMPUTE 3D of the image Keypoints
    *
    */


  // Containers for keypoints and descriptors of the model
  std::vector<cv::KeyPoint> keypoints_model;
  cv::Mat descriptors;

  // Compute keypoints and descriptors
  rmatcher.computeKeyPoints(img_in, keypoints_model);
  rmatcher.computeDescriptors(img_in, keypoints_model, descriptors);

  // Check if keypoints are on the surface of the registration image and add to the model
  for (unsigned int i = 0; i < keypoints_model.size(); ++i) {
    cv::Point2f point2d(keypoints_model[i].pt);
    cv::Point3f point3d;
    bool on_surface = pnp_registration.backproject2DPoint(&mesh, point2d, point3d);
    if (on_surface)
    {
        model.add_correspondence(point2d, point3d);
        model.add_descriptor(descriptors.row(i));
        model.add_keypoint(keypoints_model[i]);
    }
    else
    {
        model.add_outlier(point2d);
    }
  }

  // save the model into a *.yaml file
  model.save(write_path);

  // Out image
  img_vis = img_in.clone();

  // The list of the points2d of the model
  std::vector<cv::Point2f> list_points_in = model.get_points2d_in();
  std::vector<cv::Point2f> list_points_out = model.get_points2d_out();

  // Draw some debug text
  std::string num = boost::lexical_cast< std::string >(list_points_in.size());
  std::string text = "There are " + num + " inliers";
  drawText(img_vis, text, green);

  // Draw some debug text
  num = boost::lexical_cast< std::string >(list_points_out.size());
  text = "There are " + num + " outliers";
  drawText2(img_vis, text, red);

  // Draw the object mesh
  drawObjectMesh(img_vis, &mesh, &pnp_registration, blue);

  // Draw found keypoints depending on if are or not on the surface
  draw2DPoints(img_vis, list_points_in, green);
  draw2DPoints(img_vis, list_points_out, red);

  // Show the image
  cv::imshow("MODEL REGISTRATION", img_vis);

  // Wait until ESC pressed
  cv::waitKey(0);

  // Close and Destroy Window
  cv::destroyWindow("MODEL REGISTRATION");

  std::cout << "GOODBYE" << std::endl;

}