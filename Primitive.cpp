#include "Primitive.h"
#include "Shape.h"
#include "Basics.h"

class Primitive{
public:
	Shape* shape;
	virtual bool intersect(Ray ray, float* thit, Intersection* in) = 0;
	virtual bool intersectP(Ray ray) = 0;
	virtual void getBRDF(LocalGeo local, BRDF* brdf) = 0;
};

class Intersection{
public:
	LocalGeo localGeo;
	Primitive* primitive;
	Intersection(){
		localGeo = LocalGeo();
	}
	Intersection(LocalGeo l, Primitive* p){
		localGeo = l;
		primitive = p;
	}
};

class GeometricPrimitive : public Primitive{
public:
	//Transformation objToWorld, worldToObj;
	GeometricPrimitive(){	}
	GeometricPrimitive(Shape* s){
		shape = s;
	}

	bool intersect(Ray ray, float* thit, Intersection* in)  {
		Ray oray = ray;
		LocalGeo olocal = LocalGeo();
		if (!shape->intersect(oray, thit, &olocal))  return false;
		in->primitive = this;
		in->localGeo = olocal;
		return true;
	}

	bool intersectP(Ray ray)  {
		return shape->intersectP(ray);
	}
	void getBRDF(LocalGeo local, BRDF* brdf) {
		*brdf = shape->brdf;
	}
};

class AggregatePrimitive : public Primitive{

	vector<Primitive*> primitives;
public:
	AggregatePrimitive(){
		primitives = vector<Primitive*>();
	}

	AggregatePrimitive(vector<Primitive*> list){
		primitives = list;
	}

	bool intersect(Ray ray, float* thit, Intersection* in){
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

	bool intersectP(Ray ray){
		for (Primitive* element : primitives){
			if (element->intersectP(ray)){
				return true;
			}
		}
		return false;
	}

	void getBRDF(LocalGeo local, BRDF* brdf) {
		cout << "Invalid call getBRDF of AggregatePrimitive" << endl;
		exit(1);
	}
	void addPrimitive(Primitive* p){
		primitives.push_back(p);
	}
};
