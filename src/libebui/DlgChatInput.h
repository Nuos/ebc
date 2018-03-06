#pragma once
#include "afxcmn.h"
//#include "ExtRichEdit.h"
//#include "../include/pop_richmsg.h"
//#include "DlgEmotionSelect.h"
#include "afxwin.h"
//#include "EBRichMessageEventsSink.h"

//#define WM_EB_EXIT_CHAT (WM_USER+100)

// CDlgChatInput dialog
class CDlgChatInput : public CEbDialogBase
	, public CImageSelectCallback
	, public CEBRichMessageEventsSink
{
	DECLARE_DYNAMIC(CDlgChatInput)

public:
	typedef boost::shared_ptr<CDlgChatInput> pointer;
	CDlgChatInput(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgChatInput();
	static CDlgChatInput::pointer create(CWnd * pParent = NULL)
	{
		return CDlgChatInput::pointer(new CDlgChatInput(pParent));
	}
	//enum OUT_TYPE
	//{
	//	OT_PUBLIC
	//	, OT_SLEF
	//};

	void SetCallInfo(const EB_CallInfo& pCallInfo, const EB_AccountInfo& pFromAccountInfo);
	void SetCtrlColor(void);

	void ScrollToEnd(void);
	void SetScreenCopyFinished(void);	// Ctrl+V
	void SendFile(const char* lpszFilePath, bool bDropFile);
	void AddLineString(eb::bigint nMsgId,const CString& sText, int nAlignmentFormat=0);
#ifdef USES_EBCOM_TEST
	void OnUserEmpInfo(IEB_MemberInfo* pMemberInfo);
	void SelectedEmp(IEB_MemberInfo* pMemberInfo);
	void OnSentFile(IEB_ChatFileInfo* pCrFileInfo);
	void OnReceivedFile(IEB_ChatFileInfo* pCrFileInfo);
#else
	void OnUserEmpInfo(const EB_MemberInfo* pMemberInfo);
	void SelectedEmp(const EB_MemberInfo* pMemberInfo);
	void OnSentFile(const CCrFileInfo * pCrFileInfo);
	void OnReceivedFile(const CCrFileInfo * pCrFileInfo);
#endif
	void SelectedEmp(eb::bigint nUserId);

	//void AddMessage(OUT_TYPE nOutType, const CString & sText, BOOL bAddTimestamp = FALSE);
	void OnUserEnter(eb::bigint sFromAccount, const tstring& sFromInfo);
	void OnUserExit(eb::bigint sFromAccount);
#ifdef USES_EBCOM_TEST
	void OnReceiveRich(IEB_ChatRichInfo* pCrMsgInfo, CString* sOutFirstMsg);
	void OnSendRich(IEB_ChatRichInfo* pCrMsgInfo,EB_STATE_CODE nState,CString* sOutFirstMsg);
#else
	void OnReceiveRich(const CCrRichInfo* pCrMsgInfo,CString* sOutFirstMsg);
	void OnSendRich(const CCrRichInfo* pCrMsgInfo,EB_STATE_CODE nState,CString* sOutFirstMsg);
#endif
	void OnAlertingCall(void);

	//void ReceiveMsg(CChatMsgInfo::pointer pChatMsgInfo);
	//CEBChatInfo::pointer m_pChatInfo;
	//Cchatroom::pointer m_pChatRoom;
	//bool m_bReceiveOffLineMsg;
	//CEBCCallInfo::pointer m_pCallInfo;
	static void CHARFORMAT2CR(const CHARFORMAT& cf,CR_CHARFORMATA& pOutCrcf);
	//static CString CharFormat2Html(const CR_CHARFORMATA & cf, const char * lpszText);

	void MoveSize(int cx=0, int cy=0);

// Dialog Data
	enum { IDD = IDD_DLG_CHATINPUT };

protected:
	CMenu m_pSendTypeSwitchMenu;
	EB_CallInfo m_pCallInfo;
	EB_AccountInfo m_pFromAccountInfo;
	//CDlgEmotionSelect* m_pDlgEmotionSelect;
	//bool m_bMustInviteUser;	// ��������ߣ���һ�η�����Ϣ���������û����Ự
	CSplitterControl m_wndSplitter1;
	//CSplitterControl m_wndSplitter2;
	CLockMap<eb::bigint,CTreeItemInfo::pointer> m_pSendToAccount;	// uid->index

	std::vector<eb::bigint> m_pDeletMsgId;
	virtual void Fire_onContextMenuDelete(LONGLONG nMsgId,LONG nIndex);
	virtual void Fire_onContextMenuSelect(ULONG nItemData, const CEBString& sSelectString,LONGLONG nSelectMsgId);
	virtual void Fire_onItemLBtnClick(ULONG nItemData, const CEBString& sParamString, LONGLONG nSelectMsgId);
	//virtual void Fire_onItemLBtnDblClick(ULONG nItemData, const CEBString& sParamString, LONGLONG nSelectMsgId);
	virtual void OnSelectedImageInfo(const CEBImageDrawInfo& pSelectedImageInfo);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CTraButton m_btnImage;
	CTraButton m_btnEBSC;
	//CTraButton m_btnFileManager;
	CTraButton m_btnMsgRecord;
	CTraButton m_btnSendTest;
	CTraButton m_btnClose;
	CTraButton m_btnSend;
	CTraButton m_btnSwitchReturn;
	//CMrFrame m_pMrFrame;
	CWnd m_pMrFrameControl;
	CComPtr<IEBRichMessage> m_pMrFrameInterface;

	CBitComboBox m_comboSendTo;
	CLabelEx m_checkPrivate;
	//CxSkinButton m_btnFont;
	COleRichEditCtrl m_richInput;

	void GetInputTextRange(int nFirst, int nLast, CString& refString) const;
	void FormatMsg(EB_ChatRoomRichMsg* pOutMsgFormat);
	void save();
	void send();

	//void ShowImageWindow(bool bShow);

#ifdef USES_EBCOM_TEST
	void ProcessFile(bool bReceive,IEB_ChatFileInfo* pCrFileInfo);
	void ProcessMsg(bool bReceive,IEB_ChatRichInfo* pCrMsgInfo,CString* sOutFirstMsg=NULL,EB_STATE_CODE nState=EB_STATE_OK);
#else
	void ProcessFile(bool bReceive,const CCrFileInfo * pCrFileInfo);
	void ProcessMsg(bool bReceive,const CCrRichInfo* pCrMsgInfo,CString* sOutFirstMsg=NULL,EB_STATE_CODE nState=EB_STATE_OK);
#endif
	int GetTimeLength(const char* lpszWavFilePath);
	void WriteFileHICON(const char* lpszFilePath);
	void DoResize(UINT nID,int delta);

	//bool m_bFirstMsg;
	eb::bigint m_nLastMsgId;
	eb::bigint m_nFromUid;
	eb::bigint m_nToUid;
	time_t	m_tLastMsgTime;
	CTime m_tLastMsgDayTime;
	void WriteMsgDate(time_t tMsgTime);
	void WriteTitle(eb::bigint nMsgId,bool bPrivate,eb::bigint nFromUid,const tstring& sFromName,eb::bigint nToUid,const tstring& sToName,time_t tMsgTime);
	void LoadMsgRecord(void);

	DECLARE_MESSAGE_MAP()
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnMsgfilterRich(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRichEditExLink( NMHDR* in_pNotifyHeader, LRESULT* out_pResult ) ;
	afx_msg void OnRichEditProtected(NMHDR* pNMHDR, LRESULT* pResult);

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButtonSend();
	afx_msg void OnBnClickedButtonFont();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnBnClickedButtonFileManager();
	afx_msg void OnBnClickedButtonMsgRecord();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	LRESULT OnDropFile(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonEbsc();
	afx_msg void OnBnClickedButtonImage();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBnClickedButtonSwitchReturn();
	afx_msg void OnCmdSendTypeReturn(void);
	afx_msg void OnCmdSendTypeCtrlReturn(void);
	afx_msg void OnCmdSend100Test(void);
	afx_msg void OnBnClickedButtonSendTest();
};
