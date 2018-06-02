
// GameTimeManagerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GameTimeManager.h"
#include "GameTimeManagerDlg.h"
#include "afxdialogex.h"
#include <iostream>
#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	CRichEditCtrl m_ctrlVersionBoard;
	afx_msg void OnBnClickedButton1();
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RICHEDIT21, m_ctrlVersionBoard);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CAboutDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CGameTimeManagerDlg dialog



CGameTimeManagerDlg::CGameTimeManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_GAMETIMEMANAGER_DIALOG, pParent)
	, m_nMinutes(0)
	, m_bPlaying(false)
	, m_strLogFile(_T(""))
	, m_strSelfPath(_T(""))
	, m_strPasscode(_T(""))
	, m_bDebugBox(FALSE)
	, m_bDebug1(FALSE)
	, m_strConfigFile(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nMinBalance = LoadSettings("m_nMinBalance");
	m_nTotalMinPlayed = LoadSettings("m_nTotalMinPlayed");
	m_nTotalAdded = LoadSettings("m_nTotalAdded");
}

void CGameTimeManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDITminutes, m_nMinutes);
	DDV_MinMaxInt(pDX, m_nMinutes, 10, 99);
	DDX_Control(pDX, IDC_RICHEDIT21, m_ctrlBoard);
	DDX_Text(pDX, IDC_EDITPASSCODE, m_strPasscode);
}

BEGIN_MESSAGE_MAP(CGameTimeManagerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_EDITminutes, &CGameTimeManagerDlg::OnEnChangeEdit1)
	ON_BN_CLICKED(IDStart, &CGameTimeManagerDlg::OnBnClickedStart)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOKPause, &CGameTimeManagerDlg::OnBnClickedOkpause)
	ON_BN_CLICKED(IDC_BN_HISTORYLOG, &CGameTimeManagerDlg::OnBnClickedBnHistorylog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BNadd, &CGameTimeManagerDlg::OnBnClickedBnadd)
	ON_STN_CLICKED(IDC_STATICDisplayMinutes, &CGameTimeManagerDlg::OnStnClickedStaticdisplayminutes)
END_MESSAGE_MAP()


// CGameTimeManagerDlg message handlers

BOOL CGameTimeManagerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	//----------------------------------------
	BOOL bEnable = FALSE;     // TRUE to enable and FALSE to disable

	UINT nMenuf = bEnable ? (MF_BYCOMMAND) : (MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);

	CMenu* pSysMenu1 = GetSystemMenu(FALSE);
	if (pSysMenu1)
	{
		pSysMenu1->EnableMenuItem(SC_CLOSE, nMenuf);
	}
	//----------------------------------------------------------
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	// set windows on top
	char buf[512];
	int actualLength = GetModuleFileName(NULL, buf, 512);
	if (actualLength < 512) {
		CString selfpath;
		selfpath = buf;
		m_strSelfPath = selfpath.Mid(0, selfpath.ReverseFind('\\')+1);
		m_strLogFile = GetLogFileNameWithPath();
		m_strConfigFile = GetConfigFileNameWithPath();
	}

	this->SetTimer(1, 1000*60, NULL);    //one timer messaeg per minute
	ShowBalance();
	ShowTimePlayed();
	LogStartted();
	if (m_nMinBalance == 0) {
		((CWnd*)GetDlgItem(IDStart))->EnableWindow(false);
	}
	// SaveStringSettings("passcodeHistory", "");
	// m_bDebugBox = TRUE;
	// m_bDebug1 = TRUE;
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CGameTimeManagerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CGameTimeManagerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CGameTimeManagerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

#include <process.h>
#include <Tlhelp32.h>
#include <winbase.h>
#include <string.h>
#include <iostream>
#include <sstream>
/*
const wchar_t *GetWC(const char *c) {
	const size_t cSize = strlen(c) + 1;
	std::wstring wc(cSize, L'#');
	mbstowcs(&wc[0], c, cSize);
	return wc.data();
}
*/
namespace std
{
	template < typename T > std::string to_string(const T& n)
	{
		std::ostringstream stm;
		stm << n;
		return stm.str();
	}
}

bool killProcessByName(const char *filename)
{
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
	PROCESSENTRY32 pEntry;
	pEntry.dwSize = sizeof(pEntry);
	BOOL hRes = Process32First(hSnapShot, &pEntry);
	while (hRes)
	{
		std::string strExefile = pEntry.szExeFile;
		//::MessageBox(NULL, strExefile.c_str(), "processname", MB_OK);
		if (strExefile.find(filename) != std::string::npos)
		{
			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
				(DWORD)pEntry.th32ProcessID);
			if (hProcess != NULL)
			{
				TerminateProcess(hProcess, 9);
				CloseHandle(hProcess);
				CloseHandle(hSnapShot);
				return true;
			}
		}
		hRes = Process32Next(hSnapShot, &pEntry);
	}
	CloseHandle(hSnapShot);
	return false;
}

void CGameTimeManagerDlg::setOnTop(bool bOnTop)
{
	if (bOnTop)
	   // set windows on top
	   SetWindowPos(&this->wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	else
	   SetWindowPos(&this->wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

void CGameTimeManagerDlg::OnEnChangeEdit1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}
#include <iostream>
#include <fstream>
#include "afxcmn.h"

// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
std::string currentDateTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
	// for more information about date/time format
	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tstruct);

	return buf;
}
void CGameTimeManagerDlg::OnBnClickedStart()
{
	// TODO: Add your control notification handler code here
	setOnTop(false);
	((CWnd*)GetDlgItem(IDStart))->EnableWindow(false);
	((CWnd*)GetDlgItem(IDOKPause))->EnableWindow(true);
	((CWnd*)GetDlgItem(IDC_EDITminutes))->EnableWindow(false);
	((CWnd*)GetDlgItem(IDC_BNadd))->EnableWindow(false);
	((CWnd*)GetDlgItem(IDC_STATICStatus))->SetWindowText(_T("Playing"));
	m_bPlaying = true;
	UpdateData(false);
	LogPaused(2);
}

void CGameTimeManagerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == 1){
		// handle timer event
		resetTotalPlayedMinutesOnMidnight();
		if ((m_nMinBalance <= 0) || (!m_bPlaying)) {    //timeout or paused
			if (!killProcessByName("RobloxPlayer")) {
				killProcessByName("MicrosoftEdge.exe");   //kill this only if no RobloxPlayer is running
				
			}
			//kill all that is in config.cfg file
			std::ifstream in(m_strConfigFile);
			std::string str;
			while (std::getline(in, str)) {
				// output the line
				killProcessByName(str.c_str());
			}
			if (m_bPlaying) {                        //if timeout
				((CWnd*)GetDlgItem(IDC_STATICMIN))->SetWindowText(_T("0"));
			}
			m_bPlaying = false;
			if (m_nMinBalance <= 0) {
				((CWnd*)GetDlgItem(IDStart))->EnableWindow(false);
				((CWnd*)GetDlgItem(IDC_EDITminutes))->EnableWindow(true);
				((CWnd*)GetDlgItem(IDC_STATICStatus))->SetWindowText(_T("Stopped"));
				((CWnd*)GetDlgItem(IDOKPause))->EnableWindow(false);
				((CWnd*)GetDlgItem(IDC_BNadd))->EnableWindow(true);
				m_nMinBalance = 0;
			}
		}
		else if ((m_nMinBalance==1)&&(m_bPlaying)) {  //1 min grace time
			setOnTop(true);
			m_nMinBalance--;
			m_nTotalMinPlayed++;
			((CWnd*)GetDlgItem(IDOKPause))->EnableWindow(true);
			if (m_nMinBalance == 0) {
				LogPaused();
			}
			SaveSettings("m_nMinBalance", m_nMinBalance);
			SaveSettings("m_nTotalMinPlayed", m_nTotalMinPlayed);
			ShowBalance();
			ShowTimePlayed();
		}
		else {    //playing time positive and is playing, deduct time
			m_nMinBalance--;
			m_nTotalMinPlayed++;
			ShowBalance();
			ShowTimePlayed();
			SaveSettings("m_nMinBalance", m_nMinBalance);
			SaveSettings("m_nTotalMinPlayed", m_nTotalMinPlayed);
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CGameTimeManagerDlg::OnBnClickedOkpause()
{
	// TODO: Add your control notification handler code here
	m_bPlaying = !m_bPlaying;
	if (m_bPlaying) {
		setOnTop(false);
		((CWnd*)GetDlgItem(IDC_STATICStatus))->SetWindowText(_T("Playing"));
		((CWnd*)GetDlgItem(IDOKPause))->SetWindowText(_T("Pause"));
		((CWnd*)GetDlgItem(IDC_EDITminutes))->EnableWindow(false);
		((CWnd*)GetDlgItem(IDC_BNadd))->EnableWindow(false);
		((CWnd*)GetDlgItem(IDStart))->EnableWindow(false);
		LogResumed();
	}
	else {
		setOnTop(true);
		((CWnd*)GetDlgItem(IDC_STATICStatus))->SetWindowText(_T("Stopped"));
		((CWnd*)GetDlgItem(IDOKPause))->SetWindowText(_T("Resume"));
		((CWnd*)GetDlgItem(IDC_EDITminutes))->EnableWindow(true);
		((CWnd*)GetDlgItem(IDC_BNadd))->EnableWindow(true);
		LogPaused();
	}
}


void CGameTimeManagerDlg::OnBnClickedBnHistorylog()
{
	// TODO: Add your control notification handler code here
	ShellExecute(0, 0, m_strLogFile, 0, 0, SW_SHOW);
}


BOOL CAboutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_ctrlVersionBoard.SetWindowText("Version History:\nVersion 1.1 first version\nVersion 2.0: added Usage Board, Single instance, SelfPath Log\n"
		"Version 2.1: added pause log\n"
		"version 2.2: Fixed the string error for process name\n"
		"version 2.3: Added MinimizeBox\n"
		"version 2.4: Disabled the x button\n"
		"version 2.5: Disabled the close window menu from task bar (2018.4.2)\n"
		"version 2.6: Added game over event recording (2018.4.6)\n"
		"version 3.0: Added passcode for adding time (2018.4.11)\n"
		"version 3.2: Fixed code can be reused problem (2018.5.25)\n"
		"version 3.3: Added Config.cfg (2018.5.31)\n"
		);
	return TRUE;
}

void CAboutDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	char buf[512];
	CString LogFile;
	int actualLength = GetModuleFileName(NULL, buf, 512);
	if (actualLength < 512) {
		CString selfpath;
		selfpath = buf;
		time_t     now = time(0);
		struct tm  tstruct;
		char       buf1[80];
		tstruct = *localtime(&now);
		strftime(buf1, sizeof(buf), "GameTimeMgr_%Y-%m-%d.log", &tstruct);
		LogFile = selfpath.Mid(0, selfpath.ReverseFind('\\') + 1) + buf1;
	}
	ShellExecute(0, 0, LogFile, 0, 0, SW_SHOW);
}


void CGameTimeManagerDlg::LogPaused(int flag)
{
	std::string newmsg;
	if (flag ==0 ) {
		if (m_nMinBalance == 0) {
			newmsg = ":Game Over: 0 minutes left\n";
		}
		else {
			newmsg = ": Paused, Time left: " + std::to_string(m_nMinBalance) + " minutes ";
			if (m_nMinBalance > 60)
				newmsg += "(" + std::to_string(m_nMinBalance / 60) + " Hours " + std::to_string(m_nMinBalance % 60) + " Minutes)\n";
			else
				newmsg += "\n";
		}
	}
	else if (flag == 1) {
		newmsg = ": GameTimeManager Started (Total Minutes Added: " + std::to_string(m_nTotalAdded / 60) + " Hours " + std::to_string(m_nTotalAdded % 60) + " Minutes)\n";
	}
	else if (flag == 2) {
		newmsg = ": Game Started\n";
	}
	else if (flag == 3) {
		newmsg = ": Resumed\n";
	}
	else if (flag == 4) {
		newmsg = ": midnight, Totol time played today: "+ std::to_string(m_nTotalMinPlayed / 60) + " Hours " + std::to_string(m_nTotalMinPlayed % 60) + " Minutes)\n";
	}
	logAndShowMessage(newmsg);
}


void CGameTimeManagerDlg::LogResumed()
{
	LogPaused(3);
}


void CGameTimeManagerDlg::logAndShowMessage(std::string newmsg1)
{
	std::string newmsg = currentDateTime() + newmsg1;
	std::ofstream myfile;
	try {
		myfile.open(m_strLogFile, std::ofstream::out | std::ofstream::app);
		char msg[10240];
		int lengthFilled = m_ctrlBoard.GetWindowText(msg, 10240);

		std::string boardmsg;
		boardmsg = msg + newmsg;
		m_ctrlBoard.SetWindowText(boardmsg.c_str());
		myfile << newmsg;
		myfile.close();
	}
	catch (const std::ofstream::failure& e) {
		std::cout << "Exception III opening/reading file";
		MessageBox(_T("Can't open log file"));
	}
}


void CGameTimeManagerDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	//printf("closing...\n");
	//CDialogEx::OnClose();  close window menu from task bar will hit here, do not call, do not close, disable closing
}


void CGameTimeManagerDlg::OnBnClickedBnadd()
{
	// TODO: Add your control notification handler code here
	if (!UpdateData(1)) return;  //valid the number
	debugbox("d1");
	if (passcodeInvalid()) return;
	setOnTop(false);
	m_nMinBalance += m_nMinutes;
	m_nTotalAdded += m_nMinutes;
	
	SaveSettings("m_nMinBalance", m_nMinBalance);
	SaveSettings("m_nTotalAdded", m_nTotalAdded);
	//update the minute balance
	ShowBalance();
	
	//log the event to log file and one screen log window
	CString strTotalMin,strMinutesPlayed;
	strTotalMin.Format(_T("%d"), m_nTotalAdded);
	std::string newmsg = ": Added " + std::to_string(m_nMinutes) + " minutes ";
	if (m_nTotalAdded > 0) {
		newmsg += ", Total added: " + strTotalMin + " minutes";
	}
	if (m_nTotalAdded > 60)
		newmsg += "(" + std::to_string(m_nTotalAdded / 60) + " hours " + std::to_string(m_nTotalAdded % 60) + " min)\n";
	else
		newmsg += "\n";
	logAndShowMessage(newmsg);
	ShowTimePlayed();
	m_nMinutes = 0;
	UpdateData(false);
	if(m_nMinBalance>0)
		((CWnd*)GetDlgItem(IDStart))->EnableWindow(true);
}


int CGameTimeManagerDlg::LoadSettings(char * strKey)
{
	CRegKey reg;
	HKEY m_hkeyParent = HKEY_CURRENT_USER;
	LPCTSTR m_myKey = "Software\\GameTimeManager";
	DWORD dvalue;
	LONG nRes = reg.Open(m_hkeyParent, m_myKey, KEY_READ);
	if (nRes == ERROR_SUCCESS)
	{
		LONG nRes=reg.QueryDWORDValue(strKey, dvalue);
		reg.Close();
		if (nRes == 2)
			return 0;
	}
	else
	{
		LPVOID lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			nRes,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR)&lpMsgBuf,
			0,
			NULL
			);
		//::MessageBox(NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION);
		LocalFree(lpMsgBuf);
		return 0;
	}
	return dvalue;
}


void CGameTimeManagerDlg::SaveSettings(char * settingName, int value)
{
	CRegKey reg;

	HKEY m_hKeyParent = HKEY_CURRENT_USER;

	LPCTSTR m_myKey = "Software\\GameTimeManager";

	if (reg.Create(m_hKeyParent, m_myKey) == ERROR_SUCCESS)
	{
		reg.SetDWORDValue(settingName, value);
		reg.Close();
	}
}


// Show balance in minutes
void CGameTimeManagerDlg::ShowBalance()
{
	//update the minute balance
	CString strMin;
	strMin.Format(_T("%d minutes"), m_nMinBalance);
	if (m_nMinBalance > 60) {
		strMin.Format(_T("%d minutes  ( %d hours %d minutes )"), m_nMinBalance, m_nMinBalance / 60, m_nMinBalance % 60);
	}
	((CWnd*)GetDlgItem(IDC_STATICDisplayMinutes))->SetWindowText(_T("Minutes Available:"));    //balance inminutes
	((CWnd*)GetDlgItem(IDC_STATICMIN))->SetWindowText(strMin);	
	UpdateData(false);
}


void CGameTimeManagerDlg::ShowTimePlayed()
{
	//update the minute balance
	CString strMin;
	strMin.Format(_T("%d minutes"), m_nTotalMinPlayed);
	if (m_nTotalMinPlayed > 60) {
		strMin.Format(_T("%d minutes  ( %d hours %d minutes )"), m_nTotalMinPlayed, m_nTotalMinPlayed / 60, m_nTotalMinPlayed % 60);
	}
	((CWnd*)GetDlgItem(IDC_STATICTotal))->SetWindowText(_T("Minutes Played:"));    //balance inminutes
	((CWnd*)GetDlgItem(IDC_STATICnMin))->SetWindowText(strMin);
	UpdateData(false);
}


void CGameTimeManagerDlg::OnStnClickedStaticdisplayminutes()
{
	// TODO: Add your control notification handler code here
}


void CGameTimeManagerDlg::LogStartted()
{
	LogPaused(1);
}


void CGameTimeManagerDlg::resetTotalPlayedMinutesOnMidnight()
{
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[10];
	tstruct = *localtime(&now);
	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
	// for more information about date/time format
	strftime(buf, sizeof(buf), "%H:%M", &tstruct);
	buf[5] = 0;
	if (strcmp(buf, "00:00") == 0) {  //midnight, reset minutes played and also reset the log file name which changes daily
		LogPaused(4);
		m_nTotalMinPlayed = 0;
		m_strLogFile = GetLogFileNameWithPath();
		SaveStringSettings("passcodeHistory","NEWDAY");
	}
}


CString CGameTimeManagerDlg::GetLogFileNameWithPath()
{
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "GameTimeMgr_%Y-%m-%d.log", &tstruct);
	return CString(m_strSelfPath+buf);
}
CString CGameTimeManagerDlg::GetConfigFileNameWithPath()
{
	return CString(m_strSelfPath + "Config.cfg");
}

#include <ctime>
#include <time.h>
#include <string>
using namespace std;
char table[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
int getIndex(char c) {
	int len = strlen(table);
	for (int i = 0; i < len; i++) {
		if (table[i] == c) {
			return i;
		}
	}
	return -1;
}
string decode(const char * buf) {
	int len = strlen(table);
	char converted[10];
	char b = buf[8];     //last char was base index
						 //cout << "b=" << b << endl;
	int index = getIndex(b);   //get index which is base + index
							   //cout << "index=" << index << endl;
	int i = 0;
	for (i = 0; i < 8; i++) {
		char c = buf[i];
		//cout << "char" << i << ": " << c << endl;
		int bindex = getIndex(c);
		int asc = bindex - index;
		char ch = asc + 0x30;
		converted[i] = ch;
	}
	converted[i] = 0;
	return converted;
}

BOOL CGameTimeManagerDlg::passcodeInvalid()
{
	UpdateData();
	
	int len = m_strPasscode.GetLength();
	if (len != 9) {
		::MessageBox(NULL, "Passcode len is invalid !", "Passcode check 1", MB_OK);
		return 1;
	}
	debugbox("d2");
	if (passcodeUsedBefore()) {
		::MessageBox(NULL, "The passcode has been used before !", "Passcode check 2", MB_OK);
		return 1;
	}
	debugbox("d3");
	string decoded = decode(m_strPasscode);
	debugbox("d4");
	string validcode = getValidCode();
	debugbox("d5");
	if (decoded == validcode) {
		savePasscode();
		debugbox("d6");
		return 0;
	}
	debugbox("d7");
	::MessageBox(NULL, "Passcode is invalid !", "Passcode check", MB_OK);
	return 1;
}

string getTimestamp() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[10];
	tstruct = *localtime(&now);	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime	// for more information about date/time format
	strftime(buf, sizeof(buf), "%y%m%d", &tstruct);
	return string(buf);
}
string CGameTimeManagerDlg::getValidCode()
{
	UpdateData();
	string mn = std::to_string(m_nMinutes);
	string tm = getTimestamp();
	string validcode = mn + tm;
	return validcode;
}


BOOL CGameTimeManagerDlg::passcodeUsedBefore()
{
	char k[] = "passcodeHistory";   //This is the fix for version 3.2, the was hardcoded and not working
	string passcodeHistory = LoadStringSettings(k);
	string substring = string(m_strPasscode);
	if (passcodeHistory.length() > 0) {
		//if (strstr(passcodeHistory.c_str(), substring.c_str())) {
		if(passcodeHistory.find(substring)!=std::string::npos){
		    return true;
		}
	}
	debugbox("d25");
	return false;
}

void showErrorMessage(LONG nRes) {
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		nRes,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR)&lpMsgBuf,
		0,
		NULL
		);
	//::MessageBox(NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION);
	LocalFree(lpMsgBuf);
}
std::string CGameTimeManagerDlg::LoadStringSettings(char * strKey)
{
	debugbox("d201");
	CRegKey reg;
	debugbox("d202");
	HKEY m_hkeyParent = HKEY_CURRENT_USER;
	LPCTSTR m_myKey = "Software\\GameTimeManager";
	char szValue[4096];
	debugBox1("d203");
	LONG nRes = reg.Open(m_hkeyParent, m_myKey, KEY_READ);
	CString strRet;
	strRet.Format("d204a: nRes=%d, from reg.Open(Software\\GameTi...", nRes);
	debugBox1((char *)(LPCTSTR)strRet);
	debugBox1("d204");
	if (nRes == ERROR_SUCCESS)
	{
		debugBox1("d205");
		ULONG sz;
		LONG nRes = reg.QueryStringValue(strKey, szValue, &sz);
		debugBox1(szValue);
		debugBox1("d206");
		reg.Close();
		debugBox1("d207");
		if (nRes == 2)
		{
			debugBox1("d208");
			showErrorMessage(nRes);
			debugBox1("d209");
			return "";
		}
		debugBox1("d20A");
	}
	else
	{
		debugBox1("d20B");
		showErrorMessage(nRes);
		debugBox1("d20C");
		return "";
	}
	debugBox1("d20E");
	return std::string(szValue);
}


void CGameTimeManagerDlg::SaveStringSettings(char * settingName, char * strValue)
{
	CRegKey reg;

	HKEY m_hKeyParent = HKEY_CURRENT_USER;

	LPCTSTR m_myKey = "Software\\GameTimeManager";

	if (reg.Create(m_hKeyParent, m_myKey) == ERROR_SUCCESS)
	{
		reg.SetStringValue(settingName, strValue);
		reg.Close();
	}
	reg.Close();
}

void CGameTimeManagerDlg::savePasscode()
{
	//string passcodeHistory = LoadStringSettings("passcodeHistory");
	char k[] = "passcodeHistory";   //This is the fix for version 3.2, the was hardcoded and not working
	string passcodeHistory = LoadStringSettings(k);
	int len = passcodeHistory.length();
	if (len < 2) {
		SaveStringSettings(k, (char *)((string(m_strPasscode)).c_str()));
	}
	else {
		passcodeHistory = passcodeHistory + ";" + string(m_strPasscode);
		SaveStringSettings(k, (char *)passcodeHistory.c_str());
	}
}


void CGameTimeManagerDlg::debugbox(char * msg)
{
	if(m_bDebugBox)
	     ::MessageBox(NULL, msg, "Yali debug info", MB_OK);
}


void CGameTimeManagerDlg::debugBox1(char * msg)
{
	if (m_bDebug1)
		::MessageBox(NULL, msg, "Yali debug info", MB_OK);
}


