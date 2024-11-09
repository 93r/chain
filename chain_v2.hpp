/*
 * Copyright (c) 2024 Mathias Köhler (ma_ko _at_ fastmail.com)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _CHAIN2_HPP
#define _CHAIN2_HPP

namespace mako {
template<typename Container>
class chain {
	public:
	  using Iter = typename Container::iterator;
	  using Range = std::pair< Iter, Iter >;
	  using Ranges = std::vector< Range >;
	  using Ranges_Iter = typename Ranges::iterator;

	  using container_type = Container;
	  using size_type = container_type::size_type;
          using difference_type = typename Iter::difference_type;

	private:
	  struct Ranges_Start { Ranges_Iter value; };
	  struct Ranges_Stop { Ranges_Iter value; };

	public:
	  class iterator {
	    public:
	      using iterator_category = typename Iter::iterator_category;
	      using value_type = typename Iter::value_type;
	      using difference_type = typename Iter::difference_type;
	      using pointer = typename Iter::pointer;
	      using reference = typename Iter::reference;

	    private:
	      Ranges_Iter current_range;
	      Iter current;

	    public:
	      iterator(Ranges_Start range)
	        : current_range {range.value}, current{current_range->first}
	      { }

	      iterator(Ranges_Stop range)
	        : current_range {range.value}, current{current_range->second}
	      { }

	      difference_type operator-(const iterator& other) const
	      {
	        std::cerr << "in op-\n";
	        difference_type n {};
	        for (auto i = other; i != *this; ++n, ++i);
	        return n;
	      }
	      iterator& operator+=(difference_type n)
	      {
	        std::cerr << "operator+=" << n << '\n';
		auto& cr = current_range;
		auto& c = current;
	        //for (decltype(n) i {}; i < n; ++i) ++(*this);
		while (n > difference_type{0}) {
		  if (n < cr->second - c) {
		    c = c + n;
		    break;
		  }
		  else {
		    n = n - (cr->second - c);
		    ++cr;
		    c = cr->first;
		  }
		}
	        return *this;
	      }
	      iterator& operator-=(difference_type n)
	      {
	        std::cerr << "in op-=\n";
		auto& cr = current_range;
		auto& c = current;
	        //for (decltype(n) i {}; i < n; ++i) --(*this);
		while (n > difference_type{0}) {
		  if (!(c - cr->first < n)) {
		    c = c - n;
		    break;
		  }
		  else {
		    n = n - (c - cr->first);
		    --cr;
		    c = cr->second;
		  }
		}
		return *this;
	      }
	      value_type& operator*() const
	      {
	        return *current;
	      }

	      bool operator==(const iterator& other) const
	      { return current == other.current; }
	      bool operator!=(const iterator& other) const
	      {
	        return !(*this == other);
	      }

	      bool operator<(const iterator& other) const
	      {
	        return (current_range < other.current_range
	            ||
	            (
	             !(other.current_range < current_range)
	             &&
	             current < other.current
	            )
	               );
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
	          ++current_range;
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
	        if (current == current_range->first) {
	          --current_range;
	          current = current_range->second;
	        }
	        --current;
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
	  // Postcondition: !m_ranges.empty()
	  {
	    for (auto i : cont_list)
	      if (i != nullptr && !i->empty())
	        m_ranges.push_back({std::begin(*i), std::end(*i)});
	    m_ranges.push_back({ Iter{}, Iter{} });
	  }

	  bool empty()
	  {
	    // remember that it is an invariant that m_ranges.size() != 0
	    // if m_ranges.size() ever equals 0 than you have undefined 
	    // behavior and therefore all bets are off
	    return m_ranges.size() == 1;
	  }

	  size_type size()
	  {
	    auto size = size_type{0};
	    if (m_ranges.size() == size_type{1})
	      return size;
	    auto f = m_ranges.begin();
	    const auto l = m_ranges.end() - 1;
	    while (f != l) {
	      size += f->second - f->first;
	      ++f;
	    }
	    return size;
	  }

	  iterator begin() {
	    return iterator { Ranges_Start{m_ranges.begin()} };
	  }
	  iterator end() {
	    auto ranges = m_ranges.end();
	    --ranges;
	    return iterator { Ranges_Stop{ranges} };
	  }

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

#endif // _CHAIN2_HPP

