#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

#include <Dense>

using namespace Eigen;
using namespace std;

enum LightType { POINTLIGHT, DIRECTIONALLIGHT };

inline float sqr(float x) { return x*x; }

class Normal {
public:
	Vector3f xyz;
	Normal();
	Normal(Vector3f vector);
};

//Matrix
//Notes :
//Support creation of rotation, translation, scaling matrices
//May support matrix inversion if needed
//Also could support SVD, or other matrix decomposition, for future extension
class aMatrix {
public:
	float m[4][4];
	aMatrix();
	void createRotation(Vector3f rotationAxis, float angle);
	void createTranslation(float x, float y, float z);
	void createScale(float x, float y, float z);
};

class Transformation{
	aMatrix m;
	aMatrix m_inv;
public:
	Transformation();
	Transformation(aMatrix mat, aMatrix mat_inv);
	Normal operator* (Normal n);
	Ray operator* (Ray r);
	LocalGeo operator* (LocalGeo lg);
	Vector3f operator* (Vector3f v); //Vector or point?
};

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

class Color {
public:
	Vector3f rgb;
	Color();
	Color(Vector3f values);
	Color operator+(Color c);
	Color operator*(Color c);
	Color operator*(float f);
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
	Color kr; //reflection coefficient
	float sp;
	BRDF();
	BRDF(Vector3f ambient, Vector3f diffuse, Vector3f specular, Vector3f reflection, float spec);
	Color ambient();
	Color diffuse(Vector3f normal, Vector3f lightVector, Color lightColor);
	Color specular(Vector3f eye, Vector3f normal, Vector3f lightVector, Color lightColor, float power);
	float specularCoefficient();
};
