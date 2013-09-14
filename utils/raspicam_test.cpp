#include <iostream>
#include <sstream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include "raspicam.h"
using namespace std;

RaspiCam cameraBoard;
volatile int pictures_taken = 0;
const int max_pictures = 5;

///Command line functions
//returns the index of a param or -1 if not present
int findParam ( std::string param,int argc, char *argv[] );
//Returns the value of a param. If not present, returns the defvalue
string getParamString ( string id,int argc,char **argv,string defvalue="" );
float getParamVal ( string id,int argc,char **argv,float defvalue=-1 );
//prints command line usage
void usage();



/// CODE
void saveImage(string filename, unsigned char *data, unsigned int length) {
	FILE * file = fopen(filename.c_str(), "w");
	fwrite(data, 1, length, file);
	fclose(file);
}

void onImageTaken(unsigned char *data, unsigned int image_offset, unsigned int length) {
	stringstream ss;
	ss << "output_image" << pictures_taken << ".bmp";
	string pTaken;
	ss >> pTaken;
	saveImage(pTaken, data + image_offset, length);
	cout << "Saved Image\n";
	pictures_taken++;
}
//prints program command line usage
void usage()
{
  cout<<"-w val : sets image width (640 default)"<<endl;
  cout<<"-h val : sets image height (480 default)"<<endl;
  cout<<"-iso val: set iso [100,800] (400 default)"<<endl;
}


int main(int argc, char *argv[]) {
  if (findParam("--help",argc,argv)!=-1) {
      usage();
      return 0;    
  }
  
	int width = getParamVal("-w",argc,argv,640);
	int height =getParamVal("-h",argc,argv,480);
	
	unsigned int length = 62 + width*height*3 + height*2; // Header + Image Data + Padding
	
	cout << "Initializing...\n";
	cameraBoard.setWidth(width);
	cameraBoard.setHeight(height);
	cameraBoard.setEncoding(CAMERA_BOARD_ENCODING_BMP);
	cameraBoard.setISO(getParamVal("-iso",argc,argv,400));
	
	if (cameraBoard.initialize()) return -1;
	
	unsigned char * data = new unsigned char[length];

	if (cameraBoard.takePicture(data, length)) return -1;
		saveImage("output_blocking.bmp", data, length);

	while (pictures_taken < max_pictures) {
		if (cameraBoard.startCapture(onImageTaken, data, 0, length)) return -1;
		const int currentImages = pictures_taken;
		while (currentImages == pictures_taken)
			usleep(2000);
		cameraBoard.stopCapture();
		usleep(3000);
	}
	return 0;
}


//returns the index of a param or -1 if not present
int findParam ( std::string param,int argc, char *argv[] )
{
    for ( int i=0; i<argc; i++ )
        if ( string ( argv[i] ) ==param ) return i;

    return -1;
}
//Returns the value of a param. If not present, returns the defvalue
float getParamVal ( string id,int argc,char **argv,float defvalue  )
{
    if ( findParam ( id,argc,argv ) !=-1 )
        return atof ( argv[  findParam ( id,argc,argv ) +1] );
    else return defvalue;
}
//Returns the value of a param as a string.  If not present, returns the defvalue
string getParamString ( string id,int argc,char **argv,string defvalue  )
{
  int idx=findParam ( id,argc,argv );
    if ( idx !=-1 ) return  argv[  idx+1] ;
    else return defvalue;
}
