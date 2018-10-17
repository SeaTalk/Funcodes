#include <iostream>
#include<vector>
using namespace std;

struct TreeNode {
	int val;
	struct TreeNode *left;
	struct TreeNode *right;
	TreeNode(int x) :
		val(x), left(NULL), right(NULL) {
	}
};

// 插入：因为二叉搜索树不允许存在相等的值，所以插入有可能失败

// 1. 非递归版
bool BSTInsert(TreeNode* &root, int val) {
	
	if (root == NULL) {
		root = new TreeNode(val);
		return true;
	}
	TreeNode* x = root, *y;
	while (x != NULL) {
		y = x;
		if (val == x->val)
			return false;
		if (val < x->val)
			x = x->left;
		else
			x = x->right;
	}
	if (val < y->val)
		y->left = new TreeNode(val);
	else
		y->right = new TreeNode(val);
	return true;
}

// 2. 递归版
bool BSTInsert2(TreeNode* &root, int val) 
{
	if (root == NULL) {
		root = new TreeNode(val);
		return true;
	}
	if (val == root->val)
		return false;
	if (val < root->val)
		return BSTInsert2(root->left, val);
	else return BSTInsert2(root->right, val);
}

// 创建
void BSTCreate(TreeNode* &root, vector<int> &a)
{
	for (int val : a)
		BSTInsert2(root, val);
}

// 中序遍历验证是不是正确（严格递增）
void helper(vector<int> &res, TreeNode* root)
{
	if (root == NULL)
		return;
	helper(res, root->left);
	res.push_back(root->val);
	helper(res, root->right);
}
vector<int> InOrderTraverse(TreeNode* root)
{
	vector<int> res;
	helper(res, root);
	return res;
}

//查找
//1. 非递归版
bool BSTSearch(TreeNode* root, int val)
{
	while (root != NULL && root->val != val)
	{
		if (val < root->val)
			root = root->left;
		else
			root = root->right;
	}
	return root != NULL;
}

// 2.递归版
bool BSTSearch2(TreeNode* root, int val)
{
	if (root == NULL)
		return  false;
	if (root->val == val)
		return true;
	if (root->val > val)
		return BSTSearch2(root->left, val);
	else return BSTSearch2(root->right, val);
}


//删除
/*********三种情况********
 *1.删除结点为叶子，直接删除
 *2.删除结点仅有一个子树，直接用其子树替代即可
 *3.删除结点两个子树都不为空，然后用其直接前驱或直接后继替换（直接前驱的思路是找其左子树的右结点，直到右侧尽头）
 */
void deleteHelper(TreeNode* node)
{
	if (node->left == NULL)
	{
		if (node->right == NULL) {
			delete node;
		}
		else
		{
			TreeNode *q = node;
			node = node->right;
			delete q;
		}
		return;
	}
	else if (node->right == NULL)
	{
		TreeNode *q = node;
		node = node->left;
		delete q;
		return;
	}
	else
	{
		TreeNode *q = node, *l = node->left;
		while (l->right != NULL)
		{
			q = l;
			l = l->right;
		}
		node->val = l->val;
		if (q == node)
			q->left = l->left;
		else
			q->right = l->right;
	}
}

bool BSTDelete(TreeNode* root, int val)
{
	if (root == NULL)
		return false;
	if (root->val > val)
		return BSTDelete(root->left, val);
	if (root->val < val)
		return BSTDelete(root->right, val);
	deleteHelper(root);
	return true;
}

void deleteAllTree(TreeNode* root)
{
	if (root == NULL) return;
	deleteAllTree(root->left);
	deleteAllTree(root->right);
	delete root;
}

void main()
{
	vector<int> v;
	v.push_back(3);
	v.push_back(1);
	v.push_back(5);
	v.push_back(0);
	v.push_back(2);
	v.push_back(9);

	TreeNode *t = NULL;
	BSTCreate(t, v);

	vector<int> res = InOrderTraverse(t);
	for each (int r in res)
	{
		cout << r << " ";
	}

	cout << BSTDelete(t, 1) << " ";

	deleteAllTree(t);

	try 
	{
		cout << t->val  << endl;
	}
	catch(exception e)
	{
		cout << "exception:" << e.what() << endl;
	}
	
}