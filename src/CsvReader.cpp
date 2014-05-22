#include <boost/lexical_cast.hpp>
#include "CsvReader.h"

/** The default constructor of the CSV reader Class */
CsvReader::CsvReader(const std::string &path, const char &separator){
	_file.open(path.c_str(), ifstream::in);
	_separator = separator;
}

/** Read a plane text file with 3D coordinates numbers per point */
void CsvReader::read(std::vector<cv::Point3f> &list_pts)
{
	std::string line,x, y, z;
	cv::Point3f tmp_p;
  while (getline(_file, line)) {
      stringstream liness(line);
      getline(liness, x, _separator);
      getline(liness, y, _separator);
      getline(liness, z);
      if(!x.empty() && !y.empty() && !z.empty()) {
          tmp_p.x = atof(x.c_str());
          tmp_p.y = atof(y.c_str());
          tmp_p.z = atof(z.c_str());
          list_pts.push_back(tmp_p);
      }
  }
}

/* Read a plane text file with .ply format */
void CsvReader::readPLY(std::vector<Vertex> &list_vertex, std::vector<std::vector<int> > &list_triangles)
{
std::string line, tmp_str, n;
  int num_vertex, num_triangles;
  int count = 0;
  bool end_header = false;
  bool end_vertex = false;

  // Read the whole *.ply file
  while (getline(_file, line)) {
    stringstream liness(line);

    // read header
    if(!end_header)
    {
      getline(liness, tmp_str, _separator);
      if( tmp_str == "element" )
      {
          getline(liness, tmp_str, _separator);
          getline(liness, n);
          if(tmp_str == "vertex") num_vertex = boost::lexical_cast< int >(n);
          if(tmp_str == "face") num_triangles = boost::lexical_cast< int >(n);
      }
      if(tmp_str == "end_header") end_header = true;
    }

    // read file content
    else if(end_header)
    {
      // read vertex and add into 'list_vertex'
      if(!end_vertex && count < num_vertex)
      {
        string x, y, z;
        getline(liness, x, _separator);
        getline(liness, y, _separator);
        getline(liness, z);

        cv::Point3f tmp_p;
        tmp_p.x = boost::lexical_cast< float >(x);
        tmp_p.y = boost::lexical_cast< float >(y);
        tmp_p.z = boost::lexical_cast< float >(z);
        list_vertex.push_back(Vertex(count,tmp_p));

        count++;
        if(count == num_vertex)
        {
          count = 0;
          end_vertex = !end_vertex;
        }
      }
      // read faces and add into 'list_triangles'
      else if(end_vertex  && count < num_triangles)
      {
    	  std::string num_pts_per_face, id0, id1, id2;
        getline(liness, num_pts_per_face, _separator);
        getline(liness, id0, _separator);
        getline(liness, id1, _separator);
        getline(liness, id2);

        std::vector<int> tmp_triangle(3);
        tmp_triangle[0] = boost::lexical_cast< int >(id0);
        tmp_triangle[1] = boost::lexical_cast< int >(id1);
        tmp_triangle[2] = boost::lexical_cast< int >(id2);
        list_triangles.push_back(tmp_triangle);

        count++;
      }
    }
  }
}

