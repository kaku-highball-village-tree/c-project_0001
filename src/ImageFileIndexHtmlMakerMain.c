#if 0
#define ENTRY_POINT_IS_WINMAIN
#endif

#if 0
#define ENTRY_POINT_IS_MAIN_ARGUMENT3
#endif

#if 0
#define ENTRY_POINT_IS_MAIN_ARGUMENT2
#endif

#include <windows.h>
// #include <mmsystem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
char			*g_pszAppName = "ImageFileIndexHtmlMaker ` 摜index.html쐬 Ver1.00";
// #include <float.h>		// DBL_MAX, DBL_MINを使用する場合に必要となる。
// #include <math.h>		// HUGE_VALを使用する場合に必要となる。
#include "WindowLayoutOperation.h"
#include "ImageFileIndexHtmlMakerMain.h"

/////////////////////////////////////////////////////////////////////
//
// プリプロセッサ
//
/////////////////////////////////////////////////////////////////////

#define	ID_MAIN_STATIC		( WM_APP + 1 )

/////////////////////////////////////////////////////////////////////
//
// グローバル変数
//
/////////////////////////////////////////////////////////////////////

char			*g_pszAppName = "FileListMaker ～ ファイル一覧作成 Ver1.00";

char*	GetPszApplicationName(void)
{
	return g_pszAppName;
}

char			*g_pszClassName = "Muraki_Prog";

char*	GetPszClassName(void)
{
	return g_pszClassName;
}

HINSTANCE		g_hInstance;

HINSTANCE GetHInstance(void)
{
	return g_hInstance;
}

void SetHInstance( HINSTANCE hInstance )
{
	g_hInstance = hInstance;

	return;
}

HWND			g_hWnd;

void SetHWnd( HWND hWnd )
{
	g_hWnd = hWnd;
}

HWND GetHWnd(void)
{
	return g_hWnd;
}

HANDLE			g_hMutex;

void SetHMutex( HANDLE hMutex )
{
	g_hMutex = hMutex;
}

HANDLE GetHMutex(void)
{
	return g_hMutex;
}

	/////////////////////////////////////////////////////////////////
	//
	// クライアント領域のスタティックテキスト関連
	//

HWND			g_hStaticMain;

void SetHStaticMain( HWND hStaticMain )
{
	g_hStaticMain = hStaticMain;
}

HWND GetHStaticMain(void)
{
	return g_hStaticMain;
}

int			g_iStaticMainMargin;

void SetStaticMainMargin( int iStaticMainMargin )
{
	g_iStaticMainMargin = iStaticMainMargin;
}

int GetStaticMainMargin(void)
{
	return g_iStaticMainMargin;
}

char const		*pszUsage = "ファイルをここにドラッグ＆ドロップして下さい。\n";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// エントリポイント
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(ENTRY_POINT_IS_WINMAIN)

int WINAPI WinMain( HINSTANCE		hInstance, 
					HINSTANCE		hPrevInstance, 
					LPSTR			lpCmdLine, 
					int				nCmdShow )
{

#elif defined(ENTRY_POINT_IS_MAIN_ARGUMENT3) && !defined(ENTRY_POINT_IS_WINMAIN)

int main( int argc, char *argv[], char *envp[] )
{
	HINSTANCE			hInstance												 = GetModuleHandle(NULL);
	HINSTANCE			hPrevInstance											 = NULL;
	LPSTR				lpCmdLine												 = NULL;
	int					nCmdShow												 = 0;

#elif defined(ENTRY_POINT_IS_MAIN_ARGUMENT2) && !defined(ENTRY_POINT_IS_WINMAIN)

int main( int argc, char *argv[] )
{
	HINSTANCE			hInstance												 = GetModuleHandle(NULL);
	HINSTANCE			hPrevInstance											 = NULL;
	LPSTR				lpCmdLine												 = NULL;
	int					nCmdShow												 = 0;

#else

int main(void)
{
	HINSTANCE			hInstance												 = GetModuleHandle(NULL);
	HINSTANCE			hPrevInstance											 = NULL;
	LPSTR				lpCmdLine												 = NULL;
	int					nCmdShow												 = 0;

#endif
	//////////////////////////////////////////////////////////////////////
	//
	// WinMain関数のローカル変数
	//

	HWND				hWnd													 = NULL;
	MSG					objMsg;
	WNDCLASSEX			objWndClassEx;
	RECT				rectWnd													 = { 0, 0, 0, 0 };

#if defined(ENTRY_POINT_IS_WINMAIN)
	LPWSTR*				pszArgvWide												 = NULL;
	LPSTR*				pszArgv													 = NULL;
	int					iBufferSize												 = 0;
#elif defined(ENTRY_POINT_IS_MAIN_ARGUMENT3) && !defined(ENTRY_POINT_IS_WINMAIN)
	LPSTR*				pszArgv													 = NULL;
#elif defined(ENTRY_POINT_IS_MAIN_ARGUMENT2) && !defined(ENTRY_POINT_IS_WINMAIN)
	LPSTR*				pszArgv													 = NULL;
#else
	/*
	 * エントリポイントが「int main(void)」の場合，何もしない。
	 */
#endif

	int					iArgc													 = 0;

	int					i														 = 0;
	int					j														 = 0;
	int					iReturn													 = 0;
	BOOL				bReturn													 = FALSE;

	//////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////
	//
	// コマンドラインをiArgc, pszArgyに設定する。
	//
	//////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////

#if defined(ENTRY_POINT_IS_WINMAIN)

		/*
		 * Unicodeワイド文字で渡されたコマンドライン文字列を解析し，
		 * Unicodeワイド文字引数のリスト（pszArgvWide）と引数の数（iArgc）を取得する。
		 */
	pszArgvWide = CommandLineToArgvW( GetCommandLineW(), &iArgc );
	if( pszArgvWide == NULL ){
			/*
			 * コマンドラインの取得に失敗した場合
			 */

			/*
			 * メッセージループに入る前であるため，0を返す。
			 *
			 * 1. 「関数がメッセージループに入る前に終了する場合は，0を返してください。」
			 */
		return 0;
	}

	pszArgv = HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, iArgc * sizeof(char*) );
	if( pszArgv == NULL ){
			/*
			 * ヒープからのメモリブロックの割当てに失敗した場合
			 */

			/*
			 * メッセージループに入る前であるため，0を返す。
			 *
			 * 1. 「関数がメッセージループに入る前に終了する場合は，0を返してください。」
			 */
		return 0;
	}

	for( i = 0; i < iArgc; i++ ){
			/*
			 * 各要素に必要なバッファサイズを計算する。
			 */
		iBufferSize = WideCharToMultiByte( CP_ACP, 0, pszArgvWide[i], -1, NULL, 0, NULL, NULL );
		if( iBufferSize < 2 ){
			for( j = 0; j < i; j++ ){
					/*
					 * ヒープから割り当てたメモリブロックを解放する。
					 */
				HeapFree( GetProcessHeap(), 0, pszArgv[i] );
			}

				/*
				 * ヒープから割り当てたメモリブロックを解放する。
				 */
			HeapFree( GetProcessHeap(), 0, pszArgv );
				/*
				 * メッセージループに入る前であるため，0を返す。
				 *
				 * 1. 「関数がメッセージループに入る前に終了する場合は，0を返してください。」
				 */
			return 0;
		}

			/*
			 * 各要素の領域を確保する。
			 */
		pszArgv[i] = HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, iBufferSize );
		if( pszArgv[i] == NULL ){
			for( j = 0; j < i; j++ ){
					/*
					 * ヒープから割り当てたメモリブロックを解放する。
					 */
				HeapFree( GetProcessHeap(), 0, pszArgv[j] );
			}

				/*
				 * ヒープから割り当てたメモリブロックを解放する。
				 */
			HeapFree( GetProcessHeap(), 0, pszArgv );
				/*
				 * メッセージループに入る前であるため，0を返す。
				 *
				 * 1. 「関数がメッセージループに入る前に終了する場合は，0を返してください。」
				 */
			return 0;
		}

			/*
			 * 各要素毎に，UNICODE文字列をANSIコードに変換してコピーする。
			 */
		iReturn = WideCharToMultiByte( CP_ACP, 0, pszArgvWide[i], -1, pszArgv[i], iBufferSize, NULL, NULL );
		if( iReturn == 0 ){
			LPVOID		lptstrErrorMessageString;
			char		szMessage[1024+1]											 = { 0, };

			FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
						   NULL, 
						   GetLastError(), 
						   MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), 
						   (LPTSTR)&lptstrErrorMessageString, 
						   0, 
						   NULL );
			if( strlen( lptstrErrorMessageString ) < 1024 ){
				sprintf( szMessage, 
						 "エラーコード：%d\n"
						 "エラーメッセージ：%s\n", 
						 GetLastError(), 
						 lptstrErrorMessageString );
				MessageBox( GetFocus(), 
							szMessage, 
							GetPszApplicationName(), 
							MB_OK );
			}
			else{
				MessageBox( GetFocus(), 
							lptstrErrorMessageString, 
							GetPszApplicationName(), 
							MB_OK );
			}
			LocalFree( lptstrErrorMessageString );

			for( j = 0; j < i; j++ ){
					/*
					 * ヒープから割り当てたメモリブロックを解放する。
					 */
				HeapFree( GetProcessHeap(), 0, pszArgv[j] );
			}

				/*
				 * ヒープから割り当てたメモリブロックを解放する。
				 */
			HeapFree( GetProcessHeap(), 0, pszArgv );
				/*
				 * メッセージループに入る前であるため，0を返す。
				 *
				 * 1. 「関数がメッセージループに入る前に終了する場合は，0を返してください。」
				 */
			return 0;
		}
	}

	/*
	 *
	 * //#elif defined(ENTRY_POINT_IS_MAIN_ARGUMENT3) && !defined(ENTRY_POINT_IS_WINMAIN)
	 * //#elif defined(ENTRY_POINT_IS_MAIN_ARGUMENT2) && !defined(ENTRY_POINT_IS_WINMAIN)
	 *
	 */
#elif defined(ENTRY_POINT_IS_MAIN_ARGUMENT3) || defined(ENTRY_POINT_IS_MAIN_ARGUMENT2) && !defined(ENTRY_POINT_IS_WINMAIN)

	iArgc = argc;

	pszArgv = HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, iArgc * sizeof(char*) );
	if( pszArgv == NULL ){
			/*
			 * ヒープからのメモリブロックの割当てに失敗した場合
			 */

			/*
			 * メッセージループに入る前であるため，0を返す。
			 *
			 * 1. 「関数がメッセージループに入る前に終了する場合は，0を返してください。」
			 */
		return 0;
	}

	for( i = 0; i < iArgc; i++ ){
		pszArgv[i] = HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, (MAX_PATH) * sizeof(char) );
		if( pszArgv[i] == NULL ){
			for( j = 0; j < i; j++ ){
					/*
					 * ヒープから割り当てたメモリブロックを解放する。
					 */
				HeapFree( GetProcessHeap(), 0, pszArgv[j] );
			}

				/*
				 * ヒープから割り当てたメモリブロックを解放する。
				 */
			HeapFree( GetProcessHeap(), 0, pszArgv );
			return 0;
		}
	}

	for( i = 0; i < iArgc; i++ ){
		strcpy( pszArgv[i], (LPSTR)argv[i] );
	}

#else
	/*
	 * エントリポイントが「int main(void)」の場合，何もしない。
	 */
#endif

#if 0
	HWND			hWnd;
	MSG 			msg;
	WNDCLASSEX  	wcl;
	RECT			rectWnd;
#endif


#if 1
	//////////////////////////////////////////////////////////////////////
	//
	// 二重起動された場合，メッセージを表示し，このプログラムを起動させない。
	//
		/*
		 * 
		 */
	if( 0 == SelectApplicationDuplicateStartingMessageBox( NULL, GetPszClassName(), GetPszApplicationName(), NULL ) ){
		// そのまま起動する。
	}
	else{
			/*
			 * 二重起動させないため，このプログラムを起動せず，終了する。
			 */
		return 0;
	}
#elif
//
//		/*
//		 * 二重起動された場合，
//		 */
//	if( ForbidDoubleRunning( GetPszClassName(), GetPszApplicationName(), 0 ) ){
//			/*
//			 * 二重起動されたウィンドウを閉じる。
//			 */
//		return FALSE;
//	}
//
#endif

		/*
		 * プロセスのハンドルを保存する。
		 */
	SetHInstance( hInstance );


	objWndClassEx.hInstance = GetHInstance();
	objWndClassEx.lpszClassName = GetPszClassName();
	objWndClassEx.lpfnWndProc = WindowProc;
		/*
		 * ダブルクリックを認識するウィンドウにする。
		 */
	objWndClassEx.style = CS_DBLCLKS;

	objWndClassEx.cbSize = sizeof(WNDCLASSEX);
	objWndClassEx.hIcon = LoadIcon( GetHInstance(), "MURAKI_PROG_ICON" );
//	objWndClassEx.hIcon = LoadIcon( NULL, IDI_APPLICATION );
	objWndClassEx.hIconSm = LoadIcon( GetHInstance(), "MURAKI_PROG_ICON" );
//	objWndClassEx.hIconSm = LoadIcon( NULL, IDI_WINLOGO );
	objWndClassEx.hCursor = LoadCursor( NULL, IDC_ARROW );

	objWndClassEx.lpszMenuName = NULL;
	objWndClassEx.cbClsExtra = 0;
	objWndClassEx.cbWndExtra = 0;
//	objWndClassEx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	objWndClassEx.hbrBackground = (HBRUSH)( COLOR_INACTIVEBORDER+1 );

	if( !RegisterClassEx(&objWndClassEx) )
		return 0;

	hWnd = CreateWindow( GetPszClassName(),
						 GetPszApplicationName(),
						 WS_OVERLAPPEDWINDOW,
						 CW_USEDEFAULT,
						 CW_USEDEFAULT,
						 CW_USEDEFAULT,
						 CW_USEDEFAULT,
						 HWND_DESKTOP,
						 NULL,
						 GetHInstance(),
						 NULL );
		/*
		 * メインウィンドウのハンドルを設定する。
		 */
	SetHWnd( hWnd );

	//////////////////////////////////////////////////////////////////
	//
	// 引数をチェックする。
	//
	// このプログラムの実行ファイルにファイルがドラッグされた場合，
	//
#if defined(ENTRY_POINT_IS_WINMAIN)
	if( 0 < strlen(lpCmdLine) ){
		ExecuteSpecifiedArgument( hWnd, lpCmdLine );
		return 0;
	}

	/*
	 *
	 * //#elif defined(ENTRY_POINT_IS_MAIN_ARGUMENT3) && !defined(ENTRY_POINT_IS_WINMAIN)
	 * //#elif defined(ENTRY_POINT_IS_MAIN_ARGUMENT2) && !defined(ENTRY_POINT_IS_WINMAIN)
	 *
	 */
#elif defined(ENTRY_POINT_IS_MAIN_ARGUMENT3) || defined(ENTRY_POINT_IS_MAIN_ARGUMENT2) && !defined(ENTRY_POINT_IS_WINMAIN)
	if( 0 < strlen(pszArgv) ){
printf( "[imagefileindexhtmlmakermain.c]の[513行目]\n" );
		ExecuteSpecifiedArgument( hWnd, pszArgv );
printf( "[imagefileindexhtmlmakermain.c]の[514行目]\n" );
		return 0;
	}


#else
	/*
	 * エントリポイントが「int main(void)」の場合，何もしない。
	 */
#endif

	//
	//////////////////////////////////////////////////////////////////

		/*
		 * ウィンドウを3分割の大きさでデスクトップの中央に
		 * 表示する矩形値を計算する。
		 */
#if 0
//	rectWnd = GetGoldenSectionWindowRectDesktopCenterEx2( 3, 4 );
#endif
				/*
				 * 起動したエディタのじゃまにならないように，プログラム本体を移動する。
				 *
				 * ウィンドウの位置は，デスクトップの右下隅となるように，
				 * ウィンドウのサイズは，デスクトップの横 4分割・縦 4分割を基準として，
				 * 黄金分割により，ウインドウの矩形を計算する。
				 */
	rectWnd = GetWindowRectDesktopLowerRightPositionAndProportionOfDesktopToWindowHeightSizeWidthSizeBasedGoldenSectionWindowRect( 3, 4, FALSE );

printf( "[imagefileindexhtmlmakermain.c]の[525行目]\n" );
		/*
		 * 非アクティブの状態でも最前面にウィンドウを表示する。
		 *     (rect.left, rect.top)
		 *     (    0    ,    0    )
		 *           ↓
		 *            +---------------------------+
		 *            |                           |
		 *            |          RECT             |
		 *            |          矩形             |
		 *            |                           |
		 *            +---------------------------+
		 *                                        ↑
		 *                             (rect.right, rect.bottom)
		 *            <===========================>
		 *             rectWnd.right - rectWnd.left
		 */

	SetWindowPos( hWnd,
				  HWND_TOPMOST,
				  rectWnd.left,
				  rectWnd.top,
				  rectWnd.right - rectWnd.left,
				  rectWnd.bottom - rectWnd.top,
				  SWP_SHOWWINDOW  );

#if 1
	ShowWindow( GetHWnd(), SW_SHOWDEFAULT );
#else
	ShowWindow( hWnd, nWinMode );
#endif

	UpdateWindow(hWnd);

	while( GetMessage( &objMsg, NULL, 0, 0 ) ){
		TranslateMessage(&objMsg);
		DispatchMessage(&objMsg);
	}

	return objMsg.wParam;
}

LRESULT CALLBACK WindowProc( HWND   	hWnd,
							 UINT   	uiMessage,
							 WPARAM 	wParam,
							 LPARAM 	lParam )
{
	int		iReturn;

	switch(uiMessage){
		case WM_CREATE:
				/*
				 * ドラッグアンドドロップを許可する。
				 * （許可しない場合，FALSE）
				 */
			DragAcceptFiles( hWnd, TRUE );

			//////////////////////////////////////////////////////////
			//
			// クライアント領域のスタティックテキスト
			//
			{
				RECT	rectCliant;
				HWND	hStaticMain;

					/*
					 * クライアント領域の矩形を取得する。
					 */
				GetClientRect( hWnd, &rectCliant );
				SetStaticMainMargin( 10 );

				hStaticMain = CreateWindow( "STATIC",
										  NULL,
										  WS_CHILD
										   | WS_VISIBLE
										   | SS_LEFT
										   | SS_ENDELLIPSIS,
										  0,
										  0,
										  rectCliant.right,
										  rectCliant.bottom,
										  hWnd,
										  (HMENU)ID_MAIN_STATIC,
										  GetHInstance(),
										  NULL );
				SetHStaticMain( hStaticMain );

				SendMessage( GetHStaticMain(), WM_SETTEXT, 0, (LPARAM)pszUsage );
			}
			break;

		case WM_SIZE: {
			RECT	rectCliant;

				/*
				 * クライアント領域の矩形を取得する。
				 */
			GetClientRect( hWnd, &rectCliant );

			MoveWindow( GetHStaticMain(),
						rectCliant.left + GetStaticMainMargin(),
						rectCliant.top + GetStaticMainMargin(),
						rectCliant.right - 2 * GetStaticMainMargin(),
						rectCliant.bottom - 2 * GetStaticMainMargin(),
						TRUE );
			SendMessage( GetHStaticMain(), WM_SETTEXT, 0, (LPARAM)pszUsage );

			break;
		}

		case WM_DROPFILES:

				/*
				 * ドラッグアンドドロップの後の処理
				 *
				 * ちなみに，hWndは，別にいらない。
				 */
			iReturn = AfterDragAndDrop( hWnd, (HDROP)wParam );
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc( hWnd, uiMessage, wParam, lParam );
	}

	return 0;
}
