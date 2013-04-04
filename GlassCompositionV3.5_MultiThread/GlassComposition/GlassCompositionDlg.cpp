
// GlassCompositionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GlassComposition.h"
#include "GlassCompositionDlg.h"
#include "afxdialogex.h"
#include "global.h"
#include "gmp.h"
#include <set>
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

static	CRITICAL_SECTION	cs;


struct THREADDATA
{
	int miniNum;
	int curPos;
};

vector<int> vs_NeedNumber;
vector<int> repr;
vector<vector<int>> result_finally;
vector<vector<int>> result_adjust_finally;
vector<vector<int>> uniq;
vector<vector<int>> relt;
vector<vector<int>> reltM;
vector<vector<int>> OptRelt;
vector<int> vShow;
vector<int> bestRecords;
vector<int> relt_Mapping;
vector<int> vLen;
vector<int> vWid;
int miniQlys = 0;
int showIndex = 0;
int runTime = 0;
int totalNeedNumberOfChips = 0;
int maxChipNums = 0;
bool threadState = false;
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
	//	virtual void OnOK();
	//	virtual void OnCancel();
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CGlassCompositionDlg dialog




CGlassCompositionDlg::CGlassCompositionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CGlassCompositionDlg::IDD, pParent)
	, edit_Client(NULL)
	, totalNeedNumber(0)
	, totalRecords(0)
	, relt_General_Index(0)
	, relt_Adjust_Index(0)
	, totalArea(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGlassCompositionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_INPUTED, list_Inputed);
	DDX_Control(pDX, IDC_LIST_RESULT_GENERAL, list_Result_General);
	DDX_Control(pDX, IDC_LIST_RESULT_ADJUST, list_Result_Adjust);
	DDX_Control(pDX, IDC_PROGRESS_GENERAL, mPro_General);
	DDX_Control(pDX, IDC_PROGRESS_ADJUST, mPro_Adjust);
	DDX_Control(pDX, IDC_BUTTON_CALCULATE, mBut_Calculate);
	DDX_Control(pDX, IDC_EDIT_DIFFERENCE, m_Difference);
	DDX_Control(pDX, IDC_EDIT_UNIQUE_11, m_Edit_Uniq11);
	DDX_Control(pDX, IDC_EDIT_UNIQUE_12, m_Edit_Uniq12);
	DDX_Control(pDX, IDC_EDIT_UNIQUE_13, m_Edit_Uniq13);
	DDX_Control(pDX, IDC_EDIT_UNIQUE_14, m_Edit_Uniq14);
	DDX_Control(pDX, IDC_EDIT_UNIQUE_21, m_Edit_Uniq21);
	DDX_Control(pDX, IDC_EDIT_UNIQUE_22, m_Edit_Uniq22);
	DDX_Control(pDX, IDC_EDIT_UNIQUE_23, m_Edit_Uniq23);
	DDX_Control(pDX, IDC_EDIT_UNIQUE_24, m_Edit_Uniq24);
	DDX_Control(pDX, IDC_EDIT_UNIQUE_31, m_Edit_Uniq31);
	DDX_Control(pDX, IDC_EDIT_UNIQUE_32, m_Edit_Uniq32);
	DDX_Control(pDX, IDC_EDIT_UNIQUE_33, m_Edit_Uniq33);
	DDX_Control(pDX, IDC_EDIT_UNIQUE_34, m_Edit_Uniq34);
	DDX_Control(pDX, IDC_EDIT_MAXCOMNUM, m_edit_maxcomnum);
	DDX_Control(pDX, IDC_LIST_SHOW_RESULT, list_Show_Result);
	DDX_Control(pDX, IDC_EDIT_PIC_TYPE, m_Edit_PIC_Type);
	DDX_Control(pDX, IDC_EDIT_MiniQlys, m_Edit_MiniQlys);
	DDX_Control(pDX, IDC_EDIT_CURPOS, m_Edit_CurPos);
}

BEGIN_MESSAGE_MAP(CGlassCompositionDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()	
	ON_BN_CLICKED(IDC_BUTTON_INPUT_CONFIRM, &CGlassCompositionDlg::OnBnClickedButtonInputConfirm)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CGlassCompositionDlg::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_READ, &CGlassCompositionDlg::OnBnClickedButtonRead)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_INPUTED, &CGlassCompositionDlg::OnLvnColumnclickListInputed)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CGlassCompositionDlg::OnBnClickedButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_CALCULATE, &CGlassCompositionDlg::OnBnClickedButtonCalculate)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_INPUTED, &CGlassCompositionDlg::OnLvnItemchangedListInputed)
	ON_BN_CLICKED(IDCANCEL, &CGlassCompositionDlg::OnBnClickedCancel)
	ON_NOTIFY(NM_CLICK, IDC_LIST_RESULT_ADJUST, &CGlassCompositionDlg::OnNMClickListResultAdjust)
	ON_NOTIFY(NM_CLICK, IDC_LIST_RESULT_GENERAL, &CGlassCompositionDlg::OnNMClickListResultGeneral)
	ON_MESSAGE(MESSAGEFROMTHREAD,&CGlassCompositionDlg::OnMessageFromThread)
	ON_MESSAGE(MESSAGEFROMTHREAD_CURVEC,&CGlassCompositionDlg::OnMessageFromThreadCurVec)
	ON_MESSAGE(MESSAGEFROMTHREAD_CALCOM,&CGlassCompositionDlg::OnMessageFromThreadCalcom)
	ON_MESSAGE(MESSAGEFROMTHREAD_CALCOM_RET,&CGlassCompositionDlg::OnMessageFromThreadCalcomRET)
	ON_BN_CLICKED(IDC_BUTTON_CALCMBSORT, &CGlassCompositionDlg::OnBnClickedButtonCalcmbsort)
END_MESSAGE_MAP()


// CGlassCompositionDlg message handlers

BOOL CGlassCompositionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	//pFunc = &calBestOptResult;
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	// TODO: Add extra initialization here
	RECT rect;
	list_Inputed.GetClientRect( &rect );
	int width_list = rect.right - rect.left;

	list_Inputed.SetExtendedStyle(
		LVS_EX_FLATSB     // 扁平风格滚动
		| 
		LVS_EX_FULLROWSELECT     // 允许正航选中
		//| LVS_EX_HEADERDRAGDROP     // 允许标题拖拽
		| LVS_EX_GRIDLINES     // 画出网格线
		);
	list_Inputed.InsertColumn(0, "客户", LVCFMT_CENTER, 50);
	list_Inputed.InsertColumn(1, "编号", LVCFMT_CENTER, 40);	
	list_Inputed.InsertColumn(2, "长度 (mm)", LVCFMT_CENTER, 80);
	list_Inputed.InsertColumn(3, "宽度 (mm)", LVCFMT_CENTER, 80);
	list_Inputed.InsertColumn(4, "面积 (mm*mm)", LVCFMT_CENTER, 0);	
	list_Inputed.InsertColumn(5, "需求数量", LVCFMT_CENTER, 80);
	list_Inputed.InsertColumn(6, "总面积 (cm*cm)", LVCFMT_CENTER);
	list_Inputed.InsertColumn(7, "图片型号", LVCFMT_CENTER, 80);
	/*for (int i = 2; i <= 3; i ++)
	{
		list_Inputed.SetColumnWidth(i, (width_list - 370) / 2);
	}*/
	list_Inputed.SetColumnWidth(6, (width_list - 410));
	//-----------设置详细显示list-------------
	list_Show_Result.GetClientRect( &rect );
	width_list = rect.right - rect.left;
	list_Show_Result.SetExtendedStyle(
		LVS_EX_FLATSB     // 扁平风格滚动
		| LVS_EX_FULLROWSELECT     // 允许正航选中
		| LVS_EX_HEADERDRAGDROP     // 允许标题拖拽
		| LVS_EX_GRIDLINES     // 画出网格线
		);

	list_Show_Result.InsertColumn(0, "客户", LVCFMT_CENTER, 50);
	list_Show_Result.InsertColumn(1, "编号", LVCFMT_CENTER, 40);
	list_Show_Result.InsertColumn(2, "长度", LVCFMT_CENTER, 50);
	list_Show_Result.InsertColumn(3, "宽度", LVCFMT_CENTER, 50);
	list_Show_Result.InsertColumn(4, "数量", LVCFMT_CENTER, 40);
	list_Show_Result.InsertColumn(5, "总面积", LVCFMT_CENTER);
	list_Show_Result.InsertColumn(6, "图片类型", LVCFMT_CENTER, 70);
	list_Show_Result.SetColumnWidth(5, width_list - 300);
	//-------------------------------------


	list_Result_General.SetExtendedStyle(
		LVS_EX_FLATSB     // 扁平风格滚动
		| 
		LVS_EX_FULLROWSELECT     // 允许正航选中
		//| LVS_EX_HEADERDRAGDROP     // 允许标题拖拽
		| LVS_EX_GRIDLINES     // 画出网格线
		);

	list_Result_General.InsertColumn(0, "已组好情况", LVCFMT_CENTER);
	list_Result_General.InsertColumn(1, "片数", LVCFMT_CENTER, 50);
	RECT rt;
	list_Result_General.GetClientRect( &rt );
	list_Result_General.SetColumnWidth(0, rt.right - rt.left - 50);
	//int a = rt.right - rt.left - 50;
	//list_Result_Adjust.SetExtendedStyle(
	//	LVS_EX_FLATSB     // 扁平风格滚动
	//	| 
	//	LVS_EX_FULLROWSELECT     // 允许正航选中
	//	//| LVS_EX_HEADERDRAGDROP     // 允许标题拖拽
	//	| LVS_EX_GRIDLINES     // 画出网格线
	//	);
	//list_Result_Adjust.InsertColumn(0, "已组好情况", LVCFMT_CENTER);
	//list_Result_Adjust.InsertColumn(1, "片数", LVCFMT_CENTER, 50);
	//RECT rta;
	//list_Result_Adjust.GetClientRect( &rta );
	//list_Result_Adjust.SetColumnWidth(0, rta.right - rta.left - 50);
	list_Result_Adjust.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	list_Result_Adjust.SetHeadings(_T("已组好情况,194, ;片数,50"));

	/*mPro_General.SetRange(0, 90);
	mPro_General.SetStep(10);*/

	/*mPro_Adjust.SetRange(0, 90);
	mPro_Adjust.SetStep(10);*/

	m_Difference.SetWindowText("5");
	m_edit_maxcomnum.SetWindowText("10");
	/*CString temp;
	m_Difference.GetWindowText(temp);
	int a = atoi(temp);*/
	//AfxMessageBox();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CGlassCompositionDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CGlassCompositionDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CGlassCompositionDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CGlassCompositionDlg::OnBnClickedButtonInputConfirm()
{
	BOOL input_Flag = TRUE;

	CString str_Client;
	edit_Client = (CEdit *)GetDlgItem(IDC_EDIT_CLIENT);
	edit_Client->GetWindowText( str_Client );
	if (str_Client.IsEmpty())
	{
		AfxMessageBox("请填写客户");
		input_Flag = FALSE;
	}

	CString str_Num ;
	edit_Number = (CEdit*)GetDlgItem(IDC_EDIT_NUMBER);
	edit_Number->GetWindowText( str_Num );
	if (str_Num.IsEmpty())
	{
		AfxMessageBox("请填写编号");
		input_Flag = FALSE;
	}
	int num = atoi(str_Num);
	vector<int> t = v_Id;
	t.push_back(num);
	if (!JudgeSameID(t))
	{
		//judge
		AfxMessageBox("编号重复，请仔细核查");
		input_Flag = FALSE;
		edit_Number->SetFocus();
	}

	CString str_Len ;
	edit_Length = (CEdit *)GetDlgItem(IDC_EDIT_LENGTH);
	edit_Length->GetWindowText( str_Len );
	if (str_Len.IsEmpty())
	{
		AfxMessageBox("请填写长度");
		input_Flag = FALSE;
	}
	int len = atoi(str_Len);

	CString str_Wid ;
	edit_Width = (CEdit *)GetDlgItem(IDC_EDIT_WIDTH);
	edit_Width->GetWindowText( str_Wid );
	if (str_Wid.IsEmpty())
	{
		AfxMessageBox("请填写宽度");
		input_Flag = FALSE;
	}
	int wid = atoi(str_Wid);
	int area_Glass = len * wid;

	CString str_area;
	str_area.Format("%d", area_Glass);

	CString str_Num_Of_Chips ;
	edit_Number_Of_Chips = (CEdit *)GetDlgItem(IDC_EDIT_NUMBER_OF_CHIPS);
	edit_Number_Of_Chips->GetWindowText( str_Num_Of_Chips );
	if (str_Num_Of_Chips.IsEmpty())
	{
		AfxMessageBox("请填写需求数量");
		input_Flag = FALSE;
	}
	int num_of_chips = atoi(str_Num_Of_Chips);
	//------------------
	CString str_Pic_Type;
	m_Edit_PIC_Type.GetWindowText( str_Pic_Type );
	if (str_Pic_Type.IsEmpty())
	{
		AfxMessageBox("请填写图片类型");
		input_Flag = FALSE;
	}

	//------------------
	vector<int> a;
	a.push_back(len);
	a.push_back(wid);
	a.push_back(num_of_chips);
	vector<vector<int>> aa = vv_Id;
	aa.push_back(a);
	if (!JudgeSameValue(aa))
	{
		AfxMessageBox("输入的长宽片数完全相同，请核查");
		edit_Length->SetFocus();
		input_Flag = FALSE;
	}
	//-----
	double area_Glass_tt = (double)area_Glass * num_of_chips / (double)100;
	CString str_area_tt;
	str_area_tt.Format("%.2lf", area_Glass_tt);
	//str_area_tt.Format("%g", 0.02200);
	//-----

	int nItem;
	CString str_TN;
	if ( input_Flag )
	{
		nItem = list_Inputed.InsertItem( totalRecords, "");
		list_Inputed.SetItemText(nItem, 0, str_Client);
		list_Inputed.SetItemText(nItem, 1, str_Num);
		list_Inputed.SetItemText(nItem, 2, str_Len);
		list_Inputed.SetItemText(nItem, 3, str_Wid);
		list_Inputed.SetItemText(nItem, 4, str_area);
		list_Inputed.SetItemText(nItem, 5, str_Num_Of_Chips);
		list_Inputed.SetItemText(nItem, 6, str_area_tt);
		list_Inputed.SetItemText(nItem, 7, str_Pic_Type);
		totalRecords ++;

		//edit_Client->SetWindowText(NULL);
		edit_Number->SetWindowText(NULL);
		edit_Length->SetWindowText(NULL);
		edit_Width->SetWindowText(NULL);
		edit_Number_Of_Chips->SetWindowText(NULL);
		edit_Client->SetFocus();

		totalNeedNumber += num_of_chips;
		totalNeedNumberOfChips = totalNeedNumber;
		//-----
		nn_edit.push_back(num_of_chips);	
		v_Id.push_back(num);
		vv_Id.push_back(a);
		//-----
		totalArea += area_Glass_tt;
		str_TN.Format("%d", totalNeedNumber);
		GetDlgItem(IDC_TOTAL_NUMBER)->SetWindowText(str_TN);
		str_TN.Format("%.2lf", totalArea);
		GetDlgItem(IDC_TOTAL_AREA)->SetWindowText(str_TN);
	}
}



void CGlassCompositionDlg::WriteToExcel(CString savePath)
{
	CDatabase database;
	CString sDriver = "MICROSOFT EXCEL DRIVER (*.XLS)";
	CString sExcelFile = savePath;
	CString sSql;

	try
	{
		// 创建进行存取的字符串
		sSql.Format("DRIVER={%s};DSN='';FIRSTROWHASNAMES=1;READONLY=FALSE;CREATE_DB=\"%s\";DBQ=%s",sDriver, sExcelFile, sExcelFile);

		// 创建数据库 (既Excel表格文件)
		if( database.OpenEx(sSql,CDatabase::noOdbcDialog) )
		{
			//注意表明的设置，不能以下划线 " _ " 开头
			sSql = "CREATE TABLE statistics (Client_GC VARCHAR,Index_GC INT,Length_GC INT,Width_GC INT,Area_GC INT,Chips_GC INT,Area_Total_GC DOUBLE,PIC_TYPE_GC VARCHAR)";
			//sSql = "CREATE TABLE statistics (Client_GC VARCHAR,Index_GC INT,Length_GC INT,Width_GC INT,Area_GC INT,Chips_GC INT,Area_Total_GC DOUBLE)";
			database.ExecuteSQL(sSql);

			/*sSql = "INSERT INTO Exceldemo (_Client,_Index,Length,Width,_Area,_Chips) VALUES ('Jelly',1,20,15,300,4)";
			database.ExecuteSQL(sSql);*/
			int nCount = list_Inputed.GetItemCount();

			if (nCount > 0)
			{
				for (int i = 0; i < nCount; i ++)
				{
					sSql = "INSERT INTO statistics (Client_GC,Index_GC,Length_GC,Width_GC,Area_GC,Chips_GC,Area_Total_GC,PIC_TYPE_GC) VALUES ('" + list_Inputed.GetItemText(i, 0) +"',"+ list_Inputed.GetItemText(i, 1) +","+ list_Inputed.GetItemText(i, 2) + "," + list_Inputed.GetItemText(i, 3) + "," + list_Inputed.GetItemText(i, 4) + "," + list_Inputed.GetItemText(i, 5) + "," + list_Inputed.GetItemText(i, 6) + ",'" + list_Inputed.GetItemText(i, 7) + "')";
					//sSql = "INSERT INTO statistics (Client_GC,Index_GC,Length_GC,Width_GC,Area_GC,Chips_GC,Area_Total_GC) VALUES ('" + list_Inputed.GetItemText(i, 0) +"',"+ list_Inputed.GetItemText(i, 1) +","+ list_Inputed.GetItemText(i, 2) + "," + list_Inputed.GetItemText(i, 3) + "," + list_Inputed.GetItemText(i, 4) + "," + list_Inputed.GetItemText(i, 5) + "," + list_Inputed.GetItemText(i, 6) + ")";
					database.ExecuteSQL(sSql);
				}
			}
			else
			{
				AfxMessageBox("没有数据");
			}
		}      

		// 关闭数据库
		database.Close();

		AfxMessageBox("Excel文件写入成功！");
	}
	catch (CDBException ex)
	{
		AfxMessageBox("Excel驱动没有安装");
		/*TRACE1("Excel驱动没有安装: %s",sDriver);*/
		ex.ReportError();
	}
}


void CGlassCompositionDlg::OnBnClickedButtonSave()
{
	//保存文件对话框：
	CFileDialog dlg(
		FALSE, _T("bmp"), NULL,
		OFN_EXTENSIONDIFFERENT|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_HIDEREADONLY,
		_T("excel (*.xls) |*.xls"), NULL
		);
	// 打开文件对话框的标题名
	dlg.m_ofn.lpstrTitle = _T("Save Excel");
	// 判断是否获得图片
	if( dlg.DoModal() != IDOK )	
		return;

	CString str = dlg.GetPathName();
	//检查是否有同名文件
	if(PathFileExists(str))
	{
		int nSelect = AfxMessageBox("您将覆盖文件" + str, MB_OKCANCEL);
		if(nSelect == IDOK) 
		{ 
			//   选择了OK 
			DeleteFile(str);
			WriteToExcel(str);
		} 
		if(nSelect == IDCANCEL) 
		{ 
			//   选择了Cancel 
		}

	}
	else
	{
		WriteToExcel(str);
	}


}


CString CGlassCompositionDlg::GetExcelDriver(void)
{
	char szBuf[2001];
	WORD cbBufMax = 2000;
	WORD cbBufOut;
	char *pszBuf = szBuf;
	CString sDriver;

	// 获取已安装驱动的名称(涵数在odbcinst.h里)
	if (!SQLGetInstalledDrivers(szBuf, cbBufMax, &cbBufOut))
		return "";

	// 检索已安装的驱动是否有Excel...
	do
	{
		if (strstr(pszBuf, "Excel") != 0)
		{
			//发现 !
			sDriver = CString(pszBuf);
			break;
		}
		pszBuf = strchr(pszBuf, '\0') + 1;
	}
	while (pszBuf[1] != '\0');

	return sDriver;
}


void CGlassCompositionDlg::OnBnClickedButtonRead()
{
	CFileDialog dlg(
		TRUE, _T("*.bmp"), NULL,
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY,
		_T("excel (*.xls) |*.xls"), NULL
		);
	// 打开文件对话框的标题名
	dlg.m_ofn.lpstrTitle = _T("Open Excel");
	// 判断是否获得图片
	if( dlg.DoModal() != IDOK )	
		return;
	// 获取图片路径	
	CString mPath = dlg.GetPathName();

	/*CString temp;
	double a = (double)(1.0 / 10);
	temp.Format("%lf", a);
	GetDlgItem(IDC_STATIC_RATIO)->SetWindowText(temp);*/
	//int a = 5 / 8;
	/*int a = 256;
	int b = a & 0xff;
	if (b == 0x0000)
	{
	int c = 0;
	}*/

	if(totalRecords != 0)
	{
		int nSelect = AfxMessageBox("列表中存在数据,'OK':融合数据,'Cancel':返回", MB_OKCANCEL);
		if(nSelect == IDOK) 
		{ 
			//   选择了OK 
			list_Inputed.SetRedraw(FALSE);
			ReadFromExcel(mPath);
			list_Inputed.SetRedraw(TRUE);
		} 
		if(nSelect == IDCANCEL) 
		{ 
			//   选择了Cancel 

		}

	}
	else
	{
		list_Inputed.SetRedraw(FALSE);
		ReadFromExcel(mPath);
		list_Inputed.SetRedraw(TRUE);
	}
}


void CGlassCompositionDlg::ReadFromExcel(CString openPath)
{
	CDatabase database;
	CString sSql;
	CString _Client, _Index, _Length, _Width, _Area, _Chips, _Area_Total, _Pic_Type;

	CString sDriver;
	CString sDsn;
	CString sFile = openPath; 
	//AfxMessageBox(sFile);
	// 检索是否安装有Excel驱动 "Microsoft Excel Driver (*.xls)" 
	sDriver = GetExcelDriver();
	//AfxMessageBox(sDriver);

	if (sDriver.IsEmpty())
	{
		// 没有发现Excel驱动
		AfxMessageBox("没有安装Excel驱动!");
		return;
	}

	// 创建进行存取的字符串
	sDsn.Format("ODBC;DRIVER={%s};DSN='''';DBQ=%s", sDriver, sFile);

	TRY
	{
		// 打开数据库(既Excel文件)
		database.Open(NULL, false, false, sDsn);

		CRecordset recset(&database);


		// 设置读取的查询语句.注意此处表名的写法，excel里的表默认为系统表，所以访问表时表明应为[表明$]
		//sSql = "SELECT * FROM [Exceldemo$] ORDER BY Area_GC";
		sSql = "SELECT * FROM [statistics$]";

		// 执行查询语句
		recset.Open(CRecordset::forwardOnly, sSql, CRecordset::readOnly);
		//int nItem = 0;
		// 获取查询结果
		while (!recset.IsEOF())
		{
			//读取Excel内部数值
			/*recset.GetFieldValue("Name ", sItem1);
			recset.GetFieldValue("Age", sItem2);*/
			recset.GetFieldValue("Client_GC", _Client);
			recset.GetFieldValue("Index_GC", _Index);
			recset.GetFieldValue("Length_GC", _Length);
			recset.GetFieldValue("Width_GC", _Width);
			recset.GetFieldValue("Area_GC", _Area);
			recset.GetFieldValue("Chips_GC", _Chips);
			recset.GetFieldValue("Area_Total_GC", _Area_Total);
			recset.GetFieldValue("PIC_TYPE_GC", _Pic_Type);

			totalRecords = list_Inputed.InsertItem( totalRecords, "");
			list_Inputed.SetItemText(totalRecords, 0, _Client);

			int idx = atoi(_Index);
			_Index.Format("%d", idx);
			list_Inputed.SetItemText(totalRecords, 1, _Index);

			int len = atoi(_Length);
			_Length.Format("%d", len);
			list_Inputed.SetItemText(totalRecords, 2, _Length);

			int wid = atoi(_Width);
			_Width.Format("%d", wid);
			list_Inputed.SetItemText(totalRecords, 3, _Width);

			int ara = atoi(_Area);
			_Area.Format("%d", ara);
			list_Inputed.SetItemText(totalRecords, 4, _Area);

			int cps = atoi(_Chips);
			_Chips.Format("%d", cps);
			list_Inputed.SetItemText(totalRecords, 5, _Chips);			

			double tarea = atof(_Area_Total);
			_Area_Total.Format("%.2lf", tarea);
			list_Inputed.SetItemText(totalRecords, 6, _Area_Total);

			list_Inputed.SetItemText(totalRecords, 7, _Pic_Type);

			totalNeedNumber += cps;
			totalNeedNumberOfChips = totalNeedNumber;
			totalArea += tarea;
			//-------------------
			nn_edit.push_back(cps);
			v_Id.push_back(idx);
			vector<int> a;
			a.push_back(len);
			a.push_back(wid);
			a.push_back(cps);
			vv_Id.push_back(a);
			//-------------------
			CString str_TN;
			str_TN.Format("%d", totalNeedNumber);
			GetDlgItem(IDC_TOTAL_NUMBER)->SetWindowText(str_TN);
			str_TN.Format("%.2lf", totalArea);
			GetDlgItem(IDC_TOTAL_AREA)->SetWindowText(str_TN);
			// 移到下一行
			totalRecords ++;
			recset.MoveNext();
		}
		if (!JudgeSameID(v_Id))
		{
			AfxMessageBox("存在重复编号，请检查");
		}
		if (!JudgeSameValue(vv_Id))
		{
			AfxMessageBox("存在重复长宽片数，请检查");
		}
		// 关闭数据库
		database.Close();

	}
	CATCH(CDBException, e)
	{
		// 数据库操作产生异常时...
		AfxMessageBox("数据库错误: " + e->m_strError);
	}
	END_CATCH;
}

static int CALLBACK listCompare(LPARAM lParam1 , LPARAM lParam2 , LPARAM lParamSort)
{
	//AfxMessageBox("listCompare");

	DATA* dat = (DATA*)lParamSort;

	/*CString s;
	s.Format( "OnColumnclick   选择列%d ",dat->subitem); 
	AfxMessageBox(s); */

	int col = dat->subitem;

	CString strItem1 = dat->plist->GetItemText(lParam1, col);
	CString strItem2 = dat->plist->GetItemText(lParam2, col);
	//----------------------------------------------------
	if (col != 0) //比较整数
	{
		//比较两个数 
		LPCTSTR s1=(LPCTSTR)strItem1; 
		LPCTSTR s2=(LPCTSTR)strItem2; 

		//将字符串转为int型比较
		int n1=atoi(s1); 
		int n2=atoi(s2);

		if (fav)//fav是用来判断是升序还是降序排列的函数  TRUE: 升序， FALSE: 降序
		{// 比较两个数 如果返回-1表示n1排在n2前面，如果返回1表示n1排在n2后面，如果相等返回0，这里没有判断相等的情况
			if (n1<n2)  
				return -1; 
			else 
				return 1;
		} 
		else
		{
			if (n1<n2) 
				return 1; 
			else 
				return -1;
		}
	}
	else //比较字符串
	{
		LVCOLUMN Vol; 
		CString csStr("");
		TCHAR szCol[MAX_PATH];
		Vol.pszText = szCol; 
		Vol.mask=LVCF_TEXT; 
		Vol.cchTextMax=sizeof(szCol); 
		dat->plist->GetColumn(col,&Vol); 
		csStr = CString(Vol.pszText); 

		if (fav)
		{
			csStr=csStr+"▲";
		}
		else
		{
			csStr=csStr+"▼";
		}
		/*TCHAR* disp;
		disp = csStr.GetBuffer(csStr.GetLength());
		Vol.pszText = disp;
		dat->plist->SetColumn(col, &Vol);*/
		if (csStr.Right(2) == CString("▼")) 
		{ 
			return _tcscmp(strItem2.GetBuffer(MAX_PATH),strItem1.GetBuffer(MAX_PATH)); 
		} 
		else if (csStr.Right(2) == CString("▲")) 
		{ 
			return _tcscmp(strItem1.GetBuffer(MAX_PATH),strItem2.GetBuffer(MAX_PATH)); 
		} 
		else 
		{ 
			/*nlen = (csStr + CString("▲")).GetLength();
			Vol.pszText = (csStr + CString("▲")).GetBuffer(nlen);
			dat->plist->SetColumn(col, &Vol);*/
			return _tcscmp(strItem1.GetBuffer(MAX_PATH),strItem2.GetBuffer(MAX_PATH)); 
		} 
	}
	//----------------------------------------------------
}

void CGlassCompositionDlg::OnLvnColumnclickListInputed(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	/*CString s;
	s.Format( "OnColumnclick   选择列%d ",pNMLV-> iSubItem); 
	AfxMessageBox(s); */

	//AfxMessageBox("OnLvnColumnclickListInputed");

	for (int i = 0; i < list_Inputed.GetItemCount(); i ++)
	{
		list_Inputed.SetItemData(i, i);
	}

	DATA data;
	data.subitem = pNMLV->iSubItem;
	data.plist = &list_Inputed;
	fav = !fav;

	list_Inputed.SortItems(listCompare, (LPARAM)&data);

	//LVCOLUMN column; 
	//CString str("Hello");
	//TCHAR *patch;
	//patch = str.GetBuffer(str.GetLength());// + 1结果相同
	////patch[str.GetLength()] = 0;
	//column.mask=LVCF_TEXT; 
	//column.pszText = patch; 
	//list_Inputed.SetColumn(1,&column);

	*pResult = 0;
}

void CGlassCompositionDlg::OnBnClickedButtonClear()
{
	vs_NeedNumber.clear();
	nn_edit.clear();
	v_Id.clear();
	vv_Id.clear();
	list_Inputed.DeleteAllItems();
	totalRecords = 0;
	totalNeedNumber = 0;
	totalNeedNumberOfChips = totalNeedNumber;
	totalArea = 0.0;

	CString str_TN;
	str_TN.Format("%d", totalNeedNumber);
	GetDlgItem(IDC_TOTAL_NUMBER)->SetWindowText(str_TN);
	str_TN.Format("%.2lf", totalArea);
	GetDlgItem(IDC_TOTAL_AREA)->SetWindowText(str_TN);
}


BOOL CGlassCompositionDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)  
	{
		if (GetFocus() == GetDlgItem(IDC_BUTTON_INPUT_CONFIRM))
		{
			BOOL input_Flag = TRUE;

			CString str_Client;
			edit_Client = (CEdit *)GetDlgItem(IDC_EDIT_CLIENT);
			edit_Client->GetWindowText( str_Client );
			if (str_Client.IsEmpty())
			{
				AfxMessageBox("请填写客户");
				input_Flag = FALSE;
			}

			CString str_Num ;
			edit_Number = (CEdit*)GetDlgItem(IDC_EDIT_NUMBER);
			edit_Number->GetWindowText( str_Num );
			if (str_Num.IsEmpty())
			{
				AfxMessageBox("请填写编号");
				input_Flag = FALSE;
			}
			int num = atoi(str_Num);
			vector<int> t = v_Id;
			t.push_back(num);
			if (!JudgeSameID(t))
			{
				//judge
				AfxMessageBox("编号重复，请仔细核查");
				input_Flag = FALSE;
				edit_Number->SetFocus();
			}

			CString str_Len ;
			edit_Length = (CEdit *)GetDlgItem(IDC_EDIT_LENGTH);
			edit_Length->GetWindowText( str_Len );
			if (str_Len.IsEmpty())
			{
				AfxMessageBox("请填写长度");
				input_Flag = FALSE;
			}
			int len = atoi(str_Len);

			CString str_Wid ;
			edit_Width = (CEdit *)GetDlgItem(IDC_EDIT_WIDTH);
			edit_Width->GetWindowText( str_Wid );
			if (str_Wid.IsEmpty())
			{
				AfxMessageBox("请填写宽度");
				input_Flag = FALSE;
			}
			int wid = atoi(str_Wid);
			int area_Glass = len * wid;

			CString str_area;
			str_area.Format("%d", area_Glass);

			CString str_Num_Of_Chips ;
			edit_Number_Of_Chips = (CEdit *)GetDlgItem(IDC_EDIT_NUMBER_OF_CHIPS);
			edit_Number_Of_Chips->GetWindowText( str_Num_Of_Chips );
			if (str_Num_Of_Chips.IsEmpty())
			{
				AfxMessageBox("请填写需求数量");
				input_Flag = FALSE;
			}
			int num_of_chips = atoi(str_Num_Of_Chips);
			//------------------
			CString str_Pic_Type;
			m_Edit_PIC_Type.GetWindowText( str_Pic_Type );
			if (str_Pic_Type.IsEmpty())
			{
				AfxMessageBox("请填写图片类型");
				input_Flag = FALSE;
			}

			//------------------
			vector<int> a;
			a.push_back(len);
			a.push_back(wid);
			a.push_back(num_of_chips);
			vector<vector<int>> aa = vv_Id;
			aa.push_back(a);
			if (!JudgeSameValue(aa))
			{
				AfxMessageBox("输入的长宽片数完全相同，请核查");
				edit_Length->SetFocus();
				input_Flag = FALSE;
			}
			//-----
			double area_Glass_tt = (double)area_Glass * num_of_chips / (double)100;
			CString str_area_tt;
			str_area_tt.Format("%.2lf", area_Glass_tt);
			//str_area_tt.Format("%g", 0.02200);
			//-----

			int nItem;
			CString str_TN;
			if ( input_Flag )
			{
				nItem = list_Inputed.InsertItem( totalRecords, "");
				list_Inputed.SetItemText(nItem, 0, str_Client);
				list_Inputed.SetItemText(nItem, 1, str_Num);
				list_Inputed.SetItemText(nItem, 2, str_Len);
				list_Inputed.SetItemText(nItem, 3, str_Wid);
				list_Inputed.SetItemText(nItem, 4, str_area);
				list_Inputed.SetItemText(nItem, 5, str_Num_Of_Chips);
				list_Inputed.SetItemText(nItem, 6, str_area_tt);
				list_Inputed.SetItemText(nItem, 7, str_Pic_Type);
				totalRecords ++;

				//edit_Client->SetWindowText(NULL);
				edit_Number->SetWindowText(NULL);
				edit_Length->SetWindowText(NULL);
				edit_Width->SetWindowText(NULL);
				edit_Number_Of_Chips->SetWindowText(NULL);
				edit_Client->SetFocus();

				totalNeedNumber += num_of_chips;
				totalNeedNumberOfChips = totalNeedNumber;
				//-----
				nn_edit.push_back(num_of_chips);	
				v_Id.push_back(num);
				vv_Id.push_back(a);
				//-----
				totalArea += area_Glass_tt;
				str_TN.Format("%d", totalNeedNumber);
				GetDlgItem(IDC_TOTAL_NUMBER)->SetWindowText(str_TN);
				str_TN.Format("%.2lf", totalArea);
				GetDlgItem(IDC_TOTAL_AREA)->SetWindowText(str_TN);
			}
			return TRUE;
		}
		else
		{
			return TRUE;
		}		
	}

	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)   
		return TRUE;

	if (GetFocus() == GetDlgItem(IDC_LIST_INPUTED))
	{
		if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_DELETE)
		{
			POSITION pos = NULL;
			while(pos = list_Inputed.GetFirstSelectedItemPosition())
			{
				int nItem = list_Inputed.GetNextSelectedItem(pos);
				if (nItem >= 0 && nItem < list_Inputed.GetItemCount())
				{		

					int nm = atoi(list_Inputed.GetItemText(nItem, 5));
					totalNeedNumber -= nm;
					totalNeedNumberOfChips = totalNeedNumber;
					double artt = atof(list_Inputed.GetItemText(nItem, 6));
					totalArea -= artt;
					CString str_TN;
					str_TN.Format("%d", totalNeedNumber);
					GetDlgItem(IDC_TOTAL_NUMBER)->SetWindowText(str_TN);
					str_TN.Format("%.2lf", totalArea);
					GetDlgItem(IDC_TOTAL_AREA)->SetWindowText(str_TN);

					list_Inputed.DeleteItem(nItem);
					//-------------------
					vector<int>::iterator iter = nn_edit.begin() + nItem;
					nn_edit.erase(iter);
					iter = v_Id.begin() + nItem;
					v_Id.erase(iter);
					vector<vector<int>>::iterator itr = vv_Id.begin() + nItem;
					vv_Id.erase(itr);
					//-------------------
					totalRecords --;
				}
			}
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CGlassCompositionDlg::OnBnClickedButtonCalculate()
{
	if (list_Result_General.GetItemCount() != 0 || list_Result_Adjust.GetItemCount() != 0)
	{
		list_Result_General.DeleteAllItems();
		list_Result_Adjust.DeleteAllItems();
		mPro_General.SetPos(0);
		mPro_Adjust.SetPos(0);
		AfxMessageBox("结果列表中数据将被清空");
	}
	//mBut_Calculate.EnableWindow(FALSE);
	//m_Difference.EnableWindow(FALSE);
	uniq.clear();
	//-----------------------------------------
	vector<int> vi;
	CString cstr;
	m_Edit_Uniq11.GetWindowText(cstr);
	if (!cstr.IsEmpty() && atoi(cstr) != 0)
	{
		vi.push_back(atoi(cstr));
	}
	m_Edit_Uniq12.GetWindowText(cstr);
	if (!cstr.IsEmpty() && atoi(cstr) != 0)
	{
		vi.push_back(atoi(cstr));
	}
	m_Edit_Uniq13.GetWindowText(cstr);
	if (!cstr.IsEmpty() && atoi(cstr) != 0)
	{
		vi.push_back(atoi(cstr));
	}
	m_Edit_Uniq14.GetWindowText(cstr);
	if (!cstr.IsEmpty() && atoi(cstr) != 0)
	{
		vi.push_back(atoi(cstr));
	}
	uniq.push_back(vi);
	vi.clear();
	m_Edit_Uniq21.GetWindowText(cstr);
	if (!cstr.IsEmpty() && atoi(cstr) != 0)
	{
		vi.push_back(atoi(cstr));
	}
	m_Edit_Uniq22.GetWindowText(cstr);
	if (!cstr.IsEmpty() && atoi(cstr) != 0)
	{
		vi.push_back(atoi(cstr));
	}
	m_Edit_Uniq23.GetWindowText(cstr);
	if (!cstr.IsEmpty() && atoi(cstr) != 0)
	{
		vi.push_back(atoi(cstr));
	}
	m_Edit_Uniq24.GetWindowText(cstr);
	if (!cstr.IsEmpty() && atoi(cstr) != 0)
	{
		vi.push_back(atoi(cstr));
	}
	uniq.push_back(vi);
	vi.clear();
	m_Edit_Uniq31.GetWindowText(cstr);
	if (!cstr.IsEmpty() && atoi(cstr) != 0)
	{
		vi.push_back(atoi(cstr));
	}
	m_Edit_Uniq32.GetWindowText(cstr);
	if (!cstr.IsEmpty() && atoi(cstr) != 0)
	{
		vi.push_back(atoi(cstr));
	}
	m_Edit_Uniq33.GetWindowText(cstr);
	if (!cstr.IsEmpty() && atoi(cstr) != 0)
	{
		vi.push_back(atoi(cstr));
	}
	m_Edit_Uniq34.GetWindowText(cstr);
	if (!cstr.IsEmpty() && atoi(cstr) != 0)
	{
		vi.push_back(atoi(cstr));
	}
	uniq.push_back(vi);
	//-----------------------------------------
	//AfxMessageBox("进行组合计算前，系统默认对面积进行了降序排序");
	for (int i = 0; i < totalRecords; i ++)
	{
		list_Inputed.SetItemData(i, i);
	}
	DATA data;
	data.subitem = 4;
	data.plist = &list_Inputed;
	fav = FALSE;

	list_Inputed.SortItems(listCompare, (LPARAM)&data);

	//将所有需求数量读到vector中
	vs_NeedNumber.clear();
	vLen.clear();
	vWid.clear();
	for (int i = 0; i < totalRecords; i ++)
	{
		vs_NeedNumber.push_back(atoi(list_Inputed.GetItemText(i, 5)));
		vLen.push_back(atoi(list_Inputed.GetItemText(i, 2)));
		vWid.push_back(atoi(list_Inputed.GetItemText(i, 3)));
	}
	vector<int> calvs = vs_NeedNumber;
	vector<int> tempvs = vs_NeedNumber;
	//1: 取得所有数目为10，9...的组合
	//1.1:将所有需求数量读到vector中，都对其进行操作，声明一个函数，传进vector，和 10...
	//vector定义成 public。定义一个处理函数,函数里处理兼容组合，算出所有10的组合，然后将vector中相应的设为0
	//然后继续函数(9)

	int num = 10; //default 10
	CString mcn;
	m_edit_maxcomnum.GetWindowText( mcn );
	if (!mcn.IsEmpty() && atoi(mcn) != 0)
	{
		num = atoi(mcn);
		maxChipNums = num;
	}
	repr.clear();
	result_finally.clear();
	//relt_General_Index = 0; ////sdfadfadf
	//构造一个新的映射矩阵
	for (int i = 0; i < calvs.size(); i ++)
	{
		repr.push_back(i);
	}
	/*vector<vector<int>> resultLocal, resultGlobal;
	mPro_General.SetRange(0, 7);
	mPro_General.SetStep(1);

	relt_Mapping.clear();
	while (num != 3)
	{
		resultLocal = CalculateGlobalCom(calvs, num);
		mPro_General.StepIt();
		resultLocal = UniqueResult(resultLocal);
		for(int i = 0; i < resultLocal.size(); i ++)
		{
			resultGlobal.push_back(resultLocal[i]);
			relt_Mapping.push_back(num);
		}
		num --;
		
	}
	relt = resultGlobal;*/
	//reverse(relt.begin(), relt.end());

	//-------------------------------------
	mPro_General.SetRange(0, 10000);
	mPro_General.SetPos(0);
	m_hThread_Cal  = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CalCombByNumb, (LPVOID)this->m_hWnd, NULL, &m_ThreadID_Cal);
	
	//if (!WaitForSingleObject(m_hThread_Cal, INFINITE))
	//{
	//	int sz = relt.size();

	//	if (relt.size() == 1) //如果只有一个组合，直接输出
	//	{
	//		CString str;
	//		for (int i = 0; i < relt[0].size(); i ++)
	//		{
	//			relt[0][i] = repr[relt[0][i]];
	//			tempvs[relt[0][i]] = 0;
	//			CString temp = list_Inputed.GetItemText(relt[0][i], 1);
	//			relt[0][i] = atoi(temp);
	//			str += temp;
	//			if (i != relt[0].size() - 1)
	//			{
	//				str += ", ";
	//			}
	//		}
	//		result_finally.push_back(relt[0]);
	//		int nItem = list_Result_General.InsertItem( relt_General_Index, "");
	//		list_Result_General.SetItemText(nItem, 0, str);
	//		CString st;
	//		st.Format("%d", num);
	//		list_Result_General.SetItemText(nItem, 1, st);
	//		relt_General_Index ++;
	//	}
	//	else
	//	{
	//		mPro_Adjust.SetRange(0, sz - 1);
	//		mPro_Adjust.SetStep(1);
	//		vector<vector<int>> resultM(sz,sz); //以矩阵的形式表达是否相等，不相等则为1
	//		for (int i = 0; i < sz - 1; i ++)
	//		{
	//			resultM[i][i] = 1;
	//			for (int j = i + 1; j < sz; j ++)
	//			{
	//				if (!SearchCommon(relt[i], relt[j]))
	//				{
	//					resultM[i][j] = 1;
	//					resultM[j][i] = 1;
	//				}
	//			}
	//			mPro_Adjust.StepIt();
	//		}
	//		reltM = resultM;
	//		m_hThread  = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GetMinCombs, (LPVOID)this->m_hWnd, NULL, &m_ThreadID);
	//		::InitializeCriticalSection(&cs);
	//	}		
	//}
	
	//---------------------------
}

vector<vector<int>> CGlassCompositionDlg::CalculateCom(vector<int> vs, int m)
{
	vector<vector<int>> vv;
	int s = CaculateNeedSum(vs);
	if (s < m || s < 1 || m < 1)
	{
		return vv;
	}

	int groupNum = 0; //分组数
	int tr = vs.size();
	if (tr % 8 == 0)
	{
		groupNum = tr / 8;
	}
	else
	{
		groupNum = tr / 8 + 1;
	}
	//计算单个group的组合可能
	for (int i = 0; i < groupNum; i ++)
	{
		vv = CalculateGroupCom(i, i, m, 0, vs, vv);
	}

	if (tr <= 16)
	{
		vector<int> v_Need = arrayShift(vs, 0, tr);
		for (int i = 0; i < groupNum - 1; i ++) //组编号
		{
			for (int j = i + 1; j < groupNum; j ++)
			{
				vv = CalculateGroupCom(i, j, m, 0, v_Need, vv);
			}
		}
	}
	else
	{
		vector<int> v_Need = arrayShift(vs, 0, tr);

		for (int i = 0; i < groupNum - 1; i ++) //组编号
		{
			for (int j = i + 1; j < groupNum; j ++)
			{
				vv = CalculateGroupCom(i, j, m, 0, v_Need, vv);
			}
		}
		//mPro_General.StepIt();
		for (int k = 1; k < 8; k ++)
		{
			v_Need = arrayShift(vs, k, tr);
			for (int i = 0; i < groupNum - 1; i ++) //组编号
			{
				for (int j = i + 1; j < groupNum; j ++)
				{
					vv = CalculateGroupCom(i, j, m, k, v_Need, vv);
				}
			}
			//mPro_General.StepIt();
		}		
	}
	return vv;
}
/************************************************************************/
/* 组合判断，分组之后运算                                                                     */
/************************************************************************/
vector<vector<int>> CGlassCompositionDlg::CalculateGroupCom(int master, int customer, int sum, int shiftTimes, vector<int> vNeed, vector<vector<int>> resultCom)
{
	int num = 0;  //枚举次数
	int groupNum = 0;
	int lgth = vNeed.size();
	if (lgth % 8 == 0)
	{
		groupNum = lgth / 8;
	}
	else
	{
		groupNum = lgth / 8 + 1;
	}

	if (master == customer) //计算单个组
	{
		if (customer == groupNum - 1)
		{
			num = 1 << (lgth - customer * 8);
		}
		else
		{
			num = 1 << 8;
		}

		for(int i = 0; i < num; i ++)  //枚举所有情况
		{
			int s = 0;
			int j, k;
			for(j = i, k = 0; j != 0; j>>=1, k++) //针对每种情况求和，判断是否满足条件
			{
				int w = 0;
				w = master * 8 + k;
				if(j&1)
					s += vNeed[w];
			}
			//CString str;
			if(s == sum) //如果满足，打印结果
			{
				vector<int> v;
				int len,wid;
				bool flag = TRUE;
				for(j = i, k = 1; j != 0; j>>=1, k++)
				{
					if(j&1)
					{				
						int w = 0;
						w = master * 8 + (k - 1);
						v.push_back(w);
					}
				}
				if (!JudgeUniq(v))
				{
					flag = false;
				}
				//vector<int> uq;
				//for (int x = 0; x < v.size(); x ++)
				//{
				//	uq.push_back(FindUniq(v[x]));
				//}
				////vector<int> uq_uq;
				//vector<int>::iterator iter;
				//sort(uq.begin(), uq.end());
				//iter = unique(uq.begin(), uq.end());
				//if (iter != uq.end())
				//{
				//	uq.erase(iter, uq.end());
				//}
				//int us = uq.size();
				//if (us > 2)
				//{
				//	break;
				//}
				//else if (us == 1)
				//{
				//	if (uq[0] != 0)
				//	{
				//		if (uniq[uq[0]].size() != v.size())
				//		{
				//			break;
				//		}
				//	}
				//}
				//else if (us == 2)
				//{
				//	if (uq[0] != 0 || uq[1] != 0)
				//	{
				//		break;
				//	}
				//}

				/*if (shiftTimes != 0)
				{
				for (int e = 0; e < v.size(); e ++)
				{
				v[e] = (v[e] + shiftTimes) % totalRecords;
				}
				sort(v.begin(), v.end());
				}*/	
				if (flag)
				{
					len = atoi(list_Inputed.GetItemText(repr[v[0]], 2));
					wid = atoi(list_Inputed.GetItemText(repr[v[0]], 3));
					for (int h = 1; h < v.size(); h ++)
					{
						int nlen = atoi(list_Inputed.GetItemText(repr[v[h]], 2));
						int nwid = atoi(list_Inputed.GetItemText(repr[v[h]], 3));
						if (nlen <= len && nwid <= wid)
						{
							len = atoi(list_Inputed.GetItemText(repr[v[h]], 2));
							wid = atoi(list_Inputed.GetItemText(repr[v[h]], 3));						
						}
						else
						{
							flag = false;
							break;
						}
					}
					if (flag)
					{						
						resultCom.push_back(v);
					}
				}				
			}
		}
	}
	else //计算两两组
	{
		if (shiftTimes == 0) //如果没有循环移位
		{
			if (customer == groupNum - 1)
			{
				num = 1 << (8 + lgth - customer * 8);
			}
			else
			{
				num = 1 << 16;
			}

			for(int i = (1 << 8); i < num; i ++)  //枚举所有情况
			{
				int t = i & 0x00ff;
				if(t == 0)
				{
					continue;
				}

				int s = 0;
				int j, k;
				for(j = i, k = 0; j != 0; j>>=1, k++) //针对每种情况求和，判断是否满足条件
				{
					int w = 0;
					if (k > 7)
					{
						w = customer * 8 + k - 8;
					}
					else
					{
						w = master * 8 + k;
					}

					if(j&1)
						s += vNeed[w];
				}
				//CString str;
				if(s == sum) //如果满足，打印结果
				{
					vector<int> v;
					int len,wid;
					bool flag = TRUE;
					for(j = i, k = 1; j != 0; j>>=1, k++)
					{
						if(j&1)
						{				
							int w = 0;
							if((k - 1) > 7)
							{
								w = customer * 8 + (k - 1) - 8;
							}
							else
							{
								w = master * 8 + (k - 1);
							}

							v.push_back(w);
						}
					}

					if (!JudgeUniq(v))
					{
						flag = false;
					}
					/*if (shiftTimes != 0)
					{
					for (int e = 0; e < v.size(); e ++)
					{
					v[e] = (v[e] + shiftTimes) % totalRecords;
					}
					sort(v.begin(), v.end());
					}	*/
					if (flag)
					{
						len = atoi(list_Inputed.GetItemText(repr[v[0]], 2));
						wid = atoi(list_Inputed.GetItemText(repr[v[0]], 3));
						for (int h = 1; h < v.size(); h ++)
						{
							int nlen = atoi(list_Inputed.GetItemText(repr[v[h]], 2));
							int nwid = atoi(list_Inputed.GetItemText(repr[v[h]], 3));
							if (nlen <= len && nwid <= wid)
							{
								len = atoi(list_Inputed.GetItemText(repr[v[h]], 2));
								wid = atoi(list_Inputed.GetItemText(repr[v[h]], 3));						
							}
							else
							{
								flag = false;
								break;
							}
						}
						if (flag)
						{						
							resultCom.push_back(v);
						}
					}					
				}
			}
		}
		else //如果已经循环移位
		{
			if (customer == groupNum - 1)
			{
				num = 1 << (8 + lgth - customer * 8);
			}
			else
			{
				num = 1 << 16;
			}		
			//算出掩码，排除情况
			vector<int> gComNum;
			if (customer == groupNum - 1)
			{
				for (int i = 0; i < 8; i ++)
				{
					gComNum.push_back(((master * 8 + i + shiftTimes) % lgth) / 8);
				}

				for (int j = customer * 8; j < lgth; j ++)
				{
					gComNum.push_back(((j + shiftTimes) % lgth) / 8);
				}
			} 
			else
			{
				for (int i = 0; i < 8; i ++)
				{
					gComNum.push_back(((master * 8 + i + shiftTimes) % lgth) / 8);
				}

				for (int j = 0; j < 8; j ++)
				{
					gComNum.push_back(((customer * 8 + j + shiftTimes) % lgth) / 8);
				}
			}
			set<int> tp;
			for (int i = 0; i < gComNum.size(); i ++)
			{
				tp.insert(gComNum[i]);
			}
			vector<int> tpm;
			//int a = tp.count(6);
			set<int>::iterator it = tp.begin();
			while(it != tp.end())
			{
				tpm.push_back(*it);
				it ++;
			}

			int msk = 0x0000;
			int msk1 = 0x0000;
			int msk2 = 0x0000;
			int msk3 = 0x0000;
			int msk4 = 0x0000;
			//count(gComNum.begin(), gComNum.end(), 1);
			//vector<int>::iterator ito ;
			int temp;
			int f = 0; //flag
			if (tpm.size() == 3)
			{
				f = 3;
				if (gComNum[0] != gComNum[gComNum.size() - 1])
				{
					msk = 0x0001;
				}
				else
				{
					if (customer == groupNum - 1)
					{
						msk = (1 << (lgth - 8 * (groupNum - 1))) + 0x0001;		
					}
					else
					{
						if (customer == master + 1)
						{
							msk = 0x0001;
						}
						else
						{
							msk = (1 << 8) + 1;
						}

					}
				}

				int k = 0;
				for (int i = gComNum.size() - 1; i >= 0; i --)
				{
					if (gComNum[i] == tpm[0])
					{
						msk1 += 1 << k;
					}
					else if(gComNum[i] == tpm[1])
					{ 
						msk2 += 1 << k;
					}
					else
					{
						msk3 += 1 << k;
					}
					k ++;
				}
			}
			else if (tpm.size() == 2)
			{
				f = 2;
				msk == 0x0000;
			}
			else
			{
				f = 4;
				msk = 0x0101;
				int k = 0;
				for (int i = gComNum.size() - 1; i >= 0; i --)
				{
					if (gComNum[i] == tpm[0])
					{
						msk1 += 1 << k;
					}
					else if(gComNum[i] == tpm[1])
					{ 
						msk2 += 1 << k;
					}
					else if(gComNum[i] == tpm[2])
					{
						msk3 += 1 << k;
					}
					else 
					{
						msk4 += 1 << k;
					}
					k ++;
				}
			}
			//---------------------------------------//
			for(int i = 0; i < num; i ++)  //枚举所有情况
			{
				int p = 0;
				if (f == 2)
				{
					p = i & msk;
					if (p == 0)
					{
						continue;
					}
				} 
				else if(f == 3)
				{
					p = i & msk;
					if (p == 0)
					{
						continue;
					}
					p = i & msk1;
					if (p == 0)
					{
						continue;
					}
					p = i & msk2;
					if (p == 0)
					{
						continue;
					}
					p = i & msk3;
					if (p == 0)
					{
						continue;
					}
				}
				else if(f == 4)
				{
					p = i & msk;
					if (p == 0)
					{
						continue;
					}
					p = i & (msk1 + msk2);
					if (p == 0)
					{
						continue;
					}
					p = i & (msk1 + msk3);
					if (p == 0)
					{
						continue;
					}
					p = i & (msk1 + msk4);
					if (p == 0)
					{
						continue;
					}
					p = i & (msk2 + msk3);
					if (p == 0)
					{
						continue;
					}
					p = i & (msk2 + msk4);
					if (p == 0)
					{
						continue;
					}
					p = i & (msk3 + msk4);
					if (p == 0)
					{
						continue;
					}
				}

				int s = 0;
				int j, k;
				for(j = i, k = 0; j != 0; j>>=1, k++) //针对每种情况求和，判断是否满足条件
				{
					int w = 0;
					if (customer != groupNum - 1)
					{
						if(k > 7)
						{
							w = master * 8 + (8 - (k - 7));									
						}
						else
						{
							w = customer * 8 + (7 - k);
						}
					} 
					else
					{
						int tmp = lgth - (groupNum - 1) * 8;
						if (k > (tmp - 1))
						{
							w = master * 8 + (8 - (k - (tmp - 1)));	
						} 
						else
						{
							w = customer * 8 + (tmp - 1 - k);
						}
					}

					if(j&1)
						s += vNeed[w];
				}
				//CString str;
				if(s == sum) //如果满足，打印结果
				{
					vector<int> v;
					int len,wid;
					bool flag = TRUE;
					for(j = i, k = 1; j != 0; j>>=1, k++)
					{
						if(j&1)
						{				
							int w = 0;
							if (customer != groupNum - 1)
							{
								if((k - 1) > 7)
								{
									w = master * 8 + (8 - ((k - 1) - 7));									
								}
								else
								{
									w = customer * 8 + (7 - (k - 1));
								}
							} 
							else
							{
								int tmp = lgth - (groupNum - 1) * 8;
								if ((k - 1) > (tmp - 1))
								{
									w = master * 8 + (8 - ((k - 1) - (tmp - 1)));	
								} 
								else
								{
									w = customer * 8 + (tmp - 1 - (k - 1));
								}
							}
							v.push_back(w);
						}
					}

					if (shiftTimes != 0)
					{
						for (int e = 0; e < v.size(); e ++)
						{
							v[e] = (v[e] + shiftTimes) % lgth;
						}
						sort(v.begin(), v.end());
					}
					if (!JudgeUniq(v))
					{
						flag = false;
					}
					if (flag)
					{
						len = atoi(list_Inputed.GetItemText(repr[v[0]], 2));
						wid = atoi(list_Inputed.GetItemText(repr[v[0]], 3));
						for (int h = 1; h < v.size(); h ++)
						{
							int nlen = atoi(list_Inputed.GetItemText(repr[v[h]], 2));
							int nwid = atoi(list_Inputed.GetItemText(repr[v[h]], 3));
							if (nlen <= len && nwid <= wid)
							{
								len = atoi(list_Inputed.GetItemText(repr[v[h]], 2));
								wid = atoi(list_Inputed.GetItemText(repr[v[h]], 3));						
							}
							else
							{
								flag = false;
								break;
							}
						}
						if (flag)
						{						
							resultCom.push_back(v);
						}
					}									
				}
			}
		}
	}
	return resultCom;
}


int CGlassCompositionDlg::CaculateNeedSum(vector<int> v)
{
	//int max_Num = 0;
	int sum = 0;
	//遍历vector
	for (vector<int>::iterator iter = v.begin(); iter != v.end(); ++iter)
	{
		/*if (*iter >= max_Num)
		{
		max_Num = *iter;
		}*/
		sum += *iter;
	}
	return sum;
}
/************************************************************************/
/* 如果存在相同的元素的话，则返回true，否则为false                                                                     */
/************************************************************************/
bool CGlassCompositionDlg::SearchCommon(vector<int> a, vector<int> b)
{
	int len_a = a.size();
	int len_b = b.size();
	int i = 0, j = 0;
	while(i < len_a && j < len_b)
	{
		if (a[i] == b[j])
		{
			return true;
		}
		if (a[i] > b[j])
		{
			j ++;
		}
		if (j < len_b && a[i] < b[j])
		{
			i ++;
		}
	}
	return false;
}

vector<int> CGlassCompositionDlg::reverse(vector<int> ar, int left, int right)
{
	int tmp;
	for(int i=left,j=right;i<j;i++,j--)
	{
		tmp=ar[i];
		ar[i]=ar[j];
		ar[j]=tmp;
	}
	return ar;
}


vector<int> CGlassCompositionDlg::arrayShift(vector<int> arr, int m, int len)
{
	arr = reverse(arr,0,m-1);
	arr = reverse(arr,m,len-1);
	arr = reverse(arr,0,len-1);

	return arr;
}



vector<int> CGlassCompositionDlg::GetReResult(vector<vector<int>> mtx, vector<int> sumSeq)
{
	vector<int> rgn = sumSeq;
	vector<int>::iterator iter;
	sort(rgn.begin(), rgn.end());
	iter = unique(rgn.begin(), rgn.end());

	if (iter != rgn.end())
	{
		rgn.erase(iter, rgn.end());
	}

	int pos = (int)(max_element(sumSeq.begin(), sumSeq.end()) - sumSeq.begin());
	int mxn = *max_element(sumSeq.begin(), sumSeq.end());

	vector<int> line = mtx[pos];

	for (int i = rgn.size() - 1; i >= 0; i --)
	{
		int val = rgn[i];
		//int cout = count(sumSeq.begin(), sumSeq.end(), val);
		vector<int> temp;
		for (int j = 0; j < sumSeq.size(); j ++)
		{
			if (sumSeq[j] == val && j != pos)
			{
				temp.push_back(j);
			}
		}
		/*if (temp.size() == 1)
		{
		continue;
		}*/
		for (int k = 0; k < temp.size(); k ++)
		{
			int t = temp[k];
			if (line[t] == 1)
			{
				for(int h = 0; h < mtx.size(); h ++)
				{
					if (mtx[t][h] == 0 && h != t)
					{
						if (line[h] == 1)
						{
							line[h] = 0;
						}
					}
				}
			}
		}
	}
	line[pos] = 1;
	return line;
}


vector<vector<int>> CGlassCompositionDlg::UniqueResult(vector<vector<int>> relt)
{
	vector<vector<int>> vv = relt;
	vector<vector<int>>::iterator iter;
	sort(vv.begin(), vv.end());

	iter = unique(vv.begin(), vv.end());

	if (iter != vv.end())
	{
		vv.erase(iter, vv.end());
	}
	return vv;
}

vector<vector<int>> UniqueResultCal(vector<vector<int>> relt)
{
	vector<vector<int>> vv = relt;
	vector<vector<int>>::iterator iter;
	sort(vv.begin(), vv.end());

	iter = unique(vv.begin(), vv.end());

	if (iter != vv.end())
	{
		vv.erase(iter, vv.end());
	}
	return vv;
}


vector<vector<int>> CGlassCompositionDlg::CalculateComAdjust(vector<int> vs, int m)
{
	vector<vector<int>> vv;
	int s = CaculateNeedSum(vs);
	if (s < m || s < 1 || m < 1)
	{
		return vv;
	}

	int groupNum = 0; //分组数
	int tr = vs.size();
	if (tr % 8 == 0)
	{
		groupNum = tr / 8;
	}
	else
	{
		groupNum = tr / 8 + 1;
	}
	//计算单个group的组合可能
	for (int i = 0; i < groupNum; i ++)
	{
		vv = CalculateGroupComAdjust(i, i, m, 0, vs, vv);
	}

	if (tr <= 16)
	{
		vector<int> v_Need = arrayShift(vs, 0, tr);
		for (int i = 0; i < groupNum - 1; i ++) //组编号
		{
			for (int j = i + 1; j < groupNum; j ++)
			{
				vv = CalculateGroupComAdjust(i, j, m, 0, v_Need, vv);
			}
		}
	}
	else
	{
		vector<int> v_Need = arrayShift(vs, 0, tr);

		for (int i = 0; i < groupNum - 1; i ++) //组编号
		{
			for (int j = i + 1; j < groupNum; j ++)
			{
				vv = CalculateGroupComAdjust(i, j, m, 0, v_Need, vv);
			}
		}
		mPro_Adjust.StepIt();
		for (int k = 1; k < 8; k ++)
		{
			v_Need = arrayShift(vs, k, tr);
			for (int i = 0; i < groupNum - 1; i ++) //组编号
			{
				for (int j = i + 1; j < groupNum; j ++)
				{
					vv = CalculateGroupComAdjust(i, j, m, k, v_Need, vv);
				}
			}
			mPro_Adjust.StepIt();
		}		
	}
	return vv;
}


vector<vector<int>> CGlassCompositionDlg::CalculateGroupComAdjust(int master, int customer, int sum, int shiftTimes, vector<int> vNeed, vector<vector<int>> resultCom)
{
	int num = 0;  //枚举次数
	int groupNum = 0;
	int lgth = vNeed.size();
	CString difstr;
	m_Difference.GetWindowText( difstr );
	int diff = atoi( difstr );
	if (lgth % 8 == 0)
	{
		groupNum = lgth / 8;
	}
	else
	{
		groupNum = lgth / 8 + 1;
	}

	if (master == customer) //计算单个组
	{
		if (customer == groupNum - 1)
		{
			num = 1 << (lgth - customer * 8);
		}
		else
		{
			num = 1 << 8;
		}

		for(int i = 0; i < num; i ++)  //枚举所有情况
		{
			int s = 0;
			int j, k;
			for(j = i, k = 0; j != 0; j>>=1, k++) //针对每种情况求和，判断是否满足条件
			{
				int w = 0;
				w = master * 8 + k;
				if(j&1)
					s += vNeed[w];
			}
			//CString str;
			if(s == sum) //如果满足，打印结果
			{
				vector<int> v;
				int len,wid;
				bool flag = true;
				for(j = i, k = 1; j != 0; j>>=1, k++)
				{
					if(j&1)
					{				
						int w = 0;
						w = master * 8 + (k - 1);

						v.push_back(w);
					}
				}
				if (!JudgeUniq(v))
				{
					flag = false;
				}
				/*if (shiftTimes != 0)
				{
				for (int e = 0; e < v.size(); e ++)
				{
				v[e] = (v[e] + shiftTimes) % totalRecords;
				}
				sort(v.begin(), v.end());
				}*/	
				if (flag)
				{
					len = atoi(list_Inputed.GetItemText(repr[v[0]], 2));
					wid = atoi(list_Inputed.GetItemText(repr[v[0]], 3));
					for (int h = 1; h < v.size(); h ++)
					{
						/*if (atoi(list_Inputed.GetItemText(repr[v[h]], 2)) > len || atoi(list_Inputed.GetItemText(repr[v[h]], 3)) > wid)
						{
						flag = false;
						break;
						}
						else
						{
						len = atoi(list_Inputed.GetItemText(repr[v[h]], 2));
						wid = atoi(list_Inputed.GetItemText(repr[v[h]], 3));

						}*/
						int nlen = atoi(list_Inputed.GetItemText(repr[v[h]], 2));
						int nwid = atoi(list_Inputed.GetItemText(repr[v[h]], 3));
						if (nlen <= len && nwid <= wid)
						{
							len = nlen;
							wid = nwid;
						}
						else if ((len - nlen) > diff)
						{
							if ((nwid - wid) < diff)
							{
								len = nlen;
								wid = nwid;
							}
							else
							{
								flag = false;
								break;
							}
						}
						else if ((wid - nwid) > diff)
						{
							if (nlen - len < diff)
							{
								len = nlen;
								wid = nwid;
							}
							else
							{
								flag = false;
								break;
							}
						}
						else
						{
							flag = false;
							break;
						}
					}
					if (flag)
					{						
						resultCom.push_back(v);
					}
				}

			}
		}
	}
	else //计算两两组
	{
		if (shiftTimes == 0) //如果没有循环移位
		{
			if (customer == groupNum - 1)
			{
				num = 1 << (8 + lgth - customer * 8);
			}
			else
			{
				num = 1 << 16;
			}

			for(int i = (1 << 8); i < num; i ++)  //枚举所有情况
			{
				int t = i & 0x00ff;
				if(t == 0)
				{
					continue;
				}

				int s = 0;
				int j, k;
				for(j = i, k = 0; j != 0; j>>=1, k++) //针对每种情况求和，判断是否满足条件
				{
					int w = 0;
					if (k > 7)
					{
						w = customer * 8 + k - 8;
					}
					else
					{
						w = master * 8 + k;
					}

					if(j&1)
						s += vNeed[w];
				}
				//CString str;
				if(s == sum) //如果满足，打印结果
				{
					vector<int> v;
					int len,wid;
					bool flag = TRUE;
					for(j = i, k = 1; j != 0; j>>=1, k++)
					{
						if(j&1)
						{				
							int w = 0;
							if((k - 1) > 7)
							{
								w = customer * 8 + (k - 1) - 8;
							}
							else
							{
								w = master * 8 + (k - 1);
							}

							v.push_back(w);
						}
					}
					if (!JudgeUniq(v))
					{
						flag = false;
					}
					/*if (shiftTimes != 0)
					{
					for (int e = 0; e < v.size(); e ++)
					{
					v[e] = (v[e] + shiftTimes) % totalRecords;
					}
					sort(v.begin(), v.end());
					}	*/
					if (flag)
					{
						len = atoi(list_Inputed.GetItemText(repr[v[0]], 2));
						wid = atoi(list_Inputed.GetItemText(repr[v[0]], 3));
						for (int h = 1; h < v.size(); h ++)
						{
							int nlen = atoi(list_Inputed.GetItemText(repr[v[h]], 2));
							int nwid = atoi(list_Inputed.GetItemText(repr[v[h]], 3));
							if (nlen <= len && nwid <= wid)
							{
								len = nlen;
								wid = nwid;
							}
							else if ((len - nlen) > diff)
							{
								if ((nwid - wid) < diff)
								{
									len = nlen;
									wid = nwid;
								}
								else
								{
									flag = false;
									break;
								}
							}
							else if ((wid - nwid) > diff)
							{
								if (nlen - len < diff)
								{
									len = nlen;
									wid = nwid;
								}
								else
								{
									flag = false;
									break;
								}
							}
							else
							{
								flag = false;
								break;
							}
						}
						if (flag)
						{						
							resultCom.push_back(v);
						}
					}					
				}
			}
		}
		else //如果已经循环移位
		{
			if (customer == groupNum - 1)
			{
				num = 1 << (8 + lgth - customer * 8);
			}
			else
			{
				num = 1 << 16;
			}		
			//算出掩码，排除情况
			vector<int> gComNum;
			if (customer == groupNum - 1)
			{
				for (int i = 0; i < 8; i ++)
				{
					gComNum.push_back(((master * 8 + i + shiftTimes) % lgth) / 8);
				}

				for (int j = customer * 8; j < lgth; j ++)
				{
					gComNum.push_back(((j + shiftTimes) % lgth) / 8);
				}
			} 
			else
			{
				for (int i = 0; i < 8; i ++)
				{
					gComNum.push_back(((master * 8 + i + shiftTimes) % lgth) / 8);
				}

				for (int j = 0; j < 8; j ++)
				{
					gComNum.push_back(((customer * 8 + j + shiftTimes) % lgth) / 8);
				}
			}
			set<int> tp;
			for (int i = 0; i < gComNum.size(); i ++)
			{
				tp.insert(gComNum[i]);
			}
			vector<int> tpm;
			//int a = tp.count(6);
			set<int>::iterator it = tp.begin();
			while(it != tp.end())
			{
				tpm.push_back(*it);
				it ++;
			}

			int msk = 0x0000;
			int msk1 = 0x0000;
			int msk2 = 0x0000;
			int msk3 = 0x0000;
			int msk4 = 0x0000;
			//count(gComNum.begin(), gComNum.end(), 1);
			//vector<int>::iterator ito ;
			int temp;
			int f = 0; //flag
			if (tpm.size() == 3)
			{
				f = 3;
				if (gComNum[0] != gComNum[gComNum.size() - 1])
				{
					msk = 0x0001;
				}
				else
				{
					if (customer == groupNum - 1)
					{
						msk = (1 << (lgth - 8 * (groupNum - 1))) + 0x0001;		
					}
					else
					{
						if (customer == master + 1)
						{
							msk = 0x0001;
						}
						else
						{
							msk = (1 << 8) + 1;
						}

					}
				}

				int k = 0;
				for (int i = gComNum.size() - 1; i >= 0; i --)
				{
					if (gComNum[i] == tpm[0])
					{
						msk1 += 1 << k;
					}
					else if(gComNum[i] == tpm[1])
					{ 
						msk2 += 1 << k;
					}
					else
					{
						msk3 += 1 << k;
					}
					k ++;
				}
			}
			else if (tpm.size() == 2)
			{
				f = 2;
				msk == 0x0000;
			}
			else
			{
				f = 4;
				msk = 0x0101;
				int k = 0;
				for (int i = gComNum.size() - 1; i >= 0; i --)
				{
					if (gComNum[i] == tpm[0])
					{
						msk1 += 1 << k;
					}
					else if(gComNum[i] == tpm[1])
					{ 
						msk2 += 1 << k;
					}
					else if(gComNum[i] == tpm[2])
					{
						msk3 += 1 << k;
					}
					else 
					{
						msk4 += 1 << k;
					}
					k ++;
				}
			}
			//---------------------------------------//
			for(int i = 0; i < num; i ++)  //枚举所有情况
			{
				int p = 0;
				if (f == 2)
				{
					p = i & msk;
					if (p == 0)
					{
						continue;
					}
				} 
				else if(f == 3)
				{
					p = i & msk;
					if (p == 0)
					{
						continue;
					}
					p = i & msk1;
					if (p == 0)
					{
						continue;
					}
					p = i & msk2;
					if (p == 0)
					{
						continue;
					}
					p = i & msk3;
					if (p == 0)
					{
						continue;
					}
				}
				else if(f == 4)
				{
					p = i & msk;
					if (p == 0)
					{
						continue;
					}
					p = i & (msk1 + msk2);
					if (p == 0)
					{
						continue;
					}
					p = i & (msk1 + msk3);
					if (p == 0)
					{
						continue;
					}
					p = i & (msk1 + msk4);
					if (p == 0)
					{
						continue;
					}
					p = i & (msk2 + msk3);
					if (p == 0)
					{
						continue;
					}
					p = i & (msk2 + msk4);
					if (p == 0)
					{
						continue;
					}
					p = i & (msk3 + msk4);
					if (p == 0)
					{
						continue;
					}
				}

				int s = 0;
				int j, k;
				for(j = i, k = 0; j != 0; j>>=1, k++) //针对每种情况求和，判断是否满足条件
				{
					int w = 0;
					if (customer != groupNum - 1)
					{
						if(k > 7)
						{
							w = master * 8 + (8 - (k - 7));									
						}
						else
						{
							w = customer * 8 + (7 - k);
						}
					} 
					else
					{
						int tmp = lgth - (groupNum - 1) * 8;
						if (k > (tmp - 1))
						{
							w = master * 8 + (8 - (k - (tmp - 1)));	
						} 
						else
						{
							w = customer * 8 + (tmp - 1 - k);
						}
					}

					if(j&1)
						s += vNeed[w];
				}
				//CString str;
				if(s == sum) //如果满足，打印结果
				{
					vector<int> v;
					int len,wid;
					bool flag = TRUE;
					for(j = i, k = 1; j != 0; j>>=1, k++)
					{
						if(j&1)
						{				
							int w = 0;
							if (customer != groupNum - 1)
							{
								if((k - 1) > 7)
								{
									w = master * 8 + (8 - ((k - 1) - 7));									
								}
								else
								{
									w = customer * 8 + (7 - (k - 1));
								}
							} 
							else
							{
								int tmp = lgth - (groupNum - 1) * 8;
								if ((k - 1) > (tmp - 1))
								{
									w = master * 8 + (8 - ((k - 1) - (tmp - 1)));	
								} 
								else
								{
									w = customer * 8 + (tmp - 1 - (k - 1));
								}
							}
							v.push_back(w);
						}
					}
					if (shiftTimes != 0)
					{
						for (int e = 0; e < v.size(); e ++)
						{
							v[e] = (v[e] + shiftTimes) % lgth;
						}
						sort(v.begin(), v.end());
					}
					if (!JudgeUniq(v))
					{
						flag = false;
					}
					if (flag)
					{							
						len = atoi(list_Inputed.GetItemText(repr[v[0]], 2));
						wid = atoi(list_Inputed.GetItemText(repr[v[0]], 3));
						for (int h = 1; h < v.size(); h ++)
						{
							int nlen = atoi(list_Inputed.GetItemText(repr[v[h]], 2));
							int nwid = atoi(list_Inputed.GetItemText(repr[v[h]], 3));
							if (nlen <= len && nwid <= wid)
							{
								len = nlen;
								wid = nwid;
							}
							else if ((len - nlen) > diff)
							{
								if ((nwid - wid) < diff)
								{
									len = nlen;
									wid = nwid;
								}
								else
								{
									flag = false;
									break;
								}
							}
							else if ((wid - nwid) > diff)
							{
								if (nlen - len < diff)
								{
									len = nlen;
									wid = nwid;
								}
								else
								{
									flag = false;
									break;
								}
							}
							else
							{
								flag = false;
								break;
							}
						}
						if (flag)
						{						
							resultCom.push_back(v);
						}
					}					
				}
			}
		}
	}
	return resultCom;
}


void CGlassCompositionDlg::OnLvnItemchangedListInputed(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}


int CGlassCompositionDlg::FindUniq(int val)
{
	//vector<int>::iterator it = find(uniq[0].begin(), uniq[0].end(), val); 
	//if (it != NULL)
	//{
	//}
	////for (vector<int>::iterator iter = uniq.begin(); iter != uniq.end(); ++iter)
	////{
	////	/*if (*iter >= max_Num)
	////	{
	////		max_Num = *iter;
	////	}*/
	////	sum += *iter;
	////}
	//if ()
	//{
	//}
	for (vector<int>::iterator iter = uniq[0].begin(); iter != uniq[0].end(); ++iter)
	{
		if (*iter == val)
		{
			return 1;
		}
	}
	for (vector<int>::iterator iter = uniq[1].begin(); iter != uniq[1].end(); ++iter)
	{
		if (*iter == val)
		{
			return 2;
		}
	}
	for (vector<int>::iterator iter = uniq[2].begin(); iter != uniq[2].end(); ++iter)
	{
		if (*iter == val)
		{
			return 3;
		}
	}
	return 0;
}


bool CGlassCompositionDlg::JudgeUniq(vector<int> v)
{
	vector<int> uq;	
	for (int x = 0; x < v.size(); x ++)
	{
		int y = atoi(list_Inputed.GetItemText(repr[v[x]], 1));
		uq.push_back(FindUniq(y));
	}
	vector<int> uq_or = uq;
	vector<int>::iterator iter;
	sort(uq.begin(), uq.end());
	iter = unique(uq.begin(), uq.end());
	if (iter != uq.end())
	{
		uq.erase(iter, uq.end());
	}
	int us = uq.size();
	if (us > 2)
	{
		return false;
	}
	else if (us == 1)
	{
		if (uq[0] != 0)
		{
			if (uniq[uq[0] - 1].size() != v.size())
			{
				return false;
			}
		}
	}
	else if (us == 2)
	{
		if (uq[0] != 0 && uq[1] != 0)
		{
			return false;
		}
		else
		{
			int cout = 0;
			for (int z = 0; z < uq_or.size(); z ++)
			{
				if (uq_or[z] == uq[1])
				{
					cout ++;
				}
			}
			if (cout != uniq[uq[1] - 1].size())
			{
				return false;
			}
		}
	}
	return true;	
}


bool CGlassCompositionDlg::JudgeSameID(vector<int> v)
{	
	vector<int>::iterator iter;
	sort(v.begin(), v.end());

	iter = unique(v.begin(), v.end());
	if (iter == v.end())
	{
		return true;
	}

	return false;
}


bool CGlassCompositionDlg::JudgeSameValue(vector<vector<int>> vv)
{
	/*vector<vector<int>> vv = relt;
	vector<vector<int>>::iterator iter;
	sort(vv.begin(), vv.end());

	iter = unique(vv.begin(), vv.end());

	if (iter != vv.end())
	{
		vv.erase(iter, vv.end());
	}
	return vv;*/
	vector<vector<int>>::iterator iter;
	sort(vv.begin(), vv.end());
	iter = unique(vv.begin(), vv.end());

	if (iter == vv.end())
	{
		return true;
	}
	return false;
}


void CGlassCompositionDlg::OnBnClickedCancel()
{
	if (list_Inputed.GetItemCount() != 0)
	{
		int nSelect = AfxMessageBox("是否保存数据列表中记录,'确定':保存,'取消':不保存", MB_OKCANCEL);
		if(nSelect == IDOK) 
		{ 
			//   选择了OK 
			//保存文件对话框：
			CFileDialog dlg(
				FALSE, _T("bmp"), NULL,
				OFN_EXTENSIONDIFFERENT|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_HIDEREADONLY,
				_T("excel (*.xls) |*.xls"), NULL
				);
			// 打开文件对话框的标题名
			dlg.m_ofn.lpstrTitle = _T("Save Excel");
			// 判断是否获得图片
			if( dlg.DoModal() != IDOK )	
				return;

			CString str = dlg.GetPathName();
			//检查是否有同名文件
			if(PathFileExists(str))
			{
				int nSelect = AfxMessageBox("您将覆盖文件" + str, MB_OKCANCEL);
				if(nSelect == IDOK) 
				{ 
					//   选择了OK 
					DeleteFile(str);
					WriteToExcel(str);
				} 
				if(nSelect == IDCANCEL) 
				{ 
					//   选择了Cancel 
				}
			}
			else
			{
				WriteToExcel(str);
			}
		} 
		if(nSelect == IDCANCEL) 
		{ 
			//   选择了Cancel 
			::DeleteCriticalSection(&cs);
			CDialogEx::OnCancel();
		}
	}
	else
	{
		::DeleteCriticalSection(&cs);
		CDialogEx::OnCancel();	
	}	
}

void CGlassCompositionDlg::OnNMClickListResultAdjust(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	/*AfxMessageBox("AA");
	CString s;
	s.Format( "OnColumnclick   选择列%d ",pNMItemActivate-> iSubItem); 
	AfxMessageBox(s);
	s.Format( "OnColumnclick   选择列%d ",pNMItemActivate-> iItem); 
	AfxMessageBox(s);*/
	if (list_Show_Result.GetItemCount() != 0)
	{
		list_Show_Result.DeleteAllItems();
	}
	
	CString str_rel = list_Result_Adjust.GetItemText(pNMItemActivate-> iItem, 0);
	vector<int> v_click = CStringToIntArray(str_rel);
	vector<int>::iterator iter;
	for (int i = 0; i < v_click.size(); i ++)
	{
		int idx = v_click[i];
		iter = find(v_Id.begin(), v_Id.end(), idx);
		int nItem = iter - v_Id.begin();
		i = list_Show_Result.InsertItem( i, "");
		CString client = list_Inputed.GetItemText(nItem, 0);
		list_Show_Result.SetItemText(i, 0, client);
		CString idex = list_Inputed.GetItemText(nItem, 1);
		list_Show_Result.SetItemText(i, 1, idex);
		CString len = list_Inputed.GetItemText(nItem, 2);
		list_Show_Result.SetItemText(i, 2, len);
		CString wid = list_Inputed.GetItemText(nItem, 3);
		list_Show_Result.SetItemText(i, 3, wid);
		CString nnum = list_Inputed.GetItemText(nItem, 5);
		list_Show_Result.SetItemText(i, 4, nnum);
		CString areatt = list_Inputed.GetItemText(nItem, 6);
		list_Show_Result.SetItemText(i, 5, areatt);
		CString pictype = list_Inputed.GetItemText(nItem, 7);
		list_Show_Result.SetItemText(i, 6, pictype);
	}
	*pResult = 0;
}


void CGlassCompositionDlg::OnNMClickListResultGeneral(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	

	if (list_Show_Result.GetItemCount() != 0)
	{
		list_Show_Result.DeleteAllItems();
	}

	//CString str_rel = list_Result_General.GetItemText(pNMItemActivate-> iItem, 0);
	vector<int> v_click = OptRelt[pNMItemActivate-> iItem];/*CStringToIntArray(str_rel);*/
	//vector<int>::iterator iter;
	for (int i = 0; i < v_click.size(); i ++)
	{
		int nItem = v_click[i];
		//iter = find(v_Id.begin(), v_Id.end(), idx);
		//int nItem = iter - v_Id.begin();
		i = list_Show_Result.InsertItem( i, "");
		CString client = list_Inputed.GetItemText(nItem, 0);
		list_Show_Result.SetItemText(i, 0, client);
		CString idex = list_Inputed.GetItemText(nItem, 1);
		list_Show_Result.SetItemText(i, 1, idex);
		CString len = list_Inputed.GetItemText(nItem, 2);
		list_Show_Result.SetItemText(i, 2, len);
		CString wid = list_Inputed.GetItemText(nItem, 3);
		list_Show_Result.SetItemText(i, 3, wid);
		CString nnum = list_Inputed.GetItemText(nItem, 5);
		list_Show_Result.SetItemText(i, 4, nnum);
		CString areatt = list_Inputed.GetItemText(nItem, 6);
		list_Show_Result.SetItemText(i, 5, areatt);
		CString pictype = list_Inputed.GetItemText(nItem, 7);
		list_Show_Result.SetItemText(i, 6, pictype);
	}
	*pResult = 0;
}


vector<int> CGlassCompositionDlg::CStringToIntArray(CString str)
{
	vector<int> v;
	CString s1;
	int idx = str.FindOneOf(",");
	while(idx != -1)
	{
		s1 = str.Left(str.FindOneOf(","));
		v.push_back(atoi(s1));
		str = str.Mid(str.FindOneOf(",") + 2, str.GetLength());
		idx = str.FindOneOf(",");
	}
	v.push_back(atoi(str));
	return v;
}


void GetMinCombs(void* hWnd)
{
	int i = 0;
	for (i = 0; i < reltM.size(); i ++)
	{
		if (runTime != 0)
		{
			int np = 0;
			for (int j = 0; j < relt[i].size(); j ++)
			{
				np += vs_NeedNumber[relt[i][j]];
			}
			if (np != bestRecords[0])
			{
				if ((totalNeedNumberOfChips / np + 1) >= miniQlys)
				{
					vector<int> v;
					v.push_back(0);
					vector<int> *vvC = new vector<int>(v);
					::PostMessage((HWND) hWnd, MESSAGEFROMTHREAD_CURVEC, (WPARAM)vvC, (LPARAM) 0 );
					return;
				}
			}
		}
		vector<int> v = reltM[i];
		for (int j = 0; j < i; j ++)
		{
			v[j] = 0;
		}
		CalOptCombIter(hWnd, v, i);
	}
	int num = -1;
	::PostMessage((HWND) hWnd, MESSAGEFROMTHREAD, (WPARAM)num, (LPARAM) 0 );
}


void CalOptCombIter(void* hWnd, vector<int> cur, int curPos)
{

	int curSize = 0, curTotalNeedNumbers = 0;
	for (int m = 0; m <= curPos; m ++)
	{
		if (cur[m] == 1)
		{
			curSize ++;
			curTotalNeedNumbers += relt_Mapping[m];
		}
	}
	if (runTime != 0)
	{
		int curNeedCombs = 0, numtemp = 0;
		if (curPos != cur.size() - 1)
		{
			numtemp = relt_Mapping[curPos + 1];
			curNeedCombs = (totalNeedNumberOfChips - curTotalNeedNumbers)/numtemp;
			if ((totalNeedNumberOfChips - curTotalNeedNumbers)%numtemp !=0)
			{
				curNeedCombs ++;
			}
			if (curNeedCombs > (miniQlys - curSize))
			{
				return;
			}
		}
		if (curSize > miniQlys)
		{
			return;
		}
	}



	int pos = 0, k = 0;

	vector<int> v(cur.size()); 
	for (int pos = curPos + 1; pos < cur.size(); pos ++)
	{
		if (cur[pos] == 0)
		{
			continue;
		}
		for (k = 0; k < cur.size(); k ++)
		{
			v[k] = cur[k]&reltM[pos][k];  //更新vector
		}		
		CalOptCombIter(hWnd, v, pos);  
	}
	//CString str,stp;
	//stnum.Format("%d", miniNum);
	//m_Edit_MiniQlys.SetWindowText(stnum);
	vector<int> checkDulp, vCur;
	for (int i = 0; i < cur.size(); i ++)
	{
		if (cur[i] == 1)
		{
			vCur.push_back(i);
			for(int j = 0; j < relt[i].size(); j ++)
			{
				checkDulp.push_back(relt[i][j]);
			}
		}
	}
	
	vector<int>::iterator iter;
	sort(checkDulp.begin(), checkDulp.end());
	iter = unique(checkDulp.begin(), checkDulp.end());
	if (iter != checkDulp.end())
	{
		return;
	}

	showIndex ++;
	if (showIndex % 5000 == 0)
	{
		if (showIndex == 30000)
		{
			showIndex = 0;
		}
		vector<int> *vvC = new vector<int>(vCur);
		::PostMessage((HWND) hWnd, MESSAGEFROMTHREAD_CURVEC, (WPARAM)vvC, (LPARAM) 0 );
	}

	vector<vector<int>> vtemp;
	//CString ntp, numtp, str;
	int totalNum = 0, np = 0;
	curSize = 0;
	vector<int> combRcd;
	for (int i = 0; i < cur.size(); i ++)
	{
		if (cur[i] == 1)
		{
			curSize ++;
			totalNum += relt[i].size();
			vtemp.push_back(relt[i]);
			np = 0;
			for (int j = 0; j < relt[i].size(); j ++)
			{
				np += vs_NeedNumber[relt[i][j]];
			}
			combRcd.push_back(np);
		}
	}

	if (totalNum == vs_NeedNumber.size())
	{
		if (runTime == 0)
		{
			runTime ++;
		}
		
		THREADDATA *data = new THREADDATA;
		if (curSize <= miniQlys || miniQlys == 0)
		{
			if (curSize == miniQlys)
			{
				if (!betterThanBest(combRcd))
				{
					return;
				}
			}
			miniQlys = curSize;
			OptRelt = vtemp;
			bestRecords = combRcd;
			
			EnterCriticalSection(&cs);//进入临界区
			data->miniNum = curSize;
			data->curPos = curPos;
			LeaveCriticalSection(&cs);//离开临界区

			::PostMessage((HWND) hWnd, MESSAGEFROMTHREAD, (WPARAM)data, (LPARAM) 0 );
		}
	}
}

void CalCombByNumb(void* hWnd)
{
	vector<vector<int>> resultLocal, resultGlobal;
	int num = maxChipNums;
	relt_Mapping.clear();
	while (num != 2)
	{
		resultLocal = CalculateGlobalCom(hWnd, vs_NeedNumber, num);
		resultLocal = UniqueResultCal(resultLocal);
		for(int i = 0; i < resultLocal.size(); i ++)
		{
			resultGlobal.push_back(resultLocal[i]);
			relt_Mapping.push_back(num);
		}
		num --;
		
	}
	relt = resultGlobal;
	int rt = 0;
	::PostMessage((HWND) hWnd, MESSAGEFROMTHREAD_CALCOM_RET, (WPARAM)rt, (LPARAM) 0 );
	//CGlassCompositionDlg *pDlg = (CGlassCompositionDlg*)AfxGetMainWnd();
	//pDlg->OnBnClickedButtonCalcmbsort();
	//threadState = true;
}

bool betterThanBest(vector<int> lastR)
{

	int vaf = lastR[1];
	int vbf = bestRecords[1];
	
	if (vaf < vbf)
	{
		return false;
	}
	else if (vaf = vbf)
	{
		int i = 0, miniSize = 0;
		miniSize = lastR.size();
		while(lastR[i] == bestRecords[i])
		{
			i++;
			if (i == miniSize)
			{
				return false;
			}
		}
		if (bestRecords[i] > lastR[i])
		{
			return false;
		}
	}
	return true;
}

LRESULT CGlassCompositionDlg::OnMessageFromThread(WPARAM wParam, LPARAM lParam)
{
	//主线程从MESSAGEFROMTHREAD消息的参数中提取经过处理的数据
	THREADDATA *data = (THREADDATA*)wParam;
	int miniNum = data->miniNum;
	int curPos = data->curPos;
	if (miniNum == -1)
	{
		AfxMessageBox("End");
	}
	CString stnum;
	stnum.Format("%d", miniNum);
	m_Edit_MiniQlys.SetWindowText(stnum);
	CString stpos;
	stpos.Format("%d", curPos);
	m_Edit_CurPos.SetWindowText(stpos);

	delete data;

	list_Result_General.SetRedraw(FALSE);
	if (list_Result_General.GetItemCount() != 0)
	{
		list_Result_General.DeleteAllItems();	
	}		
	
	for (int i = 0; i < OptRelt.size(); i ++)
	{
		CString str, ntp, temp;
		int numtp = 0, np = 0;
		for (int j = 0; j < OptRelt[i].size(); j ++)
		{
			temp = list_Inputed.GetItemText(OptRelt[i][j], 1);
			ntp = list_Inputed.GetItemText(OptRelt[i][j], 5);
			np = atoi(ntp);
			numtp += np;
			str += temp;
			if (j != OptRelt[i].size() - 1)
			{
				str += ", ";
			}
		}					
		int nItem = list_Result_General.InsertItem( i, "");
		list_Result_General.SetItemText(nItem, 0, str);
		CString st;
		st.Format("%d", numtp);
		list_Result_General.SetItemText(nItem, 1, st);
	}
	list_Result_General.SetRedraw(TRUE);
	return 0;
}

LRESULT CGlassCompositionDlg::OnMessageFromThreadCalcom(WPARAM wParam, LPARAM lParam)
{
	//double percent = (double)wParam;
	int pos = (int)wParam;
	//mPro_General.SetRange(0, 1000);
	//mPro_General.SetPos(0);
	//mPro_General.SetRedraw(FALSE);
	mPro_General.SetPos(pos);
	//mPro_General.SetRedraw(TRUE);
	if (pos == 10000)
	{
		//calBestOptResult();
		//OnBnClickedButtonCalcmbsort();
		

		//AfxMessageBox("End!");
	}
	return 0;
}

LRESULT CGlassCompositionDlg::OnMessageFromThreadCalcomRET(WPARAM wParam, LPARAM lParam)
{
	int sz = relt.size();

	mPro_Adjust.SetRange(0, sz - 1);
	mPro_Adjust.SetStep(1);
	vector<vector<int>> resultM(sz,sz); //以矩阵的形式表达是否相等，不相等则为1
	for (int i = 0; i < sz - 1; i ++)
	{
		resultM[i][i] = 1;
		for (int j = i + 1; j < sz; j ++)
		{
			if (!SearchCommon(relt[i], relt[j]))
			{
				resultM[i][j] = 1;
				resultM[j][i] = 1;
			}
		}
		mPro_Adjust.StepIt();
	}
	reltM = resultM;
	m_hThread  = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GetMinCombs, (LPVOID)this->m_hWnd, NULL, &m_ThreadID);
	::InitializeCriticalSection(&cs);
	return 0;
}

LRESULT CGlassCompositionDlg::OnMessageFromThreadCurVec(WPARAM wParam, LPARAM lParam)
{
	vector<int> *vc = (vector<int> *)wParam;
	//CString s = *getStr;
	//delete getStr;
	vector<int> v = *vc;
	delete vc;

	if (v.size() == 1 && v[0] == 0)
	{
		AfxMessageBox("End!");
		return 0;
	}
	
	list_Result_Adjust.SetRedraw(FALSE);
	if (list_Result_Adjust.GetItemCount() != 0)
	{
		list_Result_Adjust.DeleteAllItems();	
	}
	int size_vShow = vShow.size();
	CString str, sCom, stp;					
	for (int i = 0; i < v.size(); i ++)
	{
		str.Format("%d", v[i]);		
		sCom = "";
		vector<int> vtp = relt[v[i]];
		for (int j = 0; j < vtp.size(); j ++)
		{
			//stp.Format("%d", list_Inputed.GetItemText(vtp[j], 1));
			stp = list_Inputed.GetItemText(vtp[j], 1);
			sCom += stp;
			sCom += " ,";
		}
		int index = sCom.GetLength() - 2;
		stp = sCom.Left(index);
		list_Result_Adjust.AddItem(_T(stp),_T(str));
		if ((size_vShow - 1) >= i && v[i] == vShow[i])
		{						
			list_Result_Adjust.SetItemColor(i, 1, RGB(255, 0, 0), RGB(255, 255, 255));
		}
		else
		{
			list_Result_Adjust.SetItemColor(i, 1, RGB(0, 255, 0), RGB(255, 255, 255));
		}
	}
	vShow = v;
	list_Result_Adjust.SetRedraw(TRUE);
	return 0;
}


vector<vector<int>> CalculateGlobalCom(void* hWnd, vector<int> needChips, int cobchpNum) //可对2^次方改进...
{
	int iter_num = 0;  //枚举次数
	int lgth = needChips.size();
	mpz_t m_num, i;
	mpz_init(m_num);
	mpz_init(i);
	mpz_ui_pow_ui(m_num,2,lgth);
	//iter_num = 1 << lgth;
	vector<vector<int>> vRelt;
	int showControlNum = 0;

	mpz_t kk;
	mpz_init(kk);
	mpf_t t, ti;
	mpf_init(t);
	mpf_init(ti);
	mpz_t j,l;
	mpz_init(j);
	mpz_init(l);
	for(mpz_set_ui(i, 0); mpz_cmp(i, m_num) < 0/*i < m_num*/; mpz_add_ui(i, i, 1)/*i ++*/)  //枚举所有情况
	{
		showControlNum ++;
		int pos = 0;
		
		mpz_sub_ui(kk, m_num, 1);
		if (mpz_cmp(i, kk) == 0/*i == iter_num - 1*/ && cobchpNum == 3)
		{
			pos = 10000;
			showControlNum = 0;
			::PostMessage((HWND) hWnd, MESSAGEFROMTHREAD_CALCOM, (WPARAM)pos, (LPARAM) 0 );
		}
		if (showControlNum == 5000)
		{
			
			mpf_set_z(t, m_num);
			mpf_set_z(ti, i);
			mpf_mul_ui(t, t, maxChipNums - 2);
			mpf_div(t, ti, t);
			//double d = (double)(iter_num*(maxChipNums - 2));
			//double a = (double)i/d;
			double a = mpf_get_d(t);
			double b = (double)(maxChipNums - cobchpNum)/(double)(maxChipNums - 2);
			double retData = a + b;
			pos = retData*10000;
			showControlNum = 0;
			::PostMessage((HWND) hWnd, MESSAGEFROMTHREAD_CALCOM, (WPARAM)pos, (LPARAM) 0 );
		}
		int s = 0;
		int k = 0;
		
		//mpz_init(k);
		for(mpz_set(j, i), k = 0; mpz_cmp_ui(j, 0) != 0; mpz_div_ui(j, j, 2), k++/*j = i, k = 0; j != 0; j>>=1, k++*/) //针对每种情况求和，判断是否满足条件
		{
			//int w = 0;
			mpz_set_ui(l, 1);
			mpz_and(l, j, l);
			if(mpz_cmp_ui(l, 0) != 0/*j&1*/)
				s += needChips[k];
		}
		//CString str;
		if(s == cobchpNum) //如果满足，打印结果
		{
			vector<int> v;
			int len,wid;
			bool flag = TRUE;
			for(mpz_set(j, i), k = 0; mpz_cmp_ui(j, 0) != 0; mpz_div_ui(j, j, 2), k++/*j = i, k = 0; j != 0; j>>=1, k++*/)
			{
				mpz_set_ui(l, 1);
				mpz_and(l, j, l);
				if(mpz_cmp_ui(l, 0) != 0/*j&1*/)
				{				
					//int w = 0;
					//w = master * 8 + (k - 1);
					v.push_back(k);
				}
			}
			/*if (!JudgeUniq(v))
			{
			flag = false;
			}*/
			if (flag)
			{
				len = vLen[v[0]];
				wid = vWid[v[0]];
				for (int h = 1; h < v.size(); h ++)
				{
					int nlen = vLen[v[h]];
					int nwid = vWid[v[h]];
					if (nlen <= len && nwid <= wid)
					{
						len = nlen;
						wid = nwid;		
					}
					else
					{
						flag = false;
						break;
					}
				}
				if (flag)
				{						
					vRelt.push_back(v);
				}
			}				
		}
	}
	mpz_clear(m_num);
	mpz_clear(i);
	mpz_clear(kk);
	mpf_clear(t);
	mpf_clear(ti);
	mpz_clear(j);
	mpz_clear(l);
	return vRelt;


	//int iter_num = 0;  //枚举次数
	//int lgth = needChips.size();
	//iter_num = 1 << lgth;
	//vector<vector<int>> vRelt;
	//int showControlNum = 0;

	//mpf_t t, ti;
	//mpf_init(t);
	//mpf_init(ti);
	//for(int i = 0; i < iter_num; i ++)  //枚举所有情况
	//{
	//	showControlNum ++;
	//	int pos = 0;
	//	if (i == iter_num - 1 && cobchpNum == 3)
	//	{
	//		pos = 10000;
	//		showControlNum = 0;
	//		::PostMessage((HWND) hWnd, MESSAGEFROMTHREAD_CALCOM, (WPARAM)pos, (LPARAM) 0 );
	//	}
	//	if (showControlNum == 5000)
	//	{
	//		
	//		mpf_set_ui(t, iter_num);
	//		mpf_set_ui(ti, i);
	//		mpf_mul_ui(t, t, maxChipNums - 2);
	//		mpf_div(t, ti, t);
	//		//double d = (double)(iter_num*(maxChipNums - 2));
	//		//double a = (double)i/d;
	//		double a = mpf_get_d(t);
	//		double b = (double)(maxChipNums - cobchpNum)/(double)(maxChipNums - 2);
	//		double retData = a + b;
	//		pos = retData*10000;
	//		showControlNum = 0;
	//		::PostMessage((HWND) hWnd, MESSAGEFROMTHREAD_CALCOM, (WPARAM)pos, (LPARAM) 0 );
	//	}
	//	int s = 0, k = 0, j = 0;
	//	for(j = i, k = 0; j != 0; j>>=1, k++) //针对每种情况求和，判断是否满足条件
	//	{
	//		if(j&1)
	//			s += needChips[k];
	//	}
	//	//CString str;
	//	if(s == cobchpNum) //如果满足，打印结果
	//	{
	//		vector<int> v;
	//		int len,wid;
	//		bool flag = TRUE;
	//		for(j = i, k = 0; j != 0; j>>=1, k++)
	//		{
	//			if(j&1)
	//			{
	//				v.push_back(k);
	//			}
	//		}
	//		if (flag)
	//		{
	//			len = vLen[v[0]];
	//			wid = vWid[v[0]];
	//			for (int h = 1; h < v.size(); h ++)
	//			{
	//				int nlen = vLen[v[h]];
	//				int nwid = vWid[v[h]];
	//				if (nlen <= len && nwid <= wid)
	//				{
	//					len = nlen;
	//					wid = nwid;		
	//				}
	//				else
	//				{
	//					flag = false;
	//					break;
	//				}
	//			}
	//			if (flag)
	//			{						
	//				vRelt.push_back(v);
	//			}
	//		}				
	//	}
	//}

	//return vRelt;
}

void CGlassCompositionDlg::calBestOptResult()
{
	int sz = relt.size();

	mPro_Adjust.SetRange(0, sz - 1);
	mPro_Adjust.SetStep(1);
	vector<vector<int>> resultM(sz,sz); //以矩阵的形式表达是否相等，不相等则为1
	for (int i = 0; i < sz - 1; i ++)
	{
		resultM[i][i] = 1;
		for (int j = i + 1; j < sz; j ++)
		{
			if (!SearchCommon(relt[i], relt[j]))
			{
				resultM[i][j] = 1;
				resultM[j][i] = 1;
			}
		}
		mPro_Adjust.StepIt();
	}
	reltM = resultM;
	m_hThread  = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GetMinCombs, (LPVOID)this->m_hWnd, NULL, &m_ThreadID);
	::InitializeCriticalSection(&cs);
		
}

void CGlassCompositionDlg::OnBnClickedButtonCalcmbsort()
{
	int sz = relt.size();

	mPro_Adjust.SetRange(0, sz - 1);
	mPro_Adjust.SetStep(1);
	vector<vector<int>> resultM(sz,sz); //以矩阵的形式表达是否相等，不相等则为1
	for (int i = 0; i < sz - 1; i ++)
	{
		resultM[i][i] = 1;
		for (int j = i + 1; j < sz; j ++)
		{
			if (!SearchCommon(relt[i], relt[j]))
			{
				resultM[i][j] = 1;
				resultM[j][i] = 1;
			}
		}
		mPro_Adjust.StepIt();
	}
	reltM = resultM;
	m_hThread  = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GetMinCombs, (LPVOID)this->m_hWnd, NULL, &m_ThreadID);
	::InitializeCriticalSection(&cs);
}

