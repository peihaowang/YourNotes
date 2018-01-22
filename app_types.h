
#ifndef APP_TYPES_H
#define APP_TYPES_H

///////////////////////////////////////////////////////////

typedef		long				_TCount;
typedef		long				_TSize;

typedef		long				_TItemID;
#define		_ITEM_ID_INVALID		((_TItemID)-1)

typedef		long				_TQueryID;
#define		_QUERY_ID_RESERVED		300 //Reserve some IDs for internal use;
#define		_QUERY_ID_INVALID		((_TQueryID)-1)

typedef		long				_TIconID;
#define		_ICON_ID_INVALID		((_TIconID)-1)
#define		_ICON_ID_MIN			((_TIconID)0)

typedef		long				_TBkmkID;
#define		_BKMK_ID_RESERVED		300 //Reserve some IDs for internal use;
#define		_BKMK_ID_INVALID		((_TBkmkID)-1)

typedef		void *				H_INDEX;
typedef		uint				_TIdxID;
typedef		int				_TOccurNum;
#define		_INDEX_ID_INVALID		((_TIdxID)-1)

typedef		long				_TStyleID;
#define		_STYLE_ID_RESERVED		300 //Reserve some IDs for internal use;
#define		_STYLE_ID_INVALID		((_TStyleID)-1)

///////////////////////////////////////////////////////////

#endif
