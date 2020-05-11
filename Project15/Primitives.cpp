#include "Primitives.h"


namespace primitives
{
	// Degrees

	Degrees::Degrees(Radians& rad)
		:	value(rad.value * 180 / pi)
	{	}


	// Radians
	
	void Radians::shortAngle()					// turns 390* to 360* (in radians)
	{
		if (value < 0) value += 2 * pi;
		while (value > (2 * pi))
			value -= (2 * pi);
	}


	Radians::Radians()							// sets the 360* angle
		: value(2 * pi)
	{
		shortAngle();
	}
	
	
	Radians::Radians(double value)
		: value(value)
	{
		shortAngle();
	}


	Radians::Radians(Degrees& deg)				// sets angle from degrees 
		: value(deg.value * pi / 180)
	{
		shortAngle();
	}

	double Radians::sinus() { return sin(value); }
	double Radians::cosinus() { return cos(value); }
	bool operator == (Radians& first, Radians& last) 
	{ 
		first.shortAngle();
		last.shortAngle();
		return (first.value == last.value); 
	}
	bool operator == (Radians& first, double last) 
	{
		first.shortAngle();
		return (first.value == last);
	}
	bool operator > (Radians& first, Radians& last) 
	{ 
		first.shortAngle();
		last.shortAngle();
		return (first.value > last.value); 
	}
	bool operator > (Radians& first, double last)
	{
		first.shortAngle();
		return (first.value > last);
	}
	bool operator >= (Radians& first, Radians& last) 
	{
		first.shortAngle();
		last.shortAngle();
		return (first.value >= last.value); 
	}
	bool operator >= (Radians& first, double last)
	{
		first.shortAngle();
		return (first.value >= last);
	}
	bool operator < (Radians& first, Radians& last) 
	{
		first.shortAngle();
		last.shortAngle();
		return (first.value < last.value); 
	}
	bool operator < (Radians& first, double last)
	{
		first.shortAngle();
		
		return (first.value < last);
	}
	bool operator <= (Radians& first, Radians& last) 
	{
		first.shortAngle();
		last.shortAngle();
		return (first.value <= last.value); 
	}
	bool operator <= (Radians& first, int last)
	{
		first.shortAngle();
		return (first.value <= last);
	}

	Radians operator+(const Radians& first, const Radians& last)
	{
		return Radians(first.value + last.value);
	}
	Radians operator-(const Radians& first, const Radians& last)
	{
		return Radians(first.value - last.value);
	}
	Radians operator*(const Radians& first, const Radians& last)
	{
		return Radians(first.value * last.value);
	}
	Radians operator*(const int first, const Radians& last)
	{
		return Radians(first * last.value);
	}
	Radians operator/(const Radians& first, const Radians& last)
	{
		return Radians(first.value / last.value);
	}
	void Radians::operator = (const Radians& rad)
	{
		value = rad.value;
		shortAngle();
	}

	void Radians::operator += (const Radians& rad) 
	{ 
		value += rad.value;
		shortAngle();
	}
	void Radians::operator -= (const Radians& rad) 
	{ 
		value -= rad.value;
		shortAngle();
	}
	void Radians::operator *= (const Radians& rad) 
	{ 
		value *= rad.value; 
		shortAngle();
	}
	void Radians::operator /= (const Radians& rad) 
	{ 
		value /= rad.value;
		shortAngle();
	}
	void Radians::operator = (const double rad) 
	{ 
		value = rad; 
		shortAngle();
	}
	void Radians::operator += (const double rad) 
	{ 
		value += rad;
		shortAngle();
	}
	void Radians::operator -= (const double rad) 
	{ 
		value -= rad; 
		shortAngle();
	}
	void Radians::operator *= (const double rad) 
	{ 
		value *= rad; 
		shortAngle();
	}
	void Radians::operator /= (const double rad) 
	{ 
		value /= rad; 
		shortAngle();
	}


	// Arc interface

	Radians Arc::divide(unsigned int sections)
	{
		Radians rad;
		return (rad.value /= sections);
	}

	Vector<double> Arc::setBeginVector()								// returns vector rotated of beginPointAngle
	{
		auto vector = Vector<double>(radius, 0);						// puts radius vertical, right on the x axis
		vector.rotate(beginPointAngle);
		return vector;
	}

	void Arc::moveVertex(unique_ptr<VertexChain<double>>& vertexChain)	// circle is calculated as it was found in the middlepoint of coordinates. This method moves th circle in place where it should find
	{
		auto vector = Vector<double>(center);
		for (Point<double>& vertex : vertexChain->getVertexes())
			vertex.move(vector);
	}

	Arc::Arc(Radians beginPointAngle, Radians endPointAngle, Point<double> center, double radius)
		: radius(radius),
		center(center),
		beginPointAngle(beginPointAngle),
		endPointAngle(endPointAngle)
	{	}

	Arc::Arc(Point<double> beginPoint, Point<double> endPoint, Point<double> center)
		: center(center)
	{
		auto beginPointAsVector = Vector<double>(center, beginPoint);
		beginPointAngle = beginPointAsVector.getAngle();

		auto endVectorAsVector = Vector<double>(center, endPoint);
		endPointAngle = endVectorAsVector.getAngle();
		radius = endVectorAsVector.getLength();
	}


	Point<double> Arc::getCenterPoint() { return center; }
	Radians Arc::getBeginPointAngle() { return beginPointAngle; }
	Radians Arc::getEndPointAngle() { return endPointAngle; }






	// ClockWiseArc

	ClockWiseArc::ClockWiseArc(Point<double> beginPoint, Point<double> endPoint, Point<double> center)
		: Arc(beginPoint, endPoint, center)
	{	}

	bool ClockWiseArc::isCounterClockWise() { return false; }
	void ClockWiseArc::generateVertexes(VertexChain<double>& vertexChain, unsigned int sections) 
	{
		auto centerVector = Vector<double>(center);
		Radians translatedEndPointRotation;
		translatedEndPointRotation = endPointAngle - beginPointAngle;	// both angles are now rotated: beginPointAngle = 0, endPointAngle got smaller;

		auto rotatingVector = setBeginVector();							// this vector point at beginPoint
		Radians rotation = 0;
		auto circleSection = divide(sections);

		rotatingVector.rotate(-1 * circleSection);
		rotation -= circleSection;

		while (rotation > translatedEndPointRotation)
		{
			Point<double> circlePoint = Point<double>(rotatingVector);
			circlePoint.move(centerVector);
			vertexChain.add(circlePoint);
			rotatingVector.rotate(-1 * circleSection);
			rotation -= circleSection;
		}

		auto lastPointVector = Vector<double>(radius, 0);
		lastPointVector.rotate(endPointAngle);
		auto lastPoint = Point<double>(lastPointVector);
		lastPoint.move(centerVector);
		vertexChain.add(lastPoint);
	}

	Point<double> ClockWiseArc::getPeakPoint()
	{
		Radians middleRotation;
		if (endPointAngle > beginPointAngle)
			middleRotation.value = (2 * pi - endPointAngle.value + beginPointAngle.value) / 2;
		else middleRotation.value = (beginPointAngle.value - endPointAngle.value) / 2;
		auto radiusVector = Vector<double>(radius, 0);
		radiusVector.rotate(beginPointAngle);
		radiusVector.rotate(-1 * middleRotation);

		auto peakPoint = Point<double>(radiusVector);
		auto centerVector = Vector<double>(center);
		peakPoint.move(centerVector);

		return peakPoint;
	}





	//CounterClockWiseArc

	CounterClockWiseArc::CounterClockWiseArc(Point<double> beginPoint, Point<double> endPoint, Point<double> center)
		: Arc(beginPoint, endPoint, center)
	{	}

	bool CounterClockWiseArc::isCounterClockWise() { return true; }

	void CounterClockWiseArc::generateVertexes(VertexChain<double>& vertexChain, unsigned int sections)
	{
		auto centerVector = Vector<double>(center);
		Radians transpiredEndPointRotation;
		transpiredEndPointRotation = endPointAngle - beginPointAngle;	// both angles are now rotated: beginPointAngle = 0, endPointAngle got smaller;

		auto rotatingVector = setBeginVector();		// this vector point at beginPoint
		Radians rotation = 0;						// rotatingVector.getAngle();
		auto circleSection = divide(sections);

		rotatingVector.rotate(circleSection);
		rotation += circleSection;
		
		while (rotation < transpiredEndPointRotation)
		{
			Point<double> circlePoint = Point<double>(rotatingVector);
			circlePoint.move(centerVector);
			vertexChain.add(circlePoint);
			rotatingVector.rotate(circleSection);
			rotation += circleSection;
		}

		auto lastPointVector = Vector<double>(radius, 0);
		lastPointVector.rotate(endPointAngle);
		auto lastPoint = Point<double>(lastPointVector);
		lastPoint.move(centerVector);
		vertexChain.add(lastPoint);
	}


	Point<double> CounterClockWiseArc::getPeakPoint()
	{
		Radians middleRotation;

		if (beginPointAngle > endPointAngle)
			middleRotation.value = (endPointAngle.value + 2 * pi - beginPointAngle.value) / 2;
		else middleRotation.value = (endPointAngle.value - beginPointAngle.value) / 2;

		auto radiusVector = Vector<double>(radius, 0);
		radiusVector.rotate(beginPointAngle);
		radiusVector.rotate(middleRotation);

		auto peakPoint = Point<double>(radiusVector);
		auto centerVector = Vector<double>(center);
		peakPoint.move(centerVector);

		return peakPoint;
	}




	// LineInterface

	unique_ptr<LineInterface> LineInterface::createLine(Section<double>& section)
	{
		
		unique_ptr<LineInterface> line;
		if (section.isVertical())
			return make_unique<VerticalLine>(section);

		if (section.isHorisontal())
			return make_unique<HorisontalLine>(section);

		return make_unique<Line>(section);
	}






	// setting lines perpendicular

	unique_ptr<LineInterface> VerticalLine::setPerpendicular(Point<double> point)
	{
		unique_ptr<LineInterface> perpendicularLine = make_unique<HorisontalLine>();
		perpendicularLine->moveThrough(point);
		return perpendicularLine;
	}

	unique_ptr<LineInterface> HorisontalLine::setPerpendicular(Point<double> point)
	{
		unique_ptr<LineInterface> perpendicularLine = make_unique<VerticalLine>();
		perpendicularLine->moveThrough(point);
		return perpendicularLine;
	}

	unique_ptr<LineInterface> Line::setPerpendicular(Point<double> point)
	{
		unique_ptr<LineInterface> perpendicularLine = make_unique<Line>((-1 / a), b);
		perpendicularLine->moveThrough(point);
		return perpendicularLine;
	}



	// VerticalLine

	VerticalLine::VerticalLine(Section<double>& section)
	{
		x = section.begin.x;
	}
	double VerticalLine::getX() { return x; }
	bool VerticalLine::isVertical() { return true; }
	bool VerticalLine::isHorisontal() { return false; }
	bool VerticalLine::isParallel(LineInterface& line) { return isVertical(); }

	void VerticalLine::moveThrough(Point<double>& point)
	{
		x = point.x;
	}

	bool VerticalLine::getIntersectionPoint(LineInterface& anotherLine, Point<double>& point)
	{
		if (anotherLine.isVertical())
			return false;

		if (anotherLine.isHorisontal())
		{
			HorisontalLine& line = dynamic_cast<HorisontalLine&>(anotherLine);
			point.x = x;
			point.y = line.getY();
			return true;
		}

		auto line = dynamic_cast<Line&>(anotherLine);
		point.x = x;
		point.y = line.getY(x);
		return true;
	}


	// HorisontalLine

	HorisontalLine::HorisontalLine(Section<double>& section) : y(section.begin.y) {}
	double HorisontalLine::getY() { return y; }
	bool HorisontalLine::isVertical() { return false; }
	bool HorisontalLine::isHorisontal() { return true; }
	bool HorisontalLine::isParallel(LineInterface& line) { return isHorisontal(); }

	void HorisontalLine::moveThrough(Point<double>& point) 
	{
		y = point.y;
	}

	bool HorisontalLine::getIntersectionPoint(LineInterface& anotherLine, Point<double>& point) 
	{
		if (anotherLine.isHorisontal())
			return false;

		if (anotherLine.isVertical())
		{
			auto line = dynamic_cast<VerticalLine&>(anotherLine);
			point.y = y;
			point.x = line.getX();
			return true;
		}

		auto line = dynamic_cast<Line&>(anotherLine);
		point.y = y;
		point.x = line.getX(y);
		return true;
	}




	// Line

	Line::Line(double a, double b) : a(a), b(b) {	}
	double Line::getY(double x) { return (a * x) + b; }
	double Line::getX(double y) { return (y - b) / a; }
	double Line::getA() { return a; }
	double Line::getB() { return b; }
	bool Line::isVertical() { return false; }
	bool Line::isHorisontal() { return false; }
	bool Line::isParallel(LineInterface& line) 
	{
		if ((line.isHorisontal()) || (line.isVertical())) return false;

		Line& skewLine = dynamic_cast<Line&>(line);
		return (skewLine.a == a);
	}

	Line::Line(Section<double>& section)
	{
		setOnSection(section);
	}

	Line::Line(Point<double>& beginPoint, Point<double>& endPoint)
	{
		auto section = Section<double>(beginPoint, endPoint);
		setOnSection(section);
	}

	void Line::setOnSection(Section<double>& section)
	{
		a = (section.end.y - section.begin.y) / (section.end.x - section.begin.x);
		b = section.begin.y - (a*section.begin.x);
	}


	void Line::moveThrough(Point<double>& point)
	{
		b = point.y - (a*point.x);
	}

	bool Line::getIntersectionPoint(LineInterface& anotherLine, Point<double>& point)
	{
		if (anotherLine.isHorisontal())
		{
			auto horisontalLine = dynamic_cast<HorisontalLine&>(anotherLine);
			point.y = horisontalLine.getY();
			point.x = getX(point.y);
			return true;
		}

		if (anotherLine.isVertical())
		{
			auto verticalLine = dynamic_cast<VerticalLine&>(anotherLine);
			point.x = verticalLine.getX();
			point.y = getY(point.x);
			return true;
		}

		if (this->isParallel(anotherLine)) return false;


		auto line = dynamic_cast<Line&>(anotherLine);

		point.x = (line.b - b) / (a - line.a);
		point.y = (point.x * line.a) + line.b;
		return true;
	}
}