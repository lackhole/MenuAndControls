
// ChildView.h: CChildView 클래스의 인터페이스
//


#pragma once

#include <string>
#include <functional>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include "EventListener.h"
#include "shape.h"
#include "shape_builder.h"

// CChildView 창

class CChildView : public CWnd
{
public:
	CChildView();

	enum ToolbarMode {
		kToolbarNone,
		kToolbarSelectArea,
		kToolbarRemoveSelected,
		kToolbarDrawLine,
		kToolbarDrawCurve,
		kToolbarDrawRectangle,
		kToolbarDrawCircle,
	};

	ToolbarMode m_toolbar_mode = kToolbarNone;

public:

	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

public:
	virtual ~CChildView();

protected:
	afx_msg void OnPaint();
	afx_msg void OnMyPaint(CDC* dc);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);


	enum TimerEvent {
		kTimerClock,
		kTimerPhysics,
	};

	afx_msg void OnTimer(UINT_PTR nIDEvent);

	EventListener<TimerEvent> m_timer_event_listeners;

	enum MouseEvent {
		kMouseMove,

		kMouseLButtonDown,
		kMouseLButtonUp,
		kMouseLButtonDblClk,

		kMouseRButtonDown,
		kMouseRButtonUp,
		kMouseRButtonDblClk,

		kMouseMButtonDown,
		kMouseMButtonUp,
		kMouseMButtonDblClk,
	};
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);

	EventListener<MouseEvent, UINT, CPoint> m_mouse_event_listeners;

	afx_msg void OnSelectArea();
	afx_msg void OnUpdateSelectarea(CCmdUI* pCmdUI);
	afx_msg void OnDrawLine();
	afx_msg void OnUpdateDrawLine(CCmdUI* pCmdUI);
	afx_msg void OnRemoveSelected();
	afx_msg void OnDrawRectangle();
	afx_msg void OnUpdateDrawRectangle(CCmdUI* pCmdUI);
	afx_msg void OnDrawCircle();
	afx_msg void OnUpdateDrawCircle(CCmdUI* pCmdUI);
	afx_msg void OnDrawCurve();
	afx_msg void OnUpdateDrawCurve(CCmdUI* pCmdUI);
	EventListener<ToolbarMode> m_toolbar_listeners;

	using shape_type = std::unique_ptr<ShapeBase>;
	using shape_list_type = std::list<shape_type>;
	using shape_iterator = typename shape_list_type::iterator;

	shape_list_type m_shapes;
	shape_type m_shape_preview;

	void UnselectToolbar();

	void RemoveSelectedObjects();

	std::vector<shape_iterator> FindObjectsInArea();
	void CreateSelectedHighlights();

	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	EventListener<UINT, UINT, UINT> m_keyboard_listeners;

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	std::unique_ptr<ShapeBuilderBase> m_shape_builder;

	std::vector<typename std::list<std::unique_ptr<ShapeBase>>::iterator> m_selected_shapes;
	std::vector<ShapeRectangle> m_selected_highlights;
};

