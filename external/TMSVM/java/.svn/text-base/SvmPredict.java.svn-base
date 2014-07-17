import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Vector;

import libsvm.svm;
import libsvm.svm_model;
import libsvm.svm_node;

/**
 * SVM 预测程序
 * 支持多模型的预测。
 * 输入文件没有特定格式，预测时需指定各字段的分隔符、分词的分割符号、每一个模型预测的字段
 * @author 张知临 zhzhl202@163.com
 * @date 2011-10-26
 */
public class SvmPredict {
	
	public static void main(String []args){
		
		/**
		 * 社区帖子监控示例程序。这里演示的是从文件中读入帖子数据，然后把结果写入到文件中。
		 * 输入的数据格式为 thread_id,group_id,subject,content。即帖子id,组id,标题,内容
		 */
		SvmPredict libsvm = new SvmPredict();
		String in_filename = "D:/张知临/源代码/libsvm_java/model/im_info/example.test"; //输入文件
		String tc_splitTag="\t"; //输入的数据各个部分之间的分割符号
		String str_splitTag ="\\^"; //标题和内容经过分词后，各个词的分割符号
		String str_connentTag ="^"; //如果需要对多个部分综合在一起评分，就要连接起来，其实这个和str_splitTag保持一致，但是因为在java中^和\\^不相同，所以要分开写
		String out_filename = "D:/张知临/源代码/libsvm_java/model/im_info/result.txt"; //结果保存的文件			
	
		try {			
			BufferedReader input = new BufferedReader(new FileReader(in_filename));
			DataOutputStream output = new DataOutputStream(new BufferedOutputStream(new FileOutputStream(out_filename)));
			while(true){
				String line = input.readLine();	
				if(line == null) 
					break;
				double[] post_sc = libsvm.cal_post_sc(line,tc_splitTag, str_splitTag, str_connentTag); //计算帖子的得分（标题分，标题+内容分）
				for(double score :post_sc)
					output.writeBytes(score+"\t");
				output.writeBytes("\n");
			}
			input.close();
			output.close();	
		} catch (FileNotFoundException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}catch(IOException e){
			e.printStackTrace();
		}
		
	}
	
	
	
	private String model_main_path = "D:/张知临/源代码/libsvm_java/model/im_info/"; //模型存放的主路径

	private String[] dic_path ={model_main_path+"im.key"}; //词典列表
	private String[] model_path ={model_main_path+"im.model"}; //模型列表
	private int[][] indexes={ {6} };//标题模型对应的文本就是标题，即在输入文本的下标为2。标题+内容模型对应的文本就是标题和内容，即在输入文本的下标为2和3
	
	private List< Map<String,Integer> >  dics; //标题词典,标题+内容词典
	private List< svm_model> models; 
	
	public SvmPredict(){
		init_model();
	}
	/**
	 * 	初始化部分：读入模型，包括词典和SVM模型。
	 */
	public void init_model(){
		//读入词典
		int k = dic_path.length;
		dics = new ArrayList< Map<String,Integer> >();
		models = new ArrayList< svm_model >();

		try {
			for(int i =0;i<k;i++){
				dics.add(read_dic(dic_path[i]));
				models.add(svm.svm_load_model(model_path[i]));
			}
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
/**
 * 计算分数的入口程序，给定输入，以及各种分割符号。
 * 然后利用所有的模型进行预测分数
 * @param line 读入的文本
 * @param tc_splitTag 读入文本分为几个部分，各个部分之间的分割符号
 * @param str_splitTag 分词后文本的分割符号。
 * @param str_connentTag 如果要多个部分放在一起预测，需要先用连接符号连接在一起，实际上str_splitTag和str_connentTag是一个东西。但是在java中两者不能相同 ^ \\^
 * @return
 */
	public double[] cal_post_sc(String line,String tc_splitTag,String str_splitTag,String str_connentTag){
		String []input_text = line.split(tc_splitTag);
		int k = dics.size();
		double[] scores = new double[k];
		for(int i=0;i<k;i++){
			int[] index = indexes[i];
			Map<String,Integer> dic = dics.get(i);
			svm_model model = models.get(i);
			String text = merge_text(input_text, index,str_connentTag);
			scores[i] = cal_svm_sc(text.split(str_splitTag), dic, model);

		}
		return scores;

	}
	
	/**
	 * 计算SVM模型的分数.支持多模型
	 * @param text_arr  输入的文本，必须为String[]格式
	 * @param dic 字典
	 * @param model 模型
	 * @return
	 */
	public double cal_svm_sc(String[] text_arr,Map<String,Integer> dic,svm_model model){
		Vector text = new Vector<String>(); //文本Vector
		for(int i =0;i<text_arr.length;i++){
			if (text_arr[i].trim().length()>0)
				text.add(text_arr[i]);
		}

		int nr_class = model.nr_class;
		double[] des_values =new double[nr_class*(nr_class-1)/2];
		svm_node[] x = cons_pro_for_svm(text, dic);
		if (x==null) //如果该文本的词在词典 中没有出现过，则返回一个很小的值。
			return -10;
		double pred_result = svm.svm_predict_values(model,x,des_values);
		if (nr_class>2)
			return  pred_result;
		else 
			return des_values[0];

	}

	/**
	 * 根据SVM特定的问题结构预测分数
	 * @param x SVM问题特定的输入格式
	 * @param dic 词典 
	 * @param model 模型
	 * @return
	 */
	public double predict_svm_sc(svm_node[] x,Map<String,Integer> dic,svm_model model){
		int nr_class = model.nr_class;
		double[] des_values =new double[nr_class*(nr_class-1)/2];

		if (x==null) //如果该文本的词在词典 中没有出现过，则返回一个很小的值。
			return -10;
		double pred_result = svm.svm_predict_values(model,x,des_values);
		if (nr_class>2)
			return pred_result;
		else 
			return des_values[0];
	}
	
	/**
	 * 把几个分开的文本连接起来，其中连接符号要和以后分割的符号相同
	 * @param original_text
	 * @param indexes
	 * @param str_splitTag
	 * @return
	 */
	public String merge_text(String[] original_text,int[] indexes,String str_connentTag){
		String text="";
		for(int i =0;i<indexes.length;i++)
			text+=str_connentTag+original_text[indexes[i]];
		return text;
	}
	
	/**
	 * 读入词典
	 * @param dic_path
	 * @return
	 */
	public Map<String,Integer> read_dic(String dic_path){
		Map<String,Integer>  dic = new HashMap<String,Integer>();//词典存储的Map
		BufferedReader input;
		try {
			input = new BufferedReader(new FileReader(dic_path));
			int count=0;
			while(true){
				String line = input.readLine();
				if(line == null) break;
				count+=1;
				String[] temp = line.split("\t");
				if (temp.length>1)
				dic.put(temp[0], Integer.parseInt(temp[1]));
				else dic.put(temp[0], count);
			}
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return dic;
	}
	
	/**
	 * 利用输入的文本，以及读入的词典，构造SVM模型的特定输入
	 * 该函数的目的就是构造文本的特征向量，并进行归一化处理。但是此处为了提高效率，利用Map代替Vector，只存储非0值。
	 * @param text 里面存储的为一个个的词
	 * @param dic 利用存储的为词典，String 为词，Integer为其Id
	 * @return 返回的是SVM特定的输入结构
	 */
	public svm_node[] cons_pro_for_svm(Vector<String> text,Map<String,Integer> dic){
		Map feature_map = new HashMap<Integer,Integer>();
		
		//计算文本中每个词对应与词典的位置，以及相应的词频。
		for(int i =0;i<text.size();i++){
			String term = (text.elementAt(i).toString()).trim();
	        if (dic.containsKey(term)){ //查询dic中是否包含该词
	        	int index = dic.get(term); //如果包含，则在feature_map中相应位置加1
	        	if (feature_map.containsKey(index)){ //针对两种情况，一种是该词已经在词典中，
	        		int  count = (Integer) feature_map.get(index);
	        		feature_map.put(index, count+1);
	        	}
	        	else  //另一种是该词未在词典中
	        		feature_map .put(index, 1);
	        }
		}
		//计算该文档特征向量的模
		double vec_sum = 0.0;
		Object[] keys = feature_map.keySet().toArray();
		for(int i=0;i<keys.length;i++){
			vec_sum += (Integer)feature_map.get(keys[i])*(Integer)feature_map.get(keys[i]);
		}
	    double vec_length=Math.sqrt(vec_sum);
	    
	    //归一化并构造SVM模型的输入
	    svm_node[] x=null;
	    Arrays.sort(keys); //对feature_map中的key进行排序。主要是为了保证输入的SVM格式中Index是升序排列。
	    if(vec_length>0){
	    	int m = keys.length;
			 x= new svm_node[m]; //SVM模型的输入格式
			/**此处为构造SVM输入格式的句子**/
			//计算文本中的词出现的词频数
			for(int j=0;j<m;j++)
			{
				x[j] = new svm_node();
				x[j].index = (Integer)keys[j];
				x[j].value = (double)((Integer) feature_map.get(keys[j])/vec_length); //此处要进行归一化
			}
	    }
		
	    return x;
	}

}
