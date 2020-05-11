#include "glut.h"
#include "Controler.h"
#include <Windows.h>
#include <string>


using namespace primitives;
using namespace controler;
using std::string;


int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	int argc = 1;																								// setting up unused parameters
	char *argv[1] = { (char*)"" };

	glutInit(&argc, argv);																						// always should be before Controler initialization
	
	string windowTitle = "Polyline editor";
	auto windowSize = Size<unsigned int>(1024, 768);

	Color backgroundColor = Color(0.95, 0.95, 1);
	Color polyLineColor = Color(0.45, 0.45, 0.45);
	Color peakPointColor = Color(0.7, 0.3, 0.3);

	Controler applicationControler(windowSize, windowTitle, backgroundColor, polyLineColor, peakPointColor);	// core of application

	glutMainLoop();																								// always after Controler initialization
	return 0;
}