// ItemEdit.cpp : implementation file
//

#include "stdafx.h"
#include "GlassComposition.h"
#include "ItemEdit.h"
#include "EditListCtrl.h"

// CItemEdit

IMPLEMENT_DYNAMIC(CItemEdit, CEdit)

CItemEdit::CItemEdit()
: m_bExchange(FALSE)
, m_dwData(0)
{

}

CItemEdit::~CItemEdit()
{
}


BEGIN_MESSAGE_MAP(CItemEdit, CEdit)
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()



// CItemEdit message handlers


DWORD CItemEdit::GetCtrlData(void)
{
	return m_dwData;
}


void CItemEdit::SetCtrlData(DWORD dwData)
{
	m_dwData=dwData;
}


void CItemEdit::OnSetFocus(CWnd* pOldWnd)
{
	CEdit::OnSetFocus(pOldWnd);
	//AfxMessageBox("OnSetFocus");
	m_bExchange = TRUE;
}


void CItemEdit::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);
	// TODO: Add your message handler code here
	CWnd* pParent = this->GetParent();
	::PostMessage(pParent->GetSafeHwnd(),WM_USER_EDIT_END,m_bExchange,0);
}


BOOL CItemEdit::PreTranslateMessage(MSG* pMsg)
{
	
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_RETURN)
		{
			CWnd* pParent = this->GetParent();
			m_bExchange = TRUE;
			::PostMessage(pParent->GetSafeHwnd(),WM_USER_EDIT_END,m_bExchange,0);
			return true;
		}
		else if(pMsg->wParam == VK_ESCAPE)
		{
			CWnd* pParent = this->GetParent();
			m_bExchange = FALSE;
			::PostMessage(pParent->GetSafeHwnd(),WM_USER_EDIT_END,m_bExchange,0);
			return true;
		}
	}


	return CEdit::PreTranslateMessage(pMsg);
}
