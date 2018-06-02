
// GameTimeManagerDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include <string>
// CGameTimeManagerDlg dialog
class CGameTimeManagerDlg : public CDialogEx
{
// Construction
public:
	CGameTimeManagerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GAMETIMEMANAGER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	void setOnTop(bool bOnTop);
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedStart();
	int m_nMinutes;   //minutes in input box to be added
	int m_nMinBalance;  //minutes balance
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedOkpause();
	bool m_bPlaying;
	CRichEditCtrl m_ctrlBoard;
	CString m_strLogFile;
	afx_msg void OnBnClickedBnHistorylog();
	void LogPaused(int flag=0);
	void LogResumed();
	void logAndShowMessage(std::string newmsg);
	afx_msg void OnClose();
	afx_msg void OnBnClickedBnadd();
	int LoadSettings(char * strKey);
	void SaveSettings(char * settingName, int value);
	// Total minutes played today. For daily statistic report
	int m_nTotalMinPlayed;
	// Show balance in minutes
	void ShowBalance();
	void ShowTimePlayed();
	afx_msg void OnStnClickedStaticdisplayminutes();
	void LogStartted();
	// Total Minutes Added / Purchased
	int m_nTotalAdded;
	void resetTotalPlayedMinutesOnMidnight();
	// //self path ended with a backslash
	CString m_strSelfPath;
	CString GetLogFileNameWithPath();
	CString m_strPasscode;
	BOOL passcodeInvalid();
	std::string getValidCode();
	BOOL passcodeUsedBefore();
	std::string LoadStringSettings(char * strKey);
	void SaveStringSettings(char * settingName, char * strValue);
	void savePasscode();
	void debugbox(char * msg);
	BOOL m_bDebugBox;
	// for debuging loadstringsettings
	BOOL m_bDebug1;
	void debugBox1(char * msg);
	CString m_strConfigFile;
	CString GetConfigFileNameWithPath();
};
