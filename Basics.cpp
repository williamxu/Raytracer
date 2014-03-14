#include "Basics.h"

Normal::Normal(){
	xyz = Vector3f(0, 0, 1);
}

Normal::Normal(Vector3f vector){
	xyz = vector.normalized();
}


Ray::Ray(){
	point = Vector3f(0, 0, 0);
	direction = Vector3f(0, 0, 0);
	t_min = 0;
	t_max = 0;
}
//	The ray ray(t) = point + t*direction, where t_min <= t <= t_max
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
	position = Vector3f(0, 0, 0);
	normal = Normal();
}

LocalGeo::LocalGeo(Vector3f p, Normal n){
	position = p;
	normal = n;
}

Color::Color(){
	rgb = Vector3f(0.0, 0.0, 0.0);

}
Color::Color(Vector3f values){
	rgb = values;

}
Color Color::operator+(Color c){
	return Color(Vector3f(rgb(0) + c.rgb(0), rgb(1) + c.rgb(1), rgb(2) + c.rgb(2)));

}
Color Color::operator*(Color c){
	return Color(Vector3f(rgb(0) * c.rgb(0), rgb(1) * c.rgb(1), rgb(2) * c.rgb(2)));

}
Color Color::operator*(float f){
	return Color(Vector3f(rgb(0) * f, rgb(1) * f, rgb(2) * f));

}

Light::Light(){
	color = Color();
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
	ka = Color(Vector3f(0, 0, 0));
	kd = Color(Vector3f(0, 0, 0));
	ks = Color(Vector3f(0, 0, 0));
	kr = Color(Vector3f(0, 0, 0));
	sp = 0.0;
}
BRDF::BRDF(Vector3f ambient, Vector3f diffuse, Vector3f specular, Vector3f reflection, float spec){
	ka = Color(ambient);
	kd = Color(diffuse);
	ks = Color(specular);
	kr = Color(reflection);
	sp = spec;
}
Color BRDF::ambient(){
	return ka;
}

Color BRDF::diffuse(Vector3f normal, Vector3f lightVector, Color lightColor) {
	return kd * lightColor * max(normal.dot(lightVector), 0);
}

Color BRDF::specular(Vector3f eye, Vector3f normal, Vector3f lightVector, Color lightColor, float power) {
	Vector3f reflect = (-lightVector + 2 * lightVector.dot(normal) * normal).normalized();
	return ks * lightColor * pow(max(eye.dot(reflect), 0), power);
}

float BRDF::specularCoefficient(){
	return sp;
}

Transformation::Transformation(){
	m << 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1;

	m_inv << 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1;
}

Transformation::Transformation(Matrix4f mat){
	Matrix4f inverse;
	bool invertible;
	mat.computeInverseWithCheck(inverse, invertible);
	if (invertible) {
		m_inv = inverse;
		m = mat;
	}
	else cout << "Creation of transformation failed: matrix is uninvertible" << endl;
}

Normal Transformation::operator* (Normal n){
	
}
Ray Transformation::operator* (Ray r){
}
LocalGeo Transformation::operator* (LocalGeo lg){
}
Vector3f Transformation::operator* (Vector3f v){
} //Vector or point?