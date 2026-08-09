/////////////////////////////////////////////////////////////////////
//
// グローバル変数
//
/////////////////////////////////////////////////////////////////////

char*	GetPszApplicationName(void);
char*	GetPszClassName(void);
HINSTANCE GetHInstance(void);
void SetHInstance( HINSTANCE hInstance );
void SetHWnd( HWND hWnd );
HWND GetHWnd(void);
void SetHMutex( HANDLE hMutex );
HANDLE GetHMutex(void);

	/////////////////////////////////////////////////////////////////
	//
	// クライアント領域のスタティックテキスト関連
	//

void SetHStaticMain( HWND hEditMain );
HWND GetHStaticMain(void);

void SetStaticMainMargin( int iStaticMainMargin );
int GetStaticMainMargin(void);

//////////////////////////////////////////////////////////////////////

LRESULT CALLBACK WindowProc( HWND   	hWnd,
							 UINT   	uiMessage,
							 WPARAM 	wParam,
							 LPARAM 	lParam );

