#include "ItemEdit.h"
#pragma once

#define WM_USER_EDIT_END WM_USER + 1001
//double totalArea;
// CEditListCtrl

class CEditListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CEditListCtrl)

public:
	CEditListCtrl();
	virtual ~CEditListCtrl();

protected:
	afx_msg LRESULT OnEditEnd(WPARAM wParam,LPARAM lParam = FALSE);
	DECLARE_MESSAGE_MAP();
private:
	CItemEdit m_edit;
	void ShowEdit(BOOL bShow , int nItem, int nIndex, CRect rcCtrl);
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
private:
	void Key_Shift(int & nItem, int & nSub);
	BOOL Key_Ctrl(int & nItem, int & nSub);
};
