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

//void aMatrix::createEulerRotation(float x, float y, float z){
//	aMatrix Rx;
//	aMatrix Ry;
//	aMatrix Rz;
//	for (int i = 0; i < 4; i++){
//		for (int j = 0; j < 4; j++){
//			if (i == j){
//				Rx.m[i][j] = 1;
//				Ry.m[i][j] = 1;
//				Rz.m[i][j] = 1;
//			}
//			else{
//				Rx.m[i][j] = 0;
//				Ry.m[i][j] = 0;
//				Rz.m[i][j] = 0;
//			}
//		}
//	}
//	x = x*PI / 180;
//	y = y*PI / 180;
//	z = z*PI / 180;
//	Rx.m[1][1] = cos(x);
//	Rx.m[1][2] = -sin(x);
//	Rx.m[2][1] = sin(x);
//	Rx.m[2][2] = cos(x);
//
//	Ry.m[0][0] = cos(y);
//	Ry.m[0][2] = sin(y);
//	Ry.m[2][0] = -sin(y);
//	Ry.m[2][2] = cos(y);
//
//	Rz.m[0][0] = cos(z);
//	Rz.m[0][1] = -sin(z);
//	Rz.m[1][0] = sin(z);
//	Rz.m[1][1] = cos(z);
//	
//	aMatrix t = Rx * Ry;
//	t = t * Rz;
//	for (int i = 0; i < 4; i++){
//		for (int j = 0; j < 4; j++){
//			m[i][j] = t.m[i][j];
//		}
//	}
//}
//
////void aMatrix::createQuaternionRotation(Vector3f quaternion, float z){ //quaternion representation
////	float w = quaternion.x();
////	float x = quaternion.y();
////	float y = quaternion.z();
////	for (int i = 0; i < 4; i++){
////		for (int j = 0; j < 4; j++){
////			if (i == j) m[i][j] = 1;
////			else m[i][j] = 0;
////		}
////	}
////	float xsqr = 2 * sqr(x);
////	float ysqr = 2 * sqr(y);
////	float zsqr = 2 * sqr(z);
////
////	m[0][0] = 1 - ysqr - zsqr;
////	m[0][1] = (2 * x * y) + (2 * z * w);
////	m[0][2] = (2 * x * z) - (2 * y * w);
////
////	m[1][0] = (2 * x * y) - (2 * z * w);
////	m[1][1] = 1 - xsqr - zsqr;
////	m[1][2] = 2 * y * z + 2 * x * w;
////
////	m[2][0] = 2 * x * z + 2 * y * w;
////	m[2][1] = 2 * y * z - 2 * x * w;
////	m[2][2] = 1 - xsqr - ysqr;
////
////}
//
//void aMatrix::createTranslation(float x, float y, float z){
//	for (int i = 0; i < 4; i++){
//		for (int j = 0; j < 4; j++){
//			if (i == j) m[i][j] = 1;
//			else m[i][j] = 0;
//		}
//	}
//	m[3][0] = x;
//	m[3][1] = y;
//	m[3][2] = z;
//	m[3][3] = 1;
//
//}
//
//void aMatrix::createScale(float x, float y, float z){
//	for (int i = 0; i < 4; i++){
//		for (int j = 0; j < 4; j++){
//			m[i][j] = 0;
//		}
//	}
//	m[0][0] = x;
//	m[1][1] = y;
//	m[2][2] = z;
//	m[3][3] = 1;
//}
//
//Transformation::Transformation(){
//}
//Transformation::Transformation(aMatrix mat){
//	trans = mat;
//}
//
//Transformation Transformation::operator* (Transformation t){ //this multiply transformation t
//	return Transformation(this->trans * t.trans);
//}
//
//Normal Transformation::operator*(Normal n){
//	aMatrix nMat = aMatrix();
//	aMatrix a = this->trans;
//	nMat.m[0][0] = a.m[1][1] * a.m[2][2] - a.m[1][2] * a.m[2][1];
//	nMat.m[0][1] = a.m[0][2] * a.m[2][1] - a.m[0][1] * a.m[2][2];
//	nMat.m[0][2] = a.m[0][1] * a.m[1][2] - a.m[0][2] * a.m[1][1];
//
//	nMat.m[1][0] = a.m[1][2] * a.m[2][0] - a.m[1][0] * a.m[2][2];
//	nMat.m[1][1] = a.m[0][0] * a.m[2][2] - a.m[0][2] * a.m[2][0];
//	nMat.m[1][2] = a.m[0][2] * a.m[1][0] - a.m[0][0] * a.m[1][2];
//
//	nMat.m[2][0] = a.m[1][0] * a.m[2][1] - a.m[1][1] * a.m[2][0];
//	nMat.m[2][1] = a.m[0][1] * a.m[2][0] - a.m[0][0] * a.m[2][1];
//	nMat.m[2][2] = a.m[0][0] * a.m[1][1] - a.m[0][1] * a.m[1][0];
//
//	Vector3f result = Transformation(nMat).mulDirection(n.xyz);
//	return Normal(result);
//}
//
//Ray Transformation::operator* (Ray r){
//	Vector3f newOrigin = *this * r.point;
//	Vector3f newDirection = this->mulDirection(r.direction);
//
//	Ray result = Ray(newOrigin, newDirection, r.t_min, r.t_max);
//	return result;
//}
//LocalGeo Transformation::operator* (LocalGeo lg){
//	Vector3f result1 = *this * lg.position;
//	Normal result2 = *this * lg.normal;
//	LocalGeo result = LocalGeo(result1, result2);
//	return result;
//}
//Vector3f Transformation::mulDirection(Vector3f v){
//	float x = trans.m[0][0] * v.x() + trans.m[1][0] * v.y() + trans.m[2][0] * v.z();
//	float y = trans.m[0][1] * v.x() + trans.m[1][1] * v.y() + trans.m[2][1] * v.z();
//	float z = trans.m[0][2] * v.x() + trans.m[1][2] * v.y() + trans.m[2][2] * v.z();
//	Vector3f result = Vector3f(x, y, z);
//	return result;
//}
//
//Vector3f Transformation::operator* (Vector3f v){
//	float x = trans.m[0][0] * v.x() + trans.m[1][0] * v.y() + trans.m[2][0] * v.z() + trans.m[3][0];
//	float y = trans.m[0][1] * v.x() + trans.m[1][1] * v.y() + trans.m[2][1] * v.z() + trans.m[3][1];
//	float z = trans.m[0][2] * v.x() + trans.m[1][2] * v.y() + trans.m[2][2] * v.z() + trans.m[3][2];
//	float w = trans.m[0][3] * v.x() + trans.m[1][3] * v.y() + trans.m[2][3] * v.z() + trans.m[3][3];
//	Vector3f result = Vector3f(x / w, y / w, z / w);
//	return result;
//}