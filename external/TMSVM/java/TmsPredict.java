package tms;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

//import org.apache.commons.logging.Log;
//import org.apache.commons.logging.LogFactory;
//import org.jsoup.helper.StringUtil;


public class TmsPredict  {
	
	private List<TmsModel> models; 
//	protected static final Log log = LogFactory.getLog(MsgClassifier.class);
	/**
	 * 构造函数，需要给出模型所在的路径以及配置文件的名称
	 * @param paths
	 * @param configNames
	 */
	public TmsPredict(String[] paths,String[] configNames){
		models =  new ArrayList<TmsModel>();
		for(int i =0;i<paths.length;i++){
			models.add(new TmsModel(paths[i],configNames[i]));
		}
	}
	/**
	 * 构造函数，需要给出模型所在的路径。配置文件名称默认为tms.config
	 * @param paths
	 * @param configNames
	 */
	
	public TmsPredict(String[] paths){
		models =  new ArrayList<TmsModel>();
		for(int i =0;i<paths.length;i++){
			models.add(new TmsModel(paths[i],"tms.config"));
		}
	}
	
	/**
	 * 计算分数的入口程序1，接收的List<String>类型的数据。
	 * 然后利用所有的模型进行预测分数
	 * @param text 读入的文本，List里面保存每个词。
	 * @return 返回SvmResult类型的结果。
	 */
	
	public SvmResult[] calScore(List<String> text){
		int k = models.size();
		SvmResult[] result = new SvmResult[k];
		for(int i=0;i<k;i++){
			TmsModel tmsModel = models.get(i);
			result[i] = this.calSigleTmsScore(text,tmsModel);
		}
		return result;

	}
	
//	/**
//	 * 计算分数的入口程序2，接收的String类型的数据。
//	 * 然后利用所有的模型进行预测分数
//	 * @param text 读入的文本
//	 * @return 返回SvmResult类型的结果。
//	 */
//	
//	public SvmResult[] calScore(String doc){
//		String text = this.preProcContent(doc);
//		List<String> words = this.participle(text);
//		return this.calScore(words);
//	}
	
	/**
	 * 计算分数的入口程序3，接收一个字符串，并给点分词的分隔符号。
	 * 然后利用所有的模型进行预测分数
	 * @param line 读入的文本
	 * @param str_splitTag 分词后文本的分割符号。
	 * @return 返回SvmResult类型的结果。
	 */
	
	public SvmResult[] calScore(String text,String str_splitTag){
		String[] line = text.split(str_splitTag);
		List<String > words = new ArrayList<String>();
		for(String s:line)
			words.add(s);
		return this.calScore(words);

	}
	
	/**
	 * 计算分数的入口程序4，给定输入，以及各种分割符号和指标项。
	 * 这个函数可以对所有的模型特别定制，因为不同的模型可能检测的字段不同
	 * 然后利用所有的模型进行预测分数
	 * @param line 读入的文本
	 * @param tc_splitTag 读入文本分为几个部分，各个部分之间的分割符号
	 * @param str_splitTag 分词后文本的分割符号。
	 * @param str_connentTag 如果要多个部分放在一起预测，需要先用连接符号连接在一起，实际上str_splitTag和str_connentTag是一个东西。但是在java中两者不能相同 ^ \\^
	 * @return
	 */
	public SvmResult[] calScore(String line,int[][] indexes,String tc_splitTag,String str_splitTag,String str_connentTag){
		String []input_text = line.split(tc_splitTag);
		int k = models.size();
		SvmResult[] result = new SvmResult[k];
		for(int i=0;i<k;i++){
			int[] index = indexes[i];
			TmsModel tmsModel = models.get(i);
			String text = mergeText(input_text, index,str_connentTag);
			List<String> words = new ArrayList<String>();
			for(String s:text.split(str_splitTag))
				words.add(s);
			result[i] = calSigleTmsScore(words,tmsModel);
		}
		return result;

	}
	
//	protected String preProcContent(String doc) {
//		//去除html标签
//		doc=FilterUtil.extractHtmlContextNotNull(doc);
//		//去除特殊字符
//		String specChars="≠ь≡эюя≤ш≥щ≦ъ≧ыфхцчрс≮т≯уё︰\"︱# ︳!&︴︵'︶$︷☉%︸☆*︹+︺≈(︻★)︼.︽/︾,≌︿-≒;:?>=<﹖﹗∥﹔﹕∧﹒∠@﹐﹑∣﹞﹟Ё﹜∮﹝﹚∨﹛∩∪∫∵∴∷Л﹄∶﹃﹂﹁П﹀О﹏]∽Б\\﹎﹍_﹌^﹋﹊Д﹉[ЗЪЫЮЯ`ЬЭ♂Т∏♀РСЦЧ∈Ф﹥л﹤к∕й﹦и﹡п﹠о﹣∑н﹢м∟г~∞}∝|а{з√ед¨§¤°±·⊥⊿×⊙÷ē！＃＆）ě（＋，ā：；＞？＠ī＾＼］ń｜｝～ň｛⌒ūǜǘǖǒǐǎ￤￥￢￣￠￡–‐‖―—’‘”“…‰″′‵※ˉˋˊ˙┙┘┛┚┝├┟┞┑┐┓┒┕└┗┖┈┉┊┋┌┍┎┏─━│┃┄┅┆┇┻ⅷ┺ⅶ┹ⅵ┸ⅴ┿ⅳ┾ⅲ┽ⅱ┼ⅰ┳┲┱┰┷┶┵ⅹ┴ⅸⅦ┪Ⅷ┫Ⅴ┨Ⅵ┩Ⅲ┮Ⅳ┯Ⅰ┬Ⅱ┭┢┣┠┡┦ⅪⅫ┧┤ⅨⅩ┥〡╝╜╟〣〢╞╙╘╛№╚╕╔╗╖║═╓╒℃╈╉℅╊╋╄╅℉╆╇╀╁╂╃。╳』╲『╱」╰╮〒╯〓╬【╭】╪〖╫〗╨〔╩╦╧╤╥╢〞╣╠╡〝μνξ▓ο▔θ▕ικλδεζη▁▃▂▅Ω▄Ψ▇▆▉█▋▊▍▌Π▏Σ▎▲Ξ△ΛΘΔΓ▼▽□■↘↙←↑→↓↖↗◇◆●◎○◥ω◤ψ◣◢σρπχφυτ";
//		doc=com.taobao.tbctu.help.TextHelper.skipChars(doc,specChars);
//		return doc;
//	}
//
//	protected List<String> participle(String doc) {
//		List<String> words=new ArrayList<String>();
//		if(StringUtil.isBlank(doc))
//			return words;	
//		words= KFCParticiple(doc);		
//		return words;
//	}

	/**
	 * 把几个分开的文本连接起来，其中连接符号要和以后分割的符号相同
	 * @param original_text
	 * @param indexes
	 * @param str_splitTag
	 * @return
	 */
	public String mergeText(String[] original_text,int[] indexes,String str_connentTag){
		String text="";
		for(int i =0;i<indexes.length;i++)
			text+=str_connentTag+original_text[indexes[i]];
		return text;
	}
	
	/**
	 * 计算SVM模型的分数.支持多模型
	 * @param text_arr  输入的文本，必须为String[]格式
	 * @param dic 字典
	 * @param model 模型
	 * @return
	 */
	public SvmResult calSigleTmsScore(List<String> text,TmsModel tmsModel){
		SvmModel svmModel = tmsModel.getModel();
        int label;
		double score;
        String descr;
		int nr_class = svmModel.getNrClass();
		double[] des_values =new double[nr_class*(nr_class-1)/2];
		SvmNode[] nodeList = consProForSVM(text, tmsModel);
		if (nodeList == null){ //如果该文本的词在词典 中没有出现过，则返回一个很小的值。
			return new SvmResult(0,0,"该样本太短");
		}	
		label = svmModel.predictValues(svmModel,nodeList,des_values);
		score = svmModel.sumPreValue(des_values);
		descr = tmsModel.getLabelDescr(label);
        return new SvmResult(label,score,descr);

	}

	/**
	 * 利用输入的文本，以及读入的词典，构造SVM模型(libsvm,liblinear)的特定输入
	 * 该函数的目的就是构造文本的特征向量，并进行归一化处理。但是此处为了提高效率，利用Map代替Vector，只存储非0值。
	 * @param text 里面存储的为一个个的词
	 * @param dic 利用存储的为词典，String 为词，Integer为其Id
	 * @return 返回的是SVM特定的输入结构,TmsNode[]
	 */
	
	public SvmNode[] consProForSVM(List<String> text,TmsModel tmsModel){
		Map<Integer,Double> feature_map = new HashMap<Integer,Double>();
		Map<String,Integer> dic = tmsModel.getDic();
		//计算文本中每个词对应与词典的位置，以及相应的词频。
		for(int i =0;i<text.size();i++){
			String term = (text.get(i).toString()).trim();
	        if (dic.containsKey(term)){ //查询dic中是否包含该词
	        	int index = dic.get(term); //如果包含，则在feature_map中相应位置加1
	        	if (feature_map.containsKey(index)){ //针对两种情况，一种是该词已经在词典中，
	        		double  count =  feature_map.get(index);
	        		feature_map .put(index, count+1.0);
	        	}
	        	else  //另一种是该词未在词典中
	        		feature_map .put(index, 1.0);
	        }
		}
		//根据特征权重的公式重新计算特征向量中的权重
		//依据的是局部公式和全局因子
		Object[] keys = feature_map.keySet().toArray();
		for(int i = 0;i<keys.length;i++){
			feature_map.put((Integer)keys[i],tmsModel.getLocalFun().Fun(feature_map.get(keys[i]))*tmsModel.getGlobalWeight().get(keys[i]) );
		}

		//计算该文档特征向量的模
		double vec_sum = 0.0;

		for(int i=0;i<keys.length;i++){
			vec_sum += feature_map.get(keys[i])*feature_map.get(keys[i]);
		}
	    double vec_length=Math.sqrt(vec_sum);
	    
	    //归一化并构造SVM模型的输入
	    SvmNode[] x=null;
	    Arrays.sort(keys); //对feature_map中的key进行排序。主要是为了保证输入的SVM格式中Index是升序排列。
	    if(vec_length>0){
	    	int m = keys.length;
			 x= new SvmNode[m]; //SVM模型的输入格式
			/**此处为构造SVM输入格式的句子**/
			//计算文本中的词出现的词频数
			for(int j=0;j<m;j++)
			{
				x[j] = new SvmNode();
				x[j].index = (Integer)keys[j];
				x[j].value = (double)( feature_map.get(keys[j])/vec_length); //此处要进行归一化
			}
	    }
		
	    return x;
	}
	
	/**
	 * 分词
	 * @param doc
	 * @return
	 */
//	protected List<String> KFCParticiple(String doc){
//		
//		
//		if(StringUtil.isBlank(doc))
//			return new ArrayList<String>();
//		
//		// 使用kfc 自带分词函数
//		WordPartition wp=new WordPartitionAdapter();
//		List<String> words=wp.partition(doc);
//		return words;
//		
//	}
	
	/**
	 * @param args
	 */
	public static void main(String[] args) {
		//不使用KFC的分词，利用已经分好词的样本对程序进行测试。
		String[] paths={"G:/x项目/Taobao/工作交接内容/模型/社区帖子监控/weijin_all_kinds/model","G:/x项目/Taobao/工作交接内容/模型/社区帖子监控/weijin_all_kinds/model","G:/x项目/Taobao/工作交接内容/模型/社区帖子监控/weijin_all_kinds/model","G:/x项目/Taobao/工作交接内容/模型/社区帖子监控/weijin_all_kinds/model"};
		String[] configNames ={"lineartitle.config","lineartitle_content.config","svmtitle.config","svmtitle_content.config"};
		int[][] indexes = {{4},{4,5},{4},{4,5}};
		TmsPredict libsvm = new TmsPredict(paths,configNames);
		
		String in_filename = "G:/x项目/Taobao/工作交接内容/模型/社区帖子监控/weijin_all_kinds/weijin_all_kinds_1116.train"; //输入文件
		String tc_splitTag="\t"; //输入的数据各个部分之间的分割符号
		String str_splitTag ="\\^"; //标题和内容经过分词后，各个词的分割符号
		String str_connentTag="^";
		String out_filename = "G:/x项目/Taobao/工作交接内容/模型/社区帖子监控/weijin_all_kinds/result.txt"; //结果保存的文件			
	
		try {	
			InputStream in = new FileInputStream(in_filename);				
			BufferedReader input = new BufferedReader(new InputStreamReader(in,"UTF-8"));
			PrintWriter output = new PrintWriter(out_filename);
			while(true){
				String line = input.readLine();	
				if(line == null) 
					break;
				String[] text  = line.split(tc_splitTag);
				SvmResult[] post_sc = libsvm.calScore(line,indexes,tc_splitTag,str_splitTag,str_connentTag); //计算得分（标题分，标题+内容分）
				//SvmResult[] post_sc = libsvm.calScore(line, str_splitTag); //计算得分（标题分，标题+内容分）
				output.print(text[2]+"\t"+text[0]+"\t");
				for(SvmResult score :post_sc)
					output.print(score.getLabel()+"\t"+score.getScore()+"\t");
				
				output.println();
			}
			input.close();
			output.close();	
		}catch(IOException e){
			e.printStackTrace();
		}
	}

}
