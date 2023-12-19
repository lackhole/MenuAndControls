#pragma once

#include <algorithm>
#include <memory>
#include <vector>

class ShapeBase {
public:
	ShapeBase() = default;
	virtual ~ShapeBase() = default;

	virtual void SetStartpoint(CPoint point) { m_start = point; }
	virtual const CPoint& Start() const { return m_start; }
	virtual void SetEndpoint(CPoint point) { m_end = point; }
	virtual const CPoint& End() const { return m_end; }

	virtual void SetColor(COLORREF color) {
		m_color = color;
	}

	virtual void SetPenStyle(UINT style) {
		m_pen_style = style;
	}

	virtual void Draw(CDC* dc) const = 0;

	virtual bool IsNull() const { return false; }

	virtual bool IsInArea(CPoint p1, CPoint p2) const {
		CPoint tl{ (std::min)(p1.x, p2.x), (std::min)(p1.y, p2.y) };
		CPoint br{ (std::max)(p1.x, p2.x), (std::max)(p1.y, p2.y) };

		return (tl.x <= m_start.x && m_start.x <= br.x) &&
			   (tl.y <= m_start.y && m_start.y <= br.y) &&
			   (tl.x <= m_end.x && m_end.x <= br.x) &&
			   (tl.y <= m_end.y && m_end.y <= br.y);
	}

	virtual CPoint TopLeft() const {
		return {(std::min)(m_start.x, m_end.x), (std::min)(m_start.y, m_end.y)};
	}

	virtual CPoint BottomRight() const {
		return { (std::max)(m_start.x, m_end.x), (std::max)(m_start.y, m_end.y) };
	}

protected:
	CPoint m_start;
	CPoint m_end;
	COLORREF m_color = RGB(0, 0, 0);
	UINT m_pen_style = PS_SOLID;
};


class ShapeNull : public ShapeBase {
public:
	ShapeNull() = default;

	void Draw(CDC* dc) const override { /* no-op */ }
	virtual bool IsNull() const { return true; }
};

class ShapeRectangle : public ShapeBase {
public:
	ShapeRectangle() = default;

	void Draw(CDC* dc) const override  {
		CPen pen(m_pen_style, 1, m_color);
		auto prev_pen = dc->SelectObject(&pen);

		// 투명 상자 (직선 4개)
		dc->MoveTo(m_start.x, m_start.y);
		dc->LineTo(m_start.x, m_end.y);

		dc->MoveTo(m_start.x, m_end.y);
		dc->LineTo(m_end.x, m_end.y);

		dc->MoveTo(m_end.x, m_end.y);
		dc->LineTo(m_end.x, m_start.y);

		dc->MoveTo(m_end.x, m_start.y);
		dc->LineTo(m_start.x, m_start.y);

		dc->SelectObject(prev_pen);
	}
};


class ShapeEllipse : public ShapeBase {
public:
	ShapeEllipse() = default;


	void Draw(CDC* dc) const override {
		CPen pen(m_pen_style, 1, m_color);
		auto prev_pen = dc->SelectObject(&pen);
		dc->Ellipse(m_start.x, m_start.y, m_end.x, m_end.y);

		dc->SelectObject(prev_pen);
	}
};

class ShapeLine : public ShapeBase {
public:
	ShapeLine() = default;

	void Draw(CDC* dc) const override {
		CPen pen(m_pen_style, 1, m_color);
		auto prev_pen = dc->SelectObject(&pen);

		dc->MoveTo(m_start);
		dc->LineTo(m_end);

		dc->SelectObject(prev_pen);
	}
};

class ShapeCurve : public ShapeBase {
public:
	ShapeCurve() = default;

	void SetEndpoint(CPoint point) override {
		m_points.emplace_back(point);
	}

	const CPoint& End() const override {
		if (m_points.empty()) {
			return m_start;
		}
		return m_points.back();
	}


	void Draw(CDC* dc) const override {
		if (m_points.empty()) {
			return;
		}

		CPen pen(m_pen_style, 1, m_color);
		auto prev_pen = dc->SelectObject(&pen);

		dc->MoveTo(m_start);
		dc->LineTo(m_points[0]);

		for (int i = 1; i < m_points.size(); ++i) {
			dc->MoveTo(m_points[i - 1]);
			dc->LineTo(m_points[i]);
		}

		dc->SelectObject(prev_pen);
	}

	bool IsInArea(CPoint p1, CPoint p2) const override {
		auto minmax_x = std::minmax_element(m_points.begin(), m_points.end(), [](auto a, auto b) { return a.x < b.x;  });
		auto minmax_y = std::minmax_element(m_points.begin(), m_points.end(), [](auto a, auto b) { return a.y < b.y;  });

		CPoint m1{ minmax_x.first->x, minmax_y.first->y };
		CPoint m2{ minmax_x.second->x, minmax_y.second->y };

		CPoint tl{ (std::min)(p1.x, p2.x), (std::min)(p1.y, p2.y) };
		CPoint br{ (std::max)(p1.x, p2.x), (std::max)(p1.y, p2.y) };

		return (tl.x <= m1.x && m1.x <= br.x) &&
			(tl.y <= m1.y && m1.y <= br.y) &&
			(tl.x <= m2.x && m2.x <= br.x) &&
			(tl.y <= m2.y && m2.y <= br.y);

		return false;
	}


	CPoint TopLeft() const override {
		auto min_x = std::min_element(m_points.begin(), m_points.end(), [](auto a, auto b) { return a.x < b.x;  });
		auto min_y = std::min_element(m_points.begin(), m_points.end(), [](auto a, auto b) { return a.y < b.y;  });
		return { min_x->x, min_y->y };
	}

	CPoint BottomRight() const override {
		auto max_x = std::max_element(m_points.begin(), m_points.end(), [](auto a, auto b) { return a.x < b.x;  });
		auto max_y = std::max_element(m_points.begin(), m_points.end(), [](auto a, auto b) { return a.y < b.y;  });
		return { max_x->x, max_y->y };
	}

private:
	std::vector<CPoint> m_points;
};