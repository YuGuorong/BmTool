----------------------------------------------------------------------------------------------
��ȡ����רҵ����
����
Get  /api/subjects  http/1.1

���ӣ�
Get  /api/subjects http/1.1
Host:192.168.1.61:8080
	
{"token":"11111","sessionId":"8a7852df9045e05df86cec3b131cbad5bbcdcb2c "}

��Ӧ
{"state": 401}
state��401sessionIdΪ��

{"state": 404}
state����������subjectΪ��

{"state": 200, "type": "subject", "data": "subjects"}
state��200�����������ͣ�subject�����������ݣ�data��
----------------------------------------------------------------------------------------------


Ŀ�������ֶ�

!&bookprice = price

!&contributor
	VCARD ȡFN:

!&preview_swf

!&grade_caption
	subject_volume  !&subject + !&volumne
	
!&term     = 0 ȫѧ��

!&grade_level 01(ѧǰ)-10(9�꼶)
!&grades
	<item id="" value=!&grade_level" caption=!&grade_caption term=!&term/>   
	...���
!&dirs

!&operator
	<uploader id="�ϴ���ID" caption="�ϴ�������" time="�ϴ�ʱ��"/>

!&files
<!-- �ýڼ�¼��Դ���еĸ����ļ�������ҵ��Ҫ�󣬿ɴ��ڶ�������ҿ����� -->
<item type="cover" id="" caption="" size="" hashType="md5" hashValue="����hashTypeָ�����㷨��HASH���ֵ" file=!&coverage  preview=!&preview_swf   />
<item type="���ͣ���ϵͳԤ����cover֮�⣬���Ը���ҵ����Ҫ������Ԥ��������" id="" caption="" size="" 
	  hashType="sha1" hashValue="����hashTypeָ�����㷨��HASH���ֵ" file="" preview=""/>
<item type="" id="" caption="" size=""  hashType="sha1" hashValue="����hashTypeָ�����㷨��HASH���ֵ" file="" preview=""/>

Դ�ֶ�
coverage��
	1.û��������
	2.д��files��

learning_resource_type�� �ϵ�ͬΪ!&learning_resource_type
	0501->05
	
audience  �ϵ�ͬΪ audience
	01;02->ö�٣�0��ȫ����1��ѧ����2����ʦ"

adoubed?
!&release ??������ -> ?��Ȩ�ṩ��

applicability :����
audience :01;02
berm :
book_catalog :
catalog :T_ElectronicMaterial
contribute :
contribute_unit :
contributor :BEGIN:VCARD
			VERSION:3.0
			CATEGORIES��vCARD,BENIC,CHINA
			CLASS:PUBLIC
			FN:Ƚ��
			END:VCARD
copyright :
course_order :
course_type :
courseclass :
coverage :
curriculum_name :
curriuclum_standard :
date :2015-01-27
description :�ֳƵ���ƽ�⡣ָ�����������Һ�У�δ���ķ��Ӻ������Ӽ佨���Ķ�̬ƽ���ϵ����ƽ�ⳣ����K����ʾ���Ƶ��볣������ⳣ����
displines :
duration :
educational :
entry :4f6764e6-0e0c-4838-ab6f-5f630ca24395
format :
general :
grade_group :����
grade_level :11
identifier :
isbn :
knowledge_point :
language :zh
learning_resource_type :0503
lifecycle :
location :
meta-meta_data :
metadata_scheme :BERMS�����н���ίԱ��������
metadataclass :5
pages :
press :
price :
release :
remark :
rights :
role :01
size :
subject :��ѧѡ���Ļ�ѧ��Ӧԭ��
subject_keyword :��ѧ;����ƽ��
teacher :
technical :
textbook_catelog :
textbook_code :
textbook_release :�˽̰�
title :ˮ��Һ�е�����ƽ��
type :01
version :
volume :



+	CAsyncHttp	this	0x0044d2d0 {m_strLocalFile=L"C:\\Users\\guoryu\\Documents\\�����鼮\\PROJ\\���Դ�����2.zip.zip" }	CAsyncHttp *
+	CDealSocket	this	0x0044e6b0 {m_hSocket=576 m_RecvTotLen=-842150451 m_CurRecvBytes=-842150451 ...}	CDealSocket *




POST /api/users/admin/sessions HTTP/1.1
Accept: */*
Accept-Language: zh-cn
Accept-Encoding: gzip, deflate
Host: openres.xicp.net:8091
Content-Type: application/json
Content-Length: 20
User-Agent: CEHTTP
Cache-Control: no-cache

{"password":"admin"}
HTTP/1.1 200 OK
Server: Apache-Coyote/1.1
Buffer: false
Pragma: no-cache
Expires: Thu, 01 Jan 1970 00:00:00 GMT
Cache-Control: no-cache, no-store, max-age=0
Set-Cookie: sessionId=19136f5738b79956187c3b280ab50ad4b021159e; Path=/
Content-Type: application/json;charset=utf-8
Transfer-Encoding: chunked
Date: Tue, 09 Dec 2014 16:36:11 GMT

63
{"state": 200, "sessionId": "19136f5738b79956187c3b280ab50ad4b021159e", "id" : "33542438663913472"}
0


-		this	0x00988de0 {hWnd=0x008202d6 {unused=4128831 }}	CLoginDlg *

+		this	0x0098c3c8 {m_szHttpType="" m_szRespHeader="" m_vstrHeaders={ size=0 } ...}	CAsyncHttp *
g_pHttpResMan = 0x00971478 {hWnd=0x001a0e56 {unused=3145782 }}


/*
char   szDir[MAX_PATH];
BROWSEINFO   bi;
ITEMIDLIST   *pidl;

bi.hwndOwner   =   this->m_hWnd;
bi.pidlRoot   =   NULL;
bi.pszDisplayName   =   szDir;
bi.lpszTitle   =   "��ѡ��Ŀ¼";
bi.ulFlags   =   BIF_RETURNONLYFSDIRS;
bi.lpfn   =   NULL;
bi.lParam   =   0;
bi.iImage   =   0;

pidl   =   SHBrowseForFolder(&bi);
if(pidl   ==   NULL)
return;
if(!SHGetPathFromIDList(pidl,   szDir))
return;
*/
