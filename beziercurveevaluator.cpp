#include "beziercurveevaluator.h"
#include <assert.h>
#include "mat.h"

void BezierCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts,
	std::vector<Point>& ptvEvaluatedCurvePts,
	const float& fAniLength,
	const bool& bWrap) const
{
	int iCtrlPtCount = ptvCtrlPts.size();

	int left = (iCtrlPtCount - 1) % 3 + 1; // points that can't make bezier curve

	ptvEvaluatedCurvePts.clear(); // redraw, global control

	float x1 = 0.0;
	float x2 = fAniLength;
	float y1, y2;
	Mat4f base(-1, 3, -3, 1, 3, -6, 3, 0, -3, 3, 0, 0, 1, 0, 0, 0);
	float interval = 0.001;

	if (bWrap) {
		if ((ptvCtrlPts[0].x + fAniLength) - ptvCtrlPts[iCtrlPtCount - 1].x > 0.0f) {
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
					if (ans[0][0] > x2)
					{
						ptvEvaluatedCurvePts.push_back(Point(ans[0][0] - x2, ans[0][1]));
						if (last)
						{
							y2 = ans[0][1] - interval;
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
			}
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

	// bezier curve
	for (int i = 0; i < (iCtrlPtCount - 1) / 3; ++i)
	{
		Mat4f points(ptvCtrlPts[i * 3 + 0].x, ptvCtrlPts[i * 3 + 0].y, 0, 0, ptvCtrlPts[i * 3 + 1].x, ptvCtrlPts[i * 3 + 1].y, 0, 0, ptvCtrlPts[i * 3 + 2].x, ptvCtrlPts[i * 3 + 2].y, 0, 0, ptvCtrlPts[i * 3 + 3].x, ptvCtrlPts[i * 3 + 3].y, 0, 0);
		Mat4f baseXpoints = base * points;
		Mat4f poly, ans;
		ptvEvaluatedCurvePts.push_back(Point(ptvCtrlPts[i * 3 + 0].x, ptvCtrlPts[i * 3 + 0].y)); // first point, repeated?
		for (float j = 0.0; j < 1.0; j += interval)
		{
			poly = Mat4f(pow(j, 3), pow(j, 2), j, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
			ans = poly * baseXpoints;
			ptvEvaluatedCurvePts.push_back(Point(ans[0][0], ans[0][1]));
		}
		ptvEvaluatedCurvePts.push_back(Point(ptvCtrlPts[i * 3 + 3].x, ptvCtrlPts[i * 3 + 3].y)); // last point, repeated?
	}

	if (!(bWrap && left == 3))
	{
		// point left
		for (int i = 0; i < left; ++i)
			ptvEvaluatedCurvePts.push_back(Point(ptvCtrlPts[i + iCtrlPtCount - left].x, ptvCtrlPts[i + iCtrlPtCount - left].y));
	}
}

