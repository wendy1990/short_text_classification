package tms;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.lang.Math;

//import org.apache.commons.logging.Log;
//import org.apache.commons.logging.LogFactory;


/**
 * TMS模型。
 * 
 * @author zhangzhilin.pt
 *
 */
public class TmsModel {
	
	public static void main(String args[]){
		String str = "b,";
		String []temp = str.split(",");
		System.out.println(temp.length);
		for(String s:temp){
			
		}
	}
	
	private String modelPath;//模型所在的路径。
	private SvmModel model;
	private Map<String,Integer> dic;
	private Map<Integer,Double> globalWeight;
	private LocalFun localFun; //模型训练时使用的局部函数
	private Map<Integer,String> labelDescr;//每个类标签的说明文字.  +++++
//	protected static final Log log = LogFactory.getLog(MsgClassifier.class);

	
	public TmsModel(String model_path,String configFilename){
		this.modelPath = model_path;
		initModel(this.modelPath+"/"+configFilename);
	}
	
	public TmsModel(String model_path){
		this(model_path,"tms.config");
	}

	
	
	/**
	 * 通过配置文件将模型的属性读入
	 * @param configFile
	 */
	public void initModel(String configFile){
		try {
			InputStream in = new FileInputStream(configFile);				
			BufferedReader br = new BufferedReader(new InputStreamReader(in,"UTF-8"));
			String line;
			while((line=br.readLine())!=null){
				String[] text  = line.split(":");
				if(text[0].trim().equals("ModelName")){//加载模型
					loadModel(this.modelPath+"/"+text[1].trim());
				}
				if(text[0].trim().equals("DicName")){//加载词典
					loadDic(this.modelPath+"/"+text[1].trim());
				}
				if (text[0].trim().equals("LocalFun")){
					loadLocalFun(text[1].trim());
				}
				if(text[0].trim().equals("Labels")){
					loadLabels(br);
				}
				
			}
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	/**
	 * 加载模型
	 * @param modelPath
	 */
	public void loadModel(String modelPath){
		this.model = new SvmModel(modelPath);
	}
	
	/**
	 * 加载词典以及全局因子。
	 * @param dicPath
	 */
	public void loadDic(String dicPath){
		this.dic = new HashMap<String,Integer>();
		this.globalWeight = new HashMap<Integer,Double>();
		BufferedReader input;
		try {
			InputStream in = new FileInputStream(dicPath);				
			input = new BufferedReader(new InputStreamReader(in,"UTF-8"));
			
			int count=0;
			while(true){
				String line = input.readLine();
				if(line == null) break;
				count+=1;
				String[] temp = line.split("\\s+");
				if (temp.length==1){//如果词典中只是词，则自动将index从1递增，global_weight=1
					dic.put(temp[0], count);
					this.globalWeight.put(count, 1.0);
				}
				if (temp.length==2){//如果词典中只是词和 index，则自动global_weight=1
					dic.put(temp[0], Integer.parseInt(temp[1]));
					this.globalWeight.put(Integer.parseInt(temp[1]), 1.0);
				}
				if(temp.length==3){
					dic.put(temp[0], Integer.parseInt(temp[1]));
					this.globalWeight.put(Integer.parseInt(temp[1]),Double.parseDouble(temp[2]) );
				}
				
			}
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	/**
	 * 加载局部函数
	 * @param localFunName
	 */
	public void loadLocalFun(String localFunName){
		this.localFun = new LocalFun(localFunName);
	}
	
	/**
	 * 加载类标签的说明文字 ++
	 * @param br
	 */
	public void loadLabels(BufferedReader br){
		this.labelDescr = new HashMap<Integer,String>();
		try {
			if(br.readLine().trim().equals("{")){
				String line ;
				while(!(line=br.readLine()).equals("}")){
					String []text = line.split(",");
					int label = Integer.parseInt(text[0]);
					if(text.length==1){
						if(label>0){
							this.labelDescr.put(label, "违规类");
						}else{
							this.labelDescr.put(label,"正常类");
						}
					}
					if(text.length>1){
						this.labelDescr.put(label,text[1]);
					}
				}
			}
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
	}
	
	/**
	 * 利用类标签获得该类标签的说明文字
	 * @param label
	 * @return
	 */
	public String getLabelDescr(int label){
		if(this.labelDescr.containsKey(label)){
			return this.labelDescr.get(label);
		}
		else{
			if(label>0)
				return new String("违规类");
			if(label<0)
				return new String("正常类");
			else return new String("样本太短或不符合要求");
		}
	}
	
	class LocalFun{
		/**
		 * 内部类，用来编写特征权重中局部函数的转换。		
		 */
		public String type; //局部函数的类型。
		public LocalFun(String type){
			this.type = type;
		}
		public double Fun(double x){
			if( this.type.equals("tf"))
				return this.tf(x);
			if( this.type.equals("logtf"))
				return this.logtf(x);
			if( this.type.equals("binary"))
				return this.binary(x);
			return x;
	
		}
		
		public double tf(double x){
			return x;
		}
		public double binary(double x){
			if(x>0) return 1.0;
			else return 0;
		}
		public double logtf(double x){
			return Math.log(x);
		}
		
	}

	public String getModelPath() {
		return modelPath;
	}

	public void setModelPath(String modelPath) {
		this.modelPath = modelPath;
	}

	public SvmModel getModel() {
		return model;
	}

	public void setModel(SvmModel model) {
		this.model = model;
	}

	public Map<String, Integer> getDic() {
		return dic;
	}

	public void setDic(Map<String, Integer> dic) {
		this.dic = dic;
	}

	public Map<Integer, Double> getGlobalWeight() {
		return globalWeight;
	}

	public void setGlobalWeight(Map<Integer, Double> globalWeight) {
		this.globalWeight = globalWeight;
	}

	public LocalFun getLocalFun() {
		return localFun;
	}

	public void setLocalFun(LocalFun localFun) {
		this.localFun = localFun;
	}

	public Map<Integer, String> getLabelDescr() {
		return labelDescr;
	}

	public void setLabelDescr(Map<Integer, String> labelDescr) {
		this.labelDescr = labelDescr;
	}
	
	
	

	
	

}
