#include "Basics.h"

Ray::Ray(){
	point = Vector3f();
	direction = Vector3f();
	t_min = 0;
	t_max = 0;
}

Ray::Ray(Vector3f p, Vector3f d, float tmin, float tmax){
	point = p;
	direction = d;
	t_min = tmin;
	t_max = tmax;
}

Vector3f Ray::atTime(float time){
	if (time > t_max || time < t_min){
		cout << "t is out of bounds" << endl;
	}
	return point + time * direction;
}

LocalGeo::LocalGeo(){
	position = Vector3f();
	normal = Vector3f(0, 0, 1);
}

LocalGeo::LocalGeo(Vector3f p, Normal n){
	position = p;
	normal = n.normalized();
}

Light::Light(){
	color = Color(0, 0, 0);
	l = DIRECTIONALLIGHT;
}
Light::Light(Color c, LightType lt, Vector3f vec){
	color = c;
	l = lt;
	if (l == POINTLIGHT)	pos = vec;
	else if (l == DIRECTIONALLIGHT) dir = -(vec).normalized();
}
void Light::generateLightRay(LocalGeo local, Ray* lray, Color* lcolor){
	*lcolor = color;
	if (l == POINTLIGHT){
		*lray = Ray(local.position, (pos - local.position).normalized(), .001, (pos - local.position).squaredNorm());
	}
	else if (l == DIRECTIONALLIGHT) {
		*lray = Ray(local.position, dir, .001, FLT_MAX);
	}
}

BRDF::BRDF(){
	ka = Color(0, 0, 0);
	kd = Color(0, 0, 0);
	ks = Color(0, 0, 0);
	kr = Color(0, 0, 0);
	sp = 0.0;
}
BRDF::BRDF(Color ambient, Color diffuse, Color specular, Color reflection, float spec){
	ka = ambient;
	kd = diffuse;
	ks = specular;
	kr = reflection;
	sp = spec;
}

Color BRDF::ambient(){
	return ka;
}

Color BRDF::diffuse(Normal normal, Normal light, Color lightColor) {
	return kd.cwiseProduct(lightColor) * max(normal.dot(light), 0);
}

Color BRDF::specular(Vector3f eye, Normal normal, Normal light, Color lightColor, float power) {
	Vector3f reflect = (-light + 2 * light.dot(normal) * normal).normalized();
	return ks.cwiseProduct(lightColor) * pow(max(eye.dot(reflect), 0), power);
}

float BRDF::specularCoefficient(){
	return sp;
}

Transformation Transformation::axisAngleRotation(Vector3f axis, float angle){
	Transform<float, 3, Affine> transform(AngleAxisf(angle, axis));
	cout << transform.matrix();
	Matrix4f m = transform.matrix();
	return Transformation(m);
}

//Transformation Transformation::translation(Vector3f xyz){
//	Translation<float,3> translate;
//	translate = Translation<float, 3>(xyz);
//	Matrix4f m = trans
//	return Transformation(m);
//}
//Transformation Transformation::scale(Vector3f xyz){
//}
	//Transformation operator* (Transformation t);
	//Normal operator*(Normal n);
	//Ray operator*(Ray r);
	//LocalGeo operator*(LocalGeo lg);
	//Vector3f operator*(Vector3f v);
	//Vector3f mulDirection(Vector3f d);