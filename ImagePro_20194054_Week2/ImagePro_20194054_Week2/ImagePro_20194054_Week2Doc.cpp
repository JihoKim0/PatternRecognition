
// ImagePro_20194054_Week2Doc.cpp: CImagePro20194054Week2Doc 클래스의 구현
//

#include "pch.h"
#include "framework.h"
#include <math.h>
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "ImagePro_20194054_Week2.h"
#endif

#include "ImagePro_20194054_Week2Doc.h"

#include <propkey.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CImagePro20194054Week2Doc

IMPLEMENT_DYNCREATE(CImagePro20194054Week2Doc, CDocument)

BEGIN_MESSAGE_MAP(CImagePro20194054Week2Doc, CDocument)
	ON_COMMAND(ID_REGION_MEDIAN, &CImagePro20194054Week2Doc::RegionMedian)
END_MESSAGE_MAP()

typedef struct {
	int Px;
	int Py;
	int Qx;
	int Qy;
}control_line;

// CImagePro20194054Week2Doc 생성/소멸

CImagePro20194054Week2Doc::CImagePro20194054Week2Doc() noexcept
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.
	inputImg = NULL;
	inputImg2 = NULL;
	resultImg = NULL;

	gResultImg = NULL;
}

CImagePro20194054Week2Doc::~CImagePro20194054Week2Doc()
{
}

BOOL CImagePro20194054Week2Doc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}




// CImagePro20194054Week2Doc serialization

void CImagePro20194054Week2Doc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
	}
	else
	{
		LoadImageFile(ar);
	}
}

#ifdef SHARED_HANDLERS

// 축소판 그림을 지원합니다.
void CImagePro20194054Week2Doc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 문서의 데이터를 그리려면 이 코드를 수정하십시오.
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 검색 처리기를 지원합니다.
void CImagePro20194054Week2Doc::InitializeSearchContent()
{
	CString strSearchContent;
	// 문서의 데이터에서 검색 콘텐츠를 설정합니다.
	// 콘텐츠 부분은 ";"로 구분되어야 합니다.

	// 예: strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CImagePro20194054Week2Doc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CImagePro20194054Week2Doc 진단

#ifdef _DEBUG
void CImagePro20194054Week2Doc::AssertValid() const
{
	CDocument::AssertValid();
}

void CImagePro20194054Week2Doc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CImagePro20194054Week2Doc 명령


void CImagePro20194054Week2Doc::PixelAdd()
{
	int value = 0;
	
	for(int y = 0; y < imageHeight; y++)
		for (int x = 0; x < imageWidth * depth; x++) {
			value = inputImg[y][x] + 100;	//밝게
			if (value > 255)	resultImg[y][x] = 255;
			else resultImg[y][x] = value;
		}
}


void CImagePro20194054Week2Doc::PixelHistoEq()
{
	int x, y, i, k;
	int acc_hist = 0;
	float N = (float)imageHeight * imageWidth;
	int hist[256], sum[256];

	for (k = 0; k < 256; k++)	hist[k] = 0;
	
	//명암값의 빈도수 조사
	for(y = 0; y < imageHeight; y++)
		for (x = 0; x < imageWidth * depth; x++) {
			k = inputImg[y][x];
			hist[k] = hist[k] + 1;
		}
	//누적된 히스토그램 합 계산
	for (i = 0; i < 256; i++) {
		acc_hist = acc_hist + hist[i];
		sum[i] = acc_hist;
	}
	for(y = 0; y < imageHeight; y++)
		for (x = 0; x < imageWidth; x++) {
			k = inputImg[y][x];
			resultImg[y][x] = (unsigned char)(sum[k] / N * 255);
		}
}


void CImagePro20194054Week2Doc::PixelTwoImageAdd()
{
	int value = 0;
	LoadTwoImages();

	for(int y = 0; y < imageHeight; y++)
		for (int x = 0; x < imageWidth * depth; x++) {
			value = inputImg[y][x] + inputImg2[y][x];
			if (value > 255)	resultImg[y][x] = 255;
			else resultImg[y][x] = value;
		}
}


void CImagePro20194054Week2Doc::LoadTwoImages(void)
{
	CFile file;
	CFileDialog dlg(TRUE);
	AfxMessageBox("Select the First Image");
	if (dlg.DoModal() == IDOK) {
		file.Open(dlg.GetPathName(), CFile::modeRead);
		CArchive ar(&file, CArchive::load);
		LoadImageFile(ar);
		file.Close();
	}

	AfxMessageBox("Select the Second Image");

	if (dlg.DoModal() == IDOK) {
		file.Open(dlg.GetPathName(), CFile::modeRead);
		CArchive ar(&file, CArchive::load);
		LoadSecondImageFile(ar);
		file.Close();
	}
}


void CImagePro20194054Week2Doc::LoadImageFile(CArchive &ar)
{
	int i, maxValue;
	CString type, buf;
	CFile *fp = ar.GetFile();
	CString fname = fp->GetFilePath();
	//파일의 헤더 읽기
	if(strcmp(strrchr(fname, '.'), ".ppm")==0|| strcmp(strrchr(fname, '.'), ".PPM") == 0 ||
		strcmp(strrchr(fname, '.'), ".PGM") == 0 || strcmp(strrchr(fname, '.'), ".pgm") == 0)
	{
		ar.ReadString(type);

		do {
			ar.ReadString(buf);
		} while (buf[0] == '#');
		sscanf_s(buf, "%d %d", &imageWidth, &imageHeight);

		do {
			ar.ReadString(buf);
		} while (buf[0] == '#');
		sscanf_s(buf, "%d", &maxValue);

		if (strcmp(type, "P5") == 0) depth = 1;
		else depth = 3;
	}
	else if (strcmp(strrchr(fname, '.'), ".raw") == 0 ||
		strcmp(strrchr(fname, '.'), ".RAW") == 0)
	{
		if (fp->GetLength() != 256 * 256) {
			AfxMessageBox("256*256 크기의 파일만 사용가능합니다.");
			return;
		}

		imageWidth = 256;
		imageHeight = 256;
		depth = 1;
	}

	//기억장소 할당
	inputImg = (unsigned char**)malloc(imageHeight * sizeof(unsigned char*));
	resultImg = (unsigned char**)malloc(imageHeight * sizeof(unsigned char*));

	for (i = 0; i < imageHeight; i++) {
		inputImg[i] = (unsigned char*)malloc(imageWidth*depth);
		resultImg[i] = (unsigned char*)malloc(imageWidth*depth);
	}

	//영상 데이터 읽기
	for (i = 0; i < imageHeight; i++)
		ar.Read(inputImg[i], imageWidth*depth);
}


void CImagePro20194054Week2Doc::LoadSecondImageFile(CArchive& ar)
{
	int i, maxValue;
	CString type, buf;
	CFile* fp = ar.GetFile();
	CString fname = fp->GetFilePath();
	//파일의 헤더 읽기
	if (strcmp(strrchr(fname, '.'), ".ppm") == 0 || strcmp(strrchr(fname, '.'), ".PPM") == 0 ||
		strcmp(strrchr(fname, '.'), ".PGM") == 0 || strcmp(strrchr(fname, '.'), ".pgm") == 0)
	{
		ar.ReadString(type);

		do {
			ar.ReadString(buf);
		} while (buf[0] == '#');
		sscanf_s(buf, "%d %d", &imageWidth, &imageHeight);

		do {
			ar.ReadString(buf);
		} while (buf[0] == '#');
		sscanf_s(buf, "%d", &maxValue);

		if (strcmp(type, "P5") == 0) depth = 1;
		else depth = 3;
	}
	else if (strcmp(strrchr(fname, '.'), ".raw") == 0 ||
		strcmp(strrchr(fname, '.'), ".RAW") == 0)
	{
		if (fp->GetLength() != 256 * 256) {
			AfxMessageBox("256*256 크기의 파일만 사용가능합니다.");
			return;
		}

		imageWidth = 256;
		imageHeight = 256;
		depth = 1;
	}
		//기억장소 할당
		inputImg2 = (unsigned char**)malloc(imageHeight * sizeof(unsigned char*));

		for (i = 0; i < imageHeight; i++) {
			inputImg2[i] = (unsigned char*)malloc(imageWidth * depth);
		}

		//영상 데이터 읽기
		for (i = 0; i < imageHeight; i++)
			ar.Read(inputImg2[i], imageWidth * depth);
	
}

void CImagePro20194054Week2Doc::RegionSharpening(void)
{
	float kernel[3][3] = { {0, -1, 0}, {-1, 5, -1}, {0, -1, 0} };

	Convolve(inputImg, resultImg, imageWidth, imageHeight, kernel, 0, depth);
}


void CImagePro20194054Week2Doc::Convolve(unsigned char** inputImg, unsigned char** resultImg,
	int cols, int rows, float mask[][3], int bias, int depth)
{
	int i, j, x, y;
	int red, green, blue;
	int sum;
	unsigned char** tmpImg;

	//기억장소할당
	tmpImg = (unsigned char**)malloc((imageHeight + 2) * sizeof(unsigned char*));
	for (i = 0; i < imageHeight + 2; i++)
		tmpImg[i] = (unsigned char*)malloc((imageWidth + 2) * depth);

	// 0-삽입을위해0으로초기화
	for (y = 0; y < imageHeight + 2; y++)
		for (x = 0; x < (imageWidth + 2) * depth; x++)
			tmpImg[y][x] = 0;

	// 영상복사
	for (y = 1; y < imageHeight + 1; y++)
		for (x = 1; x < imageWidth + 1; x++)
			if (depth == 1) tmpImg[y][x] = inputImg[y - 1][x - 1];
			else if (depth == 3) {
				tmpImg[y][3 * x] = inputImg[y - 1][3 * (x - 1)];
				tmpImg[y][3 * x + 1] = inputImg[y - 1][3 * (x - 1) + 1];
				tmpImg[y][3 * x + 2] = inputImg[y - 1][3 * (x - 1) + 2];
			}
	for (y = 0; y < imageHeight; y++)
		for (x = 0; x < imageWidth; x++) {
			if (depth == 1) {
				sum = 0;
				for (i = 0; i < 3; i++)
					for (j = 0; j < 3; j++)
						sum += (int)(tmpImg[y + i][x + j] * mask[i][j]);
				sum = sum + bias;

				if (sum > 255) sum = 255;
				if (sum < 0) sum = 0;
				resultImg[y][x] = (unsigned char)sum;
			}
			else if (depth == 3) {
				red = 0;
				green = 0;
				blue = 0;
				for (i = 0; i < 3; i++)
					for (j = 0; j < 3; j++) {
						red += (int)(tmpImg[y + i][3 * (x + j)] * mask[i][j]);
						green += (int)(tmpImg[y + i][3 * (x + j) + 1] * mask[i][j]);
						blue += (int)(tmpImg[y + i][3 * (x + j) + 2] * mask[i][j]);
					}
				red = red + bias;
				green = green + bias;
				blue = blue + bias;

				if (red > 255) red = 255;
				if (red < 0) red = 0;
				if (green > 255) green = 255;
				if (green < 0) green = 0;
				if (blue > 255) blue = 255;
				if (blue < 0) blue = 0;
				resultImg[y][3 * x] = (unsigned char)red;
				resultImg[y][3 * x + 1] = (unsigned char)green;
				resultImg[y][3 * x + 2] = (unsigned char)blue;
			}
		}
	//기억장소반환
	for (i = 0; i < imageHeight + 2; i++)
		free(tmpImg[i]);
	free(tmpImg);
}

void CImagePro20194054Week2Doc::RegionMeaning()
{
	float mask[3][3] = { {(float)1 / 9, (float)1 / 9, (float)1 / 9},
		{(float)1 / 9, (float)1 / 9, (float)1 / 9},
		{(float)1 / 9, (float)1 / 9, (float)1 / 9} };

	Convolve(inputImg, resultImg, imageWidth, imageHeight, mask, 0, depth);
}


void CImagePro20194054Week2Doc::RegionSobel()
{
	int i, x, y, sum;
	float mask1[3][3] = { 1,0,-1,2,0,-2,1,0,-1 };
	float mask2[3][3] = { -1,-2,-1,0,0,0,1,2,1 };
	unsigned char** Er, ** Ec;

	Er = (unsigned char **)malloc((imageHeight) * sizeof(unsigned char*));
	Ec = (unsigned char **)malloc((imageHeight) * sizeof(unsigned char*));

	for (i = 0; i < imageHeight; i++) {
		Er[i] = (unsigned char *)malloc(imageWidth * depth);
		Ec[i] = (unsigned char *)malloc(imageWidth * depth);
	}

	Convolve(inputImg, Er, imageWidth, imageHeight, mask1, 0, depth);
	Convolve(inputImg, Ec, imageWidth, imageHeight, mask2, 0, depth);

	for(y = 0; y < imageHeight; y++)
		for (x = 0; x < imageWidth * depth; x++) {
			sum = (int) sqrt( (float) (Er[y][x] * Er[y][x] + Ec[y][x] * Ec[y][x]));
			if (sum > 255) sum = 255;
			if (sum < 0) sum = 0;
			resultImg[y][x] = (unsigned char)sum;
		}
}


void CImagePro20194054Week2Doc::RegionMedian()
{
	int i, j, temp, x, y;
	int n[9];

	for (y = 1; y < imageHeight - 1; y++)
		for (x = 1; x < imageWidth - 1; x++) {
			n[0] = inputImg[y - 1][x - 1];
			n[1] = inputImg[y - 1][x];
			n[2] = inputImg[y - 1][x + 1];
			n[3] = inputImg[y][x - 1];
			n[4] = inputImg[y][x];
			n[5] = inputImg[y][x + 1];
			n[6] = inputImg[y + 1][x - 1];
			n[7] = inputImg[y + 1][x];
			n[8] = inputImg[y + 1][x + 1];


			for (i = 8; i > 0; i--)
				for (j = 0; j < i; j++) {
					if (n[j] > n[j + 1]) {
						temp = n[j + 1];
						n[j + 1] = n[j];
						n[j] = temp;
					}
					resultImg[y][x] = n[4];
				}
		}
}


void CImagePro20194054Week2Doc::Erosion()
{
	/*int x, y, min, i, j;
	for (y = 1; y < imageHeight - 1; y++)
		for (x = 1; x < imageWidth - 1; x++) {
			min = 255;
			if (inputImg[y - 1][x - 1] < min) min = inputImg[y - 1][x - 1];
			if (inputImg[y - 1][x] < min) min = inputImg[y - 1][x];
			if (inputImg[y - 1][x + 1] < min) min = inputImg[y - 1][x + 1];
			if (inputImg[y][x - 1] < min) min = inputImg[y][x - 1];
			if (inputImg[y][x] < min) min = inputImg[y][x];
			if (inputImg[y][x + 1] < min) min = inputImg[y][x + 1];
			if (inputImg[y + 1][x - 1] < min) min = inputImg[y + 1][x - 1];
			if (inputImg[y + 1][x] < min) min = inputImg[y + 1][x];
			if (inputImg[y + 1][x + 1] < min) min = inputImg[y + 1][x + 1];
			resultImg[y][x] = min;
		}*/
		//팽창 침식 변경
	int x, y, max, i, j;

	for (y = 1; y < imageHeight - 1; y++)
		for (x = 1; x < imageWidth - 1; x++) {
			max = 0;
			if (inputImg[y - 1][x - 1] > max) max = inputImg[y - 1][x - 1];
			if (inputImg[y - 1][x] > max) max = inputImg[y - 1][x];
			if (inputImg[y - 1][x + 1] > max) max = inputImg[y - 1][x + 1];
			if (inputImg[y][x - 1] > max) max = inputImg[y][x - 1];
			if (inputImg[y][x] > max) max = inputImg[y][x];
			if (inputImg[y][x + 1] > max) max = inputImg[y][x + 1];
			if (inputImg[y + 1][x - 1] > max) max = inputImg[y + 1][x - 1];
			if (inputImg[y + 1][x] > max) max = inputImg[y + 1][x];
			if (inputImg[y + 1][x + 1] > max) max = inputImg[y + 1][x + 1];
			resultImg[y][x] = max; // 최대값을 결과 영상에 저장
		}
}


void CImagePro20194054Week2Doc::Dilation()
{
	//int x, y, max, i, j;

	//for (y = 1; y < imageHeight - 1; y++)
	//	for (x = 1; x < imageWidth - 1; x++) {
	//		max = 0;
	//		if (inputImg[y - 1][x - 1] > max) max = inputImg[y - 1][x - 1];
	//		if (inputImg[y - 1][x] > max) max = inputImg[y - 1][x];
	//		if (inputImg[y - 1][x + 1] > max) max = inputImg[y - 1][x + 1];
	//		if (inputImg[y][x - 1] > max) max = inputImg[y][x - 1];
	//		if (inputImg[y][x] > max) max = inputImg[y][x];
	//		if (inputImg[y][x + 1] > max) max = inputImg[y][x + 1];
	//		if (inputImg[y + 1][x - 1] > max) max = inputImg[y + 1][x - 1];
	//		if (inputImg[y + 1][x] > max) max = inputImg[y + 1][x];
	//		if (inputImg[y + 1][x + 1] > max) max = inputImg[y + 1][x + 1];
	//		resultImg[y][x] = max; // 최대값을 결과 영상에 저장
	//	}

	//팽창 침식 변경
	int x, y, min, i, j;
	for (y = 1; y < imageHeight - 1; y++)
		for (x = 1; x < imageWidth - 1; x++) {
			min = 255;
			if (inputImg[y - 1][x - 1] < min) min = inputImg[y - 1][x - 1];
			if (inputImg[y - 1][x] < min) min = inputImg[y - 1][x];
			if (inputImg[y - 1][x + 1] < min) min = inputImg[y - 1][x + 1];
			if (inputImg[y][x - 1] < min) min = inputImg[y][x - 1];
			if (inputImg[y][x] < min) min = inputImg[y][x];
			if (inputImg[y][x + 1] < min) min = inputImg[y][x + 1];
			if (inputImg[y + 1][x - 1] < min) min = inputImg[y + 1][x - 1];
			if (inputImg[y + 1][x] < min) min = inputImg[y + 1][x];
			if (inputImg[y + 1][x + 1] < min) min = inputImg[y + 1][x + 1];
			resultImg[y][x] = min;
		}
}


void CImagePro20194054Week2Doc::Opening()
{
	Erosion();
	CopyResultToInput();
	Erosion();
	CopyResultToInput();
	Dilation();
	CopyResultToInput();
	Dilation();
}


void CImagePro20194054Week2Doc::CopyResultToInput()
{
	int x, y;
	for (y = 0; y < imageHeight; y++)
		for (x = 0; x < imageWidth; x++)
			inputImg[y][x] = resultImg[y][x];
}


void CImagePro20194054Week2Doc::Closing()
{
	Dilation();
	CopyResultToInput();
	Dilation();
	CopyResultToInput();
	Erosion();
	CopyResultToInput();
	Erosion();
}

void CImagePro20194054Week2Doc::GeometryZoominPixelCopy()
{
	int i, y, x;
	gImageWidth = imageWidth * 3;
	gImageHeight = imageHeight * 3;
	gResultImg = (unsigned char**)malloc(gImageHeight *	sizeof(unsigned char*));

	for (i = 0; i < gImageHeight; i++) {
		gResultImg[i] = (unsigned char*)malloc(gImageWidth * depth);
	}
	for (y = 0; y < gImageHeight; y++)
		for (x = 0; x < gImageWidth; x++)
			gResultImg[y][x] = inputImg[y / 3][x / 3];
}


void CImagePro20194054Week2Doc::GeometryZoominInterpolation()
{
	int i, y, x;
	float src_x, src_y; // 원시 화소 위치
	float alpha, beta;
	int scale_x, scale_y; // float scale_x, scale_y;
	int E, F; // x 방향으로 보간된 결과 값
	int Ax, Ay, Bx, By, Cx, Cy, Dx, Dy; // 보간에 사용될 4개 픽셀의 좌표
	scale_x = 3;
	scale_y = 3;

	// 결과 영상의 크기 설정
	gImageWidth = imageWidth * scale_x;
	gImageHeight = imageHeight * scale_y;
	// 결과 영상을 저장할 기억장소 공간 할당
	gResultImg = (unsigned char**)malloc(gImageHeight *	sizeof(unsigned char*));
	for (i = 0; i < gImageHeight; i++) {
		gResultImg[i] = (unsigned char*)malloc(gImageWidth * depth);
	}

	for (y = 0; y < gImageHeight; y++)
		for (x = 0; x < gImageWidth; x++) {
			// 원시 영상에서의 픽셀 좌표 계산
			src_x = x / scale_x; // src_x = x / scale_x;
			src_y = y / (float)scale_y; // src_y = y / scale_y;
			alpha = src_x - x / scale_x; // alpha = src_x – (int) src_x;
			beta = src_y - y / scale_y; // beta = src_y – (int) src_y;
			// 보간에 사용된 4 픽셀의 좌표 계산
			Ax = x / scale_x;
			Ay = y / scale_y;
			Bx = Ax + 1;
			By = Ay;
			Cx = Ax;
			Cy = Ay + 1;
			Dx = Ax + 1;
			Dy = Ay + 1;

			// 픽셀 위치가 영상의 경계를 벗어나는지 검사
			if (Bx > imageWidth - 1) Bx = imageWidth - 1;
			if (Dx > imageWidth - 1) Dx = imageWidth - 1;
			if (Cy > imageHeight - 1) Cy = imageHeight - 1;
			if (Dy > imageHeight - 1) Dy = imageHeight - 1;
			// x 방향으로 보간
			E = (int)(inputImg[Ay][Ax] * (1 - alpha) + inputImg[By][Bx] * alpha);
			F = (int)(inputImg[Cy][Cx] * (1 - alpha) + inputImg[Dy][Dx] * alpha);
			// y 방향으로 보간
			gResultImg[y][x] = (unsigned char)(E * (1 - beta) + F * beta);
		}
}


void CImagePro20194054Week2Doc::GeometryZoomoutSubsampling()
{
	int i, y, x;
	int src_x, src_y;
	int scale_x = 3, scale_y = 3;

	// 결과 영상의 크기 계산
	gImageWidth = (imageWidth % scale_x == 0) ? imageWidth / scale_x :
		imageWidth / scale_x + 1;
	gImageHeight = (imageHeight % scale_y == 0) ? imageHeight / scale_y :
		imageHeight / scale_y + 1;

	// 결과 영상 저장을 위한 기억장소 할당
	gResultImg = (unsigned char**)malloc(gImageHeight * sizeof(unsigned char*));
	for (i = 0; i < gImageHeight; i++) {
		gResultImg[i] = (unsigned char*)malloc(gImageWidth * depth);
	}

	for (y = 0; y < gImageHeight; y++)
		for (x = 0; x < gImageWidth; x++) {
			src_y = y * scale_y;
			src_x = x * scale_x;
			gResultImg[y][x] = inputImg[src_y][src_x];
		}
}


void CImagePro20194054Week2Doc::GeometryZoomoutAvg()
{
	int i, j, x, y;
	int sum;
	int src_x, src_y;
	int scale_x = 2, scale_y = 2;

	// 결과 영상의 크기 계산
	gImageWidth = (imageWidth % scale_x == 0) ? imageWidth / scale_x :
		imageWidth / scale_x + 1;
	gImageHeight = (imageHeight % scale_y == 0) ? imageHeight / scale_y :
		imageHeight / scale_y + 1;

	// 결과 영상을 저장할 기억장소 할당
	gResultImg = (unsigned char**)malloc(gImageHeight * sizeof(unsigned char*));
	for (i = 0; i < gImageHeight; i++) {
		gResultImg[i] = (unsigned char*)malloc(gImageWidth * depth);
	}

	for (y = 0; y < imageHeight; y = y + scale_y)
		for (x = 0; x < imageWidth; x = x + scale_x) {
			// 필터 윈도우 내의 픽셀값 합계 계산
			sum = 0;
			for (i = 0; i < scale_y; i++)
				for (j = 0; j < scale_x; j++) {
					src_x = x + j;
					src_y = y + i;
					if (src_x > imageWidth - 1) src_x = imageWidth - 1; // 영상의 경계 검사
					if (src_y > imageHeight - 1) src_y = imageHeight - 1;
					sum += inputImg[src_y][src_x];
				}
			sum = sum / (scale_x * scale_y); // 평균값 계산
			
			gResultImg[y / scale_y][x / scale_x] = (unsigned char)sum; // 결과 값 저장
		}
}

#define PI 3.14159
void CImagePro20194054Week2Doc::GeometryRotate()
{
	int y, x, x_source, y_source, Cx, Cy;
	float angle;
	int Oy;
	int i, xdiff, ydiff;

	Oy = imageHeight - 1;

	angle = PI / 180.0 * 30.0;	//30도를 라디안 값의 각도로 변환

	Cx = imageWidth / 2;	//회전 중심의 x좌표
	Cy = imageHeight / 2;	//회전 중심의 y좌표

	//결과 영상 크기 계산
	gImageWidth = (int)(imageHeight * cos(PI / 2.0 - angle) + imageWidth * cos(angle));
	gImageHeight = (int)(imageHeight * cos(angle) + imageWidth * cos(PI / 2.0 - angle));

	//결과 영상을 저장할 기억장소 할당
	gResultImg = (unsigned char**)malloc(gImageHeight * sizeof(unsigned char*));
	for (i = 0; i < gImageHeight; i++) {
		gResultImg[i] = (unsigned char*)malloc(gImageWidth * depth);
	}

	// 결과 영상의 x 좌표 범위 : -xdiff ~ gImageWidth - xdiff - 1
	// 결과 영상의 y 좌표 범위 : -ydiff ~ gImageHeight - ydiff - 1
	xdiff = (gImageWidth - imageWidth) / 2;
	ydiff = (gImageHeight - imageHeight) / 2;

	for (y = -ydiff; y < gImageHeight - ydiff; y++) {
		for (x = -xdiff; x < gImageWidth - xdiff; x++) {
			// 변환 단계
			// 1 단계 : 원점이 영상의 좌측 하단에 오도록 y 좌표 변환
			// 2 단계 : 회전 중심이 원점에 오도록 이동
			// 3 단계 : 각도 angle 만큼 회전
			// 4 단계 : 회전 중심이 원래 위치로 돌아가도록 이동
			x_source = (int)(((Oy - y) - Cy) * sin(angle) + (x - Cx) * cos(angle) + Cx);
			y_source = (int)(((Oy - y) - Cy) * cos(angle) - (x - Cx) * sin(angle) + Cy);

			// 5 단계 : 원점이 영상의 좌측 상단에 오도록 y 좌표 변환
			y_source = Oy - y_source;
			if (x_source < 0 || x_source > imageWidth - 1 ||
				y_source < 0 || y_source > imageHeight - 1)
				gResultImg[y + ydiff][x + xdiff] = 255;
			else
				gResultImg[y + ydiff][x + xdiff] = inputImg[y_source][x_source];
		}
	}
}

void CImagePro20194054Week2Doc::GeometryMirror()
{
	int x, y;

	for (y = 0; y < imageHeight; y++)
		for (x = 0; x < imageWidth; x++)
			resultImg[y][x] = inputImg[y][imageWidth - 1 - x];
}


void CImagePro20194054Week2Doc::GeometryFlip()
{
	int x, y;

	for (y = 0; y < imageHeight; y++)
		for (x = 0; x < imageWidth; x++)
			resultImg[imageHeight - 1 - y][x] = inputImg[y][x];
}


void CImagePro20194054Week2Doc::GeometryDiagonal()
{
	int x, y;

	for (y = 0; y < imageHeight; y++)
		for (x = 0; x < imageWidth; x++)
			resultImg[imageHeight - 1 - y][x] = inputImg[y][imageWidth - 1 - x];
}

void CImagePro20194054Week2Doc::GeometryWarping()
{
	control_line source_lines[10] = { {116,7,207,5},{34,109,90,21},{55,249,30,128},{118,320,65,261},
		{123,321,171,321},{179,319,240,264},{247,251,282,135},{281,114,228,8},
		{78,106,123,109},{187,115,235,114}};

	control_line dest_lines[10] = { {120,8,200,6},{12,93,96,16},{74,271,16,110},{126,336,96,290},
		{142,337,181,335},{192,335,232,280},{244,259,288,108},{285,92,212,13},
		{96,135,136,118},{194,119,223,125}};

	double u;	// 수직 교차점의 위치
	double h;	// 제어선으로부터 픽셀의 수직변위
	double d;	// 제어선과 픽셀사이의 거리
	double tx, ty;	// 결과영상 픽셀에 대응되는 입력영상 픽셀사이의 변위의 합
	double xp, yp;	// 각 제어선에대해 계산된 입력영상의 대응되는 픽셀위치
	double weight;	// 각 제어선의 가중치
	double totalWeight;	// 가중치의 합
	double a=0.001;
	double b=2.0;
	double p=0.75;

	int x1, x2, y1, y2;
	int src_x1, src_y1, src_x2, src_y2;
	double src_line_length, dest_line_length;
	int num_lines = 10;	// 제어선의 수
	int line;
	int x, y;
	int source_x, source_y;
	int last_row, last_col;
	last_row= imageHeight-1;
	last_col= imageWidth-1;  

	//출력영상 각 필셀에 대하여
	for (y = 0; y < imageHeight; y++) {
		for (x = 0; x < imageWidth; x++) {
			totalWeight = 0.0;
			tx = 0.0; ty = 0.0;

			//각 제어선에 대해
			for (line = 0; line < num_lines; line++) {
				x1 = dest_lines[line].Px;
				y1 = dest_lines[line].Py;
				x2 = dest_lines[line].Qx;
				y2 = dest_lines[line].Qy;
				dest_line_length = sqrt((double)(x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));

				// 수직교차점의 위치 및 픽셀의 수직변위계산
				u = (double) ((x-x1)*(x2-x1)+(y-y1)*(y2-y1)) /
					(double) ((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
				h = (double)((y - y1) * (x2 - x1) - (x - x1) * (y2 - y1)) / dest_line_length;

				// 제어선과 픽셀사이의 거리계산
				if (u < 0 ) d = sqrt((double) (x-x1) * (x-x1) + (y-y1) * (y-y1));
				else if (u > 1) d = sqrt((double) (x-x2) * (x-x2) + (y-y2) * (y-y2));
				else d =  fabs(h);
				
				src_x1 = source_lines[line].Px;
				src_y1 = source_lines[line].Py;
				src_x2 = source_lines[line].Qx;
				src_y2 = source_lines[line].Qy;
				src_line_length=sqrt((double) (src_x2-src_x1)*(src_x2-src_x1)+
					(src_y2-src_y1)*(src_y2-src_y1));

				// 입력영상에서의 대응픽셀위치계산
				xp= src_x1+u*(src_x2 -src_x1) -h * (src_y2 -src_y1) / src_line_length;
				yp= src_y1+u*(src_y2 -src_y1) + h * (src_x2 -src_x1) / src_line_length;
				
				// 제어선에대한 가중치계산
				weight = pow((pow((double)(dest_line_length),p) / (a + d)) , b);

				// 대응픽셀과의 변위계산
				tx+= (xp-x) * weight;
				ty += (yp-y) * weight;
				totalWeight+= weight;
			}
			source_x = x + (int)(tx / totalWeight + 0.5);
			source_y = y + (int)(ty / totalWeight + 0.5);
			
			// 영상의경계를 벗어나는지 검사
			if (source_x< 0) source_x= 0;
			if (source_x> last_col) source_x= last_col;
			if (source_y< 0) source_y= 0;
			if (source_y> last_row) source_y= last_row;
			resultImg[y][x] = inputImg[source_y][source_x]; 
		}
	}
}

#define NUM_FRAMES 10
void CImagePro20194054Week2Doc::GeometryMorphing()
{
	control_line source_lines[23] = 
	{ {116,7,207,5},{34,109,90,21},{55,249,30,128},{118,320,65,261},
		{123,321,171,321},{179,319,240,264},{247,251,282,135},{281,114,228,8},
		{78,106,123,109},{187,115,235,114},{72,142,99,128},{74,150,122,154},
		{108,127,123,146},{182,152,213,132},{183,159,229,157},{219,131,240,154},
		{80,246,117,212},{127,222,146,223},{154,227,174,221},{228,252,183,213},
		{114,255,186,257},{109,258,143,277},{152,278,190,262} };
	control_line dest_lines[23] = { {120,8,200,6},{12,93,96,16},{74,271,16,110},{126,336,96,290},
		{142,337,181,335},{192,335,232,280},{244,259,288,108},{285,92,212,13},
		{96,135,136,118},{194,119,223,125},{105,145,124,134},{110,146,138,151},
		{131,133,139,146},{188,146,198,134},{189,153,218,146},{204,133,221,140},
		{91,268,122,202},{149,206,159,209},{170,209,181,204},{235,265,208,199},
		{121,280,205,284},{112,286,160,301},{166,301,214,287}};

	double u;       // 수직교차점의위치
	double h;       // 제어선으로부터픽셀의수직변위
	double d;       // 제어선과픽셀사이의거리
	double tx, ty;   // 결과영상픽셀에대응되는입력영상픽셀사이의변위의합
	double xp, yp;  // 각제어선에대해계산된입력영상의대응되는픽셀위치
	double weight;     // 각제어선의가중치
	double totalWeight; // 가중치의합
	double a=0.001, b=2.0, p=0.75;
	unsigned char **warpedImg;
	unsigned char **warpedImg2;
	int frame;
	double fweight;
	control_line warp_lines[23];
	double tx2, ty2, xp2, yp2;
	int dest_x1, dest_y1, dest_x2, dest_y2, source_x2, source_y2;
	int x1, x2, y1, y2, src_x1, src_y1, src_x2, src_y2;
	double src_line_length, dest_line_length;
	int i, j;
	int num_lines= 23;         // 제어선의수
	int line, x, y, source_x, source_y, last_row, last_col;

	// 두입력영상을읽어들임
	LoadTwoImages();
	
	// 중간프레임의워핑결과를저장을위한기억장소할당
	warpedImg= (unsigned char **) malloc(imageHeight* sizeof(unsigned char *));
	for (i= 0; i< imageHeight; i++) { 
		warpedImg[i] = (unsigned char *) malloc(imageWidth* depth);
	} 
	warpedImg2 = (unsigned char **) malloc(imageHeight* sizeof(unsigned char *));
	for (i= 0; i< imageHeight; i++) {
		warpedImg2[i] = (unsigned char *) malloc(imageWidth* depth);
	} 
	for (i = 0; i < NUM_FRAMES; i++) {
		morphedImg[i] = (unsigned char**)malloc(imageHeight * sizeof(unsigned char*));
		for (j = 0; j < imageHeight; j++) {
			morphedImg[i][j] = (unsigned char*)malloc(imageWidth * depth);
		}
	}
	last_row = imageHeight - 1;
	last_col = imageWidth - 1;

	// 각중간프레임에대하여
	for (frame = 1; frame <= NUM_FRAMES; frame++) {
		// 중간프레임에대한가중치계산
		fweight = (double)(frame) / NUM_FRAMES;

		// 중간프레임에대한제어선계산
		for (line = 0; line < num_lines; line++) {
			warp_lines[line].Px = (int)(source_lines[line].Px + (dest_lines[line].Px - source_lines[line].Px) * fweight);
			warp_lines[line].Py = (int)(source_lines[line].Py + (dest_lines[line].Py - source_lines[line].Py) * fweight);
			warp_lines[line].Qx = (int)(source_lines[line].Qx + (dest_lines[line].Qx - source_lines[line].Qx) * fweight);
			warp_lines[line].Qy = (int)(source_lines[line].Qy + (dest_lines[line].Qy - source_lines[line].Qy) * fweight);
		}

		// 출력영상의각픽셀에대하여
		for (y = 0; y < imageHeight; y++) {
			for (x = 0; x < imageWidth; x++) {
				totalWeight = 0.0;
				tx = 0.0;
				ty = 0.0;
				tx2 = 0.0;
				ty2 = 0.0;
				// 각제어선에대하여
				for (line = 0; line < num_lines; line++) {
					x1 = warp_lines[line].Px;
					y1 = warp_lines[line].Py;
					x2 = warp_lines[line].Qx;
					y2 = warp_lines[line].Qy;
					dest_line_length = sqrt((double)(x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));

					// 수직교차점의위치및픽셀의수직변위계산
					u = (double)((x - x1) * (x2 - x1) + (y - y1) * (y2 - y1)) / (double)((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
					h = (double)((y - y1) * (x2 - x1) - (x - x1) * (y2 - y1)) / dest_line_length;

					// 제어선과픽셀사이의거리계산
					if (u < 0) d = sqrt((double)(x - x1) * (x - x1) + (y - y1) * (y - y1));
					else if (u > 1) d = sqrt((double)(x - x2) * (x - x2) + (y - y2) * (y - y2));
					else d = fabs(h); src_x1 = source_lines[line].Px;
					src_y1 = source_lines[line].Py;
					src_x2 = source_lines[line].Qx;
					src_y2 = source_lines[line].Qy;
					src_line_length = sqrt((double)(src_x2 - src_x1) * (src_x2 - src_x1) + (src_y2 - src_y1) * (src_y2 - src_y1));

					dest_x1 = dest_lines[line].Px;
					dest_y1 = dest_lines[line].Py;
					dest_x2 = dest_lines[line].Qx;
					dest_y2 = dest_lines[line].Qy;
					dest_line_length = sqrt((double)(dest_x2 - dest_x1) * (dest_x2 - dest_x1) + (dest_y2 - dest_y1) * (dest_y2 - dest_y1));
					// 입력영상1에서의대응픽셀위치계산
					xp = src_x1 + u * (src_x2 - src_x1) - h * (src_y2 - src_y1) / src_line_length;
					yp = src_y1 + u * (src_y2 - src_y1) + h * (src_x2 - src_x1) / src_line_length;

					// 입력영상2에서의대응픽셀위치계산
					xp2 = dest_x1 + u * (dest_x2 - dest_x1) - h * (dest_y2 - dest_y1) / dest_line_length;
					yp2 = dest_y1 + u * (dest_y2 - dest_y1) + h * (dest_x2 - dest_x1) / dest_line_length;

					// 제어선에대한가중치계산
					weight = pow((pow((double)(dest_line_length), p) / (a + d)), b);

					// 입력영상1의대응픽셀과의변위계산
					tx += (xp - x) * weight; ty += (yp - y) * weight;

					// 입력영상2의대응픽셀과의변위계산
					tx2 += (xp2 - x) * weight;
					ty2 += (yp2 - y) * weight;

					totalWeight += weight;
				}
				// 입력영상1의대응픽셀위치계산
				source_x = x + (int)(tx / totalWeight + 0.5);
				source_y = y + (int)(ty / totalWeight + 0.5);
				// 입력영상2의대응픽셀위치계산
				source_x2 = x + (int)(tx2 / totalWeight + 0.5);
				source_y2 = y + (int)(ty2 / totalWeight + 0.5);
				// 영상의경계를벗어나는지검사
				if (source_x < 0) source_x = 0;
				if (source_x > last_col) source_x = last_col;
				if (source_y < 0) source_y = 0;
				if (source_y > last_row) source_y = last_row;
				if (source_x2 < 0) source_x2 = 0;
				if (source_x2 > last_col) source_x2 = last_col;
				if (source_y2 < 0) source_y2 = 0;
				if (source_y2 > last_row) source_y2 = last_row;

				// 워핑결과저장
				warpedImg[y][x] = inputImg[source_y][source_x];
				warpedImg2[y][x] = inputImg2[source_y2][source_x2];
			}
		}
		// 모핑결과합병
		for(y=0; y<imageHeight; y++)
			for(x=0; x<imageWidth; x++) {
				int val= (int) ((1.0 -fweight) * warpedImg[y][x] + fweight* warpedImg2[y][x]);
				if (val< 0) val= 0;
				if (val> 255) val= 255;
				morphedImg[frame-1][y][x] = val;
			}
	}
}