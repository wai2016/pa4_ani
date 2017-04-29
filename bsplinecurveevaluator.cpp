#include "BsplineCurveEvaluator.h"
#include "mat.h"
#include "beziercurveevaluator.h"
#include <assert.h>

void BsplineCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts,
	std::vector<Point>& ptvEvaluatedCurvePts,
	const float& fAniLength,
	const bool& bWrap) const
{
	int iCtrlPtCount = ptvCtrlPts.size();

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
	else
	{
		// hack
		myPts.insert(myPts.begin(), ptvCtrlPts[0]);
		myPts.push_back(ptvCtrlPts[iCtrlPtCount - 1]);
		ptvEvaluatedCurvePts.push_back(ptvCtrlPts[0]);
		ptvEvaluatedCurvePts.push_back(ptvCtrlPts[iCtrlPtCount - 1]);
	}

	// bspline curve
	for (int i = 0; i < myPts.size() - 3; ++i)
	{
		displayBspline(myPts[i], myPts[i + 1], myPts[i + 2], myPts[i + 3], ptvEvaluatedCurvePts);
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

void BsplineCurveEvaluator::displayBspline(const Point c1, const Point c2, const Point c3, const Point c4, std::vector<Point>& ptvEvaluatedCurvePts) const
{
	// convert points into bezier control points
	Mat4f mat(1, 4, 1, 0, 0, 4, 2, 0, 0, 2, 4, 0, 0, 1, 4, 1);
	Mat4f points(c1.x, c1.y, 0, 0, c2.x, c2.y, 0, 0,c3.x, c3.y, 0, 0, c4.x, c4.y, 0, 0);
	Mat4f conversion = mat * points / 6;
	Point v1(conversion[0][0], conversion[0][1]);
	Point v2(conversion[1][0], conversion[1][1]);
	Point v3(conversion[2][0], conversion[2][1]);
	Point v4(conversion[3][0], conversion[3][1]);
	BezierCurveEvaluator bezier;

	bezier.displayBezier(v1, v2, v3, v4, ptvEvaluatedCurvePts);
}