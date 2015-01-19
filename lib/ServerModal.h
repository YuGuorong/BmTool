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
//	����				:	PDF�ļ����ܣ����Ѽ����ļ�ֱ�Ӹ���		//
//																	//
//	szSrcFile			:	�����ܵ�Դ�ļ�							//
//	szDstFile			:	���ܺ��Ŀ���ļ�						//
//  bFailIfExists		:   ����Ϊ1����ʾ��Ŀ���ļ��Ѵ��ڣ�����-2	//
//							����Ϊ0����ʾ��Ŀ���ļ��Ѵ��ڣ�ֱ�Ӹ���	//
//																	//
//  ����ֵ				:   -3 ��ʾĿ���ļ�����ʧ��					//
//						    -2 ��ʾĿ���ļ��Ѵ���					//
//							-1 ��ʾĿ���ļ�����ʧ��					//
//						     0 ��ʾ�ļ���СΪ0						//
//						     1 ��ʾ�ļ����ܳɹ�						//
//////////////////////////////////////////////////////////////////////
ServerModal_API int _stdcall Encrypt(LPCTSTR szSrcFile, LPCTSTR szDstFile, 
									long bFailIfExists, long lParam);

//////////////////////////////////////////////////////////////////////
//	����				:	PDF�ļ����ܣ���δ�����ļ�ֱ�Ӹ���		//
//																	//
//	szSrcFile			:	�����ܵ�Դ�ļ�							//
//	szDstFile			:	���ܺ��Ŀ���ļ�						//
//  bFailIfExists		:   ����Ϊ1����ʾ��Ŀ���ļ��Ѵ��ڣ�����-2	//
//							����Ϊ0����ʾ��Ŀ���ļ��Ѵ��ڣ�ֱ�Ӹ���	//
//																	//
//  ����ֵ				:   -3 ��ʾĿ���ļ�����ʧ��					//
//						    -2 ��ʾĿ���ļ��Ѵ���					//
//							-1 ��ʾĿ���ļ�����ʧ��					//
//						     0 ��ʾ�ļ���СΪ0						//
//						     1 ��ʾ�ļ����ܳɹ�						//
//////////////////////////////////////////////////////////////////////
ServerModal_API int _stdcall Decrypt(LPCTSTR szSrcFile, LPCTSTR szDstFile, 
									long bFailIfExists, long lParam);


#endif // !defined(_SERVERMODAL_H_)
