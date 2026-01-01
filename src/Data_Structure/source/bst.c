/* 
 * 构造一颗二叉查找树，实现树的插入、删除等基本操作 
 * 
 */  
#include<stdio.h>  
#include<stdlib.h>  
  
typedef struct node  
{  
    int count;  //记录某个元素出现的次数  
    int data;   //数据  
    struct node * left;  
    struct node * right;  
}Node,*PNode;  
  
  
int array[100];  //按序保存遍历后的元素  
int k=0;         //数组array长度  
  
//初始化一颗二叉排序树  
PNode init()  
{  
    PNode p=(PNode)malloc(sizeof(Node));  
    p->count=0;  
    p->left=p->right=NULL;  
    return p;  
}  
  
//插入结点  
void insert(PNode root ,int data)  
{  
    if(root!=NULL&&root->count==0){				//初始化后的第一个元素的count为0  
         root->data=data;  
         root->count++;  
         return;  
    }  
    if(data<root->data&&root->left==NULL){  
        PNode p=(PNode)malloc(sizeof(Node));  
        p->data=data;  
        p->count=1;  
        p->left=p->right=NULL;  
        root->left=p;  
        return;  
    }  
    if(data>root->data&&root->right==NULL){  
        PNode p=(PNode)malloc(sizeof(Node));  
        p->data=data;  
        p->count=1;  
        p->left=p->right=NULL;  
        root->right=p;  
        return;  
    }  

    if(data>root->data)  
        insert(root->right,data);  
    else if(data<root->data)  
        insert(root->left,data);  
    else{  
        root->count++;  
        return;  
    }  
}  
  
//删除结点,删除成功返回1，失败返回0  
int deleteNode(PNode* root ,int data)  
{  
    PNode p,q;  
    //寻找要删除的结点  
    if(*root==NULL)  
        return 0;  
    if((*root)->data==data){//找到要删除的结点  
        //要删除的结点是叶子结点，直接删除  
        if((*root)->left==NULL && (*root)->right==NULL){  
            p=*root;  
            *root=NULL;  
            free(p);  
            return 1;  
        }  
        //要删除的结点有左子树或者右子树,此时直接用左子树或右子树代替删除的结点  
        if((*root)->left==NULL || (*root)->right==NULL){  
            p=*root;  
            *root=(*root)->left==NULL?(*root)->right:(*root)->left;  
            p->left=p->right=NULL;  
            free(p);  
            return 1;  
        }  
        //要删除的结点有左子树和右子树。删除的结点的左子树代替删除的结点，然后  
        //一直查找删除的结点的左子树的右子树，直到出现为空。把删除结点的右子树插入  
        if((*root)->left && (*root)->right){  
            p=*root;  
            *root=(*root)->left;  
            q=*root;  
            while(q->right)  
                q=q->right;  
            q->right=p->right;  
            p->left=p->right=NULL;  
            free(p);  
            return 1;  
        }  
    }else if((*root)->data>data){  
        deleteNode(&(*root)->left,data);  
    }else  
        deleteNode(&(*root)->right,data);  
    return 0;  
}  

  
//先序遍历  
void  search_prior(PNode root)  
{  
    int i=0;  
    if(root==NULL)  
        return;   
    for(;i<root->count;i++)  
        array[k++]=root->data;  
    search_prior(root->left);    
    search_prior(root->right);  
}  
  
//后序遍历  
void  search_last(PNode root)  
{  
  
    int i=0;  
    if(root==NULL)  
        return;   
    search_prior(root->left);    
    search_prior(root->right);  
    for(;i<root->count;i++)  
        array[k++]=root->data;  
}  
  
//中序遍历  
void  search_middle(PNode root)  
{  
  
    int i=0;  
    if(root==NULL)  
        return;   
    search_middle(root->left);    
    for(;i<root->count;i++)  
        array[k++]=root->data;  
    search_middle(root->right);  
}  

  
//求树的高度  
int tree_height(PNode root)  
{  
    int h1,h2;  
    if(root!=NULL&&root->count==0)  
        return 0;  
    if(root==NULL)  
        return 0;  
    h1=1+tree_height(root->left);  
    h2=1+tree_height(root->right);  
    return h1>h2?h1:h2;  
}  
  
void main()  
{  
    int a[20];  
    int *p=a;  
    PNode pnode=init();  
    int i,n,deleteElement;  
  
    //读入要排序的数字个数和数字  
    printf("please input n(n<20):\n");  
    scanf("%d",&n);  
    for(i=0;i<n;i++)  
    {  
        printf("enter an Integer number:\n");  
        scanf("%d",p++);  
    }  
  
    //把待排序的元素插入二叉查找树中  
    for(i=0;i<n;i++){  
        insert(pnode,*(a+i));  
    }  
    search_middle(pnode);  
    printf("\n树的高度%d\n",tree_height(pnode));   
  
    //输出排序后的元素序列  
    for(i=0;i<k;i++)  
        printf("%-5d",array[i]);  
    printf("\n");  
      
    //删除结点  
    printf("请输入要删除的结点：\n");  
    scanf("%d",&deleteElement);  
    deleteNode(&pnode,deleteElement);  
    k=0;  
    search_middle(pnode);  
    printf("\n删除后：树的高度%d\n",tree_height(pnode));   
  
    //输出排序后的元素序列  
    for(i=0;i<k;i++)  
        printf("%-5d",array[i]);  
    printf("\n");  
  
    free(pnode);  
}  




