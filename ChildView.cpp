﻿
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
#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace {

CString GetSystemTimeAndDate() {
	return CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S");
}

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

void Rectangle(CDC* dc,
							 CPoint top_left, CPoint bottom_right, COLORREF color,
							 int thickness = 1, COLORREF color_line = RGB(0, 0, 0))
{
	Rectangle(dc, {top_left, bottom_right}, color, thickness, color_line);
}

}

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
	// 현재 시간 표시
	dc->TextOutW(10, 10, m_current_time);

	// 마우스 위치 표시
	std::string pos =
		"(" + std::to_string(m_mouse_pos.x) + ", "
		+ std::to_string(m_mouse_pos.y) + ")";
	dc->TextOut(10, 30, CString(pos.c_str()));

	// 마우스 이벤트 표시
	dc->TextOut(10, 50, _T("Event: ") + m_mouse_event);


	dc->TextOut(10, 70, _T("Keyboard: ") + CString(std::to_string(m_keyboard).c_str()));

	// 튀기는 공 그리기
	Circle(dc, m_ball_pos, m_ball_radius, RGB(0, 255, 255));

	Rectangle(dc, m_wall_rect, RGB(255, 255, 0));
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


void CChildView::CalculateBall() {
	m_ball_pos.x += m_ball_velocity.x;
	m_ball_pos.y += m_ball_velocity.y;

	// 화면 바깥을 넘어갔는지 검사
	{
		CRect rect;
		GetClientRect(&rect);

		if (m_ball_pos.x + m_ball_radius >= rect.Width()) {
			m_ball_pos.x -= m_ball_pos.x + m_ball_radius - rect.Width();
			m_ball_velocity.x *= -1;
		} else if (m_ball_pos.x - m_ball_radius < 0) {
			m_ball_pos.x = -(m_ball_pos.x - m_ball_radius);
			m_ball_velocity.x *= -1;
		}

		if (m_ball_pos.y + m_ball_radius >= rect.Height()) {
			m_ball_pos.y -= m_ball_pos.y + m_ball_radius - rect.Height();
			m_ball_velocity.y *= -1;
		} else if (m_ball_pos.y - m_ball_radius < 0) {
			m_ball_pos.y = -(m_ball_pos.y - m_ball_radius);
			m_ball_velocity.y *= -1;
		}
	}

	// 고무벽과 충돌했는지 검사
	CRect collision;
	if (collision.IntersectRect(CRect(m_ball_pos, SIZE{ m_ball_radius, m_ball_radius }), m_wall_rect)) {
		if (m_ball_pos.x + m_ball_radius >= m_wall_rect.left) {
			m_ball_pos.x -= m_ball_pos.x + m_ball_radius - m_wall_rect.left;
			m_ball_velocity.x *= -1;
		} else if (m_ball_pos.x - m_ball_radius < m_wall_rect.right) {
			m_ball_pos.x = m_wall_rect.right - (m_ball_pos.x - m_ball_radius);
			m_ball_velocity.x *= -1;
		}
	}
}


int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rect;
	GetClientRect(&rect);
	m_ball_pos.x = rect.Width() / 2;
	m_ball_pos.y = rect.Height() / 2;

	// 시계 타이머 설정(함수 호출 주기 설정)
	SetTimer(kTimerClock, /* ms */ 1000, nullptr);
	m_timer_event_listeners.Add(kTimerClock, [this](){ m_current_time = GetSystemTimeAndDate(); });

	// 공 튀기기 타이머 설정
	SetTimer(kTimerPhysics, /* ms */ 10, nullptr);
	m_timer_event_listeners.Add(kTimerPhysics, [this]() { CalculateBall(); });

	// 마우스 이동 이벤트 리스너 추가
	m_mouse_event_listeners.Add(kMouseMove, [this](auto, auto p) { m_mouse_pos = p; });

	m_mouse_event_listeners.Add(kMouseMove, [this](auto, auto p) {
		auto w = m_wall_rect.Width();
		auto h = m_wall_rect.Height();
		m_wall_rect.left = p.x - w / 2;
		m_wall_rect.right = p.x + w / 2;
		m_wall_rect.top = p.y - h / 2;
		m_wall_rect.bottom = p.y + h / 2;
	});

	// 마우스 클릭 이벤트 리스너
	m_mouse_event_listeners.Add(kMouseLButtonDown, [this](auto, auto p) { m_mouse_event = "LButtonDown"; });
	m_mouse_event_listeners.Add(kMouseLButtonUp, [this](auto, auto p) { m_mouse_event = "LButtonUp"; });
	m_mouse_event_listeners.Add(kMouseLButtonDblClk, [this](auto, auto p) { m_mouse_event = "LButtonDblClk"; });

	m_mouse_event_listeners.Add(kMouseRButtonDown, [this](auto, auto p) { m_mouse_event = "RButtonDown"; });
	m_mouse_event_listeners.Add(kMouseRButtonUp, [this](auto, auto p) { m_mouse_event = "RButtonUp"; });
	m_mouse_event_listeners.Add(kMouseRButtonDblClk, [this](auto, auto p) { m_mouse_event = "RButtonDblClk"; });

	m_mouse_event_listeners.Add(kMouseMButtonDown, [this](auto, auto p) { m_mouse_event = "MButtonDown"; });
	m_mouse_event_listeners.Add(kMouseMButtonUp, [this](auto, auto p) { m_mouse_event = "MButtonUp"; });
	m_mouse_event_listeners.Add(kMouseMButtonDblClk, [this](auto, auto p) { m_mouse_event = "MButtonDblClk"; });

	// 우클릭 시 그리기 모드 취소
	m_mouse_event_listeners.Add(kMouseRButtonDown, [this](auto, auto p) { m_toolbar_mode = kToolbarNone; });

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
	m_keyboard = nChar;
	m_toolbar_mode = kToolbarNone;
	switch (nChar) {
		case VK_ESCAPE:
			m_toolbar_mode = kToolbarNone;
			break;
	}
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CChildView::OnTimer(UINT_PTR nIDEvent) {
	m_timer_event_listeners(static_cast<TimerEvent>(nIDEvent));
	CWnd::Invalidate();
	CWnd::OnTimer(nIDEvent);
}

// 
void CChildView::OnSelectArea() {
	m_toolbar_mode = (m_toolbar_mode == kToolbarSelectArea ? kToolbarNone : kToolbarSelectArea);
}
void CChildView::OnUpdateSelectarea(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_toolbar_mode == kToolbarSelectArea);
}


void CChildView::OnDrawLine() {
	m_toolbar_mode = (m_toolbar_mode == kToolbarDrawLine ? kToolbarNone : kToolbarDrawLine);
}
void CChildView::OnUpdateDrawLine(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_toolbar_mode == kToolbarDrawLine);
}

void CChildView::OnDrawCurve() {
	m_toolbar_mode = (m_toolbar_mode == kToolbarDrawCurve ? kToolbarNone : kToolbarDrawCurve);
}
void CChildView::OnUpdateDrawCurve(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_toolbar_mode == kToolbarDrawCurve);
}

void CChildView::OnRemoveSelected() {
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}


void CChildView::OnDrawRectangle() {
	m_toolbar_mode = (m_toolbar_mode == kToolbarDrawRectangle ? kToolbarNone : kToolbarDrawRectangle);
}
void CChildView::OnUpdateDrawRectangle(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_toolbar_mode == kToolbarDrawRectangle);
}


void CChildView::OnDrawCircle() {
	m_toolbar_mode = (m_toolbar_mode == kToolbarDrawCircle? kToolbarNone : kToolbarDrawCircle);
}
void CChildView::OnUpdateDrawCircle(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_toolbar_mode == kToolbarDrawCircle);
}