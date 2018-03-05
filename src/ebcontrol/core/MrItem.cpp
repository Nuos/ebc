#include "StdAfx.h"
#include "MrItem.h"
#include <Mmsystem.h>
#include <atltime.h>

CMrItem::CMrItem(int nItemId, MR_ITEM_TYPE nItemType,HWND pParent, int nFontSize)
: m_nItemId(nItemId), m_nItemType(nItemType), m_nCtrlType(0), m_nParam2(0)
, m_pCallback(NULL)
, m_pParent(pParent)

{
	m_tTime = 0;
	m_image = NULL;
	m_nImageWidth = 0;
	m_nImageHeight = 0;
	m_nFrameCount = 0;
	m_nCurrentFrame = 0;
	m_dwCurrentTime = 0;
	m_pFrameDelay = NULL;
	m_nPicType = 0;

	Gdiplus::FontFamily fontFamily(theFontFamily.c_str());
	m_font = new Gdiplus::Font(&fontFamily, nFontSize, FontStyleRegular, UnitPixel);
	m_brush = new Gdiplus::SolidBrush(Gdiplus::Color::Black);
	m_pen = new Gdiplus::Pen(Gdiplus::Color::Black);
}

CMrItem::~CMrItem(void)
{
	if (m_image)
	{
		delete m_image;
		m_image = NULL;
	}
	if (m_pFrameDelay)
	{
		free(m_pFrameDelay);
		m_pFrameDelay = NULL;
	}
	if (m_font)
	{
		delete m_font;
		m_font = NULL;
	}
	if (m_brush)
	{
		delete m_brush;
		m_brush = NULL;
	}
	if (m_pen)
	{
		delete m_pen;
		m_pen = NULL;
	}

}

void CMrItem::SetImage(Gdiplus::Bitmap * pImage, int nWidth, int nHeight)
{
	if (m_image)
	{
		delete m_image;
		m_image = NULL;
	}
	if (m_pFrameDelay)
	{
		free(m_pFrameDelay);
		m_pFrameDelay = NULL;
	}
	m_image = pImage;
	m_nImageWidth = nWidth;
	m_nImageHeight = nHeight;
	m_nFrameCount = 1;
	m_nCurrentFrame = 0;
	m_dwCurrentTime = 0;
	if (m_image!=NULL)
	{
		// ���� ͼ��֡�� ��֡������GIF��
		UINT count = m_image->GetFrameDimensionsCount();
		GUID *pDimensionIDs = (GUID*)new GUID[count];
		m_image->GetFrameDimensionsList(pDimensionIDs,count);
		WCHAR strGuid[39];
		StringFromGUID2(pDimensionIDs[0],strGuid,39);
		m_nFrameCount = m_image->GetFrameCount(&pDimensionIDs[0]);
		delete []pDimensionIDs;
		UINT size = m_image->GetPropertyItemSize(PropertyTagFrameDelay);
		m_pFrameDelay = (Gdiplus::PropertyItem*)malloc(size);
		m_image->GetPropertyItem(PropertyTagFrameDelay,size,m_pFrameDelay);
		//std::vector<int> pDelayList;
		//for (int i=0; i<m_nFrameCount; i++)
		//	pDelayList.push_back(((long*)m_pFrameDelay->value)[i]*10);
		//int i=0;
		//long nDelay = ((long*)m_pFrameDelay->value)[i]*10;
	}
}
void CMrItem::SetFont(const Gdiplus::Font& font)
{
	if (font.GetSize() == m_font->GetSize() &&
		font.GetUnit() == m_font->GetUnit())
	{
		return;
	}
	delete m_font;
	m_font = font.Clone();
}
void CMrItem::SetBrush(const Gdiplus::Brush& brush)
{
	delete m_brush;
	m_brush = brush.Clone();
}
void CMrItem::SetPen(const Gdiplus::Pen& pen)
{
	delete m_pen;
	m_pen = pen.Clone();
}

const RectF& CMrItem::GetLastRect(void) const
{
	if (m_pDrawLines.empty())
	{
		return m_rect;
	}else
	{
		return m_pDrawLines[m_pDrawLines.size()-1]->GetRect();
	}
}

//void CMrItem::Move2Left(int cx, int maxwidth)
//{
//	const MR_ALIGNMENT nAlignment = m_pCallback->OnGetAlignment();
//	if (nAlignment==MR_ALIGNMENT_LEFT)
//	{
//		int nArcOffset = m_pCallback->OnGetArcOffset();
//		int nPoloygonWidth = m_pCallback->OnGetPoloygonWidth();
//		int nAlignmentValue1 = m_pCallback->OnGetAlignmentValue1();
//		int nAlignmentValue2 = m_pCallback->OnGetAlignmentValue2();
//		int nLineAlignmentValue = m_pCallback->OnGetLineAlignmentValue();
//		int nItemX = 0;//nArcOffset+(nAlignment==MR_ALIGNMENT_LEFT?nAlignmentValue1:nAlignmentValue2);
//		if (nAlignment==MR_ALIGNMENT_LEFT)
//		{
//			nItemX = nAlignmentValue1+nPoloygonWidth+nArcOffset;
//		}else
//		{
//			nItemX = nAlignmentValue2+nArcOffset;
//		}
//		const int const_max_item_width = cx-nAlignmentValue1-nAlignmentValue2-nArcOffset*2-nPoloygonWidth;
//		const RectF layoutRect(nItemX, 0, const_max_item_width, 1);
//
//		// ����룬���µ���
//		Move2Left(layoutRect,maxwidth);
//		//RectF rect0;
//		for (size_t i=0; i<m_pDrawLines.size(); i++)
//		{
//			const CMrDrawItem::pointer& pDrawItem = m_pDrawLines[i];
//			pDrawItem->Move2Left(layoutRect,maxwidth);
//		}
//	}
//}
void CMrItem::Move2Left(const RectF& rect, int maxwidth)
{
	if (rect.Width > m_rect.Width && rect.GetRight() > m_rect.GetRight())
	{
		if (m_nItemType == MR_ITEM_TYPE_ENTER)
		{
			m_rect = RectF(rect.GetRight()-m_rect.Width-4,m_rect.GetTop(),m_rect.Width,m_rect.Height);
		}else
		{
			m_rect = RectF(m_rect.GetLeft()+(rect.Width-maxwidth),m_rect.GetTop(),m_rect.Width,m_rect.Height);
		}
	}
}

void CMrItem::Move2Right(int cx, int maxwidth)
{
	const MR_ALIGNMENT nAlignment = m_pCallback->OnGetAlignment();
	if (nAlignment==MR_ALIGNMENT_RIGHT)
	{
		int nArcOffset = m_pCallback->OnGetArcOffset();
		int nPoloygonWidth = m_pCallback->OnGetPoloygonWidth();
		int nAlignmentValue1 = m_pCallback->OnGetAlignmentValue1();
		int nAlignmentValue2 = m_pCallback->OnGetAlignmentValue2();
		int nLineAlignmentValue = m_pCallback->OnGetLineAlignmentValue();
		int nItemX = 0;//nArcOffset+(nAlignment==MR_ALIGNMENT_LEFT?nAlignmentValue1:nAlignmentValue2);
		if (nAlignment==MR_ALIGNMENT_LEFT)
		{
			nItemX = nAlignmentValue1+nPoloygonWidth+nArcOffset;
		}else
		{
			nItemX = nAlignmentValue2+nArcOffset;
		}
		const int const_max_item_width = cx-nAlignmentValue1-nAlignmentValue2-nArcOffset*2-nPoloygonWidth;
		const RectF layoutRect(nItemX, 0, const_max_item_width, 1);

		// �Ҷ��룬���µ���
		Move2Right(layoutRect,maxwidth);
		//RectF rect0;
		for (size_t i=0; i<m_pDrawLines.size(); i++)
		{
			const CMrDrawItem::pointer& pDrawItem = m_pDrawLines[i];
			pDrawItem->Move2Right(layoutRect,maxwidth);
		}
	}

}

void CMrItem::Move2Right(const RectF& rect, int maxwidth)
{
	if (rect.Width > m_rect.Width && rect.GetRight() > m_rect.GetRight())
	{
		if (m_nItemType == MR_ITEM_TYPE_ENTER)
		{
			m_rect = RectF(rect.GetRight()-m_rect.Width-4,m_rect.GetTop(),m_rect.Width,m_rect.Height);
		}else
		{
			m_rect = RectF(m_rect.GetLeft()+(rect.Width-maxwidth),m_rect.GetTop(),m_rect.Width,m_rect.Height);
		}
	}
}

bool CMrItem::GetSelected(void) const
{
	for (size_t i=0; i<m_pDrawLines.size(); i++)
	{
		const CMrDrawItem::pointer& pDrawItem = m_pDrawLines[i];
		if (pDrawItem->GetSelected())
			return true;
	}
	return false;
}
std::wstring CMrItem::GetSelectString(void) const
{
	// ����
	//if (m_nItemType==MR_ITEM_TYPE_TIME && GetSelected() && !m_pDrawLines.empty())
	//	return m_pDrawLines[0]->GetText();

	//if (m_nItemType==MR_ITEM_TYPE_ENTER && GetSelected())
	//	return L"\r\n";
	//else if (m_nItemType==MR_ITEM_TYPE_SPACE && GetSelected())
	//{
	//	std::wstring result;
	//	ULONG nIndex = 0;
	//	while ((++nIndex) <= GetItemParam2())
	//	{
	//		result.append(L" ");
	//	}
	//	return result;
	//}

	std::wstring result;
	for (size_t i=0; i<m_pDrawLines.size(); i++)
	{
		const CMrDrawItem::pointer& pDrawItem = m_pDrawLines[i];
		result.append(pDrawItem->GetSelectString());
	}
	//if (!result.empty())
	//{
	//	// ?
	//	result.append(L"\r\n");
	//}
	return result;
}

//bool CMrItem::OnDblClick(const Point& pPos, const RectF& layoutRect)
//{
//	bool ret = false;
//	for (size_t i=0; i<m_pDrawLines.size(); i++)
//	{
//		const CMrDrawItem::pointer& pDrawItem = m_pDrawLines[i];
//		if (pDrawItem->SelectAll(pGraphics,pSelectStart,pSelectEnd,layoutRect))
//		{
//			ret = true;
//			if (pDrawItem->Draw(pGraphics,m_font,m_brush, layoutRect))
//			{
//				pDrawItem->InvalidateRect(layoutRect);
//			}
//		}
//	}
//	return ret;
//}

bool CMrItem::Onclick(const Point& pPos, const RectF& layoutRect, bool bCheckPosOnly)
{
	switch (m_nItemType)
	{
	case MR_ITEM_TYPE_PIC:
		{
			Point pMovePos = Point(pPos.X,pPos.Y+layoutRect.GetTop());
			if (PtInRect(pMovePos,m_rect))
			{
				if (bCheckPosOnly)
					return true;
				// 
				//this->m_nPicType = 0;
				//m_rect = RectF(0,0,0,0);
				return true;
			}
		}break;
	case MR_ITEM_TYPE_IMG:
		{
			Point pMovePos = Point(pPos.X,pPos.Y+layoutRect.GetTop());
			const RectF rectFullLine(m_rect.GetLeft(),m_rect.GetTop(),m_imgSize.Width,m_imgSize.Height);
			if (PtInRect(pMovePos,rectFullLine))
			{
				if (bCheckPosOnly || m_sPath.empty())
					return true;
				//if ((m_nCtrlType&MR_BTN_TYPE_LCLICK_CB)!=0)
				//	return true;
				if ((m_nCtrlType&MR_BTN_TYPE_LCLICK_OPEN)!=0 || ((m_nCtrlType&MR_BTN_TYPE_LDBLCLICK_OPEN)!=0 && (GetKeyState(VK_CONTROL)&0x80)!=0))
				{
					if (::PathFileExists(m_sPath.c_str()))
						ShellExecuteW(NULL, L"open", m_sPath.c_str(), NULL, m_sPath.c_str(),SW_SHOW);
					else
						MessageBox(NULL,_T("ͼƬ�ļ��Ѿ������ڣ�"),_T("��ʾͼƬ"),MB_OK|MB_ICONWARNING);
					//ShellExecuteW(::GetDesktopWindow(), L"open", m_sPath.c_str(), NULL, m_sPath.c_str(),SW_SHOW);
				}
				return true;
			}
		}break;
	case MR_ITEM_TYPE_SPACE:
		{
			Point pMovePos = Point(pPos.X,pPos.Y+layoutRect.GetTop());
			const RectF rectFullLine(m_rect.GetLeft(),m_rect.GetTop(),m_imgSize.Width,m_imgSize.Height);
			if (PtInRect(pMovePos,rectFullLine))
			{
				return true;
			}
		}break;
	case MR_ITEM_TYPE_TIME:
	case MR_ITEM_TYPE_WAV:
	case MR_ITEM_TYPE_URL:
	case MR_ITEM_TYPE_OPENFILE:
	case MR_ITEM_TYPE_OPENDIR:
	case MR_ITEM_TYPE_TEXT:
	//case MR_ITEM_TYPE_DBLCLICK_TEXT:
	case MR_ITEM_TYPE_CLICK_TEXT:
		{
			const bool bIsControl = (GetKeyState(VK_CONTROL)&0x80)==0?false:true;
			for (size_t i=0; i<m_pDrawLines.size(); i++)
			{
				const CMrDrawItem::pointer& pDrawItem = m_pDrawLines[i];
				if (pDrawItem->Onclick(pPos,layoutRect))
				{
					if (bCheckPosOnly || m_sPath.empty())
						return true;

					//if ((m_nCtrlType&MR_BTN_TYPE_LCLICK_CB)!=0)
					//	return true;
					if ((m_nCtrlType&MR_BTN_TYPE_LCLICK_OPEN)==0 && ((m_nCtrlType&MR_BTN_TYPE_LDBLCLICK_OPEN)==0 || !bIsControl))
						return true;

					if (m_nItemType==MR_ITEM_TYPE_OPENDIR)
					{
						if (::PathFileExists(m_sPath.c_str()))
						{
							const std::wstring sPath = L"/select, "+m_sPath;
							ShellExecuteW(NULL, NULL, L"explorer", sPath.c_str(), NULL, SW_SHOW);
						}else
						{
							const std::wstring::size_type find = m_sPath.rfind('\\');
							if (find!=std::wstring::npos)
							{
								const std::wstring sPath = m_sPath.substr(0,find+1);
								ShellExecuteW(NULL, L"open", sPath.c_str(), NULL, sPath.c_str(), SW_SHOW);
							}
						}
					}else if ((m_nItemType==MR_ITEM_TYPE_OPENFILE || m_nItemType==MR_ITEM_TYPE_URL)  )
					{
						ShellExecuteW(NULL, L"open", m_sPath.c_str(), NULL, m_sPath.c_str(),SW_SHOW);
						//ShellExecuteW(::GetDesktopWindow(), L"open", m_sPath.c_str(), NULL, m_sPath.c_str(),SW_SHOW);
					}else if (m_nItemType==MR_ITEM_TYPE_WAV )
					{
						sndPlaySound(m_sPath.c_str(), SND_ASYNC);
					}
					return true;
				}
			}
		}break;
	default:
		break;
	}
	return false;
}
bool CMrItem::OnDblClick(const Point& pPos, const RectF& layoutRect)
{
	switch (m_nItemType)
	{
	case MR_ITEM_TYPE_PIC:
		{
			Point pMovePos = Point(pPos.X,pPos.Y+layoutRect.GetTop());
			if (PtInRect(pMovePos,m_rect))
			{
				if ((m_nCtrlType&MR_BTN_TYPE_LDBLCLICK_OPEN)==0)
					return true;
				return true;
			}
		}break;
	case MR_ITEM_TYPE_IMG:
		{
			Point pMovePos = Point(pPos.X,pPos.Y+layoutRect.GetTop());
			const RectF rectFullLine(m_rect.GetLeft(),m_rect.GetTop(),m_imgSize.Width,m_imgSize.Height);
			if (PtInRect(pMovePos,rectFullLine))
			{
				if ((m_nCtrlType&MR_BTN_TYPE_LDBLCLICK_OPEN)==0)
					return true;
				if (!m_sPath.empty())
				{
					if (::PathFileExists(m_sPath.c_str()))
						ShellExecuteW(NULL, L"open", m_sPath.c_str(), NULL, m_sPath.c_str(),SW_SHOW);
					else
						MessageBox(NULL,_T("ͼƬ�ļ��Ѿ������ڣ�"),_T("��ʾͼƬ"),MB_OK|MB_ICONWARNING);
					//ShellExecuteW(::GetDesktopWindow(), L"open", m_sPath.c_str(), NULL, m_sPath.c_str(),SW_SHOW);
				}
				return true;
			}
		}break;
	case MR_ITEM_TYPE_SPACE:
		{
			Point pMovePos = Point(pPos.X,pPos.Y+layoutRect.GetTop());
			const RectF rectFullLine(m_rect.GetLeft(),m_rect.GetTop(),m_imgSize.Width,m_imgSize.Height);
			if (PtInRect(pMovePos,rectFullLine))
			{
				return true;
			}
		}break;
	case MR_ITEM_TYPE_TIME:
	case MR_ITEM_TYPE_WAV:
	case MR_ITEM_TYPE_URL:
	case MR_ITEM_TYPE_OPENFILE:
	case MR_ITEM_TYPE_OPENDIR:
	case MR_ITEM_TYPE_TEXT:
	case MR_ITEM_TYPE_CLICK_TEXT:
		{
			for (size_t i=0; i<m_pDrawLines.size(); i++)
			{
				const CMrDrawItem::pointer& pDrawItem = m_pDrawLines[i];
				if (pDrawItem->Onclick(pPos,layoutRect))
				{
					if ((m_nCtrlType&MR_BTN_TYPE_LDBLCLICK_OPEN)==0)
						return true;

					if (m_nItemType==MR_ITEM_TYPE_OPENDIR && !m_sPath.empty())
					{
						std::wstring sPath = L"/select, "+m_sPath;
						ShellExecuteW(NULL, NULL, L"explorer", sPath.c_str(), NULL, SW_SHOW);
					}else if ((m_nItemType==MR_ITEM_TYPE_OPENFILE || m_nItemType==MR_ITEM_TYPE_URL) && !m_sPath.empty())
					{
						ShellExecuteW(NULL, L"open", m_sPath.c_str(), NULL, m_sPath.c_str(),SW_SHOW);
						//ShellExecuteW(::GetDesktopWindow(), L"open", m_sPath.c_str(), NULL, m_sPath.c_str(),SW_SHOW);
					}else if (m_nItemType==MR_ITEM_TYPE_WAV && !m_sPath.empty())
					{
						sndPlaySound(m_sPath.c_str(), SND_ASYNC);
					}
					return true;
				}
			}
		}break;
	default:
		break;
	}
	return false;
}
int CMrItem::OnMouseMove(const Point& pPos, const RectF& layoutRect)
{
	switch (m_nItemType)
	{
	case MR_ITEM_TYPE_PIC:
		{
			Point pMovePos = Point(pPos.X,pPos.Y+layoutRect.GetTop());
			if (PtInRect(pMovePos,m_rect))
			{
				if ((m_nCtrlType&(MR_BTN_TYPE_LDBLCLICK_CB|MR_BTN_TYPE_LDBLCLICK_OPEN))!=0 && (GetKeyState(VK_CONTROL)&0x80)!=0)
					return 1;
				if ((m_nCtrlType&(MR_BTN_TYPE_LCLICK_CB|MR_BTN_TYPE_LCLICK_OPEN))==0)
					return 0;
				else
					return 1;
			}
		}break;
	case MR_ITEM_TYPE_IMG:
		{
			Point pMovePos = Point(pPos.X,pPos.Y+layoutRect.GetTop());
			const RectF rectFullLine(m_rect.GetLeft(),m_rect.GetTop(),m_imgSize.Width,m_imgSize.Height);
			if (PtInRect(pMovePos,rectFullLine))
			{
				if ((m_nCtrlType&(MR_BTN_TYPE_LDBLCLICK_CB|MR_BTN_TYPE_LDBLCLICK_OPEN))!=0 && (GetKeyState(VK_CONTROL)&0x80)!=0)
					return 1;
				if ((m_nCtrlType&(MR_BTN_TYPE_LCLICK_CB|MR_BTN_TYPE_LCLICK_OPEN))==0)
					return 0;
				else
					return 1;
			}
		}break;
	case MR_ITEM_TYPE_SPACE:
		{
			Point pMovePos = Point(pPos.X,pPos.Y+layoutRect.GetTop());
			const RectF rectFullLine(m_rect.GetLeft(),m_rect.GetTop(),m_imgSize.Width,m_imgSize.Height);
			if (PtInRect(pMovePos,rectFullLine))
			{
				return 0;
			}
		}break;
	case MR_ITEM_TYPE_TIME:
	case MR_ITEM_TYPE_WAV:
	case MR_ITEM_TYPE_URL:
	case MR_ITEM_TYPE_OPENFILE:
	case MR_ITEM_TYPE_OPENDIR:
	case MR_ITEM_TYPE_TEXT:
	//case MR_ITEM_TYPE_DBLCLICK_TEXT:
	case MR_ITEM_TYPE_CLICK_TEXT:
		{
			for (size_t i=0; i<m_pDrawLines.size(); i++)
			{
				const CMrDrawItem::pointer& pDrawItem = m_pDrawLines[i];
				if (pDrawItem->OnMouseMove(pPos,layoutRect))
				{
					if ((m_nCtrlType&(MR_BTN_TYPE_LDBLCLICK_CB|MR_BTN_TYPE_LDBLCLICK_OPEN))!=0 && (GetKeyState(VK_CONTROL)&0x80)!=0)
						return 1;
					if ((m_nCtrlType&(MR_BTN_TYPE_LCLICK_CB|MR_BTN_TYPE_LCLICK_OPEN))==0)
						return 0;
					else
						return 1;
				}
			}
		}break;
	default:
		break;
	}
	return -1;
}
void CMrItem::ClearSelect(Gdiplus::Graphics& graphics,const RectF& layoutRect)
{
	for (size_t i=0; i<m_pDrawLines.size(); i++)
	{
		const CMrDrawItem::pointer& pDrawItem = m_pDrawLines[i];
		if (pDrawItem->ClearSelect())
		{
			if (pDrawItem->Draw(graphics,m_font,m_brush, layoutRect))
				pDrawItem->InvalidateRect(layoutRect);
		}
	}
}
void CMrItem::SelectAll(Graphics& pGraphics, const RectF& layoutRect)
{
	for (size_t i=0; i<m_pDrawLines.size(); i++)
	{
		const CMrDrawItem::pointer& pDrawItem = m_pDrawLines[i];
		pDrawItem->SelectAll(layoutRect);
		if (pDrawItem->Draw(pGraphics,m_font,m_brush, layoutRect))
		{
			pDrawItem->InvalidateRect(layoutRect);
		}
	}
}
bool CMrItem::SelectAll(Graphics& pGraphics, const Point& pPos,const RectF& layoutRect)
{
	for (size_t i=0; i<m_pDrawLines.size(); i++)
	{
		const CMrDrawItem::pointer& pDrawItem = m_pDrawLines[i];
		if (pDrawItem->SelectAll(pPos,layoutRect))
		{
			if (pDrawItem->Draw(pGraphics,m_font,m_brush, layoutRect))
			{
				pDrawItem->InvalidateRect(layoutRect);
			}
			return true;
		}
	}
	return false;
}
bool CMrItem::InSelect(const Point& pPos,const RectF& layoutRect) const
{
	for (size_t i=0; i<m_pDrawLines.size(); i++)
	{
		const CMrDrawItem::pointer& pDrawItem = m_pDrawLines[i];
		if (pDrawItem->InSelect(pPos,layoutRect))
		{
			return true;
		}
	}
	return false;
}
bool CMrItem::InImgSelect(const Point& pPos,const RectF& layoutRect) const
{
	if (m_nItemType!=MR_ITEM_TYPE_IMG)
		return false;

	Point pMovePos = Point(pPos.X,pPos.Y+layoutRect.GetTop());
	const RectF rectFullLine(m_rect.GetLeft(),m_rect.GetTop(),m_imgSize.Width,m_imgSize.Height);
	if (PtInRect(pMovePos,rectFullLine))
		return true;
	else
		return false;
}
bool CMrItem::Select(Graphics& pGraphics, const Point& pSelectStart, const Point& pSelectEnd, const RectF& layoutRect)
{
	//const FontFamily fontFamily(L"Times New Roman");
	//const Gdiplus::Font font(&fontFamily, 15, FontStyleRegular, UnitPixel);
	//Gdiplus::SolidBrush brushString(Gdiplus::Color::Black);

	bool ret = false;
	for (size_t i=0; i<m_pDrawLines.size(); i++)
	{
		const CMrDrawItem::pointer& pDrawItem = m_pDrawLines[i];
		if (pDrawItem->Select(pGraphics,m_font,pSelectStart,pSelectEnd,layoutRect))
		{
			ret = true;
			if (pDrawItem->Draw(pGraphics,m_font,m_brush, layoutRect))
			{
				pDrawItem->InvalidateRect(layoutRect);
			}
		}
	}
	return ret;
}
CString theDayOfWeek[] = {
   L"����",   // Sunday
   L"��һ",   
   L"�ܶ�",   
   L"����",   
   L"����",   
   L"����",   
   L"����"
};

int CMrItem::OnSize(int y, int cx, int cy, const RectF& lastRect)
{
	Gdiplus::Graphics m_graphics(m_pParent);

	int nArcOffset = m_pCallback->OnGetArcOffset();
	int nPoloygonWidth = m_pCallback->OnGetPoloygonWidth();
	int nAlignmentValue1 = m_pCallback->OnGetAlignmentValue1();
	int nAlignmentValue2 = m_pCallback->OnGetAlignmentValue2();
	int nLineAlignmentValue = m_pCallback->OnGetLineAlignmentValue();
	const MR_ALIGNMENT nAlignment = m_pCallback->OnGetAlignment();
	int nItemX = 0;//nArcOffset+(nAlignment==MR_ALIGNMENT_LEFT?nAlignmentValue1:nAlignmentValue2);
	if (nAlignment==MR_ALIGNMENT_LEFT)
	{
		nItemX = nAlignmentValue1+nPoloygonWidth+nArcOffset;
	}else if (nAlignment==MR_ALIGNMENT_CENTER)
	{
		nItemX = 0;
	}else
	{
		nItemX = nAlignmentValue2+nArcOffset;
	}
	const int nItemY = y+nArcOffset;
	const int const_max_item_width = cx-nAlignmentValue1-nAlignmentValue2-nArcOffset*2-nPoloygonWidth;
	const RectF layoutRect(nItemX, nItemY, const_max_item_width, cy);	// �������㵱ǰITEM���Ի��Ŀռ�
	RectF layoutLastLineRect;											// ǰ��һ�п���λ�ã�NULLΪȫ������
	RectF newLineLayoutRect = layoutRect;								// ������У����п�ʼ��ſռ�
	if (lastRect.Height > 0 && layoutRect.GetRight()-lastRect.GetRight()>20)
	{
		// ǰ�����һ���п���λ��
		layoutLastLineRect = RectF(lastRect.GetRight()+1, lastRect.GetTop(), layoutRect.GetRight()-lastRect.GetRight()-1, cy);	// 1Ϊ���
		newLineLayoutRect = RectF(layoutRect.GetLeft(),lastRect.GetBottom()+nLineAlignmentValue,layoutRect.Width,layoutRect.Height);
	}

	switch (m_nItemType)
	{
	case MR_ITEM_TYPE_PIC:
		{
			const int nWidth = m_picRect.Width;
			const int nHeight = max(max(lastRect.Height,1),m_picRect.Height);
			//const int nHeight = max(max(lastRect.Height,16),m_picRect.Height);
			if (layoutLastLineRect.Height > 0)
				m_rect = RectF(layoutLastLineRect.GetLeft()+m_picRect.GetLeft(),layoutLastLineRect.GetTop()+m_picRect.GetTop(),nWidth,nHeight);
			else
				m_rect = RectF(layoutRect.GetLeft()+m_picRect.GetLeft(),layoutRect.GetTop()+m_picRect.GetTop(),nWidth,nHeight);
		}break;
	case MR_ITEM_TYPE_LINE:
		m_rect = RectF(layoutRect.GetLeft(),layoutRect.GetTop(),layoutRect.Width,nLineAlignmentValue);
		break;
	case MR_ITEM_TYPE_ENTER:
		{
			//m_rect = RectF(layoutRect.GetLeft(),layoutRect.GetTop(),layoutRect.Width,nLineAlignmentValue);
			m_rect = RectF(layoutRect.GetLeft(),layoutRect.GetTop(),1,nLineAlignmentValue);
			//return m_rect.Height;
		}break;
	case MR_ITEM_TYPE_TIME:
		{
			//const CTime pNow = CTime::GetCurrentTime();
			const CTime pTime(m_tTime);
			const CString sTime(pTime.Format(this->m_sParam.c_str()));
			//CString sTime(pTime.Format(_T("%H:%M:%S")));
			//// 
			//if ((this->GetCtrlType()&MR_TIME_TYPE_VIEW_YEAR_MONTH_DAY)!=0)
			//	sTime.Append(pTime.Format(_T("%Y-%m-%d")));
			//if ((this->GetCtrlType()&MR_TIME_TYPE_VIEW_HOUR_MINUTE_SECOND)!=0)
			//	sTime.Append(pTime.Format(_T("%H:%M:%S")));
			//else if ((this->GetCtrlType()&MR_TIME_TYPE_VIEW_HOUR_MINUTE)!=0)
			//	sTime.Append(pTime.Format(_T("%H:%M")));

			//CString sTime;
			//if (pNow.GetYear()==pTime.GetYear() && pNow.GetMonth()==pTime.GetMonth() && pNow.GetDay()==pTime.GetDay())
			//{
			//	// today
			//	//const time_t tNow = pNow.GetTime();
			//	//if (tNow-m_tTime<=5*60)						// 0-1����ǰ���ո�
			//	//	sTime = _T("�ո�");
			//	//else if (tNow-m_tTime<60*60)				// 5-60����ǰ��%d����ǰ
			//	//	sTime.Format(_T("%d����ǰ"), (int)((tNow-m_tTime)/60));
			//	//else
			//		sTime = pTime.Format(_T("%H:%M"));
			//}else if (pNow.GetYear()==pTime.GetYear() && pNow.GetMonth()==pTime.GetMonth() && pNow.GetDay()==pTime.GetDay()+1)
			//{
			//	// ����
			//	sTime = pTime.Format(_T("���� %H:%M"));
			////}else if (pNow.GetYear()==pTime.GetYear() && pNow.GetMonth()==pTime.GetMonth())
			////{
			////	// ����
			////	CString sDayOfWeek = theDayOfWeek[pTime.GetDayOfWeek()-1];
			////	sTime = pTime.Format(_T("����%d,%H:%M"));
			//}else if (pNow.GetYear()==pTime.GetYear())
			//{
			//	// ����
			//	const CString sDayOfWeek = theDayOfWeek[pTime.GetDayOfWeek()-1];
			//	sTime.Format(_T("%s %s %s"),pTime.Format(_T("%m-%d")),sDayOfWeek,pTime.Format(_T("%H:%M")));
			//}else
			//{
			//	const CString sDayOfWeek = theDayOfWeek[pTime.GetDayOfWeek()-1];
			//	sTime.Format(_T("%s %s %s"),pTime.Format(_T("%Y-%m-%d")),sDayOfWeek,pTime.Format(_T("%H:%M")));
			//}

			//USES_CONVERSION;
			const std::wstring sText(sTime);
			RectF stringLineRectTemp;
			if (layoutLastLineRect.Height > 0)
				m_graphics.MeasureString(sText.c_str(), -1, m_font, layoutLastLineRect, StringFormat::GenericTypographic(), &stringLineRectTemp);
			else
				m_graphics.MeasureString(sText.c_str(), -1, m_font, layoutRect, StringFormat::GenericTypographic(), &stringLineRectTemp);
			//m_graphics.MeasureString(sText.c_str(), -1, m_font, layoutRect, StringFormat::GenericTypographic(), &stringLineRectTemp);	// old ok
			CMrDrawItem::pointer pDrawItem;
			if (m_pDrawLines.empty())
			{
				pDrawItem = CMrDrawItem::create(m_nItemType,m_pParent);
				m_pDrawLines.push_back(pDrawItem);
			}else
			{
				pDrawItem = m_pDrawLines[0];
			}
			pDrawItem->SetText(sText);
			m_sString = sText;
			// д��ʹ��13��С���壬����������һ����
			m_rect = RectF(stringLineRectTemp.GetLeft(),stringLineRectTemp.GetTop()+1,stringLineRectTemp.Width,stringLineRectTemp.Height);
			//m_rect = RectF(stringLineRectTemp.GetLeft(),stringLineRectTemp.GetTop()+1,stringLineRectTemp.Width,stringLineRectTemp.Height);	// old ok
			pDrawItem->SetRect(m_rect);
		}break;
	case MR_ITEM_TYPE_IMG:
		{
			//if (m_image == NULL)
			//	m_image = new Gdiplus::Image(m_sString.c_str());
			if (m_image==NULL || m_image->GetWidth()==0 || m_image->GetHeight()==0)
			{
				const int const_default_image_size = 10;
				m_imgSize.Width = const_default_image_size;
				m_imgSize.Height = const_default_image_size;
			}else if (m_nImageWidth>0 && m_nImageHeight>0)
			{
				m_imgSize.Width = m_nImageWidth;
				m_imgSize.Height = m_nImageHeight;
			}else if (m_nImageWidth>0 && m_nImageHeight<=0)
			{
				m_imgSize.Width = std::min<int>((int)newLineLayoutRect.Width,m_nImageWidth);
				m_imgSize.Height = (m_imgSize.Width*m_image->GetHeight())/m_image->GetWidth();
			}else if (m_nImageWidth<=0 && m_nImageHeight>0)
			{
				m_imgSize.Height = m_nImageHeight;
				m_imgSize.Width = (m_imgSize.Height*m_image->GetWidth())/m_image->GetHeight();
			}else
			{
				m_imgSize.Width = std::min<int>((int)newLineLayoutRect.Width,m_image->GetWidth());
				m_imgSize.Height = (m_imgSize.Width*m_image->GetHeight())/m_image->GetWidth();
			}
			if (layoutLastLineRect.Width >= m_imgSize.Width)
			{
				// ��max�ߣ����ڸ�ǰ��ƥ��
				m_rect = RectF(layoutLastLineRect.GetLeft(),layoutLastLineRect.GetTop(),m_imgSize.Width,std::max<int>(lastRect.Height,m_imgSize.Height));
			}else
			{
				m_rect = RectF(newLineLayoutRect.GetLeft(),newLineLayoutRect.GetTop(),m_imgSize.Width,m_imgSize.Height);
				if (m_image!=NULL && m_imgSize.Height<m_image->GetHeight())	// ͼƬѹ����ʾ���߶���Ҫ����һ�㣬�����������֣�����������ʾ���ͼƬ�ص�
					m_rect.Height += std::min<int>(10,(m_image->GetHeight()-m_imgSize.Height));
			}
		}break;
	case MR_ITEM_TYPE_SPACE:
		{
			const int nWidth = this->GetItemParam2()*3;
			const int nHeight = max(lastRect.Height,16);
			if (layoutLastLineRect.Height > 0)
				m_rect = RectF(layoutLastLineRect.GetLeft(),layoutLastLineRect.GetTop(),nWidth,nHeight);
			else
				m_rect = RectF(layoutRect.GetLeft(),layoutRect.GetTop(),nWidth,nHeight);
		}break;
	case MR_ITEM_TYPE_WAV:
	case MR_ITEM_TYPE_URL:
	case MR_ITEM_TYPE_OPENFILE:
	case MR_ITEM_TYPE_OPENDIR:
	case MR_ITEM_TYPE_TEXT:
	//case MR_ITEM_TYPE_DBLCLICK_TEXT:
	case MR_ITEM_TYPE_CLICK_TEXT:
		{
			RectF stringLineRect;
			RectF stringLineRectTemp;
			CMrDrawItem::pointer pDrawItem;
			//size_t nStringIndex = 0;
			int nLineItemHeight = 0;
			//m_pDrawLines.clear();
			size_t nLineCount = 0;

			if (m_sString.empty())
			{
				if (layoutLastLineRect.Height>0)
					m_rect = RectF(layoutLastLineRect.GetLeft(),layoutLastLineRect.GetTop(),1,lastRect.Height);
				else
					m_rect = RectF(layoutRect.GetLeft(),layoutRect.GetTop(),1,10);	// ?10
			}else
			{
				m_rect = RectF();	// ��ʼ��
				std::wstring sText = m_sString;
				for (size_t i=1; i<=sText.size(); i++)
				{
					if (layoutLastLineRect.Height>0 && nLineCount==0)
						m_graphics.MeasureString(sText.c_str(), i, m_font, layoutLastLineRect, StringFormat::GenericTypographic(), &stringLineRectTemp);
					else
						m_graphics.MeasureString(sText.c_str(), i, m_font, newLineLayoutRect, StringFormat::GenericTypographic(), &stringLineRectTemp);
					//if (stringLineRectTemp.Width==0 || stringLineRectTemp.Width-stringLineRect.Width==0)
					//{
					//	stringLineRectTemp.Width += 3;	// ?���ܲ��ո�
					//}

					//if (i==2 && sText.c_str()[i-1]==' ')
					//{
					//	wchar_t lpszBuf[1024];
					//	wsprintf(lpszBuf,L"Text=%s,Width=%d,Height1=%d Height2=%d",sText.c_str(),(int)stringLineRectTemp.Width,(int)stringLineRectTemp.Height,(int)stringLineRect.Height);
					//	MessageBoxW(NULL,lpszBuf,L"xx",MB_OK|MB_ICONWARNING);
					//}
					if ((nLineCount==0 && i==1) ||	// ��һ���ͻ����
						(((int)stringLineRectTemp.Height - (int)stringLineRect.Height)>8))	// �Ѿ�����
					{
						if (nLineItemHeight==0)
							nLineItemHeight = stringLineRectTemp.Height;	// ÿ�и߶�
						if (pDrawItem.get()!=NULL)
						{
							// �ڶ������ϣ�����ǰһ������
							int topvalue = 0;	// ���и�
							if (nLineCount > 0)
							{
								topvalue = layoutLastLineRect.Height>0?((nLineCount-1)*nLineItemHeight):(nLineCount*nLineItemHeight);
								topvalue += (layoutLastLineRect.Height>0?((nLineCount-1)*nLineAlignmentValue):(nLineCount*nLineAlignmentValue));
							}
							RectF rectItem(stringLineRect.GetLeft(),stringLineRect.GetTop()+topvalue,stringLineRect.Width,nLineItemHeight);
							if (layoutLastLineRect.Height>0 && nLineCount==0)
							{
								const int nHeightOffset = (lastRect.Height-rectItem.Height-10)/2;	// 10��ƫ����������ǰ��ͼƬ����С������10����
								if (nHeightOffset>0)
								{
									rectItem.Y += nHeightOffset;
									rectItem.Height += (nHeightOffset+5);	// 5=10/2
								}else
								{
									rectItem.Height = std::max<int>(lastRect.Height,rectItem.Height);	// ��max�ߣ����ڸ�ǰ��ƥ��
								}
							}
							pDrawItem->SetRect(rectItem);
							pDrawItem->SetText(sText.substr(0,i-1));
							sText = sText.substr(i-1);
							//pDrawItem->SetText(sText.substr(nStringIndex-1,i-nStringIndex));
							//sText = sText.substr(pDrawItem->GetText().size());
							i = 0;	// *for���Զ���1
							if (m_rect.IsEmptyArea())
								m_rect = rectItem;//pDrawItem->GetRect();
							else
								m_rect = RectF(rectItem.GetLeft(),m_rect.GetTop(),newLineLayoutRect.Width,rectItem.GetBottom()-m_rect.GetTop());
							nLineCount++;
						}
						if (m_pDrawLines.size()>nLineCount)
							pDrawItem = m_pDrawLines[nLineCount];
						else
						{
							pDrawItem = CMrDrawItem::create(m_nItemType,m_pParent);
							pDrawItem->SetPath(m_sPath);
							m_pDrawLines.push_back(pDrawItem);
						}
						//nStringIndex = i;
					}
					stringLineRect = stringLineRectTemp;
				}
				if (pDrawItem.get()!=NULL && !sText.empty())
				//if (pDrawItem.get()!=NULL && sText.size()>=nStringIndex)
				{
					// �������һ�У���ֻ��һ������
					if (nLineItemHeight==0)
					{
						nLineItemHeight = std::max<int>(lastRect.Height,16);
						//wchar_t lpszBuf[1024];
						//wsprintf(lpszBuf,L"Text=%s,Height=%d,Width=%d",sText.c_str(),(int)nLineItemHeight,(int)stringLineRectTemp.Width);
						//MessageBoxW(NULL,lpszBuf,L"xx",MB_OK|MB_ICONWARNING);
					}
					if (stringLineRectTemp.Width==0)
						stringLineRectTemp.Width = sText.size()*3;
					int topvalue = 0;	// ���и�
					if (nLineCount > 0)
					{
						topvalue = layoutLastLineRect.Height>0?((nLineCount-1)*nLineItemHeight):(nLineCount*nLineItemHeight);
						topvalue += (layoutLastLineRect.Height>0?((nLineCount-1)*nLineAlignmentValue):(nLineCount*nLineAlignmentValue));
					}
					RectF rectItem(stringLineRectTemp.GetLeft(),stringLineRectTemp.GetTop()+topvalue,stringLineRectTemp.Width,nLineItemHeight);
					if (layoutLastLineRect.Height>0 && nLineCount==0 && lastRect.Height>rectItem.Height)
					{
						const int nHeightOffset = (lastRect.Height-rectItem.Height-10)/2;	// 10��ƫ����������ǰ��ͼƬ����С������10����
						if (nHeightOffset>0)
						{
							// 42,17
							//wchar_t lpszBuf[1024];
							//wsprintf(lpszBuf,L"nHeightOffset=%d, Top=%d,%d, Height=%d,%d(%s)",
							//	(int)nHeightOffset,(int)lastRect.Y,(int)rectItem.Y,(int)lastRect.Height,(int)rectItem.Height,m_sString.c_str());
							//MessageBoxW(NULL,lpszBuf,L"xx",MB_OK|MB_ICONWARNING);
							rectItem.Y += nHeightOffset;
							rectItem.Height += (nHeightOffset+5);	// 5=10/2
						}else
						{
							rectItem.Height = lastRect.Height;									// ��max�ߣ����ڸ�ǰ��ƥ��
						}
					}
					//if (layoutLastLineRect.Height>0 && nLineCount==0)
					//	rectItem.Height = std::max<int>(lastRect.Height,rectItem.Height);	// ��max�ߣ����ڸ�ǰ��ƥ��
					if (nAlignment==MR_ALIGNMENT_CENTER)
					{
						rectItem.X = (cx-stringLineRectTemp.Width)/2;
						rectItem.Width = stringLineRectTemp.Width;
					}
					pDrawItem->SetRect(rectItem);
					pDrawItem->SetText(sText);
					//pDrawItem->SetText(sText.substr(nStringIndex-1));
					if (m_rect.IsEmptyArea())
						m_rect = rectItem;//pDrawItem->GetRect();
					else
						m_rect = RectF(newLineLayoutRect.GetLeft(),m_rect.GetTop(),newLineLayoutRect.Width,rectItem.GetBottom()-m_rect.GetTop());
					nLineCount++;
				}
				// ȥ������������
				while (m_pDrawLines.size()>nLineCount)
				{
					m_pDrawLines.pop_back();
				}
			}

			//if (nAlignment==MR_ALIGNMENT_LEFT)
			//{
			//	wchar_t lpszBuf[1024];
			//	wsprintf(lpszBuf,L"lastX=%d,lastW=%d,lastH=%d rectX=%d,rectW=%d,rectH=%d,%d=[%s]",
			//		(int)lastRect.GetLeft(),(int)lastRect.Width,(int)lastRect.Height,(int)m_rect.GetLeft(),(int)m_rect.Width,(int)m_rect.Height,(int)m_nItemType,m_sString.c_str());
			//	MessageBoxW(NULL,lpszBuf,L"xx",MB_OK|MB_ICONWARNING);
			//}
		}break;
	default:
		break;
	}

	if (lastRect.Height > 0)
	{
		//if (m_rect.Height < lastRect.Height)
		//{
		//	m_rect.height = lastrect.height;
		//	if (!m_pDrawLines.empty())
		//	{
		//		CMrDrawItem::pointer pDrawItem = m_pDrawLines[m_pDrawLines.size()-1];
		//		RectF rect = RectF(stringLineRectTemp.GetLeft(),stringLineRectTemp.GetTop()+1,stringLineRectTemp.Width,stringLineRectTemp.Height);
		//		pDrawItem->SetRect();
		//	}
		//}
		//return m_rect.Height;
		return m_rect.GetBottom()-lastRect.GetBottom();
	}else
		return m_rect.Height;
}

void CMrItem::Draw(Graphics& pGraphics, const RectF& layoutRect)
{
	switch (m_nItemType)
	{
	case MR_ITEM_TYPE_PIC:
		{
			const int pic_x = m_picRect.X;
			const int pic_y = m_picRect.Y;
			const int pic_w = m_picRect.Width;
			const int pic_h = m_picRect.Height;
			switch (m_nPicType)
			{
			case 1:	// ��Բ
				{
					RectF rect(m_rect.GetLeft(),m_rect.GetTop()-layoutRect.GetTop(),m_picRect.Width,m_picRect.Height);
					pGraphics.FillEllipse(this->m_brush, rect);
				}break;
			case 2:	// ��ˮƽ����
				{
					const int nArcOffset = m_pCallback->OnGetArcOffset();
					const RectF& pLineRect = m_pCallback->OnGetLineItemRect();

					// pic_x ��ʾ Left ƫ����
					const int pos1x = (pLineRect.GetLeft()-nArcOffset) + pic_x;
					// pic_y ��ʾ Top ƫ����
					const int pos1y = m_rect.GetTop()-layoutRect.GetTop() + pic_y;
					// pic_w<=0 ��ʾʹ��ȫ��ƫ������+pic_w�պ��Ǹ�����
					// pic_w>0 ��ʾ pos1x ƫ����
					const int pos2x = pic_w<=0?(pLineRect.GetRight()+nArcOffset-m_pen->GetWidth()+pic_w):(pos1x+pic_w);
					const int pos2y = pos1y;

					const Gdiplus::Point pos1start(pos1x,pos1y);
					const Gdiplus::Point pos1end(pos2x,pos2y);
					pGraphics.DrawLine(m_pen, pos1start, pos1end);
				}break;
			default:
				break;
			}
		}break;
	case MR_ITEM_TYPE_SPACE:
	case MR_ITEM_TYPE_LINE:
		{
			//pGraphics.DrawImage(m_image, m_rect);
		}break;
	case MR_ITEM_TYPE_ENTER:
		{
		}break;
	case MR_ITEM_TYPE_TIME:
		//{
		//	for (size_t i=0; i<m_pDrawLines.size(); i++)
		//	{
		//		const CMrDrawItem::pointer& pDrawItem = m_pDrawLines[i];
		//		pDrawItem->Draw(pGraphics,m_font,m_brush,layoutRect);
		//	}
		//}break;
	case MR_ITEM_TYPE_WAV:
	case MR_ITEM_TYPE_URL:
	case MR_ITEM_TYPE_OPENFILE:
	case MR_ITEM_TYPE_OPENDIR:
	case MR_ITEM_TYPE_TEXT:
	case MR_ITEM_TYPE_CLICK_TEXT:
		{
			for (size_t i=0; i<m_pDrawLines.size(); i++)
			{
				const CMrDrawItem::pointer& pDrawItem = m_pDrawLines[i];
				pDrawItem->Draw(pGraphics,m_font,m_brush,layoutRect);
			}
		}break;
	case MR_ITEM_TYPE_IMG:
		{
			if (m_image!=NULL && m_image->GetType()!= Gdiplus::ImageTypeUnknown)
			{
				RectF rect(m_rect.GetLeft(),m_rect.GetTop()-layoutRect.GetTop(),m_imgSize.Width,m_imgSize.Height);
				pGraphics.DrawImage(m_image, rect);
				// �ñ���͸��������ˢ�±��������Ӳ���
				//Gdiplus::ImageAttributes pImageAttributes;
				//// ָ��͸��ɫ
				//pImageAttributes.SetColorKey(Gdiplus::Color(255,255,255),Gdiplus::Color(255,255,255));
				//pGraphics.DrawImage(m_image, rect,0,0,m_rect.Width,m_rect.Height,Gdiplus::UnitPixel,&pImageAttributes);

				m_dwCurrentTime = timeGetTime();
				m_nCurrentFrame = 0;
				//DrawGif(*pGraphics,layoutRect,timeGetTime());
			}
		}break;
	default:
		break;
	}
}

void CMrItem::DrawGif(Graphics& pGraphics,const RectF& layoutRect,DWORD dwNowTime)
{
	if (m_image != NULL && m_nFrameCount > 1 && m_pFrameDelay != NULL)
	{
		const long nDelay = ((long*)m_pFrameDelay->value)[m_nCurrentFrame]*10;	// ms
		if (m_dwCurrentTime+nDelay>dwNowTime)
			return;	// δ����ʾʱ��
		m_dwCurrentTime = dwNowTime;
		// ������һ֡
		GUID Guid = FrameDimensionTime;
		m_nCurrentFrame++;
		if(m_nFrameCount==m_nCurrentFrame)
		{
			m_nCurrentFrame = 0;
		}
		m_image->SelectActiveFrame(&Guid,m_nCurrentFrame);
		// ��ʾͼ��
		RectF rect(m_rect.GetLeft(),m_rect.GetTop()-layoutRect.GetTop(),m_imgSize.Width,m_imgSize.Height);
		pGraphics.DrawImage(m_image, rect);
		// �ñ���͸��������ˢ�±��������Ӳ���
		//Gdiplus::ImageAttributes pImageAttributes;
		//// ָ��͸��ɫ
		//pImageAttributes.SetColorKey(Gdiplus::Color(255,255,255),Gdiplus::Color(255,255,255));
		//pGraphics.DrawImage(m_image, rect,0,0,m_rect.Width,m_rect.Height,Gdiplus::UnitPixel,&pImageAttributes);
	}
}

