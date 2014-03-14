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

aMatrix::aMatrix(){
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
			if (i == j) m[i][j] = 1;
			else m[i][j] = 0;
		}
	}
}
aMatrix aMatrix::operator*(aMatrix other){
	aMatrix result;
	for (int x = 0; x < 4; x++){
		for (int y = 0; y < 4; y++){
			result.m[x][y] = 0;
		}
	}
	for (int x = 0; x < 4; x++){
		for (int y = 0; y < 4; y++){
			for (int z = 0; z < 4; z++){
				result.m[x][y] += m[z][y] * other.m[x][z];
			}

		}
	}
	return result;
}

void aMatrix::createEulerRotation(float x, float y, float z){
	aMatrix Rx;
	aMatrix Ry;
	aMatrix Rz;
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
			if (i == j){
				Rx.m[i][j] = 1;
				Ry.m[i][j] = 1;
				Rz.m[i][j] = 1;
			}
			else{
				Rx.m[i][j] = 0;
				Ry.m[i][j] = 0;
				Rz.m[i][j] = 0;
			}
		}
	}
	Rx.m[1][1] = cos(x);
	Rx.m[1][2] = sin(x);
	Rx.m[2][1] = -sin(x);
	Rx.m[2][2] = cos(x);

	Ry.m[0][0] = cos(y);
	Ry.m[0][2] = -sin(y);
	Ry.m[2][0] = sin(y);
	Ry.m[2][2] = cos(y);

	Rz.m[0][0] = cos(z);
	Rz.m[0][1] = sin(z);
	Rz.m[1][0] = -sin(z);
	Rz.m[1][1] = cos(z);
	
	aMatrix t = Rz * Ry * Rx;
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
			m[i][j] = t.m[i][j];
		}
	}
}

void aMatrix::createQuaternionRotation(Vector3f quaternion, float z){ //quaternion representation
	float w = quaternion.x();
	float x = quaternion.y();
	float y = quaternion.z();
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
			if (i == j) m[i][j] = 1;
			else m[i][j] = 0;
		}
	}
	float xsqr = 2 * sqr(x);
	float ysqr = 2 * sqr(y);
	float zsqr = 2 * sqr(z);

	m[0][0] = 1 - ysqr - zsqr;
	m[0][1] = (2 * x * y) + (2 * z * w);
	m[0][2] = (2 * x * z) - (2 * y * w);

	m[1][0] = (2 * x * y) - (2 * z * w);
	m[1][1] = 1 - xsqr - zsqr;
	m[1][2] = 2 * y * z + 2 * x * w;

	m[2][0] = 2 * x * z + 2 * y * w;
	m[2][1] = 2 * y * z - 2 * x * w;
	m[2][2] = 1 - xsqr - ysqr;

}
void aMatrix::createTranslation(float x, float y, float z){
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
			if (i == j) m[i][j] = 1;
			else m[i][j] = 0;
		}
	}
	m[3][0] = x;
	m[3][1] = y;
	m[3][2] = z;
	m[3][3] = 1;

}

void aMatrix::createScale(float x, float y, float z){
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
			m[i][j] = 0;
		}
	}
	m[0][0] = x;
	m[1][1] = y;
	m[2][2] = z;
	m[3][3] = 1;
}

Transformation::Transformation(){
}
Transformation::Transformation(aMatrix mat){
	trans = mat;
}

Transformation Transformation::operator* (Transformation t){ //this multiply transformation t
	return Transformation(this->trans * t.trans);
}

Normal Transformation::operator*(Normal n){
	float a[16];
	int z = 0;
	for (int x = 0; x < 4; x++){
		for (int y = 0; y < 4; y++){
			a[z] = this->trans.m[x][y];
			z++;
		}
	}
	Vector3f b = Vector3f(a[5] * a[10] - a[9] * a[6], a[9] * a[2] - a[1] * a[10], a[1] * a[6] - a[5] * a[2]);
	Vector3f c = Vector3f(a[8] * a[6] - a[4] * a[10], a[0] * a[10] - a[8] * a[2], a[4] * a[2] - a[0] * a[6]);
	Vector3f d = Vector3f(a[4] * a[9] - a[8] * a[5], a[8] * a[1] - a[0] * a[9], a[0] * a[5] - a[4] * a[1]);
	float i = b.x() * n.xyz.x() + c.x() * n.xyz.x() + d.x() * n.xyz.x();
	float j = b.y() * n.xyz.y() + c.y() * n.xyz.y() + d.y() * n.xyz.y();
	float k = b.z() * n.xyz.z() + c.z() * n.xyz.z() + d.z() * n.xyz.z();
	Normal result = Normal(Vector3f(i, j, k));
	return result;
}
Ray Transformation::operator* (Ray r){
	Vector3f result1 = *this * r.point;
	Vector3f result2 = *this * r.direction;
	Ray result = Ray(result1, result2, r.t_min, r.t_max);
	return result;
}
LocalGeo Transformation::operator* (LocalGeo lg){
	Vector3f result1 = *this * lg.position;
	Normal result2 = *this * lg.normal;
	LocalGeo result = LocalGeo(result1, result2);
	return result;
}

Vector3f Transformation::operator* (Vector3f v){
	float x = trans.m[0][0] * v.x() + trans.m[1][0] * v.y() + trans.m[2][0] * v.z() + trans.m[3][0];
	float y = trans.m[0][1] * v.x() + trans.m[1][1] * v.y() + trans.m[2][1] * v.z() + trans.m[3][1];
	float z = trans.m[0][2] * v.x() + trans.m[1][2] * v.y() + trans.m[2][2] * v.z() + trans.m[3][2];
	float w = trans.m[0][3] * v.x() + trans.m[1][3] * v.y() + trans.m[2][3] * v.z() + trans.m[3][3];
	Vector3f result = Vector3f(x / w, y / w, z / w);
	return result;
}