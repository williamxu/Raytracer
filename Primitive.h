#pragma once

#include "Shape.h"
#include "Basics.h"

class Intersection;

class Primitive{
public:
	Shape *shape;
	virtual bool intersect(Ray ray, float* thit, Intersection* in) = 0;
	virtual bool intersectP(Ray ray) = 0;
	virtual void getBRDF(LocalGeo local, BRDF* brdf) = 0;
};

class Intersection{
public:
	LocalGeo localGeo;
	Primitive* primitive;
	Intersection();
	Intersection(LocalGeo l, Primitive* p);
};

class GeometricPrimitive : public Primitive{
public:
	//Transformation objToWorld, worldToObj;
	GeometricPrimitive(){}
	GeometricPrimitive(Shape* s);
	//GeometricPrimitive(Shape* s, Transformation otw, Transformation wto);
	bool intersect(Ray ray, float* thit, Intersection* in);
	bool intersectP(Ray ray);
	void getBRDF(LocalGeo local, BRDF* brdf);
};

class AggregatePrimitive : public Primitive{

	vector<Primitive*> primitives;
public:
	AggregatePrimitive(){
		primitives = vector<Primitive*>();
	}
	AggregatePrimitive(vector<Primitive*> p){ primitives = p; }
	bool intersect(Ray ray, float* thit, Intersection* in);
	bool intersectP(Ray ray);
	void getBRDF(LocalGeo local, BRDF* brdf);
	void addPrimitive(Primitive* p);
};
