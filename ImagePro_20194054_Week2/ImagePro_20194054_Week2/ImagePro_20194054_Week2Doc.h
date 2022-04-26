﻿
// ImagePro_20194054_Week2Doc.h: CImagePro20194054Week2Doc 클래스의 인터페이스
//


#pragma once


class CImagePro20194054Week2Doc : public CDocument
{
protected: // serialization에서만 만들어집니다.
	CImagePro20194054Week2Doc() noexcept;
	DECLARE_DYNCREATE(CImagePro20194054Week2Doc)

// 특성입니다.
public:
	unsigned char **inputImg;
	unsigned char **inputImg2;
	unsigned char **resultImg;
	int imageWidth;
	int imageHeight;
	int depth;
	
	int gImageWidth;
	int gImageHeight;
	unsigned char** gResultImg;

	//모핑 결과 저장을 위한 변수
	unsigned char** morphedImg[10];

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 구현입니다.
public:
	virtual ~CImagePro20194054Week2Doc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 검색 처리기에 대한 검색 콘텐츠를 설정하는 도우미 함수
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	void PixelAdd();
	void PixelHistoEq();
	void PixelTwoImageAdd();
	void LoadTwoImages();
	void LoadImageFile(CArchive &ar);
	void LoadSecondImageFile(CArchive &ar);
	void RegionSharpening();
	void Convolve(unsigned char** inputImg, unsigned char** resultImg,
		int cols, int rows, float mask[][3], int bias, int depth);
	void RegionMeaning();
	void RegionSobel();
	void RegionMedian();
	void Erosion();
	void Dilation();
	void Opening();
	void CopyResultToInput();
	void Closing();
	void GeometryZoominPixelCopy();
	void GeometryZoominInterpolation();
	void GeometryZoomoutSubsampling();
	void GeometryZoomoutAvg();
	void GeometryRotate();
	void GeometryMirror();
	void GeometryFlip();
	void GeometryDiagonal();
	void GeometryWarping();
	void GeometryMorphing();
};
 