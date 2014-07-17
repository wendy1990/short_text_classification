package tms;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;
import java.util.regex.Pattern;

//import org.apache.commons.logging.Log;
//import org.apache.commons.logging.LogFactory;


import libsvm.*;
import de.bwaldvogel.liblinear.*;

/**
 * SVM模型，封装libsvm与liblinear 。
 * 需要特别注意的有3个地方：模型svm_model VS Model ; 问题类型 :svm_node VS Feature ;
 * @author 张知临 
 * @contact zhzhl202@163.com
 * @date 2011/11/9
 *
 */
public class SvmModel {
	
	public static void main(String args[]){
		FeatureNode[] NodeList = new FeatureNode[5];
	}
	
	public Model linearModel;
	public svm_model svmModel;
	private String svm_type;
	private int nr_class;//模型中类别的个数。
//	protected static final Log log = LogFactory.getLog(MsgClassifier.class);
	
	Pattern whitespace = Pattern.compile("\\s+");
	
	public SvmModel(String model_filename){
		this.load_model(model_filename);
	}
	
	public void load_model(String model_filename){
		BufferedReader fp;
		try {
			InputStream in = new FileInputStream(model_filename);				
			fp = new BufferedReader(new InputStreamReader(in,"UTF-8"));

			String line = null;
			try {
				while((line = fp.readLine())!=null){			
					String[] arg = whitespace.split(line);
					if(arg[0].equals("svm_type")){
						svm_type = "libsvm";
						svmModel = svm.svm_load_model(model_filename);
						nr_class = svmModel.nr_class;
						linearModel = null;
						break;
					}
					if(arg[0].equals("solver_type")){
						svm_type="liblinear";
						svmModel = null;
						linearModel = Linear.loadModel(new File(model_filename));
						nr_class = linearModel.getNrClass();
						break;
					}
				}
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		} catch (UnsupportedEncodingException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	/**
	 * 根据已有的SVM模型，预测得分
	 * @param model
	 * @param x
	 * @param dec_values
	 * @return
	 */
	public static int predictValues(SvmModel model, SvmNode[] problem, double[] dec_values){
		if(model.getSvmType().equals("libsvm")){
			return (int)svm.svm_predict_values(model.svmModel, (svm_node[])changeProblemType(problem,model.getSvmType()), dec_values);
		}
		if(model.getSvmType().equals("liblinear")){
			return Linear.predictValues(model.linearModel, (FeatureNode[])changeProblemType(problem,model.getSvmType()), dec_values);
		}
		else return 0;
		
	}
	
	public static Object[] changeProblemType(SvmNode[] problem,String svmType){
		if (svmType.equals("libsvm")){
			svm_node[] NodeList= new svm_node[problem.length];
			for(int i =0;i<problem.length;i++){
				NodeList[i] = new svm_node();
				NodeList[i].index = problem[i].index;
				NodeList[i].value = problem[i].value;
			}
			return NodeList;
		}
		else if(svmType.equals("liblinear")){
			FeatureNode[] NodeList = new FeatureNode[problem.length];
			for (int i =0;i<problem.length;i++){
				NodeList[i] = new FeatureNode(problem[i].index,problem[i].value);
			}
			return NodeList;
		}
		return null;
		
	}
	
	/**
	 * 返回具有最大投票数的标签所获得分数的总和
	 * @param des_values
	 */
	public double sumPreValue(double[] des_values){
		int size=1;
		double init_score=0.0;
		int k=1;
		if (this.svm_type.equals("libsvm")){
			int n = 1+(int)Math.sqrt(2.0*des_values.length+1);
			size = n-1;
			int [] vote = new int[n];
			double[] score = new double[n];
			int p =0;
			for(int i =0;i<n-1;i++){
				for(int j =i+1;j<n;j++){
					if(des_values[p]>0){
						vote[i]+=1;
						score[i]+=Math.abs(des_values[p]);
					}else{
						vote[j]+=1;
						score[j]+=Math.abs(des_values[p]);
					}
					p+=1;
				}
			}
			int max = 0;
			for(int i =1;i<n;i++)
				if(vote[i]>vote[max])
					max = i;
			k = vote[max];
			init_score = score[max];
		}
		if (this.svm_type.equals("liblinear")){
			int n =des_values.length;
		    int max = 0;
		    for(int i =1;i<n-1;i++){
		    	if(des_values[i]>des_values[max]){
		    		max = i;
		    	}
		    }
		    size =1;
		    k=1;
		    init_score = des_values[max];
		     
		}
		return init_score/(2.0*k)+k/(2.0*size) ;

	}
	
	public String getSvmType(){
		return this.svm_type;
	}
	
	public int getNrClass(){
		return this.nr_class;
	}
	
}
