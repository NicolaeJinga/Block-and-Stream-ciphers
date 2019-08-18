namespace Memory
{
	template<class T> void Destroy(T* &ptr)
	{
		if (ptr != nullptr)
		{
			delete ptr;
			ptr = nullptr;
		}
	}
	
	template<class T> void DestroyArray(T* &ptr)
	{
		if (ptr != nullptr)
		{
			delete[] ptr;
			ptr = nullptr;
		}
	}
	
	template<class T> void DestroyMatrix(T* &ptr, int n)
	{
		if (ptr != nullptr)
		{
			for (int i = 0; i < n; ++i)
			{
				if (ptr[i] != nullptr)
				{
					delete[] ptr[i];
					ptr[i] = nullptr;
				}
			}
			delete[] ptr;
			ptr = nullptr;
		}
	}
}