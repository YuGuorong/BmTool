
目标生成字段

!&bookprice = price

!&contributor
	VCARD 取FN:

!&preview_swf

!&grade_caption
	subject_volume  !&subject + !&volumne
	
!&term     = 0 全学期

!&grade_level 01(学前)-10(9年级)
!&grades
	<item id="" value=!&grade_level" caption=!&grade_caption term=!&term/>   
	...多个
!&dirs

!&operator
	<uploader id="上传者ID" caption="上传者姓名" time="上传时间"/>

!&files
<!-- 该节记录资源包中的各个文件，根据业务要求，可存在多个，并且可扩充 -->
<item type="cover" id="" caption="" size="" hashType="md5" hashValue="经过hashType指定的算法，HASH后的值" file=!&coverage  preview=!&preview_swf   />
<item type="类型，除系统预定义cover之外，可以根据业务需要，增加预定义类型" id="" caption="" size="" 
	  hashType="sha1" hashValue="经过hashType指定的算法，HASH后的值" file="" preview=""/>
<item type="" id="" caption="" size=""  hashType="sha1" hashValue="经过hashType指定的算法，HASH后的值" file="" preview=""/>

源字段
coverage：
	1.没有添加随机
	2.写到files里

learning_resource_type， 上档同为!&learning_resource_type
	0501->05
	
audience  上档同为 audience
	01;02->枚举：0：全部　1：学生　2：教师"

adoubed?
!&release ??出版商 -> ?版权提供者

applicability :高中
audience :01;02
berm :
book_catalog :
catalog :T_ElectronicMaterial
contribute :
contribute_unit :
contributor :BEGIN:VCARD
			VERSION:3.0
			CATEGORIES：vCARD,BENIC,CHINA
			CLASS:PUBLIC
			FN:冉敏
			END:VCARD
copyright :
course_order :
course_type :
courseclass :
coverage :
curriculum_name :
curriuclum_standard :
date :2015-01-27
description :又称电离平衡。指在弱电解质溶液中，未离解的分子和其离子间建立的动态平衡关系。其平衡常数用K来表示，称电离常数或离解常数。
displines :
duration :
educational :
entry :4f6764e6-0e0c-4838-ab6f-5f630ca24395
format :
general :
grade_group :高中
grade_level :11
identifier :
isbn :
knowledge_point :
language :zh
learning_resource_type :0503
lifecycle :
location :
meta-meta_data :
metadata_scheme :BERMS北京市教育委员会增补版
metadataclass :5
pages :
press :
price :
release :
remark :
rights :
role :01
size :
subject :化学选修四化学反应原理
subject_keyword :化学;离子平衡
teacher :
technical :
textbook_catelog :
textbook_code :
textbook_release :人教版
title :水溶液中的例子平衡
type :01
version :
volume :



+	CAsyncHttp	this	0x0044d2d0 {m_strLocalFile=L"C:\\Users\\guoryu\\Documents\\奥鹏书籍\\PROJ\\测试大数据2.zip.zip" }	CAsyncHttp *
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
bi.lpszTitle   =   "请选择目录";
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
