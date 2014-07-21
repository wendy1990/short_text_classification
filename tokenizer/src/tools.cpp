/**
 * @brief  公共函数和异常处理类的实现
 * @author eric.zhang@fmkj.net
 * @date   2013/10/28 
 */
#include "tools.h"
#include <cstring>
#include <string>

namespace nlp_tokenizer_wrapper
{
std::string itos(int i)
{
    std::stringstream s;
    s<<i;
    return s.str();
}

std::string get_uniform_postag(int type)
{
	std::string up;
	switch(type)
	{
        case 0:
                up="un";
                break;
	case 1:
		up="a";
		break;
	case 2:
		up="ad";
		break;
	case 3:
		up="an";
		break;
	case 4:
		up="ag";
		break;
	case 5:
		up="al";
		break;
	case 20:
		up="b";
		break;
	case 21:
		up="bg";
		break;
	case 22:
		up="bl";
		break;
	case 40:
		up="c";
		break;
	case 41:
		up="cc";
		break;
	case 60:
		up="d";
		break;
	case 61:
		up="dg";
		break;
	case 80:
		up="e";
		break;	  
	case 100:
		up="f";
		break;
        case 120:
                up="g";
                break;
	case 140:
		up="h";
		break;
	case 160:
		up="i";
		break;
	case 180:
		up="j";
		break;
	case 200:
		up="k";
		break;
	case 220:
		up="l";
		break;
	case 240:
		up="m";
		break;
	case 241:
		up="mq";
		break;
	case 242:
		up="mg";
	        break;
	case 260:
		up="n";
		break;
	case 261:
		up="nr";
		break;
	case 262:
		up="ns";
		break;
	case 263:
		up="nt";
		break;
	case 264:
		up="nz";
		break;
	case 265:
		up="nl";
		break;
	case 266:
		up="ng";
		break;
	case 267:
		up="n_new";
		break;
	case 280:
		up="o";
		break;
	case 300:
		up="p";
		break;
	case 301:
		up="pba";
		break;
	case 302:
		up="pbei";
		break;
	case 320:
		up="q";
		break;
	case 321:
		up="qv";
		break;
	case 322:
		up="qt";
		break;
	case 340:
		up="r";
		break;
	case 341:
		up="rr";
		break; 
	case 342:
		up="rz";
		break;
	case 343:
		up="ry";
		break;
	case 344:
		up="rg";
		break;
	case 360:
		up="s";
		break;
	case 380:
		up="t";
		break;
	case 381:
		up="tg";
		break;
	case 400:
		up="u";
		break;
	case 401:
		up="uzhe";
		break;  
	case 402:
		up="ule";
		break;
	case 403:
		up="uguo";
		break;
	case 404:
		up="ude1";
		break;
	case 405:
		up="ude2";
		break; 
	case 406:
		up="ude3";
		break;
	case 407:
		up="usuo";
		break;
	case 408:
		up="udeng";
		break;
	case 409:
		up="uyy";
		break;
	case 410:
		up="udh";
		break;
	case 411:
		up="uls";
		break;
	case 412:
	        up="ujl";
		break;
	case 413:
		up="uzhi";
		break;
	case 414:
		up="ulian";
		break;
	case 415:
		up="uqj";
		break;
	case 420:
		up="v";
		break;
	case 421:
		up="vd";
		break;
	case 422:
		up="vn";
		break;
	case 423:
		up="vshi";
		break;
	case 424:
		up="vyou";
		break;
	case 425:
		up="vf";
		break;
	case 426:
	        up="vx";
		break;
	case 427:
		up="vi";
		break;
	case 428:
		up="vl";
		break;
	case 429:
		up="vg";
		break;
	case 440:
		up="w";
		break;
	case 441:
		up="wkz";
		break;
	case 442:
		up="wky";
		break;
	case 443:
		up="wyb";
		break;
	case 444:
		up="wyz";
		break;
	case 445:
		up="wyy";
		break;
	case 446:
		up="wj";
		break;
	case 447:
		up="ww";
		break;
	case 448:
		up="wt";
		break;
	case 449:
		up="wd";
		break;
	case 450:
		up="wf";
		break;
	case 451:
		up="wn";
		break;
	case 452:
		up="wm";
		break;
	case 453:
		up="ws";
		break;
	case 454:
		up="wp";
		break;
	case 455:
		up="wb";
		break;
	case 456:
		up="wh";
		break;
	case 460:
		up="x";
		break;
	case 461:
		up="xx";
		break;
	case 462:
		up="xu";
		break;
	case 480:
		up="y";
		break;
	case 500:
		up="z";
		break;
        default:
		up="un";
	}
	return up;
}

std::string get_top_uniform_postag(std::string type)
{
	std::string s=type.substr(0,1);
	return s;
}

bad_creation::bad_creation(const std::string& type):logic_error("Can't create tye: "+type){}
unopen_file::unopen_file(const std::string& type):logic_error("Can't open the file: "+type){}
nomethod::nomethod(const std::string& object,const std::string& type):logic_error("This object "+object+"doesn't have "+type+"method"){}

}//end namespace nlp_tokenizer
