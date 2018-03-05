#include "StdAfx.h"
#include "MrLine.h"

CMrLine::CMrLine(HWND pParent,mr::bigint nMsgId)
: m_nAlignment(MR_ALIGNMENT_LEFT)
, m_nAlignment1(10),m_nAlignment2(30)
, m_penArc(Gdiplus::Color(224,224,224))
//, m_penArc(Gdiplus::Color::Gray)
, m_brushArc(Gdiplus::Color::LightGray)
, m_nArcOffset(0)
, m_nPolygonWidth(0)
, m_nPolygonHeight(0)
, m_bClassicMode(false)
, m_brushSelected(Gdiplus::Color(255,128,0))

, m_pParent(pParent)
, m_nMsgId(nMsgId)
, m_bShowReceiptFlag(true), m_nReceiptFlag(0)
, m_penReceiptFlag(Gdiplus::Color(0,128,255))
, m_brushReceiptFlag(Gdiplus::Color(0,128,255))

{
	m_nLineId = 0;
	m_nCurrentItemId = 0;
	m_bSelected = false;

	//m_bReceiptFlag = true;	// for test
}

CMrLine::~CMrLine(void)
{
}

void CMrLine::AddItem(const CMrItem::pointer& pItem)
{
	if (pItem.get()!=NULL)
	{
		m_pList.push_back(pItem);
	}
}
CMrItem::pointer CMrLine::GetItem(size_t nIndex) const
{
	CMrItem::pointer pItem;
	if (nIndex<m_pList.size())
	{
		pItem = m_pList[nIndex];
	}
	return pItem;
}
void CMrLine::ClearItem(void)
{
	m_pList.clear();
}

void CMrLine::SetFrameArc(int nArcOffset,int nPoloygonWidth,int nPoloygonHeight)
{
	m_nArcOffset = nArcOffset;
	m_nPolygonWidth = nPoloygonWidth;
	m_nPolygonHeight = nPoloygonHeight;
}
void CMrLine::SetFrameBorderColor(const Gdiplus::Color& colorFrame)
{
	//m_penArc.SetColor(colorFrame);
}
void CMrLine::SetFrameBackGroundColor(const Gdiplus::Color& colorBkgnd)
{
	m_brushArc.SetColor(colorBkgnd);
}
bool CMrLine::SetReceiptFlag(int v)
{
	if (m_nReceiptFlag != v)
	{
		m_nReceiptFlag = v;
		if ((m_nReceiptFlag&RECEIPT_FLAG_TRUE)==RECEIPT_FLAG_TRUE)
		{
			m_penReceiptFlag.SetColor(Gdiplus::Color(0,128,255));
			m_brushReceiptFlag.SetColor(Gdiplus::Color(0,128,255));
		}else
		{
			m_penReceiptFlag.SetColor(Gdiplus::Color(255,128,64));
			m_brushReceiptFlag.SetColor(Gdiplus::Color(255,128,64));
		}
		return true;
	}
	return false;
}

void CMrLine::SetFontSize(unsigned int nSize)
{
	Gdiplus::FontFamily fontFamily(theFontFamily.c_str());
	for (size_t i=0; i<m_pList.size(); i++)
	{
		const CMrItem::pointer& pItem = m_pList[i];
		if (nSize==0)
			pItem->SetFont(Gdiplus::Font(&fontFamily, const_font_size, FontStyleRegular, UnitPixel));
		else
			pItem->SetFont(Gdiplus::Font(&fontFamily, nSize, FontStyleRegular, UnitPixel));
	}
}
bool CMrLine::GetSelected(void) const
{
	for (size_t i=0; i<m_pList.size(); i++)
	{
		const CMrItem::pointer& pItem = m_pList[i];
		if (pItem->GetSelected())
			return true;
	}
	return false;
}
inline void AddString(std::wstring& sAddTo, std::wstring& sAddFrom)
{
	if (!sAddFrom.empty())
	{
		sAddTo.append(sAddFrom);
		sAddFrom.clear();
	}
}
std::wstring CMrLine::GetSelectString(void) const
{
	std::wstring sNeedAppendString;
	std::wstring result;
	for (size_t i=0; i<m_pList.size(); i++)
	{
		const CMrItem::pointer& pItem = m_pList[i];
		const std::wstring sSelectString = pItem->GetSelectString();
		if (sSelectString.empty())
		{
			if (!result.empty())
			{
				if (pItem->GetItemType()==MR_ITEM_TYPE_ENTER)
				{
					//AddString(result, sNeedAppendString);
					sNeedAppendString.append(L"\r\n");
				}else if (pItem->GetItemType()==MR_ITEM_TYPE_SPACE)
				{
					//AddString(result, sNeedAppendString);
					ULONG nIndex = 0;
					while ((++nIndex) <= pItem->GetItemParam2())
					{
						sNeedAppendString.append(L" ");
					}
				}
			}
			continue;
		}
		AddString(result, sNeedAppendString);

		//if (!result.empty())
		//{
		//	result.append(L"\r\n");
		//}
		result.append(sSelectString);
	}
	return result;
}

bool CMrLine::PtInLine(const Point& pPos, const RectF& layoutRect)
{
	const RectF rect(m_rect.GetLeft(),m_rect.GetTop()-layoutRect.GetTop(),m_rect.Width,m_rect.Height);
	bool bSelected = false;
	if (PtInRect(pPos,rect))
	{
		return true;
	}else
	{
		return false;
	}
}
bool CMrLine::OnDblClick(Gdiplus::Graphics& graphics,const Point& pPos, const RectF& layoutRect, CMrItem::pointer* pOutClickItem)
{
	bool bSelected = PtInLine(pPos,layoutRect);
	if (bSelected)
	{
		// ���е��
		for (size_t i=0; i<m_pList.size(); i++)
		{
			const CMrItem::pointer& pItem = m_pList[i];
			if ((pItem->GetCtrlType()&MR_BTN_TYPE_LDBLCLICK_OPEN)==0)
			{
				if (pItem->SelectAll(graphics,pPos,layoutRect))
				{
					if (pOutClickItem!=NULL)
						*pOutClickItem = pItem;
					break;
				}
			}else
			{
				if (pItem->OnDblClick(pPos,layoutRect))
				{
					if (pOutClickItem!=NULL)
						*pOutClickItem = pItem;
					break;
				}else if (pItem->SelectAll(graphics,pPos,layoutRect))
				{
					if (pOutClickItem!=NULL)
						*pOutClickItem = pItem;
					break;
				}
			}
		}
	}
	return bSelected;
}

bool CMrLine::Onclick(const Point& pPos, const RectF& layoutRect, bool bCheckPosOnly, CMrItem::pointer* pOutClickItem)
{
	bool bSelected = PtInLine(pPos,layoutRect);
	if (bSelected)
	{
		// ���е��
		for (size_t i=0; i<m_pList.size(); i++)
		{
			const CMrItem::pointer& pItem = m_pList[i];
			if (pItem->Onclick(pPos, layoutRect, bCheckPosOnly))
			{
				if (pOutClickItem!=NULL)
					*pOutClickItem = pItem;
				break;
			}
		}
	}

	//if (bSelected != m_bSelected)
	//{
	//	m_bSelected = bSelected;
	//	m_pParent->SendMessage(MR_WM_LINE_SELECTED,(WPARAM)this,m_bSelected?1:0);
	//	//m_pParent->PostMessage(MR_WM_LINE_SELECTED,(WPARAM)this,m_bSelected?1:0);
	//}
	return bSelected;
}
int CMrLine::OnMouseMove(const Point& pPos, const RectF& layoutRect,CMrItem::pointer* pOutInPtItem)
{
	if (m_rect.GetBottom() < layoutRect.GetTop() || // ������
		m_rect.GetTop() > layoutRect.GetBottom())	// ������
	{
		return -1;
	}
	for (size_t i=0; i<m_pList.size(); i++)
	{
		const CMrItem::pointer& pItem = m_pList[i];
		const int ret = pItem->OnMouseMove(pPos, layoutRect);
		if (ret==-1)	// ����λ��
			continue;
		if (pOutInPtItem!=NULL)
			*pOutInPtItem = pItem;
		return ret;		// ��λ��
	}
	return -1;
}
void CMrLine::ClearSelect(Gdiplus::Graphics& graphics, const RectF& layoutRect)
{
	for (size_t i=0; i<m_pList.size(); i++)
	{
		const CMrItem::pointer& pItem = m_pList[i];
		pItem->ClearSelect(graphics,layoutRect);
	}
}
bool CMrLine::InSelect(const Point& pPos,const RectF& layoutRect,CMrItem::pointer* pOutInPtItem) const
{
	for (size_t i=0; i<m_pList.size(); i++)
	{
		const CMrItem::pointer& pItem = m_pList[i];
		if (pItem->InSelect(pPos,layoutRect))
		{
			if (pOutInPtItem!=NULL)
				*pOutInPtItem = pItem;
			return true;
		}
	}
	return false;
}
bool CMrLine::InImgSelect(const Point& pPos,const RectF& layoutRect,CMrItem::pointer& pOutImgItem) const
{
	for (size_t i=0; i<m_pList.size(); i++)
	{
		const CMrItem::pointer& pItem = m_pList[i];
		if (pItem->InImgSelect(pPos,layoutRect))
		{
			pOutImgItem = pItem;
			return true;
		}
	}
	return false;
}
void CMrLine::SelectAll(Gdiplus::Graphics& graphics, const RectF& layoutRect)
{
	for (size_t i=0; i<m_pList.size(); i++)
	{
		const CMrItem::pointer& pItem = m_pList[i];
		pItem->SelectAll(graphics,layoutRect);
	}
}

bool CMrLine::Select(Gdiplus::Graphics& graphics, const Point& pSelectStart, const Point& pSelectEnd, const RectF& layoutRect)
{
	if (m_rect.GetBottom() < layoutRect.GetTop() || // ������
		m_rect.GetTop() > layoutRect.GetBottom())	// ������
	{
		if ((GetKeyState(VK_CONTROL)&0x80)==0x80) return false;	// ��Ctrl
		if (pSelectStart.Y < 0) return false;					// ��������ƶ�
		if (pSelectStart.Y > layoutRect.Height) return false;	// ��������ƶ�
		ClearSelect(graphics,layoutRect);
		return false;	// �����أ����ڴ���֮ǰ�Ѿ�ѡ������δѡ������
	}
	if ((pSelectStart.X==0&&pSelectStart.Y==0) || (pSelectEnd.X==0&&pSelectEnd.Y==0))
	{
		//m_bSelected = false;
		return false;
	}
	//pDC->SetBkMode(TRANSPARENT);
	//HDC m_hdcMemory = pDC->m_hDC;
	//Gdiplus::Graphics m_graphics(m_hdcMemory);

	//const Gdiplus::Font font(L"Timers New Roman",10);
	//const FontFamily fontFamily(L"Times New Roman");
	//const Gdiplus::Font font(&fontFamily, 15, FontStyleRegular, UnitPixel);

	//Gdiplus::StringFormat format;
	//format.SetAlignment(Gdiplus::StringAlignmentNear);
	//format.SetLineAlignment(Gdiplus::StringAlignmentNear);
	//Gdiplus::SolidBrush brushString(Gdiplus::Color::Black);

	//if (!PointInRect(pSelectStart,m_rectString) && !PointInRect(pSelectEnd,m_rectString)
	//	&& ((pSelectStart.Y<m_rectString.GetTop()&&pSelectEnd.Y<m_rectString.GetTop()) ||
	//	(pSelectStart.Y>m_rectString.GetBottom()&&pSelectEnd.Y>m_rectString.GetBottom())))
	//{
	//	// ��ʼѡ��㲻���ַ�����
	//	return;
	//}
	for (size_t i=0; i<m_pList.size(); i++)
	{
		const CMrItem::pointer& pItem = m_pList[i];
		if (pItem->Select(graphics,pSelectStart,pSelectEnd,layoutRect))
		{
			//return true;	// ***�п���ѡ����棻
		}
	}
	return false;
}
void CMrLine::OnSize(int y, int cx, int cy)
{
	// ���㳤��
	//y += m_nArcOffset;	// �л��ȣ�����y
	if (m_bClassicMode)
		m_rect = RectF(0,y,cx,0);
	else
		m_rect = RectF(0,y,cx,m_nArcOffset*2);
	m_pItemRect = RectF();
	// �ȼ��㳤��
	RectF lastRect;
	RectF rectAllItem;
	for (size_t i=0; i<m_pList.size(); i++)
	{
		const CMrItem::pointer& pItem = m_pList[i];
		const int nheight = pItem->OnSize(y, cx, cy, lastRect);
		lastRect = pItem->GetLastRect();
		// �����Ƿ��Ѿ����С�
		if (pItem->GetItemType()==MR_ITEM_TYPE_ENTER)
		{
			lastRect = RectF();
		}else
		{
			if (rectAllItem.IsEmptyArea())
				rectAllItem = pItem->GetRectF();
			else
				RectF::Union(rectAllItem,rectAllItem,pItem->GetRectF());
		}
		if (pItem->GetItemType()==MR_ITEM_TYPE_ENTER || nheight > 10)
		{
			// �����У��������һ���ұߣ�û�п���λ��
			y += (nheight+1);
		}
	}
	// ��Move2Right
	if (OnGetAlignment()==MR_ALIGNMENT_RIGHT)
	{
		for (size_t i=0; i<m_pList.size(); i++)
		{
			const CMrItem::pointer& pItem = m_pList[i];
			pItem->Move2Right(cx, rectAllItem.Width);
			if (pItem->GetItemType()!=MR_ITEM_TYPE_ENTER)
			{
				if (m_pItemRect.IsEmptyArea())
					m_pItemRect = pItem->GetRectF();
				else
					RectF::Union(m_pItemRect,m_pItemRect,pItem->GetRectF());
			}
		}
	}else
	{
		m_pItemRect = rectAllItem;
	}

	//for (size_t i=0; i<m_pList.size(); i++)
	//{
	//	if (!m_pItemRect.IsEmptyArea())
	//	{
	//		// �ڶ��п�ʼ�����Ӽ��
	//		y += 2;
	//	}
	//	const CMrItem::pointer& pItem = m_pList[i];
	//	pItem->OnSize(y, cx, cy);
	//	if (m_pItemRect.IsEmptyArea())
	//		m_pItemRect = pItem->GetRectF();
	//	else
	//		RectF::Union(m_pItemRect,m_pItemRect,pItem->GetRectF());
	//	y += pItem->GetRectF().Height;
	//}
	if (m_bClassicMode)
		m_rect = RectF(0,m_rect.GetTop(),cx,y-m_rect.GetTop());
	else
		m_rect = RectF(0,m_rect.GetTop(),cx,y-m_rect.GetTop()+m_nArcOffset*2);

}

void CMrLine::OnDraw(Gdiplus::Graphics& graphics, const RectF& layoutRect)
{
	if (m_rect.GetBottom() < layoutRect.GetTop() || // ������
		m_rect.GetTop() > layoutRect.GetBottom())	// ������
	{
		return;
	}
	//pDC->SetBkMode(TRANSPARENT);
	//HDC m_hdcMemory = pDC->m_hDC;
	//Gdiplus::Graphics m_graphics(m_hdcMemory);

	//Gdiplus::SolidBrush brushWhite2(Gdiplus::Color(250,250,250));
	//RectF rect(m_rect.GetLeft(),m_rect.GetTop()-layoutRect.GetTop(),m_rect.Width,m_rect.Height);
	//if (!m_bSelected)
	//{
	//	m_graphics.FillRectangle(&brushWhite2,rect);
	//}

	if (!m_bClassicMode && !m_pList.empty() && m_nArcOffset > 0)
	{
		// ���߿���ĸ�Բ��
		const int const_arc_width = m_nArcOffset*2;
		int const_arc_x = 0;
		const int const_arc_y = m_rect.GetTop()-layoutRect.GetTop();
		if (OnGetAlignment()==MR_ALIGNMENT_LEFT)
		{
			// �����
			const_arc_x = m_rect.GetLeft()+OnGetAlignmentValue1()+m_nPolygonWidth;
		//}else if (OnGetAlignment()==MR_ALIGNMENT_CENTER)
		//{
		//	const_arc_x = m_rect.GetLeft()+OnGetAlignmentValue1()+m_nPolygonWidth;
		}else
		{
			const_arc_x = m_rect.GetRight()-OnGetAlignmentValue1()-m_nPolygonWidth-m_pItemRect.Width-const_arc_width;
		}

		// ** �ĸ�Բ��
		const Gdiplus::RectF rect1(const_arc_x,const_arc_y,const_arc_width,const_arc_width);																// ����
		const Gdiplus::RectF rect2(rect1.GetLeft()+(int)m_pItemRect.Width,const_arc_y,const_arc_width,const_arc_width);			// ����
		const Gdiplus::RectF rect3(rect2.GetLeft(),rect1.GetTop()+(int)m_pItemRect.Height,const_arc_width,const_arc_width);	// ����
		const Gdiplus::RectF rect4(rect1.GetLeft(),rect3.GetTop(),const_arc_width,const_arc_width);													// ����
		// ** �ĸ�Բ������
		const Gdiplus::Point pos1start(rect1.GetLeft()+m_nArcOffset,rect1.GetTop());		// ���ϣ�˳ʱ��
		const Gdiplus::Point pos1end(rect1.GetLeft(),rect1.GetTop()+m_nArcOffset);
		const Gdiplus::Point pos2start(rect2.GetRight(),rect2.GetTop()+m_nArcOffset);		// ���ϣ���ʱ��
		const Gdiplus::Point pos2end(rect2.GetLeft()+m_nArcOffset, rect2.GetTop());
		const Gdiplus::Point pos3start(rect3.GetLeft()+m_nArcOffset,rect3.GetBottom());	// ���£�˳ʱ��
		const Gdiplus::Point pos3end(rect3.GetRight(), rect3.GetTop()+m_nArcOffset);
		const Gdiplus::Point pos4start(rect4.GetLeft(), rect4.GetTop()+m_nArcOffset);		// ���£���ʱ��
		const Gdiplus::Point pos4end(rect4.GetLeft()+m_nArcOffset,rect4.GetBottom());
		const Gdiplus::RectF rect5(pos1start.X,pos1start.Y,pos2end.X-pos1start.X,pos3start.Y-pos2end.Y);					// �м�����ռ����-�м�
		const Gdiplus::RectF rect6(pos1end.X,pos1end.Y,pos2start.X-pos1end.X,pos3end.Y-pos2start.Y);							// �м�����ռ����-����
		//const Gdiplus::RectF rect6(pos1end.X-1,pos1end.Y,pos2start.X-pos1end.X+2,pos3end.Y-pos2start.Y);						// �м�����ռ����
		//m_rectString = Gdiplus::RectF(rect5.GetLeft(),rect6.GetTop()-1,stringRect.Width,stringRect.Height);					// д�־���
		Gdiplus::Point polygon[4];		// ���Ǳ����Ϣ��ִ��
		// ** ���Ǳ�
		if (OnGetAlignment()==MR_ALIGNMENT_LEFT)
		{
			// �����
			polygon[0].X = pos1end.X;
			polygon[0].Y = pos1end.Y+2;
			polygon[1].X = pos1end.X-m_nPolygonWidth;
			polygon[1].Y = polygon[0].Y;
			polygon[2].X = polygon[0].X;
			polygon[2].Y = polygon[0].Y+m_nPolygonHeight;
		}else
		{
			// �Ҷ���
			polygon[0].X = pos2start.X;												// 
			polygon[0].Y = pos2start.Y+2;
			polygon[1].X = pos2start.X+m_nPolygonWidth;
			polygon[1].Y = polygon[0].Y+m_nPolygonHeight;
			polygon[2].X = polygon[0].X;
			polygon[2].Y = polygon[1].Y;//+m_nPolygonHeight;
		}
		//if (OnGetAlignment()==MR_ALIGNMENT_LEFT)
		//{
		//	// �����
		//	polygon[0].X = pos1end.X;
		//	polygon[0].Y = pos1end.Y+m_pItemRect.Height/2-m_nPolygonHeight/2;
		//	polygon[1].X = pos1end.X-m_nPolygonWidth;
		//}else
		//{
		//	polygon[0].X = pos2start.X;
		//	polygon[0].Y = pos2start.Y+m_pItemRect.Height/2-m_nPolygonHeight/2;
		//	polygon[1].X = pos2start.X+m_nPolygonWidth;
		//}
		//polygon[1].Y = polygon[0].Y+m_nPolygonHeight/2;	// ���������м�
		//polygon[2].X = polygon[0].X;
		//polygon[2].Y = polygon[0].Y+m_nPolygonHeight;
		graphics.SetSmoothingMode(SmoothingModeAntiAlias);	// �������
		graphics.FillEllipse(&m_brushArc,rect1);
		graphics.FillEllipse(&m_brushArc,rect2);
		graphics.FillEllipse(&m_brushArc,rect3);
		graphics.FillEllipse(&m_brushArc,rect4);
		graphics.FillRectangle(&m_brushArc,rect5);
		graphics.FillRectangle(&m_brushArc,rect6);
		graphics.SetSmoothingMode(SmoothingModeNone);	// ȡ���������
		graphics.FillPolygon(&m_brushArc,polygon,3);
		graphics.SetSmoothingMode(SmoothingModeAntiAlias);	// �������
		graphics.DrawArc(&m_penArc, rect1, -180, 90);
		graphics.DrawArc(&m_penArc, rect2, -90, 90);
		graphics.DrawArc(&m_penArc, rect3, 0, 90);
		graphics.DrawArc(&m_penArc, rect4, 90, 90);
		graphics.DrawLine(&m_penArc, pos1start, pos2end);		// ��
		graphics.DrawLine(&m_penArc, pos3start, pos4end);		// ��
		if (m_nPolygonWidth > 0 && m_nPolygonHeight > 0)
		{
			if (OnGetAlignment()==MR_ALIGNMENT_LEFT)
			{
				graphics.DrawLine(&m_penArc, pos2start, pos3end);	// ��
				graphics.DrawLine(&m_penArc, pos1end, polygon[0]);
				graphics.DrawLines(&m_penArc, polygon, 3);
				graphics.DrawLine(&m_penArc, polygon[2], pos4start);
			}else
			{
				graphics.DrawLine(&m_penArc, pos4start, pos1end);	// ��
				graphics.DrawLine(&m_penArc, pos2start, polygon[0]);
				graphics.DrawLines(&m_penArc, polygon, 3);
				graphics.DrawLine(&m_penArc, polygon[2], pos3end);
			}
		}else
		{
			graphics.DrawLine(&m_penArc, pos4start, pos1end);	// ��
			graphics.DrawLine(&m_penArc, pos2start, pos3end);	// ��
		}
		if (m_bShowReceiptFlag && (m_nReceiptFlag&RECEIPT_FLAG_SHOW)==RECEIPT_FLAG_SHOW)
		{
			// ** ��Ϣ��ִ��
			if (OnGetAlignment()==MR_ALIGNMENT_LEFT)
			{
				// ����� 
				polygon[0].X = pos2start.X + m_nPolygonWidth + 1;						// ��������ĵ�
				polygon[0].Y = pos2start.Y + m_nPolygonHeight + 1;
			}else
			{
				// �Ҷ���
				polygon[0].X = pos1end.X - m_nPolygonWidth - 1;						// ��������ĵ�
				polygon[0].Y = pos1end.Y + m_nPolygonHeight + 1;
				//polygon[0].X = polygon[0].X+m_nPolygonWidth+3;						// ��������ĵ�
				//polygon[0].Y = polygon[0].Y+3;
			}
			polygon[1].X = polygon[0].X + (m_nPolygonWidth*2)/3;		// �������Ͻǵĵ�
			polygon[1].Y = polygon[0].Y-m_nPolygonHeight;
			polygon[2].X = polygon[0].X - 1;												// �����м��
			polygon[2].Y = polygon[0].Y-3;
			polygon[3].X = polygon[0].X - m_nPolygonWidth/3-1;			// �������
			polygon[3].Y = polygon[0].Y-4;
			graphics.SetSmoothingMode(SmoothingModeNone);	// ȡ���������
			graphics.FillPolygon(&m_brushReceiptFlag,polygon,4);
			graphics.SetSmoothingMode(SmoothingModeAntiAlias);	// �������
			graphics.DrawLines(&m_penReceiptFlag, polygon, 4);
			graphics.DrawLine(&m_penReceiptFlag, polygon[3], polygon[0]);
		}
	}else
	{

	}

	// д����ȥ
	//const FontFamily fontFamily(L"Times New Roman");
	//const Gdiplus::Font font(&fontFamily, 15, FontStyleRegular, UnitPixel);
	//Gdiplus::SolidBrush brushString(Gdiplus::Color::Black);
	for (size_t i=0; i<m_pList.size(); i++)
	{
		const CMrItem::pointer& pItem = m_pList[i];
		pItem->Draw(graphics,layoutRect);
	}
}

int CMrLine::OnDrawGif(Gdiplus::Graphics& graphics,const RectF& layoutRect,DWORD dwNowTime)
{
	if (m_rect.GetBottom() < layoutRect.GetTop()) return -1;	// ������
	if (m_rect.GetTop() > layoutRect.GetBottom()) return 1;		// ������
	for (size_t i=0; i<m_pList.size(); i++)
	{
		const CMrItem::pointer& pItem = m_pList[i];
		pItem->DrawGif(graphics,layoutRect,dwNowTime);
	}
	return 0;
}

