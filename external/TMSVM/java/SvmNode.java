package tms;
import libsvm.svm_node;
import de.bwaldvogel.liblinear.FeatureNode;;

public class SvmNode {
	public int index;
	public double value;
	
	public String toString(){
		return String.valueOf(index)+"\t"+String.valueOf(value);
	}
}
