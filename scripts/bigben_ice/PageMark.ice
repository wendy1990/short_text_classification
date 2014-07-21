#pragma once
module BigBen
{
    dictionary<string, string> KeyValue;
    // doFilter DESC:
    // id: page id
    // url:
    // type: (news,blog,bbs,weibo,tqq)
    // author:
    // date: 页面发布时间
    // content: 原始内容
    // forward: 转发数（仅在type=weibo | tqq 时有效)
    // comment: 评论数
    // org_author:
    // org_date:
    // org_content:
    // org_forward:
    // org_comment:
    // keywords：提取出的关键词
    // locations: 提取出的地点
    // customKey:
    // predict_type:是被何种⽅方法预测得出负⾯面的
    // score: 分值
    interface PageMarkServerInterface
    {
        void doMark(KeyValue page);
    };
};
