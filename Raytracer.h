#ifndef BASIC
#define BASIC
#include "Basics.h"
#endif

#ifndef SHAPE
#define SHAPE
#include "Shape.h"
#endif

#ifndef PRIMITIVE
#define PRIMITIVE
#include "Primitive.h"
#endif

#ifdef _WIN32
#include <windows.h>
#endif

#include "FreeImage/FreeImage.h"

using namespace Eigen;
//global

class Sample{
public:
	float dx, dy;
	Sample();
	Sample(float x, float y);

};

class Sampler{
public:
	int dx, dy; //dimension of our output
	int curr_x = 0, curr_y = 0;
	Sampler();
	Sampler(int x, int y);
	bool generateSample(Sample* sample);
};

class Film {
public:
	FIBITMAP* bitmap;
	int dx, dy;
	Film();
	Film(int x, int y);
	void commit(Sample& sample, Color& c);
	void writeImage(char* filename);
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
	Camera();
	Camera(Vector3f p, Vector3f ll, Vector3f lr, Vector3f ul, Vector3f ur, float x, float y, float tmin, float tmax);
	void generateRay(Sample& sample, Ray* ray);
};

class RayTracer{
	vector<Light> lights;
	AggregatePrimitive primitives;
	int recursionDepth;
public:
	RayTracer();
	RayTracer(vector<Light> l, vector<Primitive*> p, int depth);
	void addLight(Light l);
	void addPrimitive(Primitive* p);
	bool shadow(Ray ray);
	void trace(Ray ray, int depth, Color* color);
	Ray createReflectRay(LocalGeo lg, Ray ray);
};

class Scene{
public:
	Vector3f eye;
	Vector3f LL, LR, UL, UR;
	float dx, dy;
	int recursionDepth;
	Sampler sampler = Sampler();
	Camera camera = Camera();
	RayTracer raytracer = RayTracer();
	Film film = Film();

	Scene();
	Scene(Vector3f e, Vector3f ll, Vector3f lr, Vector3f ul, Vector3f ur, float x, float y, int depth);
	void addLight(Light light);
	void addPrimitive(Primitive* p);
	void render(char* filename);
};