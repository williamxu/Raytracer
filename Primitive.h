class Primitive{
public:
	Shape* shape;
	virtual bool intersect(Ray ray, float* thit, Intersection* in);
	virtual bool intersectP(Ray ray);
	virtual void getBRDF(LocalGeo local, BRDF* brdf);
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
	GeometricPrimitive();
	GeometricPrimitive(Shape* s);
	bool intersect(Ray ray, float* thit, Intersection* in);
	bool intersectP(Ray ray);
	void getBRDF(LocalGeo local, BRDF* brdf);
};

class AggregatePrimitive : public Primitive{

	vector<Primitive*> primitives;
public:
	AggregatePrimitive();
	AggregatePrimitive(vector<Primitive*> list);
	bool intersect(Ray ray, float* thit, Intersection* in);
	bool intersectP(Ray ray);
	void getBRDF(LocalGeo local, BRDF* brdf);
	
	void addPrimitive(Primitive* p);
};
