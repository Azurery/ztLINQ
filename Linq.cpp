
#include <iostream>
#include <vector>

	/*对于各种Iterator类，它需要接收一个或多个表示不确定类型的迭代器，
	*并且视情况需要接受一个或多个不确定类型的lambda
	*/
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

	/*当where_iterator自增时，他会有一个谓词条件，若没有满足这个条件，则会继续自增，以此过滤不满足条件的元素
	 *where_iterator的取值操作为：直接对它所包装的迭代器进行*操作即可
	 *
	 */
	template <typename Iterator,typename Function>
	class where_iterator{
	private:
		Iterator _iterator;
		Iterator _end;
		Function _function;
	public:
		where_iterator(const Iterator& i,const Iterator& e,const Function& f):
						_iterator(i),_end(e),_function(f){
			while(iterator!=end && !_function(*iterator)){
				++iterator;
			}
		}		

		where_iterator& operator++(){
			if(iterator==_end)
				return *this;
			++iterator;
			while(iterator!=end && ！_function(*iterator)){
				++iterator;
			}
			return *this;
		}


	}

	/*linq_enumerable类中不保存对象本身，而是保存容器的迭代器，这也是一般的函数式语言的实现方式
	 *select返回的对象与where返回的对象的唯一区别就是迭代器不同。我们可以设计一个通用的迭代器，
	 *它可以处理多种不同的迭代需求，包括跳过一些元素或者对一些元素做处理，但是更好的方法是对每一个
	 操作都设计一个新类型的迭代器，比如select_iterator和where_iterator等，然后用模板来处理不同的迭代器。			
	 */
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
			
			/*只要用select_iterator做参数构造并返回一个新的linq_enumerable对象
			*就可以了。
			*/
			template <typename Function>
			auto select(const Function& _function) const ->linq_enumerable<select_iterator<Iterator,Function>>{
				return linq_enumerable<select_iterator<Iterator,Function>>(
					select_iterator<Iterator,Function>(_begin,_function),
					select_iterator<Iterator,Function>(_end,_function)
				);
			}
	};

	//from函数
	//由于不能直接在c++的数组中添加方法，便需要用数组调用一个函数来返回一个类，最后再想这个类添加LINQ方法。
	//把该函数命名为from，把类命名为linq_enumerable。
	//from函数接受一个表示容器的参数或者表示区间的参数，返回类linq_enumerable的对对象
	template <typename Container>
	auto from(const Container& _container) -> linq_enumerable<decltype(std::begin(_container))>{
		return linq_enumerable<decltype(std::begin(_container))>(std::begin(_container),std::end(_container));
	}

	int main(){
		std::vector<int> v={1,2,3,4,5,6,7,8,9};
		/*for(auto x:from(v)){
			std::cout<<x<<std::endl;
		}
		*/

		
		return 0;
	}
