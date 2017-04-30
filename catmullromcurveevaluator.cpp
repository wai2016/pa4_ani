#include "CatmullRomCurveEvaluator.h"
#include "mat.h"
#include "beziercurveevaluator.h"
#include <assert.h>

void CatmullRomCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts,
	std::vector<Point>& ptvEvaluatedCurvePts,
	const float& fAniLength,
	const bool& bWrap) const
{
	int iCtrlPtCount = ptvCtrlPts.size();
	float tension = 1.0 / 2.0;

	ptvEvaluatedCurvePts.clear(); // redraw, global control

	std::vector<Point> myPts;
	myPts.assign(ptvCtrlPts.begin(), ptvCtrlPts.end());

	if (bWrap)
	{
		Point e1(ptvCtrlPts[0].x + fAniLength, ptvCtrlPts[0].y);
		Point e2(ptvCtrlPts[1].x + fAniLength, ptvCtrlPts[1].y);
		myPts.push_back(e1);
		myPts.push_back(e2);
		Point e3;
		if (iCtrlPtCount > 2)
		{
			e3.x = ptvCtrlPts[2].x + fAniLength;
			e3.y = ptvCtrlPts[2].y;
			myPts.push_back(e3);
		}
		else
		{
			e3.x = ptvCtrlPts[1].x - fAniLength;
			e3.y = ptvCtrlPts[1].y;
			myPts.insert(myPts.begin(), e3);
		}
	}

	// catmull-rom curve
	if (myPts.size() > 2)
	{
		Point d1, d2;
		if (!bWrap)
		{
			d1 = Point(tension * (myPts[1].x - myPts[0].x) / 3, tension * (myPts[1].y - myPts[0].y) / 3);
			d2 = Point(tension * (myPts[2].x - myPts[0].x) / 3, tension * (myPts[2].y - myPts[0].y) / 3);
			displayCatmullRom(myPts[0], d1, myPts[1], d2, ptvEvaluatedCurvePts);
		}
		for (int i = 1; i < myPts.size() - 2; i++)
		{
			d1 = Point(tension * (myPts[i + 1].x - myPts[i - 1].x) / 3, tension * (myPts[i + 1].y - myPts[i - 1].y) / 3);
			d2 = Point(tension * (myPts[i + 2].x - myPts[i].x) / 3, tension * (myPts[i + 2].y - myPts[i].y) / 3);
			displayCatmullRom(myPts[i], d1, myPts[i + 1], d2, ptvEvaluatedCurvePts);
		}
		if (!bWrap)
		{
			d1 = Point(tension * (myPts[myPts.size() - 1].x - myPts[myPts.size() - 3].x) / 3, tension * (myPts[myPts.size() - 1].y - myPts[myPts.size() - 3].y) / 3);
			d2 = Point(tension * (myPts[myPts.size() - 1].x - myPts[myPts.size() - 2].x) / 3, tension * (myPts[myPts.size() - 1].y - myPts[myPts.size() - 2].y) / 3);
			displayCatmullRom(myPts[myPts.size() - 2], d1, myPts[myPts.size() - 1], d2, ptvEvaluatedCurvePts);
		}
	}

	float x1 = 0.0;
	float x2 = fAniLength;
	float y1, y2;

	if (bWrap) {
		if ((ptvCtrlPts[0].x + fAniLength) - ptvCtrlPts[iCtrlPtCount - 1].x > 0.0f) {
			float max = -1;
			int maxCount = -1;
			for (int i = 0; i < ptvEvaluatedCurvePts.size(); i++)
			{
				if (ptvEvaluatedCurvePts[i].x > x2)
					ptvEvaluatedCurvePts[i].x = ptvEvaluatedCurvePts[i].x - x2;
				else if (ptvEvaluatedCurvePts[i].x > max)
				{
					max = ptvEvaluatedCurvePts[i].x;
					maxCount = i;
				}
			}
			y2 = ptvEvaluatedCurvePts[maxCount].y;
			y1 = y2;
		}
		else
		{
			y1 = ptvCtrlPts[0].y;
			y2 = ptvCtrlPts[iCtrlPtCount - 1].y;
		}
	}
	else {
		// if wrapping is off, make the first and last segments of
		// the curve horizontal.
		y1 = ptvCtrlPts[0].y;
		y2 = ptvCtrlPts[iCtrlPtCount - 1].y;
	}

	ptvEvaluatedCurvePts.push_back(Point(x1, y1));
	ptvEvaluatedCurvePts.push_back(Point(x2, y2));
}

void CatmullRomCurveEvaluator::displayCatmullRom(const Point c1, const Point d1, const Point c2, const Point d2, std::vector<Point>& ptvEvaluatedCurvePts) const
{
	// convert points into bezier control points
	Point v1(c1);
	Point v2(c1.x + d1.x, c1.y + d1.y);
	Point v3(c2.x - d2.x, c2.y - d2.y);
	Point v4(c2);
	BezierCurveEvaluator bezier;
	//std::cout << v1 << v2 << v3 << v4 << std::endl;
	bezier.displayBezier(v1, v2, v3, v4, ptvEvaluatedCurvePts);
}