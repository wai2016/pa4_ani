#ifndef INCLUDED_C2INTERPOLATING_CURVE_EVALUATOR_H
#define INCLUDED_C2INTERPOLATING_CURVE_EVALUATOR_H

#pragma warning(disable : 4786)  

#include "CurveEvaluator.h"

//using namespace std;

class C2InterpolatingCurveEvaluator : public CurveEvaluator
{
public:
	void evaluateCurve(const std::vector<Point>& ptvCtrlPts,
		std::vector<Point>& ptvEvaluatedCurvePts,
		const float& fAniLength,
		const bool& bWrap) const;

	void displayC2Interpolating(const std::vector<Point>& ptvCtrlPts,
		std::vector<Point>& ptvEvaluatedCurvePts,
		const bool& bWrap) const;
};

#endif