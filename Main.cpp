

////void transform_test(){
////
////	//camera and image plane
////	Vector3f eye = Vector3f();
////	Vector3f ul = Vector3f(-1, 1, -3);
////	Vector3f ur = Vector3f(1, 1, -3);
////	Vector3f lr = Vector3f(1, -1, -3);
////	Vector3f ll = Vector3f(-1, -1, -3);
////
////	//scene initializer
////	Light l1 = Light(Color(Vector3f(1, 1, 1)), DIRECTIONALLIGHT, Vector3f(0.57735027, -0.57735027, -0.57735027));
////	Light l2 = Light(Color(Vector3f(1, 1, 1)), DIRECTIONALLIGHT, Vector3f(-0.57735027, 0.57735027, 0.57735027));
////
////	Sphere s1 = Sphere(Vector3f(0, 0, -17), 2,
////		BRDF(Vector3f(0.1, 0.1, 0.1), Vector3f(1, 0, 0), Vector3f(1, 1, 1), Vector3f(0.9, 0.9, 0.9), 50));
////	
////	aMatrix trans = aMatrix();
////	//trans.createTranslation(4,4,0);
////	//trans.createScale(2, 2, 2);
////	//trans.createEulerRotation(90, 0, 0);
////
////	aMatrix invtrans = aMatrix();
////	//invtrans.createTranslation(-4,-4,0);
////	//invtrans.createScale(0.5, 0.5, 0.5);
////	//invtrans.createEulerRotation(-90, 0, 0);
////	
////	Transformation t = Transformation(trans);
////	Transformation t2 = Transformation(invtrans);
////	
////	GeometricPrimitive g1 = GeometricPrimitive(&s1,t,t2);
////	//GeometricPrimitive g1 = GeometricPrimitive(&s1);
////
////	vector<Light> lights = { l1, l2 };
////	vector<Primitive*> primitives = { (Primitive*)&g1 };
////
////	Scene s = Scene(eye, ll, lr, ul, ur, 200, 200, lights, primitives, 0);
////	filename = "ellipse1.bmp";
////	//render call
////	s.render(filename);
////}
////
////void loadScene(string file){
////	ifstream input(file.c_str());
////	float dx; float dy;
////	int recursionDepth;
////	Vector3f eye;
////	Vector3f ul;
////	Vector3f ll;
////	Vector3f ur;
////	Vector3f lr;
////	vector<Vector3f> points;
////	vector<Primitive*> primitives;
////	vector<Light> lights;
////
////	Scene s;
////
////	if (!input.is_open()){
////		cout << "Error opening file" << endl;
////	}
////	else {
////		string Line;
////		while (input.good()){
////			getline(input, Line);
////			istringstream lines(Line);
////			vector<string> line;
////			string temp;
////			while (lines >> temp){
////				line.push_back(temp);
////			}
////
////			//ignore empty lines
////			if (line.size() == 0){
////				continue;
////			}
////
////			//ignore # lines
////			else if (line[0] == "#"){
////				continue;
////			}
////
////			//size width height
////			else if (line[0] == "size"){
////				dx = atof(line[1].c_str());
////				dy = atof(line[2].c_str());
////			}
////
////			//maxDepth depth
////			else if (line[0] == "maxDepth"){
////				recursionDepth = atoi(line[1].c_str());
////			}
////
////			//camera eyex eyey eyez ulx uly ulz llx lly llz urx ury urz lrx lry lrz
////			else if (line[0] == "camera"){
////				eye = Vector3f(atof(line[1].c_str()), atof(line[2].c_str()), atof(line[3].c_str()));
////				ul = Vector3f(atof(line[4].c_str()), atof(line[5].c_str()), atof(line[6].c_str()));
////				ll = Vector3f(atof(line[7].c_str()), atof(line[8].c_str()), atof(line[9].c_str()));
////				ur = Vector3f(atof(line[10].c_str()), atof(line[11].c_str()), atof(line[12].c_str()));
////				lr = Vector3f(atof(line[13].c_str()), atof(line[14].c_str()), atof(line[15].c_str()));
////				
////			}
////
////			//sphere centerx centery centerz radius kar kag kab ksr ksg ksb kdr kdg kdb krr krg krb
////			else if (line[0] == "sphere"){
////				Vector3f ka = Vector3f(atof(line[4].c_str()), atof(line[5].c_str()), atof(line[6].c_str()));
////				Vector3f ks = Vector3f(atof(line[7].c_str()), atof(line[8].c_str()), atof(line[9].c_str()));
////				Vector3f kd = Vector3f(atof(line[10].c_str()), atof(line[11].c_str()), atof(line[12].c_str()));
////				Vector3f kr = Vector3f(atof(line[13].c_str()), atof(line[14].c_str()), atof(line[15].c_str()));
////				float sp = atof(line[16].c_str());
////				BRDF color = BRDF(ka, ks, kd, kr, sp);
////				Sphere sph = Sphere(Vector3f(atof(line[1].c_str()), atof(line[2].c_str()), atof(line[3].c_str())), atof(line[4].c_str()), color);
////				GeometricPrimitive gp = GeometricPrimitive(&sph);
////				primitives.push_back((Primitive*)&gp);
////			}
////
////			//v x y z
////			else if (line[0] == "v"){
////				points.push_back(Vector3f(atof(line[1].c_str()), atof(line[2].c_str()), atof(line[3].c_str())));
////			}
////
////			//triangle v1 v2 v3 kar kag kab ksr ksg ksb kdr kdg kdb krr krg krb
////			else if (line[0] == "triangle"){
////				Vector3f ka = Vector3f(atof(line[4].c_str()), atof(line[5].c_str()), atof(line[6].c_str()));
////				Vector3f ks = Vector3f(atof(line[7].c_str()), atof(line[8].c_str()), atof(line[9].c_str()));
////				Vector3f kd = Vector3f(atof(line[10].c_str()), atof(line[11].c_str()), atof(line[12].c_str()));
////				Vector3f kr = Vector3f(atof(line[13].c_str()), atof(line[14].c_str()), atof(line[15].c_str()));
////				float sp = atof(line[16].c_str());
////				BRDF color = BRDF(ka, ks, kd, kr, sp);
////				Triangle tri = Triangle(points[atof(line[1].c_str())], points[atof(line[2].c_str())], points[atof(line[3].c_str())], color);
////				GeometricPrimitive gp = GeometricPrimitive(&tri); 
////				primitives.push_back((Primitive*)(&GeometricPrimitive(&tri)));
////			}
////
////			//pLight x y z cr cg cb
////			else if (line[0] == "pLight"){
////				Color color = Color(Vector3f(atof(line[4].c_str()), atof(line[5].c_str()), atof(line[6].c_str())));
////				Light l = Light(color, POINTLIGHT, Vector3f(atof(line[1].c_str()), atof(line[2].c_str()), atof(line[3].c_str())));
////				lights.push_back(l);
////			}
////
////			//dLight x y z cr cg cb
////			else if (line[0] == "dLight"){
////				Color color = Color(Vector3f(atof(line[4].c_str()), atof(line[5].c_str()), atof(line[6].c_str())));
////				Light l = Light(color, DIRECTIONALLIGHT, Vector3f(atof(line[1].c_str()), atof(line[2].c_str()), atof(line[3].c_str())));
////				lights.push_back(l);
////			}
////		}
////		s = Scene(eye, ll, lr, ul, ur, dx, dy, lights, primitives, recursionDepth);
////		s.render(filename);
////	}
////
////}
////
//int main(int argc, char *argv[]) {
//
//	spheretest_yellow_shading();
//	//spheretest_with_two_lights();
//	//spheretest_with_two_spheres();
//	//spheretest_viewing_angle1();
//	//spheretest_viewing_angle2();
//	//spheres_shadowtest();
//	//reflection_test();
//	//transform_test();
//	//string file(argv[1]);
//	//loadScene(file);
//	return 0;
//}
