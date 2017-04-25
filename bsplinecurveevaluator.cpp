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
	BezierCurveEvaluator bezier;

	//int left = (iCtrlPtCount - 1) % 3 + 1; // points that can't make bezier curve

	ptvEvaluatedCurvePts.clear(); // redraw, global control

	float x1 = 0.0;
	float x2 = fAniLength;
	float y1, y2;
	Mat4f base(-1, 3, -3, 1, 3, -6, 3, 0, -3, 3, 0, 0, 1, 0, 0, 0);
	float interval = 0.001;

	if (bWrap) {
		if ((ptvCtrlPts[0].x + fAniLength) - ptvCtrlPts[iCtrlPtCount - 1].x > 0.0f) {
			/*
			if (left == 3)
			{
				Mat4f points(ptvCtrlPts[iCtrlPtCount - 1 - 2].x, ptvCtrlPts[iCtrlPtCount - 1 - 2].y, 0, 0, ptvCtrlPts[iCtrlPtCount - 1 - 1].x, ptvCtrlPts[iCtrlPtCount - 1 - 1].y, 0, 0, ptvCtrlPts[iCtrlPtCount - 1].x, ptvCtrlPts[iCtrlPtCount - 1].y, 0, 0, ptvCtrlPts[0].x + fAniLength, ptvCtrlPts[0].y, 0, 0);
				Mat4f baseXpoints = base * points;
				Mat4f poly, ans;
				bool last = true;
				for (float j = 0.0; j < 1.0; j += interval)
				{
					poly = Mat4f(pow(j, 3), pow(j, 2), j, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
					ans = poly * baseXpoints;

					// special cases (x = 0)
					if (ptvCtrlPts[0].x <= 0.00001)
						y2 = ptvCtrlPts[0].y;

					if (ans[0][0] > x2)
					{
						ptvEvaluatedCurvePts.push_back(Point(ans[0][0] - x2, ans[0][1]));
						if (last)
						{
							y2 = ans[0][1];
							last = false;
						}
					}
					else
						ptvEvaluatedCurvePts.push_back(Point(ans[0][0], ans[0][1]));
				}
				y1 = y2;
			}
			else
			{
				y1 = (ptvCtrlPts[0].y * (fAniLength - ptvCtrlPts[iCtrlPtCount - 1].x) +
					ptvCtrlPts[iCtrlPtCount - 1].y * ptvCtrlPts[0].x) /
					(ptvCtrlPts[0].x + fAniLength - ptvCtrlPts[iCtrlPtCount - 1].x);
				y2 = y1;
			}*/
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

	// bspline curve
	for (int i = 0; i < iCtrlPtCount - 3; ++i)
	{
		displayBspline(ptvCtrlPts[i], ptvCtrlPts[i + 1], ptvCtrlPts[i + 2], ptvCtrlPts[i + 3], ptvEvaluatedCurvePts);
	}
	/*
	if (!(bWrap && left == 3))
	{
		// point left
		for (int i = 0; i < left; ++i)
			ptvEvaluatedCurvePts.push_back(Point(ptvCtrlPts[i + iCtrlPtCount - left].x, ptvCtrlPts[i + iCtrlPtCount - left].y));
	}
	*/
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