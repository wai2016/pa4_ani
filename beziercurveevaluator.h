#ifndef INCLUDED_BEZIER_CURVE_EVALUATOR_H
#define INCLUDED_BEZIER_CURVE_EVALUATOR_H

#pragma warning(disable : 4786)  

#include "CurveEvaluator.h"

//using namespace std;

class BezierCurveEvaluator : public CurveEvaluator
{
public:
	void evaluateCurve(const std::vector<Point>& ptvCtrlPts,
		std::vector<Point>& ptvEvaluatedCurvePts,
		const float& fAniLength,
		const bool& bWrap) const;

	void displayBezier(const Point c1, const Point c2, const Point c3, const Point c4,
		std::vector<Point>& ptvEvaluatedCurvePts) const;

	void displayBezierSmart(const Point c1, const Point c2, const Point c3, const Point c4,
		std::vector<Point>& ptvEvaluatedCurvePts) const;
};

#endif

