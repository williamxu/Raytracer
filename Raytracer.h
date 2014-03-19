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

vector<Light> lights;
AggregatePrimitive primitives;

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
	int distribution = 1;
	int counter = 0;
	Sampler();
	Sampler(int x, int y);
	Sampler(int x, int y, int aliasing);
	bool generateSample(Sample* sample);
};

class Film {
public:
	FIBITMAP* bitmap;
	int samplesPerPixel = 1;
	int dx, dy;
	vector<Color> sum;
	Film();
	Film(int x, int y);
	Film(int x, int y, int spp);
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
	int recursionDepth = 0;
public:
	RayTracer(){}
	RayTracer(int depth);
	bool shadow(Ray ray);
	void trace(Ray ray, int depth, Color* color);
	Ray createReflectRay(LocalGeo lg, Ray ray);
};

class Scene{
public:
	int recursionDepth=0;
	int numSamples=1;
	Sampler sampler = Sampler();
	Camera camera = Camera();
	RayTracer raytracer = RayTracer();
	Film film = Film();

	Scene();
	Scene(Vector3f e, Vector3f ll, Vector3f lr, Vector3f ul, Vector3f ur, float x, float y, int depth);
	Scene(Vector3f e, Vector3f ll, Vector3f lr, Vector3f ul, Vector3f ur, float x, float y, int depth, int samples);
	void render(char* filename);
	void addPrimitives(vector<Primitive*> p);
	void addLight(Light l);
};