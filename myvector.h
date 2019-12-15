//
// Created by antony on 12/11/19.
//

#ifndef MEMORY_MANAGER_MYVECTOR_H
#define MEMORY_MANAGER_MYVECTOR_H

#include <iterator>


namespace my_vector{

    template <class T>
    class V_const_iter;

    template <class T>
    class V_iter;

    template<class T>
    class vector{
    private:
        size_t m_size;
        size_t m_capacity;
        T* m_buffer;
    public:
        using const_iterator = V_const_iter<T>;
        using iterator = V_iter<T>;

        vector() : m_size(0),
                   m_capacity(0),
                   m_buffer(nullptr) {}
        explicit vector(size_t t_size);
        vector(size_t t_size, const T& initial);
        vector(T* start, T* finish);
        vector(const vector<T>& v);
        vector(vector<T>&& v) noexcept;
        ~vector();

        size_t capacity() const { return m_capacity; }
        size_t size() const { return m_size; }
        bool empty() const { return !m_size; }

        iterator begin() noexcept { return V_iter<T>(m_buffer); }
        iterator end() noexcept { return V_iter<T>(m_buffer + m_size); }
        const_iterator begin() const noexcept { return V_const_iter<T>(m_buffer); }
        const_iterator end() const noexcept { return V_const_iter<T>(m_buffer + m_size); }
        const_iterator cbegin() const noexcept { return V_const_iter<T>(m_buffer); }
        const_iterator cend() const noexcept { return V_const_iter<T>(m_buffer + m_size); }

        void push_back(const T& value);
        void insert(iterator pos, const T& val);
        void insert(iterator pos, size_t count, const T& val);
        T& at(size_t index);
        void reserve(size_t t_capacity);
        void resize(size_t t_size);
        void erase(iterator which);
        void clear();

        T& operator [](size_t index);
        T operator [](size_t index) const;
        vector<T>& operator =(const vector<T>&);
        vector<T>& operator =(vector<T>&&);
        bool operator !=(const vector<T>&) const;
    };



    template<class T>
    class V_const_iter : public __gnu_cxx::iterator<std::random_access_iterator_tag, T>{
    protected:
        T* p;
    public:
        V_const_iter() : p(nullptr) {}
        explicit V_const_iter(T* ptr) : p(ptr) {}
        V_const_iter(const V_const_iter& vci) : p(vci.p) {}
        V_const_iter(V_const_iter&& vci) noexcept : p(vci.p){ vci.p = nullptr; }

        V_const_iter& operator =(T* ptr) {this->p = ptr; return *this; }
        V_const_iter& operator =(const V_const_iter& vci) { this->p = vci.p; return *this; }
        V_const_iter& operator =( V_const_iter&& vci) { this->p = vci.p; vci.p = nullptr; return *this; }

        bool operator !=(const V_const_iter& vci) const { return p != vci.p; }
        bool operator ==(const V_const_iter& vci) const { return p == vci.p; }
        bool operator > (const V_const_iter& vci) const { return p - vci.p > 0; }
        bool operator < (const V_const_iter& vci) const { return vci > *this; }
        bool operator >=(const V_const_iter& vci) const { return !(*this < vci); }
        bool operator <=(const V_const_iter& vci) const { return !(*this > vci); }

        const T& operator *() const { if(p) return *p; throw std::runtime_error("null pointer"); }
        const T* operator ->() const { return p; }

        V_const_iter& operator +=(long long n) { p += n; return *this; }
        V_const_iter operator +(long long n) { return V_const_iter(p + n); }
        V_const_iter& operator -=(long long n) { p -= n; return *this; }
        V_const_iter operator -(long long n) { return V_const_iter(p - n); }

        V_const_iter& operator ++() { ++p; return *this; }
        V_const_iter& operator --() { --p; return *this; }
        V_const_iter operator ++(int) { const V_const_iter it(p); ++p; return it; }
        V_const_iter operator --(int) { const V_const_iter it(p); --p; return it; }

        const T& operator [](size_t n) const { if(p) return (p + n); throw(std::runtime_error("null pointer indexing")); }
    };



    template<class T>
    class V_iter : public __gnu_cxx::iterator<std::random_access_iterator_tag, T>{
    protected:
        T* p;
    public:
        V_iter() : p(nullptr) {}
        explicit V_iter(T* ptr) : p(ptr) {}
        V_iter(const V_iter& vi) : p(vi.p) {}
        V_iter(V_iter&& vi) noexcept : p(vi.p){ vi.p = nullptr; }

        V_iter& operator =(T* ptr) {this->p = ptr; return *this; }
        V_iter& operator =(const V_iter& vci) { this->p = vci.p; return *this; }
        V_iter& operator =( V_iter&& vci) { this->p = vci.p; vci.p = nullptr; return *this; }

        bool operator !=(const V_iter& vi) const { return p != vi.p; }
        bool operator ==(const V_iter& vi) const { return p == vi.p; }
        bool operator > (const V_iter& vi) const { return p - vi.p > 0; }
        bool operator < (const V_iter& vi) const { return vi > *this; }
        bool operator >=(const V_iter& vi) const { return !(*this < vi); }
        bool operator <=(const V_iter& vi) const { return !(*this > vi); }

        T& operator *() { if(p) return *p; throw std::runtime_error("null pointer dereference"); }
        T* operator ->() { return p; }

        V_iter operator +(long long n) { return V_iter(p + n); }
        V_iter operator -(long long n) { return V_iter(p - n); }
        V_iter& operator +=(long long n) { p += n; return *this; }
        V_iter& operator -=(long long n) { p -= n; return *this; }
        V_iter operator +(const V_iter& vi) { return V_iter(p + vi.p); }
        V_iter operator -(const V_iter& vi) { return V_iter(p - vi.p); }

        V_iter& operator ++() { ++p; return *this; }
        V_iter operator ++(int) { const V_iter it(p); ++p; return it; }
        V_iter& operator --() { --p; return *this; }
        V_iter operator --(int) { const V_iter it(p); --p; return it; }

        T& operator [](size_t n) const { if(p) return (p + n); throw(std::runtime_error("null pointer indexing")); }

    };



    template<class T>
    vector<T>::vector(const vector<T>& v) {
        m_size = v.m_size;
        m_capacity = v.m_capacity;
        m_buffer = new T[m_capacity];
        for(size_t i = 0; i < m_size; ++i){
            m_buffer[i] = v.m_buffer[i];
        }
    }



    template<class T>
    vector<T>::vector(vector<T>&& v) noexcept {
        m_size = v.m_size;
        m_capacity = v.m_capacity;
        m_buffer = v.m_buffer;
        v.m_size = 0;
        v.m_capacity = 0;
        v.m_buffer = nullptr;
    }



    template<class T>
    vector<T>::vector(const size_t t_size) {
        m_size = t_size;
        m_buffer = new T[m_capacity];
        m_capacity = t_size + 10;
    }



    template<class T>
    vector<T>::vector(const size_t t_size, const T& initial) {
        m_size = t_size;
        m_capacity = t_size + 10;
        m_buffer = new T[m_capacity];
        for(size_t i = 0; i < t_size; ++i){
            m_buffer[i] = initial;
        }
    }


    template<class T>
    vector<T>::vector(T* start, T* finish) {
        m_size = 0;
        m_capacity = 0;
        m_buffer = nullptr;
        size_t dist = std::distance(start, finish);
        for(size_t i = 0; i < dist; ++i){
            push_back(*(start + i));
        }
    }



    template<class T>
    vector<T>& vector<T>::operator =(const vector<T>& v) {
        if(this == &v) return *this;

        delete [] m_buffer;
        m_size = v.m_size;
        m_capacity = v.m_capacity;
        m_buffer = new T[m_capacity];
        for(size_t i = 0; i < m_size; ++i){
            m_buffer[i] = v.m_buffer[i];
        }
    }



    template<class T>
    vector<T> &vector<T>::operator =(vector<T> &&v) {
        if(&v == this) return *this;
        delete m_buffer;
        m_buffer = v.m_buffer;
        v.m_buffer = nullptr;
        m_size = v.m_size;
        m_capacity = v.m_capacity;
        v.m_size = 0;
        v.m_capacity = 0;
        return *this;
    }



    template<class T>
    vector<T>::~vector() {
         delete [] m_buffer;
    }



    template<class T>
    void vector<T>::push_back(const T& value) {
        if(m_size >= m_capacity)
            reserve(m_size + 10);
        m_buffer[m_size++] = value;
    }



    template<class T>
    void vector<T>::insert(vector::iterator pos, const T& val) {
        if(m_size + 1 >= m_capacity)
            reserve(m_size + 10);
        size_t index = std::distance(begin(), pos);
        ++m_size;
        for(size_t i = index; i < m_size; ++i){  // move elements behind index 1 pos right
            m_buffer[i + 1] = std::move(m_buffer[i]);
        }
        m_buffer[index] = val;
    }



    template<class T>
    void vector<T>::insert(vector::iterator pos, size_t count, const T &val) {
        if(m_size + count >= m_capacity)
            reserve(m_size + count + 10);
        for(size_t i = 0; i < count; ++i){
            insert(pos, val);
        }
    }



    template<class T>
    T& vector<T>::at(size_t index) {
        if(index >= m_size)
            throw std::out_of_range("vector: at: index out of range");
        return m_buffer[index];
    }



    template<class T>
    void vector<T>::reserve(size_t t_capacity) {
        T* new_buffer = new T[t_capacity];
        for(size_t i = 0; i < m_size; ++i){
            new_buffer[i] = m_buffer[i];
        }
        m_capacity = t_capacity;
        delete [] m_buffer;
        m_buffer = new_buffer;
    }



    template<class T>
    void vector<T>::resize(size_t t_size) {
        reserve(t_size + 10);
        m_size = t_size;
    }



    template<class T>
    void vector<T>::erase(vector::iterator which) {
        size_t dist = 0;
        for(auto it = begin(); it != which; ++it, ++dist){}
        if(dist >= m_size)
            throw std::out_of_range("erase error: argument out of range");
        for(size_t i = dist; i < m_size - 1; ++i){
            m_buffer[i] = std::move(m_buffer[i + 1]);
        }
        --m_size;
    }



    template<class T>
    void vector<T>::clear() {
        m_size = 0;
        m_capacity = 0;
        delete [] m_buffer;
        m_buffer = nullptr;
    }



    template<class T>
    T& vector<T>::operator [](size_t index) {
        if(index >= m_size)
            throw std::out_of_range("vector: operator []: index out of range");
        return m_buffer[index];
    }



    template<class T>
    T vector<T>::operator [](size_t index) const {
        if(index >= m_size)
            throw std::out_of_range("vector: operator []: index out of range");
        return m_buffer[index];
    }



    template<class T>
    bool vector<T>::operator!=(const vector<T>& v) const {
        if(m_size != v.m_size) return false;
        if(m_buffer != v.m_buffer) return false;
        for(size_t i = 0; i < m_size; ++i){
            if(at(i) != v.at(i)) return false;
        }
        return true;
    }

}

#endif //MEMORY_MANAGER_MYVECTOR_H
