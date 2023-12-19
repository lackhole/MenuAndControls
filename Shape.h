#pragma once
#include <atltypes.h>

class CShape {
public:
	CShape(CPoint p1 = CPoint{ 0,0 }, CPoint p2 = CPoint{ 0,0 }, bool selected = false) : p1_(p1), p2_(p2), selected_(selected) {}
	CShape(const CShape& r) : p1_(r.p1_), p2_(r.p2_), selected_(r.selected_) {}
	CShape(CShape&& r) noexcept : p1_(r.p1_), p2_(r.p2_), selected_(r.selected_) {}
	CShape& operator=(const CShape& r) {
		p1_ = r.p1_;
		p2_ = r.p1_;
		selected_ = r.selected_;
	}
	CShape& operator=(CShape&& r) noexcept {
		p1_ = r.p1_;
		p2_ = r.p1_;
		selected_ = r.selected_;
	}
	virtual ~CShape() {}
public:
	CPoint p1_;
	CPoint p2_;
	bool selected_;
};

class CRectangle : public CShape {
public:
	CRectangle(CPoint p1 = CPoint{ 0,0 }, CPoint p2= CPoint{0,0}, bool selected = false) : CShape(p1, p2, selected) {

	}
	CRectangle(const CRectangle& r) : CShape(r.p1_, r.p2_, r.selected_) {}
	CRectangle(CRectangle&& r) noexcept : CShape(r.p1_, r.p2_, r.selected_) {}
	CRectangle& operator=(const CRectangle& r) {
		p1_ = r.p1_;
		p2_ = r.p1_;
		selected_ = r.selected_;
	}
	CRectangle& operator=(CRectangle&& r) noexcept {
		p1_ = r.p1_;
		p2_ = r.p1_;
		selected_ = r.selected_;
	}
};

class CCircle : public CShape {
public:
	CCircle(CPoint p1 = CPoint{ 0,0 }, CPoint p2 = CPoint{ 0,0 }, bool selected=false) : CShape(p1, p2, selected) {

	}
	CCircle(const CCircle& r) : CShape(r.p1_, r.p2_, r.selected_) {}
	CCircle(CCircle&& r) noexcept : CShape(r.p1_, r.p2_, r.selected_) {}
	CCircle& operator=(const CCircle& r) {
		p1_ = r.p1_;
		p2_ = r.p1_;
		selected_ = r.selected_;
	}
	CCircle& operator=(CCircle&& r) noexcept {
		p1_ = r.p1_;
		p2_ = r.p1_;
		selected_ = r.selected_;
	}
};


class CLine : public CShape {
public:
	CLine(CPoint p1 = CPoint{ 0,0 }, CPoint p2 = CPoint{ 0,0 }, bool selected=false) : CShape(p1, p2, selected) {

	}
	CLine(const CLine& r) : CShape(r.p1_, r.p2_, r.selected_) {}
	CLine(CLine&& r) noexcept : CShape(r.p1_, r.p2_, r.selected_) {}
	CLine& operator=(const CLine& r) {
		p1_ = r.p1_;
		p2_ = r.p1_;
		selected_ = r.selected_;
	}
	CLine& operator=(CLine&& r) noexcept {
		p1_ = r.p1_;
		p2_ = r.p1_;
		selected_ = r.selected_;
	}
};