#include "Controler.h"



namespace controler
{
	// Functors
	// AddLine

	bool AddLine::operator()(Point<double>& point, unique_ptr<PolyLine>& polyLine)
	{
		return polyLine->addLine(point);
	}

	AddNodeFunctor* AddLine::copy() { return new AddLine(*this); }



	// AddArc

	bool AddArc::operator()(Point<double>& point, unique_ptr<PolyLine>& polyLine) 
	{
		return polyLine->addArc(point);
	}

	AddNodeFunctor* AddArc::copy()  { return new AddArc(*this); }
	



	// MainMenu

	MainMenu::MainMenu(PolyLineControler* polyLineControler, HistoryHandler* historyHandler)
	{
		MainMenu::polyLineControler = polyLineControler;
		MainMenu::historyHandler = historyHandler;
	}


	void MainMenu::initializeMenu()
	{
		glutCreateMenu(MainMenu::chooseOption);

		for (auto option : options)
		{
			glutAddMenuEntry(option.name, option.index);
		}

		glutAttachMenu(GLUT_RIGHT_BUTTON);
	}


	void MainMenu::disableOption(OptionName optionName)
	{
		Option currentOption = options[optionName];
		if (currentOption.enabled)
			currentOption.enabled = false;
	}


	void MainMenu::enableOption(OptionName optionName)
	{
		Option currentOption = options[optionName];
		if (!currentOption.enabled)
			currentOption.enabled = true;
	}


	void MainMenu::chooseOption(int option)
	{
		switch (option)
		{
		case OptionName::StopDrawing:
			polyLineControler->removePolyLine();
			break;
		case OptionName::Line:
			polyLineControler->startAddingLines();
			break;
		case OptionName::Arc:
			polyLineControler->startAddingArcs();
			break;
		case OptionName::Undo:
			historyHandler->undo();
			break;
		case OptionName::Redo:
			historyHandler->redo();
			break;
		}
	}




	// PolyLineControler

	PolyLineControler* MainMenu::polyLineControler = nullptr;

	PolyLineControler::PolyLineControler(HistoryHandler& historyHandler)
		:	currentPolyLine(),
			addArc(),
			addLine(),
			addNodeFunctor(&addLine),
			actualizeArc(),
			actualizeLine(),
			actualize(&actualizeLine),
			historyHandler(historyHandler)
	{
		historyHandler.setPolylineControler(this);
	}


	bool PolyLineControler::startAddingArcs()
	{
		if (!polyLineIsAttached())
		{
			actualize = &actualizeLine;
			addNodeFunctor = &addLine;
			return false;
		}

		if (currentPolyLine->lastNodeIndex() == 0)		// if last node is first node adding arc is impossible
		{
			actualize = &actualizeLine;
			addNodeFunctor = &addLine;
			return false;			
		}

		actualize = &actualizeArc;
		addNodeFunctor = &addArc;
		return true;
	}

	bool PolyLineControler::startAddingLines()
	{
		actualize = &actualizeLine;
		addNodeFunctor = &addLine;
		return true;
	}

	void PolyLineControler::setArcAproximationAccuracy(unsigned int accuracy) { this->arcApproximationAccuracy = accuracy; }
	
	bool PolyLineControler::polyLineIsAttached()
	{
		return (currentPolyLine != nullptr);
	}

	void PolyLineControler::removePolyLine()
	{
		startAddingLines();
		currentPolyLine.reset();
	}


	void PolyLineControler::actualizePolyLine(Point<double>& mousePosition, WindowHandler& windowHandler)
	{
		if (polyLineIsAttached())
		{
			try
			{
				currentPolyLine->unBlockDisplayNode();
				(*actualize)(mousePosition, currentPolyLine);
			}
			catch(...)
			{ }
		}
	}


	void PolyLineControler::addNode(Point<double>& point)
	{
		if (polyLineIsAttached())
		{
			currentPolyLine->blockDisplayNode();
			auto newEvent = Event(point, *addNodeFunctor);
			currentPolyLine->blockDisplayNode();
			(*addNodeFunctor)(point, currentPolyLine);
			historyHandler.addEvent(newEvent);

		}
		else
		{
			auto newPolyline = make_unique<PolyLine>(point);
			currentPolyLine.swap(newPolyline);					// current polyline is set to just created newPolyline
		}
	}


	void PolyLineControler::redoNode(AddNodeFunctor& addNode, Point<double>& point)
	{
		if (polyLineIsAttached())
		{
			addNode(point, currentPolyLine);
		}
		else
		{
			auto newPolyline = make_unique<PolyLine>(point);
			currentPolyLine.swap(newPolyline);					// current polyline is set to just created newPolyline
		}
	}


	void PolyLineControler::removeNode()
	{
		if (polyLineIsAttached())
		{
			bool nodeRemoved = currentPolyLine->removeLastNode();

			if(!nodeRemoved)
				currentPolyLine.reset(nullptr);					// if there's only one node left, the whole polyline is going to be removed
		}
	}


	void PolyLineControler::generateVertexChain(VertexChain<double>& vertexChain)
	{
		if (polyLineIsAttached())
			currentPolyLine->generateVertexChain(vertexChain, arcApproximationAccuracy);
	}


	void PolyLineControler::generatePeakPoints(vector<Point<double>>& peakPoints)
	{
		if (polyLineIsAttached())
			currentPolyLine->generatePeakPoints(peakPoints);
	}




	// Event
	
	Event::Event(Point<double>& point, AddNodeFunctor& addFunctor)
		:	point(point),
			addNodeFunctor()
	{	
		addNodeFunctor = unique_ptr<AddNodeFunctor>(addFunctor.copy());
	}


	Event::Event(const Event& eventToCopy )
		:	point(eventToCopy.point)
	{
		addNodeFunctor = unique_ptr<AddNodeFunctor>(eventToCopy.addNodeFunctor->copy());
	}


	Event& Event::operator=(const Event&  eventToCopy)
	{
		addNodeFunctor = unique_ptr<AddNodeFunctor>(eventToCopy.addNodeFunctor->copy());
		return *this;
	}


	void Event::undo(PolyLineControler& polyLineControler)
	{
		polyLineControler.removeNode();
	}


	void Event::redo(PolyLineControler& polyLineControler)
	{
		polyLineControler.redoNode(*addNodeFunctor, point);
	}



	// History Handler

	HistoryHandler* MainMenu::historyHandler = nullptr;

	
	HistoryHandler::HistoryHandler()
		:	events()
	{	}

	
	void HistoryHandler::setPolylineControler(PolyLineControler* polyLineContrl) { polyLineControler = polyLineContrl; }

	
	bool HistoryHandler::canUndo()
	{
		return (currentEventIndex >= 0);
	}

	
	bool HistoryHandler::canRedo()
	{
		int lastEventIndex = events.size() - 1;
		return (currentEventIndex != lastEventIndex);
	}

	
	void HistoryHandler::undo()
	{
		if (canUndo())
		{
			Event& currentEvent = events[currentEventIndex];
			currentEvent.undo(*polyLineControler);
			currentEventIndex--;
		}
	}

	
	void HistoryHandler::redo()
	{
		if (canRedo())
		{
			currentEventIndex++;
			Event& currentEvent = events[currentEventIndex];
			currentEvent.redo(*polyLineControler);
		}
	}

	
	void HistoryHandler::addEvent(Event& currentEvent)
	{
		events.push_back(currentEvent);
		currentEventIndex++;

		int newSize = currentEventIndex + 1;
		events.resize(newSize);
	}




	// WindowHandler

	WindowHandler::WindowHandler(Size<unsigned int>& windowSize, Color& backgroundColor, Color& polyLineColor, Color& peakPointColor)
		:	windowSize(windowSize.width, windowSize.height),
			windowOrginalSize((double)windowSize.width, (double)windowSize.height),
			centerOfScreen(windowSize.width / 2, windowSize.width / 2),
			backgroundColor(backgroundColor),
			polyLineColor(polyLineColor),
			peakPointColor(peakPointColor)
	{	}


	void WindowHandler::translateVertexChain(VertexChain<double>& vertexChain)
	{
		auto& vertexes = vertexChain.getVertexes();
		for (auto& vertex : vertexes)
			translateModelToScreen(vertex);
	}


	void WindowHandler::tanslateSetOfPoints(vector<Point<double>>& points)
	{
		for (auto& point : points)
			translateModelToScreen(point);
	}


	void WindowHandler::translateModelToScreen(Point<double>& point)
	{
		point.x = windowOrginalSize.height / windowSize.width * point.x;	// its windowOrginalSize.height because height is the reference value if the oryginal screen size is not 1:1
		point.y = windowOrginalSize.height / windowSize.height * point.y;
	}


	Point<double> WindowHandler::translateToModel(Point<unsigned int>& cursorPosition)
	{
		double x = static_cast<double>(cursorPosition.x);
		double y = static_cast<double>(cursorPosition.y);

		x -= centerOfScreen.x;
		y -= centerOfScreen.y;

		x *= 1 / centerOfScreen.x;;
		y *= -1 / centerOfScreen.y;

		x *= (windowSize.width - windowSize.width / 2) / (windowOrginalSize.height - windowOrginalSize.height / 2);	// its windowOrginalSize.height because height is the reference value if the oryginal screen size is not 1:1
		y *= (windowSize.height - windowSize.height / 2) / (windowOrginalSize.height - windowOrginalSize.height / 2);

		return Point<double>(x, y);
	}


	void WindowHandler::displayScreen(PolyLineControler& polyLineControler)
	{
		glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);	// background color

		glClear(GL_COLOR_BUFFER_BIT);

		displayVertexes(polyLineControler);
		displayPeakPoints(polyLineControler);

		glFlush();
		glutSwapBuffers();
	}

	
	void WindowHandler::displayVertexes(PolyLineControler& polyLineControler)
	{
		VertexChain<double> vertexChain;
		polyLineControler.generateVertexChain(vertexChain);
		translateVertexChain(vertexChain);

		auto vertexes = vertexChain.getVertexes();

		glColor3f(polyLineColor.r, polyLineColor.g, polyLineColor.b);
		glBegin(GL_LINE_STRIP);

		for (auto vertex : vertexes)
			glVertex2d(vertex.x, vertex.y);

		glEnd();
	}


	void WindowHandler::displayPeakPoints(PolyLineControler& polyLineControler)
	{
		vector<Point<double>> peakPoints;
		polyLineControler.generatePeakPoints(peakPoints);
		tanslateSetOfPoints(peakPoints);

		glColor3f(peakPointColor.r, peakPointColor.g, peakPointColor.b);
		glPointSize(5);
		glBegin(GL_POINTS);

		for (auto point : peakPoints)
			glVertex2d(point.x, point.y);

		glEnd();
	}


	void WindowHandler::resize(Size<int>& newSize)
	{
		windowSize = newSize;
		centerOfScreen.x = newSize.width / 2;
		centerOfScreen.y = newSize.height / 2;
	}





	// Controler

	Controler* Controler::appControler = nullptr;

	Controler::Controler(Size<unsigned int>& windowSize, string& windowTitle, Color& backgroundColor, Color& polyLineColor, Color& peakPointColor)
		:	historyHandler(),
			polyLineControler(historyHandler),
			windowHandler(windowSize, backgroundColor, polyLineColor, peakPointColor),
			menu(&polyLineControler, &historyHandler)
	{
		Controler::appControler = this;

		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
		glutInitWindowSize(windowSize.width, windowSize.height);
		glutCreateWindow(windowTitle.c_str());
		initializeGlutCallbacks();
		menu.initializeMenu();
	}


	void Controler::initializeGlutCallbacks()
	{
		glutDisplayFunc(getDisplayFunction());
		glutIdleFunc(getDisplayFunction());
		glutReshapeFunc(getWindowResizeCallback());
		glutPassiveMotionFunc(getOnMouseMoveCallback());
		glutMouseFunc(getOnMouseClickCallback()); 
	}


	Controler::~Controler()
	{
		Controler::appControler = nullptr;
	}




	// On Mouse Move event

	onMouseMoveCallback Controler::getOnMouseMoveCallback()
	{
		return Controler::onMouseMoveFunction;
	}


	void Controler::onMouseMoveFunction(int x, int y)
	{
		auto mousePoistion = Point<unsigned int>(x, y);
		if(Controler::appControler != nullptr)
			appControler->onMouseMove(mousePoistion);
	}


	void Controler::onMouseMove(Point<unsigned int>& mousePosition)
	{
		auto mousePositionMapped = windowHandler.translateToModel(mousePosition);
		polyLineControler.actualizePolyLine(mousePositionMapped, windowHandler);
	}


	// On Mouse Click event

	onMouseClickCallback Controler::getOnMouseClickCallback()
	{
		return Controler::onMouseClickFunction;
	}

	void Controler::onMouseClickFunction(int button, int state, int x, int y)
	{
		auto mousePosition = Point<unsigned int>(x, y);

		if (appControler != nullptr)
		{
			if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
				appControler->onMouseClick(mousePosition);
		}
	}

	void Controler::onMouseClick(Point<unsigned int>& mousePosition)
	{
		auto mousePositionMapped = windowHandler.translateToModel(mousePosition);
		polyLineControler.addNode(mousePositionMapped);	
	}


	// display event

	displayCallback Controler::getDisplayFunction()
	{
		return Controler::displayFunction;
	}

	void Controler::displayFunction()
	{
		if (appControler)
			appControler->windowHandler.displayScreen(appControler->polyLineControler);
	}
	


	// window resize event

	onWindowResizeCallback Controler::getWindowResizeCallback()
	{
		return Controler::onWindowResizeFunction;
	}


	void Controler::onWindowResizeFunction(int width, int height)
	{
		if (appControler)
		{
			glViewport(0,0,width, height);
			glClear(GL_COLOR_BUFFER_BIT);
			glFlush();
			glutSwapBuffers();
			auto newWindowSize = Size<int>(width, height);
			appControler->onResize(newWindowSize);
			displayFunction();
		}
	}


	void Controler::onResize(Size<int>& newSize)
	{
		windowHandler.resize(newSize);
	}
}