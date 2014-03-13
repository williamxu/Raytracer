#include "Raytracer.h"

//Globals
char* filename;
vector<Light> lights;
AggregatePrimitive primitives;


void spheretest_yellow_shading(){
	//-pl 200 200 200 0.6 0.6 0.6 - kd 1 1 0 - ka 0.1 0.1 0 - ks 0.8 0.8 0.8 - sp 16

	//camera and image plane
	Vector3f eye = Vector3f(0, 0, 2);
	Vector3f ul = Vector3f(-1, 1, 0);
	Vector3f ur = Vector3f(1, 1, 0);
	Vector3f lr = Vector3f(1, -1, 0);
	Vector3f ll = Vector3f(-1, -1, 0);

	//scene initializer
	Scene s = Scene(eye, ll, lr, ul, ur, 1000, 1000, 1);

	//lights
	s.addLight(Light(Color(Vector3f(0.6, 0.6, 0.6)), POINTLIGHT, Vector3f(200, 200, 200)));

	//spheres
	BRDF yellow = BRDF(Vector3f(0.1, 0.1, 0), Vector3f(1, 1, 0), Vector3f(0.8, 0.8, 0.8), Vector3f(0.0, 0.0, 0.0), 16);
	Sphere yellowSphere = Sphere(Vector3f(0.0, 0.0, -2.0), 1.0, yellow);
	Primitive* sphere = (Primitive*)(&GeometricPrimitive(&yellowSphere));
	s.addPrimitive(sphere);

	filename = "spheretest_yellow_shading.bmp";
	//render call
	s.render(filename);
}
void spheretest_viewing_angle1(){
	//-pl 200 200 200 0.6 0.6 0.6 - kd 1 1 0 - ka 0.1 0.1 0 - ks 0.8 0.8 0.8 - sp 16

	//camera and image plane
	Vector3f eye = Vector3f(0, 0, 0);
	Vector3f ul = Vector3f(-1, 1, 0);
	Vector3f ur = Vector3f(1, 1, -2);
	Vector3f lr = Vector3f(1, -1, -2);
	Vector3f ll = Vector3f(-1, -1, 0);

	//scene initializer
	Scene s = Scene(eye, ll, lr, ul, ur, 1000, 1000, 1);

	//lights
	s.addLight(Light(Color(Vector3f(0.6, 0.6, 0.6)), POINTLIGHT, Vector3f(200, 200, 200)));

	//spheres
	BRDF yellow = BRDF(Vector3f(0.1, 0.1, 0), Vector3f(1, 1, 0), Vector3f(0.8, 0.8, 0.8), Vector3f(0.0, 0.0, 0.0), 16);
	s.addPrimitive((Primitive*)(
		&GeometricPrimitive(
		&Sphere(Vector3f(-1, 1, -2), 1.0, yellow))
		)
		);

	filename = "spheretest_view1.bmp";
	//render call
	s.render(filename);

}
void spheretest_viewing_angle2(){
	//-pl 200 200 200 0.6 0.6 0.6 - kd 1 1 0 - ka 0.1 0.1 0 - ks 0.8 0.8 0.8 - sp 16

	//camera and image plane
	Vector3f eye = Vector3f(0, 0, 0);
	Vector3f ul = Vector3f(-1, 1, 0);
	Vector3f ur = Vector3f(1, 1, -1.3);
	Vector3f lr = Vector3f(1, -1, -1.3);
	Vector3f ll = Vector3f(-1, -1, 0);

	//scene initializer
	Scene s = Scene(eye, ll, lr, ul, ur, 1000, 1000, 1);

	//lights
	s.addLight(Light(Color(Vector3f(0.6, 0.6, 0.6)), POINTLIGHT, Vector3f(200, 200, 200)));

	//spheres
	BRDF yellow = BRDF(Vector3f(0.1, 0.1, 0), Vector3f(1, 1, 0), Vector3f(0.8, 0.8, 0.8), Vector3f(0.0, 0.0, 0.0), 16);
	Sphere yellowSphere = Sphere(Vector3f(0.0, 0.0, -2.0), 1.0, yellow);
	Primitive* sphere = (Primitive*)(&GeometricPrimitive(&yellowSphere));
	s.addPrimitive(sphere);

	filename = "spheretest_view2.bmp";
	//render call
	s.render(filename);
}
void spheretest_with_two_lights(){
	//"-pl 200 200 200 0.6 0.6 0.6 -dl 0 1 -1 0 0.4 0.4 -kd 1 1 0 -ka 0.1 0.1 0 -ks 0.8 0.8 0.8 -sp 16"

	//camera and image plane
	Vector3f eye = Vector3f(0, 0, 2);
	Vector3f ul = Vector3f(-1, 1, 0);
	Vector3f ur = Vector3f(1, 1, 0);
	Vector3f lr = Vector3f(1, -1, 0);
	Vector3f ll = Vector3f(-1, -1, 0);

	//scene initializer
	Scene s = Scene(eye, ll, lr, ul, ur, 1000, 1000, 1);

	//lights
	s.addLight(Light(Color(Vector3f(0.6, 0.6, 0.6)), POINTLIGHT, Vector3f(200, 200, 200)));
	s.addLight(Light(Color(Vector3f(0, 0.4, 0.4)), DIRECTIONALLIGHT, Vector3f(0, 1, -1)));

	//spheres
	BRDF yellow = BRDF(Vector3f(0.1, 0.1, 0), Vector3f(1, 1, 0), Vector3f(0.8, 0.8, 0.8), Vector3f(0.0, 0.0, 0.0), 16);
	Sphere yellowSphere = Sphere(Vector3f(0.0, 0.0, -2.0), 1.0, yellow);
	Primitive* sphere = (Primitive*)(&GeometricPrimitive(&yellowSphere));
	s.addPrimitive(sphere);

	filename = "spheretest_pt_dir.bmp";
	//render call
	s.render(filename);

}
void spheretest_with_two_spheres(){
	//TODO fix this test

	//"-pl 200 200 200 0.6 0.6 0.6 -kd 1 1 0 -ka 0.1 0.1 0 -ks 0.8 0.8 0.8 -sp 16"

	//camera and image plane
	Vector3f eye = Vector3f(0, 0, 2);
	Vector3f ul = Vector3f(-1, 1, 0);
	Vector3f ur = Vector3f(1, 1, 0);
	Vector3f lr = Vector3f(1, -1, 0);
	Vector3f ll = Vector3f(-1, -1, 0);

	//scene initializer
	Scene s = Scene(eye, ll, lr, ul, ur, 1000, 1000, 1);

	//lights
	s.addLight(Light(Color(Vector3f(0.6, 0.6, 0.6)), POINTLIGHT, Vector3f(200, 200, 200)));

	//spheres

	BRDF blue = BRDF(Vector3f(0, 0.05, 0.1), Vector3f(0.11, 0.20, 0.54), Vector3f(0, 1, 1), Vector3f(0.0, 0.0, 0.0), 16);
	Sphere blueSphere = Sphere(Vector3f(0.0, 0.0, -2.0), 1.0, blue);

	BRDF yellow = BRDF(Vector3f(0.1, 0.1, 0), Vector3f(1, 1, 0), Vector3f(0.8, 0.8, 0.8), Vector3f(0.0, 0.0, 0.0), 16);
	Sphere yellowSphere = Sphere(Vector3f(1, 0.9, -2.4), 1.0, yellow);

	s.addPrimitive(
		(Primitive*)(&GeometricPrimitive(&blueSphere))
		);
	s.addPrimitive(
		(Primitive*)(&GeometricPrimitive(&yellowSphere))
		);

	filename = "spheretest_two_spheres.bmp";
	//render call
	s.render(filename);
}
void triangletest_blue_shading(){
	//"-pl 200 200 200 0.6 0.6 0.6 -kd 1 1 0 -ka 0.1 0.1 0 -ks 0.8 0.8 0.8 -sp 16"


	//camera and image plane
	Vector3f eye = Vector3f(0, 0, 2);
	Vector3f ul = Vector3f(-1, 1, 0);
	Vector3f ur = Vector3f(1, 1, 0);
	Vector3f lr = Vector3f(1, -1, 0);
	Vector3f ll = Vector3f(-1, -1, 0);

	//scene initializer
	Scene s = Scene(eye, ll, lr, ul, ur, 1000, 1000, 1);

	//lights
	s.addLight(Light(Color(Vector3f(0.6, 0.6, 0.6)), POINTLIGHT, Vector3f(200, 200, 200)));

	//triangles
	BRDF blue = BRDF(Vector3f(0, 0.2, 0.2), Vector3f(0.21, 1, 1), Vector3f(1, 1, 1), Vector3f(0.0, 0.0, 0.0), 16);

	Triangle t1 = Triangle(
		Vector3f(0, 1, -1), Vector3f(1, 0, -1), Vector3f(-1, 0, -1), blue);

	Triangle t2 = Triangle(
		Vector3f(1, 0, -1), Vector3f(1, 1, -2), Vector3f(0, 1, -1), blue);

	Sphere s1 = Sphere(Vector3f(0, -1, -2), 0.5, blue);
	s.addPrimitive((Primitive*)(
		&GeometricPrimitive(
		&t1)));

	s.addPrimitive((Primitive*)(
		&GeometricPrimitive(
		&t2)));
	s.addPrimitive((Primitive*)(
		&GeometricPrimitive(
		&s1)));

	filename = "triangletest_blue_shading.bmp";
	//render call
	s.render(filename);
}
void spheres_shadowtest(){

	//camera and image plane
	Vector3f eye = Vector3f(0, 0, 0);
	Vector3f ul = Vector3f(-1, 1, -3);
	Vector3f ur = Vector3f(1, 1, -3);
	Vector3f lr = Vector3f(1, -1, -3);
	Vector3f ll = Vector3f(-1, -1, -3);

	//scene initializer
	Scene s = Scene(eye, ll, lr, ul, ur, 500,500, 1);
	//Scene s = Scene(eye, ll, lr, ul, ur, 200, 200, 1);

	//lights
	Light l1 = Light(Color(Vector3f(1, 1, 1)), DIRECTIONALLIGHT, Vector3f(0.57735027, -0.57735027, -0.57735027));
	Light l2 = Light(Color(Vector3f(0, 0, 1)), DIRECTIONALLIGHT, Vector3f(0.57735027, 0.57735027, -0.57735027));

	//primitives
	Sphere s1 = Sphere(Vector3f(0, 0, -20), 3, BRDF(Vector3f(0.1, 0.1, 0.1), Vector3f(1, 0, 1), Vector3f(1, 1, 1), Vector3f(0, 0, 0), 50));
	GeometricPrimitive g1 = GeometricPrimitive(&s1);
	Sphere s2 = Sphere(Vector3f(-2, 2, -15), 1, BRDF(Vector3f(0.1, 0.1, 0.1), Vector3f(1, 1, 0), Vector3f(1, 1, 1), Vector3f(0, 0, 0), 50));
	GeometricPrimitive g2 = GeometricPrimitive(&s2);
	Sphere s3 = Sphere(Vector3f(-2, -2, -15), 1, BRDF(Vector3f(0.1, 0.1, 0.1), Vector3f(0, 1, 1), Vector3f(1, 1, 1), Vector3f(0, 0, 0), 50));
	GeometricPrimitive g3 = GeometricPrimitive(&s3);
	Triangle t1 = Triangle(Vector3f(5, 5, -17), Vector3f(1, 4, -20), Vector3f(6, -1, -20), BRDF(Vector3f(0.1, 0.1, 0.1), Vector3f(0.1, 0.1, 0.1), Vector3f(1, 1, 1), Vector3f(1, 1, 1), 50));
	GeometricPrimitive g4 = GeometricPrimitive(&t1);

	vector<Primitive*> pr = vector<Primitive*> {(Primitive*)(&g1), (Primitive*)(&g2), (Primitive*)(&g3), (Primitive*)(&g4)};
	vector<Light> lights = vector<Light> {l1, l2};
	s.raytracer = RayTracer(lights, pr, 1);

	filename = "shadows.bmp";
	//render call
	s.render(filename);


}

void triangle_test(){

	//camera and image plane
	Vector3f eye = Vector3f(0, 0, 0);
	Vector3f ul = Vector3f(-1, 1, -3);
	Vector3f ur = Vector3f(1, 1, -3);
	Vector3f lr = Vector3f(1, -1, -3);
	Vector3f ll = Vector3f(-1, -1, -3);

	//scene initializer
	//Scene s = Scene(eye, ll, lr, ul, ur, 500,500, 1);
	Scene s = Scene(eye, ll, lr, ul, ur, 200, 200, 1);

	Light l1 = Light(Color(Vector3f(0, 0, 1)), DIRECTIONALLIGHT, Vector3f(0.57735027, 0.57735027, -0.57735027));

	//primitives
	Triangle t1 = Triangle(
		Vector3f(5, 5, -17), Vector3f(1, 4, -20), Vector3f(6, -1, -20),
		BRDF(Vector3f(0.1, 0.1, 0.1), Vector3f(0.1, 0.1, 0.1), Vector3f(1, 1, 1), Vector3f(1, 1, 1), 50));
	GeometricPrimitive g1 = GeometricPrimitive(&t1);

	vector<Primitive*> pr = vector<Primitive*> {(Primitive*)(&g1)};
	vector<Light> lights = vector<Light> {l1};
	s.raytracer = RayTracer(lights, pr, 1);

	filename = "triangles.bmp";
	//render call
	s.render(filename);


}


void loadScene(string file){
	ifstream input(file.c_str());
	float dx; float dy;
	int recursionDepth = 5;
	Vector3f eye;
	Vector3f ul;
	Vector3f ll;
	Vector3f ur;
	Vector3f lr;
	vector<Vector3f> points;
	Scene s;

	if (!input.is_open()){
		cout << "Error opening file" << endl;
	}
	else {
		string Line;
		while (input.good()){
			getline(input, Line);
			istringstream lines(Line);
			vector<string> line;
			string temp;
			while (lines >> temp){
				line.push_back(temp);
			}

			//ignore empty lines
			if (line.size() == 0){
				continue;
			}

			//ignore # lines
			else if (line[0] == "#"){
				continue;
			}

			//size width height
			else if (line[0] == "size"){
				dx = atof(line[1].c_str());
				dy = atof(line[2].c_str());
			}

			//maxDepth depth
			else if (line[0] == "maxDepth"){
				recursionDepth = atoi(line[1].c_str());
			}

			//camera eyex eyey eyez ulx uly ulz llx lly llz urx ury urz lrx lry lrz
			else if (line[0] == "camera"){
				eye = Vector3f(atof(line[1].c_str()), atof(line[2].c_str()), atof(line[3].c_str()));
				ul = Vector3f(atof(line[4].c_str()), atof(line[5].c_str()), atof(line[6].c_str()));
				ll = Vector3f(atof(line[7].c_str()), atof(line[8].c_str()), atof(line[9].c_str()));
				ur = Vector3f(atof(line[10].c_str()), atof(line[11].c_str()), atof(line[12].c_str()));
				lr = Vector3f(atof(line[13].c_str()), atof(line[14].c_str()), atof(line[15].c_str()));
				s = Scene(eye, ll, lr, ul, ur, dx, dy, recursionDepth);
			}

			//sphere centerx centery centerz radius kar kag kab ksr ksg ksb kdr kdg kdb krr krg krb
			else if (line[0] == "sphere"){
				Vector3f ka = Vector3f(atof(line[4].c_str()), atof(line[5].c_str()), atof(line[6].c_str()));
				Vector3f ks = Vector3f(atof(line[7].c_str()), atof(line[8].c_str()), atof(line[9].c_str()));
				Vector3f kd = Vector3f(atof(line[10].c_str()), atof(line[11].c_str()), atof(line[12].c_str()));
				Vector3f kr = Vector3f(atof(line[13].c_str()), atof(line[14].c_str()), atof(line[15].c_str()));
				float sp = atof(line[16].c_str());
				BRDF color = BRDF(ka, ks, kd, kr, sp);
				Sphere sph = Sphere(Vector3f(atof(line[1].c_str()), atof(line[2].c_str()), atof(line[3].c_str())), atof(line[4].c_str()), color);
				s.addPrimitive(
					(Primitive*)(&GeometricPrimitive(&sph))
					);
			}

			//v x y z
			else if (line[0] == "v"){
				points.push_back(Vector3f(atof(line[1].c_str()), atof(line[2].c_str()), atof(line[3].c_str())));
			}

			//triangle v1 v2 v3 kar kag kab ksr ksg ksb kdr kdg kdb krr krg krb
			else if (line[0] == "triangle"){
				Vector3f ka = Vector3f(atof(line[4].c_str()), atof(line[5].c_str()), atof(line[6].c_str()));
				Vector3f ks = Vector3f(atof(line[7].c_str()), atof(line[8].c_str()), atof(line[9].c_str()));
				Vector3f kd = Vector3f(atof(line[10].c_str()), atof(line[11].c_str()), atof(line[12].c_str()));
				Vector3f kr = Vector3f(atof(line[13].c_str()), atof(line[14].c_str()), atof(line[15].c_str()));
				float sp = atof(line[16].c_str());
				BRDF color = BRDF(ka, ks, kd, kr, sp);
				Triangle tri = Triangle(points[atof(line[1].c_str())], points[atof(line[2].c_str())], points[atof(line[3].c_str())], color);
				s.addPrimitive(
					(Primitive*)(&GeometricPrimitive(&tri))
					);
			}

			//pLight x y z cr cg cb
			else if (line[0] == "pLight"){
				Color color = Color(Vector3f(atof(line[4].c_str()), atof(line[5].c_str()), atof(line[6].c_str())));
				s.addLight(Light(color, POINTLIGHT, Vector3f(atof(line[1].c_str()), atof(line[2].c_str()), atof(line[3].c_str()))));
			}

			//dLight x y z cr cg cb
			else if (line[0] == "dLight"){
				Color color = Color(Vector3f(atof(line[4].c_str()), atof(line[5].c_str()), atof(line[6].c_str())));
				s.addLight(Light(color, DIRECTIONALLIGHT, Vector3f(atof(line[1].c_str()), atof(line[2].c_str()), atof(line[3].c_str()))));
			}
		}
		s.render(filename);
	}

}

int main(int argc, char *argv[]) {

	//spheretest_yellow_shading();
	//spheretest_with_two_lights();
	//spheretest_with_two_spheres();
	//spheretest_viewing_angle1();
	//spheretest_viewing_angle2();
	//triangletest_blue_shading();
	spheres_shadowtest();
	//triangle_test();
	//string file(argv[1]);
	//loadScene(file);
	return 0;
}









