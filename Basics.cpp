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
	float m[4][4] = {{ 1, 0, 0, 0 }, 
					{ 0, 1, 0, 0 }, 
					{ 0, 0, 1, 0 }, 
					{ 0, 0, 0, 1 }};
}

void aMatrix::createRotation(Vector3f rotationAxis, float angle){ //quaternion representation
	//todo
}
void aMatrix::createTranslation(float x, float y, float z){
	m[0][3] = x;
	m[1][3] = y;
	m[2][3] = z;
	m[3][3] = 1;
}

void aMatrix::createScale(float x, float y, float z){
	m[0][0] = x;
	m[1][1] = y;
	m[2][2] = z;
	m[3][3] = 1;
}

Transformation::Transformation(){
	trans = aMatrix();
	trans_inv = aMatrix();
}
Transformation::Transformation(aMatrix mat, aMatrix matInv){
	trans = mat;
	trans_inv = matInv;
}

Transformation Transformation::operator* (Transformation t){
	aMatrix result; 
	for (int x = 0; x < 4; x++){
		for (int y = 0; y < 4; y++){
			result.m[x][y] = trans.m[x][0] * t.trans.m[0][y] + trans.m[x][1] * t.trans.m[1][y] + trans.m[x][2] * t.trans.m[2][y] + trans.m[x][3] * t.trans.m[3][y];
		}
	}
	aMatrix resultInv;
	Matrix4f resultM = Matrix4f();
	for (int x = 0; x < 4; x++){
		for (int y = 0; y < 4; y++){
			resultM(x, y) = result.m[x][y];
		}
	}
	Matrix4f resultInvM = resultM.inverse();
	for (int x = 0; x < 4; x++){
		for (int y = 0; y < 4; y++){
			resultInv.m[x][y] = resultInvM(x, y);
		}
	}
	return Transformation(result, resultInv);
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
	float x = trans.m[0][0] * v.x() + trans.m[0][1] * v.x() + trans.m[0][2] * v.x() + trans.m[0][3] * v.x();
	float y = trans.m[1][0] * v.y() + trans.m[1][1] * v.y() + trans.m[1][2] * v.y() + trans.m[1][3] * v.y();
	float z = trans.m[2][0] * v.z() + trans.m[2][1] * v.z() + trans.m[2][2] * v.z() + trans.m[2][3] * v.z();
	float w = trans.m[3][0] + trans.m[3][1] + trans.m[3][2] + trans.m[3][3];
	Vector3f result = Vector3f(x/w, y/w, z/w);
	return result;
} 