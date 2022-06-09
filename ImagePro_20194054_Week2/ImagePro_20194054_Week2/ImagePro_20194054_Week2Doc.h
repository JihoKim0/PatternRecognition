
// ImagePro_20194054_Week2Doc.h: CImagePro20194054Week2Doc 클래스의 인터페이스
//


#pragma once

typedef struct {
	double re;
	double im;
}complex_num;

#define MAX_CONTOURS 1024

typedef struct contourStruct
{
	int* x; // 윤곽선의 각 정점들의 x좌표
	int* y; // 윤곽선의 각 정점들의 y좌표
	int nVertices; // 윤곽선의 정점의 수
	double* angle; // 윤곽선의 각 정점들의 내각
	double cx, cy; // 윤곽선 내부 영역의 무게 중심 위치
}ipCONTOUR;

typedef struct tag_ipPOINT {
	int x;
	int y;
}ipPOINT;

typedef struct tag_ipSquares {
	ipPOINT pt[4];
} ipSQUARE;

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
	
	complex_num **fft_result;	//FFT 결과를 저장하기 위한 기억 장소 포인터
	complex_num** ifft_result;	//IFFT 결과를 저장하기 위한 기억 장소 포인터

	ipCONTOUR contours[MAX_CONTOURS];
	int nContour;

	ipSQUARE squares[1024];
	int nSquares;

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
	void FFT_2D();
	void FFT_1D(complex_num x[], int N, int log2N);
	void shuffle_data(complex_num x[], int N, int log2N);
	int reverse_bit_order(int index, int log2N);
	void butterfly_computation(complex_num x[], int N, int log2N);
	void Inverse_FFT_2D();
	void Inverse_FFT_1D(complex_num x[], int N, int log2N);
	void inverse_butterfly_computation(complex_num x[], int N, int log2N);
	void LowPassFilter();
	void HighPassFilter();
	void NoiseRemove();
	void ZoomInDialog(float zoom_in_ratio);
	void PixelAddValue(int added_value);
	void GeometryWarpingMouse(int Ax, int Ay, int Bx, int By);
	void FindContours(unsigned char** binaryImg, unsigned char** contourImg);
	double ContourLength(ipCONTOUR* contour);
	double ContourArea(ipCONTOUR* contour);
	void ContourCentroid(ipCONTOUR* contour, double contour_area);
	int IsConvex(ipCONTOUR* contour);
	void GeometricalFeatures();
	void GeometricalFeaturesLineApprox();
	ipCONTOUR* ApproxPoly(int k, double eps);
	ipCONTOUR* CreateContour();
	double ContourAngle(ipCONTOUR* contour);
	double angle(ipPOINT* p1, ipPOINT* p2, ipPOINT* p0);
	void FindTriangles();
	void MarkerRecognition();
	unsigned char** CreateImage(int width, int height, int depth);
	int ContourDirection(ipCONTOUR* contour);
	void FreeContour(ipCONTOUR* contour);
	void Warping(unsigned char** srcImg, unsigned char** dstImg, int width, int height, ipPOINT pt[]);
	void Resize(unsigned char** srcImg, unsigned char** dstImg, int srcW, int srcH, int dstW, int dstH, int depth);
	void FreeImage(unsigned char** image, int height);
	unsigned char** ReadBMPImage(char* fname, int* width, int* height, int* depth);
};
 