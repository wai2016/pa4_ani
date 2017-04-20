#include "beziercurveevaluator.h"
#include <assert.h>
#include "mat.h"

void BezierCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts,
	std::vector<Point>& ptvEvaluatedCurvePts,
	const float& fAniLength,
	const bool& bWrap) const
{
	int iCtrlPtCount = ptvCtrlPts.size();

	ptvEvaluatedCurvePts.assign(ptvCtrlPts.begin(), ptvCtrlPts.end());

	float x = 0.0;
	float y1;

	if (bWrap) {
		// if wrapping is on, interpolate the y value at xmin and
		// xmax so that the slopes of the lines adjacent to the
		// wraparound are equal.

		if ((ptvCtrlPts[0].x + fAniLength) - ptvCtrlPts[iCtrlPtCount - 1].x > 0.0f) {
			y1 = (ptvCtrlPts[0].y * (fAniLength - ptvCtrlPts[iCtrlPtCount - 1].x) +
				ptvCtrlPts[iCtrlPtCount - 1].y * ptvCtrlPts[0].x) /
				(ptvCtrlPts[0].x + fAniLength - ptvCtrlPts[iCtrlPtCount - 1].x);
		}
		else
			y1 = ptvCtrlPts[0].y;
	}
	else {
		// if wrapping is off, make the first and last segments of
		// the curve horizontal.

		y1 = ptvCtrlPts[0].y;
	}

	ptvEvaluatedCurvePts.push_back(Point(x, y1));

	for (int i = 0; i < (iCtrlPtCount - 1) / 3; ++i)
	{
		Mat4f base(-1, 3, -3, 1, 3, -6, 3, 0, -3, 3, 0, 0, 1, 0, 0, 0);
		Mat4f points(ptvCtrlPts[i * 3 + 0].x, ptvCtrlPts[i * 3 + 0].y, 0, 0, ptvCtrlPts[i * 3 + 1].x, ptvCtrlPts[i * 3 + 1].y, 0, 0, ptvCtrlPts[i * 3 + 2].x, ptvCtrlPts[i * 3 + 2].y, 0, 0, ptvCtrlPts[i * 3 + 3].x, ptvCtrlPts[i * 3 + 3].y, 0, 0);
		Mat4f baseXpoints = base * points;
		Mat4f poly, ans;
		for (float j = 0.0; j < 1.0; j += 0.1)
		{
			poly = Mat4f(pow(j, 3), pow(j, 2), j, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
			ans = poly * baseXpoints;
			ptvEvaluatedCurvePts.push_back(Point(ans[0][0], ans[0][1]));
		}
	}

	/// set the endpoint based on the wrap flag.
	float y2;
	x = fAniLength;
	if (bWrap)
		y2 = y1;
	else
		y2 = ptvCtrlPts[iCtrlPtCount - 1].y;

	ptvEvaluatedCurvePts.push_back(Point(x, y2));
	
}

