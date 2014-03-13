#include "Raytracer.h"

class Sample{
public:
	float dx, dy; // Store screen coordinate
	Sample(){
		dx = 0;
		dy = 0;
	}

	Sample(float x, float y){
		dx = x;
		dy = y;
	}

};

class Sampler{
public:
	int dx, dy; //dimension of our output
	int curr_x = 0, curr_y = 0;
	Sampler(){
		dx = 0;
		dy = 0;
	}
	Sampler(int x, int y){
		dx = x - 1;
		dy = y - 1;
	}
	//generates one sample per pixel
	bool generateSample(Sample* sample){
		*sample = Sample(curr_x, curr_y);
		if (curr_x >= dx) {
			if (curr_y >= dy){
				return false;
			}
			curr_y += 1;
			curr_x = 0;
		}
		else{
			curr_x += 1;
		}
		return true;
	}
};

class Film {

public:
	FIBITMAP* bitmap;
	int dx, dy;
	Film(){
		dx = 0;
		dy = 0;
	}
	Film(int x, int y){
		FreeImage_Initialise();
		dx = x;
		dy = y;
		bitmap = FreeImage_Allocate(dx, dy, 24); //24?
	}
	// Will write the color to (sample.x, sample.y) on the image
	void commit(Sample& sample, Color& c){
		RGBQUAD color;
		color.rgbRed = min(c.rgb[0], 1) * 255;
		color.rgbGreen = min(c.rgb[1], 1) * 255;
		color.rgbBlue = min(c.rgb[2], 1) * 255;
		FreeImage_SetPixelColor(bitmap, (int)sample.dx, (int)sample.dy, &color);

	}
	// Output image to a file
	void writeImage(char* filename){

		int index = string(filename).find_last_of('.') + 1;
		if (FreeImage_Save(FreeImage_GetFIFFromFormat(filename + index), bitmap, filename)) {
			cout << "Image saved successfully." << endl;
		}
		FreeImage_DeInitialise();
	}
};

class Camera{
public:
	Vector3f eye;
	Vector3f LL;
	Vector3f LR;
	Vector3f UL;
	Vector3f UR;
	float dx, dy;
	float t_min;
	float t_max;
	Camera(){
		eye = Vector3f(0, 0, 1);
		LL = Vector3f(-1, -1, 0);
		LR = Vector3f(1, -1, 0);
		UL = Vector3f(-1, 1, 0);
		UR = Vector3f(1, 1, 0);
		dx = 0; dy = 0;
		t_min = 1;
		t_max = FLT_MAX;
	}
	Camera(Vector3f p, Vector3f ll, Vector3f lr, Vector3f ul, Vector3f ur, float x, float y, float tmin, float tmax){
		LL = ll;
		LR = lr;
		UL = ul;
		UR = ur;
		eye = p;
		dx = x;
		dy = y;
		t_min = tmin;
		t_max = tmax;
	}
	void generateRay(Sample& sample, Ray* ray){
		float u = (sample.dx + 0.5) / dx;
		float v = (sample.dy + 0.5) / dy;
		if (u > 1 || v > 1){
			cout << "u or v is greater than 1" << endl;
			return;
		}
		Vector3f p = (1 - u) * ((1 - v) * LL + v * UL) + u * ((1 - v) * LR + v * UR);
		*ray = Ray(eye, p - eye, t_min, t_max);
	}
};

class RayTracer{
	vector<Light> lights;
	AggregatePrimitive primitives;
public:
	RayTracer(){
		primitives = AggregatePrimitive();
		lights = vector<Light>();
	}

	RayTracer(vector<Light> l, vector<Primitive*> p){
		primitives = AggregatePrimitive(p);
		lights = l;
	}
	void addLight(Light l){
		lights.push_back(l);
	}
	void addPrimitive(Primitive* p){
		primitives.addPrimitive(p);
	}

	boolean shadow(Ray ray){
		return primitives.intersectP(ray);
		//for (Shape* s : shapes){
		//	if (s->intersectP(ray)){
		//		return true;
		//	}
		//}
		//return false;
	}
	void trace(Ray& ray, int depth, Color* color) {
		Color c = Color();
		float thit = 0;
		Intersection in = Intersection();

		if (depth <= 0){
			*color = c; //Make the color black and return
			return;
		}
		if (primitives.intersect(ray, &thit, &in)){
			BRDF b;
			in.primitive->getBRDF(in.localGeo, &b);
			Ray lightray;
			Color lightColor;
			c = c + b.ambient();
			for (Light l : lights){
				l.generateLightRay(in.localGeo, &lightray, &lightColor);
				if (!shadow(lightray)){
					c = c + b.diffuse(in.localGeo.normal.xyz, lightray.direction, lightColor);
					c = c + b.specular(in.localGeo.normal.xyz, lightray.direction, lightColor, b.specularCoefficient());
				}
			}
		}

		*color = c;
	}

	// Handle mirror reflection
	//if (brdf.kr > 0) {
	//reflectRay = createReflectRay(in.local, ray);

	// Make a recursive call to trace the reflected ray
	//trace(reflectRay, depth + 1, &tempColor);
	//*color += brdf.kr * tempColor;
	//}

	//		Beware when you generate reflection ray, make sure the ray don’t start
	//		exactly on the surface, or the intersection routine may return
	//		intersection point at the starting point of the ray. (This apply to light
	//		ray generation as well)

};

class Scene{
public:
	Vector3f eye; //the camera. can get position
	Vector3f LL, LR, UL, UR; // the four corners of the image plane
	float dx, dy; //the output image dimensions in pixels
	int recursionDepth;
	Sampler sampler = Sampler();
	Camera camera = Camera();
	RayTracer raytracer = RayTracer();
	Film film = Film();

	Scene(){
		eye = Vector3f(0, 0, 0);
		LL = Vector3f(0, 0, 0);
		LR = Vector3f(0, 0, 0);
		UL = Vector3f(0, 0, 0);
		UR = Vector3f(0, 0, 0);
		dx = 0;
		dy = 0;
		recursionDepth = 0;
		sampler = Sampler();
		camera = Camera();
		film = Film();
	}
	//shapes list
	//necessary objects
	Scene(Vector3f e, Vector3f ll, Vector3f lr, Vector3f ul, Vector3f ur, float x, float y, int depth){
		eye = e;
		LL = ll;
		LR = lr;
		UL = ul;
		UR = ur;
		dx = x;
		dy = y;
		recursionDepth = depth;
		sampler = Sampler(dx, dy);
		camera = Camera(eye, ll, lr, ul, ur, x, y, 1.0, FLT_MAX);
		film = Film(dx, dy);
	}


	void addLight(Light light){
		raytracer.addLight(light);
	}
	void addPrimitive(Primitive* p){
		raytracer.addPrimitive(p);
	}

	void render() {
		Sample sample = Sample();
		Ray ray = Ray();
		Color color = Color();
		while (sampler.generateSample(&sample)) {
			camera.generateRay(sample, &ray);
			raytracer.trace(ray, recursionDepth, &color);
			film.commit(sample, color);
		}
		char* filename = "output.png";
		film.writeImage(filename);
	}
};