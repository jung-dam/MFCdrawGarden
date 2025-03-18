
// MFCdrawGardenDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "MFCdrawGarden.h"
#include "MFCdrawGardenDlg.h"
#include "afxdialogex.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define POINT 3 //클릭할 점 개수

// GDI+ 초기화 객체
CGdiPlusInit gdiInit;

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCdrawGardenDlg 대화 상자



CMFCdrawGardenDlg::CMFCdrawGardenDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCDRAWGARDEN_DIALOG, pParent)
	,circleRadius(50), circleThickness(2), editThickness(1), isDragging(false), indexDrag(-1), pointRadius(10)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCdrawGardenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCdrawGardenDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_RESET, &CMFCdrawGardenDlg::OnBnClickedBtnReset)
	ON_BN_CLICKED(IDC_BTN_RANDOM, &CMFCdrawGardenDlg::OnBnClickedBtnRandom)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_EN_CHANGE(IDC_EDIT_THICKNESS, &CMFCdrawGardenDlg::OnEnChangeEditThickness)
	ON_EN_CHANGE(IDC_EDIT_POINT_RDS, &CMFCdrawGardenDlg::OnEnChangeEditPointRds)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CMFCdrawGardenDlg 메시지 처리기

BOOL CMFCdrawGardenDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 반지름 기본값 설정
	SetDlgItemInt(IDC_EDIT_POINT_RDS, pointRadius, FALSE);
	// 두께 기본값 설정
	SetDlgItemInt(IDC_EDIT_THICKNESS, circleThickness, FALSE);

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CMFCdrawGardenDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

//깜빡임 현상 줄이기 위해 추가
BOOL CMFCdrawGardenDlg::OnEraseBkgnd(CDC* pDC)
{
    return TRUE; // 배경을 지우지 않음
}

void CMFCdrawGardenDlg::OnPaint()
{
	CPaintDC dc(this);
	int nBpp = 32;  // 32비트 ARGB 색상 적용을 위해

	// 다이얼로그의 크기 가져오기
	CRect clientRect;
	GetClientRect(&clientRect);
	int nWidth = clientRect.Width();
	int nHeight = clientRect.Height();

	// 이미지 생성 (메모리에서 작업할 공간 할당)
	if (m_image.IsNull() || m_image.GetWidth() != nWidth || m_image.GetHeight() != nHeight) {
		m_image.Destroy(); // 기존 이미지 삭제
		m_image.Create(nWidth, -nHeight, nBpp);
	}

	// 메모리 DC 가져오기
	HDC hMemDC = m_image.GetDC();
	Gdiplus::Graphics graphics(hMemDC);

	// 배경을 회색으로 초기화
	Gdiplus::SolidBrush backgroundBrush(Gdiplus::Color(255, 240, 240, 240));
	graphics.FillRectangle(&backgroundBrush, 0, 0, nWidth, nHeight);

	// 클릭한 지점의 원 그리기
	for (const auto& point : clickPoints)
	{
		drawCircles((unsigned char*)m_image.GetBits(), point.position.x, point.position.y, point.radius, 0x00);
	}

	// 클릭한 지점의 좌표를 텍스트로 표시
	Gdiplus::SolidBrush textBrush(Gdiplus::Color(255, 137, 119, 173));
	Gdiplus::Font font(L"Arial", 10); 
	Gdiplus::StringFormat format;

	for (const auto& point : clickPoints)
	{
		CString str;
		str.Format(L"(%d, %d)", point.position.x, point.position.y);

		Gdiplus::PointF textPos(point.position.x + point.radius + 5, point.position.y - point.radius - 5);
		graphics.DrawString(str, -1, &font, textPos, &format, &textBrush);
	}

	// 원 (정원이 되는 경우) 그리기
	if (clickPoints.size() == 3)
	{
		Gdiplus::Pen circlePen(Gdiplus::Color(255, 0, 0, 0), circleThickness);
		graphics.DrawEllipse(&circlePen,
			circleCenter.x - circleRadius,
			circleCenter.y - circleRadius,
			circleRadius * 2,
			circleRadius * 2);
	}

	// 메모리 DC 해제
	m_image.ReleaseDC();

	// 메모리 이미지를 화면에 출력
	m_image.Draw(dc, 0, 0);
}


void CMFCdrawGardenDlg::drawCircles(unsigned char* fm, int x, int y, int nRadius, int nGray)
{

	int nCenterX = x;
	int nCenterY = y;
	int nPitch = m_image.GetPitch();
	int nBpp = 4;  // 32비트 (ARGB)에서 픽셀 당 4바이트

	for (int j = y - nRadius; j <= y + nRadius; j++) {
		for (int i = x - nRadius; i <= x + nRadius; i++) {
			if (isInCircle(i, j, nCenterX, nCenterY, nRadius))
			{
				// 좌표가 이미지 영역 내에 있는지 확인
				if (i >= 0 && i < m_image.GetWidth() && j >= 0 && j < m_image.GetHeight()) {
					int index = j * abs(nPitch) + (i * nBpp);
					fm[index] = 0;      // Blue 채널
					fm[index + 1] = 0;  // Green 채널
					fm[index + 2] = 0;  // Red 채널
					fm[index + 3] = 255; // Alpha (불투명)
				}
			}
		}
	}
}


bool CMFCdrawGardenDlg::isInCircle(int i, int j, int nCenterX, int nCenterY, int nRadius)
{
	bool bRet = false;

	double dX = i - nCenterX;
	double dY = j - nCenterY;
	double dDist = dX * dX + dY * dY;

	if (dDist < nRadius * nRadius) {
		bRet = true;
	}

	return bRet;
}


// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMFCdrawGardenDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMFCdrawGardenDlg::OnBnClickedBtnReset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	clickPoints.clear(); //클릭 지점 초기화
	circleRadius = 50;
	
	//두께, 반지름 초기화
	pointRadius = 10;
	circleThickness=2;
	editThickness=1;
	
	// 현재 마우스 활동 상태 
	isDragging=false;
	indexDrag = -1;

	SetDlgItemInt(IDC_EDIT_POINT_RDS, pointRadius, FALSE);
	SetDlgItemInt(IDC_EDIT_THICKNESS, circleThickness, FALSE);
	
	circleCenter = CPoint(0, 0);
	Invalidate();

}

void CMFCdrawGardenDlg::OnBnClickedBtnRandom()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	thread([this]() {
		random_device rd;
		mt19937 gen(rd());
		uniform_int_distribution<int> distX(50, 400);
		uniform_int_distribution<int> distY(50, 400);

		for (int i = 0; i < 10; i++)
		{
			for (auto& point : clickPoints)
			{
				point.position = CPoint(distX(gen), distY(gen));
			}
			drawCircle();
			Invalidate();
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
		}).detach();
}


void CMFCdrawGardenDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialogEx::OnLButtonDown(nFlags, point);
	if (clickPoints.size() < POINT)
	{
		// 사용자 입력값 가져오기
		pointRadius = GetDlgItemInt(IDC_EDIT_POINT_RDS, NULL, FALSE);
		editThickness = GetDlgItemInt(IDC_EDIT_THICKNESS, NULL, FALSE);

		circleThickness = editThickness;

		// 클릭한 원의 정보 저장
		ClickPoint newPoint;
		newPoint.position = point;
		newPoint.radius = pointRadius;  // 반지름 설정
		newPoint.thickness = editThickness;  // 두께 적용
		clickPoints.push_back(newPoint);

		//사용자가 점 3개를 다 클릭했을 시 정원 그리기
		if (clickPoints.size() == POINT) 
		{
			drawCircle();
		}
		Invalidate();
	}

	
	else
	{
		for (int i = 0; i < POINT; i++)
		{
			if (abs(clickPoints[i].position.x - point.x) < 10 && abs(clickPoints[i].position.y - point.y) < 10)
			{
				isDragging = true;
				indexDrag = i;
				break;
			}
		}
	}
	Invalidate();
}

void CMFCdrawGardenDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	//마우스를 떼면 드래그 상태 해제

	isDragging = false;
	indexDrag = -1;

	CDialogEx::OnLButtonUp(nFlags, point);
}

void CMFCdrawGardenDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if (isDragging && indexDrag != -1)
	{
		clickPoints[indexDrag].position = point; //드래그한 위치로 점 이동
		drawCircle();  //움직임에 따라 원 다시 그리기
		Invalidate(); 
	}

	CDialogEx::OnMouseMove(nFlags, point);
}


void CMFCdrawGardenDlg::drawCircle()
{
	if (clickPoints.size() < POINT)
	{
		return;
	}

	CPoint nPoint1 = clickPoints[0].position;
	CPoint nPoint2 = clickPoints[1].position;
	CPoint nPoint3 = clickPoints[2].position;

	// 3점의 center, radius 계산
	calculateCircle(nPoint1, nPoint2, nPoint3, circleCenter, circleRadius);

	Invalidate();  //원 다시 실시간으로 그리기 위해 WM_PAINT 호출
}


bool CMFCdrawGardenDlg::calculateCircle(CPoint p1, CPoint p2, CPoint p3, CPoint& center, int& radius)
{
	int x1 = p1.x, y1 = p1.y;
	int x2 = p2.x, y2 = p2.y;
	int x3 = p3.x, y3 = p3.y;

	//원의 중심 좌표
	double A = x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2);

	if (A == 0.0) return false;  // 세 점이 일직선일 때

	double B = (x1 * x1 + y1 * y1) * (y3 - y2) + (x2 * x2 + y2 * y2) * (y1 - y3) + (x3 * x3 + y3 * y3) * (y2 - y1);
	double C = (x1 * x1 + y1 * y1) * (x2 - x3) + (x2 * x2 + y2 * y2) * (x3 - x1) + (x3 * x3 + y3 * y3) * (x1 - x2);

	//원의 중심 좌표
	double centerX = -B / (2.0 * A);
	double centerY = -C / (2.0 * A);

	//반지름 계산
	double r = sqrt((centerX - x1) * (centerX - x1) + (centerY - y1) * (centerY - y1));

	center.x = static_cast<int>(centerX);
	center.y = static_cast<int>(centerY);
	radius = r;

	return true;
}

void CMFCdrawGardenDlg::OnEnChangeEditThickness()
{
	UpdateEditThickness();
	Invalidate();
}

void CMFCdrawGardenDlg::UpdateEditThickness()
{
	BOOL success;
	int newThickness = GetDlgItemInt(IDC_EDIT_THICKNESS, &success, FALSE);
	if (success && newThickness > 0)
	{
		circleThickness = newThickness;
	}
}


void CMFCdrawGardenDlg::OnEnChangeEditPointRds()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateEditRadius();
	Invalidate();
}

void CMFCdrawGardenDlg::UpdateEditRadius()
{
	BOOL success;
	int newRadius = GetDlgItemInt(IDC_EDIT_THICKNESS, &success, FALSE);
	if (success && newRadius > 0)
	{
		pointRadius = newRadius;
	}
}
