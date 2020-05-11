#pragma once
#include <vector>
#include <memory>
#include <cmath>

namespace primitives
{
	using std::vector;
	using std::unique_ptr;
	using std::make_unique;


	class VerticalLine;
	class HorisontalLine;
	class Line;


	const double pi = 3.14159265358979323846;


	template<class T>
	struct Size
	{
		T width, height;
		Size(T w, T h) : width(w), height(h) {}
	};

	struct Color
	{
		float  r, g, b, a;
		Color(float r, float g, float b, float a = 1) : r(r), g(g), b(b), a(a) {}
	};

	struct Radians;

	struct Degrees
	{
		double value;
		Degrees(Radians& rad);

	};



	// radians are defined for avoiding degree to radians errors

	struct Radians
	{		
		double value;

		void shortAngle();			// turns 390* to 360* (in radians)
		Radians();					// sets the 360* angle
		Radians(double value);		// sets value from radians
		Radians(Degrees& deg);		// sets angle from degrees 			
		
		double sinus();
		double cosinus();
		friend bool operator == ( Radians& first,  Radians& last);
		friend bool operator == ( Radians& first,  double last);
		friend bool operator > ( Radians& first,  Radians& last);
		friend bool operator > ( Radians& first, double last);
		friend bool operator >= ( Radians& first,  Radians& last);
		friend bool operator >= ( Radians& first, double last);
		friend bool operator < ( Radians& first,  Radians& last);
		friend bool operator < ( Radians& first, double last);
		friend bool operator <= ( Radians& first,  Radians& last);
		friend bool operator <= ( Radians& first, double last);
		friend Radians operator+(const Radians& first, const Radians& last);
		friend Radians operator-(const Radians& first, const Radians& last);
		friend Radians operator*(const int first, const Radians& last);
		friend Radians operator*(const Radians& first, const Radians& last);
		friend Radians operator/(const Radians& first, const Radians& last);
		void operator = (const Radians& rad);
		void operator += (const Radians& rad);
		void operator -= (const Radians& rad);
		void operator *= (const Radians& rad);
		void operator /= (const Radians& rad);
		void operator = (const double rad);
		void operator += (const double rad);
		void operator -= (const double rad);
		void operator *= (const double rad);
		void operator /= (const double rad);
		
	};

	


	template<class>
	struct Vector;

	template<class T>
	struct Point
	{
		T x, y;
		Point() = default;
		Point(T x, T y) : x(x), y(y) {}
		Point(Vector<T>& vector) : x(vector.x), y(vector.y) {}
		void move(Vector<T>& vect)									// moves point by value of given vector
		{ 
			this->x += vect.x;
			this->y += vect.y; 
		}
	};



	template<class T>
	struct Vector
	{
		T x, y;
		Vector(T x, T y) : x(x), y(y) {}
		Vector(Point<T>& point):x(point.x), y(point.y) {}										// sets vector from zero point of coordinates
		Vector(Point<T>& first, Point<T>& last) : x(last.x - first.x), y(last.y - first.y) {}	// creates vector from 2 given points
		void rotate(Radians angle)																// rotates vector of given angle
		{
			if (angle < 0) angle = (2 * pi) + angle;

			T sinus = static_cast<T>(angle.sinus());
			T cosinus = static_cast<T>(angle.cosinus());
			T xCopy = x;
			T yCopy = y;
			x = xCopy * cosinus - yCopy * sinus;
			y = xCopy * sinus + yCopy * cosinus;
		}

		Radians getAngle()											// returns angle between vector and x coordinate (from 0 to 2*pi, values rise counterclockwise)
		{
			double angle = atan2(y, x);
		
			
			if (angle < 0) angle = (2 * pi) + angle;
			
			return Radians(angle);
		}

		inline double getLength()									// returns length of vector
		{
			return sqrt((x*x) + (y*y));
		}
	};



	template<class T>
	struct Section
	{
		Point<T>& begin;		// points are thought as references because this class is designed to be temporary
		Point<T>& end;

		Section(Point<T>& first, Point<T>& last) :begin(first), end(last) {}
		Point<T> getMiddlePoint()													// returns the point on the middle of givensection
		{
			Point<T> point;
			point.x = static_cast<T>((begin.x + end.x) / 2);
			point.y = static_cast<T>((begin.y + end.y) / 2);
			return point;
		}

		bool isVertical() { return(begin.x == end.x); }
		bool isHorisontal() { return(begin.y == end.y); }

	};


	// Line has to be polymorphic because vertical line is not a function so at least it has to be treated separately
	// horisontal line was separated as well, to avoid dividing by 0 (a parameter = 0)
	class LineInterface
	{
	public:
		static unique_ptr<LineInterface> createLine(Section<double>& section);							// method that creates lines							
		virtual void moveThrough(Point<double>&) = 0;													// move the line through specific point
		virtual bool isVertical() = 0;
		virtual bool isHorisontal() = 0;
		virtual bool isParallel(LineInterface& line) = 0;
		virtual ~LineInterface() = default;
		virtual bool getIntersectionPoint(LineInterface& anotherLine, Point<double>& point) = 0;		// finds intersection point and sets it to Point<T> point, returns true if there is intersection
		virtual unique_ptr<LineInterface> setPerpendicular(Point<double> point) = 0;					// returns the line that is perpendicular to the given line and movesit through point
	};



	class VerticalLine
		: public LineInterface
	{
		double x;

	public:
		VerticalLine() = default;
		VerticalLine(Section<double>& section);
		inline double getX();
		bool isVertical() override;
		bool isHorisontal() override;
		bool isParallel(LineInterface& line) override;
		void moveThrough(Point<double>& point) override;
		bool getIntersectionPoint(LineInterface& anotherLine, Point<double>& point) override;
		unique_ptr<LineInterface> setPerpendicular(Point<double> point) override;
	};





	class HorisontalLine
		: public LineInterface
	{

		double y;

	public:
		HorisontalLine() = default;
		HorisontalLine(Section<double>& section);
		inline double getY();

		bool isVertical() override;
		bool isHorisontal() override;
		bool isParallel(LineInterface& line) override;
		void moveThrough(Point<double>& point) override;
		bool getIntersectionPoint(LineInterface& anotherLine, Point<double>& point) override;
		unique_ptr<LineInterface> setPerpendicular(Point<double> point) override;
	};



	class Line
		: public LineInterface
	{


		double a, b;			// y = a x + b
	public:
		Line() = default;
		Line(double a, double b);
		inline double getY(double x);
		inline double getX(double y);
		inline double getA();
		inline double getB();
	
		bool isVertical() override;
		bool isHorisontal() override;
		bool isParallel(LineInterface& line) override;

		Line(Section<double>& section);
		Line(Point<double>& beginPoint, Point<double>& endPoint);
		inline void setOnSection(Section<double>& section);
		void moveThrough(Point<double>& point) override;
		bool getIntersectionPoint(LineInterface& anotherLine, Point<double>& point) override;
		unique_ptr<LineInterface> setPerpendicular(Point<double> point) override;
	};




	template<class T>
	class VertexChain
	{
		vector<Point<T>> vertexes;
	public:
		VertexChain() : vertexes() {}
		inline vector<Point<T>>& getVertexes() { return vertexes; }
		void add(Point<T> point) { vertexes.push_back(point); }
		void operator+=(Point<T> point)	{ vertexes.push_back(point); }
	};




	// Arc is polymorphic, there are two types of arcs clockwise and countrclockwise
	class Arc
	{
	protected:

		Point<double> center;
		double radius;
		Radians beginPointAngle;
		Radians endPointAngle;

		inline Radians divide(unsigned int sections);					// divides circles on sections
		inline Vector<double> setBeginVector();							// returns vector rotated of beginPointAngle
		void moveVertex(unique_ptr<VertexChain<double>>& vertexChain);	// circle is calculated as it was found in the middlepoint of coordinates. This method moves th circle in place where it should find
	public:
		Arc(Radians beginPointAngle, Radians endPointAngle, Point<double> center, double radius);
		Arc(Point<double> beginPoint, Point<double> endPoint, Point<double> center);
		virtual ~Arc() = default;

		Point<double> getCenterPoint();									// returns the center of circle
		inline Radians getBeginPointAngle();							// gets the angle between x coordinate and point
		inline Radians getEndPointAngle();								// gets the angle between x coordinate and point
		virtual bool isCounterClockWise() = 0;
		virtual Point<double> getPeakPoint() = 0;						// returns this circle peak point
		virtual void generateVertexes(VertexChain<double>& vertexChain, unsigned int sections) = 0;		// generates vertexes and sets it in vertexChain, so they can be displayed
	};


	class ClockWiseArc
		: public Arc
	{
	public:
		ClockWiseArc(Point<double> beginPoint, Point<double> endPoint, Point<double> center);
		bool isCounterClockWise() override;
		Point<double> getPeakPoint() override;
		void generateVertexes(VertexChain<double>& vertexChain, unsigned int sections) override;
	};


	class CounterClockWiseArc
		: public Arc
	{
	public:
		CounterClockWiseArc(Point<double> beginPoint, Point<double> endPoint, Point<double> center);
		bool isCounterClockWise() override;
		Point<double> getPeakPoint() override;
		void generateVertexes(VertexChain<double>& vertexChain, unsigned int sections) override;
	};
}