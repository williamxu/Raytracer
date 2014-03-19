#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

#include <Dense>
#include <LU>

using namespace Eigen;
using namespace std;

#define PI 3.14159265

enum LightType { POINTLIGHT, DIRECTIONALLIGHT };

inline float sqr(float x) { return x*x; }

typedef Vector3f Normal;
typedef Vector3f Color;

class Ray{
public:
	Vector3f point;
	Vector3f direction;
	float t_min, t_max;
	Ray();
	Ray(Vector3f p, Vector3f d, float tmin, float tmax);
	Vector3f atTime(float time);
};

class LocalGeo{
public:
	Vector3f position;
	Normal normal;
	LocalGeo();
	LocalGeo(Vector3f p, Normal n);
};

class Light{
public:
	Color color;
	LightType l;
	Vector3f pos;
	Vector3f dir;
	Light();
	Light(Color c, LightType lt, Vector3f vec);
	void generateLightRay(LocalGeo local, Ray* lray, Color* lcolor);
};

class BRDF{
public:
	Color ka;
	Color kd;
	Color ks;
	Color kr;
	float sp;
	BRDF();
	BRDF(Vector3f ambient, Vector3f diffuse, Vector3f specular, Vector3f reflection, float spec);
	Color ambient();
	Color diffuse(Vector3f normal, Vector3f lightVector, Color lightColor);
	Color specular(Vector3f eye, Vector3f normal, Vector3f lightVector, Color lightColor, float power);
	float specularCoefficient();
};

//class Transformation{
//public:
//	aMatrix trans;
//	Transformation();
//	Transformation(aMatrix mat);
//	Transformation operator* (Transformation t);
//	Normal operator*(Normal n);
//	Ray operator*(Ray r);
//	LocalGeo operator*(LocalGeo lg);
//	Vector3f operator*(Vector3f v);
//	Vector3f mulDirection(Vector3f d);
//};