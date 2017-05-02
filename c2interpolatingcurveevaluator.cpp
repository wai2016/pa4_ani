#include "C2InterpolatingCurveEvaluator.h"
#include "beziercurveevaluator.h"
#include <assert.h>

void C2InterpolatingCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts,
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
		// using one point should not be enough because
		// acceleration is not correct but
		// aftering using more points,
		// there are double curves in the same time segment.
		Point e1(ptvCtrlPts[0].x + fAniLength, ptvCtrlPts[0].y);
		//Point e2(ptvCtrlPts[1].x + fAniLength, ptvCtrlPts[1].y);
		myPts.push_back(e1);
		//myPts.push_back(e2);
		//Point e3;
		//if (iCtrlPtCount > 2)
		//{
		//	e3.x = ptvCtrlPts[2].x + fAniLength;
		//	e3.y = ptvCtrlPts[2].y;
		//	myPts.push_back(e3);
		//}
		//else
		//{
		//	e3.x = ptvCtrlPts[1].x - fAniLength;
		//	e3.y = ptvCtrlPts[1].y;
		//	myPts.insert(myPts.begin(), e3);
		//}
	}

	// c2interpolating curve
	displayC2Interpolating(myPts, ptvEvaluatedCurvePts, bWrap);

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

// retrieve from https://martin-thoma.com/solving-linear-equations-with-gaussian-elimination/
std::vector<float> gauss(std::vector< std::vector<float> > A) {
	int n = A.size();

	for (int i = 0; i<n; i++) {
		// Search for maximum in this column
		float maxEl = abs(A[i][i]);
		int maxRow = i;
		for (int k = i + 1; k<n; k++) {
			if (abs(A[k][i]) > maxEl) {
				maxEl = abs(A[k][i]);
				maxRow = k;
			}
		}

		// Swap maximum row with current row (column by column)
		for (int k = i; k<n + 1; k++) {
			float tmp = A[maxRow][k];
			A[maxRow][k] = A[i][k];
			A[i][k] = tmp;
		}

		// Make all rows below this one 0 in current column
		for (int k = i + 1; k<n; k++) {
			float c = -A[k][i] / A[i][i];
			for (int j = i; j<n + 1; j++) {
				if (i == j) {
					A[k][j] = 0;
				}
				else {
					A[k][j] += c * A[i][j];
				}
			}
		}
	}

	// Solve equation Ax=b for an upper triangular matrix A
	std::vector<float> x(n);
	for (int i = n - 1; i >= 0; i--) {
		x[i] = A[i][n] / A[i][i];
		for (int k = i - 1; k >= 0; k--) {
			A[k][n] -= A[k][i] * x[i];
		}
	}
	return x;
}

std::vector<float> calculateD(const std::vector<float>& ptvCtrlPts, const bool& bWrap)
{
	int iCtrlPtCount = ptvCtrlPts.size();

	std::vector<float> line(iCtrlPtCount + 1, 0);
	std::vector< std::vector<float> > A(iCtrlPtCount, line);

	// initialise all to be 0
	for (int i = 0; i<iCtrlPtCount; i++) {
		for (int j = 0; j<iCtrlPtCount + 1; j++) {
			A[i][j] = 0;
		}
	}

	// set endpoints
	if (bWrap)
	{
		A[0][iCtrlPtCount - 1] = 1;
		A[0][0] = 4;
		A[0][1] = 1;

		A[iCtrlPtCount - 1][iCtrlPtCount - 2] = 1;
		A[iCtrlPtCount - 1][iCtrlPtCount - 1] = 4;
		A[iCtrlPtCount - 1][0] = 1;
	}
	else
	{
		A[0][0] = 2;
		A[0][1] = 1;

		A[iCtrlPtCount - 1][iCtrlPtCount - 2] = 1;
		A[iCtrlPtCount - 1][iCtrlPtCount - 1] = 2;
	}

	A[0][iCtrlPtCount] = (ptvCtrlPts[1] - ptvCtrlPts[0]) * 3;
	A[iCtrlPtCount - 1][iCtrlPtCount] = (ptvCtrlPts[iCtrlPtCount - 1] - ptvCtrlPts[iCtrlPtCount - 2]) * 3;

	// set the points in the middle
	for (int i = 1; i<iCtrlPtCount - 1; i++) {
		A[i][i - 1] = 1;
		A[i][i] = 4;
		A[i][i + 1] = 1;

		A[i][iCtrlPtCount] = (ptvCtrlPts[i + 1] - ptvCtrlPts[i - 1]) * 3;
	}

	return gauss(A);
}


void C2InterpolatingCurveEvaluator::displayC2Interpolating(const std::vector<Point>& ptvCtrlPts, std::vector<Point>& ptvEvaluatedCurvePts, const bool& bWrap) const
{
	int iCtrlPtCount = ptvCtrlPts.size();

	// calculate x
	std::vector<float> pX(iCtrlPtCount);
	for (int i = 0; i < iCtrlPtCount; i++)
	{
		pX[i] = ptvCtrlPts[i].x;
	}

	std::vector<float> dX(iCtrlPtCount);
	dX = calculateD(pX, bWrap);

	// calculate y
	std::vector<float> pY(iCtrlPtCount);
	for (int i = 0; i < iCtrlPtCount; i++)
	{
		pY[i] = ptvCtrlPts[i].y;
	}

	std::vector<float> dY(iCtrlPtCount);
	dY = calculateD(pY, bWrap);

	// convert points into bezier control points
	Point v1;
	Point v2;
	Point v3;
	Point v4;
	BezierCurveEvaluator bezier;

	for (int i = 0; i < iCtrlPtCount - 1; i++)
	{
		v1 = Point(ptvCtrlPts[i]);
		v2 = Point(ptvCtrlPts[i].x + dX[i] / 3, ptvCtrlPts[i].y + dY[i] / 3);
		v3 = Point(ptvCtrlPts[i + 1].x - dX[i + 1] / 3, ptvCtrlPts[i + 1].y - dY[i + 1] / 3);
		v4 = Point(ptvCtrlPts[i + 1]);
		bezier.displayBezier(v1, v2, v3, v4, ptvEvaluatedCurvePts);
	}
}