#include "predict.h"
#include "short_TFIDF_predict.h" 
#include <iostream>
#include <fstream>
#include "boost/algorithm/string.hpp"
#include <math.h>

using namespace std;
using namespace bigben;

int main()
{
// log_wrapper lw;
// lw.initialize("STLOG");
// LOG_INFO("1231232132131");
 string conf = "../conf/short_tfidf_predict.conf";
 ifstream input_f("../../../data/temp/post.test.segmented_0.txt");
 ofstream output_f("output.test");
 string line;
 string str;
 model_info info;
 predict_ret ret;
 short_tfidf_predict pre;
 cout << "初始化..."<<endl;
 pre.init(conf,info);
 cout <<"预测..."<<endl;
 while(getline(input_f, line))
 {
      vector<std::string> segs;
      boost::algorithm::split(segs,line,boost::algorithm::is_any_of("\t"));
      str = segs[1].c_str();
      pre.do_predict(str,ret);
    //  cout <<"分类类型:"<<ret.type<<" "<<"得分:"<<ret.score<<endl;
      int label;
      label = (ret.type ==  negative) ? -1 : 1;       
      output_f << label <<"\t"<< ret.score <<endl;
 }
 return 0;
}

