
// ChildView.cpp: CChildView 클래스의 구현
//
#include "pch.h"
#include "framework.h"
#include "MenuAndControls.h"
#include "ChildView.h"

#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <string>
#include <thread>
#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace {

CString GetSystemTimeAndDate() {
	return CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S");
}


/*
\brief: 원을 그립니다


\dc: 디바이스 컨텍스트
\center: 원의 중심 좌표
\radius: 원의 반지름
\color: 도형의 내부 색
\thickness: 테두리 두께
\color_line: 테두리의 색
*/
void Circle(CDC* dc,
			CPoint center, int radius, COLORREF color,
			int thickness = 1, COLORREF color_line = RGB(0, 0, 0))
{
  // 새로운 펜 객체 사용
	CPen pen(thickness <= 0 ? PS_NULL : PS_SOLID, thickness, color_line);
	auto pen_prev = dc->SelectObject(&pen);

	// 새로운 브러쉬(채우기) 객체 사용
	CBrush brush(color);
	CBrush* prev_brush = dc->SelectObject(&brush);

	// 원(타원) 그리기
	dc->Ellipse(center.x - radius, center.y - radius,
				center.x + radius, center.y + radius);

	// 사용하는 펜과 객체 도구를 원래대로 되돌리기
	dc->SelectObject(prev_brush);
	dc->SelectObject(pen_prev);
}

/*
\brief: 직사각형을 그립니다


\dc: 디바이스 컨텍스트
\rect: 직사각형의 좌표
\color: 도형의 내부 색
\thickness: 테두리 두께
\color_line: 테두리의 색
*/
void Rectangle(CDC* dc,
				CRect rect, COLORREF color,
				int thickness = 1, COLORREF color_line = RGB(0, 0, 0)) {
	CPen pen(thickness <= 0 ? PS_NULL : PS_SOLID, thickness, color_line);
	auto pen_prev = dc->SelectObject(&pen);

	CBrush brush(color);
	CBrush* prev_brush = dc->SelectObject(&brush);
	dc->Rectangle(rect.left, rect.top, rect.right, rect.bottom);

	dc->SelectObject(prev_brush);
	dc->SelectObject(pen_prev);
}


/*
\brief: 직사각형을 그립니다


\dc: 디바이스 컨텍스트
\top_left: 직사각형의 왼쪽 위의 좌표
\bottom_right: 직사각형의 오른쪽 아래의 좌표
\color: 도형의 내부 색
\thickness: 테두리 두께
\color_line: 테두리의 색
*/
void Rectangle(CDC* dc,
							 CPoint top_left, CPoint bottom_right, COLORREF color,
							 int thickness = 1, COLORREF color_line = RGB(0, 0, 0))
{
	Rectangle(dc, {top_left, bottom_right}, color, thickness, color_line);
}

/*
\brief: 직선을 그립니다


\dc: 디바이스 컨텍스트
\start: 직선의 시작점 좌표
\end: 직선의 끝점 좌표
\thickness: 직선의 두께
\color_line: 직선의 색
*/
void Line(CDC* dc,
		  CPoint start, CPoint end, int thickness = 1, COLORREF color = RGB(0, 0, 0)) {
	CPen pen(PS_SOLID, thickness, color);
	auto pen_prev = dc->SelectObject(&pen);

	dc->MoveTo(start);
	dc->LineTo(end);

	dc->SelectObject(pen_prev);
}



/*
\brief: 다각형을 그립니다


\dc: 디바이스 컨텍스트
\points: 다각형의 점 좌표들
\color: 도형의 내부 색
\thickness: 테두리 두께
\color_line: 테두리의 색
*/
void Polygon(CDC* dc,
						 const std::vector<CPoint>& points, COLORREF color,
						 int thickness = 1, COLORREF color_line = RGB(0, 0, 0)) {
	CPen pen(thickness <= 0 ? PS_NULL : PS_SOLID, thickness, color_line);
	CBrush brush(color);
	
	CPen* pen_prev = dc->SelectObject(&pen);
	CBrush* prev_brush = dc->SelectObject(&brush);

	dc->Polygon(points.data(), points.size());
	
	dc->SelectObject(prev_brush);
	dc->SelectObject(pen_prev);
}

} // anonymous namespace

// CChildView

CChildView::CChildView()
{
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_CONTEXTMENU()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_COMMAND(ID_SELECTAREA, &CChildView::OnSelectArea)
	ON_UPDATE_COMMAND_UI(ID_SELECTAREA, &CChildView::OnUpdateSelectarea)
	ON_COMMAND(ID_DRAW_LINE, &CChildView::OnDrawLine)
	ON_UPDATE_COMMAND_UI(ID_DRAW_LINE, &CChildView::OnUpdateDrawLine)
	ON_COMMAND(ID_REMOVE_SELECTED, &CChildView::OnRemoveSelected)
	ON_COMMAND(ID_DRAW_RECTANGLE, &CChildView::OnDrawRectangle)
	ON_UPDATE_COMMAND_UI(ID_DRAW_RECTANGLE, &CChildView::OnUpdateDrawRectangle)
	ON_COMMAND(ID_DRAW_CIRCLE, &CChildView::OnDrawCircle)
	ON_UPDATE_COMMAND_UI(ID_DRAW_CIRCLE, &CChildView::OnUpdateDrawCircle)
	ON_COMMAND(ID_DRAW_CURVE, &CChildView::OnDrawCurve)
	ON_UPDATE_COMMAND_UI(ID_DRAW_CURVE, &CChildView::OnUpdateDrawCurve)
	ON_WM_KEYDOWN()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CChildView 메시지 처리기

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(nullptr, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), nullptr);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

	// Rought double buffering 
	CRect rect;
	GetClientRect(&rect);

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	memDC.SelectObject(&bitmap);
	memDC.Rectangle(0, 0, rect.Width(), rect.Height());

	OnMyPaint(&memDC);

	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC,
						0, 0, SRCCOPY);
}

afx_msg void CChildView::OnMyPaint(CDC* dc) {
	// 저장되어 있는 shape 객체 전체 그리기
	for (const auto& shape : m_shapes) {
		shape->Draw(dc);
	}

	// shape preview 그리기
	m_shape_preview->Draw(dc);

	// 선택된 shape 의 테두리 highlight
	for (const auto& shape : m_selected_highlights) {
		shape.Draw(dc);
	}
}

void CChildView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	/*
	CMenu* pMenu = AfxGetMainWnd()->GetMenu()->GetSubMenu(2);
	if(pMenu != NULL)
	pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y,
		AfxGetMainWnd());
	*/
}


int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// shape preview 초기화
	m_shape_preview = std::make_unique<ShapeNull>();

	// <영역 선택> 클릭 시
	m_toolbar_listeners.Add(kToolbarSelectArea, [this]() {
		m_shape_builder = std::make_unique<ShapeBuilder<ShapeRectangle>>();
		m_shape_preview = m_shape_builder->Create();
		m_shape_preview->SetColor(RGB(255, 0, 0));
		m_selected_highlights.clear();
	});

	// <도형 지우기> 클릭 시
	m_toolbar_listeners.Add(kToolbarRemoveSelected, [this]() {
		RemoveSelectedObjects();
	});

	// <DELETE> 키 누를 시
	m_keyboard_listeners.Add(VK_DELETE, [this](...) {
		RemoveSelectedObjects();
	});

	// <BACKSPACE> 키 누를 시
	m_keyboard_listeners.Add(VK_BACK, [this](...) {
		RemoveSelectedObjects();
	});

	// <사각형 그리기> 클릭 시
	m_toolbar_listeners.Add(kToolbarDrawRectangle, [this]() {
		m_shape_builder = std::make_unique<ShapeBuilder<ShapeRectangle>>();
		m_shape_preview = m_shape_builder->Create();
	});

	// <원 그리기> 클릭 시
	m_toolbar_listeners.Add(kToolbarDrawCircle, [this]() {
		m_shape_builder = std::make_unique<ShapeBuilder<ShapeEllipse>>();
		m_shape_preview = m_shape_builder->Create();
	});

	// <선 그리기> 클릭 시
	m_toolbar_listeners.Add(kToolbarDrawLine, [this]() {
		m_shape_builder = std::make_unique<ShapeBuilder<ShapeLine>>();
		m_shape_preview = m_shape_builder->Create();
	});

	// <곡선 그리기> 클릭 시
	m_toolbar_listeners.Add(kToolbarDrawCurve, [this]() {
		m_shape_builder = std::make_unique<ShapeBuilder<ShapeCurve>>();
		m_shape_preview = m_shape_builder->Create();
	});

	// 마우스 드래그 시
	m_mouse_event_listeners.Add(kMouseMove, [this](auto flags, auto p) {
		// 드래그 중 인지 검사
		if (flags & MK_LBUTTON) {
			m_shape_preview->SetEndpoint(p);
			m_selected_highlights.clear();
		}
	});

	// 왼 마우스 버튼 누를 시
	m_mouse_event_listeners.Add(kMouseLButtonDown, [this](auto, auto p) {
		if (m_toolbar_mode != kToolbarNone && m_toolbar_mode != kToolbarRemoveSelected) {
			m_shape_preview->SetStartpoint(p);
			m_shape_preview->SetEndpoint(p);
		}
	});

	// 왼 마우스 버튼 뗄 시
	m_mouse_event_listeners.Add(kMouseLButtonUp, [this](...) {
		if (m_toolbar_mode == kToolbarNone)
			return;

		// <영역 선택> 모드였을 경우
		if (m_toolbar_mode == kToolbarSelectArea) {
			m_selected_shapes = FindObjectsInArea();
			CreateSelectedHighlights();
			m_shape_preview->SetStartpoint({ 0, 0 });
			m_shape_preview->SetEndpoint({ 0, 0 });
			return;
		}

		// <무언가 그리기> 모드였을 경우

		// shape preview 가 NULL 일 때
		if (m_shape_preview->IsNull())
			return;

		// 그 외의 경우(실제 그리기 모드인 경우)
		m_shapes.emplace_back(std::move(m_shape_preview));
		m_shape_preview = m_shape_builder->Create();
	});

	// 우클릭 시 그리기 모드 취소
	m_mouse_event_listeners.Add(kMouseRButtonDown, [this](auto, auto p) {
		UnselectToolbar();
	});
	// ESC 시 그리기 모드 취소
	m_keyboard_listeners.Add(VK_ESCAPE, [this](...) {
		UnselectToolbar();
	});

	return 0;
}

void CChildView::OnMouseMove(UINT nFlags, CPoint point) {
	m_mouse_event_listeners(kMouseMove, nFlags, point);
	CWnd::Invalidate();
	CWnd::OnMouseMove(nFlags, point);
}

void CChildView::OnLButtonDown(UINT nFlags, CPoint point) {
	m_mouse_event_listeners(kMouseLButtonDown, nFlags, point);
	CWnd::SetCapture();
	CWnd::Invalidate();
	CWnd::OnLButtonDown(nFlags, point);
}

void CChildView::OnLButtonUp(UINT nFlags, CPoint point) {
	m_mouse_event_listeners(kMouseLButtonUp, nFlags, point);
	ReleaseCapture();
	CWnd::Invalidate();
	CWnd::OnLButtonUp(nFlags, point);
}

void CChildView::OnLButtonDblClk(UINT nFlags, CPoint point) {
	m_mouse_event_listeners(kMouseLButtonDblClk, nFlags, point);
	CWnd::Invalidate();
	CWnd::OnLButtonDblClk(nFlags, point);
}


void CChildView::OnRButtonDown(UINT nFlags, CPoint point) {
	m_mouse_event_listeners(kMouseRButtonDown, nFlags, point);
	CWnd::SetCapture();
	CWnd::Invalidate();
	CWnd::OnRButtonDown(nFlags, point);
}

void CChildView::OnRButtonUp(UINT nFlags, CPoint point) {
	m_mouse_event_listeners(kMouseRButtonUp, nFlags, point);
	ReleaseCapture();
	CWnd::Invalidate();
	CWnd::OnRButtonUp(nFlags, point);
}

void CChildView::OnRButtonDblClk(UINT nFlags, CPoint point) {
	m_mouse_event_listeners(kMouseRButtonDblClk, nFlags, point);
	CWnd::Invalidate();
	CWnd::OnRButtonDblClk(nFlags, point);
}

void CChildView::OnMButtonDown(UINT nFlags, CPoint point) {
	m_mouse_event_listeners(kMouseMButtonDown, nFlags, point);
	CWnd::SetCapture();
	CWnd::Invalidate();
	CWnd::OnMButtonDown(nFlags, point);
}

void CChildView::OnMButtonUp(UINT nFlags, CPoint point) {
	m_mouse_event_listeners(kMouseMButtonUp, nFlags, point);
	ReleaseCapture();
	CWnd::Invalidate();
	CWnd::OnMButtonUp(nFlags, point);
}

void CChildView::OnMButtonDblClk(UINT nFlags, CPoint point) {
	m_mouse_event_listeners(kMouseMButtonDblClk, nFlags, point);
	CWnd::Invalidate();
	CWnd::OnMButtonDblClk(nFlags, point);
}


void CChildView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	m_keyboard_listeners(nChar, nRepCnt, nFlags);
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CChildView::OnTimer(UINT_PTR nIDEvent) {
	m_timer_event_listeners(static_cast<TimerEvent>(nIDEvent));
	CWnd::Invalidate();
	CWnd::OnTimer(nIDEvent);
}

void CChildView::OnSelectArea() {
	m_toolbar_mode = (m_toolbar_mode == kToolbarSelectArea ? kToolbarNone : kToolbarSelectArea);
	if (m_toolbar_mode == kToolbarSelectArea) {
		m_toolbar_listeners(kToolbarSelectArea);
	} else {
		UnselectToolbar();
	}
}
void CChildView::OnUpdateSelectarea(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_toolbar_mode == kToolbarSelectArea);
}


void CChildView::OnDrawLine() {
	m_toolbar_mode = (m_toolbar_mode == kToolbarDrawLine ? kToolbarNone : kToolbarDrawLine);
	if (m_toolbar_mode == kToolbarDrawLine) {
		m_toolbar_listeners(kToolbarDrawLine);
	} else {
		UnselectToolbar();
	}
}
void CChildView::OnUpdateDrawLine(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_toolbar_mode == kToolbarDrawLine);
}

void CChildView::OnDrawCurve() {
	m_toolbar_mode = (m_toolbar_mode == kToolbarDrawCurve ? kToolbarNone : kToolbarDrawCurve);
	if (m_toolbar_mode == kToolbarDrawCurve) {
		m_toolbar_listeners(kToolbarDrawCurve);
	} else {
		UnselectToolbar();
	}
}
void CChildView::OnUpdateDrawCurve(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_toolbar_mode == kToolbarDrawCurve);
}

void CChildView::OnRemoveSelected() {
	m_toolbar_listeners(kToolbarRemoveSelected);
}


void CChildView::OnDrawRectangle() {
	m_toolbar_mode = (m_toolbar_mode == kToolbarDrawRectangle ? kToolbarNone : kToolbarDrawRectangle);
	if (m_toolbar_mode == kToolbarDrawRectangle) {
		m_toolbar_listeners(kToolbarDrawRectangle);
	} else {
		UnselectToolbar();
	}
}
void CChildView::OnUpdateDrawRectangle(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_toolbar_mode == kToolbarDrawRectangle);
}


void CChildView::OnDrawCircle() {
	m_toolbar_mode = (m_toolbar_mode == kToolbarDrawCircle? kToolbarNone : kToolbarDrawCircle);
	if (m_toolbar_mode == kToolbarDrawCircle) {
		m_toolbar_listeners(kToolbarDrawCircle);
	} else {
		UnselectToolbar();
	}
}
void CChildView::OnUpdateDrawCircle(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_toolbar_mode == kToolbarDrawCircle);
}


BOOL CChildView::OnEraseBkgnd(CDC* pDC) {
	return TRUE;
	// return CWnd::OnEraseBkgnd(pDC);
}


void CChildView::UnselectToolbar() {
	m_toolbar_mode = kToolbarNone;
	m_shape_builder = std::make_unique<ShapeBuilder<ShapeNull>>();
	m_shape_preview = m_shape_builder->Create();
	m_selected_highlights.clear();
}

void CChildView::RemoveSelectedObjects() {
	std::cout << "Removing " << m_selected_shapes.size() << " objects\n";
	for (auto it : m_selected_shapes) {
		m_shapes.erase(it);
	}
	m_selected_shapes.clear();
	m_selected_highlights.clear();
	CWnd::Invalidate();
}

std::vector<CChildView::shape_iterator> CChildView::FindObjectsInArea() {
	std::vector<shape_iterator> out;

	const auto tl = m_shape_preview->Start();
	const auto br = m_shape_preview->End();

	for (auto it = m_shapes.begin(); it != m_shapes.end(); ++it) {
		if ((*it)->IsInArea(tl, br)) {
			out.push_back(it);
		}
	}

	std::cout << "Select " << out.size() << " objects\n";
	return out;
}

void CChildView::CreateSelectedHighlights() {
	m_selected_highlights.clear();

	for (auto it : m_selected_shapes) {
		auto tl = (*it)->TopLeft();
		auto br = (*it)->BottomRight();

		// 하이라이트의 크기는 shape 보다 조금 크게 설정
		tl.x -= 5;
		tl.y -= 5;
		br.x += 5;
		br.y += 5;

		ShapeRectangle area;
		area.SetStartpoint(tl);
		area.SetEndpoint(br);
		area.SetColor(RGB(255, 0, 0));
		area.SetPenStyle(PS_DASH);

		m_selected_highlights.emplace_back(std::move(area));
	}
}
