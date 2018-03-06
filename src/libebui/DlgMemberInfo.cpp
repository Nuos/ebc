// DlgMemberInfo.cpp : implementation file
//

#include "stdafx.h"
#include "EBUIApp.h"
#include "DlgMemberInfo.h"

#define TIMERID_CHECK_RESOURCEFILE 100

// CDlgMemberInfo dialog

IMPLEMENT_DYNAMIC(CDlgMemberInfo, CEbDialogBase)

CDlgMemberInfo::CDlgMemberInfo(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CDlgMemberInfo::IDD, pParent)
	, m_nGroupType(EB_GROUP_TYPE_DEPARTMENT), m_sGroupCode(0)
	, m_sMemberCode(0)
	, m_sMemberUserId(0)
	, m_sMemberAccount(_T(""))
	, m_sUserName(_T(""))
	, m_sJobTitle(_T(""))
	, m_nJobPosition(0)
	, m_sCellPhone(_T(""))
	, m_sWorkPhone(_T(""))
	, m_sEmail(_T(""))
	, m_sFax(_T(""))
	, m_sAddress(_T(""))
	, m_sDescription(_T(""))
	, m_bNewEemployee(false)
	, m_nGender(0)
	, m_nBirthday(0)
{
	m_imageHead = NULL;
	m_pDlgChangeHead = NULL;
	m_nOldFileSize = 0;
	m_bSetTimer = false;
}

CDlgMemberInfo::~CDlgMemberInfo()
{
}

void CDlgMemberInfo::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btnClose);

	DDX_Text(pDX, IDC_EDIT_ACCOUNT, m_sMemberAccount);
	DDX_Text(pDX, IDC_EDIT_USERNAME, m_sUserName);
	DDX_Text(pDX, IDC_EDIT_DEPNAME, m_sGroupName);
	DDX_Text(pDX, IDC_EDIT_JOB_TITLE, m_sJobTitle);
	DDX_Text(pDX, IDC_EDIT_JOB_POSITION, m_nJobPosition);
	DDX_Text(pDX, IDC_EDIT_CELL_PHONE, m_sCellPhone);
	DDX_Text(pDX, IDC_EDIT_WORK_PHONE, m_sWorkPhone);
	DDX_Text(pDX, IDC_EDIT_EMAIL, m_sEmail);
	DDX_Text(pDX, IDC_EDIT_FAX, m_sFax);
	DDX_Text(pDX, IDC_EDIT_ADDRESS, m_sAddress);
	DDX_Text(pDX, IDC_EDIT_DESCRIPTION, m_sDescription);
	DDX_Control(pDX, IDC_COMBO_GENDER, m_comboGender);
	DDX_CBIndex(pDX, IDC_COMBO_GENDER, m_nGender);
	DDX_Control(pDX, IDC_COMBO_BIR_YEAR, m_comboBirYear);
	//DDX_CBString(pDX, IDC_COMBO_BIR_YEAR, m_nBirYear);
	DDX_Control(pDX, IDC_COMBO_BIR_MONTH, m_comboBirMonth);
	//DDX_CBString(pDX, IDC_COMBO_BIR_MONTH, m_nBirMonth);
	DDX_Control(pDX, IDC_COMBO_BIR_DAY, m_comboBirDay);
	//DDX_CBString(pDX, IDC_COMBO_BIR_DAY, m_nBirDay);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_BUTTON_CHANGEHEAD, m_btnChangeHead);
	DDX_Control(pDX, IDC_BUTTON_DEFAULT_EMP, m_btnDefaultEmp);

	DDX_Control(pDX, IDC_EDIT_ACCOUNT, m_editMemberAccount);
	DDX_Control(pDX, IDC_EDIT_DEPNAME, m_editGroupName);
	DDX_Control(pDX, IDC_EDIT_USERNAME, m_editUserName);
	DDX_Control(pDX, IDC_EDIT_JOB_TITLE, m_editJobTitle);
	DDX_Control(pDX, IDC_EDIT_JOB_POSITION, m_editJobPosition);
	DDX_Control(pDX, IDC_EDIT_CELL_PHONE, m_editCellPhone);
	DDX_Control(pDX, IDC_EDIT_WORK_PHONE, m_editWorkPhone);
	DDX_Control(pDX, IDC_EDIT_EMAIL, m_editEmail);
	DDX_Control(pDX, IDC_EDIT_FAX, m_editFax);
	DDX_Control(pDX, IDC_EDIT_ADDRESS, m_editAddress);
	DDX_Control(pDX, IDC_EDIT_DESCRIPTION, m_editDescription);
}


BEGIN_MESSAGE_MAP(CDlgMemberInfo, CEbDialogBase)
	ON_BN_CLICKED(IDOK, &CDlgMemberInfo::OnBnClickedOk)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CDlgMemberInfo::OnBnClickedButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_DEFAULT_EMP, &CDlgMemberInfo::OnBnClickedButtonDefaultEmp)
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_CHANGEHEAD, &CDlgMemberInfo::OnBnClickedButtonChangehead)
END_MESSAGE_MAP()


// CDlgMemberInfo message handlers

void CDlgMemberInfo::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	if (m_sMemberAccount.IsEmpty())
	{
		this->GetDlgItem(IDC_EDIT_ACCOUNT)->SetFocus();
		CDlgMessageBox::EbDoModal(this,_T("�������¼�ʺţ�"),_T(""),CDlgMessageBox::IMAGE_ERROR);
		return;
	}else if (m_bNewEemployee && theApp.GetAccountPrefix().IsEmpty())
	{
		const std::string sAccount = m_sMemberAccount;
		const std::string::size_type find = sAccount.find("@",0);
		//eb::bigint nPhone = 0;
		if (find != std::string::npos)
		{
			// xx@xx�����ʽ
		}else if (sAccount.size()==11 && sAccount.substr(0,1)=="1")
		{
			//nPhone = eb_atoi64(sAccount.c_str());
		}else
		{
			// �����ʽ��
			this->GetDlgItem(IDC_EDIT_ACCOUNT)->SetFocus();
			CDlgMessageBox::EbDoModal(this,"�ʺŸ�ʽ����","������������ȷ�����ַ���ֻ����룡",CDlgMessageBox::IMAGE_WARNING);
			return;
		}
	}
	if (m_sUserName.IsEmpty())
	{
		this->GetDlgItem(IDC_EDIT_USERNAME)->SetFocus();
		CDlgMessageBox::EbDoModal(this,_T("������������"),_T(""),CDlgMessageBox::IMAGE_ERROR);
		return;
	}
	if (m_bNewEemployee)
	{
#ifdef USES_EBCOM_TEST
		if (theEBClientCore->EB_IsExistMemberByAccount(m_sGroupCode, (LPCTSTR)m_sMemberAccount))
#else
		if (theEBAppClient.EB_IsExistMemberByAccount(m_sGroupCode, m_sMemberAccount))
#endif
		{
			this->GetDlgItem(IDC_EDIT_ACCOUNT)->SetFocus();
			CString sText;
			sText.Format(_T("[%s]�����������룡"), m_sMemberAccount);
			CDlgMessageBox::EbDoModal(this,_T("�ʺ��Ѿ����ڣ�"),sText,CDlgMessageBox::IMAGE_ERROR);
			return;
		}
	}
	// ������������
	CString stext;
	m_comboBirYear.GetWindowText(stext);
	int nBirYear = atoi(stext);
	m_comboBirMonth.GetWindowText(stext);
	int nBirMonth = atoi(stext);
	m_comboBirDay.GetWindowText(stext);
	int nBirDay = atoi(stext);
	m_nBirthday = nBirYear*10000+nBirMonth*100+nBirDay;

	OnOK();
}

BOOL CDlgMemberInfo::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();
	ModifyStyle(0, WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_SYSMENU);
	SetTransparentType(CEbDialogBase::TT_STATIC);

	//CRect rect;
	//m_editMemberAccount.GetRect(&rect);
	//rect.top += 4;
	//m_editMemberAccount.SetRectNP(&rect);
	m_editMemberAccount.ModifyStyleEx(0,WS_EX_CLIENTEDGE);
	//this->GetDlgItem(IDC_EDIT_ACCOUNT)->ModifyStyleEx(0,WS_EX_CLIENTEDGE);

	m_btnClose.SetAutoSize(false);
	m_btnClose.Load(IDB_PNG_CLOSE);
	m_btnClose.SetToolTipText(_T("�ر�"));
	m_btnOk.SetWindowText(_T("����"));
	m_btnCancel.SetWindowText(_T("�ر�"));
	m_btnOk.SetTextHotMove(false);
	m_btnCancel.SetTextHotMove(false);
	m_btnOk.SetDrawPanel(true,theApp.GetOkColor1(),theApp.GetHotColor(),theApp.GetNormalColor());
	m_btnCancel.SetDrawPanel(true,theDefaultBtnWhiteColor,theApp.GetHotColor(),theApp.GetNormalColor());

	m_editMemberAccount.SetRectangleColor(theApp.GetHotColor2(),theApp.GetNormalColor());
	m_editGroupName.SetRectangleColor(theApp.GetHotColor2(),theApp.GetNormalColor());
	m_editUserName.SetRectangleColor(theApp.GetHotColor2(),theApp.GetNormalColor());
	m_editJobTitle.SetRectangleColor(theApp.GetHotColor2(),theApp.GetNormalColor());
	m_editJobPosition.SetRectangleColor(theApp.GetHotColor2(),theApp.GetNormalColor());
	m_editCellPhone.SetRectangleColor(theApp.GetHotColor2(),theApp.GetNormalColor());
	m_editWorkPhone.SetRectangleColor(theApp.GetHotColor2(),theApp.GetNormalColor());
	m_editEmail.SetRectangleColor(theApp.GetHotColor2(),theApp.GetNormalColor());
	m_editFax.SetRectangleColor(theApp.GetHotColor2(),theApp.GetNormalColor());
	m_editAddress.SetRectangleColor(theApp.GetHotColor2(),theApp.GetNormalColor());
	m_editDescription.SetRectangleColor(theApp.GetHotColor2(),theApp.GetNormalColor());

	m_btnChangeHead.SetToolTipText(_T("����Ⱥ�����ţ���ʾͷ��"));
	m_btnChangeHead.SetNorTextColor(RGB(0,128,255));
	m_btnChangeHead.SetHotTextColor(RGB(0,128,255));
	m_btnDefaultEmp.SetToolTipText(_T("Ĭ����Ƭ�������촰����ʾ������Ƭ"));
	m_btnDefaultEmp.SetNorTextColor(RGB(0,128,255));
	m_btnDefaultEmp.SetHotTextColor(RGB(0,128,255));
	//m_btnDefaultEmp.Load(IDB_PNG_89X35);
	//m_btnDefaultEmp.Load(IDB_PNG_BTN);
	m_btnDefaultEmp.SetWindowText(_T("��ΪĬ����Ƭ"));
	if (m_sMemberCode>0)
	{
#ifdef USES_EBCOM_TEST
		const eb::bigint sDefaultMemberCode = theEBClientCore->EB_MyDefaultMemberCode;
#else
		eb::bigint sDefaultMemberCode;
		theEBAppClient.EB_GetMyDefaultMemberCode(sDefaultMemberCode);
#endif
		if (m_sMemberCode==sDefaultMemberCode)
		{
			m_btnDefaultEmp.SetWindowText(_T("ȡ��Ĭ����Ƭ"));
		}
	}

	m_comboGender.SetComboBitmap(IDB_COMBO_LEFT, IDB_COMBO_RIGHT, IDB_COMBO_CENTER);
	m_comboGender.SetHighlightColor( RGB(115,138,174),RGB(255,255,255));
	m_comboGender.SetNormalPositionColor( RGB(255,255,255),RGB(0,0,0));

	m_comboBirYear.SetComboBitmap(IDB_COMBO_LEFT, IDB_COMBO_RIGHT, IDB_COMBO_CENTER);
	m_comboBirYear.SetHighlightColor( RGB(115,138,174),RGB(255,255,255));
	m_comboBirYear.SetNormalPositionColor( RGB(255,255,255),RGB(0,0,0));
	m_comboBirMonth.SetComboBitmap(IDB_COMBO_LEFT, IDB_COMBO_RIGHT, IDB_COMBO_CENTER);
	m_comboBirMonth.SetHighlightColor( RGB(115,138,174),RGB(255,255,255));
	m_comboBirMonth.SetNormalPositionColor( RGB(255,255,255),RGB(0,0,0));
	m_comboBirDay.SetComboBitmap(IDB_COMBO_LEFT, IDB_COMBO_RIGHT, IDB_COMBO_CENTER);
	m_comboBirDay.SetHighlightColor( RGB(115,138,174),RGB(255,255,255));
	m_comboBirDay.SetNormalPositionColor( RGB(255,255,255),RGB(0,0,0));

	m_editMemberAccount.SetPromptText(_T("**��¼�ʺ�**"));
	m_editUserName.SetPromptText(_T("**�û�����**"));
	this->GetDlgItem(IDC_EDIT_DEPNAME)->EnableWindow(FALSE);
	if (!m_sMemberAccount.IsEmpty())
	{
		m_bNewEemployee = false;
		this->GetDlgItem(IDC_EDIT_ACCOUNT)->EnableWindow(FALSE);

#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_GetGroupType(m_sGroupCode,m_nGroupType);
		if (!theEBClientCore->GetEB_CanEditMyBaseMemberInfo(m_sGroupCode))
#else
		theEBAppClient.EB_GetGroupType(m_sGroupCode,m_nGroupType);
		if (!theEBAppClient.EB_CanEditMyBaseMemberInfo(m_sGroupCode))
#endif
		{
			this->GetDlgItem(IDC_EDIT_USERNAME)->EnableWindow(FALSE);
			this->GetDlgItem(IDC_COMBO_GENDER)->EnableWindow(FALSE);
			this->GetDlgItem(IDC_COMBO_BIR_YEAR)->EnableWindow(FALSE);
			this->GetDlgItem(IDC_COMBO_BIR_MONTH)->EnableWindow(FALSE);
			this->GetDlgItem(IDC_COMBO_BIR_DAY)->EnableWindow(FALSE);
			this->GetDlgItem(IDC_EDIT_CELL_PHONE)->EnableWindow(FALSE);
			this->GetDlgItem(IDC_EDIT_EMAIL)->EnableWindow(FALSE);
			this->GetDlgItem(IDC_EDIT_JOB_TITLE)->EnableWindow(FALSE);
			this->GetDlgItem(IDC_EDIT_JOB_POSITION)->EnableWindow(FALSE);
			this->GetDlgItem(IDC_EDIT_WORK_PHONE)->EnableWindow(FALSE);
			this->GetDlgItem(IDC_EDIT_FAX)->EnableWindow(FALSE);
			this->GetDlgItem(IDC_EDIT_ADDRESS)->EnableWindow(FALSE);
		}
	}else
	{
		m_bNewEemployee = true;
		this->GetDlgItem(IDC_BUTTON_CHANGEHEAD)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_BUTTON_DEFAULT_EMP)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_EDIT_ACCOUNT)->EnableWindow(TRUE);
		m_editMemberAccount.SetReadOnly(FALSE);
		this->EnableToolTips();
		this->SetToolTipText(IDC_EDIT_ACCOUNT,_T("�����������ַ�����ֻ����룩�����ڶ�����¼�ʺţ�"));
	}
	// �Ա�
	m_comboGender.AddString(_T(""));
	m_comboGender.AddString(_T("����"));
	m_comboGender.AddString(_T("Ů��"));
	CString stext;
	// ��
	for (int year=1960; year<=2012; year++)
	{
		stext.Format(_T("%04d��"), year);
		m_comboBirYear.AddString(stext);
	}
	const int nBirYear = m_nBirthday/10000;
	if (nBirYear > 0)
	{
		stext.Format(_T("%04d��"), nBirYear);
		m_comboBirYear.SetWindowText(stext);
	}
	// ��
	for (int month=1; month<=12; month++)
	{
		stext.Format(_T("%02d��"), month);
		m_comboBirMonth.AddString(stext);
	}
	const int nBirMonth = (m_nBirthday%10000)/100;
	if (nBirMonth > 0)
	{
		stext.Format(_T("%02d��"), nBirMonth);
		m_comboBirMonth.SetWindowText(stext);
	}
	// ��
	for (int day=1; day<=31; day++)
	{
		stext.Format(_T("%02d��"), day);
		m_comboBirDay.AddString(stext);
	}
	const int nBirDay = m_nBirthday%100;
	if (nBirDay > 0)
	{
		stext.Format(_T("%02d��"), nBirDay);
		m_comboBirDay.SetWindowText(stext);
	}

	const int const_y1	= 60;
	const int const_y2	= 148;
	const int const_x1	= 34;
	const int const_x2	= 314;
	const int const_x_interval	= 70;
	const int const_static_width	= 70;
	const int const_edit_width1		= 175;
	const int const_edit_width2		= 455;
	const int const_edit_height		= 21;
	const int const_y_interval	= 35;
	this->GetDlgItem(IDC_EDIT_ACCOUNT)->MoveWindow(123,const_y1,const_edit_width1,const_edit_height);
	this->GetDlgItem(IDC_EDIT_DEPNAME)->MoveWindow(123,const_y1+const_edit_height+2,const_edit_width1,const_edit_height);
	m_btnChangeHead.MoveWindow(123,const_y1+(const_edit_height+2)*2,80,32);

	int nX = const_x1;
	int nY = const_y2;
	this->GetDlgItem(IDC_STATIC_USERNAME)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_EDIT_USERNAME)->MoveWindow(nX,nY,const_edit_width1,const_edit_height);
	nX = const_x2;
	this->GetDlgItem(IDC_STATIC_JOB_TITLE)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_EDIT_JOB_TITLE)->MoveWindow(nX,nY,const_edit_width1,const_edit_height+1);

	nX = const_x1;
	nY += const_y_interval;
	this->GetDlgItem(IDC_STATIC_GENDER)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_COMBO_GENDER)->MoveWindow(nX,nY,const_edit_width1,const_edit_height+1);
	nX = const_x2;
	this->GetDlgItem(IDC_STATIC_JOB_POSITION)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_EDIT_JOB_POSITION)->MoveWindow(nX,nY,const_edit_width1,const_edit_height);

	nX = const_x1;
	nY += const_y_interval;
	this->GetDlgItem(IDC_STATIC_BIRTHDAY)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_COMBO_BIR_YEAR)->MoveWindow(nX,nY,71,const_edit_height+1);
	nX += 70;
	this->GetDlgItem(IDC_COMBO_BIR_MONTH)->MoveWindow(nX,nY,54,const_edit_height+1);
	nX += 53;
	this->GetDlgItem(IDC_COMBO_BIR_DAY)->MoveWindow(nX,nY,53,const_edit_height+1);
	nX = const_x2;
	this->GetDlgItem(IDC_STATIC_WORK_PHONE)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_EDIT_WORK_PHONE)->MoveWindow(nX,nY,const_edit_width1,const_edit_height);

	nX = const_x1;
	nY += const_y_interval;
	this->GetDlgItem(IDC_STATIC_CELL_PHONE)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_EDIT_CELL_PHONE)->MoveWindow(nX,nY,const_edit_width1,const_edit_height);
	nX = const_x2;
	this->GetDlgItem(IDC_STATIC_FAX)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_EDIT_FAX)->MoveWindow(nX,nY,const_edit_width1,const_edit_height);

	nX = const_x1;
	nY += const_y_interval;
	this->GetDlgItem(IDC_STATIC_EMAIL)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_EDIT_EMAIL)->MoveWindow(nX,nY,const_edit_width1,const_edit_height);
	nX = const_x2;
	this->GetDlgItem(IDC_STATIC_ADDRESS)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_EDIT_ADDRESS)->MoveWindow(nX,nY,const_edit_width1,const_edit_height);

	nX = const_x1;
	nY += const_y_interval;
	this->GetDlgItem(IDC_STATIC_DESCRIPTION)->MoveWindow(nX,nY,const_static_width,const_edit_height);
	nX += const_x_interval;
	this->GetDlgItem(IDC_EDIT_DESCRIPTION)->MoveWindow(nX,nY,const_edit_width2,53);


	const int POS_DLG_WIDTH = 612;
	const int POS_DLG_HEIGHT = 445;
	CRect rectClient;
	this->GetWindowRect(&rectClient);
	rectClient.right = rectClient.left + POS_DLG_WIDTH;
	rectClient.bottom = rectClient.top + POS_DLG_HEIGHT;
	this->MoveWindow(&rectClient);
	SetCircle();

	m_rectHead.left = 25;
	m_rectHead.right = m_rectHead.left + 80;
	m_rectHead.top = 55;
	m_rectHead.bottom = m_rectHead.top + 80;
	//m_sHeadResourceFile = theApp.GetAppPath()+_T("\\img\\myimg.png");
	//USES_CONVERSION;
	//if (!m_sHeadResourceFile.IsEmpty() && PathFileExists(m_sHeadResourceFile))
	//{
	//	m_imageHead = new Gdiplus::Image((const WCHAR*)A2W_ACP(m_sHeadResourceFile));   
	//}

	if (!m_bNewEemployee)
	{
		UpdateData(FALSE);
	}

	SetWindowPos(&CWnd::wndTopMost,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgMemberInfo::OnPaint()
{
	if (IsIconic())
	{
		//CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		//SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		//// ʹͼ���ڹ����������о���
		//int cxIcon = GetSystemMetrics(SM_CXICON);
		//int cyIcon = GetSystemMetrics(SM_CYICON);
		//CRect rect;
		//GetClientRect(&rect);
		//int x = (rect.Width() - cxIcon + 1) / 2;
		//int y = (rect.Height() - cyIcon + 1) / 2;

		//// ����ͼ��
		//dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		//CEbDialogBase::OnPaint();
		DrawInfo();
	}
}

void CDlgMemberInfo::DrawInfo(void)
{
	CPaintDC dc2(this); // ���ڻ��Ƶ��豸������
	CRect rectClient;
	this->GetClientRect(&rectClient);
	CSkinMemDC memDC(&dc2, rectClient);
	this->ClearBgDrawInfo();
	if (theApp.GetColorGradient())
		this->AddBgDrawInfo(CEbBackDrawInfo(theDefaultBgColorTitleHeight,theDefaultBgColorTitleLight1,true));
	else
		this->AddBgDrawInfo(CEbBackDrawInfo(theDefaultBgColorTitleHeight,1.0,false));
	this->AddBgDrawInfo(CEbBackDrawInfo(350,theDefaultBgColorBgLight1,false));
	this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorBgLight3,false));
	this->DrawPopBg(&memDC, theApp.GetMainColor());
	Gdiplus::Graphics graphics(memDC.m_hDC);
	//Gdiplus::Image * pImage = NULL;
	//if (libEbc::ImageFromIDResource(IDB_PNG_IMG1_PEOPLE,_T("png"),pImage))
	//{
	//	graphics.DrawImage(pImage,PointF(10,7));
	//	delete pImage;
	//}
	graphics.DrawImage(theApp.m_image1People,PointF(10,7));
	USES_CONVERSION;
	const FontFamily fontFamily(theFontFamily.c_str());

	// д����
	const Gdiplus::Font fontEbTitle(&fontFamily, 13, FontStyleBold, UnitPixel);
	Gdiplus::Color colorTitle;
	colorTitle.SetFromCOLORREF(theApp.GetTitleTextColor());
	Gdiplus::SolidBrush brushEbTitle(colorTitle);
	//Gdiplus::SolidBrush brushEbTitle(Gdiplus::Color(38,38,38));
	//graphics.DrawImage(theApp.m_imageEbIcon, Gdiplus::RectF(3,3,20,20));
	//const Gdiplus::PointF pointTitle(25,7);
	const Gdiplus::PointF pointTitle(40,13);
	if (m_nGroupType==EB_GROUP_TYPE_DEPARTMENT)
		graphics.DrawString(L"Ա������",-1,&fontEbTitle,pointTitle,&brushEbTitle);
	else
		graphics.DrawString(L"��Ա����",-1,&fontEbTitle,pointTitle,&brushEbTitle);

	const tstring sHeadResourceFile = m_pMemberInfo.m_sHeadResourceFile;
	if (!sHeadResourceFile.empty() && PathFileExists(sHeadResourceFile.c_str()))
	{
		if (m_nOldFileSize==0)
		{
			m_nOldFileSize = libEbc::GetFileSize(sHeadResourceFile.c_str());
		}
		Gdiplus::Image imageHead((const WCHAR*)A2W_ACP(sHeadResourceFile.c_str()));   
		graphics.DrawImage(&imageHead, m_rectHead.left, m_rectHead.top, m_rectHead.Width(), m_rectHead.Height());
	}else
	{
		Image * pImage = theApp.m_imageDefaultMember->Clone();;
		graphics.DrawImage(pImage, m_rectHead.left, m_rectHead.top, m_rectHead.Width(), m_rectHead.Height());
		delete pImage;
	}
	if (theApp.GetSendRegMail() && m_bNewEemployee)
	{
		Gdiplus::SolidBrush brushTip(Gdiplus::Color(255,0,128));
		graphics.DrawString(L"��ӳɹ����������֤�ʼ�����֤�����ʺţ�",-1,&fontEbTitle,PointF(310,63),&brushTip);
	}
	//if (m_imageHead!=NULL)
	//{
	//	graphics.DrawImage(m_imageHead, m_rectHead.left, m_rectHead.top, m_rectHead.Width(), m_rectHead.Height());
	//}

	//CPaintDC dc(this); // ���ڻ��Ƶ��豸������
	//this->DrawPopBg(&dc, theApp.GetMainColor());
	//CRect rectClient;
	//this->GetClientRect(&rectClient);
	//HDC m_hdcMemory = dc.m_hDC;

	//// д����
	//CFont pNewFontTitle;
	//pNewFontTitle.CreatePointFont(115, _T("����"));//������ʾ�ı�������
	//HGDIOBJ pOldFond = SelectObject(m_hdcMemory, pNewFontTitle.m_hObject);
	//::SetBkMode(m_hdcMemory, TRANSPARENT);
	//SetTextColor(m_hdcMemory, RGB(0, 0, 0));	// ��ɫ
	//CString sOutText = _T("Ⱥ��Ƭ");
	//TextOut(m_hdcMemory, 6, 5, sOutText, sOutText.GetLength());
	//SelectObject(m_hdcMemory, pOldFond);

}

void CDlgMemberInfo::OnSize(UINT nType, int cx, int cy)
{
	CEbDialogBase::OnSize(nType, cx, cy);

	int btnx = 0;
	btnx = cx-m_btnClose.GetImgWidth()-2;
	m_btnClose.MovePoint(btnx, 2);

	const int const_btn_width = 81;//m_btnCancel.GetImgWidth();// 60;
	const int const_btn_height = 32;//m_btnCancel.GetImgHeight();// 24;
	int x = cx-const_btn_width-10;					// ���ұ߼��
	const int const_btn_y = cy-const_btn_height-10;	// ��������
	m_btnCancel.MovePoint(x,const_btn_y,const_btn_width,const_btn_height);
	x -= (const_btn_width+10);						// ��ť���
	m_btnOk.MovePoint(x,const_btn_y,const_btn_width,const_btn_height);
	x = 20;
	m_btnDefaultEmp.MovePoint(x,const_btn_y,100,const_btn_height);
}

void CDlgMemberInfo::OnBnClickedButtonClose()
{
	PostMessage(WM_CLOSE, 0, 0);
}

void CDlgMemberInfo::OnBnClickedButtonDefaultEmp()
{
	bool bAlreadyMyDefaultEmp = false;
	if (m_sMemberCode>0)
	{
#ifdef USES_EBCOM_TEST
		const eb::bigint sDefaultMemberCode = theEBClientCore->EB_MyDefaultMemberCode;
#else
		eb::bigint sDefaultMemberCode;
		theEBAppClient.EB_GetMyDefaultMemberCode(sDefaultMemberCode);
#endif
		if (m_sMemberCode==sDefaultMemberCode)
		{
			bAlreadyMyDefaultEmp = true;
		}
	}

	if (bAlreadyMyDefaultEmp)
	{
		// ȡ��Ĭ����Ƭ
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_MyDefaultMemberCode = 0;
#else
		theEBAppClient.EB_SetMyDefaultMemberCode(0);
#endif
		m_btnDefaultEmp.SetWindowText(_T("��ΪĬ����Ƭ"));
	}else
	{

#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_MyDefaultMemberCode = m_sMemberCode;
#else
		theEBAppClient.EB_SetMyDefaultMemberCode(m_sMemberCode);
#endif
		m_btnDefaultEmp.SetWindowText(_T("ȡ��Ĭ����Ƭ"));
	}
}

void CDlgMemberInfo::OnDestroy()
{
	if (m_pDlgChangeHead)
	{
		m_pDlgChangeHead->DestroyWindow();
		delete m_pDlgChangeHead;
		m_pDlgChangeHead = NULL;
	}
	CEbDialogBase::OnDestroy();
	if (m_imageHead!=NULL)
	{
		delete m_imageHead;
		m_imageHead = NULL;
	}
}

void CDlgMemberInfo::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	//if (!m_pMemberInfo.m_sHeadResourceFile.empty())
	if (!m_bNewEemployee)
	{
		CPoint pos;
		GetCursorPos(&pos);
		ScreenToClient(&pos);
		if (m_rectHead.PtInRect(pos))
		{
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_HAND));
			return;
		}
	}
	CEbDialogBase::OnMouseMove(nFlags, point);
}

void CDlgMemberInfo::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (!m_bNewEemployee)
	{
		ReleaseCapture();
		CPoint pos;
		GetCursorPos(&pos);
		ScreenToClient(&pos);
		if (m_rectHead.PtInRect(pos))
		{
			OnBnClickedButtonChangehead();
		}
	}
	CEbDialogBase::OnLButtonUp(nFlags, point);
}

void CDlgMemberInfo::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (!m_bNewEemployee)
	{
		CPoint pos;
		GetCursorPos(&pos);
		ScreenToClient(&pos);
		if (m_rectHead.PtInRect(pos))
		{
			this->SetCapture();
		}
	}
	CEbDialogBase::OnLButtonDown(nFlags, point);
}

void CDlgMemberInfo::OnTimer(UINT_PTR nIDEvent)
{
	if (TIMERID_CHECK_RESOURCEFILE == nIDEvent)
	{
		//if (m_pMemberInfo.get()!=NULL)
		{
			const tstring sHeadResourceFile = m_pMemberInfo.m_sHeadResourceFile;
			if (!sHeadResourceFile.empty() && PathFileExists(sHeadResourceFile.c_str()))
			{
				unsigned int nFileSize = libEbc::GetFileSize(sHeadResourceFile.c_str());
				if (nFileSize>0 && nFileSize!=m_nOldFileSize)
				{
					m_nOldFileSize = nFileSize;
					this->Invalidate();
				}
			}
		}
	}
	CEbDialogBase::OnTimer(nIDEvent);
}

void CDlgMemberInfo::OnBnClickedButtonChangehead()
{
	//if (m_pMemberInfo.m_sHeadResourceFile..get()!=NULL)
	if (!m_bNewEemployee)
	{
		if (!m_bSetTimer)
		{
			m_bSetTimer = true;
			SetTimer(TIMERID_CHECK_RESOURCEFILE,6*1000,NULL);
		}
		if (m_pDlgChangeHead==NULL)
		{
			AFX_SWITCH_INSTANCED();
			m_pDlgChangeHead = new CDlgChangeHead(this);
			m_pDlgChangeHead->m_sGroupCode = this->m_sGroupCode;
			m_pDlgChangeHead->m_sHeadResourceFile = m_pMemberInfo.m_sHeadResourceFile;
			m_pDlgChangeHead->Create(CDlgChangeHead::IDD,this);
			m_pDlgChangeHead->SetCallback(this);	// **������������
		}
		m_pDlgChangeHead->ShowWindow(SW_SHOW);
	}
}

void CDlgMemberInfo::OnSelectedImageInfo(const CEBImageDrawInfo& pSelectedImageInfo)
{
#ifdef USES_EBCOM_TEST
	int ret = theEBClientCore->EB_SetMyGroupHeadRes(m_sGroupCode,pSelectedImageInfo.m_sResId);
#else
	int ret = theEBAppClient.EB_SetMyGroupHeadRes(m_sGroupCode,pSelectedImageInfo.m_sResId);
#endif
	if (ret == 0)
	{
		m_pDlgChangeHead->ShowWindow(SW_HIDE);
		m_pMemberInfo.m_sHeadResourceFile = pSelectedImageInfo.m_sResFile;
		m_nOldFileSize = libEbc::GetFileSize(m_pMemberInfo.m_sHeadResourceFile.c_str());
		this->Invalidate();
	}
}

BOOL CDlgMemberInfo::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam==' ')
	{
		// ���Ʋ�������ո�
		if (pMsg->hwnd == m_editMemberAccount.GetSafeHwnd())
		{
			return TRUE;
		}
	}

	return __super::PreTranslateMessage(pMsg);
}
