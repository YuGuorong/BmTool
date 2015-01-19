#if !defined(_SERVERMODAL_H_)
#define _SERVERMODAL_H_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#pragma comment(lib, ".\\lib\\ServerModal.lib")

#ifdef DLLEXPORT
#	define ServerModal_API extern "C" __declspec(dllexport)
#else
#	define ServerModal_API extern "C" __declspec(dllimport)
#endif
//////////////////////////////////////////////////////////////////////
//	功能				:	PDF文件加密，若已加密文件直接复制		//
//																	//
//	szSrcFile			:	待加密的源文件							//
//	szDstFile			:	加密后的目标文件						//
//  bFailIfExists		:   设置为1，表示若目标文件已存在，返回-2	//
//							设置为0，表示若目标文件已存在，直接覆盖	//
//																	//
//  返回值				:   -3 表示目标文件创建失败					//
//						    -2 表示目标文件已存在					//
//							-1 表示目标文件加密失败					//
//						     0 表示文件大小为0						//
//						     1 表示文件加密成功						//
//////////////////////////////////////////////////////////////////////
ServerModal_API int _stdcall Encrypt(LPCTSTR szSrcFile, LPCTSTR szDstFile, 
									long bFailIfExists, long lParam);

//////////////////////////////////////////////////////////////////////
//	功能				:	PDF文件解密，若未加密文件直接复制		//
//																	//
//	szSrcFile			:	待解密的源文件							//
//	szDstFile			:	解密后的目标文件						//
//  bFailIfExists		:   设置为1，表示若目标文件已存在，返回-2	//
//							设置为0，表示若目标文件已存在，直接覆盖	//
//																	//
//  返回值				:   -3 表示目标文件创建失败					//
//						    -2 表示目标文件已存在					//
//							-1 表示目标文件解密失败					//
//						     0 表示文件大小为0						//
//						     1 表示文件解密成功						//
//////////////////////////////////////////////////////////////////////
ServerModal_API int _stdcall Decrypt(LPCTSTR szSrcFile, LPCTSTR szDstFile, 
									long bFailIfExists, long lParam);


#endif // !defined(_SERVERMODAL_H_)
