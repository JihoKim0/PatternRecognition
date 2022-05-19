
// ImageProView.cpp: CImageProView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "ImagePro_20194054_Week2.h"
#endif

#include "ImagePro_20194054_Week2Doc.h"
#include "ImageProView.h"

#include <vfw.h>

#include "CInputDlg.h"
#include "CSliderDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TWO_IMAGES 1
#define THREE_IMAGES 2
#define TWO_IMAGES_SCALED 4
#define MORPHING 8
#define AVI_FILE 16
#define CUP_TRACKING 4

// CImageProView

IMPLEMENT_DYNCREATE(CImageProView, CScrollView)

BEGIN_MESSAGE_MAP(CImageProView, CScrollView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CImageProView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_PIXEL_ADD, &CImageProView::OnPixelAdd)
	ON_COMMAND(ID_PIXEL_HISTO_EQ, &CImageProView::OnPixelHistoEq)
	ON_COMMAND(ID_PIXEL_TWO_IMAGE_ADD, &CImageProView::OnPixelTwoImageAdd)
	ON_COMMAND(ID_REGION_SHARPENING, &CImageProView::OnRegionSharpening)
	ON_COMMAND(ID_REGION_MEANING, &CImageProView::OnRegionMeaning)
	ON_COMMAND(ID_REGION_SOBEL, &CImageProView::OnRegionSobel)
	ON_COMMAND(ID_REGION_MEDIAN, &CImageProView::OnRegionMedian)
	ON_COMMAND(ID_EROSION, &CImageProView::OnErosion)
	ON_COMMAND(ID_DILATION, &CImageProView::OnDilation)
	ON_COMMAND(ID_OPENING, &CImageProView::OnOpening)
	ON_COMMAND(ID_Closing, &CImageProView::OnClosing)
	ON_COMMAND(ID_GEOMETRY_ZOOMIN_PIXEL_COPY, &CImageProView::OnGeometryZoominPixelCopy)
	ON_COMMAND(ID_GEOMETRY_ZOOMIN_INTERPOLATION, &CImageProView::OnGeometryZoominInterpolation)
	ON_COMMAND(ID_GEOMETRY_ZOOMOUT_SUBSAMPLING, &CImageProView::OnGeometryZoomoutSubsampling)
	ON_COMMAND(ID_GEOMETRY_ZOOMOUT_AVG, &CImageProView::OnGeometryZoomoutAvg)
	ON_COMMAND(ID_GEOMETRY_ROTATE, &CImageProView::OnGeometryRotate)
	ON_COMMAND(ID_GEOMETRY_MIRROR, &CImageProView::OnGeometryMirror)
	ON_COMMAND(ID_GEOMETRY_FLIP, &CImageProView::OnGeometryFlip)
	ON_COMMAND(ID_GEOMETRY_DIAGONAL, &CImageProView::OnGeometryDiagonal)
	ON_COMMAND(ID_GEOMETRY_WARPING, &CImageProView::OnGeometryWarping)
	ON_COMMAND(ID_GEOMETRY_MORPHING, &CImageProView::OnGeometryMorphing)
	ON_COMMAND(ID_AVI_VIEW, &CImageProView::OnAviView)
	ON_COMMAND(ID_CUP_TRACKING, &CImageProView::OnCupTracking)
	ON_COMMAND(ID_FFT_2D, &CImageProView::OnFft2d)
	ON_COMMAND(ID_Inverse_FFT_2D, &CImageProView::OnInverseFft2d)
	ON_COMMAND(ID_LOW_PASS_FILTER, &CImageProView::OnLowPassFilter)
	ON_COMMAND(ID_HIGH_PASS_FILTER, &CImageProView::OnHighPassFilter)
	ON_COMMAND(ID_NOISE_REMOVE, &CImageProView::OnNoiseRemove)
	ON_COMMAND(ID_ZOOM_IN_DIALOG, &CImageProView::OnZoomInDialog)
	ON_COMMAND(ID_PIXEL_ADD_SLIDER, &CImageProView::OnPixelAddSlider)
END_MESSAGE_MAP()

// CImageProView 생성/소멸
CImageProView::CImageProView() noexcept
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CImageProView::~CImageProView()
{
}

BOOL CImageProView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CScrollView::PreCreateWindow(cs);
}

// CImageProView 그리기

void CImageProView::OnDraw(CDC* pDC)
{
	CImagePro20194054Week2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (viewMode == AVI_FILE) {
		loadAVIFile(pDC);
		return;
	}
	if (pDoc->inputImg == NULL)
		return;
	if (pDoc->depth == 1) {	//흑백 영상 출력
		for (int y = 0; y < pDoc->imageHeight; y++)		//print original image
			for (int x = 0; x < pDoc->imageWidth; x++)	//RGB에 같은 값을 넣어서 흑백
				pDC->SetPixel(x, y, RGB(pDoc->inputImg[y][x],
					pDoc->inputImg[y][x],
					pDoc->inputImg[y][x]));

		if (viewMode == THREE_IMAGES) {
			for (int y = 0; y < pDoc->imageHeight; y++)
				for (int x = 0; x < pDoc->imageWidth; x++)
					pDC->SetPixel(x + pDoc->imageWidth + 30, y, RGB(pDoc->inputImg2[y][x],
						pDoc->inputImg2[y][x],
						pDoc->inputImg2[y][x]));
			for (int y = 0; y < pDoc->imageHeight; y++)		//print result image
				for (int x = 0; x < pDoc->imageWidth; x++)
					pDC->SetPixel(x + pDoc->imageWidth * 2 + 60, y, RGB(pDoc->resultImg[y][x],
						pDoc->resultImg[y][x],
						pDoc->resultImg[y][x]));
		}
		else if (viewMode == TWO_IMAGES_SCALED) {
			for (int y = 0; y < pDoc->gImageHeight; y++) // 크기가 변한 결과 영상 출력
				for (int x = 0; x < pDoc->gImageWidth; x++)
					pDC->SetPixel(x + pDoc->imageWidth + 30, y,
						RGB(pDoc->gResultImg[y][x],
							pDoc->gResultImg[y][x],
							pDoc->gResultImg[y][x]));
		}
		else if (viewMode == MORPHING) {
			for (int y = 0; y < pDoc->imageHeight; y++)       // 두번째입력영상출력
				for(int x=0; x< pDoc->imageWidth; x++)
					pDC->SetPixel(x+pDoc->imageWidth+30, y,
						RGB(pDoc->inputImg2[y][x],
							pDoc->inputImg2[y][x],
							pDoc->inputImg2[y][x])); 
			
			for (int y = 0; y < pDoc->imageHeight; y++)       // 모핑결과출력
				for(int x=0; x< pDoc->imageWidth; x++)
					pDC->SetPixel(x+pDoc->imageWidth*2+60,y,
						RGB(pDoc->morphedImg[3][y][x],
							pDoc->morphedImg[3][y][x],
							pDoc->morphedImg[3][y][x]));
		} 
		else {
			for (int y = 0; y < pDoc->imageHeight; y++)		//print result image
				for (int x = 0; x < pDoc->imageWidth; x++)
					pDC->SetPixel(x + pDoc->imageWidth + 30, y, RGB(pDoc->resultImg[y][x],
						pDoc->resultImg[y][x],
						pDoc->resultImg[y][x]));
		}

	}
	else if (pDoc->depth == 3) {	//컬러 영상 출력
		for (int y = 0; y < pDoc->imageHeight; y++)
			for (int x = 0; x < pDoc->imageWidth; x++)
				pDC->SetPixel(x, y, RGB(pDoc->inputImg[y][3 * x],
					pDoc->inputImg[y][3 * x + 1],
					pDoc->inputImg2[y][3 * x + 2]));
		if (viewMode == THREE_IMAGES) {
			for (int y = 0; y < pDoc->imageHeight; y++)   // 두번째입력영상출력
				for (int x = 0; x < pDoc->imageWidth; x++)
					pDC->SetPixel(x + pDoc->imageWidth + 30, y, RGB(pDoc->inputImg2[y][3 * x],
						pDoc->inputImg2[y][3 * x + 1], pDoc->inputImg2[y][3 * x + 2]));
			for (int y = 0; y < pDoc->imageHeight; y++)       // 결과영상출력
				for (int x = 0; x < pDoc->imageWidth; x++)
					pDC->SetPixel(x + pDoc->imageWidth * 2 + 60, y,
						RGB(pDoc->resultImg[y][3 * x],
							pDoc->resultImg[y][3 * x + 1],
							pDoc->resultImg[y][3 * x + 2]));
		}
		else {
			for (int y = 0; y < pDoc->imageHeight; y++)       // 결과영상출력
				for (int x = 0; x < pDoc->imageWidth; x++)
					pDC->SetPixel(x + pDoc->imageWidth + 30, y,
						RGB(pDoc->resultImg[y][3 * x],
							pDoc->resultImg[y][3 * x + 1], pDoc->resultImg[y][3 * x + 2]));
		}
	}
}

void CImageProView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: 이 뷰의 전체 크기를 계산합니다.
	sizeTotal.cx = 2048;
	sizeTotal.cy = 1024;
	SetScrollSizes(MM_TEXT, sizeTotal);
}


// CImageProView 인쇄


void CImageProView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CImageProView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CImageProView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CImageProView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}

void CImageProView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CImageProView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CImageProView 진단

#ifdef _DEBUG
void CImageProView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CImageProView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CImagePro20194054Week2Doc* CImageProView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CImagePro20194054Week2Doc)));
	return (CImagePro20194054Week2Doc*)m_pDocument;
}
#endif //_DEBUG


// CImageProView 메시지 처리기


void CImageProView::OnPixelAdd()
{
	CImagePro20194054Week2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (pDoc->inputImg == NULL) return;// 입력 영상 있는지 검사

	pDoc->PixelAdd();
	viewMode = TWO_IMAGES;
	Invalidate(FALSE);
}


void CImageProView::OnPixelHistoEq()
{
	CImagePro20194054Week2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (pDoc->inputImg == NULL) return;

	pDoc->PixelHistoEq();
	viewMode = TWO_IMAGES;
	Invalidate(FALSE);	//화면갱신
}


void CImageProView::OnPixelTwoImageAdd()
{
	CImagePro20194054Week2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	pDoc->PixelTwoImageAdd();

	viewMode = THREE_IMAGES;

	Invalidate(FALSE);
}


void CImageProView::OnRegionSharpening()
{
	CImagePro20194054Week2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	if (pDoc->inputImg == NULL) return;
	pDoc->RegionSharpening();
	viewMode = TWO_IMAGES;
	Invalidate(FALSE);
}


void CImageProView::OnRegionMeaning()
{
	CImagePro20194054Week2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (pDoc->inputImg == NULL) return;
	pDoc->RegionMeaning();
	viewMode = TWO_IMAGES;
	Invalidate(FALSE);
}


void CImageProView::OnRegionSobel()
{
	CImagePro20194054Week2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc->inputImg == NULL) return;
	pDoc->RegionSobel();
	viewMode = TWO_IMAGES;
	Invalidate(FALSE);
}



void CImageProView::OnRegionMedian()
{
	CImagePro20194054Week2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc->inputImg == NULL) return;
	pDoc->RegionMedian();
	viewMode = TWO_IMAGES;
	Invalidate(FALSE);
}


void CImageProView::OnErosion()
{
	CImagePro20194054Week2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc->inputImg == NULL) return;
	pDoc->Erosion();
	viewMode = TWO_IMAGES;
	Invalidate(FALSE);
}


void CImageProView::OnDilation()
{
	CImagePro20194054Week2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc->inputImg == NULL) return;
	pDoc->Dilation();
	viewMode = TWO_IMAGES;
	Invalidate(FALSE);
}


void CImageProView::OnOpening()
{
	CImagePro20194054Week2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc->inputImg == NULL) return;
	pDoc->Opening();
	viewMode = TWO_IMAGES;
	Invalidate(FALSE);
}


void CImageProView::OnClosing()
{
	CImagePro20194054Week2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc->inputImg == NULL) return;
	pDoc->Closing();
	viewMode = TWO_IMAGES;
	Invalidate(FALSE);
}


void CImageProView::OnGeometryZoominPixelCopy()
{
	CImagePro20194054Week2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc->inputImg == NULL) return;
	pDoc->GeometryZoominPixelCopy();
	viewMode = TWO_IMAGES_SCALED;
	Invalidate(FALSE);
}


void CImageProView::OnGeometryZoominInterpolation()
{
	CImagePro20194054Week2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc->inputImg == NULL) return;
	pDoc->GeometryZoominInterpolation();
	viewMode = TWO_IMAGES_SCALED;
	Invalidate(FALSE);
}


void CImageProView::OnGeometryZoomoutSubsampling()
{
	CImagePro20194054Week2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc->inputImg == NULL) return;
	pDoc->GeometryZoomoutSubsampling();
	viewMode = TWO_IMAGES_SCALED;
	Invalidate(FALSE);
}


void CImageProView::OnGeometryZoomoutAvg()
{
	CImagePro20194054Week2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc->inputImg == NULL) return;
	pDoc->GeometryZoomoutAvg();
	viewMode = TWO_IMAGES_SCALED;
	Invalidate(FALSE);
}


void CImageProView::OnGeometryRotate()
{
	CImagePro20194054Week2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc->inputImg == NULL) return;
	pDoc->GeometryRotate();
	viewMode = TWO_IMAGES_SCALED;
	Invalidate(FALSE);
}


void CImageProView::OnGeometryMirror()
{
	CImagePro20194054Week2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc->inputImg == NULL) return;
	pDoc->GeometryMirror();
	viewMode = TWO_IMAGES;
	Invalidate(FALSE);
}


void CImageProView::OnGeometryFlip()
{
	CImagePro20194054Week2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc->inputImg == NULL) return;
	pDoc->GeometryFlip();
	viewMode = TWO_IMAGES;
	Invalidate(FALSE);
}


void CImageProView::OnGeometryDiagonal()
{
	CImagePro20194054Week2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc->inputImg == NULL) return;
	pDoc->GeometryDiagonal();
	viewMode = TWO_IMAGES;
	Invalidate(FALSE);
}


void CImageProView::OnGeometryWarping()
{
	CImagePro20194054Week2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc->inputImg == NULL) return;
	pDoc->GeometryWarping();
	viewMode = TWO_IMAGES;
	Invalidate(FALSE);
}


void CImageProView::OnGeometryMorphing()
{
	CImagePro20194054Week2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc->inputImg == NULL) return;
	pDoc->GeometryMorphing();
	viewMode = MORPHING;
	Invalidate(FALSE);
}


void CImageProView::OnAviView()
{
	CFile file;
	CFileDialog dlg(TRUE);

	if (dlg.DoModal() == IDOK) {
		strcpy_s(AVIFileName, dlg.GetPathName());
		viewMode = AVI_FILE;
	}
	Invalidate(FALSE);
}


void CImageProView::loadAVIFile(CDC * pDC)
{
	LPBITMAPINFOHEADER pbmih;
	PAVIFILE pavi = NULL;
	PAVISTREAM pstm = NULL;
	PGETFRAME pfrm = NULL;
	AVIFILEINFO fi;
	AVISTREAMINFO si;
	int stm;
	int frame;
	int x, y;
	unsigned char* image;

	AVIFileInit();
	AVIFileOpen(&pavi, AVIFileName, OF_READ | OF_SHARE_DENY_NONE,
		NULL);
	AVIFileInfo(pavi, &fi, sizeof(AVIFILEINFO));
	for (stm = 0; stm < fi.dwStreams; stm++) {
		AVIFileGetStream(pavi, &pstm, 0, stm);
		AVIStreamInfo(pstm, &si, sizeof(si));
		if (si.fccType == streamtypeVIDEO) {
			pfrm = AVIStreamGetFrameOpen(pstm, NULL);
			for (frame = 0; frame < si.dwLength; frame++) {
				pbmih = (LPBITMAPINFOHEADER)AVIStreamGetFrame(pfrm, frame);

				if (!pbmih) continue;

				image = (unsigned char*)((LPSTR)pbmih + (WORD)pbmih->biSize);
				if (operation == CUP_TRACKING) Find_Cup(image);
				for (y = 0; y < fi.dwHeight; y++)
					for (x = 0; x < fi.dwWidth; x++)
						pDC->SetPixel(x, fi.dwHeight - y - 1,
							RGB(image[(y * fi.dwWidth + x) * 3 + 2],
								image[(y * fi.dwWidth + x) * 3 + 1],
								image[(y * fi.dwWidth + x) * 3]));
			}
		}
	}
	AVIStreamGetFrameClose(pfrm);
	AVIStreamRelease(pstm);
	AVIFileRelease(pavi);
	AVIFileExit();
}


void CImageProView::OnCupTracking()
{
	CFile file;
	CFileDialog dlg(TRUE);

	if (dlg.DoModal() == IDOK) {
		strcpy_s(AVIFileName, dlg.GetPathName());
		viewMode = AVI_FILE;
		operation = CUP_TRACKING;
	}
	Invalidate(FALSE);
}


void CImageProView::Find_Cup(unsigned char * image)
{
	int x, y;
	int hue_col_max;
	int hue_row_max;
	int count;
	int hue_col_avg[320];
	int hue_row_avg[240];
	int Cup_xmin;
	int Cup_xmax;
	int Cup_ymin;
	int Cup_ymax;
	int hue_row_thresh;
	int hue_col_thresh;
	int imageWidth = 320;
	int imageHeight = 240;

	// row, column 별로 hue값 평균과 명암값 평균 계산
	for (x = 0; x < imageWidth; x++) {
		hue_col_avg[x] = 0;
		for (y = 0; y < imageHeight; y++) {
			hue_col_avg[x] += compute_hue(image, x, y);
		}
		hue_col_avg[x] = hue_col_avg[x] / imageHeight;
	}
	for (y = 0; y < imageHeight; y++) {
		hue_row_avg[y] = 0;
		for (x = 0; x < imageWidth; x++) {
			hue_row_avg[y] += compute_hue(image, x, y);
		}
		hue_row_avg[y] = hue_row_avg[y] / imageWidth;
	}

	// 최대값 계산
	hue_col_max = 0;
	for (x = 0; x < imageWidth; x++)
		if (hue_col_avg[x] > hue_col_max)
			hue_col_max = hue_col_avg[x];
	hue_row_max = 0;
	for (y = 0; y < imageHeight; y++)
		if (hue_row_avg[y] > hue_row_max)
			hue_row_max = hue_row_avg[y];
	hue_col_thresh = (int)(hue_col_max * 0.6);
	hue_row_thresh = (int)(hue_row_max * 0.6);

	// column, row 모두 hue값이 임계치 이상인 부분 검출
	Cup_xmax = imageWidth - 1;
	Cup_xmin = 0;
	Cup_ymax = imageHeight - 1;
	Cup_ymin = 0;

	count = 0;
	for (x = 0; x < imageWidth; x++) {
		if (hue_col_avg[x] > hue_col_thresh)
			count++;
		else
			count = 0;
		if (count > 10) {
			Cup_xmin = x;
			break;
		}
	}
	count = 0;
	for (x = imageWidth
		- 1; x > 0; x--) {
		if (hue_col_avg[x] > hue_col_thresh)
			count++;
		else
			count = 0;
		if (count > 10) {
			Cup_xmax = x;
			break;
		}
	}

	count = 0;
	for (y = 0; y < imageHeight; y++) {
		if (hue_row_avg[y] > hue_row_thresh)
			count++;
		else
			count = 0;
		if (count > 10) {
			Cup_ymin = y;
			break;
		}
	}
	count = 0;
	for (y = imageHeight
		- 1; y > 0; y--) {
		if (hue_row_avg[y] > hue_row_thresh)
			count++;
		else
			count = 0;
		if (count > 10) {
			Cup_ymax = y;
			break;
		}
	}
	draw_rect(image, Cup_xmin, Cup_ymin, Cup_xmax, Cup_ymax);
}


void CImageProView::draw_rect(unsigned char* image, int xmin, int ymin, int xmax, int ymax)
{
	int x, y;
	for (x = xmin; x <= xmax; x++) {
		image[(ymin * 320 * 3) + x * 3] = 255;
		image[(ymin * 320 * 3) + x * 3 + 1] = 0;
		image[(ymin * 320 * 3) + x * 3 + 2] = 0;
		image[(ymax * 320 * 3) + x * 3] = 0;
		image[(ymax * 320 * 3) + x * 3 + 1] = 255;
		image[(ymax * 320 * 3) + x * 3 + 2] = 0;
	}
	for (y = ymin; y <= ymax; y++) {
		image[(y * 320 * 3) + xmin * 3] = 0;
		image[(y * 320 * 3) + xmin * 3 + 1] = 0;
		image[(y * 320 * 3) + xmin * 3 + 2] = 255;
		image[(y * 320 * 3) + xmax * 3] = 255;
		image[(y * 320 * 3) + xmax * 3 + 1] = 255;
		image[(y * 320 * 3) + xmax * 3 + 2] = 255;
	}
}


int CImageProView::compute_hue(unsigned char* image, int x, int y)
{
	int hue;
	float b, g, r;

	b = (float)image[3 * y * 320 + 3 * x];
	g = (float)image[3 * y * 320 + 3 * x + 1];
	r = (float)image[3 * y * 320 + 3 * x + 2];

	if (r > g)	hue = 255 - (int)(255 * (g / r));
	else hue = 0;

	return hue;
}


void CImageProView::OnFft2d()
{
	CImagePro20194054Week2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc->inputImg == NULL) return;
	pDoc->FFT_2D();
	viewMode = TWO_IMAGES;
	Invalidate(FALSE);
}


void CImageProView::OnInverseFft2d()
{
	CImagePro20194054Week2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (pDoc->inputImg == NULL) return;
	pDoc->Inverse_FFT_2D();
	viewMode = TWO_IMAGES;
	Invalidate(FALSE);
}

void CImageProView::OnLowPassFilter()
{
	CImagePro20194054Week2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (pDoc->inputImg == NULL) return;
	pDoc->LowPassFilter();
	viewMode = TWO_IMAGES;
	Invalidate(FALSE);
}


void CImageProView::OnHighPassFilter()
{
	CImagePro20194054Week2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (pDoc->inputImg == NULL) return;
	pDoc->HighPassFilter();
	viewMode = TWO_IMAGES;
	Invalidate(FALSE);
}


void CImageProView::OnNoiseRemove()
{
	CImagePro20194054Week2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (pDoc->inputImg == NULL) return;
	pDoc->NoiseRemove();
	viewMode = TWO_IMAGES;
	Invalidate(FALSE);
}


void CImageProView::OnZoomInDialog()
{
	CImagePro20194054Week2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc->inputImg == NULL) return;
	CInputDlg inputDlg; // 대화상자 클래스의 인스턴스 생성
	inputDlg.DoModal(); // 대화상자가 화면에 나타나도록 함
	// 확대 함수 호출하고 사용자가 입력한 확대 비율 전달
	pDoc->ZoomInDialog(inputDlg.m_zoom_in_ratio);
	viewMode = TWO_IMAGES_SCALED;
	Invalidate(FALSE);
}


void CImageProView::OnPixelAddSlider()
{
	CImagePro20194054Week2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (pDoc->inputImg == NULL) return;

	CSliderDlg sliderDlg; // 대화상자 인스턴스 생성
	sliderDlg.DoModal(); // 대화상자가 나타나도록 함

}
