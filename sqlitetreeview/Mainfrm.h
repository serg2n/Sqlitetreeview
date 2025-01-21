/////////////////////////////
// Mainfrm.h
//

#ifndef MAINFRM_H
#define MAINFRM_H

#include "SplitterPanes.h"

// The docker identifiers (dock IDs)
const int ID_DOCK_LIST = 1;
const int ID_DOCK_TREE = 2;
const int ID_DOCK_DATALIST = 3;
const int ID_DOCK_TEXT = 4;
const int ID_DOCK_HEXLIST = 5;


///////////////////////////////////////////////////////////
// CMainFrame manages the application's main window.
// The main window is a frame that has a menubar, toolbar,
// statusbar and view window.
class CMainFrame : public CDockFrame
{
public:
    CMainFrame();
    virtual ~CMainFrame() override;
    virtual HWND Create(HWND parent = nullptr) override;

protected:
    // Virtual functions that override base class functions
    virtual DockPtr NewDockerFromID(int id) override;
    virtual BOOL OnCommand(WPARAM wparam, LPARAM lparam) override;
    virtual void OnInitialUpdate() override;
    virtual void PreCreate(CREATESTRUCT& cs) override;
    virtual LRESULT WndProc(UINT msg, WPARAM wparam, LPARAM lparam) override;

private:
    CMainFrame(const CMainFrame&) = delete;
    CMainFrame& operator=(const CMainFrame&) = delete;

    // Command handlers
    BOOL OnFileExit();
    BOOL OnFileOpen();

    // Message handlers
    LRESULT OnGetMinMaxInfo(UINT msg, WPARAM wparam, LPARAM lparam);

    void LoadDefaultWindowPanes();

    // Member variables
    CViewSimple m_mainView;
    CDockText* m_pDockText;
    CDockTree* m_pDockTree;
    CDockList* m_pDockList;
    CViewDockLHex* m_pDockHexList;
 //   CDockViewScroll * m_pDockDataScroll;
};


#endif //MAINFRM_H

