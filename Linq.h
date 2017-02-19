

namespace ztLINQ{
	
	/*
	 *select_iterator为select函数的迭代器
	 */
	template <typename Iterator,typename Function>
	class select_iterator{
	private:
		Iterator _iterator;
		Function _function;
	public:
		select_iterator(const Iterator& i,const Function& f):
						_iterator(i),_function(f){}
		
		select_iterator& operator++(){
			++_iterator;
			return *this;
		}
		//重载*操作符
		auto operator*() const->decltype(_function(*_iterator)){
			return _function(*_iterator);
		}

		bool operator ==(const select_iterator& si) const{
			return si._iterator==_iterator;
		}
		
		bool operator !=(const select_iterator& si) const{
			return !(si._iterator==_iterator);
		}
	};

	template <typename Iterator>
	class linq_enumerable{
	private:
		Iterator _begin;	//迭代器起始位置
		Iterator _end;		//迭代器末尾
	public:
			linq_enumerable(const Iterator& b,const Iterator& e):
							_begin(b),_end(e){}

			Iterator begin() const {
				return _begin;
			}

			Iterator end() const {
				return _end;
			}

			template <typename Function>
			auto select(const Function& _function) const->
	}
}
