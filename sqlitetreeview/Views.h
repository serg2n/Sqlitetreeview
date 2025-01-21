/////////////////////////////
// Views.h
//

#ifndef VIEWS_H
#define VIEWS_H

#include "stdafx.h"
#include "sqlite3.h"


class CDockText;
class CDockTree;
class CDockText;
class CDockList;
class CViewDockLHex;

enum
{
  HEX_STR_LENDTH=32
};

///////////////////////////////////////
// CViewSimple manages a simple window.
class CViewSimple : public CWnd
{
public:
    CViewSimple() {}
    virtual ~CViewSimple() override {}

protected:
    // Virtual functions that override base class functions
    virtual void OnDraw(CDC& dc) override;
    virtual LRESULT WndProc(UINT msg, WPARAM wparam, LPARAM lparam) override;

private:
    CViewSimple(const CViewSimple&) = delete;
    CViewSimple& operator=(const CViewSimple&) = delete;
};


//////////////////////////////////////////////
// CViewText manages a rich edit control.
// It is used as the view window by CDockText.
class CViewText : public CRichEdit
{
public:
    CViewText() {}
    virtual ~CViewText() override {}
    void SetDPIFont();
     void clearText();
protected:
    // Virtual functions that override base class functions
    virtual void OnAttach() override;
    virtual LRESULT WndProc(UINT msg, WPARAM wparam, LPARAM lparam) override;

private:
    CViewText(const CViewText&) = delete;
    CViewText& operator=(const CViewText&) = delete;
    CFont m_font;
};


//////////////////////////////////////////////
// CViewText manages a tree view control.
// It is used as the view window by CDockTree.
class CViewList;
class CViewDataList;

class CViewTree : public CTreeView
{
public:
    CViewTree();
    virtual ~CViewTree() override;
    bool OpenDataBase(CString str);
    sqlite3* getBase(){return dBase;}
    void readDataRows();
    void setListPtr( CViewList* vlist){viewList=vlist;}
protected:
    // Virtual functions that override base class functions
    virtual void OnAttach() override;
    virtual LRESULT OnNotifyReflect(WPARAM wparam, LPARAM lparam) override;
    virtual LRESULT WndProc(UINT msg, WPARAM wparam, LPARAM lparam) override;
private:
    CViewTree(const CViewTree&) = delete;
    CViewTree& operator=(const CViewTree&) = delete;
    sqlite3* dBase;
    sqlite3_stmt* stmTr;
    std::wstring tablName;
    CViewList* viewList;
     std::vector<std::string> szTypes;
    static int callback(void *count, int argc, char **argv, char **azColName);
    static int readRow(void* data, int argc, char** argv, char** azColName);
    void readColumnTypes(sqlite3* db, const std::string& tableName, HTREEITEM htiRoot);
    void readRowsval(sqlite3* db, const std::string& query, HTREEITEM hParentItem);
    std::vector<std::string>GetChildItems(HWND hwndTreeView, HTREEITEM hParentItem);
    std::vector<std::wstring> GetListViewItemText(HWND hwndListView, int subItemIndex);
    // Message handlers
    LRESULT OnDpiChangedBeforeParent(UINT msg, WPARAM wparam, LPARAM lparam);
};

/////////////////////////////////////////////
// CViewList manages a list view control.
// It is used as the view window by CDockList.
class CViewList : public CListView
{
public:
    CViewList();
    virtual ~CViewList() override;
    void SetListViewHeaderColor(HWND hListView, COLORREF color);
    void initPtrs(CViewDockLHex* ptHexList,CDockList* ptDataList,CDockText * ptDataScroll){ptDockHexList=ptHexList;
                                                                                                   ptDockDataList=ptDataList;ptDockDataScroll=ptDataScroll; }
    void setdbPtr(sqlite3* db_ptr){dockBase=db_ptr;}
    void setTableName(std::wstring namTabl){tablName=namTabl;}
    void setviewText(CViewText* text){textData=text;}
 protected:
    // Virtual functions that override base class functions
    virtual void OnAttach() override;
    virtual LRESULT WndProc(UINT msg, WPARAM wparam, LPARAM lparam) override;
private:
    sqlite3* dockBase;
    std::wstring tablName;
    CViewList(const CViewList&) = delete;
    CViewList& operator=(const CViewList&) = delete;
    CViewText* textData;
    CViewDockLHex* ptDockHexList;
    CDockList* ptDockDataList;
    CDockText * ptDockDataScroll;
    // Message handlers
    LRESULT OnDpiChangedBeforeParent(UINT msg, WPARAM wparam, LPARAM lparam);
    std::vector<unsigned char> ReadBlobFromDatabase(sqlite3 *db, const char* sqlQuery, int row, int iCol);
};
//--------------------------------
class CViewLHex : public CListView
{
public:
    CViewLHex();
    virtual ~CViewLHex() override;
    void setDataTable();
     std::vector<unsigned char> blobData;
     void clearTable();
     void createHeader();
     void setTableFonts();
     void fillTable();
     void SetListViewHeaderColor(HWND hListView, COLORREF color);
protected:
    // Virtual functions that override base class functions
    virtual void OnAttach() override;
    virtual LRESULT WndProc(UINT msg, WPARAM wparam, LPARAM lparam) override;

private:
    CViewLHex(const CViewLHex&) = delete;
    CViewLHex& operator=(const CViewLHex&) = delete;
};
#endif // VIEWS_H
