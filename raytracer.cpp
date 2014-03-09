
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

class Intersection;
class Light;

enum LightType { POINTLIGHT, DIRECTIONALLIGHT };

inline float sqr(float x) { return x*x; }


//****************************************************
// Global Variables
//****************************************************
vector<Light>	pLights;
vector<Light>	dLights;
float specularCoefficient = 16; //0
char* filename = "output.bmp";

//****************************************************
// Some Classes
//****************************************************


class Normal {
public:
	Vector3f xyz;
	Normal(){
		xyz = Vector3f(0, 0, 1);
	}
	Normal(Vector3f vector){
		xyz = vector.normalized();
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
	Vector3f atTime(float time){
		if (time > t_max || time < t_min){  //simple test
			cout << "t is out of bounds" << endl;
		}
		return point + time * direction;
	}

};

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

class Light{
public:
	Color color;
	LightType l;
	Vector3f pos;
	Vector3f dir;
	Light(){
		color = Color();
		l = DIRECTIONALLIGHT;
	}
	Light(Color c, LightType lt, Vector3f vec){
		color = c;
		l = lt;
		if (l == POINTLIGHT)	pos = vec;
		else if (l == DIRECTIONALLIGHT) dir = -(vec).normalized();
	}
	void generateLightRay(LocalGeo local, Ray* lray, Color* lcolor){
		*lcolor = color;
		if (l == POINTLIGHT){
			*lray = Ray(local.position, (pos - local.position).normalized(), 0, (pos - local.position).squaredNorm());
		}
		else if (l == DIRECTIONALLIGHT) {
			*lray = Ray(local.position, dir, 0, FLT_MAX);
		}
	}
};

class BRDF{
	Color ka;
	Color kd;
	Color ks;
	Vector3f kr; //reflection coefficient

public:
	BRDF(){
		ka = Color(Vector3f(0, 0, 0));
		kd = Color(Vector3f(0, 0, 0));
		ks = Color(Vector3f(0, 0, 0));
		kr = Vector3f(0, 0, 0);
	}
	BRDF(Vector3f ambient, Vector3f diffuse, Vector3f specular, Vector3f reflection){
		ka = Color(ambient);
		kd = Color(diffuse);
		ks = Color(specular);
		kr = reflection;
	}
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
};

//class Material{
////	 Class for storing material. 
////	 For this example, it just returns a constant material regardless of what local is.
////	 Later on, when we want to support texture mapping, this need to be modified.
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



class Shape{
public:
	BRDF brdf;
	//please override these functions
	bool intersect(Ray& ray, float* thit, LocalGeo* local){
		return true;
	}
	bool intersectP(Ray& ray){
		return true;
	}

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
		if (intersectP(ray)){
			Vector3f d = ray.direction;
			Vector3f e = ray.point;
			Vector3f c = center;
			float x = sqr(d.dot(e - c)) - ((d.dot(d)) * ((e - c).dot(e - c) - sqr(radius)));
			float t = (-d.dot(e - c) - (sqrt(x))) / (d.dot(d));
			if (t >= ray.t_min && t < ray.t_max){
				*thit = t;
				Vector3f p = ray.atTime(t);
				*local = LocalGeo(p, Normal(p - c));
				return true;
			}
		}
		return false;
	}
	bool intersectP(Ray& ray) {
		Vector3f d = ray.direction;
		Vector3f e = ray.point;
		Vector3f c = center;
		float A = d.dot(d);
		float B = (2 * d).dot(e - c);
		float C = (e - c).dot(e - c) - sqr(radius);
		float disc = sqr(B) - 4 * A*C;
		return disc >= 0;
	}
};

class Triangle : public Shape{
public:
	Vector3f p1; Vector3f p2; Vector3f p3;
	Triangle(){
		p1 = Vector3f(0, 0, 0);
		p2 = Vector3f(0, 0, 0);
		p3 = Vector3f(0, 0, 0);
	}
	Triangle(Vector3f P1, Vector3f P2, Vector3f P3, BRDF color){
		p1 = P1;
		p2 = P2;
		p3 = P3;
		brdf = color;
	}
	bool intersectP(Ray& ray){
		float a = p1.x() - p2.x();
		float b = p1.y() - p2.y();
		float c = p1.z() - p2.z();
		float d = p1.x() - p3.x();
		float e = p1.y() - p3.y();
		float f = p1.z() - p3.z();
		float g = ray.direction.x();
		float h = ray.direction.y();
		float i = ray.direction.z();
		float j = p1.x() - ray.point.x();
		float k = p1.y() - ray.point.y();
		float l = p1.z() - ray.point.z();
		float M = a * (e * i - h * f) + b * (g * f - d * i) + c * (d * h - e * g);
		float t = -(f * (a * k - j * b) + e * (j * c - a * l) + d * (b * l - k * c)) / M;
		if (t < ray.t_min || t > ray.t_max){
			return false;
		}
		float gamma = (i * (a * k - j * b) + h * (j * c - a * l) + g * (b * l - k * c)) / M;
		if (gamma < 0 || gamma > 1){
			return false;
		}
		float beta = (j * (e * i - h * f) + k * (g * f - d * i) + l * (d * h - e * g)) / M;
		if (beta < 0 || beta > 1 - gamma){
			return false;
		}
		return true;
	}
	bool intersect(Ray& ray, float* thit, LocalGeo* local){
		float a = p1(0) - p2(0);
		float b = p1(1) - p2(1);
		float c = p1(2) - p2(2);
		float d = p1(0) - p3(0);
		float e = p1(1) - p3(1);
		float f = p1(2) - p3(2);
		float g = ray.direction(0);
		float h = ray.direction(1);
		float i = ray.direction(2);
		float j = p1(0) - ray.point(0);
		float k = p1(1) - ray.point(1);
		float l = p1(2) - ray.point(2);
		float M = a * (e * i - h * f) + b * (g * f - d * i) + c * (d * h - e * g);
		float t = (-1) * (f * (a * k - j * b) + e * (j * c - a * l) + d * (b * l - k * c)) / M;
		if (t < ray.t_min || t > ray.t_max){
			return false;
		}
		float gamma = (i * (a * k - j * b) + h * (j * c - a * l) + g * (b * l - k * c)) / M;
		if (gamma < 0 || gamma > 1){
			return false;
		}
		float beta = (j * (e * i - h * f) + k * (g * f - d * i) + l * (d * h - e * g)) / M;
		if (beta < 0 || beta >(1 - gamma)){
			return false;
		}
		*thit = t;
		Vector3f p = ray.atTime(t);
		Vector3f norm = (p2 - p1).cross(p3 - p1);
		*local = LocalGeo(p, Normal(norm));
		return true;
	}
};

//Primitive will transform the ray from world space to object space
//and hand it to Shape to do intersection in object space.
//Shape then computes the intersection and returns the result to Primitive.
//
//class Primitive{
//public:
//	//Abstract class for primitives in the scene
//	virtual bool intersect(Ray& ray, float* thit, Intersection* in) = 0;
//	virtual void getBRDF(LocalGeo& local, BRDF* brdf) = 0;
//	//Primitive will transform the ray from world space to object space
//	//and hand it to Shape to do intersection in object space.
//};
//
//class GeometricPrimitive : Primitive{
//public:
//	//Transformation objToWorld, worldToObj;
//	Shape* shape;
//	//Material* mat;
//	
//	bool intersect(Ray& ray, float* thit, Intersection* in)  {
//		Ray oray = worldToObj * ray;
//		LocalGeo olocal;
//		if (!shape->intersect(oray, thit, &olocal))  return false;
//		in->primitive = this;
//		in->localGeo = objToWorld * olocal;
//		return true;
//	}
//	void getBRDF(LocalGeo& local, BRDF* brdf) {
//		*brdf = shape.brdf;
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

//class Intersection{
//public:
//	LocalGeo localGeo;
//	Primitive* primitive;
//};

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
	int dx, dy; //dimension of our output
	int curr_x = 0, curr_y = 0;
	Sampler(){
		dx = 0;
		dy = 0;
	}
	Sampler(int x, int y){
		dx = x - 1;
		dy = y - 1;
	}
	//generates one sample per pixel
	bool generateSample(Sample* sample){
		*sample = Sample(curr_x, curr_y);
		if (curr_x >= dx) {
			if (curr_y >= dy){
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
		color.rgbRed = min(c.rgb[0], 1) * 255;
		color.rgbGreen = min(c.rgb[1], 1) * 255;
		color.rgbBlue = min(c.rgb[2], 1) * 255;
		FreeImage_SetPixelColor(bitmap, (int)sample.dx, (int)sample.dy, &color);

	}
	// Output image to a file
	void writeImage(){

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
	float dx, dy;
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
		if (u > 1 || v > 1){
			cout << "u or v is greater than 1" << endl;
			return;
		}
		Vector3f p = (1 - u) * ((1 - v) * LL + v * UL) + u * ((1 - v) * LR + v * UR);
		*ray = Ray(eye, p - eye, t_min, t_max);
	}

	//Notes:
	//	Create a ray starting from the camera that passes through the
	//		corresponding pixel(sample.x, sample.y) on the image plane.
	//		(from last week discussion, and also section 10.1 in Shirley’s book)
};

class RayTracer{
public:
	Triangle triangle;
	vector<Sphere> spheres;


	RayTracer(){

	}
	void addSphere(Sphere s){
		spheres.push_back(s);
	}
	void addTriangle(Triangle t){
		triangle = t;
	}
	void trace(Ray& ray, int depth, Color* color) {
		Color c = Color();
		float thit = 0;
		LocalGeo lg = LocalGeo();
		if (depth <= 0){
			*color = c; //Make the color black and return
			return;
		}
		for (unsigned int si = 0; si < spheres.size(); si++){
			if (spheres[si].intersect(ray, &thit, &lg)){
				BRDF b = spheres[si].brdf;
				Ray lightray;
				Color lightColor;
				c = c + b.ambient();
				for (unsigned int i = 0; i < pLights.size(); i++){	// loop through all light sources
					pLights[0].generateLightRay(lg, &lightray, &lightColor);
					//if (!sphere.intersectP(lightray)){
					c = c + b.diffuse(lg.normal.xyz, lightray.direction, lightColor);
					c = c + b.specular(lg.normal.xyz, lightray.direction, lightColor, specularCoefficient);
				}
				for (unsigned int i = 0; i < dLights.size(); i++){	// loop through all light sources
					dLights[i].generateLightRay(lg, &lightray, &lightColor);
					//if (!spheres[si].intersectP(lightray)){
					c = c + b.diffuse(lg.normal.xyz, lightray.direction, lightColor);
					c = c + b.specular(lg.normal.xyz, lightray.direction, lightColor, specularCoefficient);
					//}
				}
			}			
		}
		*color = c;

		//if (!triangle.intersect(ray, &thit, &lg)){
		//	*color = c;
		//	return;
		//}
		//*color = Color(Vector3f(1.0, 0, 0));


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
		camera = Camera(eye, ll, lr, ul, ur, x, y, 1.0, FLT_MAX);
		film = Film(dx, dy);
	}

	void addSphere(Sphere s){
		raytracer.addSphere(s);
	}
	void addLight(Light light){
		if (light.l == POINTLIGHT){
			pLights.push_back(light);
		}
		else if (light.l == DIRECTIONALLIGHT){
			dLights.push_back(light);
		}
	}

	void addTriangle(Triangle t){
		raytracer.addTriangle(t);
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


void spheretest_yellow_shading(){
	//-pl 200 200 200 0.6 0.6 0.6 - kd 1 1 0 - ka 0.1 0.1 0 - ks 0.8 0.8 0.8 - sp 16

	//camera and image plane
	Vector3f eye = Vector3f(0, 0, 2);
	Vector3f ul = Vector3f(-1, 1, 0);
	Vector3f ur = Vector3f(1, 1, 0);
	Vector3f lr = Vector3f(1, -1, 0);
	Vector3f ll = Vector3f(-1, -1, 0);

	//scene initializer
	Scene s = Scene(eye, ll, lr, ul, ur, 1000, 1000, 1);

	//lights
	s.addLight(Light(Color(Vector3f(0.6, 0.6, 0.6)), POINTLIGHT, Vector3f(200, 200, 200)));

	//spheres
	BRDF yellow = BRDF(Vector3f(0.1, 0.1, 0), Vector3f(1,1,0), Vector3f(0.8, 0.8, 0.8), Vector3f(0.0, 0.0, 0.0));
	Sphere sp = Sphere(Vector3f(0.0, 0.0, -2.0), 1.0, yellow);
	s.addSphere(sp);

	filename = "spheretest_yellow_shading.bmp";
	//render call
	s.render();
	pLights.clear();
	dLights.clear();
}
void spheretest_viewing_angle1(){
	//-pl 200 200 200 0.6 0.6 0.6 - kd 1 1 0 - ka 0.1 0.1 0 - ks 0.8 0.8 0.8 - sp 16

	//camera and image plane
	Vector3f eye = Vector3f(0, 0, 0);
	Vector3f ul = Vector3f(-1, 1, 0);
	Vector3f ur = Vector3f(1, 1, -2);
	Vector3f lr = Vector3f(1, -1, -2);
	Vector3f ll = Vector3f(-1, -1, 0);

	//scene initializer
	Scene s = Scene(eye, ll, lr, ul, ur, 1000, 1000, 1);

	//lights
	s.addLight(Light(Color(Vector3f(0.6, 0.6, 0.6)), POINTLIGHT, Vector3f(200, 200, 200)));

	//spheres
	BRDF yellow = BRDF(Vector3f(0.1, 0.1, 0), Vector3f(1, 1, 0), Vector3f(0.8, 0.8, 0.8), Vector3f(0.0, 0.0, 0.0));
	Sphere sp = Sphere(Vector3f(-1,1,-2), 1.0, yellow);
	s.addSphere(sp);

	filename = "spheretest_view1.bmp";
	//render call
	s.render();
	pLights.clear();
	dLights.clear();
}
void spheretest_viewing_angle2(){
	//-pl 200 200 200 0.6 0.6 0.6 - kd 1 1 0 - ka 0.1 0.1 0 - ks 0.8 0.8 0.8 - sp 16

	//camera and image plane
	Vector3f eye = Vector3f(0, 0, 2);
	Vector3f ul = Vector3f(-1, 1, 0);
	Vector3f ur = Vector3f(1, 1, 1);
	Vector3f lr = Vector3f(1, -1, 0);
	Vector3f ll = Vector3f(-1, -1, -1);

	//scene initializer
	Scene s = Scene(eye, ll, lr, ul, ur, 1000, 1000, 1);

	//lights
	s.addLight(Light(Color(Vector3f(0.6, 0.6, 0.6)), POINTLIGHT, Vector3f(200, 200, 200)));

	//spheres
	BRDF yellow = BRDF(Vector3f(0.1, 0.1, 0), Vector3f(1, 1, 0), Vector3f(0.8, 0.8, 0.8), Vector3f(0.0, 0.0, 0.0));
	Sphere sp = Sphere(Vector3f(0.0, 0.0, -2.0), 1.0, yellow);
	s.addSphere(sp);

	filename = "spheretest_view2.bmp";
	//render call
	s.render();
	pLights.clear();
	dLights.clear();
}
void spheretest_with_two_lights(){
	//"-pl 200 200 200 0.6 0.6 0.6 -dl 0 1 -1 0 0.4 0.4 -kd 1 1 0 -ka 0.1 0.1 0 -ks 0.8 0.8 0.8 -sp 16"

	//camera and image plane
	Vector3f eye = Vector3f(0, 0, 2);
	Vector3f ul = Vector3f(-1, 1, 0);
	Vector3f ur = Vector3f(1, 1, 0);
	Vector3f lr = Vector3f(1, -1, 0);
	Vector3f ll = Vector3f(-1, -1, 0);

	//scene initializer
	Scene s = Scene(eye, ll, lr, ul, ur, 1000, 1000, 1);

	//lights
	s.addLight(Light(Color(Vector3f(0.6, 0.6, 0.6)), POINTLIGHT, Vector3f(200, 200, 200)));
	s.addLight(Light(Color(Vector3f(0, 0.4, 0.4)), DIRECTIONALLIGHT, Vector3f(0, 1, -1)));

	//spheres
	BRDF yellow = BRDF(Vector3f(0.1, 0.1, 0), Vector3f(1, 1, 0), Vector3f(0.8, 0.8, 0.8), Vector3f(0.0, 0.0, 0.0));
	Sphere sp = Sphere(Vector3f(0.0, 0.0, -2.0), 1.0, yellow);
	s.addSphere(sp);

	filename = "spheretest_pt_dir.bmp";
	//render call
	s.render();
	pLights.clear();
	dLights.clear();
}
void spheretest_with_two_spheres(){
	//"-pl 200 200 200 0.6 0.6 0.6 -kd 1 1 0 -ka 0.1 0.1 0 -ks 0.8 0.8 0.8 -sp 16"

	//camera and image plane
	Vector3f eye = Vector3f(0, 0, 2);
	Vector3f ul = Vector3f(-1, 1, 0);
	Vector3f ur = Vector3f(1, 1, 0);
	Vector3f lr = Vector3f(1, -1, 0);
	Vector3f ll = Vector3f(-1, -1, 0);

	//scene initializer
	Scene s = Scene(eye, ll, lr, ul, ur, 1000, 1000, 1);

	//lights
	s.addLight(Light(Color(Vector3f(0.6, 0.6, 0.6)), POINTLIGHT, Vector3f(200, 200, 200)));

	//spheres
	BRDF blue = BRDF(Vector3f(0, 0.05, 0.1), Vector3f(0.11, 0.20, 0.54), Vector3f(0,1,1), Vector3f(0.0, 0.0, 0.0));
	Sphere sp = Sphere(Vector3f(-0.6, 0.0, -2.0), 1.0, blue);
	s.addSphere(sp);

	BRDF yellow = BRDF(Vector3f(0.1, 0.1, 0), Vector3f(1, 1, 0), Vector3f(0.8, 0.8, 0.8), Vector3f(0.0, 0.0, 0.0));
	sp = Sphere(Vector3f(1, 0.9, -2.4), 1.0, yellow);
	s.addSphere(sp);

	filename = "spheretest_two_spheres.bmp";
	//render call
	s.render();
	pLights.clear();
	dLights.clear();
}

int main(int argc, char *argv[]) {

	spheretest_yellow_shading();
	spheretest_with_two_lights();
	spheretest_with_two_spheres();
	spheretest_viewing_angle1();
	spheretest_viewing_angle2();

	return 0;
}








