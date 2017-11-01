#ifndef _AVL_H_
#define _AVL_H_


template <class T>
class AVL{

	struct Node{
		int key;
		T data;
		unsigned char height;
		Node* left;
		Node* right;
		Node(T d, int k){
			data = d;
			key = k;
			left = right = 0; height = 1;
		}
	};	

	unsigned char height(Node* p){
		return p ? p->height : 0;
	}
	int bfactor(Node* p){
		return height(p->right) - height(p->left);
	}
	void fixheight(Node* p){
		unsigned char hl = height(p->left);
		unsigned char hr = height(p->right);
		p->height = (hl > hr ? hl : hr) + 1;
	}
	Node* rotateright(Node* p){
		Node* q = p->left;
		p->left = q->right;
		q->right = p;
		fixheight(p);
		fixheight(q);
		return q;
	}
	Node* rotateleft(Node* q){
		Node* p = q->right;
		q->right = p->left;
		p->left = q;
		fixheight(q);
		fixheight(p);
		return p;
	}

	Node* balance(Node* p){
		fixheight(p);
		if (bfactor(p) == 2){
			if (bfactor(p->right) < 0)
				p->right = rotateright(p->right);
			return rotateleft(p);
		}
		if (bfactor(p) == -2){
			if (bfactor(p->left) > 0)
				p->left = rotateleft(p->left);
			return rotateright(p);
		}
		return p;
	}

	Node* insert(Node* p, T d, int k){
		if (!p) return new Node(d, k);
		if (k < p->key){
			p->left = insert(p->left, d, k);
		}
		else{
			p->right = insert(p->right, d, k);
		}
		return balance(p);
	}

	Node* findmin(Node* p){
		return p->left ? findmin(p->left) : p;
	}

	Node* removemin(Node* p){
		if (p->left == 0)
			return p->right;
		p->left = removemin(p->left);
		return balance(p);
	}

	Node* remove(Node* p, int k){
		if (!p) return 0;
		if (k < p->key)
			p->left = remove(p->left, k);
		else if (k > p->key)
			p->right = remove(p->right, k);
		else{
			Node* q = p->left;
			Node* r = p->right;
			delete p;
			if (!r) return q;
			Node* min = findmin(r);
			min->right = removemin(r);
			min->left = q;
			return balance(min);
		}
		return balance(p);
	}

	Node* search(Node* p, int k){
		if (p != 0) {
			if (k == p->key)
				return p;
			if (k < p->key)
				return search(p->left, k);
			else
				return search(p->right, k);
		}
		else return 0;
	}

	Node* root;

public:

	AVL() : root(0) {}

	void insert(T d, int k){
		root = insert (root, d, k);
	}

	T findByID(int k){
		Node* node = search(root, k);
		T data = node->data;

		return data;
	}

	T findAndRemoveByID(int k){
		Node* node = search(root, k);
		T data;
		if (node){
			data = node->data;
			root = remove (root, k);
		}
		return data;
	}

	void remove (int k){
		Node* node = search(root, k);
		if (node)
			root = remove (root, k);
	}
};


#endif
