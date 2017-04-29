#ifndef INCLUDED_CATMULLROM_CURVE_EVALUATOR_H
#define INCLUDED_CATMULLROM_CURVE_EVALUATOR_H

#pragma warning(disable : 4786)  

#include "CurveEvaluator.h"

//using namespace std;

class CatmullRomCurveEvaluator : public CurveEvaluator
{
public:
	void evaluateCurve(const std::vector<Point>& ptvCtrlPts,
		std::vector<Point>& ptvEvaluatedCurvePts,
		const float& fAniLength,
		const bool& bWrap) const;

	void displayCatmullRom(const Point c1, const Point c2, const Point c3, const Point c4,
		std::vector<Point>& ptvEvaluatedCurvePts, 
		const float tension) const;
};

#endif