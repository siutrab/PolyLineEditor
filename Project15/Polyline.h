#pragma once
#include "Primitives.h"
#include <vector>
#include <memory>



namespace obj
{
	using namespace primitives;
	using std::vector;
	using std::unique_ptr;
	using std::make_unique;

	class PolyLine;
	
	// PolyLine is made of nodes, that cam be Linear or Circular, so they're displayed and created in different ways
	class Node
	{
	protected:
		Point<double> endPoint;
	public:
		Node() = default;
		Node(Point<double>& newPoint);
		inline Point<double> getEndPoint();
		
		virtual inline bool isFirstNode() = 0;
		virtual inline bool isArc() = 0;
		virtual void generateVertexChain(VertexChain<double>& vertexChain, unsigned int accuracy) = 0;	// sets vertexes in vertex chain. Method is used to display polyline on screen. Accuracy is number of section for circle aproximation
		virtual void generatePeakPoints(vector<Point<double>>&) = 0;									// sets vector of points, that is used to display circles peak points
	};





	class FirstNode
		: public Node
	{
	public:
		FirstNode(Point<double>& point);

		inline bool isArc() override;
		inline bool isFirstNode() override;

		void generateVertexChain(VertexChain<double>& vertexChain, unsigned int accuracy) override;
		void generatePeakPoints(vector<Point<double>>&) override { return; };
	};


	
	class LineNode
		: public Node
	{

	public:
		LineNode(Point<double> newPoint);

		inline bool isArc() override;
		inline bool isFirstNode() override;	
		
		void generateVertexChain(VertexChain<double>& vertexChain, unsigned int accuracy) override;
		void generatePeakPoints(vector<Point<double>>&) override { return; }
	};




	class ArcNode
		: public Node
	{
		unique_ptr<Arc> arc;

		// arc's center is placed on the intersection of radius line (which is perpendicular to previous node) and axis node (that is perpendicular to new node)
		void setAxisLine(Point<double>& beginPoint, Point<double>& endPoint, unique_ptr<LineInterface>& axisLine);
		unique_ptr<Arc> createArcAfterSection(Point<double>& previousNodeBeginPt, Point<double>& previousNodeEndPt, Point<double>& newPoint);
		unique_ptr<Arc> createArcAfterArc(Point<double>& previousNodeCenter, Point<double>& previousSectionEndPoint, Point<double>& newSectionEndpoint, Node& previousNode);
		bool arcWillBeClockWiseAfterSection(Point<double>& previousNodeBeginPt, Point<double>& arcFirstPoint, Point<double>& arcCenter);								// used when previous node was straighnt line
		bool arcWillBeClockWiseAfterArc(Point<double>& previousNodeCenter, Point<double>& previousNodePeak, Point<double>& arcFirstPoint, Point<double>& newPoint);		// used when previous nodw was an arc
	public:
		ArcNode(Node& previousNode, PolyLine* polyLine, Point<double> newPoint);
		inline bool isArc() override;
		inline bool isFirstNode() override;
		inline Point<double> arcCenter();
		inline Point<double> getPeakPoint();
		void generateVertexChain(VertexChain<double>& vertexChain, unsigned int accuracy) override;		// sets vertexes in vertex chain so it can be displayed on screen in given accuracy (vertexes per whole 360 deg circle)
		void generatePeakPoints(vector<Point<double>>& points) override;								// sets peak points of arc so it can be displayed on screen
	};



	class PolyLine
	{
		vector<unique_ptr<Node>> nodes;	
		unique_ptr<Node> displayNode;						// used to show the shape of polyline after mouse move
		bool displayNodeBlocked;							// flag that is used to block gl functions that are running parallel
		
	public:
		PolyLine(Point<double>& point);						// creates Polyline with first node in given point
		~PolyLine();
		PolyLine(const PolyLine&) = delete;
		
		void blockDisplayNode();		
		void unBlockDisplayNode();
		bool addLine(Point<double>& point);					// adds new vertex after given point
		bool addArc(Point<double>& point);					// adds new vertex at the end of polyline
		bool addDisplayLineNode(Point<double>& point);		// adds DISPLAY node after mouse move
		bool addDisplayArcNode(Point<double>& point);		// adds DISPLAY node after mouse move
		inline Node& getNodeAt(unsigned int index);			// returns node at given index
		inline unsigned int lastNodeIndex();				// returns last real nodes index
		bool removeLastNode();								// returns false if the last node is PolyLineFirstNode, that cannot be removed
		void generatePeakPoints(vector<Point<double>>& peakPoints);								// generates peak points on every arc and sets them to the vector
		void generateVertexChain(VertexChain<double>& vertexChain, unsigned int accuracy);		// generates polyline with given accuracy, so it can be displayed
	};
}
