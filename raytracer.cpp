#include "Raytracer.h"

char* filename;
vector<Primitive*> pr = vector<Primitive*>();

Sample::Sample(){
	dx = 0;
	dy = 0;
}

Sample::Sample(float x, float y){
	dx = x;
	dy = y;
}

Sampler::Sampler(){
	dx = 0;
	dy = 0;
}
Sampler::Sampler(int x, int y){
	dx = x - 1;
	dy = y - 1;
}
Sampler::Sampler(int x, int y, int aliasing){
	dx = x - 1;
	dy = y - 1;
	distribution = aliasing;
}

bool Sampler::generateSample(Sample* sample){
	if (distribution > 1){
		float a = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		*sample = Sample(curr_x + a, curr_y + b);
		if (counter > distribution){
			counter = 0;
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
		}
		else{
			counter += 1;
		}
		return true;
	}

	else{
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
}

Film::Film(){
	FreeImage_Initialise();
	dx = 99;
	dy = 99;
	sum = vector<Color>(100 * 100, Color(0, 0, 0));
	bitmap = FreeImage_Allocate(100, 100, 24);
}
Film::Film(int x, int y){
	FreeImage_Initialise();
	dx = x-1;
	dy = y-1;
	sum = vector<Color>(x*y, Color(0, 0, 0));
	bitmap = FreeImage_Allocate(dx, dy, 24); //24?
}
Film::Film(int x, int y, int spp){
	FreeImage_Initialise();
	dx = x-1;
	dy = y-1;
	samplesPerPixel = spp;
	sum = vector<Color>(x*y, Color(0, 0, 0));
	bitmap = FreeImage_Allocate(dx, dy, 24); //24?
}

// Will write the color to (sample.x, sample.y) on the image
void Film::commit(Sample& sample, Color& c){
	int x = (int)sample.dx;
	int y = (int)sample.dy;
	sum[x + dx*y] = sum[x + dx*y] + c;
}
// Output image to a file
void Film::writeImage(char* filename){

	for (int x = 0; x < dx; x++){
		for (int y = 0; y < dy; y++){
			Color c = sum[x + dx*y];
			RGBQUAD color;
			color.rgbRed = min(c.x() / samplesPerPixel, 1) * 255;
			color.rgbGreen = min(c.y() / samplesPerPixel, 1) * 255;
			color.rgbBlue = min(c.z() / samplesPerPixel, 1) * 255;
			FreeImage_SetPixelColor(bitmap, x, y, &color);
		}
	}
	int index = string(filename).find_last_of('.') + 1;
	if (FreeImage_Save(FreeImage_GetFIFFromFormat(filename + index), bitmap, filename)) {
		cout << "Image saved successfully." << endl;
	}
	FreeImage_DeInitialise();
}

Camera::Camera(){
	eye = Vector3f(0, 0, 1);
	LL = Vector3f(-1, -1, 0);
	LR = Vector3f(1, -1, 0);
	UL = Vector3f(-1, 1, 0);
	UR = Vector3f(1, 1, 0);
	dx = 0; dy = 0;
	t_min = 1;
	t_max = FLT_MAX;
}
Camera::Camera(Vector3f p, Vector3f ll, Vector3f lr, Vector3f ul, Vector3f ur, float x, float y, float tmin, float tmax){
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
void Camera::generateRay(Sample& sample, Ray* ray){
	float u = sample.dx / dx;
	float v = sample.dy / dy;
	if (u > 1 || v > 1){
		cout << "u or v is greater than 1" << endl;
		return;
	}
	Vector3f p = (1 - u) * ((1 - v) * LL + v * UL) + u * ((1 - v) * LR + v * UR);
	*ray = Ray(eye, p - eye, t_min, t_max);
}

RayTracer::RayTracer(int depth){
	recursionDepth = depth;
}
bool RayTracer::shadow(Ray ray){
	return primitives.intersectP(ray);
}
void RayTracer::trace(Ray ray, int depth, Color* color) {
	Color c = Color(0, 0, 0);
	float thit = 0;
	Intersection in = Intersection();

	if (depth < 0){
		*color = c; //Make the color black and return
		return;
	}
	if (!primitives.intersect(ray, &thit, &in)){
		*color = c;
		return;
	}
	BRDF b;
	in.primitive->getBRDF(in.localGeo, &b);
	Ray lightray;
	Color lightColor;
	if (depth == recursionDepth) c = c + b.ambient();
	for (Light l : lights){
		l.generateLightRay(in.localGeo, &lightray, &lightColor);
		if (!shadow(lightray)){
			c = c + b.diffuse(in.localGeo.normal, lightray.direction, lightColor);
			c = c + b.specular(-(ray.direction.normalized()), in.localGeo.normal, lightray.direction, lightColor, b.specularCoefficient());
		}
	}
	// Handle mirror reflection
	if (!b.kr.isZero()) {
		Ray reflectRay = createReflectRay(in.localGeo, ray);
		Color tempColor = Color(0, 0, 0);
		// Make a recursive call to trace the reflected ray
		trace(reflectRay, depth - 1, &tempColor);
		c = c + tempColor.cwiseProduct(b.kr);
	}

	*color = c;
}

Ray RayTracer::createReflectRay(LocalGeo lg, Ray ray){
	Vector3f r = ray.direction - 2 * (ray.direction.dot(lg.normal)) * lg.normal;
	return Ray(lg.position, r, 0.01, FLT_MAX);
}

Scene::Scene(){
	lights = vector<Light>();
	primitives = AggregatePrimitive();
	raytracer = RayTracer();
	sampler = Sampler();
	camera = Camera();
	film = Film();
}
Scene::Scene(Vector3f e, Vector3f ll, Vector3f lr, Vector3f ul, Vector3f ur, float x, float y, int depth){
	lights = vector<Light>();
	primitives = AggregatePrimitive();
	recursionDepth = depth;
	raytracer = RayTracer(depth);
	sampler = Sampler(x, y);
	camera = Camera(e, ll, lr, ul, ur, x, y, 1.0, FLT_MAX);
	film = Film(x, y);
}
Scene::Scene(Vector3f e, Vector3f ll, Vector3f lr, Vector3f ul, Vector3f ur, float x, float y, int depth, int samples){
	lights = vector<Light>();
	primitives = AggregatePrimitive();
	recursionDepth = depth;
	raytracer = RayTracer(depth);
	sampler = Sampler(x, y, samples);
	camera = Camera(e, ll, lr, ul, ur, x, y, 1.0, FLT_MAX);
	film = Film(x, y, samples);
	numSamples = samples;
}

void Scene::addPrimitives(vector<Primitive*> p){
	for (Primitive* pr : p){
		primitives.addPrimitive(pr);
	}
}
void Scene::addLight(Light l){
	lights.push_back(l);
}

void Scene::render(char* filename) {
	Sample sample = Sample();
	Ray ray = Ray();
	Color color = Color(0, 0, 0);
	while (sampler.generateSample(&sample)) {
		camera.generateRay(sample, &ray);
		raytracer.trace(ray, recursionDepth, &color);
		film.commit(sample, color);
	}
	film.writeImage(filename);
}

void spheretest_yellow_shading(){

	//initialization of scene
	Vector3f eye = Vector3f(0, 0, 2);
	Vector3f ul = Vector3f(-1, 1, 0);
	Vector3f ur = Vector3f(1, 1, 0);
	Vector3f lr = Vector3f(1, -1, 0);
	Vector3f ll = Vector3f(-1, -1, 0);
	Scene s = Scene(eye, ll, lr, ul, ur, 1000, 1000, 25);

	//primitives
	BRDF yellow = BRDF(Vector3f(0.1, 0.1, 0), Vector3f(1, 1, 0), Vector3f(0.8, 0.8, 0.8), Vector3f(0.0, 0.0, 0.0), 16);
	GeometricPrimitive g1 = GeometricPrimitive(new Sphere(Vector3f(0.0, 0.0, -2.0), 1.0, yellow));
	pr.push_back((Primitive*)&g1);
	s.addPrimitives(pr);

	//lights
	s.addLight(Light(Color(Vector3f(0.6, 0.6, 0.6)), POINTLIGHT, Vector3f(200, 200, 200)));

	//render call
	filename = "spheretest_yellow_shading.bmp";
	s.render(filename);
}

void spheretest_with_two_lights(){

	//initialization of scene
	Vector3f eye = Vector3f(0, 0, 2);
	Vector3f ul = Vector3f(-1, 1, 0);
	Vector3f ur = Vector3f(1, 1, 0);
	Vector3f lr = Vector3f(1, -1, 0);
	Vector3f ll = Vector3f(-1, -1, 0);
	Scene s = Scene(eye, ll, lr, ul, ur, 1000, 1000, 0);

	//primitives
	BRDF yellow = BRDF(Vector3f(0.1, 0.1, 0), Vector3f(1, 1, 0), Vector3f(0.8, 0.8, 0.8), Vector3f(0.0, 0.0, 0.0), 16);
	GeometricPrimitive g1 = GeometricPrimitive(new Sphere(Vector3f(0.0, 0.0, -2.0), 1.0, yellow));
	pr.push_back((Primitive*)&g1);
	s.addPrimitives(pr);

	//lights
	s.addLight(Light(Color(Vector3f(0.6, 0.6, 0.6)), POINTLIGHT, Vector3f(200, 200, 200)));
	s.addLight(Light(Color(Vector3f(0, 0.4, 0.4)), DIRECTIONALLIGHT, Vector3f(0, 1, -1)));

	//render call
	filename = "spheretest_pt_dir.bmp";
	s.render(filename);

}

//void spheretest_with_two_spheres(){
//
//	//initialization of scene
//	Vector3f eye = Vector3f(0, 0, 2);
//	Vector3f ul = Vector3f(-1, 1, 0);
//	Vector3f ur = Vector3f(1, 1, 0);
//	Vector3f lr = Vector3f(1, -1, 0);
//	Vector3f ll = Vector3f(-1, -1, 0);
//	Scene s = Scene(eye, ll, lr, ul, ur, 1000, 1000, 0);
//
//	//primitives
//	BRDF yellow = BRDF(Vector3f(0.1, 0.1, 0), Vector3f(1, 1, 0), Vector3f(0.8, 0.8, 0.8), Vector3f(0.0, 0.0, 0.0), 16);
//	BRDF blue = BRDF(Vector3f(0, 0.05, 0.1), Vector3f(0.11, 0.20, 0.54), Vector3f(0, 1, 1), Vector3f(0.0, 0.0, 0.0), 16);
//	Sphere* s1 = new Sphere(Vector3f(0.0, 0.0, -2.0), 1.0, yellow);
//	Sphere* s2 = new Sphere(Vector3f(1, 1, -2), 1.0, blue);
//	GeometricPrimitive g1 = GeometricPrimitive(s1);
//	GeometricPrimitive g2 = GeometricPrimitive(s2);
//	
//	pr = { (Primitive*)&g1, (Primitive*)&g2 };
//	s.addPrimitives(pr);
//
//	//lights
//	s.addLight(Light(Color(Vector3f(0.6, 0.6, 0.6)), POINTLIGHT, Vector3f(200, 200, 200)));
//
//	//render call
//	filename = "spheretest_two_spheres.bmp";
//	s.render(filename);
//}

void shadowtest(){

	//initialization of scene
	Vector3f eye = Vector3f(0, 0, 0);
	Vector3f ul = Vector3f(-1, 1, -3);
	Vector3f ur = Vector3f(1, 1, -3);
	Vector3f lr = Vector3f(1, -1, -3);
	Vector3f ll = Vector3f(-1, -1, -3);
	Scene s = Scene(eye, ll, lr, ul, ur, 1000, 1000, 1);

	//primitives
	GeometricPrimitive g1 = GeometricPrimitive(
		new Sphere(Vector3f(0, 0, -20), 3,
		BRDF(Vector3f(0.1, 0.1, 0.1), Vector3f(1, 0, 1), Vector3f(1, 1, 1), Vector3f(), 50)));
	GeometricPrimitive g2 = GeometricPrimitive(
		new Sphere(Vector3f(-2, 2, -15), 1,
		BRDF(Vector3f(0.1, 0.1, 0.1), Vector3f(1, 1, 0), Vector3f(1, 1, 1), Vector3f(), 50)));
	GeometricPrimitive g3 = GeometricPrimitive(
		new Sphere(Vector3f(-2, -2, -15), 1, 
		BRDF(Vector3f(0.1, 0.1, 0.1), Vector3f(0, 1, 1), Vector3f(1, 1, 1), Vector3f(), 50)));
	GeometricPrimitive g4 = GeometricPrimitive(
		new Triangle(Vector3f(5, 5, -17), Vector3f(1, 4, -20), Vector3f(6, -1, -20), 
		BRDF(Vector3f(0.1, 0.1, 0.1), Vector3f(0.1, 0.1, 0.1), Vector3f(1, 1, 1), Vector3f(1, 1, 1), 50)));
	
	pr = vector<Primitive*> {(Primitive*)(&g1), (Primitive*)(&g2), (Primitive*)(&g3), (Primitive*)(&g4)};
	s.addPrimitives(pr);

	//lights
	s.addLight(Light(Color(Vector3f(1, 1, 1)), DIRECTIONALLIGHT, Vector3f(0.57735027, -0.57735027, -0.57735027)));
	s.addLight(Light(Color(Vector3f(0, 0, 1)), DIRECTIONALLIGHT, Vector3f(0.57735027, 0.57735027, -0.57735027)));

	//render call
	filename = "shadows.bmp";
	s.render(filename);

}



int main(int argc, char *argv[]) {

	//spheretest_yellow_shading();
	//spheretest_with_two_lights();
	//spheretest_with_two_spheres();
	shadowtest();
	//reflection_test();
	//transform_test();
	//string file(argv[1]);
	//loadScene(file);
	return 0;
}