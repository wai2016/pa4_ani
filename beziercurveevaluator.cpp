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
		displayBezierSmart(ptvCtrlPts[i * 3 + 0], ptvCtrlPts[i * 3 + 1], ptvCtrlPts[i * 3 + 2], ptvCtrlPts[i * 3 + 3], ptvEvaluatedCurvePts);
	}

	if (!(bWrap && left == 3))
	{
		// point left
		for (int i = 0; i < left; ++i)
			ptvEvaluatedCurvePts.push_back(Point(ptvCtrlPts[i + iCtrlPtCount - left].x, ptvCtrlPts[i + iCtrlPtCount - left].y));
	}
}

void BezierCurveEvaluator::displayBezier(const Point c1, const Point c2, const Point c3, const Point c4, std::vector<Point>& ptvEvaluatedCurvePts) const
{
	Mat4f base(-1, 3, -3, 1, 3, -6, 3, 0, -3, 3, 0, 0, 1, 0, 0, 0);
	float interval = 0.001;
	Mat4f points(c1.x, c1.y, 0, 0,c2.x, c2.y, 0, 0, c3.x, c3.y, 0, 0, c4.x, c4.y, 0, 0);
	Mat4f baseXpoints = base * points;
	Mat4f poly, ans;
	ptvEvaluatedCurvePts.push_back(c1); // first point, repeated?
	for (float j = 0.0; j < 1.0; j += interval)
	{
		poly = Mat4f(pow(j, 3), pow(j, 2), j, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		ans = poly * baseXpoints;
		ptvEvaluatedCurvePts.push_back(Point(ans[0][0], ans[0][1]));
	}
	ptvEvaluatedCurvePts.push_back(c4); // last point, repeated?
}

float dist(const Point c1, const Point c2)
{
	float detlaX = c1.x - c2.x;
	float detlaY = c1.y - c2.y;
	return (sqrt(detlaX * detlaX + detlaY * detlaY));
}

bool flatEnough(const Point c1, const Point c2, const Point c3, const Point c4)
{
	if ((dist(c1, c2) + dist(c2, c3) + dist(c3, c4)) / dist(c1, c4) < 1 + CurveEvaluator::s_fFlatnessEpsilon)
	{
		return true;
	}
	return false;
}

Point midPt(const Point c1, const Point c2)
{
	// midpoint theorem
	float mX = (c1.x + c2.x) / 2;
	float mY = (c1.y + c2.y) / 2;
	return Point(mX, mY);
}

void subDivide(const Point c1, const Point c2, const Point c3, const Point c4, std::vector<Point>& l, std::vector<Point>& r)
{
	l.clear();
	r.clear();

	Point l2 = midPt(c1, c2);
	Point extra = midPt(c2, c3);
	Point r3 = midPt(c3, c4);
	Point l3 = midPt(l2, extra);
	Point r2 = midPt(extra, r3);
	Point q = midPt(l3, r2);

	l.push_back(c1);
	l.push_back(l2);
	l.push_back(l3);
	l.push_back(q);
	r.push_back(q);
	r.push_back(r2);
	r.push_back(r3);
	r.push_back(c4);
}

void adaptive(const Point c1, const Point c2, const Point c3, const Point c4, std::vector<Point>& ptvEvaluatedCurvePts) 
{
	if (flatEnough(c1, c2, c3, c4))
	{
		ptvEvaluatedCurvePts.push_back(c1);
		ptvEvaluatedCurvePts.push_back(c4);
		return;
	}
	else
	{
		std::vector<Point> l;
		std::vector<Point> r;
		subDivide(c1, c2, c3, c4, l, r);
		adaptive(l[0], l[1], l[2], l[3], ptvEvaluatedCurvePts);
		adaptive(r[0], r[1], r[2], r[3], ptvEvaluatedCurvePts);
	}
}

void BezierCurveEvaluator::displayBezierSmart(const Point c1, const Point c2, const Point c3, const Point c4, std::vector<Point>& ptvEvaluatedCurvePts) const
{
	//std::vector<Point> l;
	//std::vector<Point> r;
	adaptive(c1, c2, c3, c4, ptvEvaluatedCurvePts);
}

