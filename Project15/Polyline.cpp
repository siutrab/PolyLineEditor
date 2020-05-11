#include "Polyline.h"


namespace obj
{
	// Node

	Node::Node(Point<double>& beginPoint)
		:	endPoint(beginPoint)
	{	}
	
	Point<double> Node::getEndPoint() { return this->endPoint; }
	



	//FirstNode
	
	bool FirstNode::isArc() { return false; }
	bool FirstNode::isFirstNode() { return true; }
	bool ArcNode::isFirstNode() { return false; }
	FirstNode::FirstNode(Point<double>& point)
		: Node(point)
	{	}

	void FirstNode::generateVertexChain(VertexChain<double>& vertexChain, unsigned int accuracy)
	{
		vertexChain.add(endPoint);
	}
	



	// LineNode
	
	bool LineNode::isArc() { return false; }

	LineNode::LineNode(Point<double> newPoint)
		:	Node(newPoint)
	{	}

	bool LineNode::isFirstNode() { return false; }

	void LineNode::generateVertexChain(VertexChain<double>& vertexChain, unsigned int accuracy)
	{
		vertexChain.add(endPoint);
	}





	// ArcNode

	bool ArcNode::isArc() { return true; }


	Point<double> ArcNode::arcCenter() { return arc->getCenterPoint(); }


	void ArcNode::generateVertexChain(VertexChain<double>& vertexChain, unsigned int accuracy)
	{
		arc->generateVertexes(vertexChain, accuracy);
	}

	void ArcNode::generatePeakPoints(vector<Point<double>>& points)
	{
		auto peakPoint = arc->getPeakPoint();
		points.push_back(peakPoint);
	}


	Point<double> ArcNode::getPeakPoint() { return arc->getPeakPoint(); }

	ArcNode::ArcNode(Node& previousNode, PolyLine* polyLine, Point<double> newPoint)
		:	Node(newPoint)
	{
		if (previousNode.isFirstNode()) throw std::exception();

		auto lastNodeIndex = polyLine->lastNodeIndex();
		auto previousNodeEndPt = polyLine->getNodeAt(lastNodeIndex).getEndPoint();
		auto previousNodeBeginPt = polyLine->getNodeAt(lastNodeIndex - 1).getEndPoint();

		if (previousNode.isArc())
		{
			ArcNode& previousArc = dynamic_cast<ArcNode&>(previousNode);
			
			auto arcCenter = previousArc.arcCenter();
			auto newArc = createArcAfterArc(arcCenter, previousNodeEndPt, newPoint, previousNode);

			arc.swap(newArc);
		}
		else
		{
			auto newArc = createArcAfterSection(previousNodeBeginPt, previousNodeEndPt, newPoint);

			arc.swap(newArc);
		}
	}


	unique_ptr<Arc> ArcNode::createArcAfterSection(Point<double>& previousNodeBeginPt, Point<double>& previousNodeEndPt, Point<double>& newPoint)
	{
		auto previousSection = Section<double>(previousNodeBeginPt, previousNodeEndPt);
		auto previousLine = LineInterface::createLine(previousSection);

		auto radiusLine = previousLine->setPerpendicular(previousNodeEndPt);

		unique_ptr<LineInterface> circleAxisLine;
		setAxisLine(previousNodeEndPt, newPoint, circleAxisLine); 

		Point<double> arcCenter;
		if (radiusLine->getIntersectionPoint(*circleAxisLine, arcCenter))			
			if(arcWillBeClockWiseAfterSection(previousNodeBeginPt, previousNodeEndPt, arcCenter))
				return make_unique<ClockWiseArc>(previousNodeEndPt, newPoint, arcCenter);
			else 
				return make_unique<CounterClockWiseArc>(previousNodeEndPt, newPoint, arcCenter);
		else
			throw std::exception();
	}

	bool ArcNode::arcWillBeClockWiseAfterSection(Point<double>& previousNodeBeginPt, Point<double>& arcFirstPoint, Point<double>& arcCenter)
	{
		auto arcRadius = Vector<double>(arcCenter, arcFirstPoint);
		auto angle = arcRadius.getAngle();

		auto hypotenuse = Vector<double>(arcCenter, previousNodeBeginPt);
		hypotenuse.rotate(-1 * angle);

		return (hypotenuse.y > 0);
	}

	unique_ptr<Arc> ArcNode::createArcAfterArc(Point<double>& previousNodeCenter, Point<double>& previousNodeEndPt, Point<double>& newPoint, Node& previousNode)
	{
		unique_ptr<LineInterface> axisLine;
		setAxisLine(previousNodeEndPt, newPoint, axisLine);
			
		auto radiusSection = Section<double>(previousNodeCenter, previousNodeEndPt);
		auto radiusLine = LineInterface::createLine(radiusSection);
		
		ArcNode& previousNodeAsArc = dynamic_cast<ArcNode&>(previousNode);
		Point<double> prewviousNodePeakPoint = previousNodeAsArc.getPeakPoint();

		Point<double> arcCenter;
		if (axisLine->getIntersectionPoint(*radiusLine, arcCenter))
			if (arcWillBeClockWiseAfterArc(previousNodeCenter, prewviousNodePeakPoint, previousNodeEndPt, newPoint))
				return make_unique<ClockWiseArc>(previousNodeEndPt, newPoint, arcCenter);		
			else return make_unique<CounterClockWiseArc>(previousNodeEndPt, newPoint, arcCenter);
		else
			throw std::exception();
	}

	bool ArcNode::arcWillBeClockWiseAfterArc(Point<double>& previousNodeCenter, Point<double>& previousNodePeak, Point<double>& arcFirstPoint, Point<double>& newPoint)
	{
		auto previousArcRadius = Vector<double>(previousNodeCenter, arcFirstPoint);
		auto newPointVector = Vector<double>(arcFirstPoint, newPoint);
		auto centerPeakVector = Vector<double>(previousNodeCenter, previousNodePeak);

		auto radiusRotation = previousArcRadius.getAngle();
		auto angleToRotateBack = -1 * radiusRotation;
		newPointVector.rotate(angleToRotateBack);
		centerPeakVector.rotate(angleToRotateBack);

		if (centerPeakVector.y > 0)
		{
			bool clockWise = (newPointVector.x < 0);
			return clockWise;
		}
		else
		if(centerPeakVector.y < 0)
		{
			bool clockWise = (newPointVector.x > 0);
			return clockWise;
		}
	}

	void ArcNode::setAxisLine(Point<double>& beginPoint, Point<double>& endPoint, unique_ptr<LineInterface>& axisLine)
	{
		auto newSection = Section<double>(beginPoint, endPoint);
		auto axisPoint = newSection.getMiddlePoint();

		auto line = LineInterface::createLine(newSection);
		if (line)
		{
			auto newAxisLine = line->setPerpendicular(axisPoint);
			axisLine.swap(newAxisLine);
		}
	}





	// PolyLine

	PolyLine::PolyLine(Point<double>& point)
		: nodes()
	{
		unique_ptr<Node> firstNode = make_unique<FirstNode>(point);
		nodes.push_back(move(firstNode));
	}


	PolyLine::~PolyLine()
	{	}

	void PolyLine::blockDisplayNode() { displayNodeBlocked = true; }
	void PolyLine::unBlockDisplayNode() { displayNodeBlocked = false; }


	bool PolyLine::addDisplayLineNode(Point<double>& point)
	{
		if (!displayNodeBlocked)
		{
			unique_ptr<Node> newDisplayNode = make_unique<LineNode>(point);
			displayNode.swap(newDisplayNode);
			return true;
		}
		return false;
	}


	bool PolyLine::addLine(Point<double>& point)
	{
		try
		{
			displayNode.reset();
			unique_ptr<Node> newNode = make_unique<LineNode>(point);	
			nodes.push_back(move(newNode));

			return true;
		}
		catch (...)
		{
			return false;
		}
	}
	
	bool PolyLine::addDisplayArcNode(Point<double>& point)
	{
		if (!displayNodeBlocked)
		{
			auto& lastNode = nodes.back();
			unique_ptr<Node> newDisplaNode = make_unique<ArcNode>(*lastNode, this, point);
			displayNode.swap(newDisplaNode);

			return true;
		}
		return false;
	}

	bool PolyLine::addArc(Point<double>& point)
	{
		try
		{
			if (nodes.back()->isFirstNode()) return false;		// arc cannot be made from first node

			auto& lastNode = nodes.back();
			unique_ptr<Node> newArcNode = make_unique<ArcNode>(*lastNode, this, point);
			nodes.push_back(move(newArcNode));

			return true;
		}
		catch (...)
		{
			displayNodeBlocked = false;
			return false;
		}
	}

	bool PolyLine::removeLastNode()
	{
		displayNodeBlocked = true;
		displayNode.reset();

		if (nodes.back()->isFirstNode()) return false;			// first node cannot be removed
		
		nodes.pop_back();
		displayNodeBlocked = false;
		return true;
	}

	void PolyLine::generateVertexChain(VertexChain<double>& vertexChain, unsigned int accuracy)
	{
		for (auto& node : nodes)
			node->generateVertexChain(vertexChain, accuracy);

		if((!displayNodeBlocked) && displayNode)
			displayNode->generateVertexChain(vertexChain, accuracy);
	}

	
	void PolyLine::generatePeakPoints(vector<Point<double>>& peakPoints)
	{
		for (auto& node : nodes)
			node->generatePeakPoints(peakPoints);
		if ((!displayNodeBlocked) && displayNode)
			displayNode->generatePeakPoints(peakPoints);
	}
	
	Node& PolyLine::getNodeAt(unsigned int index)
	{
		return *nodes[index];
	}

	unsigned int PolyLine::lastNodeIndex()
	{
		return (nodes.size() - 1);
	}
}