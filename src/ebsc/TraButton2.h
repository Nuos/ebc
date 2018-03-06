#pragma once
#include <atlimage.h>

typedef enum TRA_BTNSTATE{//��ť״̬
	TRA_BTN_NOR,
	TRA_BTN_HOT,
	TRA_BTN_PRE,	// ���»�ѡ��
	TRA_BTN_DIS
};

typedef struct _TRATRAPNGINFO_
{
	int nWidth;
	int nHeight;
	CImage*  pImg;
}TRAPNGINFO;


class CTraButton : public CButton
{
	// ��׼���캯��
public:
	CTraButton();
	virtual ~CTraButton();

public:
	//���ã����뱳��ͼ
	void Load(UINT IDBkGroup, int width=-1, int height=0, const CString& resourceType = _T("PNG"));
	//���ã�����Ӧ����ͼ
	void SetAutoSize(bool bAutoSize);

	void MovePoint(int x, int y);
	void SetChecked(bool b);
	bool GetChecked() const;

	int GetImgWidth(void) const {return m_btninfoNor.pImg==NULL?0:m_btninfoNor.pImg->GetWidth();}
	int GetImgHeight(void) const {return m_btninfoNor.pImg==NULL?0:m_btninfoNor.pImg->GetHeight();}
	void SetToolTipText(LPCTSTR s);
	void SetToolTipColor(COLORREF crText, COLORREF crBkgnd);

protected:
	//�ػ�
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
protected:
	//{{AFX_MSG(CTraButton)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void RelayEvent(UINT message, WPARAM wParam, LPARAM lParam);

private:
	TRAPNGINFO		m_btninfoNor;				//Normalʱ��ͼƬ
	TRAPNGINFO		m_btninfoHot;				//Hotʱ��ͼƬ
	TRAPNGINFO		m_btninfoPre;				//Pressʱ��ͼƬ
	TRAPNGINFO		m_btninfoDis;				//Disableʱ��ͼƬ
	bool			m_bHot;						//�Ƿ�ΪHot
	bool			m_bPress;					//�Ƿ���
	bool			m_bSelected;				// �Ƿ�ѡ�� add by hd 

	bool			m_bAutoSize;				//�Զ���Ӧ
	CToolTipCtrl m_tooltip;

private:
	//��ͼ
	void DrawBK(HDC dc, CImage* img,TRA_BTNSTATE btnstate);
	//д��
	void DrawBtnText(HDC dc, const CString& strText, int nMove, TRA_BTNSTATE btnstate);
	//�ı䰴ť���
	virtual void PreSubclassWindow();
};