#include "Primitive.h"

Intersection::Intersection(){
	localGeo = LocalGeo();
}
Intersection::Intersection(LocalGeo l, Primitive* p){
	localGeo = l;
	primitive = p;
}

GeometricPrimitive::GeometricPrimitive(Shape* s){
	shape = s;
	worldToObj = Transformation();
	objToWorld = Transformation();
}
GeometricPrimitive::GeometricPrimitive(Shape* s, Transformation otw, Transformation wto){
	shape = s;
	objToWorld = otw;
	worldToObj = wto;
}

bool GeometricPrimitive::intersect(Ray ray, float* thit, Intersection* in)  {
	//Ray oray = ray;
	Ray oray = worldToObj * ray; //the new ray is taking the inverse of objToWorld
	shape->transform(worldToObj);
	LocalGeo olocal = LocalGeo();
	if (!shape->intersect(oray, thit, &olocal)){
		shape->transform(objToWorld);
		return false;
	}
	shape->transform(objToWorld);
	in->primitive = this;
	in->localGeo = objToWorld * olocal;
	//in->localGeo = olocal;
	return true;
}

bool GeometricPrimitive::intersectP(Ray ray)  {
	//Ray oray = ray;
	Ray oray = worldToObj * ray;
	return shape->intersectP(oray);
}
void GeometricPrimitive::getBRDF(LocalGeo local, BRDF* brdf) {
	*brdf = shape->brdf;
}

AggregatePrimitive::AggregatePrimitive(vector<Primitive*> list){
	primitives = list;
}

bool AggregatePrimitive::intersect(Ray ray, float* thit, Intersection* in){
	bool hit = false;
	float nearestT = FLT_MAX;
	*thit = FLT_MAX;
	Intersection nearestI = Intersection();
	for (Primitive* p : primitives){
		if (p->intersect(ray, &nearestT, &nearestI)){
			hit = true;
			if (nearestT < *thit){
				*thit = nearestT;
				*in = nearestI;
			}
		}
	}
	return hit;
}

bool AggregatePrimitive::intersectP(Ray ray){
	for (Primitive* element : primitives){
		if (element->intersectP(ray)){
			return true;
		}
	}
	return false;
}

void AggregatePrimitive::getBRDF(LocalGeo local, BRDF* brdf) {
	cout << "Invalid call getBRDF of AggregatePrimitive" << endl;
	exit(1);
}
void AggregatePrimitive::addPrimitive(Primitive* p){
	primitives.push_back(p);
}