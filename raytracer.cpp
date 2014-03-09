
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

inline float sqr(float x) { return x*x; }
//****************************************************
// Global Variables
//****************************************************


//****************************************************
// Some Classes
//****************************************************

class Intersection;

class Normal {
public:
	Vector3f xyz;
	Normal(){
		xyz = Vector3f(0,0,1);
	}
	Normal(Vector3f vector){
		xyz = vector.normalized();
	}
	/*Notes:
	Constructor from 3 floats
		Support + , -
	Note : Need to be normalized after operations(Be careful for 0 vector)*/
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
	Vector3f atTime(float time){
		if (time > t_max || time < t_min){  //simple test
			cout << "t is out of bounds" << endl;
		}
		return point + time * direction;
	}

};

//Matrix

//Notes :
//Support creation of rotation, translation, scaling matrices
//May support matrix inversion if needed
//Also could support SVD, or other matrix decomposition, for future extension

//class Transformation{
//public:
//	Matrix4f m;
//	Matrix4f m_inv;
//	Transformation(){
//		m = Matrix4f();
//	}
//	Transformation(Matrix4f mat, Matrix4f mat_inv){
//		m = mat;
//		m_inv = mat_inv;
//	}
//	//Normal operator* (Normal n){
//	//	
//	//}
//	//Ray operator* (Ray r){
//	//}
//	//LocalGeo operator* (LocalGeo lg){
//	//}
//
//	//Notes :
//	//Support Point, Vector, Normal, Ray, LocalGeo transformation by
//	//operator * overloading
//
//};

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

//class Light{

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

class BRDF{
public:
	Vector3f ka;
	Vector3f kd;
	Vector3f ks;
	Vector3f kr; //reflection coefficient
	BRDF(){
		ka = Vector3f(0, 0, 0);
		kd = Vector3f(0, 0, 0);
		ks = Vector3f(0, 0, 0);
		kr = Vector3f(0, 0, 0);
	}
	BRDF(Vector3f ambient, Vector3f diffuse, Vector3f specular, Vector3f reflection){
		ka = ambient;
		kd = diffuse;
		ks = specular;
		kr = reflection;
	}
};

//class Material{
//	// Class for storing material. 
//	// For this example, it just returns a constant material regardless of what local is.
//	// Later on, when we want to support texture mapping, this need to be modified.
//public:
//	BRDF constantBRDF;
//	Material(){
//		constantBRDF = BRDF();
//	}
//	Material(BRDF b){
//		constantBRDF = b;
//	}
//	BRDF getBRDF(LocalGeo& local, BRDF* brdf) {
//		return constantBRDF;
//	}
//};

//class Intersection{
//public:
//	LocalGeo localGeo;
//	Primitive* primitive;
//};

class LocalGeo{
	//	Store the local geometry at the intersection point.May need to store
	//		other quantities(eg.texture coordinate) in a more complicated
	//		raytracer.
public:
	Vector3f position;
	Normal normal;
	LocalGeo(){
		position = Vector3f(0, 0, 0);
		normal = Normal();
	}
	LocalGeo(Vector3f p, Normal n){
		position = p;
		normal = n;
	}

};

class Shape{
public:
	BRDF brdf;
	virtual bool intersect(Ray& ray, float* thit, LocalGeo* local) = 0;
	virtual bool intersectP(Ray& ray) = 0;
};

class Sphere : public Shape{
public:
	Vector3f center;
	float radius;
	Sphere(){
		center = Vector3f(0, 0, 0);
		radius = 1.0;
	}
	Sphere(Vector3f c, float r, BRDF color){
		brdf = color;
		center = c;
		radius = r;
	}
	bool intersect(Ray& ray, float* thit, LocalGeo* local) {
		float A = ray.direction.squaredNorm();
		float B = (ray.direction * 2.0).dot(ray.point - center);
		float C = (ray.point - center).squaredNorm() - sqr(radius);
		float disc =  sqr(B) - 4*A*C;
		if (disc >= 0) {
			float t = min((-B + sqrt(disc)) / (2 * A), (-B - sqrt(disc)) / (2 * A));
			if (t > ray.t_min && t < ray.t_max){
					*thit = t;
					Vector3f p = ray.atTime(t);
					*local = LocalGeo(p, Normal(2 * (p - center)));
					return true;
				}
			}
		return false;
	}
	bool intersectP(Ray& ray) {
		float A = ray.direction.squaredNorm();
		float B = (ray.direction * 2.0).dot(ray.point - center);
		float C = (ray.point - center).squaredNorm() - sqr(radius);
		float disc = sqr(B) - 4 * A*C;
		return disc >= 0;
	}
};

//Primitive will transform the ray from world space to object space
//and hand it to Shape to do intersection in object space.
//Shape then computes the intersection and returns the result to Primitive.
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
//class GeometricPrimitive : Primitive{
//public:
//	Transformation objToWorld, worldToObj;
//	Shape* shape;
//	Material* mat;
//	
//	bool intersect(Ray& ray, float* thit, Intersection* in)  {
//		Ray oray = worldToObj * ray;
//		LocalGeo olocal;
//		if (!shape->intersect(oray, thit, &olocal))  return false;
//		in->primitive = this;
//		in->localGeo = objToWorld * olocal;
//		return true;
//	}
//
//	bool intersectP(Ray& ray) {
//		Ray oray = worldToObj*ray;
//		return shape->intersectP(oray);
//	}
//
//	void getBRDF(LocalGeo& local, BRDF* brdf) {
//		mat = getBRDF(local, brdf);
//	}
//};
//
//class AggregatePrimitive{
//	AggregatePrimitive(vector<Primitive*> list);
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



class Sample{
public:
	float dx, dy; // Store screen coordinate
	Sample(){
		dx = 0;
		dy = 0;
	}

	Sample(float x, float y){
		dx = x;
		dy = y;
	}

};

class Sampler{
public:
	float dx, dy; //dimension of our output
	float curr_x = 0.0;
	float curr_y = 0.0;
	Sampler(){
		dx = 0.0;
		dy = 0.0;
	}
	Sampler(float x, float y){
		dx = x;
		dy = y;
	}
	//generates one sample per pixel
	bool generateSample(Sample* sample){
		*sample = Sample(curr_x, curr_y);
		if (curr_x >= dx) {
			if (curr_y >= dy){
				return false;
			}
			curr_y += 1.0;
			curr_x = 0.0;
		}
		else{
			curr_x += 1.0;
		}
		return true;
	}
};

class Film {

public:
	FIBITMAP* bitmap;
	int dx, dy;
	Film(){
		dx = 0.0;
		dy = 0.0;
	}
	Film(float x, float y){
		FreeImage_Initialise();
		dx = (int) x; 
		dy = (int) y;
		bitmap = FreeImage_Allocate(dx, dy, 24); //24?
	}
	// Will write the color to (sample.x, sample.y) on the image
	void commit(Sample& sample, Color& c){
		RGBQUAD color;
		color.rgbRed = min(c.rgb[0], 1) * 255.0;
		color.rgbGreen = min(c.rgb[1], 1) * 255.0;
		color.rgbBlue = min(c.rgb[2], 1) * 255.0;
		FreeImage_SetPixelColor(bitmap, (int) sample.dx, (int) sample.dy, &color);

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
};

class Camera{
public:
	Vector3f eye;
	Vector3f LL;
	Vector3f LR;
	Vector3f UL;
	Vector3f UR;
	float dx;
	float dy;
	float t_min;
	float t_max;
	Camera(){
		eye = Vector3f(0, 0, 1);
		LL = Vector3f(-1, -1, 0);
		LR = Vector3f(1, -1, 0);
		UL = Vector3f(-1, 1, 0);
		UR = Vector3f(1, 1, 0);
		dx = 0; dy = 0;
		t_min = 1;
		t_max = FLT_MAX;
	}
	Camera(Vector3f p, Vector3f ll, Vector3f lr, Vector3f ul, Vector3f ur, float x, float y, float tmin, float tmax){
		LL = ll;
		LR = lr;
		UL = ul;
		UR = ur;
		eye = p;
		dx = x;
		dy = y;
		t_min = tmin;
		t_max = tmax;
	}
	void generateRay(Sample& sample, Ray* ray){
		float u = (sample.dx + 0.5) / dx;
		float v = (sample.dy + 0.5) / dy;
		Vector3f p = u * (v * LL + (1 - v) * UL) + (1 - u) * (v * LR + (1 - v) * UR);
		*ray = Ray(eye, p - eye, t_min, t_max);
	}

	//Notes:
	//	Create a ray starting from the camera that passes through the
	//		corresponding pixel(sample.x, sample.y) on the image plane.
	//		(from last week discussion, and also section 10.1 in Shirley’s book)
};

class RayTracer{
public:
	Sphere sphere;

	RayTracer(){

	}
	void addSphere(Sphere s){
		sphere = s;
	}
	void trace(Ray& ray, int depth, Color* color) {
		Color c = Color();
		float thit = 0;
		LocalGeo lg = LocalGeo();
		if (depth <= 0){
		//Make the color black and return
			*color = c;
			return;
		}
		//if (!primitive.intersect(ray, &thit, &in) {
		// No intersection
		//Make the color black and return
		//}
		if (!sphere.intersect(ray, &thit, &lg)){
			*color = c;
			return;
		}
		*color = Color(Vector3f(1.0, 0, 0));

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

class Scene{
public:
	Vector3f eye; //the camera. can get position
	Vector3f LL, LR, UL, UR; // the four corners of the image plane
	float dx, dy; //the output image dimensions in pixels
	int recursionDepth;
	Sampler sampler = Sampler();
	Camera camera = Camera();
	RayTracer raytracer = RayTracer();
	Film film = Film();

	//shapes list
	//necessary objects
	Scene(Vector3f e, Vector3f ll, Vector3f lr, Vector3f ul, Vector3f ur, float x, float y, int depth){
		eye = e;
		LL = ll;
		LR = lr;
		UL = ul;
		UR = ur;
		dx = x;
		dy = y;
		recursionDepth = depth;
		sampler = Sampler(dx, dy);
		camera = Camera(eye, ll, lr, ul, ur, x, y, 1, FLT_MAX);
		film = Film(dx, dy);
	}

	void addSphere(Sphere s){
		raytracer.addSphere(s);
	}

	void render() {	
		Sample sample = Sample();
		Ray ray = Ray();
		Color color = Color();
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

	Vector3f eye = Vector3f(0, 0, 0);
	Vector3f ll = Vector3f(-1, -1, -1);
	Vector3f lr = Vector3f(1, -1, -1);
	Vector3f ul = Vector3f(-1, 1, -1);
	Vector3f ur = Vector3f(1, 1, -1);
	Scene s = Scene(eye, ll, lr, ul, ur, 400, 400, 1);
	BRDF spherebrdf = BRDF(Vector3f(1.0, 0, 0), Vector3f(0, 0, 0), Vector3f(0, 0, 0), Vector3f(0, 0, 0));
	Sphere sp = Sphere(Vector3f(0, 0, -2), 1.0, spherebrdf);
	s.addSphere(sp);
	s.render();
	return 0;
}








