#include "Shape.h"


Sphere::Sphere(){
	center = Vector3f(0, 0, 0);
	radius = 1.0;
}
Sphere::Sphere(Vector3f c, float r, BRDF color){
	brdf = color;
	center = c;
	radius = r;
}
void Sphere::transform(Transformation t){
	center = t * center;
}

bool Sphere::intersect(Ray& ray, float* thit, LocalGeo* local) {
	if (intersectP(ray)){
		Vector3f d = ray.direction;
		Vector3f e = ray.point;
		Vector3f c = center;
		float x = sqr(d.dot(e - c)) - ((d.dot(d)) * ((e - c).dot(e - c) - sqr(radius)));
		float t = (-d.dot(e - c) - (sqrt(x))) / (d.dot(d));
		float t2 = (-d.dot(e - c) + (sqrt(x))) / (d.dot(d));
		if (t >= ray.t_min && t < ray.t_max){
			*thit = t;
			Vector3f p = ray.atTime(t);
			*local = LocalGeo(p, Normal(p - c));
			return true;
		}
		else if (t2 >= ray.t_min && t2 < ray.t_max){
			*thit = t2;
			Vector3f p = ray.atTime(t2);
			*local = LocalGeo(p, Normal(p - c));
			return true;
		}
	}
	return false;
}
bool Sphere::intersectP(Ray& ray) {
	Vector3f d = ray.direction;
	Vector3f e = ray.point;
	Vector3f c = center;
	float A = d.dot(d);
	float B = (2 * d).dot(e - c);
	float C = (e - c).dot(e - c) - sqr(radius);
	float disc = sqr(B) - 4 * A*C;
	if (disc >= 0.0) {
		float x = sqr(d.dot(e - c)) - ((d.dot(d)) * ((e - c).dot(e - c) - sqr(radius)));
		float t1 = (-d.dot(e - c) - (sqrt(x))) / (d.dot(d));
		float t2 = (-d.dot(e - c) + (sqrt(x))) / (d.dot(d));
		return ((t1 >= ray.t_min && t1 < ray.t_max) || (t2 >= ray.t_min && t2 < ray.t_max));
	}
	return false;
}

Triangle::Triangle(){
	p1 = Vector3f(0, 0, 0);
	p2 = Vector3f(1, 0, 0);
	p3 = Vector3f(0, 1, 0);
}
Triangle::Triangle(Vector3f P1, Vector3f P2, Vector3f P3, BRDF color){
	p1 = P1;
	p2 = P2;
	p3 = P3;
	brdf = color;
}
void Triangle::transform(Transformation t){
	p1 = t * p1;
	p2 = t * p2;
	p3 = t * p3;
}
bool Triangle::intersectP(Ray& ray){
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
bool Triangle::intersect(Ray& ray, float* thit, LocalGeo* local){
	if (intersectP(ray)){
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

		*thit = t;
		Vector3f p = ray.atTime(t);
		Vector3f norm = (p2 - p1).cross(p3 - p1);
		*local = LocalGeo(p, Normal(norm));
		return true;
	}
	return false;
}
