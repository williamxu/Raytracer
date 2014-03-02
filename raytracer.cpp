
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

#include <time.h>
#include <math.h>

#include <Dense>
#include "FreeImage/FreeImage.h"

inline float sqr(float x) { return x*x; }

using namespace Eigen;
using namespace std;

//****************************************************
// Some Classes
//****************************************************

class Viewport {
  public:
    int w, h; // width and height
};

class Color {
public:
	Vector3f rgb;
	Color(){
		rgb = Vector3f(0.0, 0.0, 0.0);
	}
	Color(Vector3f values){
		rgb = values;
	}
	Color operator+(Color c){
		return Color(Vector3f(rgb(0) + c.rgb(0), rgb(1) + c.rgb(1), rgb(2) + c.rgb(2)));
	}
	Color operator*(Color c){
		return Color(Vector3f(rgb(0) * c.rgb(0), rgb(1) * c.rgb(1), rgb(2) * c.rgb(2)));
	}
	Color operator*(float f){
		return Color(Vector3f(rgb(0) * f, rgb(1) * f, rgb(2) * f));
	}

};

class Light {
public:
	int t; //enums not working, 0 = point, 1 = directional
	Vector3f location;
	Vector3f direction;
	Color color;
	Light(int s, Vector3f l, Color c){
		t = s;
		if (s == 0)	{ 
			location = l; 
		}
		direction = -l.normalized();
		color = c;
	}
	Vector3f dVector(Vector3f v){
		if (t == 1) return direction;
		return (location - v).normalized();
	}
};

//****************************************************
// Global Variables
//****************************************************
Viewport	viewport;
Color	ka = Color(Vector3f(0.0,0.0,0.0));
Color	kd = Color(Vector3f(0.0, 0.0, 0.0));
Color	ks = Color(Vector3f(0.0, 0.0, 0.0));
//Color	cool = Color(Vector3f(0.0, 0.0, 0.0));
//Color	warm = Color(Vector3f(0.0, 0.0, 0.0));
//
//float	sp = 0;
//
//vector<Light>	pLights;
//vector<Light>	dLights;
//
//int		ss = 0;
//int		cw = 0;
//int		toon = 0;
char*	filename = "";

FIBITMAP* bitmap = FreeImage_Allocate(400, 400, 24);
RGBQUAD color;

Color ambient(){
	return ka;
}

Color diffuse(Vector3f normal, Vector3f lightVector, Color lightColor) {
	return kd * lightColor * max(normal.dot(lightVector), 0);
}

Color specular(Vector3f normal, Vector3f lightVector, Color lightColor, float power) {
	Vector3f reflect = (-lightVector + 2 * lightVector.dot(normal) * normal).normalized();
	return ks * lightColor * pow(max(reflect.dot(Vector3f(0.0, 0.0, 1.0)), 0), power);
}

//float warmth(Vector3f normal, Vector3f lightVector) {
//	return (1 + normal.dot(lightVector)) / 2;
//}

void setPixel(int x, int y, Color c) {
	//glColor3f(c.rgb(0), c.rgb(1), c.rgb(2));
	//glVertex2f(x + 0.5, y + 0.5);   // The 0.5 is to target pixel centers 
}

//void saveFile(){
//	
//	FreeImage_SetPixelColor(bitmap, i, j, &color);
//				
//	int index = string(filename).find_last_of('.') + 1;
//	if (FreeImage_Save(FreeImage_GetFIFFromFormat(filename + index), bitmap, filename)) {
//		cout << "Image saved successfully." << endl;
//	}
//	
//}


int main(int argc, char *argv[]) {
	FreeImage_Initialise();
	viewport.w = 400;
	viewport.h = 400;	
	FreeImage_DeInitialise();
	return 0;
}








