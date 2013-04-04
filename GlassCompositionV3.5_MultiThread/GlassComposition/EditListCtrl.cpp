// EditListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "GlassComposition.h"
#include "GlassCompositionDlg.h"
#include "EditListCtrl.h"

//extern double totalArea;
// CEditListCtrl

IMPLEMENT_DYNAMIC(CEditListCtrl, CListCtrl)

	CEditListCtrl::CEditListCtrl()
{
	m_edit.m_hWnd = NULL;
}

CEditListCtrl::~CEditListCtrl()
{
}

BEGIN_MESSAGE_MAP(CEditListCtrl, CListCtrl)
	ON_WM_LBUTTONDBLCLK()
	ON_NOTIFY_REFLECT(NM_CLICK, &CEditListCtrl::OnNMClick)
	ON_MESSAGE(WM_USER_EDIT_END, OnEditEnd)
END_MESSAGE_MAP()


void CEditListCtrl::ShowEdit(BOOL bShow , int nItem, int nIndex, CRect rcCtrl)
{
	//AfxMessageBox("ShowEdit");
	// 如果编辑框对象尚未创建
	if(m_edit.m_hWnd == NULL)
	{
		//创建一个编辑框（大小为零）
		m_edit.Create(ES_AUTOHSCROLL|WS_CHILD|ES_LEFT
			|ES_WANTRETURN|WS_BORDER,CRect(0,0,0,0),this,IDC_EDIT_HIDE);
		m_edit.ShowWindow(SW_HIDE);// 隐藏

		//使用默认字体
		CFont tpFont;
		tpFont.CreateStockObject(DEFAULT_GUI_FONT);
		m_edit.SetFont(&tpFont);
		tpFont.DeleteObject();
	}
	//如果bShow为true，显示编辑框
	if(bShow == TRUE)
	{
		CString strItem = CListCtrl::GetItemText(nItem,nIndex);//获取列表控件中数据项的内容
		m_edit.MoveWindow(rcCtrl);// 移动到子项所在区域
		m_edit.ShowWindow(SW_SHOW);//显示控件		
		m_edit.SetWindowText(strItem);// 显示数据
		::SetFocus(m_edit.GetSafeHwnd());//设置焦点
		DWORD dt = nItem << 16 | nIndex;
		m_edit.SetCtrlData(dt);
		::SendMessage(m_edit.GetSafeHwnd(), EM_SETSEL, 0, -1);//使数据处于选择状态
	}
	else
		m_edit.ShowWindow(SW_HIDE);
}




void CEditListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	//AfxMessageBox("OnLButtonDbClk");
	CRect rcCtrl;        //数据项所在区域
	LVHITTESTINFO lvhti; //用于列表控件子项鼠标点击测试的数据结构
	lvhti.pt = point;  //输入鼠标位置
	int nItem = CListCtrl::SubItemHitTest(&lvhti);//调用基类的子项测试函数，返回行号
	if(nItem == -1)   //如果鼠标在控件外双击，不做任何处理
		return;
	int nSubItem = lvhti.iSubItem;//获得列号
	if (nSubItem != 4)
	{
		CListCtrl::GetSubItemRect(nItem,nSubItem,LVIR_LABEL,rcCtrl);
		//获得子项所在区域，存入rcCtrl
		ShowEdit(TRUE,nItem,nSubItem,rcCtrl); //调用自定义函数，显示编辑框
	}

	CListCtrl::OnLButtonDblClk(nFlags, point);
}


void CEditListCtrl::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here

	if(m_edit.m_hWnd != NULL)
	{
		DWORD dwStyle = m_edit.GetStyle();
		if((dwStyle&WS_VISIBLE) == WS_VISIBLE)
		{
			m_edit.ShowWindow(SW_HIDE);
		}
	} 

	*pResult = 0;
}


LRESULT CEditListCtrl::OnEditEnd(WPARAM wParam , LPARAM lParam)
{

	if(wParam == TRUE)
	{
		CString strText(_T(""));
		m_edit.GetWindowText(strText);
		DWORD dwData = m_edit.GetCtrlData();
		int nItem= dwData>>16;
		CString st;
		st.Format("%d", nItem);
		//AfxMessageBox(st);
		int nIndex = dwData&0x0000ffff;
		st.Format("%d", nIndex);
		//AfxMessageBox(st);
		CListCtrl::SetItemText(nItem,nIndex,strText);

		CWnd *pwnd = AfxGetMainWnd();

		if (nIndex == 1)
		{
			((CGlassCompositionDlg *)pwnd)->v_Id[nItem] = atoi(CListCtrl::GetItemText(nItem, 1));
			if (!(((CGlassCompositionDlg *)pwnd)->JudgeSameID(((CGlassCompositionDlg *)pwnd)->v_Id)))
			{
				AfxMessageBox("编号重复，请检查");
			}
		}

		if (nIndex == 2 || nIndex == 3)
		{
			CString area_cur = CListCtrl::GetItemText(nItem, 6);
			double area_cur_t = atof(area_cur);
			//CGlassCompositionDlg *pDlg = (CGlassCompositionDlg *)GetParent()->GetParent();
			//CWnd *pwnd = AfxGetMainWnd();
			double totla = ((CGlassCompositionDlg *)pwnd)->totalArea;
			totla -= area_cur_t;
			//CString a = theApp.m_pMainWnd->GetWindowText(IDC_TOTAL_AREA);
			//totalArea -= area_cur_t;
			CString str_len, str_wid, str_area, str_area_tt;
			int len, wid, ae;
			double ae_tt;
			str_len = CListCtrl::GetItemText(nItem, 2);
			str_wid = CListCtrl::GetItemText(nItem, 3);
			len = atoi(str_len);
			wid = atoi(str_wid);
			ae = len * wid;
			ae_tt = (double)ae * atoi(CListCtrl::GetItemText(nItem, 5)) / 100;
			str_area.Format("%d", ae);
			CListCtrl::SetItemText(nItem, 4, str_area);
			str_area_tt.Format("%.2lf", ae_tt);
			CListCtrl::SetItemText(nItem, 6, str_area_tt);

			CString ta;
			totla += ae_tt;
			((CGlassCompositionDlg *)pwnd)->totalArea = totla;
			ta.Format("%.2lf", totla);
			((CGlassCompositionDlg *)pwnd)->GetDlgItem(IDC_TOTAL_AREA)->SetWindowText(ta);
		}
		if (nIndex == 5)
		{
			//--------------
			CString area_cur = CListCtrl::GetItemText(nItem, 6);
			double area_cur_t = atof(area_cur);
			CWnd *pwnd = AfxGetMainWnd();
			double totla = ((CGlassCompositionDlg *)pwnd)->totalArea;
			totla -= area_cur_t;

			//CString need_num = CListCtrl::GetItemText(nItem, 5);
			//int nn = atoi(need_num);
			int tnn = ((CGlassCompositionDlg *)pwnd)->totalNeedNumber;
			tnn -= ((CGlassCompositionDlg *)pwnd)->nn_edit[nItem];
			//--------------
			CString str_area, str_area_tt;
			int ae;
			double ae_tt;
			str_area = CListCtrl::GetItemText(nItem, 4);
			ae = atoi(str_area);
			ae_tt = (double)ae * atoi(CListCtrl::GetItemText(nItem, 5)) / (double)100;
			str_area_tt.Format("%.2lf", ae_tt);
			CListCtrl::SetItemText(nItem, 6, str_area_tt);

			CString ta;
			totla += ae_tt;
			((CGlassCompositionDlg *)pwnd)->totalArea = totla;
			ta.Format("%.2lf", totla);
			((CGlassCompositionDlg *)pwnd)->GetDlgItem(IDC_TOTAL_AREA)->SetWindowText(ta);

			CString tn;
			tnn += atoi(CListCtrl::GetItemText(nItem, 5));
			((CGlassCompositionDlg *)pwnd)->totalNeedNumber = tnn;
			tn.Format("%d", tnn);
			((CGlassCompositionDlg *)pwnd)->GetDlgItem(IDC_TOTAL_NUMBER)->SetWindowText(tn);
		}
		((CGlassCompositionDlg *)pwnd)->vv_Id[nItem][0] = atoi(CListCtrl::GetItemText(nItem, 2));
		((CGlassCompositionDlg *)pwnd)->vv_Id[nItem][1] = atoi(CListCtrl::GetItemText(nItem, 3));
		((CGlassCompositionDlg *)pwnd)->vv_Id[nItem][2] = atoi(CListCtrl::GetItemText(nItem, 5));
		if (!(((CGlassCompositionDlg *)pwnd)->JudgeSameValue(((CGlassCompositionDlg *)pwnd)->vv_Id)))
		{
			AfxMessageBox("存在重复长宽片数，请检查");
		}
	}
	else
	{   
	}
	if(lParam == FALSE)
		m_edit.ShowWindow(SW_HIDE);
	return 0;

}


BOOL CEditListCtrl::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{		
		//拦截Tab键
		if(pMsg->wParam == VK_TAB && m_edit.m_hWnd!= NULL)
		{
			//检测编辑框是否处于显示状态
			DWORD dwStyle = m_edit.GetStyle();
			if((dwStyle&WS_VISIBLE) == WS_VISIBLE)
			{
				OnEditEnd(TRUE,TRUE);//更新前一个子项的数据
				CRect rcCtrl;  
				int nItem;
				int nSub;
				//if(FALSE == Key_Ctrl(nItem,nSub)) //实现Ctrl+Tab的功能
				Key_Shift(nItem,nSub);//调用Key_Shift更改行号及列号
				//获得跳转后子项区域
				CListCtrl::GetSubItemRect(nItem,nSub,LVIR_LABEL,rcCtrl);
				//进入编辑状态
				CPoint pt(rcCtrl.left+1,rcCtrl.top+1);
				OnLButtonDblClk(0,pt);
				//控制行被选中状态
				POSITION pos = CListCtrl::GetFirstSelectedItemPosition();
				if (pos == NULL)
				{ }
				else
				{
					while (pos)
					{
						int ntpItem = CListCtrl::GetNextSelectedItem(pos);
						CListCtrl::SetItemState(ntpItem,0,LVIS_SELECTED);
					}
				}
				CListCtrl::SetItemState(nItem,  LVIS_SELECTED,  LVIS_SELECTED);
				return TRUE;
			}
		}
	}

	return CListCtrl::PreTranslateMessage(pMsg);
}


void CEditListCtrl::Key_Shift(int & nItem, int & nSub)
{
	//列表总行数
	int nItemCount = CListCtrl::GetItemCount();
	//当前编辑框所在位置
	DWORD dwData = m_edit.GetCtrlData();
	nItem= dwData>>16;
	nSub = dwData&0x0000ffff;
	//获取标题控件指针
	CHeaderCtrl* pHeader = CListCtrl::GetHeaderCtrl();
	if(pHeader == NULL)
		return;

	// 检测SHIFT键的状态，最高位为1-触发；0-未触发
	short sRet = GetKeyState(VK_SHIFT);
	int nSubcCount = pHeader->GetItemCount();//总列数
	sRet = sRet >>15;

	if(sRet == 0)//未触发
	{
		nSub += 1;//列号递增
		if(nSub >= nSubcCount)//到行尾
		{
			if(nItem == nItemCount-1)//到表尾，跳回表头
			{
				nItem = 0;
				nSub  = 0;
			}else //未到表尾，跳到下一行行首
			{
				nSub = 0;
				nItem += 1;
			}
		}

		if(nItem >= nItemCount)
			nItem = nItemCount-1;
	}
	else//触发
	{
		nSub -= 1;//列号递减
		if(nSub < 0)//到行首,跳到上一行行尾
		{
			nSub = nSubcCount -1;
			nItem --;
		}
		if(nItem < 0)//到表首，跳到表尾
			nItem = nItemCount-1;
	}

}


BOOL CEditListCtrl::Key_Ctrl(int & nItem, int & nSub)
{
	short sRet = GetKeyState(VK_CONTROL);
	DWORD dwData = m_edit.GetCtrlData();
	nItem= dwData>>16;
	nSub = dwData&0x0000ffff;

	sRet = sRet >>15;
	int nItemCount = CListCtrl::GetItemCount();
	if(sRet != 0)
	{
		nItem = nItem >=nItemCount-1? 0:nItem+=1;
		return TRUE;
	}
	return FALSE;

}
