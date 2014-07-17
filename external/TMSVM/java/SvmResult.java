package tms;

public class SvmResult {
    public int label;
    public double score;
    public String descr;

    public SvmResult( int label, double score ,String descr) {
        this.label = label;
        this.score = score;
        this.descr = descr;
    }

	public double getLabel() {
		return label;
	}

	public void setLabel(int label) {
		this.label = label;
	}

	public double getScore() {
		return score;
	}

	public void setScore(double score) {
		this.score = score;
	}

	public String getDescr() {
		return descr;
	}

	public void setDescr(String descr) {
		this.descr = descr;
	}
    
	
    
}
