
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


//****************************************************
// Some Classes
//****************************************************
//
//class Normal {
//public:
//	Vector3f xyz;
//	Normal(){
//		xyz = Vector3f(0,0,0);
//	}
//	Normal(Vector3f vector){
//		xyz = vector;
//	}
//	/*Notes:
//	Constructor from 3 floats
//		Support + , -
//	Note : Need to be normalized after operations(Be careful for 0 vector)*/
//};
//

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

class Ray{
public:
	Vector3f point;
	Vector3f direction;
	float t_min, t_max;
	Ray(){
		point = Vector3f(0, 0, 0);
		direction = Vector3f(0, 0, 0);
		t_min = 0;
		t_max = 0;
	}
	//	The ray ray(t) = point + t*direction, where t_min <= t <= t_max
	Ray(Vector3f p, Vector3f d, float tmin, float tmax){
		point = p;
		direction = d;
		t_min = tmin;
		t_max = tmax;
	}

};
//
////Matrix
////Notes :
//	  //Support creation of rotation, translation, scaling matrices
//	  //May support matrix inversion if needed
//	  //Also could support SVD, or other matrix decomposition, for future extension
//
//class Transformation{
//public:
//	Transformation(){}
//	//Matrix m;
//	//Matrix m_invt;
////Notes :
//	//Support Point, Vector, Normal, Ray, LocalGeo transformation by
//	//operator * overloading
//
//};
//
//class BRDF{
//public:
//	Vector3f ka;
//	Vector3f kd;
//	Vector3f ks;
//	Vector3f kr; //reflection coefficient
//	// Storing information enough for shading (it is not the actual BRDF function // in the rendering equation that will be covered later in the semester)
//};
//
class Sample{
public:
	int dx, dy; // Store screen coordinate
	Sample(){
		dx = 0;
		dy = 0;
	}

	Sample(int x, int y){
		dx = x; dy = y;
	}

};
//
//class LocalGeo{
//public:
//	Vector3f position;
//	Normal normal;
////Notes:
////	Store the local geometry at the intersection point.May need to store
////		other quantities(eg.texture coordinate) in a more complicated
////		raytracer.
//};
//
//class Shape{
//public:
//	// Test if ray intersects with the shape or not (in object space), if so,
//	// return intersection point and normal
//	bool intersect(Ray& ray, float* thit, LocalGeo* local){
//	//TODO
//	}
//	// Same as intersect, but just return whether there is any intersection or
//	// not
//	bool intersectP(Ray& ray){
//	//TODO
//	}
//
////Notes:
//	// Triangle and Sphere are probably best implemented here
//	// The intersection with the ray at t outside the range [t_min, t_max]
//	// should return false.
//
//};
//
//class Primitive{
//public:
//
//	bool intersect(Ray& ray, float* thit, Intersection* in){
//	//TODO
//	}
//	bool intersectP(Ray& ray){
//	//TODO
//	}
//	void getBRDF(LocalGeo& local, BRDF* brdf);
//
////Notes:
//	//Abstract class for primitives in the scene
//};
//
//class Intersection{
//public:
//	LocalGeo localGeo;
//	Primitive* primitive;
//};
//
//class GeometricPrimitive{
//public:
//	Transformation objToWorld, worldToObj;
//	Shape* shape;
//	Material* mat;
//	bool intersect(Ray& ray, float* thit, Intersection* in)  {
//		/*Ray oray = worldToObj*ray;
//		LocalGeo olocal;
//		if (!shape->intersect(oray, thit, &olocal))  return false;
//		in->primitive = this;
//		in->local = objToWorld*olocal;
//		return true;*/
//	}
//
//	bool intersectP(Ray& ray) {
//		//Ray oray = worldToObj*ray;
//		//return shape->intersectP(oray);
//	}
//
//	void getBRDF(LocalGeo& local, BRDF* brdf) {
//		//mat = getBRDF(local, brdf);
//	}
//};
//
//class AggregatePrimitive{
//	//AggregatePrimitive(vector<Primitive*> list);
//	bool intersect(Ray& ray, float* thit, Intersection* in);
//	bool intersectP(Ray& ray);
//	void getBRDF(LocalGeo& local, BRDF* brdf) {
//		exit(1);
//		// This should never get called, because in->primitive will
//		// never be an aggregate primitive
//	}
//
////Notes:
////	Constructor store the STL vector of pointers to primitives.
////		Intersect just loops through all the primitives in the list and
////		call the intersect routine.Compare thit and return that of the nearest one(because we want the first hit).
////		Also, the in->primitive should be set to the pointer to that primitive.
////		When you implement acceleration structure, it will replace this class.
//};
//
//class Material{
//	BRDF constantBRDF;
//	BRDF getBRDF(LocalGeo& local, BRDF* brdf) {
//		return constantBRDF;
//	}
//
////Notes:
////	Class for storing material.For this example, it just returns a constant
////		material regardless of what local is.Later on, when we want to support
////		texture mapping, this need to be modified.
//
//};
//
class Camera{
public:
	Vector3f position;
	Camera(){
		position = Vector3f(0, 0, 0);
	}
	Camera(Vector3f p){
		position = p;
	}
	void generateRay(Sample& sample, Ray* ray){

	}

//Notes:
//	Create a ray starting from the camera that passes through the
//		corresponding pixel(sample.x, sample.y) on the image plane.
//		(from last week discussion, and also section 10.1 in Shirley’s book)
};

class Sampler{
public:
	int dx, dy; //dimension of our output
	int curr_x = 0;
	int curr_y = 0;
	Sampler(){
		dx = 0;
		dy = 0;
	}
	Sampler(int x, int y){
		dx = x;
		dy = y;
	}
	bool generateSample(Sample* sample){
		sample = &Sample(curr_x, curr_y);
		if (curr_x == dx) {
			if (curr_y == dy){
				return false;
			}
			curr_y += 1;
			curr_x = 0;
		}
		else{
			curr_x += 1;
		}
		return true;
	}
	//Notes:
	//It will generate(x, y) of a screen sample and return true.
	//Next time it gets called, it will generate another sample for the next pixel.
	//It will return false when all the samples from all the pixels are generated.
	//(In our case, we generate 1 sample per pixel, at the pixel sample.
	//Later on, if we want to do multi - sample per pixel, we need to modify this class.
};

class Film {

public:
	FIBITMAP* bitmap;
	int dx, dy;
	Film(){
		dx = 0;
		dy = 0;
	}
	Film(int x, int y){
		FreeImage_Initialise();
		dx = x; 
		dy = y;
		bitmap = FreeImage_Allocate(dx, dy, 24); //24?


	}
	// Will write the color to (sample.x, sample.y) on the image
	void commit(Sample& sample, Color& c){
		RGBQUAD color;
		color.rgbRed = min(c.rgb[0], 1) * 255.0;
		color.rgbGreen = min(c.rgb[1], 1) * 255.0;
		color.rgbBlue = min(c.rgb[2], 1) * 255.0;
		FreeImage_SetPixelColor(bitmap, sample.dx, sample.dy, &color);

	}
	// Output image to a file
	void writeImage(){
		char* filename = "output.bmp";
		int index = string(filename).find_last_of('.') + 1;
		if (FreeImage_Save(FreeImage_GetFIFFromFormat(filename + index), bitmap, filename)) {
			cout << "Image saved successfully." << endl;
		}
		FreeImage_DeInitialise();
	}
	//Notes:
	//Can be implemented just by a 2D array of Color(Later on, we can
	//implement more complicated things such as multi - sample per pixel, or
	//post processing, eg.tone mapping in this class)
};

class RayTracer{
public:

	void trace(Ray& ray, int depth, Color* color) {
		//if (depth <= 0){
		//Make the color black and return
		color = &Color();
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

//class Light{
//
//public:
////	Vector3f location;
////	Vector3f direction;
//	Color color;
//	//Light(int s, Vector3f l, Color c){
////		if (s == 0)	{
////			location = l;
////		}
////		direction = -l.normalized();
////		color = c;
////	}
////	Vector3f dVector(Vector3f v){
////		if (t == 1) return direction;
////		return (location - v).normalized();
////	}
//	void generateLightRay(LocalGeo& local, Ray* lray, Color* lcolor);
//	//Notes:
//	//	This is an abstract class that will generate a ray starting from
//	//		the position stored in local to the position of the light source.
//	//		You might want to consider creating 2 derived classes for
//	//		point light source and directional light source.
//	//		For directional light, the origin of the ray is the same, and the ray points to the light direction, however, t_max is infinity.
//};

class Scene{
public:
	Vector3f eye; //the camera. can get position
	Vector3f LL, LR, UL, UR; // the four corners of the image plane
	int dx, dy; //the output image dimensions in pixels
	int recursionDepth;
	Sampler sampler = Sampler();
	Camera camera = Camera();
	RayTracer raytracer = RayTracer();
	Film film = Film();
	Sample sample = Sample();
	Ray ray = Ray();
	Color color = Color();
	//necessary objects
	Scene(Vector3f e, Vector3f ll, Vector3f lr, Vector3f ul, Vector3f ur, int x, int y, int depth){
		eye = e;
		LL = ll;
		LR = lr;
		UL = ul;
		UR = ur;
		dx = x;
		dy = y;
		recursionDepth = depth;
		sampler = Sampler(dx, dy);
		camera = Camera(eye);
		film = Film(dx, dy);
	}

	void render() {	
		while (sampler.generateSample(&sample)) {
			camera.generateRay(sample, &ray);
			raytracer.trace(ray, recursionDepth, &color);
			film.commit(sample, color);
		}
		film.writeImage();
	}
};

int main(int argc, char *argv[]) {
	
	//hardcoded values:
	Vector3f eye = Vector3f(0, 0, 1);
	Vector3f ll = Vector3f(-1, -1, 0);
	Vector3f lr = Vector3f(1, -1, 0);
	Vector3f ul = Vector3f(-1, 1, 0);
	Vector3f ur = Vector3f(1, 1, 0);
	Scene s = Scene(eye, ll, lr, ul, ur, 100, 100, 1);
	s.render();
	return 0;
}








