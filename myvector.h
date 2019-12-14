//
// Created by antony on 12/11/19.
//

#ifndef MEMORY_MANAGER_MYVECTOR_H
#define MEMORY_MANAGER_MYVECTOR_H

#include <cmath>
#include <stdexcept>

namespace my_vector{ // todo set classes of iterators

    template <class T>
    class V_const_iter;

    template <class T>
    class V_iter;

    template<class T>
    class vector{
    private:
        size_t m_size;
        size_t m_capacity;
        T* buffer;
    public:
        using const_iterator = V_const_iter<T>;
        using iterator = V_iter<T>;

        vector();
        explicit vector(size_t t_size);
        vector(size_t t_size, const T& initial);
        vector(const vector<T>& v);
        ~vector();

        size_t capacity() const;
        size_t size() const;
        bool empty() const;
        iterator begin() noexcept { return V_iter<T>(buffer); }
        iterator end() noexcept { return V_iter<T>(buffer + m_size); }
        const_iterator cbegin() const noexcept { return V_const_iter<T>(buffer); }
        const_iterator cend() const noexcept { return V_const_iter<T>(buffer + m_size); }
        void push_back(const T& value);
        T pop_back();
        void reserve(size_t t_capacity);
        void resize(size_t t_size);
        void erase(iterator which);
        void clear();

        T& operator [](size_t index);
        const T& operator [](size_t index) const;
        vector<T>& operator =(const vector<T>&);


    };



    template<class T>
    class V_const_iter{
    protected:
        T* p;
    public:
        V_const_iter() : p(nullptr) {}
        explicit V_const_iter(T* ptr) : p(ptr) {}
        V_const_iter(const V_const_iter& vci) : p(vci.p) {}
        V_const_iter(V_const_iter&& vci) noexcept : p(vci.p){ vci.p = nullptr; }

        V_const_iter& operator=(T* ptr) {this->p = ptr; return *this; }
        V_const_iter& operator=(const V_const_iter& vci) { this->p = vci.p; return *this; }
        V_const_iter& operator=( V_const_iter&& vci) { this->p = vci.p; vci.p = nullptr; return *this; }

        bool operator!=(const V_const_iter& vci) const { return p != vci.p; }
        bool operator==(const V_const_iter& vci) const { return p == vci.p; }

        const T& operator*() const { if(p) return *p; throw std::runtime_error("null pointer"); }
        const T* operator->() const { return p; }

        V_const_iter& operator+=(long long n) { p += n; return *this; }
        V_const_iter operator+(long long n) { return V_const_iter(p + n); }
        V_const_iter& operator-=(long long n) { p -= n; return *this; }
        V_const_iter operator-(long long n) { return V_const_iter(p - n); }

        V_const_iter& operator++() { ++p; return *this; }
        const V_const_iter operator++(int) { const V_const_iter it(p); ++p; return it; }
        V_const_iter& operator--() { --p; return *this; }
        const V_const_iter operator--(int) { const V_const_iter it(p); --p; return it; }

    };



    template<class T>
    class V_iter{
    protected:
        T* p;
    public:
        V_iter() : p(nullptr) {}
        explicit V_iter(T* ptr) : p(ptr) {}
        V_iter(const V_iter& vci) : p(vci.p) {}
        V_iter(V_iter&& vci) noexcept : p(vci.p){ vci.p = nullptr; }

        V_iter& operator=(T* ptr) {this->p = ptr; return *this; }
        V_iter& operator=(const V_iter& vci) { this->p = vci.p; return *this; }
        V_iter& operator=( V_iter&& vci) { this->p = vci.p; vci.p = nullptr; return *this; }

        bool operator!=(const V_iter& vci) const { return p != vci.p; }
        bool operator==(const V_iter& vci) const { return p == vci.p; }

        T& operator*() { if(p) return *p; throw std::runtime_error("null pointer"); }
        T* operator->() { return p; }

        V_iter& operator+=(long long n) { p += n; return *this; }
        V_iter operator+(long long n) { return V_iter(p + n); }
        V_iter& operator-=(long long n) { p -= n; return *this; }
        V_iter operator-(long long n) { return V_iter(p - n); }

        V_iter& operator++() { ++p; return *this; }
        const V_iter operator++(int) { const V_iter it(p); ++p; return it; }
        V_iter& operator--() { --p; return *this; }
        const V_iter operator--(int) { const V_iter it(p); --p; return it; }

    };



    template<class T>
    vector<T>::vector() : m_size(0),
                          m_capacity(0),
                          buffer(nullptr) {}



    template<class T>
    vector<T>::vector(const vector<T>& v) {
        m_size = v.m_size;
        m_capacity = v.m_capacity;
        buffer = new T[m_capacity];
        for(size_t i = 0; i < m_size; ++i){
            buffer[i] = v.buffer[i];
        }
    }



    template<class T>
    vector<T>::vector(const size_t t_size) {
        m_size = t_size;
        buffer = new T[m_capacity];
        m_capacity = t_size + 10;
    }



    template<class T>
    vector<T>::vector(const size_t t_size, const T& initial) {
        m_size = t_size;
        m_capacity = t_size + 10;
        buffer = new T[m_capacity];
        for(size_t i = 0; i < t_size; ++i){
            buffer[i] = initial;
        }
    }



    template<class T>
    vector<T>& vector<T>::operator=(const vector<T>& v) {
        if(this == &v) return *this;

        delete [] buffer;
        m_size = v.m_size;
        m_capacity = v.m_capacity;
        buffer = new T[m_capacity];
        for(size_t i = 0; i < m_size; ++i){
            buffer[i] = v.buffer[i];
        }
    }



    template<class T>
    vector<T>::~vector() {
         delete [] buffer;
    }



    template<class T>
    size_t vector<T>::capacity() const {
        return m_capacity;
    }



    template<class T>
    size_t vector<T>::size() const {
        return m_size;
    }



    template<class T>
    bool vector<T>::empty() const {
        return m_size == 0;
    }



    template<class T>
    void vector<T>::push_back(const T& value) {
        if(m_size >= m_capacity){
            reserve(m_size + 10);
        }
        buffer[m_size++] = value;
    }



    template<class T>
    T vector<T>::pop_back() {
        return buffer[m_size--];
    }



    template<class T>
    void vector<T>::reserve(size_t t_capacity) {
        T* new_buffer = new T[t_capacity];
        for(size_t i = 0; i < m_size; ++i){
            new_buffer[i] = buffer[i];
        }
        m_capacity = t_capacity;
        delete [] buffer;
        buffer = new_buffer;
    }



    template<class T>
    void vector<T>::resize(size_t t_size) {
        reserve(t_size + 10);
        m_size = t_size;
    }



    template<class T>
    void vector<T>::erase(vector::iterator which) {

    }


}

#endif //MEMORY_MANAGER_MYVECTOR_H
