#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FileCommonOperation.h"
#include "FileFullPathOperation.h"
#include "StringCommonOperation.h"
#include "OtherWindowOperation.h"
#include "DynamicArrayOperation.h"
#include "ImageFileIndexHtmlMakerMain.h"

int ExecuteSpecifiedArgument( HWND hWnd, char const *pszArgument );
int AfterDragAndDrop( HWND hWnd, HDROP hDrop );
int DragAndFileList( const char *pszFullPathShort );
int DragAndUrlFileList( const char *pszFullPathShort );
int DragAndUrlIndexHtml( const char *pszFullPathShort );

//////////////////////////////////////////////////////////////////////
//
// 引数を指定して実行した場合
//   又は，アイコンにファイルをドラッグした場合
//

int ExecuteSpecifiedArgument( HWND hWnd, char const *pszArgument )
{
	char	**pszFileFullPathShort								 = NULL;
	int		iFileCount											 = 0;

		//////////////////////////////////////////////////////////////
		//
		// 標準のエディタで開くもの
		//
	char	**pszFileFullPathLong								 = NULL;
	char	*pszFileList										 = NULL;
	char	*pDivideMark										 = NULL;

		//////////////////////////////////////////////////////////////
		//
		// その他のもの
		//
	int		i													 = 0;
	int		iReturn												 = 0;

	//////////////////////////////////////////////////////////////////
	//
	// 引数をチェックする。
	//

	if( pszArgument == NULL ){
		return -1;
	}

	if( strlen( pszArgument ) < 1 ){
		return -2;
	}

	//////////////////////////////////////////////////////////////////
	//
	// 引数に含まれるファイルの数を計算する。
	//
	// 部分文字列の数を計算する（両端入れ）。
	//

#if 0
//	iFileCount = String2DivideStringsCount( pszArgument, 0x20 );
#endif

	iFileCount = GetPartStringCountBasedOnDelimiterCharacterInFullStringBothEndsIncluded( pszArgument, 0x20 );
	if( 1 < iFileCount ){ // 両端入れなので，部分文字列は最低２個ある。
		/*
		 * 区切り文字が存在するので，下で処理する。
		 */
	}
	else if( iFileCount < 0 ){
		return -3;
	}
	else{
			/*
			 * ここに来ることはないはずである。
			 */
		return -4;
	}


	//////////////////////////////////////////////////////////////////
	//
	// 引数に含まれるファイルの一覧を格納する領域を確保する。
	//

	pszFileFullPathShort = CreateCharArray2D( iFileCount, ((_MAX_PATH)+1) );
	pszFileFullPathLong = CreateCharArray2D( iFileCount, ((_MAX_PATH)+1) );

	//////////////////////////////////////////////////////////////////
	//
	// 引数に含まれるファイルの一覧を作成する。
	//
	// 部分文字列を取得する。
	//
//int DivideFullString2PartStringBasedOnDelimiterCharacter( char const		*pszOriginalString, 
//														  int const			iOriginalDelimiterCharacter, 
//														  char 				**pszDividedStringArray, 
//														  int const			iDividedStringArrayCount, 
//														  int const			iDividedStringArrayLength )

	//////////////////////////////////////////////////////////////////
	//
	// 部分文字列を取得する。
	//

	iReturn = DivideFullString2PartStringBasedOnDelimiterCharacter( pszArgument, 
												   0x20,
												   pszFileFullPathShort,
												   iFileCount,
												   ((_MAX_PATH)+1) );

#if 0
//	iReturn = DivideString2Strings( pszArgument,
//									0x20,
//									pszFileFullPathShort,
//									iFileCount,
//									((_MAX_PATH)+1) );
#endif

	if( iReturn == 0 ){
		iReturn = IDNO;

		if( iReturn == IDCANCEL ){
			return 1;
		}
		else if( iReturn == IDYES ){
			for( i = 0; i < iFileCount; i++ ){
				DragAndUrlFileList( pszFileFullPathShort[i] );
			}
		}
		else if( iReturn == IDNO ){
			for( i = 0; i < iFileCount; i++ ){
				DragAndFileList( pszFileFullPathShort[i] );
			}
		}
		else{

		}
	}

	//////////////////////////////////////////////////////////////////
	//
	// 引数に含まれるファイルの一覧をメモ帳で開く。
	//

	pszFileList = CreateCharArray1D( iFileCount * _MAX_PATH );

	for( i = 0; i < iFileCount; i++ ){
		ShortFullPath2LongFullPath( pszFileFullPathShort[i], pszFileFullPathLong[i] );
		strcat( pszFileList, pszFileFullPathLong[i] );
		strcat( pszFileList, "\r\n" );
	}

	OpenNotePadSpecifiedString( pszFileList );

	DeleteCharArray1D( pszFileList );

	//////////////////////////////////////////////////////////////////
	//
	// 引数に含まれるファイルの一覧を格納する領域を解放する。
	//

	DeleteCharArray2D( pszFileFullPathShort, iFileCount );
	DeleteCharArray2D( pszFileFullPathLong, iFileCount );

	return 0;
}

//////////////////////////////////////////////////////////////////////
//
// ファイルをドラッグした場合
//

int AfterDragAndDrop( HWND hWnd, HDROP hDrop )
{
#define DRAGANDDROP_FILE_COUNT_MAX 3000
#define MESSAGEBOX_MESSAGE_MAX 1024
	int			i														 = 0;
	UINT		uiFileCount												 = 0;
	char		szFullPathShort[_MAX_PATH+1]							 = { 0, };
	int			iReturn													 = 0;
	char		szMessage[MESSAGEBOX_MESSAGE_MAX+1]						 = { 0, };

	char		szFileName[((_MAX_PATH)+1)]								 = { 0, };
	char		szDesktopFullPath[((_MAX_PATH)+1)]						 = { 0, };

	FILE		*fpInputFile											 = NULL;
	FILE		*fpOutputFile											 = NULL;
	char		szInputFile[((_MAX_PATH)+1)]							 = { 0, };
	char		szOutputFile[((_MAX_PATH)+1)]							 = { 0, };

	GetDesktopFullPath( szDesktopFullPath, ((_MAX_PATH)+1) );
	sprintf( szOutputFile, "%s\\style_index.css", szDesktopFullPath );

printf( "[imagefileindexhtmlmaker.c]の[199行目]\n" );
printf( "szOutputFile = [%s]\n", szOutputFile );
	fpOutputFile = fopen( szOutputFile, "w" );
	if( fpOutputFile == NULL ){
		return 1;
	}

	fprintf( fpOutputFile, "/* style_index.css */\n" );
	fprintf( fpOutputFile, "/* 見出しの基本設定。本文の体裁は既定のまま。 */\n" );
	fprintf( fpOutputFile, "/* 使用文字は ASCII と第一・第二水準漢字のみ。絵文字・機種依存文字は不使用。 */\n" );
	fprintf( fpOutputFile, "\n" );
	fprintf( fpOutputFile, "/* 見出しの基本設定（横線なし版） */\n" );
	fprintf( fpOutputFile, "h1, h2, h3, h4 {\n" );
	fprintf( fpOutputFile, "  font-family: system-ui, -apple-system, \"Segoe UI\", Meiryo, \"Yu Gothic UI\", sans-serif;\n" );
	fprintf( fpOutputFile, "  line-height: 1.35;\n" );
	fprintf( fpOutputFile, "  color: #222;\n" );
	fprintf( fpOutputFile, "  margin: 1.2em 0 0.6em;\n" );
	fprintf( fpOutputFile, "}\n" );
	fprintf( fpOutputFile, "\n" );
	fprintf( fpOutputFile, "/* H1：左線のみ。横線や帯は付けない */\n" );
	fprintf( fpOutputFile, "h1 {\n" );
	fprintf( fpOutputFile, "  font-size: 2.0rem;\n" );
	fprintf( fpOutputFile, "  font-weight: 700;\n" );
	fprintf( fpOutputFile, "  border-left: 0.5rem solid #222;\n" );
	fprintf( fpOutputFile, "  padding-left: 0.6rem;\n" );
	fprintf( fpOutputFile, "  background-image: none;          /* 横方向の淡い帯を無効化 */\n" );
	fprintf( fpOutputFile, "}\n" );
	fprintf( fpOutputFile, "\n" );
	fprintf( fpOutputFile, "/* H2：横線なし */\n" );
	fprintf( fpOutputFile, "h2 {\n" );
	fprintf( fpOutputFile, "  font-size: 1.6rem;\n" );
	fprintf( fpOutputFile, "  font-weight: 700;\n" );
	fprintf( fpOutputFile, "  padding-bottom: 0;               /* 下線をなくすため余白も調整 */\n" );
	fprintf( fpOutputFile, "  border-bottom: none;\n" );
	fprintf( fpOutputFile, "}\n" );
	fprintf( fpOutputFile, "\n" );
	fprintf( fpOutputFile, "/* H3：横線なし */\n" );
	fprintf( fpOutputFile, "h3 {\n" );
	fprintf( fpOutputFile, "  font-size: 1.3rem;\n" );
	fprintf( fpOutputFile, "  font-weight: 600;\n" );
	fprintf( fpOutputFile, "  padding-bottom: 0;\n" );
	fprintf( fpOutputFile, "  border-bottom: none;\n" );
	fprintf( fpOutputFile, "}\n" );
	fprintf( fpOutputFile, "\n" );
	fprintf( fpOutputFile, "/* H4：薄い帯＋左線（横線なし） */\n" );
	fprintf( fpOutputFile, "h4 {\n" );
	fprintf( fpOutputFile, "  font-size: 1.15rem;\n" );
	fprintf( fpOutputFile, "  font-weight: 600;\n" );
	fprintf( fpOutputFile, "  padding: 0.15em 0.4em;\n" );
	fprintf( fpOutputFile, "  background-color: #f4f4f4;\n" );
	fprintf( fpOutputFile, "  border-left: 0.3rem solid #888;\n" );
	fprintf( fpOutputFile, "}\n" );
	fprintf( fpOutputFile, "\n" );
	fprintf( fpOutputFile, "/* 本文用の基本設定 */\n" );
	fprintf( fpOutputFile, "p {\n" );
	fprintf( fpOutputFile, "  margin: 0 0 1em;      /* 段落下に余白を付ける */\n" );
	fprintf( fpOutputFile, "  line-height: 1.8;     /* 行間 */\n" );
	fprintf( fpOutputFile, "}\n" );
	fprintf( fpOutputFile, "\n" );
	fprintf( fpOutputFile, "ul, ol {\n" );
	fprintf( fpOutputFile, "  margin: 0 0 1em 1.5em; /* 箇条書きにも少し余白 */\n" );
	fprintf( fpOutputFile, "}\n" );
	fclose( fpOutputFile );

printf( "[imagefileindexhtmlmaker.c]の[261行目]\n" );
		/*
		 * ドラッグアンドドロップされたファイルの数を取得する。
		 */

	uiFileCount = DragQueryFile( hDrop, 0xFFFFFFFF, NULL, 0 );

		/*
		 * ドラッグアンドドロップされたファイルの数毎の処理
		 */

	if( uiFileCount == 1 ){
/*
		iReturn = MessageBox( hWnd,
							  "\"file://\"を付加してURLにしますか",
							  GetPszApplicationName(),
							  MB_YESNOCANCEL | MB_ICONQUESTION | MB_DEFBUTTON1 );
*/
		iReturn = IDNO;

		DragQueryFile( hDrop, 0, szFullPathShort, sizeof(szFullPathShort) );

		if( iReturn == IDCANCEL ){
			return 1;
		}
		else if( iReturn == IDYES ){
			DragAndUrlFileList(szFullPathShort);
		}
		else if( iReturn == IDNO ){
			DragAndFileList(szFullPathShort);
printf( "[imagefileindexhtmlmaker.c]の[290行目]\n" );
		}
		else{

		}
	}
	else if( uiFileCount <= DRAGANDDROP_FILE_COUNT_MAX ){
/*
		iReturn = MessageBox( hWnd,
							  "\"file://\"を付加してURLにしますか",
							  GetPszApplicationName(),
							  MB_YESNOCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2 );
*/
		iReturn = IDNO;

		if( iReturn == IDCANCEL ){
			return 1;
		}
		else if( iReturn == IDYES ){
			for( i = 0; i < uiFileCount; i++ ){
				DragQueryFile( hDrop, i, szFullPathShort, sizeof(szFullPathShort) );
				DragAndUrlFileList(szFullPathShort);
			}
		}
		else if( iReturn == IDNO ){
			for( i = 0; i < uiFileCount; i++ ){
				DragQueryFile( hDrop, i, szFullPathShort, sizeof(szFullPathShort) );
				DragAndFileList(szFullPathShort);
			}
		}
		else{

		}
	}
	else{
		sprintf( szMessage,
				 "ドラッグするのは %d個までにして下さい。",
				 DRAGANDDROP_FILE_COUNT_MAX );
		MessageBox( hWnd,
					szMessage,
					GetPszApplicationName(),
					MB_OK );
		DragFinish(hDrop);
		return 1;
	}

	DragAndUrlIndexHtml( szFullPathShort );

	DragFinish(hDrop);
	return 0;
#undef DRAGANDDROP_FILE_COUNT_MAX
#undef MESSAGEBOX_MESSAGE_MAX
}

int DragAndFileList( const char *pszFullPathShort )
{
	int			iReturn											 = 0;
	FILE		*fp												 = NULL;
	char		szFileName[((_MAX_PATH)+1)]						 = { 0, };
	char		szDesktopFullPath[((_MAX_PATH)+1)]				 = { 0, };
	char		szFullPathLong[((_MAX_PATH)+1)]					 = { 0, };
	char		szBroadFilename[((_MAX_PATH)+1)]				 = { 0, };

	FILE		*fpOutputFile2									 = NULL;
	char		szOutputFile2[((_MAX_PATH)+1)]					 = { 0, };

	FILE		*fpOutputFile3									 = NULL;
	char		szOutputFile3[((_MAX_PATH)+1)]					 = { 0, };

printf( "[imagefileindexhtmlmaker.c]の[273行目]\n" );
	ShortFullPath2LongFullPath( pszFullPathShort, szFullPathLong );
	iReturn = FullPath2FileNameExtension( szFullPathLong, 
										  szBroadFilename );
		/*
		 * 出力ファイル名を作成する。
		 */
#if 0
	GetDesktopFullPath(szDesktopFullPath);
#endif
	GetDesktopFullPath( szDesktopFullPath, ((_MAX_PATH)+1) );
	sprintf( szFileName, "%s\\FileList.txt", szDesktopFullPath );

printf( "%s\n", szFileName );

	fp = fopen( szFileName, "a" );
	if( fp == NULL ){
printf( "[imagefileindexhtmlmaker.c]の[284行目]\n" );
		return 1;
	}
//
// <img src="http://" alt="" width="" height="" border="0">
//
#if 0
printf( "%s\n", szFullPathLong );
printf( "%s\n", szBroadFilename );
#endif

	fprintf( fp, "<img src=\"./%s\" alt=\"\" width=\"\" height=\"\" border=\"0\">\n", szBroadFilename );
#if 0
	sprintf( szFullPathLong, "<img src=\"./%s\" alt=\"\" width=\"\" height=\"\" border=\"0\">\n", szBroadFilename );
printf( "%s\n", szFullPathLong );
#endif


	fclose(fp);

//
// <img src="http://" alt="" width="" height="" border="0">
// <img src="./ClipBoardBitmap0022.jpg" alt="" width="" height="" border="0">
//

	sprintf( szOutputFile2, "%s\\FileList_outer_frame.txt", szDesktopFullPath );

	fpOutputFile2 = fopen( szOutputFile2, "a" );
	if( fpOutputFile2 == NULL ){
		return 1;
	}

	fprintf( fpOutputFile2, "<img src=\"./%s\" alt=\"\" width=\"\" height=\"\" border=\"0\">\n", szBroadFilename );

	fclose( fpOutputFile2 );




//
// <figure>
//   <img src="./ClipBoardBitmap0022.jpg" alt="画像の説明" width="" height="" border="0">
//   <figcaption>画像の説明を書きます。必要に応じて適宜変更してください。</figcaption>
// </figure>
//
	sprintf( szOutputFile3, "%s\\FileList_caption_frame.txt", szDesktopFullPath );

	fpOutputFile3 = fopen( szOutputFile3, "a" );
	if( fpOutputFile3 == NULL ){
		return 1;
	}



	fprintf( fpOutputFile3, "<figure>\n" );
	fprintf( fpOutputFile3, "  <img src=\"./%s\" alt=\"画像の説明\" width=\"\" height=\"\" border=\"0\">\n", szBroadFilename );
	fprintf( fpOutputFile3, "  <figcaption>画像の説明を書きます。必要に応じて適宜変更してください。</figcaption>\n" );
	fprintf( fpOutputFile3, "</figure>\n" );

	fclose( fpOutputFile3 );





	return 0;
}

int DragAndUrlFileList( const char *pszFullPathShort )
{

	FILE	*fp											 = NULL;
	char	szFileName[((_MAX_PATH)+1)]					 = { 0, };
	char	szDesktopFullPath[((_MAX_PATH)+1)]			 = { 0, };
	char	szFullPathLong[((_MAX_PATH)+1)]				 = { 0, };

		/*
		 * 出力ファイル名を作成する。
		 */
#if 0
	GetDesktopFullPath(szDesktopFullPath);
#endif
	GetDesktopFullPath( szDesktopFullPath, ((_MAX_PATH)+1) );
	sprintf( szFileName, "%s\\FileList.txt", szDesktopFullPath );

	ShortFullPath2LongFullPath( pszFullPathShort, szFullPathLong );

	fp = fopen( szFileName, "a" );
	if( fp == NULL ){
		return 1;
	}

	fprintf( fp, "file://%s\n", szFullPathLong );

	fclose(fp);

	return 0;
}

int DragAndUrlIndexHtml( const char *pszFullPathShort )
{

	FILE		*fpInputFile								 = NULL;
	FILE		*fpOutputFile								 = NULL;
	char		szInputFile[((_MAX_PATH)+1)]				 = { 0, };
	char		szOutputFile[((_MAX_PATH)+1)]				 = { 0, };
	char		szDesktopFullPath[((_MAX_PATH)+1)]			 = { 0, };
	char		szFullPathLong[((_MAX_PATH)+1)]				 = { 0, };

	FILE		*fpOutputFile2								 = NULL;
	char		szOutputFile2[((_MAX_PATH)+1)]				 = { 0, };

	FILE		*fpOutputFile3								 = NULL;
	char		szOutputFile3[((_MAX_PATH)+1)]				 = { 0, };

	int			iReturn										 = 0;
	int			iTextFileSize								 = 0;

	char		*pszFileListContents						 = NULL;
	size_t		uiReadSize									 = 0;

		/*
		 * 出力ファイル名を作成する。
		 */
#if 0
	GetDesktopFullPath(szDesktopFullPath);
#endif
	GetDesktopFullPath( szDesktopFullPath, ((_MAX_PATH)+1) );
	sprintf( szInputFile, "%s\\FileList.txt", szDesktopFullPath );
	sprintf( szOutputFile, "%s\\index.html.txt", szDesktopFullPath );
	sprintf( szOutputFile2, "%s\\index_outer_frame.html", szDesktopFullPath );
	sprintf( szOutputFile3, "%s\\index_caption_frame.html", szDesktopFullPath );

//	ShortFullPath2LongFullPath( pszFullPathShort, szFullPathLong );



	iReturn = GetTextFileSize( szInputFile );
	if( iReturn == 0 ){
		return -1;
	}
	else if( iReturn < 0 ){
		return -1;
	}
	else if( 0 < iReturn ){
		iTextFileSize = iReturn;
	}
	else{
		// ここに来ることはないはずである。
		return -1;
	}

	pszFileListContents = CreateCharArray1D( (iTextFileSize+1) );
	if( pszFileListContents == NULL ){
		return -1;
	}

	fpInputFile = fopen( szInputFile, "rb" );
	if( fpInputFile == NULL ){
		return 1;
	}

		/*
		 * 一発で読み込む
		 */
	uiReadSize = fread( pszFileListContents, 1, iTextFileSize, fpInputFile );
		/*
		 * 念のため終端を付ける
		 */
	pszFileListContents[uiReadSize] = '\0';
	fclose( fpInputFile );

	fpOutputFile = fopen( szOutputFile, "a" );
	if( fpOutputFile == NULL ){
		return 1;
	}

	fprintf( fpOutputFile, "<!doctype html>\n" );
	fprintf( fpOutputFile, "<html lang=\"\">\n" );
	fprintf( fpOutputFile, "<head>\n" );
	fprintf( fpOutputFile, "  <meta charset=\"utf-8\">\n" );
	fprintf( fpOutputFile, "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n" );
	fprintf( fpOutputFile, "  <title></title>\n" );
	fprintf( fpOutputFile, "  <link rel=\"stylesheet\" href=\"style_index.css\">\n" );
	fprintf( fpOutputFile, "  <meta name=\"description\" content=\"\">\n" );
	fprintf( fpOutputFile, "  <meta property=\"og:title\" content=\"\">\n" );
	fprintf( fpOutputFile, "  <meta property=\"og:type\" content=\"\">\n" );
	fprintf( fpOutputFile, "  <meta property=\"og:url\" content=\"\">\n" );
	fprintf( fpOutputFile, "  <meta property=\"og:image\" content=\"\">\n" );
	fprintf( fpOutputFile, "  <meta property=\"og:image:alt\" content=\"\">\n" );
	fprintf( fpOutputFile, "  <link rel=\"icon\" href=\"/favicon.ico\" sizes=\"any\">\n" );
	fprintf( fpOutputFile, "  <link rel=\"icon\" href=\"/icon.svg\" type=\"image/svg+xml\">\n" );
	fprintf( fpOutputFile, "  <link rel=\"apple-touch-icon\" href=\"icon.png\">\n" );
	fprintf( fpOutputFile, "  <link rel=\"manifest\" href=\"site.webmanifest\">\n" );
	fprintf( fpOutputFile, "  <meta name=\"theme-color\" content=\"#fafafa\">\n" );
	fprintf( fpOutputFile, "</head>\n" );
	fprintf( fpOutputFile, "<body>\n" );
	fprintf( fpOutputFile, "\n" );
	fprintf( fpOutputFile, "<h1>Hello, world!!</h1>\n" );
	fprintf( fpOutputFile, "<h2>Hello, world!!</h2>\n" );
	fprintf( fpOutputFile, "<h3>Hello, world!!</h3>\n" );
	fprintf( fpOutputFile, "<h4>Hello, world!!</h4>\n" );
	fprintf( fpOutputFile, "<p>\n" );
	fprintf( fpOutputFile, "Hello, world!!<br>\n" );
	fprintf( fpOutputFile, "Hello, world!!\n" );
	fprintf( fpOutputFile, "</p>\n" );
	fprintf( fpOutputFile, "<p>\n" );
	fprintf( fpOutputFile, "Hello, world!!\n" );
	fprintf( fpOutputFile, "</p>\n" );
	fprintf( fpOutputFile, "\n" );
	fprintf( fpOutputFile, "<!-- ここから -->\n" );
	fprintf( fpOutputFile, "%s\n", pszFileListContents );
	fprintf( fpOutputFile, "<!-- ここまで -->\n" );
	fprintf( fpOutputFile, "\n" );
	fprintf( fpOutputFile, "\n" );
	fprintf( fpOutputFile, "</body>\n" );
	fprintf( fpOutputFile, "</html>\n" );
	fclose( fpOutputFile );

	fpOutputFile2 = fopen( szOutputFile2, "a" );
	if( fpOutputFile2 == NULL ){
		return 1;
	}

	fprintf( fpOutputFile2, "<!doctype html>\n" );
	fprintf( fpOutputFile2, "<html lang=\"\">\n" );
	fprintf( fpOutputFile2, "<head>\n" );
	fprintf( fpOutputFile2, "  <meta charset=\"shift_jis\">\n" );
//	fprintf( fpOutputFile2, "  <meta charset=\"utf-8\">\n" );
	fprintf( fpOutputFile2, "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n" );
	fprintf( fpOutputFile2, "  <title>outer frame</title>\n" );
	fprintf( fpOutputFile2, "  <link rel=\"stylesheet\" href=\"style_index.css\">\n" );
	fprintf( fpOutputFile2, "  <style>\n" );
	fprintf( fpOutputFile2, "    /* 画像を見分けやすくする外枠（このページ専用） */\n" );
	fprintf( fpOutputFile2, "    img {\n" );
	fprintf( fpOutputFile2, "      border: 2px solid #444;\n" );
	fprintf( fpOutputFile2, "      padding: 4px;\n" );
	fprintf( fpOutputFile2, "      background-color: #fff;\n" );
	fprintf( fpOutputFile2, "      box-shadow: 2px 2px 6px rgba(0,0,0,0.2);\n" );
	fprintf( fpOutputFile2, "      margin: 1em 0;\n" );
	fprintf( fpOutputFile2, "      max-width: 100%%;\n" );
	fprintf( fpOutputFile2, "      height: auto;\n" );
	fprintf( fpOutputFile2, "    }\n" );
	fprintf( fpOutputFile2, "  </style>\n" );
	fprintf( fpOutputFile2, "</head>\n" );
	fprintf( fpOutputFile2, "<body>\n" );
	fprintf( fpOutputFile2, "\n" );
	fprintf( fpOutputFile2, "<h1>Hello, world!!</h1>\n" );
	fprintf( fpOutputFile2, "<h2>Hello, world!!</h2>\n" );
	fprintf( fpOutputFile2, "<h3>Hello, world!!</h3>\n" );
	fprintf( fpOutputFile2, "<h4>Hello, world!!</h4>\n" );
	fprintf( fpOutputFile2, "\n" );
	fprintf( fpOutputFile2, "<p>このページは、画像に外枠のみを付けた例です。</p>\n" );
	fprintf( fpOutputFile2, "\n" );
	fprintf( fpOutputFile2, "<!-- ここから -->\n" );
	fprintf( fpOutputFile2, "%s\n", pszFileListContents );
	fprintf( fpOutputFile2, "<!-- ここまで -->\n" );
	fprintf( fpOutputFile2, "\n" );
	fprintf( fpOutputFile2, "</body>\n" );
	fprintf( fpOutputFile2, "</html>\n" );
	fclose( fpOutputFile2 );

	fpOutputFile3 = fopen( szOutputFile3, "a" );
	if( fpOutputFile3 == NULL ){
		return 1;
	}

	fprintf( fpOutputFile3, "<!doctype html>\n" );
	fprintf( fpOutputFile3, "<html lang=\"\">\n" );
	fprintf( fpOutputFile3, "<head>\n" );
	fprintf( fpOutputFile3, "  <meta charset=\"shift_jis\">\n" );
//	fprintf( fpOutputFile3, "  <meta charset=\"utf-8\">\n" );
	fprintf( fpOutputFile3, "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n" );
	fprintf( fpOutputFile3, "  <title>caption + frame</title>\n" );
	fprintf( fpOutputFile3, "  <link rel=\"stylesheet\" href=\"style_index.css\">\n" );
	fprintf( fpOutputFile3, "  <style>\n" );
	fprintf( fpOutputFile3, "    /* 画像＋キャプション用の最小スタイル（このページ専用） */\n" );
	fprintf( fpOutputFile3, "    figure {\n" );
	fprintf( fpOutputFile3, "      display: inline-block;\n" );
	fprintf( fpOutputFile3, "      text-align: center;\n" );
	fprintf( fpOutputFile3, "      margin: 1.5em 0;\n" );
	fprintf( fpOutputFile3, "    }\n" );
	fprintf( fpOutputFile3, "    figure img {\n" );
	fprintf( fpOutputFile3, "      border: 2px solid #444;\n" );
	fprintf( fpOutputFile3, "      padding: 4px;\n" );
	fprintf( fpOutputFile3, "      background-color: #fff;\n" );
	fprintf( fpOutputFile3, "      box-shadow: 2px 2px 6px rgba(0,0,0,0.2);\n" );
	fprintf( fpOutputFile3, "      max-width: 100%%;\n" );
	fprintf( fpOutputFile3, "      height: auto;\n" );
	fprintf( fpOutputFile3, "    }\n" );
	fprintf( fpOutputFile3, "    figcaption {\n" );
	fprintf( fpOutputFile3, "      font-size: 0.9rem;\n" );
	fprintf( fpOutputFile3, "      color: #555;\n" );
	fprintf( fpOutputFile3, "      margin-top: 0.5em;\n" );
	fprintf( fpOutputFile3, "    }\n" );
	fprintf( fpOutputFile3, "  </style>\n" );
	fprintf( fpOutputFile3, "</head>\n" );
	fprintf( fpOutputFile3, "<body>\n" );
	fprintf( fpOutputFile3, "\n" );
	fprintf( fpOutputFile3, "<h1>Hello, world!!</h1>\n" );
	fprintf( fpOutputFile3, "<h2>Hello, world!!</h2>\n" );
	fprintf( fpOutputFile3, "<h3>Hello, world!!</h3>\n" );
	fprintf( fpOutputFile3, "<h4>Hello, world!!</h4>\n" );
	fprintf( fpOutputFile3, "\n" );
	fprintf( fpOutputFile3, "<p>このページは、画像に外枠とキャプションを付けた例です。</p>\n" );
	fprintf( fpOutputFile3, "\n" );
	fprintf( fpOutputFile3, "<figure>\n" );
	fprintf( fpOutputFile3, "<!-- ここから -->\n" );
	fprintf( fpOutputFile3, "%s\n", pszFileListContents );
	fprintf( fpOutputFile3, "<!-- ここまで -->\n" );
	fprintf( fpOutputFile3, "  <figcaption>画像の説明を書きます。必要に応じて適宜変更してください。</figcaption>\n" );
	fprintf( fpOutputFile3, "</figure>\n" );
	fprintf( fpOutputFile3, "\n" );
	fprintf( fpOutputFile3, "</body>\n" );
	fprintf( fpOutputFile3, "</html>\n" );
	fclose( fpOutputFile3 );

	DeleteCharArray1D( pszFileListContents );

	return 0;
}
