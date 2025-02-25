template <typename Node, typename Update>

struct lazySGT
{
	int n;
	vector<Node> segtree;
	vector<bool> lazy;
	vector<Update> upd;
	vector<int> arr; // the type of array may change for higher dimensions

	lazySGT(int a_size, vector<int> &a)
	{
		n = a_size;
		arr = a;
		segtree.resize(4 * n);
		fill(all(segtree), Node());
		lazy.assign(4 * n, false);
		upd.resize(4 * n);
		fill(all(upd), Update());
		build(0, n - 1, 0);
	}

	// [l, r] -> node range of the tree

	void build(int l, int r, int index)
	{
		// if leaf node of segment tree
		if (l == r)
		{
			segtree[index] = Node(arr[l]);
			return;
		}
		int mid = (l + r) / 2;
		build(l, mid, 2 * index + 1);
		build(mid + 1, r, 2 * index + 2);
		segtree[index].merge(segtree[2 * index + 1], segtree[2 * index + 2]);
	}

	void pushdown(int l, int r, int index)
	{
		if (lazy[index])
		{
			int mid = (l + r) / 2;
			apply(l, mid, 2 * index + 1, upd[index]);
			apply(mid + 1, r, 2 * index + 2, upd[index]);
			lazy[index] = false;
			upd[index] = Update();
		}
	}

	void apply(int l, int r, int index, Update &u)
	{
		if (l != r)
		{
			lazy[index] = true;
			upd[index].combine(l, r, u); // combine of updates
		}
		u.apply(l, r, segtree[index]);
		if (l == r)
		{
			u.leafUpdate(arr[l]);
		}
	}

	void update(int l, int r, int index, int start, int end, Update u)
	{
		// start ... l ... r .... end -> complete overlap
		if (start <= l && r <= end)
		{
			apply(l, r, index, u);
			return;
		}
		// l ... r ... start .... end || start ... end ... l ... r -> no overlap
		if (r < start || end < l)
		{
			return;
		}
		pushdown(l, r, index);
		int mid = (l + r) / 2;
		update(l, mid, 2 * index + 1, start, end, u);
		update(mid + 1, r, 2 * index + 2, start, end, u);
		segtree[index].merge(segtree[2 * index + 1], segtree[2 * index + 2]);
	}

	Node query(int l, int r, int index, int start, int end)
	{
		// start ... l ... r ... end -> complete overlap
		if (start <= l && r <= end)
		{
			pushdown(l, r, index);
			return segtree[index];
		}
		// l .... r ..... start .... end || start ... end .... l .... r -> no overlap
		if (r < start || end < l)
		{
			return Node();
		}
		pushdown(l, r, index);
		int mid = (l + r) / 2;
		Node res;
		Node leftAns = query(l, mid, 2 * index + 1, start, end);
		Node rightAns = query(mid + 1, r, 2 * index + 2, start, end);
		res.merge(leftAns, rightAns);
		return res;
	}

	void make_update(int l, int r, int val)
	{
		Update u(val);
		update(0, n - 1, 0, l, r, u);
	}

	Node make_query(int l, int r)
	{
		return query(0, n - 1, 0, l, r);
	}
};

struct Node1
{
	int val1;
	Node1()
	{
		val1 = 0; // identity element for range query operation
	}
	Node1(int a)
	{
		val1 = a;
	}
	void merge(const Node1 &a, const Node1 &b)
	{
		val1 = a.val1 + b.val1; // range query operation
	}
};

struct Update1
{
	int val1;
	Update1()
	{
		val1 = 0; // identity element for range update operation
	}
	Update1(int a)
	{
		val1 = a;
	}
	void apply(int l, int r, Node1 &a)
	{
		a.val1 = (r - l + 1) * val1; // how the update operation will combine together with range query operation
	}
	void combine(int l, int r, Update1 &other)
	{
		val1 = other.val1; // other.val is the new update as they get combined
	}
	void leafUpdate(int &a)
	{
		a = val1; // update on the leaf
	}
};