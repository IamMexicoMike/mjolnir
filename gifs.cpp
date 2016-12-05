#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <vector>

using namespace cv;
using namespace std;

void mostrar_gif(string filename)
{
  VideoCapture cap;
  cap.open(filename);
  if (!cap.isOpened())
  {
    cout << "error leyendo gif" << endl;
  }

  Mat frame;
  vector<Mat> frames;
  frames.reserve(100);
  int idx=0;
  while(true)
  {
    cap >> frame;
    if (frame.empty())
      break;
    frames.emplace_back(frame.clone());
    ++idx;
  }

  while(true)
  {
    for(unsigned int i=0; i<frames.size() - 1; ++i)
    {
      imshow("frames", frames[i]);
      if (waitKey(30) == 27)
      {
        destroyWindow("frames");
        return;
      }

    }
  }
}
