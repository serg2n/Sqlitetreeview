/////////////////////////////
// Views.cpp
//

#include "stdafx.h"
#include "Views.h"
#include "resource.h"
#include "utfwstring.h"
#include <iostream>
#include <thread>
#include <cstdio>
#include <fstream>
#include <string>
#include <windows.h>

#include "SplitterPanes.h"

////////////////////////////////////
// CViewSimple function definitions.
//
void CViewSimple::OnDraw(CDC& dc)
{
    // Use the message font for Windows 7 and higher.
    if (GetWinVersion() >= 2601)
    {
        NONCLIENTMETRICS info = GetNonClientMetrics();
        LOGFONT lf = DpiScaleLogfont(info.lfMessageFont, 10);
        dc.CreateFontIndirect(lf);
    }

    // Centre some text in the window.
    CRect rc = GetClientRect();
    dc.DrawText(L"Simple View", -1, rc, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
}

LRESULT CViewSimple::WndProc(UINT msg, WPARAM wparam, LPARAM lparam)
{
    try
    {
        switch (msg)
        {
        case WM_SIZE:
            Invalidate();
            break;
        }

        return WndProcDefault(msg, wparam, lparam);
    }

    // Catch all unhandled CException types.
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


//////////////////////////////////
// CViewList function definitions.
//
CViewList::CViewList()
{
    textData=NULL;
    dockBase=NULL;
    ptDockHexList=nullptr;
    ptDockDataList=nullptr;
    ptDockDataScroll=nullptr;
}

CViewList::~CViewList()
{
    if (IsWindow()) DeleteAllItems();
}
// Insert 4 list view items.

void CViewList::OnAttach()
{
    // Call the base class function.
    CListView::OnAttach();
    // Set the report style
    DWORD style = GetStyle();
    SetStyle((style & ~LVS_TYPEMASK) | LVS_REPORT);
    ListView_SetExtendedListViewStyle(GetHwnd(), LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_TRANSPARENTBKGND );
    ListView_SetBkColor(GetHwnd(), RGB(240, 255, 255));

    ListView_SetTextColor(GetHwnd(), RGB(94, 9, 9));
    HFONT hFont = CreateFont( 18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                              OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS,L"MS Shell Dlg");
    if (hFont)
        SendMessage(GetHwnd(), WM_SETFONT, (WPARAM)hFont, TRUE);
    SetBkMode((HDC)GetDC(), TRANSPARENT);
}
LRESULT CALLBACK HeaderProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR);
//---------------------------------------------
void CViewList::SetListViewHeaderColor(HWND hListView, COLORREF color)
{
    HWND hHeader = ListView_GetHeader(hListView);
    SetWindowSubclass(hHeader, HeaderProc, 1, (DWORD_PTR)color);
}
void DrawVerticalText(HDC hdc, RECT rect, LPCWSTR text)
{
    // Save the current state of the HDC
    int savedDC = SaveDC(hdc);

    // Set up the transformation for vertical text
    SetBkMode(hdc, TRANSPARENT);
    SetTextAlign(hdc, TA_CENTER | TA_TOP);
    XFORM xForm = { 0 };
    xForm.eM11 = 0.0f;
    xForm.eM12 = -1.0f;
    xForm.eM21 = 1.0f;
    xForm.eM22 = 0.0f;
    xForm.eDx = (FLOAT)(rect.left + (rect.right - rect.left) / 2);
    xForm.eDy = (FLOAT)(rect.bottom - (rect.bottom - rect.top) / 2);
    SetWorldTransform(hdc, &xForm);

    // Draw the text
    TextOut(hdc, 0, 0, text, lstrlen(text));

    // Restore the HDC
    RestoreDC(hdc, savedDC);
}
//------------------------------------------------------
LRESULT CALLBACK HeaderProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR , DWORD_PTR dwRefData)
{
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        // Set the background to transparent
        SetBkMode(hdc, TRANSPARENT);
        RECT recth;
        GetClientRect(hwnd,&recth);
        HBRUSH hBrush = CreateSolidBrush((COLORREF)dwRefData);
        FillRect(hdc, &recth, hBrush);
        // Draw the text
        SetTextColor(hdc,RGB(15,90,15));
        HFONT hFont = CreateFont( 18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                                  OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS,L"MS Shell Dlg");
        HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
        // Get the header item count
        int itemCount = (int)SendMessage(hwnd, HDM_GETITEMCOUNT, 0, 0);
        RECT rect;
        RECT rectbg;
        HDITEM hdi;
        TCHAR text[256];

        // Loop through each header item
        for (int i = 0; i < itemCount-1; ++i)
        {
            // Get the item rectangle
            SendMessage(hwnd, HDM_GETITEMRECT, i, (LPARAM)&rect);
            // Get the item text
            hdi.mask = HDI_TEXT| HDI_FORMAT;
            hdi.pszText = text;
            hdi.cchTextMax = sizeof(text) / sizeof(text[0]);
            SendMessage(hwnd, HDM_GETITEM, i, (LPARAM)&hdi);
            Rectangle(hdc,rect.left,rect.top,rect.right,rect.bottom);
            SetRect(&rectbg,rect.left+1,rect.top,rect.right,rect.bottom-1);
            FillRect(hdc, &rectbg, hBrush);
            DrawText(hdc, hdi.pszText, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            //  DrawVerticalText(hdc,rect,hdi.pszText);
        }
        // Get the item rectangle
        int n=itemCount-1;
        SendMessage(hwnd, HDM_GETITEMRECT,n, (LPARAM)&rect);
        // Get the item text
        hdi.mask = HDI_TEXT;
        hdi.pszText = text;
        hdi.cchTextMax = sizeof(text) / sizeof(text[0]);
        SendMessage(hwnd, HDM_GETITEM, n, (LPARAM)&hdi);
        Rectangle(hdc,rect.left,rect.top,rect.right,rect.bottom);
        SetRect(&rectbg,rect.left+1,rect.top,rect.right-1,rect.bottom-1);
        FillRect(hdc, &rectbg, hBrush);
        DrawText(hdc, hdi.pszText, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        SelectObject(hdc, hOldFont);
        // Delete the custom font
        DeleteObject(hFont);
        DeleteObject(hBrush);
        EndPaint(hwnd, &ps);
        return 0;
    }
    default:
        return DefSubclassProc(hwnd, msg, wParam, lParam);
    }
}

// Called in response to a WM_DPICHANGED_BEFOREPARENT message that is sent to child
// windows after a DPI change. A WM_DPICHANGED_BEFOREPARENT is only received when the
// application is DPI_AWARENESS_PER_MONITOR_AWARE.
LRESULT CViewList::OnDpiChangedBeforeParent(UINT msg, WPARAM wparam, LPARAM lparam)
{
    return FinalWindowProc(msg, wparam, lparam);
}
//------------------------------------
std::vector<unsigned char> CViewList::ReadBlobFromDatabase(sqlite3* db, const char* sqlQuery,int row, int iCol)
{
    std::vector<unsigned char> blobData;
    //  std::basic_string <unsigned char> blobData;
    blobData.clear();
    sqlite3_stmt* stmt;
    int rc;
    // Prepare the SQL statement
  //  std::cout<<sqlQuery<<std::endl;
    rc = sqlite3_prepare_v2(db, sqlQuery, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to execute query: " << sqlite3_errmsg(db) << std::endl;
        return blobData;
    }
    int currentRow = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        if (currentRow == row)
        {
            const void* blob = sqlite3_column_blob(stmt, iCol);
            int blobSize = sqlite3_column_bytes(stmt, iCol);
            // Execute the query and read the BLOB
            blobData.assign((unsigned char*)blob, (unsigned char*)blob + blobSize);
        }
        currentRow++;
    }
    // Finalize the statement and close the database
    sqlite3_finalize(stmt);
    return blobData;
}
// Process the listview window messages.
LRESULT CViewList::WndProc(UINT msg, WPARAM wparam, LPARAM lparam)
{
   HWND hEdit=NULL;
    try
    {
#if 1
        switch (msg)
        {
        case WM_LBUTTONDOWN:
            if (hEdit != NULL){SendMessage(hEdit,WM_KILLFOCUS,0,0);};
            LVHITTESTINFO itemclicked;
            DWORD style = DS_NO_UNDOCK | DS_NO_CAPTION | DS_CLIENTEDGE;
            long x, y;
            x = (long)LOWORD(lparam);
            y = (long)HIWORD(lparam);
            itemclicked.pt.x = x;
            itemclicked.pt.y = y;
            int lResult = ListView_SubItemHitTest(GetHwnd(),&itemclicked);
        //    MessageBoxW( GetItemText(itemclicked.iItem,itemclicked.iSubItem),L"Test",MB_OK);
    #if 1
            if (lResult!=-1)
            {
                if(GetItemText(itemclicked.iItem,itemclicked.iSubItem)==L"BLOB")
                {
                 //   if(nRows.size()>=itemclicked.iSubItem)
                    {
                        if(dockBase)
                        {
                            std::wstring sql =utf8_to_wstring("SELECT * FROM '");
                            sql.append(tablName+L"\'");
                            //  MessageBoxW(std::to_wstring(nRows.at(itemclicked.iSubItem)).c_str(),L"Test",MB_OK);
                            ptDockHexList->getHexListView()->blobData= ReadBlobFromDatabase(dockBase,wstring_to_utf8(sql).c_str(),itemclicked.iItem,itemclicked.iSubItem);
                            if(ptDockHexList->getHexListView()->blobData.size()>0)
                            {
                                if( ptDockHexList)
                                {
                                    if (!ptDockHexList->IsDocked())
                                    {
                                        ptDockDataScroll->Hide();
                                        ptDockDataList->Dock(ptDockHexList, style | DS_DOCKED_BOTTOM);
                                    }
                                    ptDockHexList->getHexListView()->fillTable();
                                }
                            }
                        }
                    }
                }
                else
                {
                    if(textData)
                    {
                        if( ptDockHexList)
                        {
                            if (ptDockHexList->IsDocked())
                            {
                                ptDockHexList->Hide();
                                ptDockDataList->Dock(ptDockDataScroll, style | DS_DOCKED_BOTTOM);
                            }
                            std::wstring szData=GetItemText(itemclicked.iItem,itemclicked.iSubItem).c_str();
                            szData.append(L"\n");
                            ptDockDataScroll->getTextView()->clearText();
                            ptDockDataScroll->getTextView()->AppendText(szData.c_str());
                        }
                    }
                }
            }
    #endif
            break;
        }

#endif
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


//////////////////////////////////
// CViewTree function definitions.
//
CViewTree::CViewTree()
{
    dBase=NULL;
    viewList=NULL;
}

CViewTree::~CViewTree()
{
    if (IsWindow()) DeleteAllItems();
}

void CViewTree::OnAttach()
{
    DWORD style = GetStyle();
    style |= TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT;
    SetStyle(style);
    TreeView_SetBkColor(GetHwnd(), RGB(255, 255, 240));
    TreeView_SetTextColor(GetHwnd(), RGB(10, 10, 200));
    HFONT hFont = CreateFont( 18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                              OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS,L"MS Shell Dlg");
    if (hFont)
    {
        SendMessage(GetHwnd(), WM_SETFONT, (WPARAM)hFont, TRUE);
    }

}
LRESULT CViewTree::OnDpiChangedBeforeParent(UINT msg, WPARAM wparam, LPARAM lparam)
{
    return FinalWindowProc(msg, wparam, lparam);
}


//--------------------------------------------------
bool CViewTree::OpenDataBase( CString str)
{
    bool bOpen=false;
    sqlite3_stmt* stmt;
    char myString[256];
    str.Replace('\\','/');
    size_t nNumCharConverted;
    wcstombs_s(&nNumCharConverted, myString, 256, str.GetString().data(), 256);
    if (sqlite3_open(myString, &dBase) != SQLITE_OK)
    {
        throw std::runtime_error(std::string("Can't open database: ") + sqlite3_errmsg(dBase));
    }
    else
    {
        std::string sql="SELECT name FROM sqlite_master WHERE type='table'";
        if (sqlite3_prepare_v2(dBase, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(dBase) << std::endl;
            sqlite3_close(dBase);
            return 1;
        }
        // Execute the statement and print table names
        typedef std::basic_string <unsigned char> ustring;
        DeleteAllItems();
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            sqlite3_stmt* stmt1;
            const unsigned char *s1 =(const unsigned char *) "SELECT * FROM '";
            ustring sqlreq(s1);
            ustring s =sqlite3_column_text(stmt, 0);
            std::wstring ws(s.begin(), s.end());
            std::string tableName="'"+wstring_to_utf8(ws)+"'";
#if 1
            char* err_msg = 0;
            int count = 0;
            std::wstring sql =utf8_to_wstring("SELECT COUNT (*) FROM '");
            sql.append(ws);
            sql.append(L"'");
            int rc = sqlite3_exec(dBase,  wstring_to_utf8(sql).c_str(), callback, &count, &err_msg);
            if (rc != SQLITE_OK)
            {
                fprintf(stderr, "SQL error: %s\n", err_msg);
                sqlite3_free(err_msg);
            }
            else
            {
                // MessageBoxW(std::to_wstring(count).c_str(),L"Count",MB_OK);
                ws.append(utf8_to_wstring(" table total ")+std::to_wstring(count)+utf8_to_wstring(" rows"));
                HTREEITEM htiRoot = InsertItem(ws.c_str(), 0, 0);
                sqlreq+=s;
                sqlreq+='\'';
                rc = sqlite3_prepare_v2(dBase,(const char*)sqlreq.c_str(), -1, &stmt1, nullptr);
                if (rc != SQLITE_OK)
                {
                    std::cerr << "Failed to execute statement11111111111: " << sqlite3_errmsg(dBase) << std::endl;
                    //  sqlite3_close(dBase);
                    //    return rc;
                }
                else
                    readColumnTypes(dBase,tableName,htiRoot);
            }
#endif

        }

        // Clean up
        sqlite3_finalize(stmt);
        // sqlite3_close(dBase);
        return true;
    }
    return  bOpen;
}
//--------------------------------------------
int CViewTree::callback(void *count, int argc, char **argv, char **azColName)
{
    int *c =(int*) count;
    *c = atoi(argv[0]);
    return 0;
}
//------------------------------------------
int CViewTree::readRow(void* data, int argc, char** argv, char** azColName) {
    for (int i = 0; i < argc; i++) {
        std::cout << azColName[i] << ": " << (argv[i] ? argv[i] : "NULL") << std::endl;
    }
    std::cout << std::endl;
    return 0;
}
//------------------------------------
void CViewTree::readColumnTypes(sqlite3* db, const std::string& tableName,HTREEITEM htiRoot)
{
    sqlite3_stmt* stmt;
    std::string query = "PRAGMA table_info(" + tableName + ")";
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK)
    {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return;
    }
    int i=0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        std::basic_string <unsigned char> s =sqlite3_column_text(stmt, 1);
        std::wstring ws(s.begin(), s.end());
        HTREEITEM htiColums = InsertItem(ws.c_str(), 1, i, htiRoot);
        TVITEM tvi = {0};
        tvi.mask = TVIF_HANDLE | TVIF_PARAM;
        tvi.hItem = htiColums;
        tvi.lParam = i;
        TreeView_SetItem(GetHwnd(), &tvi);
        i++;
    }
    if (rc != SQLITE_DONE) {
        std::cerr << "Failed to execute statement22222222222: " << sqlite3_errmsg(db) << std::endl;
    }
    sqlite3_finalize(stmt);
}
//-----------------------------------------
void CViewTree::readRowsval(sqlite3* db, const std::string& query,HTREEITEM hParentItem)
{
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmTr, nullptr);

    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return;
    }
    if(viewList)
    {
        viewList->setdbPtr(db);
        szTypes= GetChildItems(GetHwnd(),hParentItem);
        viewList->DeleteAllItems();
        int columnCount = Header_GetItemCount(ListView_GetHeader(viewList->GetHwnd()));
        for (int i = columnCount - 1; i >= 0; --i)
            viewList->DeleteColumn(i);
        //   ListView_DeleteColumn(viewList->GetHwnd(), i);
        HFONT hFonth = CreateFont( 20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                                   OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS,L"MS Shell Dlg");
        if (hFonth)
            SendMessage(ListView_GetHeader(viewList->GetHwnd()), WM_SETFONT, (WPARAM)hFonth, TRUE);
        //   readDataRows();
        std::thread t1(&CViewTree::readDataRows,this);
        t1.detach();
        viewList->SetListViewHeaderColor(viewList->GetHwnd(),RGB(255, 255, 225));
    }
}
//-----------------------------------------
LPWSTR convertToLPWSTR(const unsigned char* str)
{
    // Determine the length of the wide string
    LPWSTR lpwstr=NULL;
    int len = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(str), -1, NULL, 0);
    if (len != 0)
    {
        std::wstring szData;
        szData.resize(len);
        lpwstr = const_cast<wchar_t*>(szData.c_str());
        // Perform the conversion
        MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(str), -1, lpwstr, len);
    }
    return lpwstr;
}

//----------------------------------------------
void CViewTree::readDataRows()
{
    int item =0;
    int rc;
    for (int i = 0; i < sqlite3_column_count(stmTr); i++)
    {
        const char* columnName = sqlite3_column_name(stmTr, i);
        std::wstring szColumn=utf8_to_wstring(columnName);
        viewList->InsertColumn(i,szColumn.c_str());
        viewList->SetColumnWidth(i,90);
    }
    ListView_SetExtendedListViewStyle(viewList->GetHwnd(), LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
    HWND hwnd=viewList->GetHwnd();
    while ((rc = sqlite3_step(stmTr)) == SQLITE_ROW)
    {
# if 1
        item =viewList->InsertItem(item, L"");
        for (int i = 0; i < sqlite3_column_count(stmTr); i++)
        {
            int columnType = sqlite3_column_type(stmTr, i);
       //     std::cout << "Column Type: " << columnType << std::endl;
            const unsigned char* columnValue = sqlite3_column_text(stmTr, i);
            if(columnValue)
            {
                if(columnType==SQLITE_BLOB)
                    viewList->SetItemText(item, i, L"BLOB");
                else
                {
                    LPWSTR value=convertToLPWSTR(columnValue);
                    if(value)
                    {
                        ListView_SetItemText(hwnd,item, i, value);
                    }
                }
            }
            else
                viewList->SetItemText(item, i, L"Null");
       }
       item++;
#endif
    }
    for (int i = 0; i <Header_GetItemCount(viewList->GetHeader()); i++)
        viewList->SetColumnWidth(i,LVSCW_AUTOSIZE_USEHEADER);
    if (rc != SQLITE_DONE) {
        std::cerr << "Failed to execute statement333333333: " << sqlite3_errmsg(dBase) << std::endl;
    }
    sqlite3_finalize(stmTr);
}
//------------------------------------------------

// Process window messages for the tree-view control.
LRESULT CViewTree::WndProc(UINT msg, WPARAM wparam, LPARAM lparam)
{
    try
    {
        switch (msg)
        {
        case WM_DPICHANGED_BEFOREPARENT: return OnDpiChangedBeforeParent(msg, wparam, lparam);
        case WM_NOTIFY: return OnNotifyReflect(wparam, lparam);
        }

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
//----------------------------------
LRESULT CViewTree::OnNotifyReflect(WPARAM, LPARAM lparam)
{
    if (((LPNMHDR) lparam)->hwndFrom == GetHwnd())
    {
        switch (((LPNMHDR) lparam)->code)
        {
        case TVN_SELCHANGED:
        {
            LPNMTREEVIEW lpnmtv = (LPNMTREEVIEW) lparam;
            HTREEITEM hSelectedItem = lpnmtv->itemNew.hItem; // Retrieve item text
            TVITEM tvi = {0};
            tvi.mask = TVIF_HANDLE | TVIF_PARAM|TVIF_TEXT;
            tvi.mask = TVIF_TEXT;
            tvi.hItem = hSelectedItem;
            wchar_t szText[256];
            tvi.pszText = szText;
            tvi.cchTextMax = sizeof(szText) / sizeof(szText[0]);
            TreeView_GetItem( GetHwnd(), &tvi);
            if(TreeView_GetParent(GetHwnd(), hSelectedItem))
            {
                //  MessageBoxW( tvi.pszText, L"Selected Item Text", MB_OK);
                std::vector<std::wstring> valItems= GetListViewItemText(viewList->GetHwnd(),tvi.lParam);
            }
            else
            {
                char* err_msg = 0;
                int count = 0;
                std::vector<std::wstring> namTable=Split(tvi.pszText,' ');
                tablName=namTable.at(0);
                std::wstring sql =utf8_to_wstring("SELECT COUNT(*) FROM '");
                sql.append(namTable.at(0));
                sql.append(L"'");
                int rc = sqlite3_exec(dBase, wstring_to_utf8(sql).c_str(), callback, &count, &err_msg);
                if (rc != SQLITE_OK) {
                    fprintf(stderr, "SQL error: %s\n", err_msg);
                    sqlite3_free(err_msg);
                }
                else
                {
                    viewList->setTableName(namTable.at(0));
                    readRowsval(dBase,"SELECT * FROM '"+wstring_to_utf8(namTable.at(0))+"'",hSelectedItem);
                }
            }
        }
            break;
        }
    }
    return 0;
}
//-------------------------------------
std::vector<std::wstring>  CViewTree::GetListViewItemText(HWND hwndListView, int subItemIndex)
{
    std::vector<std::wstring> wItems;
    wItems.clear();
    for(int i=0;i<viewList->GetItemCount();i++)
    {
        wchar_t buffer[256];
        LVITEM lvItem;
        lvItem.iSubItem = subItemIndex;
        lvItem.pszText = buffer;
        lvItem.cchTextMax = sizeof(buffer) / sizeof(buffer[0]);
        ListView_GetItemText(hwndListView,i, subItemIndex, buffer, lvItem.cchTextMax);
        wItems.push_back(std::wstring(buffer));
    }
    return  wItems;
}

//-------------------------------------
std::vector<std::string> CViewTree:: GetChildItems(HWND hwndTreeView, HTREEITEM hParentItem)
{
    std::vector<std::string> szType;
    HTREEITEM hChildItem = TreeView_GetChild(hwndTreeView, hParentItem);
    while (hChildItem != NULL)
    {
        TVITEMW tvi;
        tvi.mask = TVIF_TEXT;
        tvi.hItem = hChildItem;
        wchar_t szText[256];
        tvi.pszText = szText;
        tvi.cchTextMax = sizeof(szText) / sizeof(szText[0]);
        TreeView_GetItem( GetHwnd(), &tvi);
        szType.push_back(wstring_to_utf8(tvi.pszText));
        hChildItem = TreeView_GetNextSibling(hwndTreeView, hChildItem);
    }
    return szType;
}


//////////////////////////////////
// CViewText function definitions.
//
void CViewText::OnAttach()
{
   //  DWORD style = GetStyle();
    SetOptions(ECOOP_OR,ECO_READONLY|ECO_SAVESEL );
    SendMessage(GetHwnd(), EM_SETBKGNDCOLOR, 0, (LPARAM)RGB(255, 255, 235));
    SetDPIFont();
}

// Adjusts the font size in response to window DPI changes.
void CViewText::SetDPIFont()
{
    m_font.CreatePointFont(100, L"Arial Bold");
    m_font = DpiScaleFont(m_font,14);
    SetFont(m_font);
}
//---------------------------------
void CViewText::clearText()
{
    SetSel(0,-1);
    ReplaceSel(L"");
}
// Handle the window's messages.
LRESULT CViewText::WndProc(UINT msg, WPARAM wparam, LPARAM lparam)
{
    try
    {
        //        switch (msg)
        //        {

        //        }
        // Pass unhandled messages on for default processing.
        return WndProcDefault(msg, wparam, lparam);
    }

    // Catch all unhandled CException types.
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
//---------------------------------------
CViewLHex::CViewLHex()
{
}

CViewLHex::~CViewLHex()
{
    if (IsWindow()) DeleteAllItems();
}
//--------------------------------------
void CViewLHex::OnAttach()
{
    // Call the base class function.
    CListView::OnAttach();

    // Set the report style
    DWORD style = GetStyle();
    SetStyle((style & ~LVS_TYPEMASK) | LVS_REPORT);
    ListView_SetExtendedListViewStyle(GetHwnd(), LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_TRANSPARENTBKGND );
    ListView_SetBkColor(GetHwnd(), RGB(255, 255, 235));

    ListView_SetTextColor(GetHwnd(), RGB(9, 9, 94));
    HFONT hFont = CreateFont( 14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                              OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS,L"Lucida Console");
    if (hFont)
        SendMessage(GetHwnd(), WM_SETFONT, (WPARAM)hFont, TRUE);
    SetBkMode((HDC)GetDC(), TRANSPARENT);
}
LRESULT CALLBACK HeaderProcHex(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR);
//---------------------------------------------
void CViewLHex::SetListViewHeaderColor(HWND hListView, COLORREF color)
{
    HWND hHeader = ListView_GetHeader(hListView);
    SetWindowSubclass(hHeader, HeaderProcHex, 1, (DWORD_PTR)color);
}
//------------------------------------------------------
LRESULT CALLBACK HeaderProcHex(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR , DWORD_PTR dwRefData)
{
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        // Set the background to transparent
        SetBkMode(hdc, TRANSPARENT);
        RECT recth;
        GetClientRect(hwnd,&recth);
        HBRUSH hBrush = CreateSolidBrush((COLORREF)dwRefData);
        FillRect(hdc, &recth, hBrush);
        // Draw the text
        SetTextColor(hdc,RGB(15,90,15));
        HFONT hFont = CreateFont( 14, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                                  OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS,L"Lucida Console");
        HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
        // Get the header item count
        int itemCount = (int)SendMessage(hwnd, HDM_GETITEMCOUNT, 0, 0);
        RECT rect;
        RECT rectbg;
        HDITEM hdi;
        TCHAR text[256];

        // Loop through each header item
        for (int i = 0; i < itemCount; ++i)
        {
            // Get the item rectangle
            SendMessage(hwnd, HDM_GETITEMRECT, i, (LPARAM)&rect);

            // Get the item text
            hdi.mask = HDI_TEXT;
            hdi.pszText = text;
            hdi.cchTextMax = sizeof(text) / sizeof(text[0]);
            SendMessage(hwnd, HDM_GETITEM, i, (LPARAM)&hdi);
            Rectangle(hdc,rect.left,rect.top,rect.right,rect.bottom);
            SetRect(&rectbg,rect.left+1,rect.top,rect.right,rect.bottom-1);
            FillRect(hdc, &rectbg, hBrush);
            DrawText(hdc, hdi.pszText, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
        int n=itemCount-1;
        SendMessage(hwnd, HDM_GETITEMRECT,n, (LPARAM)&rect);
        // Get the item text
        hdi.mask = HDI_TEXT;
        hdi.pszText = text;
        hdi.cchTextMax = sizeof(text) / sizeof(text[0]);
        SendMessage(hwnd, HDM_GETITEM, n, (LPARAM)&hdi);
        Rectangle(hdc,rect.left,rect.top,rect.right,rect.bottom);
        SetRect(&rectbg,rect.left+1,rect.top,rect.right-1,rect.bottom-1);
        FillRect(hdc, &rectbg, hBrush);
        DrawText(hdc, hdi.pszText, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        SelectObject(hdc, hOldFont);
        // Delete the custom font
        DeleteObject(hFont);
        DeleteObject(hBrush);
        EndPaint(hwnd, &ps);
        return 0;
    }
    default:
        return DefSubclassProc(hwnd, msg, wParam, lParam);
    }
}

// Process the listview window messages.
LRESULT CViewLHex::WndProc(UINT msg, WPARAM wparam, LPARAM lparam)
{
    try
    {
#if 0
        switch (msg)
        {

        }
#endif
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
//---------------------------------------------
void CViewLHex::setDataTable()
{
    int item =0;
    int col=0;
    std::vector<unsigned char> szViewData;
    size_t blobSize=blobData.size();
    for(int i=0;i<blobSize;i+=HEX_STR_LENDTH)
    {
        szViewData.clear();
        col=0;
        InsertItem(item,ucharToHexWstring(blobData.at(i)).c_str());
        szViewData.push_back(blobData.at(i));
        col++;
        while((col<HEX_STR_LENDTH)&&(i+col)<blobSize)
        {
            SetItemText(item,col,ucharToHexWstring(blobData.at(i+col)).c_str());
            szViewData.push_back(blobData.at(i+col));
            col++;
        }
        std::wstring viewString(szViewData.begin(), szViewData.end());
        char replacement_char = '.';
        for (auto& ch : viewString)
        {
            if (static_cast<unsigned char>(ch) < 0x30)
            {
                ch = replacement_char;
            }
        }
        SetItemText(item,HEX_STR_LENDTH,viewString.c_str());
        SetColumnWidth(HEX_STR_LENDTH,LVSCW_AUTOSIZE);
        item++;
    }
    SetColumnWidth(HEX_STR_LENDTH,LVSCW_AUTOSIZE);
    ListView_SetExtendedListViewStyle(GetHwnd(), LVS_EX_FULLROWSELECT);
}//---------------------------------
void CViewLHex::clearTable()
{
    HWND hWndHdr = (HWND)::SendMessage(GetHwnd(), LVM_GETHEADER, 0, 0);
    int count = (int)::SendMessage(hWndHdr, HDM_GETITEMCOUNT, 0, 0L);
    for (int i = count - 1; i >= 0; --i)
        DeleteColumn(i);
    DeleteColumn(0);
    DeleteAllItems();
}
//------------------------------------
void CViewLHex::createHeader()
{
    HFONT hFont = CreateFont( 14, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                              OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS,L"Lucida Console");
    if (hFont)
    {
        SendMessage(ListView_GetHeader(GetHwnd()), WM_SETFONT, (WPARAM)hFont, TRUE);
    }
    LVCOLUMN lvColumn = {0};
    lvColumn.mask = LVCF_WIDTH|LVCF_FMT; // Specify that we are setting the width
    lvColumn.cx = 28;
    lvColumn.fmt=LVCFMT_FIXED_WIDTH|LVCFMT_CENTER;
    for(unsigned char i=0;i<HEX_STR_LENDTH;i++)
    {
        InsertColumn(i,ucharToHexWstring(i).c_str());
        ListView_SetColumn(GetHwnd(), i, &lvColumn);
        SetListViewHeaderColor(GetHwnd(),RGB(255, 200, 127));
    }
    InsertColumn(HEX_STR_LENDTH,L"String");
    lvColumn = {0};
    lvColumn.mask = LVCF_WIDTH|LVCF_FMT; // Specify that we are setting the width
    lvColumn.cx = 250;
    lvColumn.fmt=LVCFMT_FIXED_WIDTH|LVCFMT_CENTER;
    ListView_SetColumn(GetHwnd(), HEX_STR_LENDTH, &lvColumn);
    SetListViewHeaderColor(GetHwnd(),RGB(255, 200, 127));
}
//--------------------------------------
void CViewLHex::setTableFonts()
{
    HFONT hFont = CreateFont( 14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                              OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS,L"Lucida Console");
    if (hFont)
    {
        SendMessage(GetHwnd(), WM_SETFONT, (WPARAM)hFont, TRUE);
    }
}
//--------------------------------------
void CViewLHex::fillTable()
{
    clearTable();
    createHeader();
    std::thread t2(&CViewLHex::setDataTable,this);
    t2.detach();
}
