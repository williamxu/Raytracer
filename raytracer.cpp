#include "Raytracer.h"

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
//generates one sample per pixel
bool Sampler::generateSample(Sample* sample){
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

Film::Film(){
	dx = 0;
	dy = 0;
}
Film::Film(int x, int y){
	FreeImage_Initialise();
	dx = x;
	dy = y;
	bitmap = FreeImage_Allocate(dx, dy, 24); //24?
}
// Will write the color to (sample.x, sample.y) on the image
void Film::commit(Sample& sample, Color& c){
	RGBQUAD color;
	color.rgbRed = min(c.rgb[0], 1) * 255;
	color.rgbGreen = min(c.rgb[1], 1) * 255;
	color.rgbBlue = min(c.rgb[2], 1) * 255;
	FreeImage_SetPixelColor(bitmap, (int)sample.dx, (int)sample.dy, &color);

}
// Output image to a file
void Film::writeImage(char* filename){

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
	float u = (sample.dx + 0.5) / dx;
	float v = (sample.dy + 0.5) / dy;
	if (u > 1 || v > 1){
		cout << "u or v is greater than 1" << endl;
		return;
	}
	Vector3f p = (1 - u) * ((1 - v) * LL + v * UL) + u * ((1 - v) * LR + v * UR);
	*ray = Ray(eye, p - eye, t_min, t_max);
}

RayTracer::RayTracer(){
	primitives = AggregatePrimitive();
	lights = vector<Light>();
}
RayTracer::RayTracer(vector<Light> l, vector<Primitive*> p, int depth){
	primitives = AggregatePrimitive(p);
	lights = l;
	recursionDepth = depth;
}
void RayTracer::addLight(Light l){
	lights.push_back(l);
}
void RayTracer::addPrimitive(Primitive* p){
	primitives.addPrimitive(p);
}

bool RayTracer::shadow(Ray ray){
	return primitives.intersectP(ray);
}
void RayTracer::trace(Ray ray, int depth, Color* color) {
	Color c = Color();
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
			c = c + b.diffuse(in.localGeo.normal.xyz, lightray.direction, lightColor);
			c = c + b.specular(-(ray.direction.normalized()), in.localGeo.normal.xyz, lightray.direction, lightColor, b.specularCoefficient());
		}
	}
	// Handle mirror reflection
	if (!b.kr.rgb.isZero()) {
		Ray reflectRay = createReflectRay(in.localGeo, ray);
		Color tempColor = Color();
		// Make a recursive call to trace the reflected ray
		trace(reflectRay, depth - 1, &tempColor);
		c = c + tempColor * b.kr;
	}

	*color = c;
}

Ray RayTracer::createReflectRay(LocalGeo lg, Ray ray){
	Vector3f r = ray.direction - 2 * (ray.direction.dot(lg.normal.xyz)) * lg.normal.xyz;
	return Ray(lg.position, r, 0.01, FLT_MAX);
}

Scene::Scene(){
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
Scene::Scene(Vector3f e, Vector3f ll, Vector3f lr, Vector3f ul, Vector3f ur, float x, float y, vector<Light> lights, vector<Primitive*> primitives, int depth){
	eye = e;
	LL = ll;
	LR = lr;
	UL = ul;
	UR = ur;
	dx = x;
	dy = y;
	recursionDepth = depth;
	raytracer = RayTracer(lights, primitives, depth);
	sampler = Sampler(dx, dy);
	camera = Camera(eye, ll, lr, ul, ur, x, y, 1.0, FLT_MAX);
	film = Film(dx, dy);
}


void Scene::addLight(Light light){
	raytracer.addLight(light);
}
void Scene::addPrimitive(Primitive* p){
	raytracer.addPrimitive(p);
}

void Scene::render(char* filename) {
	Sample sample = Sample();
	Ray ray = Ray();
	Color color = Color();
	while (sampler.generateSample(&sample)) {
		camera.generateRay(sample, &ray);
		raytracer.trace(ray, recursionDepth, &color);
		film.commit(sample, color);
	}
	film.writeImage(filename);
}