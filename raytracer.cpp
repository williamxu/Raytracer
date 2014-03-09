
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


using namespace Eigen;
using namespace std;


//****************************************************
// Global Variables
//****************************************************

char*	filename = "output.bmp";
FIBITMAP* bitmap;
//RGBQUAD color;

//****************************************************
// Some Classes
//****************************************************
class Normal {
	Vector3f xyz;
	Normal(Vector3f vector){
		xyz = vector;
	}
/*Notes:
	Constructor from 3 floats
		Support + , -
	Note : Need to be normalized after operations(Be careful for 0 vector)*/
};

class Ray{
	Vector3f point;
	Vector3f direction;
	float t_min, t_max;
//Notes:
//	It represent the ray ray(t) = pos + t*dir, where t_min <= t <= t_max
};

//Matrix
//Notes :
	  //Support creation of rotation, translation, scaling matrices
	  //May support matrix inversion if needed
	  //Also could support SVD, or other matrix decomposition, for future extension

class Transformation{
	//Matrix m;
	//Matrix m_invt;
//Notes :
	//Support Point, Vector, Normal, Ray, LocalGeo transformation by
	//operator * overloading

};

class BRDF{
	Vector3f ka;
	Vector3f kd;
	Vector3f ks;
	Vector3f kr; //reflection coefficient
	// Storing information enough for shading (it is not the actual BRDF function // in the rendering equation that will be covered later in the semester)
};

class Sample{
	float x, y; // Store screen coordinate

};

class LocalGeo{
	Vector3f position;
	Normal normal;
//Notes:
//	Store the local geometry at the intersection point.May need to store
//		other quantities(eg.texture coordinate) in a more complicated
//		raytracer.
};

class Shape{
	// Test if ray intersects with the shape or not (in object space), if so,
	// return intersection point and normal
	bool intersect(Ray& ray, float* thit, LocalGeo* local){
	//TODO
	}
	// Same as intersect, but just return whether there is any intersection or
	// not
	bool intersectP(Ray& ray){
	//TODO
	}

//Notes:
	// Triangle and Sphere are probably best implemented here
	// The intersection with the ray at t outside the range [t_min, t_max]
	// should return false.

};

class Primitive{
	bool intersect(Ray& ray, float* thit, Intersection* in){
	//TODO
	}
	bool intersectP(Ray& ray){
	//TODO
	}
	void getBRDF(LocalGeo& local, BRDF* brdf);

//Notes:
	//Abstract class for primitives in the scene
};

class Intersection{
	LocalGeo localGeo;
	Primitive* primitive;
};

class GeometricPrimitive{
	Transformation objToWorld, worldToObj;
	Shape* shape;
	Material* mat;
	bool intersect(Ray& ray, float* thit, Intersection* in)  {
		/*Ray oray = worldToObj*ray;
		LocalGeo olocal;
		if (!shape->intersect(oray, thit, &olocal))  return false;
		in->primitive = this;
		in->local = objToWorld*olocal;
		return true;*/
	}

	bool intersectP(Ray& ray) {
		//Ray oray = worldToObj*ray;
		//return shape->intersectP(oray);
	}

	void getBRDF(LocalGeo& local, BRDF* brdf) {
		//mat = getBRDF(local, brdf);
	}
};

class AggregatePrimitive{
	//AggregatePrimitive(vector<Primitive*> list);
	bool intersect(Ray& ray, float* thit, Intersection* in);
	bool intersectP(Ray& ray);
	void getBRDF(LocalGeo& local, BRDF* brdf) {
		exit(1);
		// This should never get called, because in->primitive will
		// never be an aggregate primitive
	}

//Notes:
//	Constructor store the STL vector of pointers to primitives.
//		Intersect just loops through all the primitives in the list and
//		call the intersect routine.Compare thit and return that of the nearest one(because we want the first hit).
//		Also, the in->primitive should be set to the pointer to that primitive.
//		When you implement acceleration structure, it will replace this class.
};

class Material{
	BRDF constantBRDF;
	BRDF getBRDF(LocalGeo& local, BRDF* brdf) {
		return constantBRDF;
	}

//Notes:
//	Class for storing material.For this example, it just returns a constant
//		material regardless of what local is.Later on, when we want to support
//		texture mapping, this need to be modified.

};

class Sampler{
	bool getSample(Sample* sample);
//Notes:
//	It will generate(x, y) of a screen sample and return true.
//		Next time it gets called, it will generate another sample for the next pixel.It will return false when all the samples from all the pixels
//		are generated. (In our case, we generate 1 sample per pixel, at the pixel sample.Later on, if we want to do multi - sample per pixel, we need to modify this class.
};

class Camera{
	void generateRay(Sample& sample, Ray* ray);

//Notes:
//	Create a ray starting from the camera that passes through the
//		corresponding pixel(sample.x, sample.y) on the image plane.
//		(from last week discussion, and also section 10.1 in Shirley’s book)
};

class RayTracer{

	void trace(Ray& ray, int depth, Color* color) {
		//if (depth exceed some threshold) {
			//Make the color black and return
		//}
		//if (!primitive.intersect(ray, &thit, &in) {
			// No intersection
			//Make the color black and return
		//}
		// Obtain the brdf at intersection point
		//in.primitive->getBRDF(in.local, &brdf);

		// There is an intersection, loop through all light source
		//for (i = 0; i < #lights; i++) {
			//lights[i].generateLightRay(in.local, &lray, &lcolor);

			// Check if the light is blocked or not
			//if (!primitive->intersectP(lray))
				// If not, do shading calculation for this
				// light source
				//*color += shading(in.local, brdf, lray, lcolor);
		//}

		// Handle mirror reflection
		//if (brdf.kr > 0) {
			//reflectRay = createReflectRay(in.local, ray);

			// Make a recursive call to trace the reflected ray
			//trace(reflectRay, depth + 1, &tempColor);
			//*color += brdf.kr * tempColor;
		//}

	}
//Notes:
//	Shading is similar to hw2
//		Beware when you generate reflection ray, make sure the ray don’t start
//		exactly on the surface, or the intersection routine may return
//		intersection point at the starting point of the ray. (This apply to light
//		ray generation as well)

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

class Light{

//public:
//	int t; //enums not working, 0 = point, 1 = directional
//	Vector3f location;
//	Vector3f direction;
//	Color color;
//	Light(int s, Vector3f l, Color c){
//		t = s;
//		if (s == 0)	{
//			location = l;
//		}
//		direction = -l.normalized();
//		color = c;
//	}
//	Vector3f dVector(Vector3f v){
//		if (t == 1) return direction;
//		return (location - v).normalized();
//	}
	void generateLightRay(LocalGeo& local, Ray* lray, Color* lcolor);
	//Notes:
	//	This is an abstract class that will generate a ray starting from
	//		the position stored in local to the position of the light source.
	//		You might want to consider creating 2 derived classes for
	//		point light source and directional light source.
	//		For directional light, the origin of the ray is the same, and the ray points to the light direction, however, t_max is infinity.
};

class Scene{
public:
	void render() {
		while (!sampler.generateSample(&sample)) {
			camera.generateRay(sample, &ray);
			raytracer.trace(ray, &color);
			film.commit(sample, color);
		}
		film.writeImage();
	}
};

class Sampler{
public:
	bool generateSample(Sample* sample);
	//Notes:
	//It will generate(x, y) of a screen sample and return true.
	//Next time it gets called, it will generate another sample for the next pixel.
	//It will return false when all the samples from all the pixels are generated.
	//(In our case, we generate 1 sample per pixel, at the pixel sample.
	//Later on, if we want to do multi - sample per pixel, we need to modify this class.
};

class Film {
public:
	// Will write the color to (sample.x, sample.y) on the image
	void commit(Sample& sample, Color& color);
	// Output image to a file
	void writeImage();
//Notes:
	//Can be implemented just by a 2D array of Color(Later on, we can
		//implement more complicated things such as multi - sample per pixel, or
		//post processing, eg.tone mapping in this class)
};

//void saveFile(int w, int h){
//	bitmap = FreeImage_Allocate(w, h, 24);
//	color.rgbRed = 0;
//	color.rgbGreen = 0;
//	color.rgbBlue = 0;
//	for (int i = 0; i < w; i++){
//		for (int j = 0; i < h; i++){
//			FreeImage_SetPixelColor(bitmap, i, j, &color);
//		}
//	}			
//	int index = string(filename).find_last_of('.') + 1;
//	if (FreeImage_Save(FreeImage_GetFIFFromFormat(filename + index), bitmap, filename)) {
//		cout << "Image saved successfully." << endl;
//	}
//	
//}

int main(int argc, char *argv[]) {
	FreeImage_Initialise();
	for (int i = 1; i < argc; i++){
		//camera location
		//4 corners of image plane
		//output dimensions in pixels
	}
	FreeImage_DeInitialise();
	return 0;
}








