class Shape{
public:
	BRDF brdf;
	virtual bool intersect(Ray& ray, float* thit, LocalGeo* local);
	virtual bool intersectP(Ray& ray);
};

class Sphere : public Shape{
public:
	Vector3f center;
	float radius;
	Sphere();
	Sphere(Vector3f c, float r, BRDF color);
	bool intersect(Ray& ray, float* thit, LocalGeo* local);
	bool intersectP(Ray& ray);
};

class Triangle : public Shape{
public:
	Vector3f p1;
	Vector3f p2;
	Vector3f p3;
	Triangle();
	Triangle(Vector3f P1, Vector3f P2, Vector3f P3, BRDF color);
	bool intersectP(Ray& ray);
	bool intersect(Ray& ray, float* thit, LocalGeo* local);
};
