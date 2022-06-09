﻿
// ImageProView.h: CImageProView 클래스의 인터페이스
//

#pragma once


class CImageProView : public CScrollView
{
protected: // serialization에서만 만들어집니다.
	CImageProView() noexcept;
	DECLARE_DYNCREATE(CImageProView)

// 특성입니다.
public:
	CImagePro20194054Week2Doc* GetDocument() const;
	int viewMode;
	char AVIFileName[256];
	int operation;
	int mouse_input_started;
	int Ax, Ay, Bx, By;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // 생성 후 처음 호출되었습니다.
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 구현입니다.
public:
	virtual ~CImageProView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPixelAdd();
	afx_msg void OnPixelHistoEq();
	afx_msg void OnPixelTwoImageAdd();
	afx_msg void OnRegionSharpening();
	afx_msg void OnRegionMeaning();
	afx_msg void OnRegionSobel();
	afx_msg void OnRegionMedian();
	afx_msg void OnErosion();
	afx_msg void OnDilation();
	afx_msg void OnOpening();
	afx_msg void OnClosing();
	afx_msg void OnGeometryZoominPixelCopy();
	afx_msg void OnGeometryZoominInterpolation();
	afx_msg void OnGeometryZoomoutSubsampling();
	afx_msg void OnGeometryZoomoutAvg();
	afx_msg void OnGeometryRotate();
	afx_msg void OnGeometryMirror();
	afx_msg void OnGeometryFlip();
	afx_msg void OnGeometryDiagonal();
	afx_msg void OnGeometryWarping();
	afx_msg void OnGeometryMorphing();
	afx_msg void OnAviView();
	void loadAVIFile(CDC* pDC);
	afx_msg void OnCupTracking();
	void Find_Cup(unsigned char * image);
	void draw_rect(unsigned char* image, int xmin, int ymin, int xmax, int ymax);
	int compute_hue(unsigned char* image, int x, int y);
	afx_msg void OnFft2d();
	afx_msg void OnInverseFft2d();
	afx_msg void OnLowPassFilter();
	afx_msg void OnHighPassFilter();
	afx_msg void OnNoiseRemove();
	afx_msg void OnZoomInDialog();
	afx_msg void OnPixelAddSlider();
	afx_msg void OnGeometryWarpingMouse();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnFindContours();
	afx_msg void OnGeometricalFeatures();
	afx_msg void OnGeometricalFeaturesLineApprox();
	afx_msg void OnFindTriangles();
	afx_msg void OnMarkerRecognition();
};

#ifndef _DEBUG  // ImageProView.cpp의 디버그 버전
inline CImagePro20194054Week2Doc* CImageProView::GetDocument() const
   { return reinterpret_cast<CImagePro20194054Week2Doc*>(m_pDocument); }
#endif

