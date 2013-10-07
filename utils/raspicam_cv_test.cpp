#include "raspicam_cv.h"
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
using namespace std;
int main(int argc,char **argv){
  
    try{
      if (argc!=1){cerr<<"Usage: "<<endl;return -1;}
      RaspiCam_Cv Camera;
      if(!Camera.open()) {cerr<<"Could not open camera"<<endl;return -1;}
      //grab n images and save at the end
      int nFrames=15;
      cout<<"Start capture of "<<nFrames<<" images"<<endl;
      double startT=cv::getTickCount();
      for(int i=0;i<nFrames;i++){
	cout<<i<< " "<<std::flush;
	Camera.grab();
      }
      
      double fps=(cv::getTickCount()-startT)/double(cv::getTickFrequency())/float(nFrames);//time in secs
      
      cout<<endl<<"Capturing at "<<fps<<" fps"<<endl;
      cv::Mat image;
      Camera.retrieve(image);
      cv::imwrite("image_cv.jpg",image);
      cout<<"Output saved in image_cv.jpg"<<endl;
      
    }catch(std::exception &ex){
      cerr<<ex.what()<<endl;
    }
}
