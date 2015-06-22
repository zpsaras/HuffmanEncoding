class charFreq
{
public:
	charFreq();
	char character;
	unsigned long freq;
	charFreq * left;
	charFreq * right;
	bool operator<(charFreq other) const{
		return freq > other.freq;
	}
	struct PointerCompare{
		bool operator()(const charFreq* l, const charFreq* r){
			return *l < *r;
		}
	};


	/*
	* This lump of code written by dasblinkenlight
	* Grabbed from http://stackoverflow.com/questions/13484943/print-a-binary-tree-in-a-pretty-way */
	void postorder(charFreq* p, int indent = 0)
	{
		if (p != NULL) {
			if (p->left) postorder(p->left, indent + 4);
			if (p->right) postorder(p->right, indent + 4);
			if (indent) {
				std::cout << std::setw(indent) << ' ';
			}
			std::cout << p->freq << "\n ";
		}
	}

	void preorder(charFreq* p){
		if (p != NULL) {
			std::cout << p->freq << " ";
			if (p->left)
				preorder(p->left);
			if (p->right)
				preorder(p->right);
		}
	}
};

//Classic constructor
charFreq::charFreq()
{
	freq = 0;
	character = NULL;
	left = NULL;
	right = NULL;
}

class huffNode
{
public:
	huffNode();
	~huffNode();
	char c;
	std::bitset<8> code;
	huffNode * right;
	huffNode * left;
private:

};

huffNode::huffNode()
{
}

huffNode::~huffNode()
{
}