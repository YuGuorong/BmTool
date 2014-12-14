#ifndef __LISTCTRLEX__
#define __LISTCTRLEX__

#include <map>
#include <vector>
#include <list>
#include "ListCtrlCellEx.h"
//////////////////////////////////////////////////////////////////////////
#define USING_CUSTOM_DRAW // define this to control the cell control theme (whether or not to draw them hand on ))
/* usaqge demo:

m_list.ModifyStyle(0, LVS_SINGLESEL|LVS_REPORT|LVS_SHOWSELALWAYS);
m_list.SetExtendedStyle(m_list.GetExtendedStyle() |LVS_EX_FULLROWSELECT );

m_list.InsertColumn(0, _T("Normal Text"), 100, ListCtrlEx::Normal, LVCFMT_CENTER, ListCtrlEx::SortByDigit);
m_list.InsertColumn(1, _T("Check Box"), 100, ListCtrlEx::CheckBox, LVCFMT_CENTER, ListCtrlEx::SortByString);
m_list.InsertColumn(2, _T("Edit Box"), 100, ListCtrlEx::EditBox, LVCFMT_CENTER, ListCtrlEx::SortByString);
m_list.InsertColumn(3, _T("Combo Box"), 100, ListCtrlEx::ComboBox, LVCFMT_CENTER, ListCtrlEx::SortByString);
m_list.InsertColumn(4, _T("Radio Box"), 100, ListCtrlEx::RadioBox, LVCFMT_CENTER, ListCtrlEx::SortByString);
m_list.InsertColumn(5, _T("Progress Bar"), 100, ListCtrlEx::Progress, LVCFMT_CENTER, ListCtrlEx::SortByString);

for (int i=0; i <20; ++i)
{
	CString strItem;
	strItem.Format(_T("%d"), i+1);
	int nRow = m_list.InsertItem(m_list.GetItemCount(), strItem);

	int nSubCnt =m_list.GetColumnCount() -1;
	for (int j = 1; j <= nSubCnt; ++j)
	{
		CString sub;
		sub.Format(_T("sub item %d %d"), nRow, j);
		m_list.SetItemText(nRow, j, sub);
		if(ListCtrlEx::ComboBox == m_list.GetColumnType(j))
		{
			ListCtrlEx::CStrList strlist;
			strlist.push_back(sub);
			CString str;
			str.Format(_T("some other %d, %d"), nRow, j);
			strlist.push_back(str);
			m_list.SetCellStringList(nRow, j, strlist);
		}
		else if (ListCtrlEx::Progress == m_list.GetColumnType(j))
		{
			m_list.SetCellProgressMaxValue(nRow, j, 100*j);
			m_list.SetCellProgressValue(nRow, j, 100*nRow);

		}
		m_list.SetCellEnabled(nRow, j, (nRow != j));
	}
}

m_list.SetShowProgressPercent(TRUE);
m_list.SetSupportSort(TRUE);
*/
//////////////////////////////////////////////////////////////////////////
namespace ListCtrlEx
{
	using namespace std;

	enum CellType
	{
		Normal, // can show icon
		CheckBox, // can be checked
		RadioBox,  // can be checked
		ComboBox, // can be shown in place
		EditBox,  // can be shown in place
		Progress  // can show progress here
	};

	enum SortType
	{
		SortByString,
		SortByDigit,
		SortByDate
	};

	struct CheckCellMsg
	{
		CheckCellMsg(int nRow=-1, int nCol=-1, BOOL bChecked=TRUE)
			:m_nRow(nRow), m_nColumn(nCol), m_bChecked(bChecked)
		{}
		int m_nRow;
		int m_nColumn;
		BOOL m_bChecked;
	} ;

	struct EnableCellMsg 
	{
		EnableCellMsg(int nRow=-1, int nCol=-1, BOOL bEnabled=TRUE)
			:m_nRow(nRow), m_nColumn(nCol), m_bEnabled(bEnabled)
		{}
		int m_nRow;
		int m_nColumn;
		BOOL m_bEnabled;
	} ;
	// user messages
	#define WM_ListCtrlEx_LBUTTONDOWN		WM_USER+8001
	#define WM_ListCtrlEx_LBUTTONDBLCLK	WM_USER+8002
	#define WM_ListCtrlEx_RBUTTONDOWN		WM_USER+8003
	#define WM_ListCtrlEx_CHECKCELL			WM_USER+8004
	#define WM_ListCtrlEx_ENABLECELL			WM_USER+8005
	#define WM_ListCtrlEx_CHECK_CHANGED  WM_USER+8006
	#define WM_ListCtrlEx_ENABLE_CHANGED WM_USER+8007
	// User define message 
	// This message is posted to the parent
	// The message can be handled to make the necessary validations, if any
	#define WM_VALIDATE		WM_USER + 8008

	// User define message 
	// This message is posted to the parent
	// The message should be handled to spcify the items to the added to the combo
	#define WM_SET_ITEMS	WM_USER + 8009

	// CListCtrlEx
	class  CListCtrlEx : public CListCtrl
	{
		DECLARE_DYNAMIC(CListCtrlEx)
	public:
	
		struct CellData 
		{
		private:
			DWORD		m_dwData; // enabled at the first bit and checked at the second bit (count from the back)
			/*BOOL			m_bEnabled;
			BOOL			m_bChecked;*/
		public:
			#define Enable_Mask  0x00000001
			#define Check_Mask  0x00000002
			CellData(BOOL bEnabled=TRUE, BOOL bChecked=FALSE, int nRadioGroup=0, int nImage=-1, UINT uProgMaxVal=100) 
				: m_nRadioGroup(nRadioGroup), m_strValidChars(_T("")),
				m_nImage(nImage), m_uProgresVal(0), m_uProgressMax(uProgMaxVal)
			{
				SetEnable(bEnabled);
				SetCheck(bChecked);
			};

			int				m_nImage;
			
			int				m_nRadioGroup;
			CStrList		m_lstString;
			CString		m_strValidChars;
			UINT			m_uProgresVal;
			UINT			m_uProgressMax;

			inline void SetEnable(BOOL bEnbaled=TRUE) {
				if (bEnbaled)
					m_dwData |= Enable_Mask;
				else
					m_dwData &=(~Enable_Mask);
			}

			inline BOOL GetEnable() const {
				return (m_dwData&Enable_Mask);
			}

			inline void SetCheck(BOOL bChecked=TRUE){
				if(bChecked)
					m_dwData|=Check_Mask;
				else
					m_dwData &=(~Check_Mask);
			}

			inline BOOL GetCheck() const {
				return (m_dwData&Check_Mask);
			}
			
		};

		typedef CellType ColumnType;
		typedef map<int, ColumnType> ColMap; // column to column type
		typedef pair<int, int> CellIndex; // row, column
		typedef map<CellIndex, CellData> CellDataMap; // cell index(row, column) to cell data
		typedef map<CellIndex, CProgressCtrl*> ProgressMap;
	public:
		CListCtrlEx();
		virtual ~CListCtrlEx();
// overrides
	public:
		CImageList*	SetImageList( CImageList* pImageList, int nImageListType );
		int					InsertColumn(int nCol, CString strColHead, int nWidth=-1,ColumnType eColType=Normal, 
												int nFormat= LVCFMT_CENTER , SortType eSortBy=SortByString, int nSubItem= -1 );
		BOOL				DeleteColumn( int nCol );
		BOOL				DeleteAllItems( );
		BOOL				DeleteItem( int nItem );

// operations
	public:
		static BOOL		DrawGradientRect(CDC *pDC, CRect &rect, COLORREF crTopLeft, COLORREF crBottomRight);
		BOOL				GetCheckRect(int iRow, int iCol, CRect &rect);
		BOOL				GetCellRect(CellIndex ix, CRect &rect);
		BOOL				GetCellRect(int iRow, int iCol, CRect &rect, int nArea=LVIR_BOUNDS);
		BOOL				GetRowRect(int nRow, CRect &rcRow);
		BOOL				GetColRect(int nCol, CRect &rcCol);
		int					GetColumnCount() { return GetHeaderCtrl()->GetItemCount(); }
		int					GetRowCount() { return GetItemCount(); }

		ColumnType	GetColumnType(int nColIndex);
		void				SetColumnType(int nColIndex, ColumnType eColType);
		BOOL				IsSupportSort();
		void				SetSupportSort(BOOL bSuptSort=TRUE);
		SortType		GetColumnSortBy(int nColIndex);
		void				SetColumnSortBy(int nColIndex, SortType eSortBy=SortByString);
		BOOL				IsColumnSortAsc(int nColIndex);
		// for all cell types
		BOOL				GetCellEnabled(int nRow, int nCol);
		void				SetCellEnabled(int nRow, int nCol, BOOL bEnabled=TRUE);
		// for normal & edit box
		void				SetCellImage(int nRow, int nCol, int nImage=-1);
		// for radio & check box
		BOOL				GetCellChecked(int nRow, int nCol);
		void				SetCellChecked(int nRow, int nCol, BOOL bChecked=TRUE);
		// for radio only
		int					GetCellRadioGrp(int nRow, int nCol);
		void				SetCellRadioGrp(int nRow, int nCol, int nGroupNO=0);
		// for combo box only
		void				SetCellStringList(int nRow, int nCol, CStrList &lstStrings);
		void				AddCellString(int nRow, int nCol, CString &str);
		// for edit box only
		void				SetCellValidChars(int nRow, int nCol, CString &strValidChars);
		// for progress bar only
		BOOL				IsShowPogressPercent();
		void				SetShowProgressPercent(BOOL bShowProgPercent=TRUE);
		void				SetCellProgressMaxValue(int nRow, int nCol, UINT uMaxValue=100);
		void				SetCellProgressValue(int nRow, int nCol, UINT uValue);
		//
		CellIndex			Point2Cell(const CPoint &point);

	protected:
		virtual void		PreSubclassWindow();
		DECLARE_MESSAGE_MAP()
		// message handlers
		afx_msg void	OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
		afx_msg void	OnLButtonDown(UINT nFlags, CPoint point);
		afx_msg void	OnLButtonDblClk(UINT nFlags, CPoint point);
		afx_msg void	OnRButtonDown(UINT nFlags, CPoint point);
		afx_msg void	OnLvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
		afx_msg void	OnLvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
		afx_msg void	OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult);
		afx_msg void	OnDestroy();
	
// implementation
	private:
		inline void		DrawCell(CDC *pDC,  CString &strText, CRect &rcCell, BOOL bSelected, 
											int nRow, int nCol, LRESULT *pResult);
		void				DrawNormal(CDC *pDC, CString &strText, CRect &rcCell, BOOL bSelected, const CellData &cellData, int uLvcFmt=LVCFMT_CENTER);
		void				DrawCheckBox(CDC *pDC,  CString &strText, CRect &rcCell, BOOL bSelected, const CellData &cellData);	
		void				DrawRadioBox(CDC *pDC,  CString &strText, CRect &rcCell, BOOL bSelected, const CellData &cellData);
		void				DrawComboBox(CDC *pDC,  CString &strText, CRect &rcCell, BOOL bSelected, const CellData &cellData);
		void				DrawEditBox(CDC *pDC,  CString &strText, CRect &rcCell, BOOL bSelected, const CellData &cellData, int uLvcFmt=LVCFMT_CENTER);
		void				DrawProgress(CDC *pDC,  CString &strText, CRect &rcCell, BOOL bSelected, const CellData &cellData);

		int				    GetColumnFmt(int nCol);
		inline void		ShowCellInPlace(CellIndex ix, CellType eCellType);
		void				ShowInPlaceCombo(CellIndex ix);
		void				ShowInPlaceEdit(CellIndex ix);
	private:	
		void				SetColumnSortDirection(int nColIndex, BOOL bIsAsc=TRUE);
		int					GetProgBarSize(const CellData &cellData, CRect &rcProg, float *pPersent=NULL);
		CImageList*	TheImageList();
		CellData&		FindCellData(CellIndex ix);
		CellData&		FindCellData(int nRow, int nCol);
		CellDataMap	m_mapCell2Data;
		ColMap			m_mapCol2ColType;
		DWORD			m_dwEditStyle;
		DWORD			m_dwComboStyle;
		int					m_nImageListType;
		DWORD			m_dwListCtrlExStyle; // 

	protected:
		struct _ColumnSort_t
		{
			_ColumnSort_t(SortType eSortType=SortByString, BOOL bIsAsc=TRUE)
				:m_eSortType(eSortType), m_bIsAsc(bIsAsc)
			{};
			SortType	m_eSortType;
			BOOL			m_bIsAsc;
		};

		struct _SortParam_t 
		{
			_ColumnSort_t  m_ColParam;
			int					 m_nSortCol;
			CListCtrlEx		 *m_pTheList;
		};
		typedef map<int, _ColumnSort_t> ColSortMap;
		ColSortMap				m_mapCol2Sort;	
		CListCtrlExSortHead	m_ctlSortHead;
		static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2,  LPARAM lParamSort);
		static bool		IsDigitStr(LPCTSTR str);
		static int		IntCompare(LPCTSTR strInt1st, LPCTSTR strInt2nd);
		static int		StrCompare(LPCTSTR str1st, LPCTSTR str2nd);
		void				PreSortItems( vector<DWORD_PTR> &vec2StoreData );
		void				PostSortItems( vector<DWORD_PTR> &vec2StoreData );
};

}
#endif