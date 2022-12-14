#pragma once


template <typename T>
struct EBuffer {
	int len;
	int bufferSize;
	T* data;

	static EBuffer fromBuffer(EBuffer<char>& eb, int szCell)
	{
		EBuffer ebConverted = { 0 };
		ebConverted.data = (T*)eb.data;
		ebConverted.bufferSize = eb.bufferSize;
		ebConverted.len = (eb.bufferSize / szCell);
		return ebConverted;
	}

	static EBuffer fromVector(std::vector<T> lst)
	{
		int i;
		int len;
		EBuffer vb;

		len = (int)lst.size();
		T* arr = new T[len];
		for (i = 0; i < len; i++)
		{
			arr[i] = lst[i];
		}
		vb.len = len;
		vb.data = arr;
		vb.bufferSize = len * sizeof(T);

		return vb;
	}

	static EBuffer fromVectorOffset(std::vector<T> lst, T off)
	{
		int i;
		int len;
		EBuffer vb;

		len = (int)lst.size();
		T* arr = new T[len];
		for (i = 0; i < len; i++)
		{
			arr[i] = off + lst[i];
		}
		vb.len = len;
		vb.data = arr;
		vb.bufferSize = len * sizeof(T);

		return vb;
	}

	void freeBuffer()
	{
		auto ptr = data;
		if (ptr)
		{
			data = NULL;
			free(ptr);
		}
	}
};
