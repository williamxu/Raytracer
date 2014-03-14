#ifndef SHAPE
#define SHAPE
#include "Shape.h"
#endif

#ifndef BASIC
#define BASIC
#include "Basics.h"
#endif

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
	GeometricPrimitive(){}
public:
	Transformation objToWorld, worldToObj;
	GeometricPrimitive(Shape* s);
	GeometricPrimitive(Shape* s, Transformation otw, Transformation wto);
	bool intersect(Ray ray, float* thit, Intersection* in);
	bool intersectP(Ray ray);
	void getBRDF(LocalGeo local, BRDF* brdf);
};

class AggregatePrimitive : public Primitive{

	vector<Primitive*> primitives;
public:
	AggregatePrimitive(){}
	AggregatePrimitive(vector<Primitive*> list);
	bool intersect(Ray ray, float* thit, Intersection* in);
	bool intersectP(Ray ray);
	void getBRDF(LocalGeo local, BRDF* brdf);

	void addPrimitive(Primitive* p);
};
