#pragma once
#include "Primitives.h"
#include "Polyline.h"
#include "glut.h"
#include <memory>
#include <string>


namespace primitives
{
	template<class> struct Point;
	struct Color;
}

namespace polyline
{
	class PolyLine;
}




namespace controler
{ 
	using namespace primitives;
	using namespace obj;
	using std::make_unique;
	using std::unique_ptr;
	using std::move;
	using std::string;


	class WindowHandler;
	class PolyLineControler;
	class HistoryHandler;



	// this functors is called when its needed to edit polyline. It's used for plain edition as well as for "undo" "redo" operations
	struct AddNodeFunctor
	{
		virtual bool operator()(Point<double>&, unique_ptr<PolyLine>&) = 0;
		virtual AddNodeFunctor* copy() = 0;
		virtual ~AddNodeFunctor() = default;
	};

	struct AddLine
		: public AddNodeFunctor
	{
		AddLine() = default;
		~AddLine() = default;
		bool operator()(Point<double>& point, unique_ptr<PolyLine>& polyLine) override;
		AddNodeFunctor* copy() override;
	};

	struct AddArc
		: public AddNodeFunctor
	{
		AddArc() = default;
		~AddArc() = default;
		bool operator()(Point<double>& point, unique_ptr<PolyLine>& polyLine) override;
		AddNodeFunctor* copy() override;
	};

	// below functors are called when it's need to actualize the shape of polyline
	struct Actualize
	{
		virtual bool operator()(Point<double>&, unique_ptr<PolyLine>&) = 0;
	};

	struct ActualizeLine
		: public Actualize
	{
		ActualizeLine() = default;
		bool operator()(Point<double>& point, unique_ptr<PolyLine>& polyLine) override
		{
			return polyLine->addDisplayLineNode(point);
		}
	};

	struct ActualizArc
		: public Actualize
	{
		ActualizArc() = default;
		bool operator()(Point<double>& point, unique_ptr<PolyLine>& polyLine) override
		{
			return polyLine->addDisplayArcNode(point);
		}
	};
	


	struct Option
	{
		int index;
		const char* name;
		bool enabled;
	};


	class MainMenu
	{
		static PolyLineControler* polyLineControler;
		static HistoryHandler* historyHandler;

		vector<Option> options
		{
			{StopDrawing, "Stop drawing", false},
			{Line, "Line", true},
			{Arc, "Arc", false},
			{Undo, "Undo", false},
			{Redo, "Redo", false}
		};

	public:

		enum OptionName
		{
			StopDrawing,
			Line,
			Arc,
			Undo,
			Redo
		};

		MainMenu(PolyLineControler* polyLineControler, HistoryHandler* historyHandler);
		void initializeMenu();																// sets options in main menu
		void disableOption(OptionName option);												// disables option
		void enableOption(OptionName option);												// enables option
		static void chooseOption(int option);												// static method that is called by OpenGl after peaking menu option
	};




	class Event
	{
		unique_ptr<AddNodeFunctor> addNodeFunctor;
		Point<double> point;
	public:
		Event() = default;
		~Event() = default;
		Event(const Event&);
		Event& operator=(const Event& ) ;
		Event(Point<double>& point, AddNodeFunctor& addFunctor);
		void undo(PolyLineControler& polyLineControler);									// removes the last node
		void redo(PolyLineControler& polyLineControler);									// calls functor that was used for creating node that's wanted to be appear again
	};




	class HistoryHandler
	{
		PolyLineControler* polyLineControler;
		vector<Event> events;
		int currentEventIndex = -1;

	public:
		HistoryHandler();
		bool canUndo();													// returns true when undo operation is possible
		bool canRedo();													// returns true when redo operation is possible
		void undo();													// if possible calls undo method in current functor
		void redo();													// if possible calls redo method in current functor
		void addEvent(Event& currentEvent);								// adds event to collection, so it could be called again, when it's need to reuse it (redo operations)
		inline void setPolylineControler(PolyLineControler*);			// sets polyLineControler pointer
	};






	class PolyLineControler
	{
		AddLine addLine;
		AddArc addArc;
		AddNodeFunctor* addNodeFunctor;							// it points to addLine or addArc, depending on what type of node is wanted to be add

		ActualizeLine actualizeLine;
		ActualizArc actualizeArc;
		Actualize* actualize;									// it points to actualizeArc or actualizeLine. These functors are adding display node to polyline

		HistoryHandler& historyHandler;
		unique_ptr<PolyLine> currentPolyLine;
		unsigned int arcApproximationAccuracy = 64;				// approximation of arc. It's a number of vertxes in polygon that imitates an arc. If it's set to ex. 100, there would be 100 sections around whole 360 degree arc

		inline bool polyLineIsAttached();						// returns true when some polyline is attached to the class
	public:
		PolyLineControler(HistoryHandler& historyHandl);
		void removePolyLine();
		bool startAddingArcs();																	// sets addNodeFunctor for arcs
		bool startAddingLines();																// sets addNodeFunctor for lines
		void addNode(Point<double>& point);	
		void redoNode(AddNodeFunctor& addNode, Point<double>& point);							// called on redo event
		void removeNode();
		void actualizePolyLine(Point<double>& mousePosition, WindowHandler& windowHandler);		// sets the shape of polyline so it can be displayed
		void generateVertexChain(VertexChain<double>& vertexChain);								// generates the "multi xertex line" that would be displayed on screen
		void generatePeakPoints(vector<Point<double>>& peakPoints);								// generates dots on peek of every arc
		inline void setArcAproximationAccuracy(unsigned int accuracy);
	};






	class WindowHandler
	{
		Size<const double> windowOrginalSize;
		Size<int> windowSize;
		Point<double> centerOfScreen;											// it's double not int, because odd numbers would give incorrect result
		Color backgroundColor;
		Color polyLineColor;
		Color peakPointColor;
		
		inline void translateModelToScreen(Point<double>& point);				// translates model coordinates to screen coordinates
		void translateVertexChain(VertexChain<double>& vertexChain);			// translates model coordinates to screen coordinates
		void tanslateSetOfPoints(vector<Point<double>>& points);				// translates model coordinates to screen coordinates
		void displayVertexes(PolyLineControler& polyLineControler);				// displays collected vertexes (shape of polyline)
		void displayPeakPoints(PolyLineControler& polyLineControler);			// displays collected peak points of polylines arcs on screen
	public:
		WindowHandler(Size<unsigned int>& windowSize, Color& backgroundColor, Color& polyLineColor, Color& peakPointColor);
		inline void resize(Size<int>& newSize);									// resets class fields after the window resize event
		void displayScreen(PolyLineControler& polyLineControler);				// displays model to the screen
		Point<double> translateToModel(Point<unsigned int>& cursorPosition);	// translates cursor position to model coordinates
	};




	

	typedef void(*onMouseMoveCallback)(int, int);
	typedef void(*onMouseClickCallback)(int, int, int, int);
	typedef void(*displayCallback)(void);
	typedef void(*menuOptionCallback)(int);
	typedef void(*onWindowResizeCallback)(int, int);

	class Controler
	{
		// static members for openGl events handling
		// below methods are layers that connect openGl with OOP, so some methods have to be static, because there's a need for function pointer to be given
		static Controler* appControler;

		static void onMouseMoveFunction(int x, int y);
		void onMouseMove(Point<unsigned int>& mousePosition);

		static void onMouseClickFunction(int button, int state, int x, int y);
		void onMouseClick(Point<unsigned int>& mousePosition);

		static void onWindowResizeFunction(int width, int height);
		void onResize(Size<int>& newWindowSize);

		static void displayFunction();
		inline void initializeGlutCallbacks();
		// end of layer

		WindowHandler windowHandler;
		HistoryHandler historyHandler;
		PolyLineControler polyLineControler;
		MainMenu menu;

	public:
		Controler(Size<unsigned int>& windowSize, string& windowTitle, Color& backgroundColor, Color& polyLineColor, Color& peakPointColor);
		~Controler();

		displayCallback getDisplayFunction();					// getters for openGl mathods. Returns funtion pointers for methods handling openGl events
		onMouseMoveCallback getOnMouseMoveCallback();		
		onMouseClickCallback getOnMouseClickCallback();
		onWindowResizeCallback getWindowResizeCallback();
	};	
}

