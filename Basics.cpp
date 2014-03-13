#include "Basics.h"

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
		if (time > t_max || time < t_min){
			cout << "t is out of bounds" << endl;
		}
		return point + time * direction;
	}
};

class LocalGeo{
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
			*lray = Ray(local.position, (pos - local.position).normalized(), .001, (pos - local.position).squaredNorm());
		}
		else if (l == DIRECTIONALLIGHT) {
			*lray = Ray(local.position, dir, .001, FLT_MAX);
		}
	}
};

class BRDF{
	Color ka;
	Color kd;
	Color ks;
	Vector3f kr; //reflection coefficient
	float sp;

public:
	BRDF(){
		ka = Color(Vector3f(0, 0, 0));
		kd = Color(Vector3f(0, 0, 0));
		ks = Color(Vector3f(0, 0, 0));
		kr = Vector3f(0, 0, 0);
		sp = 0.0;
	}
	BRDF(Vector3f ambient, Vector3f diffuse, Vector3f specular, Vector3f reflection, float spec){
		ka = Color(ambient);
		kd = Color(diffuse);
		ks = Color(specular);
		kr = reflection;
		sp = spec;
	}
	Color ambient(){
		return ka;
	}

	Color diffuse(Vector3f n, Vector3f l, Color lightColor) {
		return kd * lightColor * max(n.dot(l), 0);
	}

	Color specular(Vector3f n, Vector3f l, Color lightColor, float power) {
		Vector3f reflect = (-l + 2 * l.dot(n) * n).normalized();
		return ks * lightColor * pow(max(reflect.dot(Vector3f(0.0, 0.0, 1.0)), 0), power);
	}
	float specularCoefficient(){
		return sp;
	}
};
