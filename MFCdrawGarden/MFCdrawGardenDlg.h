
// MFCdrawGardenDlg.h: 헤더 파일
//
#include <iostream>
#include <vector>
#include <GdiPlus.h>
#include <random>
#include <thread>
#include <chrono>
using namespace std;
#pragma once

// GDI+ 초기화 클래스
class CGdiPlusInit
{
public:
	CGdiPlusInit() { Gdiplus::GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL); }
	~CGdiPlusInit() { Gdiplus::GdiplusShutdown(m_gdiplusToken); }
private:
	Gdiplus::GdiplusStartupInput m_gdiplusStartupInput;
	ULONG_PTR m_gdiplusToken;
};

//사용자가 설정하는 클릭 원 구조체
struct ClickPoint {
	CPoint position;  // 클릭한 지점 좌표
	int radius;       // 원 반지름 크기
	int thickness;    // 원 테두리 두께
};

// CMFCdrawGardenDlg 대화 상자
class CMFCdrawGardenDlg : public CDialogEx
{
// 생성입니다.
public:
	CMFCdrawGardenDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCDRAWGARDEN_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

// 구현입니다.
protected:
	HICON m_hIcon;
	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	vector<ClickPoint> clickPoints;  // POINT 3개
	CPoint circleCenter; // 계산된 원의 중심 좌표
	int circleRadius; // 계산된 원의 반지름
	int circleThickness; // 원의 테두리 두께
	int editThickness; //사용자가 입력하는 원의 반지름 변수
	int pointRadius; //클릭한 원의 반지름 크기
	int indexDrag; // 드래그 중인 점 인덱스 /드래그 point (0~2), 드래그 중이 아닐 때 -1
	bool isDragging; // 드래그 여부 확인

public:
	afx_msg void OnBnClickedBtnReset();
	afx_msg void OnBnClickedBtnRandom();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnEnChangeEditThickness();
	afx_msg void OnEnChangeEditPointRds();
	bool calculateCircle(CPoint p1, CPoint p2, CPoint p3, CPoint& center, int& radius);
	void drawCircle();
	void UpdateEditThickness();
	void UpdateEditRadius();
};
