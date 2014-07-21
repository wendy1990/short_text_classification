#pragma once


module BigBen
{
    dictionary<string, string> KeyValue;


	// DEST:
	//          id:  page id
	//          url:  
	//          type:  (news,blog,bbs,weibo,tqq)
	//          author:
	//          date: 页面发布时间
	//          title:
	//          content: 原始内容
	//          forward: 转发数（仅在type=weibo | tqq 时有效)
	//          comment: 评论数
	//          org_author:
	//          org_date:
	//          org_content:
	//          org_forward:
	//          org_comment:
	//          seg_title: 分词、降噪后的title，term之间以“ ”分隔
	//          seg_content: 分词、降噪、去停用词处理后的内容， term之间以“ ”分隔
	//          pos_content: 词性，与seg_content一一对应
	//          keywords：提取出的关键词
	//          locations: 提取出的地点
	//          customKey: 
	interface TextClassificationServerInterface
	{
	    void doPredict(KeyValue page);
	};


};
