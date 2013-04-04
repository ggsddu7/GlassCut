#pragma once
class global
{
public:
	global(void);
	~global(void);
};

typedef struct DATA
{
	int subitem;// 点击表头的列数
	CListCtrl* plist; //listctrl的指针
};

extern DATA* dat;
extern bool fav;