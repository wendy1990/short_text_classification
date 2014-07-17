#!/usr/bin/python
#_*_ coding: utf-8 _*_
#author: 张知临 zhzhl202@163.com
#Filename: lsa.py

''' the main function of LSA Model,include :
X(m,n) = U(m,k)S(k,k)V(k,n)'
the reconstructed document vector is S(k,k)*V(k,n)
and we will do the preprocess to the new coming document following the formula:Q=q(1,m)*U(m,k)

the input file format: label score values


the main steps:
step1:read sample to construct the sparse matrix,
step2: execute the SVD decompose
step3:construct svm classfier and get the initial score
step4:set the local region and do the local latent semantic analysis 

# the method that sort dictionary: 
    sorted(dic.items(),key=lamda dic:dic[0],reverse = False):ascending ordering by key
    sorted(dic.items(),key=lamda dic:dic[1],reverse = True):decending ordering by value
'''

from scipy import *
from scipy.sparse.linalg import svds
from scipy.sparse import *
from numpy import *
import os.path
import pickle

c_p = os.path.dirname(os.getcwd())+"/"

score_dic={} #{id:{label,{1:2,4:2}}} id begin from 0
label_dic = {} #class label 
feature_dic = {} # 
#M=12 #pre_establish demostrate the size of dictionary
#N=9 #pre_define indicate the size of trainset
def ctm_read_problem(filename):
    '''sample id bigin from 0'''
    global score_dic
    global label_dic
    global feature_dic
    f =file(filename,'r')
    count = 0
    for line in f.readlines():
        line = line.split(None, 2)
        # In case an instance with all zero features
        if len(line) == 2: line += ['']
        label, score,features = line
        label_dic[count]=int(label)
        score_dic[count] = float(score)
        feature_dic[count]={}
        for e in features.split():
            ind, val = e.split(":")
            feature_dic[count][int(ind)] = float(val)
        count+=1
    f.close()   

def ctm_sort(dic,sorted_key=1,reverse = True):
    '''sort the dictionary'''
    return sorted(dic.items(),key=lambda dic:dic[sorted_key],reverse=reverse)
def select_top_n_by_threhold(sorted_list,threhold):
    if threhold<sorted_list[len(sorted_list)-1][1]:
        return len(sorted_list)
    if threhold>sorted_list[0][1]:
        return 0   
    right = len(sorted_list)-1
    left = 0
    midial =(right+left)/2
    while right>=left:
        if  sorted_list[midial][1]>=threhold and sorted_list[midial+1][1]<=threhold:
            break
        if sorted_list[midial][1]<threhold:
            right = midial
            midial = (right+left)/2
        if sorted_list[midial][1]>threhold:
            left = midial
            midial = (right+left)/2
    return midial+1     

def cal_weight(rs,a=5.0,b=0.2):
    return 1.0/(1.0+exp(-a*(rs+b)))
    
def weight_feature(weight_fun=cal_weight):
    global score_dic
    global feature_dic
    for key1 in score_dic.keys():
        weight = weight_fun(float(score_dic[key1]))
        for key2 in feature_dic[key1]:
            feature_dic[key1][key2]*=weight 
def cons_sparse_matrix(sorted_list,n,M):
    global feature_dic
    indptr=[0]
    indices = []
    data=[]
    count=0
    for i in range(n):# select top n
        id = sorted_list[i][0]
        temp_dic = feature_dic[id]
        sorted_feature_list = sorted(temp_dic.items(),key=lambda dic:dic[0])
        for key in sorted_feature_list:
           count+=1
           indices+=[key[0]-1]
           data+=[float(key[1])]
        indptr+=[int(count)] 
    return csc_matrix( (array(data),array(indices),array(indptr)) ,shape=(M,n),dtype=float32)
    
def save_sample_for_cla(f,lab,matrix):
    m,n = matrix.shape
    for i in range(m):
        f.write(str(lab[i])+"\t")
        for j in range(n):
            if matrix[i][j]!=0:
                f.write(str(j+1)+":"+str(matrix[i][j])+"\t")
        f.write("\n")
                
def save_all_sample_for_cla(save_path,U,S,V,sorted_list,n,M):
    '''save all the trainset'''
    f= file(save_path,'w')
#    matrix= dot(V,S)
#    svd_label =[]
#    for i in range(n):
#        svd_label+=[int(label_dic[sorted_list[i][0]])]
#    #
#    save_sample_for_cla(f,[1]*matrix.shape[0],matrix)
    '''experiment has show that time eaclpse for calcalate a entire matrix and all rows of matrix is equal'''
    for i in range(0,len(sorted_list)):
        id = sorted_list[i][0]
        label = [label_dic[id]]
        vec = [0]*M
        temp_dic = feature_dic[id]
        sorted_feature_list = sorted(temp_dic.items(),key=lambda dic:dic[0])
        for key in sorted_feature_list:
            if key[1]!=0:
                vec[int(key[0])-1]=float(key[1])
        matrix = array([vec])
        #map to the reduced space
        matrix = pre_doc_svds(matrix,U)
        save_sample_for_cla(f,label,matrix)
    f.close()
#    f_temp=file("lsa_test_id.txt",'w')
#    for i in range(len(sorted_list)):
#        f_temp.write(str(sorted_list[i][0])+"\n")
#    f_temp.close()


def ctm_svds(matrix,k):
    '''matrix(m,n) U(m,k) S(k),V(n,k)'''
    U,S,V = svds(matrix,k,min(2*k+1,matrix.shape[1]))
    return U,diag(S),V.transpose()

def set_k_svds(U,S,V,k):
    return U[:,U.shape[1]-k:U.shape[1]],S[S.shape[0]-k:S.shape[0],S.shape[1]-k:S.shape[1]],V[:,V.shape[1]-k:V.shape[1]]

def pre_doc_svds(doc_vec,U):
    '''preprocess the new document,the dimen of the doc is 1*m ,m describe the size of dictionary
    Formula:Q = q*U(m,k)
    
    '''
    return dot(doc_vec,U)

def save_lsa_model(U,S,V,save_path):
    '''save LSA model containing the matrix U,S,V'''
    pickle.dump(U,open(save_path+".U",'wb'))
    pickle.dump(S,open(save_path+".S",'wb'))
    pickle.dump(V,open(save_path+".V",'wb'))
    
def load_lsa_model(lsa_model_path,matrix="U"):
    '''need to specify the matrix(U or S or V) to load'''
    return pickle.load(open(lsa_model_path+"."+str(matrix),'rb'))

def ctm_lsa(M,threhold,k,for_lsa_train_save_path,train_save_path,model_save_path):
    ctm_read_problem(for_lsa_train_save_path)
    sorted_list=ctm_sort(score_dic)
    weight_feature()
    n=select_top_n_by_threhold(sorted_list,threhold)
    print n
    print "now is constructing the sparse matrix"
    matrix = cons_sparse_matrix(sorted_list,n,M)
    pickle.dump(matrix,open("../model/im_info_lsa/lsa.matrix",'wb'))
    print "now is do the SVD decomposing"
    U,S,V=ctm_svds(matrix,k=k)
    U,S,V=set_k_svds(U,S,V,k=k)
    print "now is saving the training sample"
    save_all_sample_for_cla(train_save_path,U,S,V,sorted_list,n,M)
    save_lsa_model(U,S,V,model_save_path)
    
def local_lsa():
    '''set a local region,and do the local svd,'''
    
#sorted_list  = [10,9,8,7,6,5,4,3,2,1]
#threhold =10
#print select_top_n_by_threhold(sorted_list,threhold)
#def main():
#    M=12
#    N=9
#    filename = c_p + "model/model_0829_dic1/ref.result"
#    save_path = c_p + "model/model_0829_dic1/lsa.train"
#    ctm_read_problem(filename)
#    sorted_list=ctm_sort(score_dic)
#    weight_feature()
#    n = 5
#    k=4
#    print "now is constructing the sparse matrix"
#    matrix = cons_sparse_matrix(sorted_list,n,M)
#    print "now is do the SVD decomposing"
#    U,S,V=ctm_svds(matrix,k=k)
#    U,S,V=set_k_svds(U,S,V,k=k)
#    print "now is saving the training sample"
#    save_all_sample_for_cla(save_path,U,S,V,sorted_list,n,M,N)

#filename = c_p + "model/model_0829_dic1/ref.result"
#train_save_path = c_p + "model/model_0829_dic1/ref_lsa.train"
#model_save_path = c_p + "model/model_0829_dic1/ref_lsa"
#M=12
#N=9
#n = 5
#k=3
#ctm_lsa(M,N,n,k,filename,train_save_path,model_save_path)
    
    
    
    
    
