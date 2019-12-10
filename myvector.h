//
// Created by antony on 12/11/19.
//

#ifndef MEMORY_MANAGER_MYVECTOR_H
#define MEMORY_MANAGER_MYVECTOR_H

namespace my_vector{
    template<class T>
    class vector{
    private:
        size_t m_size;
        size_t m_capacity;
        T* buffer;
    public:
        typedef T* iterator;
        vector();
        vector(unsigned int size);
        vector(unsigned int size, const T& initial);
        vector(const vector<T>& v);
        ~vector();

        size_t capacity() const;
        size_t size() const;
        bool empty() const;
        iterator begin();
        iterator end();
        T& front();
        T& back();
        void push_back(const T& value);
        void erase(size_t position);

    };
}

#endif //MEMORY_MANAGER_MYVECTOR_H
