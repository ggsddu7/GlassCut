
// GlassCompositionDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "EditListCtrl.h"
#include "ColorListCtrl.h"


#include <vector>
using namespace std;
#define MESSAGEFROMTHREAD   (WM_USER+1001)
#define MESSAGEFROMTHREAD_CURVEC   (WM_USER+1002)
#define MESSAGEFROMTHREAD_CALCOM   (WM_USER+1003)
#define MESSAGEFROMTHREAD_CALCOM_RET   (WM_USER+1004)
// CGlassCompositionDlg dialog
class CGlassCompositionDlg : public CDialogEx
{
// Construction
public:
	CGlassCompositionDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_GLASSCOMPOSITION_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	HANDLE m_hThread;
	DWORD m_ThreadID;
	HANDLE m_hThread_Cal;
	DWORD m_ThreadID_Cal;
	CEditListCtrl list_Inputed;
	CEdit *edit_Client;
	CEdit *edit_Number;
	CEdit *edit_Length;
	CEdit *edit_Width;
	CEdit *edit_Number_Of_Chips;
	afx_msg void OnBnClickedButtonInputConfirm();
	//int index_Inputed;
	void WriteToExcel(CString savePath);
	afx_msg void OnBnClickedButtonSave();
	CString GetExcelDriver(void);
	afx_msg void OnBnClickedButtonRead();
	void ReadFromExcel(CString openPath);
	afx_msg void OnLvnColumnclickListInputed(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonClear();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	int totalNeedNumber;
	int totalRecords;
	afx_msg void OnBnClickedButtonCalculate();
	vector<vector<int>> CalculateCom(vector<int> vs, int m);
	vector<vector<int>> CalculateComAdjust(vector<int> vs, int m);
	int CaculateNeedSum(vector<int> v);
	bool SearchCommon(vector<int> a, vector<int> b);
	vector<vector<int>> CalculateGroupCom(int master, int customer, int sum, int shiftTimes, vector<int> vNeed, vector<vector<int>> resultCom);	
	vector<vector<int>> CalculateGroupComAdjust(int master, int customer, int sum, int shiftTimes, vector<int> vNeed, vector<vector<int>> resultCom);
	vector<int> reverse(vector<int> arr, int left, int right);
	vector<int> arrayShift(vector<int> a, int m, int len);
	vector<int> GetReResult(vector<vector<int>> mtx, vector<int> sumSeq);
	vector<vector<int>> UniqueResult(vector<vector<int>> relt);
	CListCtrl list_Result_General;
	int relt_General_Index;
	int relt_Adjust_Index;
	CColorListCtrl list_Result_Adjust;
	CProgressCtrl mPro_General;
	CProgressCtrl mPro_Adjust;
	CButton mBut_Calculate;
	afx_msg void OnLvnItemchangedListInputed(NMHDR *pNMHDR, LRESULT *pResult);
	CEdit m_Difference;
	CEdit m_Edit_Uniq11;
	CEdit m_Edit_Uniq12;
	CEdit m_Edit_Uniq13;
	CEdit m_Edit_Uniq14;
	CEdit m_Edit_Uniq21;
	CEdit m_Edit_Uniq22;
	CEdit m_Edit_Uniq23;
	CEdit m_Edit_Uniq24;
	CEdit m_Edit_Uniq31;
	CEdit m_Edit_Uniq32;
	CEdit m_Edit_Uniq33;
	CEdit m_Edit_Uniq34;
	int FindUniq(int val);
	bool JudgeUniq(vector<int> v);
	CEdit m_edit_maxcomnum;
	double totalArea;
	vector<int> nn_edit;
	bool JudgeSameID(vector<int> v);
	vector<int> v_Id;
	vector<vector<int>> vv_Id;
	bool JudgeSameValue(vector<vector<int>> vv);
//	afx_msg void OnClose();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnNMClickListResultAdjust(NMHDR *pNMHDR, LRESULT *pResult);
	CListCtrl list_Show_Result;
	vector<int> CStringToIntArray(CString str);
	afx_msg void OnNMClickListResultGeneral(NMHDR *pNMHDR, LRESULT *pResult);
	CEdit m_Edit_PIC_Type;
	//
protected:
	afx_msg LRESULT OnMessageFromThread(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMessageFromThreadCurVec(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMessageFromThreadCalcom(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMessageFromThreadCalcomRET(WPARAM wParam, LPARAM lParam);
public:
	CEdit m_Edit_MiniQlys;
	CEdit m_Edit_CurPos;
	void calBestOptResult();
	afx_msg void OnBnClickedButtonCalcmbsort();
	//void (*pFunc)() = &calBestOptResult();
};
vector<vector<int>> UniqueResultCal(vector<vector<int>> relt);
vector<vector<int>> CalculateGlobalCom(void* hWnd, vector<int> needChips, int cobchpNum);
void CalCombByNumb(void* hWnd);
bool betterThanBest(vector<int> lastR);
void GetMinCombs(void* hWnd);
void CalOptCombIter(void* hWnd, vector<int> cur, int curPos);