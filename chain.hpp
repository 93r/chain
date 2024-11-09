#ifndef _CHAIN_HPP
#define _CHAIN_HPP

namespace mako {
template<typename Container>
class chain {
	public:
		using Iter = typename Container::iterator;
		using Ranges = std::vector< std::pair< Iter, Iter > >;
		using Ranges_Iter = typename Ranges::iterator;

		class iterator {
			public:
				using iterator_category = typename Iter::iterator_category;
				using value_type = typename Iter::value_type;
				using difference_type = typename Iter::difference_type;
				using pointer = typename Iter::pointer;
				using reference = typename Iter::reference;

			private:
				Ranges_Iter first_range;
				Ranges_Iter current_range;
				Ranges_Iter last_range;
				Iter current;

				bool ranges_all_equal() const
				{ return first_range == current_range && current_range == last_range; }
				void cmp_with_current_default_constructed(const char* ctx) const
				{
					Iter current2;
					std::cout
						<< "in context " << ctx << ": "
						<< '|'
						<< (current == current2)
						<< '|'
						<< '\n'; // so default constructed iterators are the same
				}

			public:
				iterator(Ranges_Iter first_range, Ranges_Iter last_range)
					: first_range {first_range}
				, current_range {first_range}
				, last_range {last_range}
				{
					cmp_with_current_default_constructed("chain cons before ranges_all_equal");
					if (!ranges_all_equal()) {
						if (current_range < last_range) {
							current = current_range->first;
						}
						else if (current_range > last_range) {
							std::swap(first_range, last_range);
							current_range = last_range - 1;
							current = current_range->second;
						}
					} else {
							std::cerr << "all equal\n";
							std::cerr << "first_range: " << &*first_range << '\n';
							std::cerr << "current_range: " << &*current_range << '\n';
							std::cerr << "last_range: " << &*last_range << '\n';
							std::cerr << "current_range - 1: " << (&*current_range - 1) << '\n';
							std::cerr << "last_range - 1: " << (&*last_range - 1) << '\n';
							std::cerr << &*((last_range - 1)->second) << '\n';
							current = current_range->first;
							std::cerr << " all " << &*current << '\n';
					}
				}

				difference_type operator-(const iterator& other) const
				{
					difference_type n {};
					for (auto i = other; i != *this; ++n, ++i);
					return n;
				}
				iterator& operator+=(difference_type n)
				{
					std::cerr << "operator+=" << n << '\n';
					for (decltype(n) i {}; i < n; ++i) ++(*this);
					return *this;
				}
				iterator& operator-=(difference_type n)
				{
					for (decltype(n) i {}; i < n; ++i) --(*this);
					return *this;
				}
				value_type& operator*() const
				{ return *current; }
				bool operator!=(const iterator& other) const
				{ return !(*this == other); }
				bool operator==(const iterator& other) const
				{ return current == other.current; }
				bool operator<(const iterator& other) const
				{
					if (current_range < other.last_range) {
						return (current_range < (other.last_range - 1))
							||
							current < other.current;
					}

					return false;
				}
				bool operator>=(const iterator& other) const
				{ return !(*this < other); }
				bool operator>(const iterator& other) const
				{ return other < *this; }
				bool operator<=(const iterator& other) const
				{ return !(other < *this); }
				iterator operator++()
				{
					++current;
					if (current == current_range->second) {
						if (++current_range != last_range) 
							current = current_range->first;
					}

					return *this;
				}
				iterator operator++(int)
				{
					auto tmp = *this;
					++(*this);
					return tmp;
				}
				iterator operator--()
				{
					// special case
					// how should the op behave???
					//if (all_ranges_equal()) // should be undefined behaviour
					if (current != current_range->first) {
						--current;
					}
					else if (current_range != first_range) {
						--current_range;
						if (current_range->first == current_range->second)
							// considering that empty ranges are not allowed then
							// there would be no problem but otherwise... that doesn't work either
							current = current_range->first; // THIS IS WRONG!!!!! // But cannot be done better.
						else
							current = current_range->second - 1; // should be std::prev(current_range->second)
					} // else do nothing, but is that right? Should be considered UB.
					/*
					if (current-- == current_range->first && current_range-- != first_range) {
						current = current_range->second;
						--current;
					}
					*/

					return *this;
				}
				iterator operator--(int)
				{
					auto tmp = *this;
					--(*this);
					return tmp;
				}
				value_type& operator[](difference_type n)
				{
					return *(current + n);
				}
				value_type operator[](difference_type n) const
				{
					return *(current + n);
				}
		};

		chain(std::initializer_list<Container*> cont_list)
			: m_ranges { }
		{
			for (auto i : cont_list)
				m_ranges.push_back({std::begin(*i), std::end(*i)});
		}

		iterator begin() { return iterator { std::begin(m_ranges), std::end(m_ranges) }; }
		iterator end() { return iterator { std::end(m_ranges), std::begin(m_ranges) }; }
		iterator end2() { return iterator { std::end(m_ranges), std::end(m_ranges) }; } // might not be needed anymore

	private:
		Ranges m_ranges;
};

template<typename It>
It operator-(It it, typename It::difference_type n)
{ return it -= n; }

template<typename It>
It operator+(It it, typename It::difference_type n)
{ return it += n; }

} // namespace mako

#endif // _CHAIN_HPP

