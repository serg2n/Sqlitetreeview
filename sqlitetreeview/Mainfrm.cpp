/////////////////////////////
// Mainfrm.cpp
//

#include "stdafx.h"
#include "Mainfrm.h"
#include "resource.h"

#include <io.h>



using namespace std;

///////////////////////////////////
// CMainFrame function definitions.
//
CMainFrame::CMainFrame() : m_pDockText(nullptr), m_pDockTree(nullptr), m_pDockList(nullptr)
{
    m_pDockHexList=nullptr;
    SetView(m_mainView);
}

CMainFrame::~CMainFrame()
{
}

// Create the frame window.
HWND CMainFrame::Create(HWND parent)
{
    // Set the registry key name, and load the initial window position.
    // Use a registry key name like "CompanyName\\Application".
    //  LoadRegistrySettings(L"Win32++\\Splitter Sample");
    InitValues inVals=GetInitValues();
    inVals.showStatusBar=false;
    inVals.showToolBar=false;
    SetInitValues(inVals);
    return CDockFrame::Create(parent);
}

// Load the default arrangement of the window panes.
void CMainFrame::LoadDefaultWindowPanes()
{
    // Set the styles for splitter panes.
    DWORD style = DS_NO_UNDOCK | DS_NO_CAPTION | DS_CLIENTEDGE;
    SetDockStyle(style);

    CRect viewRect = GetViewRect();

    // Add the bottom pane first. It is a child of the main pane.
    m_pDockTree = static_cast<CDockTree*>(AddDockedChild(new CDockTree, style | DS_DOCKED_LEFT| DS_DOCKED_TOP, viewRect.Width() / 5, ID_DOCK_TREE));

    // Add the bottom right pane. It is a child of the bottom pane.
    m_pDockList = static_cast<CDockList*>(m_pDockTree->AddDockedChild(new CDockList, style | DS_DOCKED_RIGHT, viewRect.Width() *4/ 5, ID_DOCK_LIST));
    // Add the top right pane. It is a child of the main pane.
     m_pDockText = static_cast<CDockText*>(m_pDockList->AddDockedChild(new  CDockText,  style | DS_DOCKED_BOTTOM, viewRect.Height() / 3,ID_DOCK_TEXT));
      m_pDockHexList = static_cast<CViewDockLHex*>(m_pDockList->AddDockedChild(new CViewDockLHex,  style | DS_DOCKED_BOTTOM, viewRect.Height() / 3,ID_DOCK_HEXLIST));
    m_pDockHexList->getHexListView()->setTableFonts();
    m_pDockHexList->Hide();
    m_pDockList->getListView()->initPtrs(m_pDockHexList,m_pDockList,m_pDockText);
}

// Adds a new docker. The id specifies the dock type.
// The id is used by LoadDockRegistrySettings to restore the
// previous window pane arrangement.
DockPtr CMainFrame::NewDockerFromID(int id)
{
    DockPtr docker;
    switch (id)
    {
    case ID_DOCK_LIST:
        docker = make_unique<CDockList>();
        break;
    case ID_DOCK_TREE:
        docker = make_unique<CDockTree>();
        break;
    case ID_DOCK_TEXT:
        docker = make_unique<CDockText>();
        break;
    case ID_DOCK_HEXLIST:
        docker = make_unique<CViewDockLHex>();
        break;
    default:
        TRACE("Unknown Dock ID\n");
        break;
    }

    return docker;
}

// Responds to menu and toolbar input.
BOOL CMainFrame::OnCommand(WPARAM wparam, LPARAM)
{
    UINT id = LOWORD(wparam);

    switch(id)
    {
    case IDM_FILE_EXIT:         return OnFileExit();
    case IDM_FILE_OPEN:         return OnFileOpen();
    case IDM_HELP_ABOUT:        return OnHelp();
    }
    return FALSE;
}

// Called from the file exit menu command.
BOOL CMainFrame::OnFileExit()
{
    // Issue a close request to the frame   (*.cpp; *.h)\0*.cpp; *.h\0All Files (*.*)\0*.*\0")
    Close();
    return TRUE;
}
//-----------------------------------------
BOOL CMainFrame::OnFileOpen()
{
    CFileDialog fileDlg(TRUE, L"db", nullptr, OFN_FILEMUSTEXIST, L"Data base Files (*.db; *.db3; *.sqlite)\0*.db; *.db3; *.sqlite\0GeoBase Files (*.gpkg)\0*.gpkg\0All Files (*.*)\0*.*\0");

    if (fileDlg.DoModal(*this) == IDOK)
    {
        CString str = fileDlg.GetPathName();
        if(m_pDockTree)
        {
            if( m_pDockTree->GetViewTree()->OpenDataBase(str))
            {
                m_pDockTree->GetViewTree()->setListPtr(m_pDockList->getListView());
                m_pDockList->getListView()->setviewText(m_pDockText->getTextView());
            }
        }
    }

    return TRUE;
}
// Limit the minimum size of the window.
LRESULT CMainFrame::OnGetMinMaxInfo(UINT msg, WPARAM wparam, LPARAM lparam)
{
    LPMINMAXINFO lpMMI = (LPMINMAXINFO)lparam;
    const CSize minimumSize(400, 350);
    lpMMI->ptMinTrackSize.x = DpiScaleInt(minimumSize.cx);
    lpMMI->ptMinTrackSize.y = DpiScaleInt(minimumSize.cy);
    return FinalWindowProc(msg, wparam, lparam);
}

// Called after the frame window is created.
void CMainFrame::OnInitialUpdate()
{
    LoadDefaultWindowPanes();
    // PreCreate initially set the window as invisible, so show it now.
    SetTitle(L"SqliteTreeView");
     ShowWindow(GetInitValues().showCmd);
}

// Specify the CREATESTRUCT parameters before the window is created.
void CMainFrame::PreCreate(CREATESTRUCT& cs)
{
    // Call base class to set defaults
    CDockFrame::PreCreate(cs);

    // Hide the window initially by removing the WS_VISIBLE style
    cs.style &= ~WS_VISIBLE;
}

// Specifies the images used on menu items.
// Process the frame's window messages.
LRESULT CMainFrame::WndProc(UINT msg, WPARAM wparam, LPARAM lparam)
{
    try
    {
        switch (msg)
        {
        case WM_SIZE:
            m_pDockTree->SetDockSize(GetClientRect().Width()/5);
            m_pDockList->SetDockSize(GetClientRect().Width()*4/5);
            m_pDockText->SetDockSize( GetClientRect().Height()/3);
            m_pDockHexList->SetDockSize( GetClientRect().Height()/3);
            break;
        }
        // Pass unhandled messages on for default processing.
        return WndProcDefault(msg, wparam, lparam);
    }

    catch (const CException& e)
    {
        // Display the exception and continue.
        CString str1;
        str1 << e.GetText() << L'\n' << e.GetErrorString();
        CString str2;
        str2 << "Error: " << e.what();
        ::MessageBox(nullptr, str1, str2, MB_ICONERROR);
    }

    // Catch all unhandled std::exception types.
    catch (const std::exception& e)
    {
        // Display the exception and continue.
        CString str1 = e.what();
        ::MessageBox(nullptr, str1, L"Error: std::exception", MB_ICONERROR);
    }

    return 0;
}

