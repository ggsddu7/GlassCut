#pragma once


// CItemEdit

class CItemEdit : public CEdit
{
	DECLARE_DYNAMIC(CItemEdit)

public:
	CItemEdit();
	virtual ~CItemEdit();

protected:
	DECLARE_MESSAGE_MAP()
private:
	BOOL m_bExchange;
	DWORD m_dwData;
public:
	DWORD GetCtrlData(void);
	void SetCtrlData(DWORD dwData);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


